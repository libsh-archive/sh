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
#ifndef SHSTREAM_HPP
#define SHSTREAM_HPP

#include <list>
#include "DllExport.hpp"
#include "BaseTexture.hpp"

namespace SH {

/** Dynamic list of channels.
 * The stream keep track (by reference) of an ordered lists of 
 * data channels, to be used as inputs and outputs of stream operations.
 * @see Channel
 */
class
SH_DLLEXPORT Stream {
public:
  typedef std::list<BaseTexture> NodeList;
  typedef NodeList::iterator iterator;
  typedef NodeList::const_iterator const_iterator;
  typedef NodeList::size_type size_type;

  Stream();
  Stream(const BaseTexture& array);

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  size_type size() const;

  void append(const BaseTexture& array);
  void prepend(const BaseTexture& array);

  // Execute fully bound stream program and place results in stream.
  Stream& operator=(const Program& program);
  
private:
  NodeList m_nodes;
};

/** Combine two streams.
 * This concatenates the list of channels in the component streams.
 */
SH_DLLEXPORT
Stream combine(const BaseTexture& left, const BaseTexture& right);

/** Combine a stream and a channel.
 * This concatenates the given channel to the end of the list of
 * channels in the stream.
 */
SH_DLLEXPORT
Stream combine(const Stream& left, const BaseTexture& right);

/** Combine a channel and a stream.
 * This concatenates the given channel to the start of the list of
 * channels in the stream.
 */
SH_DLLEXPORT
Stream combine(const BaseTexture& left, const Stream& right);

SH_DLLEXPORT
Stream combine(const Stream& left, const Stream& right);

/** An operator alias for combine between channels.
 */
SH_DLLEXPORT
Stream operator&(const BaseTexture& left, const BaseTexture& right);

/** An operator alias for combine between a stream and a channel.
 */
SH_DLLEXPORT
Stream operator&(const Stream& left, const BaseTexture& right);

/** An operator alias for combine between a channel and a stream.
 */
SH_DLLEXPORT
Stream operator&(const BaseTexture& left, const Stream& right);

/** An operator alias for combine between two streams.
 */
SH_DLLEXPORT
Stream operator&(const Stream& left, const Stream& right);

/** Apply a program to a stream. 
 * This function connects streams onto the output of programs
 */
SH_DLLEXPORT
Program connect(const Stream& stream, const Program& program);
// TODO: is this right?  why is the stream argument first?

/** An operator alias for connect(p,s).
 */
SH_DLLEXPORT
Program operator<<(const Program& program, const Stream& stream);

SH_DLLEXPORT
Program connect(const BaseTexture& array, const Program& program);

SH_DLLEXPORT
Program operator<<(const Program& program, const BaseTexture& array);

}

#endif
