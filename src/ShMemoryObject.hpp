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
#ifndef SHMEMORYOBJECT_HPP
#define SHMEMORYOBJECT_HPP

#include "ShRefCount.hpp"

/** \file ShMemoryObject.hpp
 * This file is an implementation of memory objects.
 */

namespace SH {

class ShMemoryObject;
typedef ShRefCount<ShMemoryObject> ShMemoryObjectPtr;

/** A memory object class that can be used to manipulate buffers
 * and bind as images to texture maps.
 */
class ShMemoryObject: public ShRefCountable {
  public:
    /** \brief Constructor for ShMemoryObject
     * Constructs a ShMemoryObject of the given width, height, depth
     * and # of elements per memory location.
     */
    ShMemoryObject(int width, int height, int depth, int elements);
    //TODO Memory objects should eventually handle data of arbitrary dimension

    /** \brief Destructor for ShMemoryObject
     */
    virtual ~ShMemoryObject();

    int width() const; ///< Determine the width of the memory object 
    int height() const; ///< Determine the height of the memory object 
    int depth() const; ///< Determine the depth of the memory object 
    int elements() const; ///< Determine the elements (floats per element) of the memory object


    /** \brief Copies contents of memory object b into this 
     * 
     * The default version of this checks for compatibility of dimensions
     * and uses setData and data to copy memory through the host.  
     * Subclasses may impelement optimized versions.
     *
     * returns true if b has data and copy succeeds
     */
    virtual bool copy(ShMemoryObjectPtr b);

    /** \brief Writes data into this memory object
     * Writes a chunk of memory with size width * height * depth * elements
     * into this memory object.
     */
    virtual void setData(const float* data) = 0; 

    // TODO may want format specifier in setData so we can perform format conversion
    // if necessary

    /** \brief Reads the data from this memory object
     * Returns a float array of the data allocated with new, or 0 if memory object
     * is not initialized.
     */
    virtual float* data() const = 0;

    // TODO data() is messy? 
    // The alternative is to allocate a fixed buffer in the class
    // specifically used for returning data() calls.
    // But that requires allocating memory in thclass

  protected:
    int m_width, m_height, m_depth, m_elements;

    /** \brief Checks if b has compatible dimensions.
     * Returns true only if width, height, depth, and elements match
     */
    bool compatibleWith(ShMemoryObjectPtr b);
};

/** A memory object that resides in a floating point array on the 
 * host system
 */
class ShDataMemoryObject: public ShMemoryObject {
  public:
     /** constructs and ShMemory object with empty data
     * of the given width, height and # of elements
     * Use 1 for dimensions that are not needed
     */
    ShDataMemoryObject(int width, int height, int depth, int elements);
    
    /** Writes a slice of data into this memory object
     *
     * data must have size width * height * depth * elements 
     * and slice must = 0 to write over all the data. 
     *
     * Otherwise, use a slice in { 0, 1, ..., depth - 1 }
     * and data of size width * height * elements to set a specific slice.
     */
    void setData(const float *data, int slice);

    /** \brief Writes data into this memory object
     * Writes a chunk of memory with size width * height * depth * elements
     * into this memory object.
     */
    void setData(const float *data);

    /** \brief Writes a partial set of data into this memory object
     * Writes a chunk of memory of length count*elements to this memory object.
     */
     void setPartialData(const float* data, int count);
  
    /** \brief Reads the data from this memory object
     * Returns a float array of the data allocated with new, or 0 if memory object
     * is not initialized.
     */
    float* data() const;

  private:
    float *m_data;
};

typedef ShRefCount<ShDataMemoryObject> ShDataMemoryObjectPtr;

/** A custom memory object 
 * 
 * This is a temporary hack.  When the super buffer specs stabilize, 
 * each backend should present an interface for manipulating buffers.
 * 
 * But for now, the ARB backend only needs to call attach 
 * after binding a texture if that texture is bound to this memory object.
 * 
 * And the rest is implemented externally as a light wrapper around the 
 * über buffer methods.  
 */
class ShExternalMemoryObject: public ShMemoryObject {
  public:
    ShExternalMemoryObject(int width, int height, int depth, int elements);
    virtual ~ShExternalMemoryObject();

    /** Attaches memory object to "currently active texture"
     *
     * This a temporary hack for über buffer support
     * If this memory object is attached to a texture, the backend
     * must call attach() while that texture is "active". 
     *
     * Then this memory object (über buffer) will be attached to the 
     * active texture.
     */
    virtual void attach() = 0; 
};

typedef ShRefCount<ShExternalMemoryObject> ShExternalMemoryObjectPtr;

}

#endif
