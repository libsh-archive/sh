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

  // TODO: compile the program? See issue129.
  
}

void shCompile(ShProgram& prg)
{
  ShBackendPtr backend = ShBackend::get_backend(prg.target());
  if (!backend) return;
  prg.compile(backend);
}

void shCompile(ShProgram& prg, const std::string& target)
{
  ShBackendPtr backend = ShBackend::get_backend(target);
  if (!backend) return;
  prg.compile(target, backend);
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
  ShBackendPtr backend = ShBackend::get_backend(prg.target());
  if (!backend) return;
  prg.code(backend)->bind();
}

void shBind(const ShProgramSet& s)
{
  ShBackendPtr backend = ShBackend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->bind();
}

void shBind(const std::string& target, ShProgram& prg)
{
  ShBackendPtr backend = ShBackend::get_backend(target);
  if (!backend) return;
  prg.code(target, backend)->bind();
}

void shUnbind()
{
  for (ShBackend::ShBackendList::iterator i = ShBackend::begin(); 
       i != ShBackend::end(); i++) {
    (*i)->unbind_all();
  }
}

void shUnbind(ShProgram& prg)
{
  ShBackendPtr backend = ShBackend::get_backend(prg.target());
  if (!backend) return;
  prg.code(backend)->unbind();
}

void shUnbind(const ShProgramSet& s)
{
  ShBackendPtr backend = ShBackend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->unbind();
}

void shUnbind(const std::string& target, ShProgram& prg)
{
  ShBackendPtr backend = ShBackend::get_backend(target);
  if (!backend) return;
  prg.code(target, backend)->unbind();
}

void shLink(const ShProgramSet& s)
{
  ShBackendPtr backend = ShBackend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->link();
}

typedef std::map<std::string, ShProgram> BoundProgramMap;

void shUpdate()
{
  for (BoundProgramMap::iterator i = ShContext::current()->begin_bound(); 
       i != ShContext::current()->end_bound(); i++) {
    ShBackendPtr backend = ShBackend::get_backend(i->second.target());
    if (backend) {
      i->second.code(backend)->update();
    }
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
  SH::ShBackend::clear_backends();
  return SH::ShBackend::use_backend(name);
}

bool shUseBackend(const std::string& name)
{
  return SH::ShBackend::use_backend(name);
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
