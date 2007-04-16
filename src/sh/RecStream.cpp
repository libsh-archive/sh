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

#include "Debug.hpp"
#include "DataType.hpp"
#include "TypeInfo.hpp"
#include "RecStream.hpp"

namespace SH { 

// Turns a record into a one element stream copying the current values
Stream toStream(const Record &record)
{
  Stream result;
  for(Record::const_iterator I = record.begin(); I != record.end(); ++I) {
    SH_DEBUG_ASSERT(I->swizzle().identity());

    // generate memory
    VariantPtr variant = I->node()->getVariant();
    HostMemoryPtr mem(new HostMemory(
          variant->size() * variant->datasize(), 
          variant->array()));

    ChannelNodePtr channel(new ChannelNode(I->node()->specialType(),
        I->size(), I->valueType(), mem, 1));
    result.append(channel);
  }
  return result;
}

Record toRecord(const Stream &stream, int index)
{
  Record result;
  Stream::const_iterator C = stream.begin();

  for(; C != stream.end(); ++C) {
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    MemoryPtr mem = (*C)->memory();
    HostStoragePtr hostStrg = 
      shref_dynamic_cast<HostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    ValueType valueType = (*C)->valueType();
    int datasize = typeInfo(valueType, SH_HOST)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    VariantPtr variant = variantFactory(valueType, SH_HOST)->
      generate(data, size, false);

    Variable var(new VariableNode(SH_TEMP, size, valueType));
    var.node()->changeVariant(variant);
    result.append(var);
  }
  return result;
}

Stream makeStream(const Record &record, int n)
{
  Stream result;
  for(Record::const_iterator I = record.begin(); I != record.end(); ++I) {
    int size = I->size();
    ValueType valueType = I->valueType();
    SemanticType specialType = I->node()->specialType();
    const std::string& name = I->name();
    int datasize = typeInfo(valueType, SH_MEM)->datasize();

    HostMemoryPtr mem(new HostMemory(
          size * datasize * n));

    ChannelNodePtr channel(new ChannelNode(specialType,
          size, valueType, mem, n));
    channel->name(name);
    result.append(channel);
  }
  return result;
}

Record makeRecord(const Stream &stream, BindingType kind)
{
  Record result;
  for(Stream::const_iterator I = stream.begin();
      I != stream.end(); ++I) {
    VariableNodePtr node = new VariableNode(kind,
        (*I)->size(), (*I)->valueType(), (*I)->specialType());

    result.append(Variable(node));
  }
  return result;
}

// Fetches the indexed element from a stream as a record
void lookup(Record& record, const Stream &stream, int index)
{
  SH_DEBUG_ASSERT(record.size() == stream.size());
  Record::iterator R = record.begin();
  Stream::const_iterator C = stream.begin();

  for(; C != stream.end(); ++R, ++C) {
    SH_DEBUG_ASSERT((*C)->size() == R->size());
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    MemoryPtr mem = (*C)->memory();
    HostStoragePtr hostStrg = 
      shref_dynamic_cast<HostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    ValueType valueType = (*C)->valueType();
    int datasize = typeInfo(valueType, SH_MEM)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    VariantPtr variant = variantFactory(valueType, SH_MEM)->
      generate(data, size, false);
    SH_DEBUG_PRINT("Getting " << size << " elements from idx " << index << 
        " = " << variant->encodeArray());

    R->setVariant(variant);
  }
}

// Fetches the indexed element from a stream as a record
void set(Stream &stream, int index, const Record& record)
{
  SH_DEBUG_ASSERT(record.size() == stream.size());
  Record::const_iterator R = record.begin();
  Stream::iterator C = stream.begin();

  for(; C != stream.end(); ++R, ++C) {
    // @todo range - factor out some of this code with above
    SH_DEBUG_ASSERT((*C)->size() == R->size());
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    MemoryPtr mem = (*C)->memory();
    HostStoragePtr hostStrg = 
      shref_dynamic_cast<HostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    int datasize = typeInfo((*C)->valueType(), SH_MEM)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    VariantPtr chanVariant = variantFactory((*C)->valueType(), SH_MEM)->
      generate(data, size, false);

    chanVariant->set(R->getVariant());
  }
}

}
