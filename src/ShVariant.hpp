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
#ifndef SHVARIANT_HPP
#define SHVARIANT_HPP

#include <string>
#include <vector>
#include <ostream>
#include "ShDllExport.hpp"
#include "ShSwizzle.hpp"
#include "ShRefCount.hpp"

namespace SH {


class 
SH_DLLEXPORT ShVariant: public ShRefCountable {
  public:
    ShVariant();
    virtual ~ShVariant();


    /// Gives the associated type identifier string mapped to a unique integer index 
    // (Note this is here only for convenience and the value is cached
    // somewhere better like in the ShVariableNode)
    virtual int typeIndex() const = 0; 

    /// Gives the associated type name 
    virtual std::string typeName() const = 0; 

    // Gives the number of elements stored in this data wrapper 
    virtual int size() const = 0;

    // The only two required operations on data values - negation and copy
    // assignment. 
    virtual void negate() = 0;

    // Copies components from other, applying swizzle as a write mask and
    // negating if neg = true
    //
    // @todo if other is not the same type as this, then it must be
    // automatically castable (implement explicit cast?)
    virtual void set(ShPointer<const ShVariant> other) = 0;
    virtual void set(ShPointer<const ShVariant> other, int index) = 0;
    virtual void set(ShPointer<const ShVariant> other, bool neg, const ShSwizzle &writemask) = 0;

    // Creates a copy of the Variant with the given swizzle and negation
    // swizzle.m_srcSize must equal size()
    virtual ShPointer<ShVariant> get() const = 0;
    virtual ShPointer<ShVariant> get(int index) const = 0;
    virtual ShPointer<ShVariant> get(bool neg, const ShSwizzle &swizzle) const = 0; 

    // Returns true iff other is the same size, type, and has the same values
    // (values must be EqualityComparable)
    virtual bool equals(ShPointer<const ShVariant> other) const = 0;

    // Returns true iff the value of every tuple element equals
    // the TRUE value for the storage type
    virtual bool isTrue() const = 0;

    /// Encodes the data value as a string
    virtual std::string encode() const = 0;

    /// Decodes the data value from a string
    virtual int decode(std::string s) = 0;
};

typedef ShPointer<ShVariant> ShVariantPtr;
typedef ShPointer<const ShVariant> ShVariantCPtr;

template<typename T>
class ShDataVariant: public ShVariant {
  public:
    typedef ShPointer<ShDataVariant<T> > PtrType;
    typedef ShPointer<const ShDataVariant<T> > CPtrType;
    typedef T DataType;
    typedef std::vector<DataType> DataVec;
    typedef typename DataVec::iterator iterator;
    typedef typename DataVec::const_iterator const_iterator;

    // Constructs a data holding wrapper and sets the value to a default value
    // (typically zero)
    ShDataVariant(int N); 

    // Constructs a data holding wrapper and sets the value to a given value 
    ShDataVariant(int N, const T &value); 

    // Constructs a data holding wrapper using values from another wrapper
    // swizzled and negated as requested. 
    ShDataVariant(const ShDataVariant<T> &other);
    ShDataVariant(const ShDataVariant<T> &other, bool neg, const ShSwizzle &swizzle); 

    /// Decodes a string from encode() to initialize both the size and values
    // of the tuple elements
    ShDataVariant(std::string s);

    // gets the type index for this variant typej 
    int typeIndex() const; 

    /// Gives the associated type name 
    std::string typeName() const; 

    //std::string typeName() const; 
    int size() const; 

    void negate();

    void set(ShVariantCPtr other);
    void set(ShVariantCPtr other, int index);
    void set(ShVariantCPtr other, bool neg, const ShSwizzle &writemask);

    ShVariantPtr get() const; 
    ShVariantPtr get(int index) const; 
    ShVariantPtr get(bool neg, const ShSwizzle &swizzle) const; 

    bool equals(ShVariantCPtr other) const; 

    bool isTrue() const;

    T& operator[](int index);
    const T& operator[](int index) const;

    /// returns the value stored in this wrapper 
    /// TODO should enforce that you can only change values in here, not
    // insert/delete
    // (maybe wrap/subclass vector)
    DataVec& data(); 

    /// returns the value stored in this wrapper 
    const DataVec& data() const; 

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    /// encodes the tuple values into a string
    /// for now, the encoding cannot contain the character '$' 
    /// and the istream >> T function cannot read $'s
    std::string encode() const;
    // TODO switch to fixed byte-length encodings so we don't
    // need these whacky special characters 
    //
    // For now, make all encodings human-readable so they will
    // still be useful if we switch to byte encodings

    /// deocdes values from a string (inverse of encode)
    // returns new size.  
    // TODO maybe if size changes there are problems?
    int decode(std::string value);
    
  protected:
    DataVec m_data; // actual data value
};

// utility functions
template<typename T>
ShPointer<ShDataVariant<T> > variant_cast(ShVariantPtr c);

template<typename T>
ShPointer<const ShDataVariant<T> > variant_cast(ShVariantCPtr c);

#define SH_VARIANT_POINTER_TYPE(name, typeString) \
  typedef ShPointer<ShDataVariant<name> > Sh ## typeString ## VariantPtr; \
  typedef ShPointer<const ShDataVariant<name> > Sh ## typeString ## VariantCPtr;

SH_VARIANT_POINTER_TYPE(double, d );
SH_VARIANT_POINTER_TYPE(float, f );

/*
SH_VARIANT_POINTER_TYPE(int, "i");
SH_VARIANT_POINTER_TYPE(short, "s");
SH_VARIANT_POINTER_TYPE(char, "b");
SH_VARIANT_POINTER_TYPE(unsigned int, "ui");
SH_VARIANT_POINTER_TYPE(unsigned short, "us");
SH_VARIANT_POINTER_TYPE(unsigned char, "ub");
*/

}

#include "ShVariantImpl.hpp"

#endif
