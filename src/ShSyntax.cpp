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
#include "ShTokenizer.hpp"
#include "ShToken.hpp"
#include "ShProgram.hpp"
#include "ShBackend.hpp"
#include "ShOptimizer.hpp"

namespace SH {

ShProgram shBeginShader(const std::string& target)
{
  assert(!ShEnvironment::insideShader);
  ShEnvironment::shader = new ShProgramNode(target);
  ShEnvironment::insideShader = true;
  return ShEnvironment::shader;
}

void shEndShader()
{
  assert(ShEnvironment::insideShader);
  
  ShEnvironment::shader->ctrlGraph = new ShCtrlGraph(ShEnvironment::shader->tokenizer.blockList());

  ShOptimizer optimizer(ShEnvironment::shader->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  ShEnvironment::shader->collectVariables();
  
  ShEnvironment::insideShader = false;

  if (ShEnvironment::shader->target() == "gpu:vertex" ||
      ShEnvironment::shader->target() == "gpu:fragment") {
    shCompileShader(ShEnvironment::shader);
  }
}

void shCompileShader(ShProgram& shader)
{
  if (!ShEnvironment::backend) return;
  shader->compile(ShEnvironment::backend);
}

void shCompileShader(const std::string& target, ShProgram& shader)
{
  if (!ShEnvironment::backend) return;
  shader->compile(target, ShEnvironment::backend);
}

void shBindShader(ShProgram& shader)
{
  if (!ShEnvironment::backend) return;
  shader->code(ShEnvironment::backend)->bind();
}

void shBindShader(const std::string& target, ShProgram& shader)
{
  if (!ShEnvironment::backend) return;
  shader->code(target, ShEnvironment::backend)->bind();
}

void shIf(bool)
{
  ShRefCount<ShToken> token = new ShToken(SH_TOKEN_IF);

  token->arguments.push_back(ShEnvironment::shader->tokenizer.getArgument());
  ShEnvironment::shader->tokenizer.popArgQueue();

  ShEnvironment::shader->tokenizer.blockList()->addBlock(token);
}

void shEndIf()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDIF));
}

void shElse()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ELSE));
}

void shWhile(bool)
{
  ShRefCount<ShToken> token = new ShToken(SH_TOKEN_WHILE);

  token->arguments.push_back(ShEnvironment::shader->tokenizer.getArgument());
  ShEnvironment::shader->tokenizer.popArgQueue();

  ShEnvironment::shader->tokenizer.blockList()->addBlock(token);
}

void shEndWhile()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDWHILE));
}

void shDo()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_DO));
}

void shUntil(bool)
{
  ShRefCount<ShToken> token = new ShToken(SH_TOKEN_UNTIL);

  token->arguments.push_back(ShEnvironment::shader->tokenizer.getArgument());
  ShEnvironment::shader->tokenizer.popArgQueue();

  ShEnvironment::shader->tokenizer.blockList()->addBlock(token);
}

void shFor(bool)
{
  ShRefCount<ShToken> token = new ShToken(SH_TOKEN_FOR);

  for (int i = 0; i < 3; i++) {
    token->arguments.push_back(ShEnvironment::shader->tokenizer.getArgument());
  }
  ShEnvironment::shader->tokenizer.popArgQueue();

  ShEnvironment::shader->tokenizer.blockList()->addBlock(token);
}

void shEndFor()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDFOR));
}

void ShBreak()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_BREAK));
}

void ShContinue()
{
  ShEnvironment::shader->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_CONTINUE));
}

}
