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
#include <iostream>
#include <fstream>
#include <cassert>
#include "ShSyntax.hpp"
#include "ShEnvironment.hpp"
#include "ShContext.hpp"
#include "ShTokenizer.hpp"
#include "ShToken.hpp"
#include "ShProgram.hpp"
#include "ShBackend.hpp"
#include "ShOptimizations.hpp"

namespace SH {

ShProgram shBeginShader(const std::string& target)
{
  ShProgram prg(target);
  ShContext::current()->enter(prg.node());
  return prg;
}

void shEndShader()
{
  ShContext* context = ShContext::current();

  ShProgramNodePtr parsing = context->parsing();
  assert(parsing);
  
  parsing->ctrlGraph = new ShCtrlGraph(parsing->tokenizer.blockList());

  optimize(parsing);
  
  parsing->collectVariables();
  
  context->exit();

  // TODO. See issue129.
//   if (!ShEnvironment::shader->target().empty()) {
//     shCompile(ShEnvironment::shader);
//   }
}

void shCompile(ShProgram& prg)
{
  if (!ShEnvironment::backend) return;
  prg.compile(ShEnvironment::backend);
}

void shCompile(ShProgram& prg, const std::string& target)
{
  if (!ShEnvironment::backend) return;
  prg.compile(target, ShEnvironment::backend);
}

void shCompileShader(ShProgram& prg)
{
  shCompile(prg);
}

void shCompileShader(const std::string& target, ShProgram& prg)
{
  shCompile(prg, target);
}

void shBind(ShProgram& prg)
{
  if (!ShEnvironment::backend) return;
  prg.code(ShEnvironment::backend)->bind();
}

void shBind(ShProgram& prg, const std::string& target)
{
  if (!ShEnvironment::backend) return;
  prg.code(target, ShEnvironment::backend)->bind();
}

void shBindShader(ShProgram& shader)
{
  shBind(shader);
}

void shBindShader(const std::string& target, ShProgram& shader)
{
  shBind(shader, target);
}

bool shSetBackend(const std::string& name)
{
  ShBackendPtr backend = SH::ShBackend::lookup(name);
  if (!backend) return false;
  SH::ShEnvironment::backend = backend;
  return true;
}

void shInit()
{
  // TODO: Initialize backends
}

void shIf(bool)
{
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_IF);

  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndIf()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDIF));
}

void shElse()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ELSE));
}

void shWhile(bool)
{
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_WHILE);

  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndWhile()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDWHILE));
}

void shDo()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_DO));
}

void shUntil(bool)
{
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_UNTIL);

  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shFor(bool)
{
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_FOR);

  for (int i = 0; i < 3; i++) {
    token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  }
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndFor()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDFOR));
}

void ShBreak()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_BREAK));
}

void ShContinue()
{
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_CONTINUE));
}

}
