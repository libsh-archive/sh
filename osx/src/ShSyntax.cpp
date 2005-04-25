// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShInfo.hpp"
#include "ShStatement.hpp"
#include "ShProgram.hpp"
#include "ShBackend.hpp"
#include "ShTransformer.hpp"
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
  
  context->exit();

  parsing->finish();

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

void shBind(const ShProgramSet& s)
{
  if (!ShEnvironment::backend) return;
  s.backend_set(ShEnvironment::backend)->bind();
}

void shBind(const std::string& target, ShProgram& prg)
{
  if (!ShEnvironment::backend) return;
  prg.code(target, ShEnvironment::backend)->bind();
}

void shUnbind()
{
  if (!ShEnvironment::backend) return;
  ShEnvironment::backend->unbind_all();
}

void shUnbind(ShProgram& prg)
{
  if (!ShEnvironment::backend) return;
  prg.code(ShEnvironment::backend)->unbind();
}

void shUnbind(const ShProgramSet& s)
{
  if (!ShEnvironment::backend) return;
  s.backend_set(ShEnvironment::backend)->unbind();
}

void shUnbind(const std::string& target, ShProgram& prg)
{
  if (!ShEnvironment::backend) return;
  prg.code(target, ShEnvironment::backend)->unbind();
}

void shLink(const ShProgramSet& s)
{
  if (!ShEnvironment::backend) return;
  s.backend_set(ShEnvironment::backend)->link();
}

typedef std::map<std::string, ShProgram> BoundProgramMap;

void shUpdate()
{
  if (!ShEnvironment::backend) return;
  
  for (BoundProgramMap::iterator i = ShContext::current()->begin_bound(); 
       i != ShContext::current()->end_bound(); i++) {
    i->second.code(ShEnvironment::backend)->update();
  }
}

void shBindShader(ShProgram& shader)
{
  shBind(shader);
}

void shBindShader(const std::string& target, ShProgram& shader)
{
  shBind(target, shader);
}

bool shSetBackend(const std::string& name)
{
  ShBackendPtr backend = SH::ShBackend::lookup(name);
  if (!backend) return false;
  SH::ShEnvironment::backend = backend;
  return true;
}

bool shEvaluateCondition(const ShVariable& arg)
{
  bool cond = false;
  for (int i = 0; i < arg.size(); i++) {
    cond = cond || arg.getVariant(i)->isTrue();
  }
  return cond;
}


bool shProcessArg(const ShVariable& arg,
                  bool* internal_cond)
{
  if (ShContext::current()->parsing()) {
    ShContext::current()->parsing()->tokenizer.processArg(arg);
  } else {
    if (internal_cond) *internal_cond = shEvaluateCondition(arg);
  }
  return true;
}

bool shPushArgQueue()
{
  if (ShContext::current()->parsing()) ShContext::current()->parsing()->tokenizer.pushArgQueue();
  return true;
}

bool shPushArg()
{
  if (ShContext::current()->parsing()) ShContext::current()->parsing()->tokenizer.pushArg();
  return true;
}

void shInit()
{
  ShContext::current();
  // TODO: Initialize backends
}

void shIf(bool)
{
  if (!ShContext::current()->parsing()) return;
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_IF);
    
  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();
    
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndIf()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDIF));
}

void shElse()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ELSE));
}

void shWhile(bool)
{
  if (!ShContext::current()->parsing()) return;
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_WHILE);

  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndWhile()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDWHILE));
}

void shDo()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_DO));
}

void shUntil(bool)
{
  if (!ShContext::current()->parsing()) return;
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_UNTIL);

  token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shFor(bool)
{
  if (!ShContext::current()->parsing()) return;
  ShPointer<ShToken> token = new ShToken(SH_TOKEN_FOR);

  for (int i = 0; i < 3; i++) {
    token->arguments.push_back(ShContext::current()->parsing()->tokenizer.getArgument());
  }
  ShContext::current()->parsing()->tokenizer.popArgQueue();

  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void shEndFor()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDFOR));
}

void ShBreak()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_BREAK));
}

void ShContinue()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_CONTINUE));
}

void shBeginSection()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_STARTSEC));
}

void shEndSection()
{
  if (!ShContext::current()->parsing()) return;
  ShContext::current()->parsing()->tokenizer.blockList()->addBlock(new ShToken(SH_TOKEN_ENDSEC));
}

void shComment(const std::string& comment)
{
  if (!ShContext::current()->parsing()) return;
  ShStatement stmt(SH_OP_COMMENT);
  stmt.add_info(new ShInfoComment(comment));
  ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
}

}
