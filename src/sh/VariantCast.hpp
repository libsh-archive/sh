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
#ifndef SHVARIANTCAST_HPP
#define SHVARIANTCAST_HPP

#include "DllExport.hpp"
#include "VariableType.hpp"
#include "DataType.hpp"
#include "Variant.hpp"

namespace SH {

// forward declaration
class Variant;

/// @file VariantCast.hpp
/// Declares a cast between one data type of a storage type and another.  

class 
SH_DLLEXPORT
VariantCast {
  public:
    virtual ~VariantCast() {}

    /** Casts data from src into dest.  src must have Src value type and SrcDT
     * data type.  dest similarly must match Dest and DestDT.
     * Also, must guarantee dest != src (it will work if they are the same
     * but what a waste...)
     * @{ */
    virtual void doCast(Variant* dest, const Variant* src) const = 0;
    // @}

    virtual void getCastTypes(ValueType &dest, DataType &destDT, 
                              ValueType &src, DataType &srcDT) const = 0;

    /** Returns whether the destination of this caster matches the given types
     **/
    virtual void getDestTypes(ValueType &valueType, DataType &dataType) const = 0;
};

/** @brief Handles casting between S and D storage types.
 *
 * The actual data cast will have type HostType<SRC> to HostType<DEST>
 * and may have some extra conversion code (e.g. clamping) applied
 * in addition to the default C cast for those types. 
 */
template<typename Dest, DataType DestDT, 
  typename Src, DataType SrcDT>
struct DataVariantCast: public VariantCast {
  public:
    static const ValueType DestValueType = StorageTypeInfo<Dest>::value_type;
    static const ValueType SrcValueType = StorageTypeInfo<Src>::value_type;
    typedef typename DataTypeCppType<Dest, DestDT>::type D;
    typedef typename DataTypeCppType<Src, SrcDT>::type S;

    typedef DataVariant<Dest, DestDT> DestVariant;
    typedef const DataVariant<Src, SrcDT> SrcVariant;

    void doCast(Variant* dest, const Variant* src) const;

    void getCastTypes(ValueType &dest, DataType &destDT, 
                      ValueType &src, DataType &srcDT) const;

    void getDestTypes(ValueType &valueType, DataType &dataType) const; 

    void doCast(D &dest, const S &src) const;

    static const DataVariantCast *instance();
  private:
    static DataVariantCast *m_instance;
    DataVariantCast() {}
};


}

#include "VariantCastImpl.hpp"

#endif
