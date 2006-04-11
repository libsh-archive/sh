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
#include "Tokenizer.hpp"
#include "Error.hpp"

namespace SH {

TokenizerException::TokenizerException(const std::string& error)
  : Exception("Tokenizer error: " + error)
{
}
  
Tokenizer::Tokenizer()
{
  m_listStack.push(new BlockList());
}

bool Tokenizer::pushArgQueue()
{
  m_argQueueStack.push(std::queue<TokenArgument>());
  return true;
}

bool Tokenizer::pushArg()
{
  m_listStack.push(new BlockList(true));
  return true;
}

bool Tokenizer::processArg(const Variable& result)
{
  
  if (!m_listStack.top()->isArgument()) {
    error( TokenizerException("Attempt to process an argument outside of an argument context.") );
  }
  
  if (m_argQueueStack.empty()) {
    error( TokenizerException("Attempt to process an argument without a context.") );
  }
  
  m_argQueueStack.top().push(TokenArgument(result, m_listStack.top()));
  m_listStack.pop();
  return true;
}

BlockListPtr Tokenizer::blockList()
{
  if (m_listStack.empty()) {
    error( TokenizerException("No current tokenized list.") );
  }
  return m_listStack.top();
}

TokenArgument Tokenizer::getArgument()
{
  if (m_argQueueStack.empty()) {
    error( TokenizerException("getArgument(): Argument stack underflow") );
  }
  if (m_argQueueStack.top().empty()) {
    error( TokenizerException("getArgument(): Argument is empty") );
  }
  TokenArgument r = m_argQueueStack.top().front();
  m_argQueueStack.top().pop();
  return r;
}

void Tokenizer::popArgQueue()
{
  if (m_argQueueStack.empty()) {
    error( TokenizerException("popArgQueue(): Argument stack underflow") );
  }
  m_argQueueStack.pop();
}

}



