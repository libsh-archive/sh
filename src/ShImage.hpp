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
#ifndef SHIMAGE_HPP
#define SHIMAGE_HPP

#include <string>
#include "ShRefCount.hpp"

namespace SH {

class ShImage : public ShRefCountable {
public:
  ShImage();
  ShImage(int width, int height, int depth);
  ShImage(const ShImage& other);

  ~ShImage();

  ShImage& operator=(const ShImage& other);

  int width() const;
  int height() const;
  int depth() const;

  float operator()(int x, int y, int i) const;
  float& operator()(int x, int y, int i);

  void loadPng(const std::string& filename);

  const float* data() const;
  
private:
  int m_width, m_height;
  int m_depth;
  float* m_data;
};

}

#endif
