// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#include "ShDllExport.hpp"
#include "ShVariableType.hpp"
#include "ShDataType.hpp"
#include "ShVariant.hpp"

namespace SH {

// forward declaration
class ShVariant;

/// @file ShVariantCast.hpp
/// Declares a cast between one data type of a storage type and another.  

class 
SH_DLLEXPORT
ShVariantCast {
  public:
    virtual ~ShVariantCast() {}

    /** Casts data from src into dest.  src must have Src value type and SrcDT
     * data type.  dest similarly must match Dest and DestDT.
     * Also, must guarantee dest != src (it will work if they are the same
     * but what a waste...)
     * @{ */
    virtual void doCast(ShVariant* dest, const ShVariant* src) const = 0;
    // @}

    virtual void getCastTypes(ShValueType &dest, ShDataType &destDT, 
                              ShValueType &src, ShDataType &srcDT) const = 0;

    /** Returns whether the destination of this caster matches the given types
     **/
    virtual void getDestTypes(ShValueType &valueType, ShDataType &dataType) const = 0;
};

/** @brief Handles casting between S and D storage types.
 *
 * The actual data cast will have type ShHostType<SRC> to ShHostType<DEST>
 * and may have some extra conversion code (e.g. clamping) applied
 * in addition to the default C cast for those types. 
 */
template<typename Dest, ShDataType DestDT, 
  typename Src, ShDataType SrcDT>
struct ShDataVariantCast: public ShVariantCast {
  public:
    static const ShValueType DestValueType = ShStorageTypeInfo<Dest>::value_type;
    static const ShValueType SrcValueType = ShStorageTypeInfo<Src>::value_type;
    typedef typename ShDataTypeCppType<Dest, DestDT>::type D;
    typedef typename ShDataTypeCppType<Src, SrcDT>::type S;

    typedef ShDataVariant<Dest, DestDT> DestVariant;
    typedef const ShDataVariant<Src, SrcDT> SrcVariant;

    void doCast(ShVariant* dest, const ShVariant* src) const;

    void getCastTypes(ShValueType &dest, ShDataType &destDT, 
                      ShValueType &src, ShDataType &srcDT) const;

    void getDestTypes(ShValueType &valueType, ShDataType &dataType) const; 

    void doCast(D &dest, const S &src) const;

    static const ShDataVariantCast *instance();
  private:
    static ShDataVariantCast *m_instance;
    ShDataVariantCast() {}
};


}

#include "ShVariantCastImpl.hpp"

#endif
