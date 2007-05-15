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
#ifndef SH_AAVARIABLE_HPP
#define SH_AAVARIABLE_HPP

#include <iosfwd>
#include "RefCount.hpp"
#include "Variable.hpp"
#include "Record.hpp"
#include "AaSyms.hpp"

namespace SH {

/** @file AaVariable.hpp
 * The AaVariableNode and AaVariable behave similar to  
 * VariableNode and Variable, except that they wrap several VariableNodes
 * used to store statically assigned error terms.  This is most useful
 * during code generation for platforms that don't support AA directly. 
 *
 * @see AaSymPlacer.hpp
 * @see AaOpHandler.hpp
 */

/** An affine variable node holds the center/error symbol temp tuples 
 * for a certain affine variable (which may correspond to an existing
 * VariableNode).
 *
 * The actual swizzle and error terms in use at a point in an Program are
 * represented by an AaVariable that references this Node.
 *
 * This encapsulation allows us to easily change how the error symbols are
 * vectorized across a tuple) by replacing this class.
 *
 * (Currently, the vectorization is one vector for the center values, and 
 * one vector for each tuple element's error symbols.  The reason for this is
 * that with more than one or two  error symbols, the cost of this approach
 * should be less on swizzling-capable vector platforms for common affine
 * operations
 *
 * @todo range There is a special case if all tuple elements have exactly 1 error symbol.
 *
 * @todo range In that case, 
 * there is one vector for center and one vector for the error symbols
 * of all tuple elements)
 *
 * Note: The Program to which the wrapped temporary Variables should belong
 * must be  
 */
struct AaVariableNode: public RefCountable {
  /** Constructs an AaVariableNode corresponding to an existing variable
   * node, given the static error term allocation 
   *
   * This wraps a set of Variables of the requested binding type 
   * for the center and error symbols 
   * (Note that syms.size() does not need to correspond to node.size() if
   * the syms represents a subset of the tuple elements and you KNOW that) */
  AaVariableNode(VariableNodeCPtr node, const AaSyms& syms, 
      BindingType kind=SH_TEMP, const char* suffix="");

  /** Constructs an AaVariableNode using the given information */
  AaVariableNode(const std::string& name, BindingType kind, 
      ValueType valueType, SemanticType type, const AaSyms& syms);

  /** Constructs a clone of an affine variable node, but only with the
   * indicated syms rather than the original syms.
   *
   * This wraps a set of temp variables (undefined content)*/ 
  AaVariableNode(const AaVariableNode& other, const AaSyms& syms); 


  /** Adds wrapped variables to a input/output varlist before pos */
  void varlistInsert(ProgramNode::VarList& varlist, ProgramNode::VarList::iterator pos);

  /** Returns number of tuple elements */
  int size() const { return m_center.size(); } 

  /** Returns value type of the regular tuples */
  ValueType valueType() const { return m_center.valueType(); }

  /** Returns value type of the regular tuples */
  SemanticType specialType() const { return m_center.node()->specialType(); }

  /** Returns the error sym assignment */ 
  const AaSyms& syms() const { return m_syms; } 

  /** Returns the error sym assignment for tuple element i */ 
  const AaIndexSet& operator()(int i)  { return m_syms[i]; }

  /** Returns the center */
  Variable center() { return m_center; }

  /** Returns the radius */
  Variable radius();

  /** Returns the error sym variable for tuple element i */
  Variable operator[](int i) { return (m_packed ? m_symvar[0](i) : m_symvar[i]); }

  /** Returns the name of the variable */
  const std::string& name() const { return m_name; }

  void name(const std::string& s) { m_name = s; }

  /** Makes a temporary with the same value type as center and the given suffix
   * attached to name() */
  Variable makeTemp(int size, const std::string& suffix); 

  /** Makes a record from this node's variables */
  Record record(); 


  protected:
    bool m_packed; // whether m_symvar contains separate variables per tuple element, or if 
                   // all are packed into m_symvar[0]
                   // Currently only supports packed if m_syms.isSingles() (i.e.
                   // one symbol per tuple element)

    /* After m_syms, m_center, m_name are set up, this sets up symvar */
    void init();

    const AaSyms m_syms; ///< Syms assigned to this variable

    Variable m_center; ///< variable for the center of the affine term

    typedef std::vector<Variable> VarVec;
    VarVec m_symvar; ///< one error sym variable for each tuple element

    std::string m_name; ///< name
};
typedef Pointer<AaVariableNode> AaVariableNodePtr; 
typedef Pointer<const AaVariableNode> AaVariableNodeCPtr; 

/** An AaVariable represents an affine arithmetic tuple at a certain use
 * point in the Program.  
 * It holds the center and error term tuples corresponding to an Variable in the 
 * original program, as well as information about the non-zero error symbols at
 * the current use-point.
 *
 * Implements a number of basic ops using the above information to make it
 * easier to write replacements for more complex operations.
 *
 * All the operators assign result to this (based on a "two-address" architecture) 
 * so we don't have to ever worry about making temporary AaVariables
 * with some unknown subset of error symbols
 * @todo range figure out if there is a faster way...for now make this work! 
 */
struct AaVariable {
  /** Constructs null variable with no AaVariableNode */ 
  AaVariable();

  /** Constructs an AaVariable with no swizzling/negation/used error symbol
   * constraints. 
   */
  AaVariable(AaVariableNodePtr node);
   
  /** Constructs an AaVariable with given swizzle, negation, and set of used
   * symbol info. 
   */
  AaVariable(AaVariableNodePtr node, const Swizzle& swizzle, bool neg, const AaSyms& use);


  // default copy constructor copies other without assignment
  // default operator= copies without inserting assignments
  
  /** Clone operator generates a clone using appropriate ASN statements on
   * the wrapped Variables.
   */
  AaVariable clone() const;

  /** Swizzles this node 
   * Note that these ops are VERY expensive as they copy the syms...
   *
   * @todo range don't copy syms...
   * @{*/
  AaVariable operator()(int) const;
  AaVariable operator()(int, int) const;
  AaVariable operator()(int, int, int) const;
  AaVariable operator()(int, int, int, int) const;
  AaVariable operator()(int swizSize, int indices[]) const;
  AaVariable operator()(const Swizzle& swizzle) const;
  AaVariable repeat(int n) const;
  // @}

  /** Returns the AaVariableNode */
  AaVariableNodePtr node() const { return m_node; }

  /** Returns the size of the variable  */
  int size() const { return m_swizzle.size(); }

  /** Returns value type of the regular tuples */
  ValueType valueType() const { return m_node->valueType(); } 

  /** Returns value type of the regular tuples */
  SemanticType specialType() const { return m_node->specialType(); } 

  /** Returns the name of the variable */
  const std::string& name() const { return m_node->name(); }
  void name(const std::string& s) { m_node->name(s); }

  /** Returns the center (swizzled appropriately) */
  Variable center() const; 

  /** Returns the error sym variable for the given tuple element (swizzled
   * based on syms in use presently compared to allocated syms) */ 
  Variable err(int i) const; 

  /** Returns the error sym variable for the given tuple element, swizzled
   * based on the given syms compared to allocated syms. */ 
  Variable err(int i, const AaIndexSet& use) const; 

  /** Returns the error sym variable for the given tuple element, swizzled
   * to give the requested symbol index from the allocated syms */ 
  Variable err(int i, int idx) const; 

  /** Returns the used error syms */
  const AaSyms& use() const;

  /** Returns the used error syms for a tuple element */
  const AaIndexSet& use(int i) const;

  /** Several useful operations on a variable start here 
   * For operations with an AaVariable argument, the symbols allocated
   * for this must be a superset of the argument's allocated symbols */ 

  /** Returns negation of the variable (points to the same AaVarNode) */
  AaVariable NEG() const;

  /** Assigns other to this.
   * @param used restricts assignment to the given error symbols
   * @param changeCenter if false, does not assign to center
   *
   * @todo range - do we need to zero out anything in this, or does
   * the static symbol assignment work out properly? */ 
  AaVariable& ASN(const AaVariable& other); 
  AaVariable& ASN(const AaVariable& other, const AaSyms& used, bool changeCenter=true); 
  AaVariable& ASN(const Variable& other, const AaSyms& used); 

  /** Zeros the error syms in use 
   * @todo range - should this zero all? */ 
  AaVariable& ZERO(); 

  /** Adds other to this 
   * @{ */
  AaVariable& ADD(const AaVariable& other); 
  AaVariable& ADD(const AaVariable& other, const AaSyms& used, bool changeCenter=true); 
  AaVariable& ADD(const Variable& other); 
  // @}
  
  /** Multiples this with a scalar */
  AaVariable& MUL(const Variable& other); 

  /** Multiples scalar with other and adds the result to this) */
  AaVariable& MAD(const AaVariable& other, const Variable& scalar); 
  AaVariable& MAD(const AaVariable& other, const Variable& scalar, const AaSyms& used, bool changeCenter=true); 

  /** Sets tuple element by element equal to other based on the condition 
   * where condvar > 0, use the other tuple element 
   * where condvar <=0, the original value remains 
   **/ 
  AaVariable& COND(const Variable& condvar, const Variable& other);
  AaVariable& COND(const Variable& condvar, const AaVariable& other);
  AaVariable& COND(const Variable& condvar, const AaVariable& other, const AaSyms& used, bool changeCenter=true); 

  /** Sets error symbols indicated by used with the values in other.
   * Like ASN, this takes only the first sym from each tuple element in used */
  AaVariable& setErr(const Variable& other, const AaSyms& used); 

  /** Adds to error symbols indicated by used with the values in other.
   * Like ASN, this takes only the first sym from each tuple element in used 
   * Takes absolute value of existing error first, so the total magnitude works
   * out */
  AaVariable& addErr(const Variable& other, const AaSyms& used); 

  /** Methods that return certain computed quantities from the affine 
   * arithmetic terms 
   * @{ */
  Variable radius() const;
  Variable width() const;
  Variable lo() const; 
  Variable hi() const;
  void lohi(Variable& lo, Variable& hi) const;
  /* @} */

  /** Returns a temporary of the same size() as this, using a regular base storage
   * type of this affine arithmetic storage type */ 
  Variable temp(std::string name) const; 

  /** Returns a Record structure representing the wrapped variables in this */
  Record record();

  friend std::ostream& operator<<(std::ostream& out, const AaVariable &var);

  private:
    /** Returns the swizzle that should be applied to the allocated error sym
     * variable in the node for a given tuple element i, based on 
     * requested sym(s) indices (or the ones stored in this). 
     * @{ */ 
    Swizzle getSymSwiz(int i, const AaIndexSet& used) const; 
    Swizzle getSymSwiz(int i, int idx) const; 
    Swizzle getSymSwiz(int i) const; 
    /* @} */

    Variable makeTemp(const std::string& suffix) const; 

    AaVariableNodePtr m_node; ///< node wrapping the Variables for center and error syms

    Swizzle m_swizzle; ///< Swizzle for this variable at its use point
    bool m_neg; ///< Negation for this variable at its use point

    /** Active symbols for the variable at its use point (This helps
     * reduce unnecessary ops on error symbols early on.  This problem will
     * disappear if we switch to SSA-form, where each SSA variable will only
     * ever use a single set of error symbols, but in that case we would need to
     * add in merges on error symbol sequences as a tradeoff). */
    AaSyms m_used;
};

}

#endif

