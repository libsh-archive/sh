#include <iostream>
#include <fstream>
#include <cassert>
#include "ShSyntax.hpp"
#include "ShEnvironment.hpp"
#include "ShTokenizer.hpp"
#include "ShToken.hpp"
#include "ShShader.hpp"
#include "ShBackend.hpp"
#include "ShDomTree.hpp"

namespace SH {

ShShader shBeginShader(int kind)
{
  assert(!ShEnvironment::insideShader);
  ShEnvironment::shader = new ShShaderNode(kind);
  ShEnvironment::insideShader = true;
  return ShEnvironment::shader;
}

void shEndShader()
{
  assert(ShEnvironment::insideShader);

  // DEBUG
  std::cerr << "--- Tokenized: " << std::endl;
  std::cerr << *ShEnvironment::shader->tokenizer.blockList();
  
  ShEnvironment::shader->ctrlGraph = new ShCtrlGraph(ShEnvironment::shader->tokenizer.blockList());

  ShDomTree domTree(ShEnvironment::shader->ctrlGraph);
  domTree.debugDump();

  /*
  std::cerr << "--- Control graph: " << std::endl;
  ShEnvironment::shader->ctrlGraph->print(std::cerr, 0);
  */
  std::ofstream dotfile("control.dot");
  ShEnvironment::shader->ctrlGraph->graphvizDump(dotfile);

  ShEnvironment::shader->collectVariables();

  ShBackendPtr backend = ShBackend::lookup("sm");
  ShBackendCodePtr code = backend->generateCode(ShEnvironment::shader);
  code->print(std::cerr);
  
  ShEnvironment::insideShader = false;
}

void shBindShader(ShShader& shader)
{
  if (!ShEnvironment::backend) return;
  shader->code(ShEnvironment::backend)->bind();
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
