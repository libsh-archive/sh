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
#ifndef SHCLOAK_HPP
#define SHCLOAK_HPP

#include <string>
#include <vector>
#include <ostream>
#include "ShSwizzle.hpp"
#include "ShRefCount.hpp"

namespace SH {


class ShCloak: public ShRefCountable {
  public:
    ShCloak();
    virtual ~ShCloak();


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
    virtual void set(ShPointer<const ShCloak> other) = 0;
    virtual void set(ShPointer<const ShCloak> other, int index) = 0;
    virtual void set(ShPointer<const ShCloak> other, bool neg, const ShSwizzle &writemask) = 0;

    // Creates a copy of the Cloak with the given swizzle and negation
    // swizzle.m_srcSize must equal size()
    virtual ShPointer<ShCloak> get() const = 0;
    virtual ShPointer<ShCloak> get(int index) const = 0;
    virtual ShPointer<ShCloak> get(bool neg, const ShSwizzle &swizzle) const = 0; 

    /// Encodes the data value as a string
    virtual std::string encode() const = 0;

    /// Decodes the data value from a string
    virtual int decode(std::string s) = 0;
};

typedef ShPointer<ShCloak> ShCloakPtr;
typedef ShPointer<const ShCloak> ShCloakCPtr;

template<typename T>
class ShDataCloak: public ShCloak {
  public:
    typedef ShPointer<ShDataCloak<T> > PtrType;
    typedef ShPointer<const ShDataCloak<T> > CPtrType;
    typedef T DataType;
    typedef std::vector<DataType> DataVec;
    typedef typename DataVec::iterator iterator;
    typedef typename DataVec::const_iterator const_iterator;

    // Constructs a data holding wrapper and sets the value to a default value
    // (typically zero)
    ShDataCloak(int N); 

    // Constructs a data holding wrapper and sets the value to a given value 
    ShDataCloak(int N, const T &value); 

    // Constructs a data holding wrapper using values from another wrapper
    // swizzled and negated as requested. 
    ShDataCloak(const ShDataCloak<T> &other);
    ShDataCloak(const ShDataCloak<T> &other, bool neg, const ShSwizzle &swizzle); 

    /// Decodes a string from encode() to initialize both the size and values
    // of the tuple elements
    ShDataCloak(std::string s);

    // gets the type index for this cloak typej 
    int typeIndex() const; 

    /// Gives the associated type name 
    std::string typeName() const; 

    //std::string typeName() const; 
    int size() const; 

    void negate();

    void set(ShCloakCPtr other);
    void set(ShCloakCPtr other, int index);
    void set(ShCloakCPtr other, bool neg, const ShSwizzle &writemask);

    ShCloakPtr get() const; 
    ShCloakPtr get(int index) const; 
    ShCloakPtr get(bool neg, const ShSwizzle &swizzle) const; 

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
ShPointer<ShDataCloak<T> > cloak_cast(ShCloakPtr c);

template<typename T>
ShPointer<const ShDataCloak<T> > cloak_cast(ShCloakCPtr c);

#define SH_CLOAK_POINTER_TYPE(name, typeString) \
  typedef ShPointer<ShDataCloak<name> > Sh ## typeString ## CloakPtr; \
  typedef ShPointer<const ShDataCloak<name> > Sh ## typeString ## CloakCPtr;

SH_CLOAK_POINTER_TYPE(double, d );
SH_CLOAK_POINTER_TYPE(float, f );

/*
SH_CLOAK_POINTER_TYPE(int, "i");
SH_CLOAK_POINTER_TYPE(short, "s");
SH_CLOAK_POINTER_TYPE(char, "b");
SH_CLOAK_POINTER_TYPE(unsigned int, "ui");
SH_CLOAK_POINTER_TYPE(unsigned short, "us");
SH_CLOAK_POINTER_TYPE(unsigned char, "ub");
*/

}

#include "ShCloakImpl.hpp"

#endif
