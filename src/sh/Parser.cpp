// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include <string>
#include <sstream>
#include "Parser.hpp"
#include "BasicBlock.hpp"
#include "CfgBlock.hpp"
#include "Tokenizer.hpp"
#include "Exception.hpp"
#include "Variable.hpp"
#include "Error.hpp"

namespace SH {

void checkCond(bool cond, const std::string& message = "Internal Error")
{
  if (!cond) error( ParseException(message) );
}


Parser* Parser::instance()
{
  static Parser* instance = new Parser();
  return instance;
}

void Parser::parse(CtrlGraphNodePtr& head,
                     CtrlGraphNodePtr& tail,
                     const BlockListPtr& blocks)
{
  parseStmts(head, tail, blocks);
  checkCond(blocks->empty(), "Unexpected token"); // TODO: improve
}


void Parser::parseStmts(CtrlGraphNodePtr& head,
                          CtrlGraphNodePtr& tail,
                          const BlockListPtr& blocks)
{
  BlockPtr next = blocks->getFront();
  head = tail = 0;
  
  if (!next) {
    return;
  }
  
  BasicBlockPtr basic = shref_dynamic_cast<BasicBlock>(next);
  CfgBlockPtr cfg = shref_dynamic_cast<CfgBlock>(next);
  TokenPtr token = shref_dynamic_cast<Token>(next);

  if (basic) {
    parseBlock(head, tail, basic);
    blocks->removeFront();
  } else if (token) {
    // This is a token. Check what type it has.
    switch(token->type()) {
    case TOKEN_IF:
      parseIf(head, tail, blocks);
      break;
    case TOKEN_FOR:
      parseFor(head, tail, blocks);
      break;
    case TOKEN_WHILE:
      parseWhile(head, tail, blocks);
      break;
    case TOKEN_DO:
      parseDo(head, tail, blocks);
      break;
    case TOKEN_BREAK:
      parse_break(head, tail, blocks);
      break;
    case TOKEN_CONTINUE:
      parse_continue(head, tail, blocks);
      break;
    case TOKEN_STARTSWITCH:
    case TOKEN_ENDSWITCH:
    case TOKEN_CASE:
      return; // TODO: switch semantics
    case TOKEN_STARTSEC:
      parseSection(head, tail, blocks);
      break;
    case TOKEN_ELSE:
    case TOKEN_ENDIF:
    case TOKEN_ENDWHILE:
    case TOKEN_UNTIL:
    case TOKEN_ENDFOR:
    case TOKEN_ENDSEC:
      return;
    }
  } else if (cfg) {
    head = cfg->entry();
    tail = cfg->exit();
    blocks->removeFront();
  } else { // not a basic/cfg block, not a token
    return;
  }
  if (tail) {
    CtrlGraphNodePtr nh, nt;
    parseStmts(nh, nt, blocks);
    if (nh) {
      tail->append(nh);
      tail = nt;
    }
  }
}

void Parser::parseBlock(CtrlGraphNodePtr& head,
                          CtrlGraphNodePtr& tail, const BasicBlockPtr& block)
{
  head = tail = new CtrlGraphNode();
  tail->block = block;
  
  /* Move OP_DECL statement sinto the node's declaration set */
  for(BasicBlock::StmtList::iterator I = block->begin(); I != block->end();) {
    if(I->op == OP_DECL) {
      head->addDecl(I->dest.node());
      I = block->erase(I);
    } else ++I;
  }
}

void Parser::parseIf(CtrlGraphNodePtr& head,
                       CtrlGraphNodePtr& tail, const BlockListPtr& blocks)
{
  head = tail = 0;
  
  TokenPtr ifToken = popToken(blocks, TOKEN_IF, 1); 
  
  TokenArgument condArg = ifToken->arguments.front();
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new CtrlGraphNode();
  }

  CtrlGraphNodePtr headIf = 0, tailIf = 0, headElse = 0, tailElse = 0;
  parseStmts(headIf, tailIf, blocks);
  
  TokenPtr nt = popToken(blocks); 

  if (nt->type() == TOKEN_ELSE) {
    parseStmts(headElse, tailElse, blocks);
    nt = popToken(blocks);
  }
  checkCond(nt->type() == TOKEN_ENDIF);

  if (!headIf) headIf = tailIf = new CtrlGraphNode();
  if (!headElse) headElse = tailElse = new CtrlGraphNode();
  
  tail->append(headIf, condArg.result);
  tail->append(headElse);
  
  CtrlGraphNodePtr merge = new CtrlGraphNode();
  tailIf->append(merge);
  tailElse->append(merge);
  
  tail = merge;
}

void Parser::parseFor(CtrlGraphNodePtr& head,
                        CtrlGraphNodePtr& tail, const BlockListPtr& blocks)
{
  head = tail = 0;
  
  TokenPtr forToken = popToken(blocks, TOKEN_FOR, 3); 
  
  TokenArgument initArg = forToken->arguments[0];
  TokenArgument condArg = forToken->arguments[1];
  TokenArgument updateArg = forToken->arguments[2];
  
  parseStmts(head, tail, initArg.blockList);
  if (!tail) {
    head = tail = new CtrlGraphNode();
  }

  CtrlGraphNodePtr headCond = 0, tailCond = 0;
  parseStmts(headCond, tailCond, condArg.blockList);
  if (!tailCond) {
    headCond = tailCond = new CtrlGraphNode();
  }
  
  CtrlGraphNodePtr headUpdate = 0, tailUpdate = 0;
  parseStmts(headUpdate, tailUpdate, updateArg.blockList);
  if (!tailUpdate) {
    headUpdate = tailUpdate = new CtrlGraphNode();
  }

  CtrlGraphNodePtr exit = new CtrlGraphNode();
  push_scope(exit, headUpdate);

  CtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new CtrlGraphNode();
  }
  
  TokenPtr nt = popToken(blocks, TOKEN_ENDFOR);

  tail->append(headCond);
  tailCond->append(headBody, condArg.result);
  tailCond->append(exit);
  tailBody->append(headUpdate);
  tailUpdate->append(headCond);
  tail = exit;

  pop_scope();
}

void Parser::parseWhile(CtrlGraphNodePtr& head,
                          CtrlGraphNodePtr& tail, const BlockListPtr& blocks)
{
  head = tail = 0;
  
  TokenPtr whileToken = popToken(blocks, TOKEN_WHILE, 1);
  
  TokenArgument condArg = whileToken->arguments[0];
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new CtrlGraphNode();
  }

  CtrlGraphNodePtr exit = new CtrlGraphNode();
  push_scope(exit, head);

  CtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new CtrlGraphNode();
  }
  
  TokenPtr nt = popToken(blocks, TOKEN_ENDWHILE);

  tail->append(headBody, condArg.result);
  tail->append(exit); // after the loop has exited
  tailBody->append(head);
  tail = exit;

  pop_scope();
}

void Parser::parseDo(CtrlGraphNodePtr& head,
                       CtrlGraphNodePtr& tail, const BlockListPtr& blocks)
{
  head = tail = new CtrlGraphNode();

  TokenPtr doToken = popToken(blocks, TOKEN_DO);

  CtrlGraphNodePtr exit = new CtrlGraphNode();
  CtrlGraphNodePtr loop_end = new CtrlGraphNode();
  push_scope(exit, loop_end);

  CtrlGraphNodePtr head_body = 0, tail_body = 0;
  parseStmts(head_body, tail_body, blocks);
  if (!tail_body) {
    head_body = tail_body = new CtrlGraphNode();
  }

  TokenPtr nt = popToken(blocks, TOKEN_UNTIL, 1);

  TokenArgument condArg = nt->arguments[0];

  CtrlGraphNodePtr headCond = 0, tailCond = 0;
  parseStmts(headCond, tailCond, condArg.blockList);
  if (!tailCond) {
    headCond = tailCond = new CtrlGraphNode();
  }

  tail->append(head_body);
  tail_body->append(loop_end);
  loop_end->append(headCond);

  // If the result of the UNTIL test is true, break out of the loop
  tailCond->append(exit, condArg.result);

  // Otherwise, continue looping.
  tailCond->append(head_body);
  tail = exit;

  pop_scope();
}

void Parser::parseSection(CtrlGraphNodePtr& head,
                           CtrlGraphNodePtr& tail,
                           const BlockListPtr& blocks)
{
  TokenPtr startToken = popToken(blocks, TOKEN_STARTSEC);

  // @todo range this is a really stupid way of grabbing the name...
  BasicBlockPtr basic = shref_dynamic_cast<BasicBlock>(blocks->getFront());
  SH_DEBUG_ASSERT(basic && !basic->empty() && basic->begin()->op == OP_COMMENT);
  std::string name = basic->begin()->get_info<InfoComment>()->comment;
  basic->erase(basic->begin());

  CtrlGraphNodePtr secHead, secTail;
  parseStmts(secHead, secTail, blocks);
  if(!secTail) {
    secHead = secTail = new CtrlGraphNode();
  }

  TokenPtr nt = popToken(blocks, TOKEN_ENDSEC);

  head = new CtrlGraphNode();
  head->block = new BasicBlock();

  Statement startStmt(OP_STARTSEC);
  startStmt.add_info(new InfoComment(name));
  head->block->addStatement(startStmt);

  tail = new CtrlGraphNode();
  tail->block = new BasicBlock();
  //tail->block->addStatement(Statement(foo, OP_ENDSEC, foo));
  Statement endStmt(OP_ENDSEC);
  endStmt.add_info(new InfoComment(name));
  tail->block->addStatement(endStmt);

  head->append(secHead);
  secTail->append(tail);
}

void Parser::parse_break(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, 
                           const BlockListPtr& blocks)
{
  head = tail = 0;
  
  TokenPtr break_token = popToken(blocks, TOKEN_BREAK, 1); 
  
  TokenArgument cond_arg = break_token->arguments.front();
  
  parseStmts(head, tail, cond_arg.blockList);
  if (!tail) {
    head = tail = new CtrlGraphNode();
  }

  tail->append(m_break_node.top(), cond_arg.result);
}

void Parser::parse_continue(CtrlGraphNodePtr& head, CtrlGraphNodePtr& tail, 
                              const BlockListPtr& blocks)
{
  head = tail = 0;
  
  TokenPtr continue_token = popToken(blocks, TOKEN_CONTINUE, 1); 
  
  TokenArgument cond_arg = continue_token->arguments.front();
  
  parseStmts(head, tail, cond_arg.blockList);
  if (!tail) {
    head = tail = new CtrlGraphNode();
  }

  tail->append(m_continue_node.top(), cond_arg.result);
}

TokenPtr Parser::popToken(const BlockListPtr& blocks, TokenType expectedType, unsigned int expectedArgs)
{
  TokenPtr result = shref_dynamic_cast<Token>(blocks->getFront());
  checkCond(result);
  checkCond(result->type() == expectedType);
  checkCond(result->arguments.size() == expectedArgs);
  blocks->removeFront();
  return result;
}

TokenPtr Parser::popToken(const BlockListPtr& blocks)
{
  TokenPtr result = shref_dynamic_cast<Token>(blocks->getFront());
  checkCond(result);
  blocks->removeFront();
  return result;
}

void Parser::push_scope(CtrlGraphNodePtr& break_node, 
                          CtrlGraphNodePtr& continue_node)
{
  m_break_node.push(break_node);
  m_continue_node.push(continue_node);
}

void Parser::pop_scope()
{
  m_break_node.pop();
  m_continue_node.pop();
}

}
