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
#ifndef SHQUATERNION_HPP
#define SHQUATERNION_HPP

#include <iostream>
#include "ShLib.hpp"

namespace SH {
  class ShQuaternion
  {
    friend std::ostream& operator<<(std::ostream& out, const ShQuaternion& q);
    public:
      ShQuaternion();
      ShQuaternion(const ShQuaternion& other);
      ShQuaternion(const ShVector4f& values);
      ShQuaternion(const ShAttrib1f& angle, const ShVector3f& axis);

      ShQuaternion& operator=(const ShQuaternion& other);
      ShQuaternion& operator+=(const ShQuaternion& right);
      ShQuaternion& operator-=(const ShQuaternion& right);
      ShQuaternion& operator*=(const ShQuaternion& right);
      ShQuaternion& operator*=(const ShAttrib1f& right);

      ShQuaternion operator+(const ShQuaternion& q2);
      ShQuaternion operator-(const ShQuaternion& q2);
      ShQuaternion operator*(const ShQuaternion& q2);
      ShQuaternion operator*(const ShAttrib1f& c);

      ShQuaternion conjugate() const;
      ShQuaternion inverse() const;
      ShMatrix4x4f getMatrix() const;
    private:
      ShVector4f m_data;
  };

  extern ShQuaternion operator*(const ShAttrib1f& c, const ShQuaternion& q);
}



#endif
