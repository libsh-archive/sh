/// Sh: A GPU metaprogramming language.
//
/// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
/// Project administrator: Michael D. McCool
/// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
///          Michael D. McCool
/// 
/// This software is provided 'as-is', without any express or implied
/// warranty. In no event will the authors be held liable for any damages
/// arising from the use of this software.
/// 
/// Permission is granted to anyone to use this software for any purpose,
/// including commercial applications, and to alter it and redistribute it
/// freely, subject to the following restrictions:
/// 
/// 1. The origin of this software must not be misrepresented; you must
/// not claim that you wrote the original software. If you use this
/// software in a product, an acknowledgment in the product documentation
/// would be appreciated but is not required.
/// 
/// 2. Altered source versions must be plainly marked as such, and must
/// not be misrepresented as being the original software.
/// 
/// 3. This notice may not be removed or altered from any source
/// distribution.
// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHVARIANT_HPP
#define SHVARIANT_HPP

#include <string>
#include <vector>
#include <ostream>
#include "DllExport.hpp"
#include "Pool.hpp"
#include "Swizzle.hpp"
#include "VariableType.hpp"
#include "DataType.hpp"
#include "RefCount.hpp"

namespace SH {


/** An Variant is a wrapper around a fixed-size array of data
 * of a data type.  It is used internally for
 * holding tuple data for VariableNodes, and occasionally as temporaries
 * for larger data arrays.
 *
 * There are DataVariant<V> implementations that hold a T array. 
 *
 * @see DataVariant
 */
class 
SH_DLLEXPORT 
Variant: public RefCountable {
  public:
    Variant();
    virtual ~Variant();


    //// Gives the associated type identifier string mapped to a unique integer index 
    /// (Note this is here only for convenience and the value is cached
    /// somewhere better like in the VariableNode)
    virtual ValueType valueType() const = 0; 

    /// Gives the data type held by this variant 
    virtual DataType dataType() const = 0; 

    /// Returns whether the value/data type in this variant match the given ones.
    virtual bool typeMatches(ValueType valueType, DataType dataType) const = 0;

    //// Gives the associated type name 
    virtual const char* typeName() const = 0; 

    /// Gives the number of elements stored in this data array 
    virtual int size() const = 0;

    /// Gives the number of bytes per element stored in this data array 
    virtual int datasize() const = 0;

    /// Returns true if the array held in the Variant was allocated
    /// by this.
    virtual bool managed() const = 0;

    /// The only two required operations on data values - negation and copy
    /// assignment. 
    virtual void negate() = 0;


    /** Sets the values of this from other.  
     * size() must equal other.size() 
     * @{ */
    virtual void set(const Pointer<const Variant>& other) = 0;
    virtual void set(const Variant* other) = 0;
    // @}

    /** Sets the value of the indexed element in this from the first element of other 
     * @{ */
    virtual void set(const Pointer<const Variant>& other, int index) = 0;
    virtual void set(const Variant* other, int index) = 0;
    // @}

    /** Sets the value of this from other using the given negation and writemask
     * on this.  other.size() must equal writemask.size()
     * @{ */
    virtual void set(const Pointer<const Variant>& other, bool neg, const Swizzle &writemask) = 0;
    virtual void set(const Variant* other, bool neg, const Swizzle &writemask) = 0;
    // @}

    /// Creates a copy of this Variant.
    virtual Pointer<Variant> get() const = 0;

    /// Creates single element Variant with the indexed element from this.  
    virtual Pointer<Variant> get(int index) const = 0;

    /// Creates a copy of the Variant with the given swizzle and negation
    /// for a given number of elements (defaults to 1)
    /// swizzle.m_srcSize must equal size()
    virtual Pointer<Variant> get(bool neg, const Swizzle &swizzle, int count=1) const = 0; 

    /// Returns true iff other is the same size, type, and has the same values
    // This uses dataTypeEquals
    //
    // @see DataType.hpp
    //@{
    virtual bool equals(const Pointer<const Variant>& other) const = 0;
    virtual bool equals(const Variant* other) const = 0;
    //@}

    /// Returns whether every tuple element is positive 
    // @see DataType.hpp
    virtual bool isTrue() const = 0;

    /// Returns a pointer to the beginning of the array 
    //@{
    virtual void* array() = 0;
    virtual const void* array() const = 0;
    // @}


    /// Encodes the data value as a string
    virtual std::string encode() const = 0;
    virtual std::string encode(int index, int repeats=1) const = 0;
    virtual std::string encode(bool neg, const Swizzle &swizzle) const = 0;

    /// C++ array declaration compatible encoding.
    /// Generates a string that can be used as an array initializer 
    /// i.e. T foo[size] = { encodeArray() };
    /// @todo type may want to put this somewhere else, it doesn't really belong
    virtual std::string encodeArray() const = 0;
};

typedef Pointer<Variant> VariantPtr;
typedef Pointer<const Variant> VariantCPtr;

/* A fixed-size array of a specific data type that can act as an Variant 
 *
 * This is different from Memory objects which hold arbitrary typed
 * data in byte arrays (that eventually may include some 
 * unordered collection of several types)
 *
 * @see Memory 
 **/ 
template<typename T, DataType DT=HOST>
class DataVariant: public Variant {
  public:
    static const ValueType value_type = StorageTypeInfo<T>::value_type;
    static const DataType data_type = DT;
    typedef DataVariant<T, DT>* PtrType;
    typedef const DataVariant<T, DT>* CPtrType;
    typedef typename DataTypeCppType<T, DT>::type CppDataType;
    typedef CppDataType* iterator;
    typedef const CppDataType* const_iterator;
    
    /// Constructs an empty array
    DataVariant();

    /// Constructs a data array and sets the value to a default value
    /// (typically zero)
    DataVariant(int N); 

    /// Constructs a data array and sets the value to a given value 
    DataVariant(int N, const CppDataType &value); 

    /// Constructs a data array that reads its size and values from
    /// a string encoding (must be from the encode() method of a DataVariant
    /// of the same type)
    DataVariant(std::string encodedValue);

    /// Constructs a data array from an existing array of type T
    /// of the given size.  This uses the given array internally
    /// iff managed = false, otherwise it allocates a new array 
    /// and makes a copy.
    DataVariant(int N, void *data, bool managed = true);

    /// Constructs a data array using values from another array
    /// swizzled and negated as requested. 
    DataVariant(const DataVariant<T, DT> &other);
    DataVariant(const DataVariant<T, DT> &other, bool neg, const Swizzle &swizzle, int count=1); 

    virtual ~DataVariant();

    /// Assigns another data array to this
    DataVariant& operator=(const DataVariant<T, DT>& other);

    ValueType valueType() const; 
    DataType dataType() const; 
    bool typeMatches(ValueType valueType, DataType dataType) const; 

    //// Gives the associated type name 
    const char* typeName() const; 

    //std::string typeName() const; 
    
    int size() const; 
    int datasize() const; 

    bool managed() const;

    void negate();

    void set(const VariantCPtr& other);
    void set(const Variant* other);
    void set(const VariantCPtr& other, int index);
    void set(const Variant* other, int index);
    void set(const VariantCPtr& other, bool neg, const Swizzle &writemask);
    void set(const Variant* other, bool neg, const Swizzle &writemask);

    VariantPtr get() const; 
    VariantPtr get(int index) const; 
    VariantPtr get(bool neg, const Swizzle &swizzle, int count=1) const; 

    bool equals(const VariantCPtr& other) const; 
    bool equals(const Variant* other) const; 

    bool isTrue() const;

    void* array(); 
    const void* array() const; 

    CppDataType& operator[](int index);
    const CppDataType& operator[](int index) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    //// Encodes the tuple values into a string
    //// For now, the encoding cannot contain the character '$' 
    //// and the istream >> T function cannot read $'s
    std::string encode() const;
    std::string encode(int index, int repeats=1) const; 
    std::string encode(bool neg, const Swizzle &swizzle) const; 

    /// TODO switch to fixed byte-length encodings so we don't
    /// need these whacky special characters 
    //
    /// For now, make all encodings human-readable so they will
    /// still be useful if we switch to byte encodings
    
    std::string encodeArray() const;

#ifdef USE_MEMORY_POOL
    // Memory pool stuff.
    void* operator new(std::size_t size);
    void operator delete(void* d, std::size_t size);
#endif
    
  protected:
    CppDataType *m_begin; ///< Start of the data array
    CppDataType *m_end; ///< One after the end of the array

    bool m_managed; ///< true iff we are responsible for array alloc/delete 

    /// allocates an array of size N and sets m_begin, m_end
    void alloc(int N);

#ifdef USE_MEMORY_POOL
    static Pool* m_pool;
#endif
};

/// utility functions

// Cast to the specified data variant using dynamic_cast
//
// Refcounted and non-refcounted versions
//@{
template<typename T, DataType DT>
Pointer<DataVariant<T, DT> > variant_cast(const VariantPtr& c);

template<typename T, DataType DT>
Pointer<const DataVariant<T, DT> > variant_cast(const VariantCPtr& c);

template<typename T, DataType DT>
DataVariant<T, DT>* variant_cast(Variant* c);

template<typename T, DataType DT>
const DataVariant<T, DT>* variant_cast(const Variant* c);
// @}

// Make a copy of c cast to the requested type 
//@{
template<typename T, DataType DT>
Pointer<DataVariant<T, DT> > variant_convert(const VariantCPtr& c);
// @}


}

#include "VariantImpl.hpp"

#endif
