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
#ifndef SH_ARRAY_DATA_HPP
#define SH_ARRAY_DATA_HPP

#include <vector>
#include <string>

namespace SH {

// 
  enum RenderType {
    SH_TRI, 
    SH_TRIANGLE_STRIPS,
    SH_QUAD_STRIPS,
    SH_QUADS, 
    SH_UNKNOWN_RENDERTYPE,
  };

  enum FormatType {
    SH_RGB, 
    SH_RGBA, 
    SH_2D, 
    SH_UNKNOWN_FORMATTYPE,
  };

template<typename T, typename  N>
class ShArrayData {


public:
  ShArrayData();

  ShArrayData(const ShArrayData&);
  ShArrayData& operator=(const ShArrayData&);

  // unimplemented yet
  ShArrayData& operator+(const ShArrayData&);
  ShArrayData& operator+=(const ShArrayData&);


  ~ShArrayData();

  /// API

  // return 1 for success
  int assertvalid()const;

  /// Input
  unsigned int addData(T* data, FormatType width, unsigned int nrec, std::string label, bool bCopy = 0);
  unsigned  int addIndex(N* data, RenderType rtype, unsigned int nrec, std::string label, bool bCopy = 0);
     

  /// OUtput
  T* getData(unsigned int i, bool bCopy = 0)const;
  N* getIndex(unsigned int i, bool bCopy = 0)const;
  unsigned int getDataSize(unsigned int i)const;
  unsigned int getIndexSize(unsigned int i)const;
  
  unsigned int getDataSize()const;
  unsigned int getIndexSize()const;
  
  std::string getDataLabel(unsigned int i)const;
  std::string getIndexLabel(unsigned int i)const;

  FormatType getFormatType(unsigned int i)const;
  RenderType getRenderType(unsigned int i)const;

  unsigned int getWidth(FormatType)const;

private:

  // stores the various arrays
  std::vector<T*> data;

  // stores the various indices
  std::vector<N*> index;

  /// rendering type
  std::vector<RenderType> type;

  /// data width
  std::vector<FormatType> width;

  /// buffer and index sizes
  std::vector<unsigned int> data_size, index_size;
  
  /// labels
  std::vector<std::string> data_labels, index_labels;

  /// copy flags: for the destructors
  std::vector<bool> data_copy_flags, index_copy_flags;

  // the copy functionality is here since it is not trivial and we use it multiple times
  void copy(const ShArrayData&);

  // free functionality. We need it here since it is used in multiple places
  void clear();
};

typedef ShArrayData<double, unsigned short> ShVertexArray;

}

#endif

