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



