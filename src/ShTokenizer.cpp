#include "ShTokenizer.hpp"

namespace SH {

ShTokenizerException::ShTokenizerException(const std::string& error)
  : ShException("Tokenizer error: " + error)
{
}
  
ShTokenizer::ShTokenizer()
{
  m_listStack.push(new ShBlockList());
}

bool ShTokenizer::pushArgQueue()
{
  m_argQueueStack.push(std::queue<ShTokenArgument>());
  return true;
}

bool ShTokenizer::pushArg()
{
  m_listStack.push(new ShBlockList(true));
  return true;
}

bool ShTokenizer::processArg(const ShVariable& result)
{
  
  if (!m_listStack.top()->isArgument()) {
    throw ShTokenizerException("Attempt to process an argument outside of an argument context.");
  }
  
  if (m_argQueueStack.empty()) {
    throw ShTokenizerException("Attempt to process an argument without a context.");
  }
  
  m_argQueueStack.top().push(ShTokenArgument(result, m_listStack.top()));
  m_listStack.pop();
  return true;
}

ShBlockListPtr ShTokenizer::blockList()
{
  if (m_listStack.empty()) {
    throw ShTokenizerException("No current tokenized list.");
  }
  return m_listStack.top();
}

ShTokenArgument ShTokenizer::getArgument()
{
  if (m_argQueueStack.empty()) {
    throw ShTokenizerException("getArgument(): Argument stack underflow");
  }
  if (m_argQueueStack.top().empty()) {
    throw ShTokenizerException("getArgument(): Argument is empty");
  }
  ShTokenArgument r = m_argQueueStack.top().front();
  m_argQueueStack.top().pop();
  return r;
}

void ShTokenizer::popArgQueue()
{
  if (m_argQueueStack.empty()) {
    throw ShTokenizerException("popArgQueue(): Argument stack underflow");
  }
  m_argQueueStack.pop();
}

}



