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
#ifndef SHFRAMEBUFFER_HPP
#define SHFRAMEBUFFER_HPP

#include "ShRefCount.hpp"
#include "ShUberbuffer.hpp"

/** \file ShFramebuffer.hpp
 * This file is an implementation of memory objects.
 */

namespace SH {

/** A render target abstraction. 
 */
class ShFramebuffer: public ShRefCountable {
  public:
    /** \brief Constructor for ShFramebuffer
     * Constructs a ShFramebuffer of the given width, height, depth
     * and # of elements per memory location.
     */
    ShFramebuffer(int width, int height, int depth, int elements);
    //TODO Memory objects should eventually handle data of arbitrary dimension

    /** \brief Destructor for ShFramebuffer
     */
    virtual ~ShFramebuffer();

    int width() const; ///< Determine the width of the memory object 
    int height() const; ///< Determine the height of the memory object 
    int depth() const; ///< Determine the depth of the memory object 
    int elements() const; ///< Determine the elements (floats per element) of the memory object

    /** \brief 
     */
    void bind( ShUberbufferPtr ); 
    // TODO check that ub has same dimensions

    /** \brief Returns the attached uber buffer, or 0 if none is attached.
     */
    ShUberbufferPtr getUberbuffer();


  protected:
    int m_width, m_height, m_depth, m_elements;

    ShUberbufferPtr m_ub;
};

typedef ShRefCount<ShFramebuffer> ShFramebufferPtr;

}

#endif
