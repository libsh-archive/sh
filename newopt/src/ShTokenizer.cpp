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
#include "ShTokenizer.hpp"
#include "ShError.hpp"

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
    shError( ShTokenizerException("Attempt to process an argument outside of an argument context.") );
  }
  
  if (m_argQueueStack.empty()) {
    shError( ShTokenizerException("Attempt to process an argument without a context.") );
  }
  
  m_argQueueStack.top().push(ShTokenArgument(result, m_listStack.top()));
  m_listStack.pop();
  return true;
}

ShBlockListPtr ShTokenizer::blockList()
{
  if (m_listStack.empty()) {
    shError( ShTokenizerException("No current tokenized list.") );
  }
  return m_listStack.top();
}

ShTokenArgument ShTokenizer::getArgument()
{
  if (m_argQueueStack.empty()) {
    shError( ShTokenizerException("getArgument(): Argument stack underflow") );
  }
  if (m_argQueueStack.top().empty()) {
    shError( ShTokenizerException("getArgument(): Argument is empty") );
  }
  ShTokenArgument r = m_argQueueStack.top().front();
  m_argQueueStack.top().pop();
  return r;
}

void ShTokenizer::popArgQueue()
{
  if (m_argQueueStack.empty()) {
    shError( ShTokenizerException("popArgQueue(): Argument stack underflow") );
  }
  m_argQueueStack.pop();
}

}



