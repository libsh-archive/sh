// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "ShParser.hpp"
#include <string>
#include "ShToken.hpp"
#include "ShBasicBlock.hpp"
#include "ShTokenizer.hpp"
#include "ShException.hpp"
#include "ShError.hpp"

namespace SH {

void checkCond(bool cond, const std::string& message = "Internal Error")
{
  if (!cond) shError( ShParseException(message) );
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
  ShTokenPtr token = shref_dynamic_cast<ShToken>(next);

  if (basic) {
    // This is a basic block. Just make a node for it.
    head = tail = new ShCtrlGraphNode();
    tail->block = basic;
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
    case SH_TOKEN_CONTINUE:
      return; // TODO: break, continue semantics
    case SH_TOKEN_ELSE:
    case SH_TOKEN_ENDIF:
    case SH_TOKEN_ENDWHILE:
    case SH_TOKEN_UNTIL:
    case SH_TOKEN_ENDFOR:
      return;
    }
  } else { // not a basic block, not a token
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

void ShParser::parseIf(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr ifToken = shref_dynamic_cast<ShToken>(blocks->getFront());
  blocks->removeFront();
  
  checkCond(ifToken);
  checkCond(ifToken->type() == SH_TOKEN_IF);
  checkCond(ifToken->arguments.size() == 1);

  ShTokenArgument condArg = ifToken->arguments.front();
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr headIf = 0, tailIf = 0, headElse = 0, tailElse = 0;
  parseStmts(headIf, tailIf, blocks);
  
  ShTokenPtr nt = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(nt);

  if (nt->type() == SH_TOKEN_ELSE) {
    blocks->removeFront();
    parseStmts(headElse, tailElse, blocks);
    
    nt = shref_dynamic_cast<ShToken>(blocks->getFront());
    checkCond(nt);
  }
  checkCond(nt->type() == SH_TOKEN_ENDIF);
  blocks->removeFront();

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
  
  ShTokenPtr forToken = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(forToken);
  checkCond(forToken->type() == SH_TOKEN_FOR);
  checkCond(forToken->arguments.size() == 3);
  blocks->removeFront();
  
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
  
  ShCtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new ShCtrlGraphNode();
  }
  
  ShTokenPtr nt = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(nt);
  checkCond(nt->type() == SH_TOKEN_ENDFOR);
  blocks->removeFront();

  tail->append(headCond);
  tailCond->append(headBody, condArg.result);
  tailBody->append(headUpdate);
  tailUpdate->append(headCond);
  tail = tailCond;
}

void ShParser::parseWhile(ShCtrlGraphNodePtr& head,
                          ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr whileToken = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(whileToken);
  checkCond(whileToken->type() == SH_TOKEN_WHILE);
  checkCond(whileToken->arguments.size() == 1);
  blocks->removeFront();
  
  ShTokenArgument condArg = whileToken->arguments[0];
  
  parseStmts(head, tail, condArg.blockList);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShCtrlGraphNodePtr headBody = 0, tailBody = 0;
  parseStmts(headBody, tailBody, blocks);
  if (!tailBody) {
    headBody = tailBody = new ShCtrlGraphNode();
  }
  
  ShTokenPtr nt = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(nt);
  checkCond(nt->type() == SH_TOKEN_ENDWHILE);
  blocks->removeFront();

  tail->append(headBody, condArg.result);
  tailBody->append(head);
}

void ShParser::parseDo(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail, ShBlockListPtr blocks)
{
  head = tail = 0;
  
  ShTokenPtr doToken = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(doToken);
  checkCond(doToken->type() == SH_TOKEN_DO);
  blocks->removeFront();

  parseStmts(head, tail, blocks);
  if (!tail) {
    head = tail = new ShCtrlGraphNode();
  }

  ShTokenPtr nt = shref_dynamic_cast<ShToken>(blocks->getFront());
  checkCond(nt);
  checkCond(nt->type() == SH_TOKEN_UNTIL);
  checkCond(nt->arguments.size() == 1);
  blocks->removeFront();

  ShTokenArgument condArg = nt->arguments[0];

  ShCtrlGraphNodePtr headCond = 0, tailCond = 0;
  parseStmts(headCond, tailCond, condArg.blockList);
  if (!tailCond) {
    headCond = tailCond = new ShCtrlGraphNode();
  }

  tail->append(headCond);

  // Add empty fallthrough node.
  ShCtrlGraphNodePtr fallthrough = new ShCtrlGraphNode();

  // If the result of the UNTIL test is true, break out of the loop
  tailCond->append(fallthrough, condArg.result);

  // Otherwise, continue looping.
  tailCond->append(head);
  tail = fallthrough;
}

}
