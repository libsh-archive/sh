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

#include "ShDebug.hpp"
#include "ShDataType.hpp"
#include "ShTypeInfo.hpp"
#include "ShRecStream.hpp"

namespace SH { 

// Turns a record into a one element stream copying the current values
ShStream toStream(const ShRecord &record)
{
  ShStream result;
  for(ShRecord::const_iterator I = record.begin(); I != record.end(); ++I) {
    SH_DEBUG_ASSERT(I->swizzle().identity());

    // generate memory
    ShVariantPtr variant = I->node()->getVariant();
    ShHostMemoryPtr mem(new ShHostMemory(
          variant->size() * variant->datasize(), 
          variant->array()));

    ShChannelNodePtr channel(new ShChannelNode(I->node()->specialType(),
        I->size(), I->valueType(), mem, 1));
    result.append(channel);
  }
  return result;
}

ShRecord toRecord(const ShStream &stream, int index)
{
  ShRecord result;
  ShStream::const_iterator C = stream.begin();

  for(; C != stream.end(); ++C) {
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    ShMemoryPtr mem = (*C)->memory();
    ShHostStoragePtr hostStrg = 
      shref_dynamic_cast<ShHostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    ShValueType valueType = (*C)->valueType();
    int datasize = shTypeInfo(valueType, SH_HOST)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    ShVariantPtr variant = shVariantFactory(valueType, SH_HOST)->
      generate(data, size, false);

    ShVariable var(new ShVariableNode(SH_TEMP, size, valueType));
    var.node()->changeVariant(variant);
    result.append(var);
  }
  return result;
}

ShStream makeStream(const ShRecord &record, int n)
{
  ShStream result;
  for(ShRecord::const_iterator I = record.begin(); I != record.end(); ++I) {
    int size = I->size();
    ShValueType valueType = I->valueType();
    ShSemanticType specialType = I->node()->specialType();
    const std::string& name = I->name();
    int datasize = shTypeInfo(valueType, SH_MEM)->datasize();

    ShHostMemoryPtr mem(new ShHostMemory(
          size * datasize * n));

    ShChannelNodePtr channel(new ShChannelNode(specialType,
          size, valueType, mem, n));
    channel->name(name);
    result.append(channel);
  }
  return result;
}

ShRecord makeRecord(const ShStream &stream, ShBindingType kind)
{
  ShRecord result;
  for(ShStream::const_iterator I = stream.begin();
      I != stream.end(); ++I) {
    ShVariableNodePtr node = new ShVariableNode(kind,
        (*I)->size(), (*I)->valueType(), (*I)->specialType());

    result.append(ShVariable(node));
  }
  return result;
}

// Fetches the indexed element from a stream as a record
void lookup(ShRecord& record, const ShStream &stream, int index)
{
  SH_DEBUG_ASSERT(record.size() == stream.size());
  ShRecord::iterator R = record.begin();
  ShStream::const_iterator C = stream.begin();

  for(; C != stream.end(); ++R, ++C) {
    SH_DEBUG_ASSERT((*C)->size() == R->size());
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    ShMemoryPtr mem = (*C)->memory();
    ShHostStoragePtr hostStrg = 
      shref_dynamic_cast<ShHostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    ShValueType valueType = (*C)->valueType();
    int datasize = shTypeInfo(valueType, SH_MEM)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    ShVariantPtr variant = shVariantFactory(valueType, SH_MEM)->
      generate(data, size, false);
    SH_DEBUG_PRINT("Getting " << size << " elements from idx " << index << 
        " = " << variant->encodeArray());

    R->setVariant(variant);
  }
}

// Fetches the indexed element from a stream as a record
void set(ShStream &stream, int index, const ShRecord& record)
{
  SH_DEBUG_ASSERT(record.size() == stream.size());
  ShRecord::const_iterator R = record.begin();
  ShStream::iterator C = stream.begin();

  for(; C != stream.end(); ++R, ++C) {
    // @todo range - factor out some of this code with above
    SH_DEBUG_ASSERT((*C)->size() == R->size());
    SH_DEBUG_ASSERT(0 <= index && index < (*C)->count());

    // Grab the data as a memory variant
    ShMemoryPtr mem = (*C)->memory();
    ShHostStoragePtr hostStrg = 
      shref_dynamic_cast<ShHostStorage>(mem->findStorage("host"));
    SH_DEBUG_ASSERT(hostStrg);

    int datasize = shTypeInfo((*C)->valueType(), SH_MEM)->datasize();
    int size = (*C)->size();
    int byteOffset = index * size * datasize;
    char* data = reinterpret_cast<char*>(hostStrg->data()) + byteOffset; 

    ShVariantPtr chanVariant = shVariantFactory((*C)->valueType(), SH_MEM)->
      generate(data, size, false);

    chanVariant->set(R->getVariant());
  }
}

}
