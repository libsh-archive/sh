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
#ifndef SHEVAL_HPP 
#define SHEVAL_HPP

#include <vector>
#include <map>
#include "HashMap.hpp"
#include "Info.hpp"
#include "Statement.hpp"
#include "Variant.hpp"
#include "Operation.hpp"
#include "RefCount.hpp"
#include "Half.hpp"

namespace SH {

/** 
 * Note: Everything here is meant for internal use.
 *
 * Type specific definitions for the internal instructions
 * as they are used in host computations.
 *
 * Each EvalOp can evaluate a subset of operations
 * for specific input types (i.e. arithmetic ops, with T <- T x T)
 *
 * Most normal ops fall into the dest & src typse all = T category,
 * but some (like interval lo/hi accessors) have different dest 
 * type from src type.
 *
 * Initially, each evalOp registers all the ops it can handle
 * with the Eval (the (opcode X src types) must be unique for now).
 * Then Instructions passes Eval a bunch of stuff and Eval
 * will look up the appropriate evalOp for an operation.
 *
 * Extend later to handle n-ary operations if necessary (there are a few in the
 * shref, but none implemented as IR operations yet...)
 */


// forward declarations
//class Variant;
class EvalOp;
//typedef Pointer<Variant> Variant*;
//typedef Pointer<const Variant> const Variant*;

struct 
SH_DLLEXPORT
EvalOpInfo: public Info {
  Operation m_op;

  const EvalOp* m_evalOp;

  // type indices of the destination and sources
  // These are set to a valid value type except when the src/dest is not
  // used for m_op 
  ValueType m_dest;
  ValueType m_src[3]; 

  EvalOpInfo(Operation op, const EvalOp* evalOp, ValueType dest, 
      ValueType src0, ValueType src1, ValueType src2);

  Info* clone() const;

  std::string encode() const;
};

class 
SH_DLLEXPORT
Eval {
  public:
     // TODO: should really break this out into separate functions.  EvalOps can
     //       have a single function in the interface

    /** Decides which evaluation evalOp to use and calls it up.
     * If an op is unary, leave b, c = 0.
     * If an op is binary leave c = 0.
     * If an op has no dest, leave dest = 0.
     * @{*/
    void operator()(Operation op, Variant* dest,
        const Variant* a, const Variant* b, const Variant* c) const;

    void operator()(Operation op, const VariantPtr& dest,
        const VariantCPtr& a, const VariantCPtr& b, const VariantCPtr& c) const;
    // @}

    /** Registers a evalOp for a certain operation/source type index combination */ 
    void addOp(Operation op, const EvalOp* evalOp, ValueType dest, 
        ValueType src0, ValueType src1 = VALUETYPE_END, 
        ValueType src2 = VALUETYPE_END); 

    /** Returns a new op info representing the types that arguments
     * should be cast into for an operation.
     * Caches the result.
     *
     * @{
     */
    const EvalOpInfo* getEvalOpInfo(Operation op, ValueType dest,
        ValueType src0, ValueType src1 = VALUETYPE_END, 
        ValueType src2 = VALUETYPE_END) const;
    const EvalOpInfo* getEvalOpInfo(const Statement &stmt) const;
    //@}

    /** debugging function */ 
    std::string availableOps() const;

    static Eval* instance();

  private:
    Eval();


    typedef std::list<EvalOpInfo> OpInfoList;
    typedef OpInfoList OpInfoMap[OPERATION_END];
    OpInfoMap m_evalOpMap; 

    typedef PairPairHashMap<Operation, ValueType, ValueType, ValueType, const EvalOpInfo*> EvalOpCache;
    mutable EvalOpCache  m_evalOpCache;

    static Eval* m_instance;
};

class 
SH_DLLEXPORT
EvalOp {
  public:
    virtual ~EvalOp();

    // Wraps an operation where at least dest and a are non-null.
    virtual void operator()(Variant* dest, const Variant* a, 
        const Variant* b, const Variant* c) const = 0;
};

// The strategy for defining ops is to use separate classes to hold
// and register operations for different categories of types.
//
// Each of these "concrete" evaluation evalOps must implement
// 1) the virtual void EvalOp::operator()(...)
//    This is used to evaluate Variables
//
// 2) Functions 
//    template<Operation S>
//    static void unaryOp(DataVariant<T1> &dest, const DataVariant<T2> &src);
//
//    and similarly for binary, ternary ops 
//    (for most ops, only T1 = T2 is supported directly,
//    but for range arithmetic ops, some args may be in a range arithmetic
//    type and other args might not)
//
//    These can be specialized to implement specific ops,
//    and thus used directly for computation on data from
//    Generics without going through ANY layers of virtual 
//    function calls.

/** A RegularOp is one where all the arguments and the destination
 * are variants of type V (data type HOST).
 */
template<Operation S, typename T>
struct RegularOp: public EvalOp {
  void operator()(Variant* dest, const Variant* a, 
      const Variant* b, const Variant* c) const; 
};

// If functions could be partially specialized, then wouldn't need
// to wrap this in a class.

// TODO might want to break this down into a few more subclasses to handle
// 1) special float/double cmath functions (for C built in types)
// 2) other special functions (sgn, rcp, rsq, etc.) that C types don't have
// OR, use helper functions 
template<Operation S, typename T>
struct ConcreteRegularOp {
  typedef DataVariant<T, HOST> Variant;
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0);
};

/// evalOp that uses cmath functions and
// has little code snippets for functions like sgn, rcp, rsq, etc.
// that are not built-in cmath functions.
//
//TODO - not all the functions make sense on integer types...may
//want to not declare the ones that don't make sense...
template<Operation S, typename T>
struct ConcreteCTypeOp {
  typedef DataVariant<T, HOST> Variant;
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0);
};

template<Operation S, typename T>
struct RegularOpChooser {
  typedef ConcreteRegularOp<S, T> Op;
};

#define OPC_CTYPE_OP(T)\
template<Operation S>\
struct RegularOpChooser<S, T> {\
  typedef ConcreteCTypeOp<S, T> Op;\
};

OPC_CTYPE_OP(double);
OPC_CTYPE_OP(float);
OPC_CTYPE_OP(Half);
OPC_CTYPE_OP(int);

// initializes the regular Ops for a floating point type T
// with OpEvalOp<OP, T> objects
template<typename T>
void _initFloatOps();

// initializes the regular Ops for an integer type T
// (a subset of the floating point ones)
template<typename T>
void _initIntOps();

}

#include "EvalImpl.hpp"
#include "ConcreteRegularOpImpl.hpp"
#include "ConcreteCTypeOpImpl.hpp"

#endif
