#include <iostream>
#include <fstream>
#include "ShSyntax.hpp"
#include "ShEnvironment.hpp"
#include "ShTokenizer.hpp"
#include "ShToken.hpp"
#include "ShBackend.hpp"

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
  ShEnvironment::insideShader = false;

  // DEBUG
  std::cerr << "--- Tokenized: " << std::endl;
  std::cerr << *ShEnvironment::shader->tokenizer.blockList();
  
  ShEnvironment::shader->ctrlGraph = new ShCtrlGraph(ShEnvironment::shader->tokenizer.blockList());

  /*
  std::cerr << "--- Control graph: " << std::endl;
  ShEnvironment::shader->ctrlGraph->print(std::cerr, 0);
  */
  std::ofstream dotfile("control.dot");
  ShEnvironment::shader->ctrlGraph->graphvizDump(dotfile);

  ShEnvironment::shader->collectVariables();
  
  std::cerr << "--- SM code:" << std::endl;
  ShBackendPtr backend = ShBackend::lookup("sm");
  if (backend) {
    ShBackendCodePtr code = backend->generateCode(ShEnvironment::shader);
    code->print(std::cerr);
  } else {
    std::cerr << "SM backend not loaded!" << std::endl;
  }
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
