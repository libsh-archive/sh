#ifndef SHEVAL_HPP 
#define SHEVAL_HPP

#include <vector>
#include <map>
#include "ShOperation.hpp"
#include "ShRefCount.hpp"
#include "ShInterval.hpp"

namespace SH {

/** 
 * Note: Everything here is meant for internal use.
 *
 * Type specific definitions for the internal instructions
 * as they are used in host computations.
 *
 * Each ShEvalOp can evaluate a subset of operations
 * for specific input types (i.e. arithmetic ops, with T <- T x T)
 *
 * Most normal ops fall into the dest & src typse all = T category,
 * but some (like interval lo/hi accessors) have different dest 
 * type from src type.
 *
 * Initially, each evalOp registers all the ops it can handle
 * with the ShEval (the (opcode X src types) must be unique for now).
 * Then ShInstructions passes ShEval a bunch of stuff and ShEval
 * will look up the appropriate evalOp for an operation.
 *
 * Extend later to handle n-ary operations if necessary (there are a few in the
 * shref, but none implemented as IR operations yet...)
 */

#include "ShStatement.hpp"

// forward declarations
class ShVariant;
class ShEvalOp;
typedef ShPointer<ShVariant> ShVariantPtr;
typedef ShPointer<const ShVariant> ShVariantCPtr;

struct ShEvalOpInfo: public ShStatementInfo {
  ShOperation m_op;

  // type indices of the destination and sources
  // These are non-zer except when the operation does not use
  // a certain dest/src
  int m_dest;
  int m_src[3]; 

  ShEvalOpInfo(ShOperation op, int dest, 
      int src0, int src1, int src2);
  ShStatementInfo* clone() const;

  bool operator<(const ShEvalOpInfo &other) const;
  std::string encode() const;
};

class ShEval {
  public:
    /** Decides which evaluation evalOp to use and calls it up.
     * If an op is unary, leave b, c = 0.
     * If an op is binary leave c = 0.
     * If an op has no dest, leave dest = 0.
     *
     * TODO (should really break this out into separate functions.  EvalOps can
     * have a single function in th einterface)
     */
    void operator()(ShOperation op, ShVariantPtr dest,
        ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const;

    /** Registers a evalOp for a certain operation/source type index combination */ 
    void addOp(ShOperation op, ShPointer<const ShEvalOp> evalOp, int dest, 
        int src0, int src1 = 0, int src2 = 0); 

    /** Returns a new op info representing the types that arguments
     * should be cast into for an operation.
     */
    const ShEvalOpInfo* getEvalOpInfo(ShOperation op, int dest,
        int src0, int src1 = 0, int src2 = 0) const;

    /** debugging function */ 
    std::string availableOps() const;

    static ShEval* instance();

  private:
    ShEval();


    typedef std::map<ShEvalOpInfo, ShPointer<const ShEvalOp> > InfoOpMap;
    InfoOpMap m_evalOpMap; 

    static ShEval* m_instance;
};

class ShEvalOp: public ShRefCountable {
  public:
    virtual ~ShEvalOp();

    // Wraps an operation where at least dest and a are non-null.
    virtual void operator()(ShVariantPtr dest, ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const = 0;
};

typedef ShPointer<ShEvalOp> ShEvalOpPtr;
typedef ShPointer<const ShEvalOp> ShEvalOpCPtr;

// The strategy for defining ops is to use separate classes to hold
// and register operations for different categories of types.
//
// Each of these "concrete" evaluation evalOps must implement
// 1) the virtual void ShEvalOp::operator()(...)
//    This is used to evaluate ShVariables
//
// 2) Functions 
//    template<ShOperation S>
//    static void unaryOp(std::vector<T1> &dest, const std::vector<T2> &src);
//
//    and similarly for binary, ternary ops 
//    (for most ops, only T1 = T2 is supported directly,
//    but for range arithmetic ops, some args may be in a range arithmetic
//    type and other args might not)
//
//    These can be specialized to implement specific ops,
//    and thus used directly for computation on data from
//    ShGenerics without going through ANY layers of virtual 
//    function calls.

// TODO fix these comments...
// Defines the set of operations available on a certain types 
// FloatEvalOp has the usual operations on floating point 
// values and float-like types (e.g. double, float, half, fixed point types)
//
// These include most basic ops (except for range arithmetic specific ops) 
template<ShOperation S, typename T>
struct ShRegularOp: public ShEvalOp {
  void operator()(ShVariantPtr dest, ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const; 
};

// If functions could be partially specialized, then wouldn't need
// to wrap this in a class.

// TODO might want to break this down into a few more subclasses to handle
// 1) special float/double cmath functions (for C built in types)
// 2) other special functions (sgn, rcp, rsq, etc.) that C types don't have
// OR, use helper functions 
template<ShOperation S, typename T>
struct ShConcreteRegularOp {
  static void doop(std::vector<T> *dest, 
      const std::vector<T> *a, 
      const std::vector<T> *b = 0, 
      const std::vector<T> *c = 0);
};

/// evalOp that uses cmath functions and
// has little code snippets for functions like sgn, rcp, rsq, etc.
// that are not built-in cmath functions.
//
//TODO - not all the functions make sense on integer types...may
//want to not declare the ones that don't make sense...
template<ShOperation S, typename T>
struct ShConcreteCTypeOp {
  static void doop(std::vector<T> *dest, 
      const std::vector<T> *a, 
      const std::vector<T> *b = 0, 
      const std::vector<T> *c = 0);
};

template<ShOperation S, typename T>
struct ShRegularOpChooser {
  typedef ShConcreteRegularOp<S, T> Op;
};

#define SHOPC_CTYPE_OP(T)\
template<ShOperation S>\
struct ShRegularOpChooser<S, T> {\
  typedef ShConcreteCTypeOp<S, T> Op;\
};

SHOPC_CTYPE_OP(double);
SHOPC_CTYPE_OP(float);
SHOPC_CTYPE_OP(int);
SHOPC_CTYPE_OP(short);
SHOPC_CTYPE_OP(char);
SHOPC_CTYPE_OP(unsigned int);
SHOPC_CTYPE_OP(unsigned short);
SHOPC_CTYPE_OP(unsigned char);


template<ShOperation S, typename T1, typename T2>
struct ShIntervalOp: public ShEvalOp {
  void operator()(ShVariantPtr dest, ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const; 
};

template<ShOperation S, typename T1, typename T2>
struct ShConcreteIntervalOp{
  static void doop(std::vector<T1> &dest, 
      const std::vector<T2> &a);
      
};


// initializes the regular Ops for a floating point type T
// with ShOpEvalOp<OP, T> objects
template<typename T>
void _shInitFloatOps();

// initializes the regular Ops for an integer type T
// (a subset of the floating point ones)
template<typename T>
void _shInitIntOps();

template<typename T>
void _shInitIntervalOps();


}

#include "ShEvalImpl.hpp"
#include "ShConcreteRegularOpImpl.hpp"
#include "ShConcreteCTypeOpImpl.hpp"
#include "ShConcreteIntervalOpImpl.hpp"
//#include "ShIntervalEvalImpl.hpp"

#endif
