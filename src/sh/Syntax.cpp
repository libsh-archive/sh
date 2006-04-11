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
#include <iostream>
#include <fstream>
#include <cassert>
#include "Syntax.hpp"
#include "Context.hpp"
#include "Tokenizer.hpp"
#include "Token.hpp"
#include "Info.hpp"
#include "Statement.hpp"
#include "Program.hpp"
#include "Backend.hpp"
#include "Transformer.hpp"
#include "Optimizations.hpp"

namespace SH {

Program beginShader(const std::string& target)
{
  Program prg(target);
  Context::current()->enter(prg.node());
  return prg;
}

void endShader()
{
  Context* context = Context::current();

  ProgramNodePtr parsing = context->parsing();
  assert(parsing);
  
  parsing->ctrlGraph = new CtrlGraph(parsing->tokenizer.blockList());
  
  optimize(parsing);
  
  context->exit();

  parsing->finish();
}

void compile(Program& prg)
{
  compile(prg, prg.target());
}

void compile(Program& prg, const std::string& target)
{
  BackendPtr backend = Backend::get_backend(target);
  if (!backend) return;
  prg.compile(target, backend);
}

void compileShader(Program& prg)
{
  compile(prg);
}

void compileShader(const std::string& target, Program& prg)
{
  compile(prg, target);
}

void bind(Program& prg)
{
  BackendPtr backend = Backend::get_backend(prg.target());
  if (!backend) return;
  prg.code(backend)->bind();
}

void bind(const ProgramSet& s)
{
  BackendPtr backend = Backend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->bind();
}

void bind(const std::string& target, Program& prg)
{
  BackendPtr backend = Backend::get_backend(target);
  if (!backend) return;
  prg.code(target, backend)->bind();
}

void unbind()
{
  Backend::unbind_all_backends();
}

void unbind(Program& prg)
{
  BackendPtr backend = Backend::get_backend(prg.target());
  if (!backend) return;
  prg.code(backend)->unbind();
}

void unbind(const ProgramSet& s)
{
  BackendPtr backend = Backend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->unbind();
}

void unbind(const std::string& target, Program& prg)
{
  BackendPtr backend = Backend::get_backend(target);
  if (!backend) return;
  prg.code(target, backend)->unbind();
}

void link(const ProgramSet& s)
{
  BackendPtr backend = Backend::get_backend((*(s.begin()))->target());
  if (!backend) return;
  s.backend_set(backend)->link();
}

typedef std::map<std::string, Program> BoundProgramMap;

void update()
{
  const BoundProgramMap::iterator begin = Context::current()->begin_bound();
  const BoundProgramMap::iterator end   = Context::current()->end_bound();
  for (BoundProgramMap::iterator i = begin; i != end; ++i) {
    BackendPtr backend = Backend::get_backend(i->second.target());
    if (backend) {
      i->second.code(backend)->update();
    }
  }
}

void bindShader(Program& shader)
{
  bind(shader);
}

void bindShader(const std::string& target, Program& shader)
{
  bind(target, shader);
}

bool setBackend(const std::string& name)
{
  SH::Backend::clear_backends();
  return useBackend(name);
}

bool useBackend(const std::string& name)
{
  if (name.empty()) return false;
  return SH::Backend::use_backend(name);
}

bool haveBackend(const std::string& name)
{
  if (name.empty()) return false;
  return SH::Backend::have_backend(name);
}

void clearBackends()
{
  return SH::Backend::clear_backends();
}

std::string findBackend(const std::string& target)
{
  if (target.empty()) return "";
  return SH::Backend::target_handler(target, false);
}

void registerBackend(const std::string& backend_name, SH::Backend::InstantiateEntryPoint *instantiate, SH::Backend::TargetCostEntryPoint *target_cost) {
  SH::Backend::register_backend(backend_name, instantiate, target_cost);
}

bool evaluateCondition(const Variable& arg)
{
  bool cond = false;
  for (int i = 0; i < arg.size(); i++) {
    cond = cond || arg.getVariant(i)->isTrue();
  }
  return cond;
}

bool processArg(const Variable& arg,
                  bool* internal_cond)
{
  if (Context::current()->parsing()) {
    Context::current()->parsing()->tokenizer.processArg(arg);
  } else {
    if (internal_cond) *internal_cond = evaluateCondition(arg);
  }
  return true;
}

bool pushArgQueue()
{
  if (Context::current()->parsing()) Context::current()->parsing()->tokenizer.pushArgQueue();
  return true;
}

bool pushArg()
{
  if (Context::current()->parsing()) Context::current()->parsing()->tokenizer.pushArg();
  return true;
}

void init()
{
  Context::current();
  // TODO: Initialize backends
}

void internal_if(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_IF);
    
  token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  Context::current()->parsing()->tokenizer.popArgQueue();
    
  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void endIf()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_ENDIF));
}

void internal_else()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_ELSE));
}

void internal_while(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_WHILE);

  token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  Context::current()->parsing()->tokenizer.popArgQueue();

  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void endWhile()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_ENDWHILE));
}

void internal_do()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_DO));
}

void until(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_UNTIL);

  token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  Context::current()->parsing()->tokenizer.popArgQueue();

  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void internal_for(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_FOR);

  for (int i = 0; i < 3; i++) {
    token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  }
  Context::current()->parsing()->tokenizer.popArgQueue();

  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void endFor()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_ENDFOR));
}

void internal_break(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_BREAK);

  token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  Context::current()->parsing()->tokenizer.popArgQueue();

  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void internal_continue(bool)
{
  if (!Context::current()->parsing()) return;
  Pointer<Token> token = new Token(TOKEN_CONTINUE);

  token->arguments.push_back(Context::current()->parsing()->tokenizer.getArgument());
  Context::current()->parsing()->tokenizer.popArgQueue();

  Context::current()->parsing()->tokenizer.blockList()->addBlock(token);
}

void beginSection()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_STARTSEC));
}

void endSection()
{
  if (!Context::current()->parsing()) return;
  Context::current()->parsing()->tokenizer.blockList()->addBlock(new Token(TOKEN_ENDSEC));
}

void comment(const std::string& comment)
{
  if (!Context::current()->parsing()) return;
  Statement stmt(OP_COMMENT);
  stmt.add_info(new InfoComment(comment));
  Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
}

}
