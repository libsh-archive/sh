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
#ifndef SHAFFINE_HPP
#define SHAFFINE_HPP

#include <iostream>
#include <vector>
#include "ShInterval.hpp"

namespace SH {

/*** Sh class for affine arithmetic, without rounding
 *
 * Derived from NuB.hh and NuB.cc from the MetaMedia project.
 *
 * Currently does not handle rounding errors yet 
 */
template<typename T>
struct ShAffineSymbol {
  //typedef unsigned long long index_type;
  typedef long index_type;
  typedef T data_type;

  index_type m_index;
  data_type m_err;

  /** Generate an error symbol with unknown index. */
  ShAffineSymbol();

  /** Generate an error symbol with the given index.
   * (i < m_max_index)
   */
  ShAffineSymbol(index_type index, data_type err);

  /** Generate an error symbol with a new index = m_max_index
   * and increment m_max_index */
  ShAffineSymbol(data_type err);

  template<typename T2>
  ShAffineSymbol(const ShAffineSymbol<T2> &other);

  /** Accessor methods 
   * @{ */
  data_type& err();
  data_type err() const;

  index_type& index() ;
  index_type index() const;
  //@}

  /** Absolute error */
  data_type abs_err() const;

  ShAffineSymbol& operator=(const ShAffineSymbol &other);

  template<typename T2>
  ShAffineSymbol& operator=(const ShAffineSymbol<T2> &other);

  /** Returns whether this index is less than the other index */
  bool operator<(const ShAffineSymbol& other) const;

  /** Returns an error symbol with same index but -m_err  */
  ShAffineSymbol operator-() const;

  /** Returns an error symbol with error = m_err + other.m_err
   * and index = m_index (should check indices, but for the sake of mind-blowing
   * speed, meh) */ 
  ShAffineSymbol operator+(const ShAffineSymbol& other) const;

  /** Returns an error symbol with error = m_err - other.m_err
   * and index = m_index (should check indices, but for the sake of mind-blowing
   * speed, meh) */ 
  ShAffineSymbol operator-(const ShAffineSymbol& other) const;

  /** Returns an error symbol with error = m_err * scale */
  ShAffineSymbol operator*(const T& scale) const;

  /** Returns an error symbol with error = m_err * scale */
  ShAffineSymbol& operator*=(const T& scale);

  /** Returns whether both index and error equals other 
   * @todo range - may want to put an EPS on this? */ 
  bool operator==(const ShAffineSymbol &other) const;

  /** Returns whether either index or error don't equal other 
   * @todo range - may want to put an EPS on this? */ 
  bool operator!=(const ShAffineSymbol &other) const;

  /** Output **/
  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const ShAffineSymbol<TT>& sym);

  /** Input **/
  template<typename TT>
  friend std::istream& operator>>(std::istream& out, ShAffineSymbol<TT>& sym);


  private:
    static index_type m_max_index;
};

template<typename T>
struct ShAffine {
  typedef T data_type; 
  typedef ShAffineSymbol<T> error_type; 
  typedef typename error_type::index_type index_type; 
  typedef ShInterval<T> interval_type;

  typedef std::vector<error_type> SymbolVec;
  typedef typename SymbolVec::iterator iterator;
  typedef typename SymbolVec::const_iterator const_iterator;

  T m_center; //< center of the interval
  SymbolVec m_syms;

  /** Constructs an affine interval with undefined value */
  ShAffine();

  /** Constructs an affine interval = value */
  ShAffine(const T& value);

  /** Constructs an affine interval with a single error symbol */
  ShAffine(const T& lo, const T& hi);

  ShAffine(const ShAffine& other);

  template<typename T2>
  ShAffine(const ShAffine<T2>& other);

  /** Construct affine interval out of constant interval */
  template<typename T2>
  ShAffine(const ShInterval<T2>& other);

  ~ShAffine();

  /** Construct an constant interval out of this 
   * @{ */
  operator interval_type() const;
  interval_type to_interval() const;
  // @}

  /** Accessors for the error symbols 
   * @{*/
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  error_type& operator[](int i);
  const error_type& operator[](int i) const;
  int size() const;
  // @}

  /** Useful helpers **/ 
  /** Lower bound */
  T lo() const;

  /** Upper bound */
  T hi() const;

  /** Upper bound - lower bound */ 
  const T width() const;

  /** Returns (m_hi + m_lo) / 2 **/
  const T center() const;

  /** Returns width() / 2 **/
  const T radius() const;

  /** Arithmetic operators **/
  ShAffine& operator=(const T& value);
  ShAffine& operator=(ShInterval<T> value);
  ShAffine& operator=(const ShAffine<T>& other);
  ShAffine& operator+=(const T& value);
  ShAffine& operator+=(const ShAffine<T>& other);
  ShAffine& operator-=(const T& value);
  ShAffine& operator-=(const ShAffine<T>& other);
  ShAffine& operator*=(const T& value);
  ShAffine& operator*=(const ShAffine<T>& other);
  ShAffine& operator/=(const T& value);
  ShAffine& operator/=(const ShAffine<T>& other);

  /** Float modulus - result is always positive 
   *@{*/
  ShAffine& operator%=(const T& value);
  ShAffine& operator%=(const ShAffine<T>& other);
  // @}

  /** Scalar arithmetic operators **/

  /** Negation **/
  ShAffine operator-() const;


private:
    //< add new error term.  the index will be > all other indices in the vec
    void add(const data_type &err); 
    void add(const error_type& err); 

    //< add a range of error terms.  the indices must be > all other indices 
    void add(const_iterator first, const_iterator last); 

    void clear(); ///< clear error symbols
    void resize(int size); ///< resize error symbols to new size
    void reserve(int size);

    // Given a functor F that implements three static functions
    // f(T x)
    //   a convex, continuous function within the bounds 
    //   (so you should check for weird cases before)
    // dfinv(T x) 
    //   = inverse of df/dx
    // fdfinv(T x) - 
    //   = f(dfinv(x)) 
    //   this is just for speed on special cases like
    //   f(x) = e^x, where dfinv(x) = log(x), so fdfinv(x) = x 
    //
    // This calculates an affine approximation for the function applied ot this affine interval 
    //
    // @param bounds  You can pass in the this.to_interval() if you already used
    // it for invalid bounds checking.
    //
    // @{
    template<typename F>
    ShAffine convex_approx() const;
    template<typename F>
    ShAffine convex_approx(const interval_type &bounds) const;
    // @}
    
    /** Returns an affine interval equal to 
     * alpha * (*this) + beta
     * with an extra error symbol delta
     */
    ShAffine affine_approx(const T& alpha, const T& beta, const T& delta) const; 
 
public:
  /** Output operator **/
  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const ShAffine<TT>& value);

  /** Input operator (format matches output) **/
  template<typename TT>
  friend std::istream& operator>>(std::istream& out, ShAffine<TT>& value);

  /** Arithmetic operators **/
  // TODO fill in the remaining affine interval with scalar ops
  template<typename TT>
  friend ShAffine<TT> operator+(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator-(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator*(const ShAffine<TT>& a, const ShAffine<TT>& b);
  template<typename TT>
  friend ShAffine<TT> operator*(const TT& a, const ShAffine<TT>& b);
  template<typename TT>
  friend ShAffine<TT> operator*(const ShAffine<TT>& a, const TT& b);

  template<typename TT>
  friend ShAffine<TT> operator/(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator/(const ShAffine<TT>& a, const TT& b);

  template<typename TT>
  friend ShAffine<TT> operator%(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> cbrt(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> exp(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> exp2(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> exp10(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> log(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> log2(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> log10(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> frac(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> fmod(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> pow(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> rcp(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> rsq(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> sgn(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> sqrt(const ShAffine<TT>& a);

  /** Trig Operators */
  template<typename TT>
  friend ShAffine<TT> acos(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> asin(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> atan(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> atan2(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> cos(const ShAffine<TT>& a); 
  template<typename TT>
  friend ShAffine<TT> cosh(const ShAffine<TT>& a); 
  template<typename TT>
  friend ShAffine<TT> sin(const ShAffine<TT>& a);
  template<typename TT>
  friend ShAffine<TT> sinh(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> tan(const ShAffine<TT>& a);

  /** Comparison Operators **/
  // @todo should think about how to represent tri-state logic values.
  // For now output is affine interval (follows the t x t -> t convention of
  // types for the standard operators)
  template<typename TT>
  friend ShAffine<TT> operator<(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator<=(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator>(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator>(const ShAffine<TT>& a, const TT& b); 

  template<typename TT>
  friend ShAffine<TT> operator>=(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator==(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> operator!=(const ShAffine<TT>& a, const ShAffine<TT>& b);

  /// Returns true iff lo = a.lo and hi = a.hi 
  template<typename TT>
  bool boundsEqual(const ShAffine<TT>& a);


  /** Clamping operators **/
  template<typename TT>
  friend ShAffine<TT> min(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> max(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> floor(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> ceil(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> rnd(const ShAffine<TT>& a);

  template<typename TT>
  friend ShAffine<TT> abs(const ShAffine<TT>& a);

  /** Misc Operators **/
  template<typename TT>
  friend ShAffine<TT> cond(const ShAffine<TT>& a, const ShAffine<TT>& b, const ShAffine<TT>& c);

  template<typename TT>
  friend ShAffine<TT> lerp(const ShAffine<TT>& a, const ShAffine<TT>& b, const ShAffine<TT>& c);

  /** Special Affine Ops **/
  template<typename TT>
  friend ShAffine<TT> range_union(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> range_isct(const ShAffine<TT>& a, const ShAffine<TT>& b);

  template<typename TT>
  friend ShAffine<TT> range_contains(const ShAffine<TT>& a, const ShAffine<TT>& b);
};


}

#include "ShAffineImpl.hpp"
  
#endif
