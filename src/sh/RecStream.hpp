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
#ifndef SHRECSTREAM_HPP
#define SHRECSTREAM_HPP

#include "DllExport.hpp"
#include "Record.hpp"
#include "Stream.hpp"

/** @file RecStream.hpp
 * Stream/Record interconversion functions.
 * (Hopefully this will soon be unnecessary if the internal representation of 
 * variables/channels becomes the same)
 *
 * @todo range - lots of these functions won't work if the data stored in 
 * memory doesn't match the host computation type
 */
namespace SH {
// Turns a record into a one element stream.
// This uses the record's memory DIRECTLY, so you can
// set values in the stream...
Stream toStream(const Record &record);

// Turns a stream element into a record 
// This uses the stream's memory DIRECTLY, so you can
// set values in the stream...
Record toRecord(const Stream &stream, int index);

// Generates an n-element stream with undefined values using the record as a
// pattern 
Stream makeStream(const Record &record, int n); 

// Generates a record using the stream as a pattern
Record makeRecord(const Stream &stream, BindingType kind=SH_TEMP);

// Fetches the indexed element from a stream as a record
void lookup(Record& record, const Stream &stream, int index);

// Fetches the indexed element from a stream as a record
void set(Stream &stream, int index, const Record& record);

}

#endif
