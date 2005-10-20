// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShParser.hpp"
#include "ShBasicBlock.hpp"
#include "ShCfgBlock.hpp"
#include "ShTokenizer.hpp"
#include "ShException.hpp"
#include "ShVariable.hpp"
#include "ShError.hpp"

namespace SH {

void checkCond(bool cond, const std::string& message = "Internal Error")
{
  if (!cond) shError( ShParseException(message) );
}


ShParser* ShParser::instance()
{
  static ShParser* instance = new ShParser();
  return instance;
}

void ShParser::parse(ShCtrlGraphNodePtr& head,
                     ShCtrlGraphNodePtr& tail,
                     ShBlockListPtr blocks)
{
  parseStmts(head, tail, blocks);
  checkCond(blocks->empty(), "Unexpected token"); // TODO: improve
}


void ShParser::parseStmts(ShCtrlGraphNodePtr& head,
                          ShCtrlGraphNodePtr& tail,
                          ShBlockListPtr blocks)
{
  ShBlockPtr next = blocks->getFront();
  head = tail = 0;
  
  if (!next) {
    return;
  }
  
  ShBasicBlockPtr basic = shref_dynamic_cast<ShBasicBlock>(next);
  ShCfgBlockPtr cfg = shref_dynamic_cast<ShCfgBlock>(next);
  ShTokenPtr token = shref_dynamic_cast<ShToken>(next);

  if (basic) {
    parseBlock(head, tail, basic);
    blocks->removeFront();
  } else if (token) {
    // This is a token. Check what type it has.
    switch(token->type()) {
    case SH_TOKEN_IF:
      parseIf(head, tail, blocks);
      break;
    case SH_TOKEN_FOR:
      parseFor(head, tail, blocks);
      break;
    case SH_TOKEN_WHILE:
      parseWhile(head, tail, blocks);
      break;
    case SH_TOKEN_DO:
      parseDo(head, tail, blocks);
      break;
    case SH_TOKEN_BREAK:
      parse_break(head, tail, blocks);
      break;
    case SH_TOKEN_CONTINUE:
      parse_continue(head, tail, blocks);
      break;
    case SH_TOKEN_STARTSWITCH:
    case SH_TOKEN_ENDSWITCH:
    case SH_TOKEN_CASE:
      return; // TODO: switch semantics
    case SH_TOKEN_STARTSEC:
      parseSection(head, tail, blocks);
      break;
    case SH_TOKEN_ELSE:
    case SH_TOKEN_ENDIF:
    case SH_TOKEN_ENDWHILE:
    case SH_TOKEN_UNTIL:
    case SH_TOKEN_ENDFOR:
    case SH_TOKEN_ENDSEC:
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
    ShCtrlGraphNodePtr nh, nt;
    parseStmts(nh, nt, blocks);
    if (nh) {
      tail->append(nh);
      tail = nt;
    }
  }
}

void ShParser::parseBlock(ShCtrlGraphNodePtr& head,
                          ShCtrlGraphNodePtr& tail, ShBasicBlockPtr block)
{
  head = tail = new ShCtrlGraphNode();
  tail->block = block;
  
  /* Move SH_OP_DECL statement sinto the node's declaration set */
  for(ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
    if(I->op == SH_OP_DECL) {
      head->addDecl(I->dest.node());
      I = block->erase(I);
    } else ++I;
  }
}

void ShParser::parseIf(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr ifToken = popToken(blocks, SH_TOKEN_IF, 1); 
  
  ShTokenArgument condArg = ifToken->arguments.front();
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr headIf = 0, tailIf = 0, headElse = 0, tailElse = 0;
  parseStmts(headIf, tailIf, blocks);
  
  ShTokenPtr nt = popToken(blocks); 

  if (nt->type() == SH_TOKEN_ELSE) {
    parseStmts(headElse, tailElse, blocks);
    nt = popToken(blocks);
  }
  checkCond(nt->type() == SH_TOKEN_ENDIF);

  if (!headIf) headIf = tailIf = new ShCtrlGraphNode();
  if (!headElse) headElse = tailElse = new ShCtrlGraphNode();
  
  tail->append(headIf, condArg.result);
  tail->append(headElse);
  
  ShCtrlGraphNodePtr merge = new ShCtrlGraphNode();
  tailIf->append(merge);
  tailElse->append(merge);
  
  tail = merge;
}

void ShParser::parseFor(ShCtrlGraphNodePtr& head,
                        ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr forToken = popToken(blocks, SH_TOKEN_FOR, 3); 
  
  ShTokenArgument initArg = forToken->arguments[0];
  ShTokenArgument condArg = forToken->arguments[1];
  ShTokenArgument updateArg = forToken->arguments[2];
  
  parseStmts(head, tail, initArg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr headCond = 0, tailCond = 0;
  parseStmts(headCond, tailCond, condArg.blockList);
  if (!tailCond) {
    headCond = tailCond = new ShCtrlGraphNode();
  }
  
  ShCtrlGraphNodePtr headUpdate = 0, tailUpdate = 0;
  parseStmts(headUpdate, tailUpdate, updateArg.blockList);
  if (!tailUpdate) {
    headUpdate = tailUpdate = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr exit = new ShCtrlGraphNode();
  push_scope(exit, headUpdate);

  ShCtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new ShCtrlGraphNode();
  }
  
  ShTokenPtr nt = popToken(blocks, SH_TOKEN_ENDFOR);

  tail->append(headCond);
  tailCond->append(headBody, condArg.result);
  tailCond->append(exit);
  tailBody->append(headUpdate);
  tailUpdate->append(headCond);
  tail = exit;

  pop_scope();
}

void ShParser::parseWhile(ShCtrlGraphNodePtr& head,
                          ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr whileToken = popToken(blocks, SH_TOKEN_WHILE, 1);
  
  ShTokenArgument condArg = whileToken->arguments[0];
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr exit = new ShCtrlGraphNode();
  push_scope(exit, head);

  ShCtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new ShCtrlGraphNode();
  }
  
  ShTokenPtr nt = popToken(blocks, SH_TOKEN_ENDWHILE);

  tail->append(headBody, condArg.result);
  tail->append(exit); // after the loop has exited
  tailBody->append(head);
  tail = exit;

  pop_scope();
}

void ShParser::parseDo(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = new ShCtrlGraphNode();

  ShTokenPtr doToken = popToken(blocks, SH_TOKEN_DO);

  ShCtrlGraphNodePtr exit = new ShCtrlGraphNode();
  push_scope(exit, head);

  ShCtrlGraphNodePtr head_body = 0, tail_body = 0;
  parseStmts(head_body, tail_body, blocks);
  if (!tail_body) {
    head_body = tail_body = new ShCtrlGraphNode();
  }

  ShTokenPtr nt = popToken(blocks, SH_TOKEN_UNTIL, 1);

  ShTokenArgument condArg = nt->arguments[0];

  ShCtrlGraphNodePtr headCond = 0, tailCond = 0;
  parseStmts(headCond, tailCond, condArg.blockList);
  if (!tailCond) {
    headCond = tailCond = new ShCtrlGraphNode();
  }

  tail->append(head_body);
  tail_body->append(headCond);

  // If the result of the UNTIL test is true, break out of the loop
  tailCond->append(exit, condArg.result);

  // Otherwise, continue looping.
  tailCond->append(head_body);
  tail = exit;

  pop_scope();
}

void ShParser::parseSection(ShCtrlGraphNodePtr& head,
                           ShCtrlGraphNodePtr& tail,
                           ShBlockListPtr blocks)
{
  ShTokenPtr startToken = popToken(blocks, SH_TOKEN_STARTSEC);

  // @todo range this is a really stupid way of grabbing the name...
  ShBasicBlockPtr basic = shref_dynamic_cast<ShBasicBlock>(blocks->getFront());
  SH_DEBUG_ASSERT(basic && !basic->empty() && basic->begin()->op == SH_OP_COMMENT);
  std::string name = basic->begin()->get_info<ShInfoComment>()->comment;
  basic->erase(basic->begin());

  ShCtrlGraphNodePtr secHead, secTail;
  parseStmts(secHead, secTail, blocks);
  if(!secTail) {
    secHead = secTail = new ShCtrlGraphNode();
  }

  ShTokenPtr nt = popToken(blocks, SH_TOKEN_ENDSEC);

  head = new ShCtrlGraphNode();
  head->block = new ShBasicBlock();

  ShStatement startStmt(SH_OP_STARTSEC);
  startStmt.add_info(new ShInfoComment(name));
  head->block->addStatement(startStmt);

  tail = new ShCtrlGraphNode();
  tail->block = new ShBasicBlock();
  //tail->block->addStatement(ShStatement(foo, SH_OP_ENDSEC, foo));
  ShStatement endStmt(SH_OP_ENDSEC);
  endStmt.add_info(new ShInfoComment(name));
  tail->block->addStatement(endStmt);

  head->append(secHead);
  secTail->append(tail);
}

void ShParser::parse_break(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, 
                           ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr break_token = popToken(blocks, SH_TOKEN_BREAK, 1); 
  
  ShTokenArgument cond_arg = break_token->arguments.front();
  
  parseStmts(head, tail, cond_arg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  tail->append(m_break_node.top(), cond_arg.result);
}

void ShParser::parse_continue(ShCtrlGraphNodePtr& head, ShCtrlGraphNodePtr& tail, 
                              ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr continue_token = popToken(blocks, SH_TOKEN_CONTINUE, 1); 
  
  ShTokenArgument cond_arg = continue_token->arguments.front();
  
  parseStmts(head, tail, cond_arg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  tail->append(m_continue_node.top(), cond_arg.result);
}

ShTokenPtr ShParser::popToken(ShBlockListPtr blocks, ShTokenType expectedType, unsigned int expectedArgs)
{
  ShTokenPtr result = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(result);
  checkCond(result->type() == expectedType);
  checkCond(result->arguments.size() == expectedArgs);
  blocks->removeFront();
  return result;
}

ShTokenPtr ShParser::popToken(ShBlockListPtr blocks)
{
  ShTokenPtr result = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(result);
  blocks->removeFront();
  return result;
}

void ShParser::push_scope(ShCtrlGraphNodePtr& break_node, 
                          ShCtrlGraphNodePtr& continue_node)
{
  m_break_node.push(break_node);
  m_continue_node.push(continue_node);
}

void ShParser::pop_scope()
{
  m_break_node.pop();
  m_continue_node.pop();
}

}
