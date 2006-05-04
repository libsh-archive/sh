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
#ifndef SHLIBWORLEYIMPL_HPP 
#define SHLIBWORLEYIMPL_HPP

#include "LibWorley.hpp"
#include <cstdlib>
#include "Syntax.hpp"
#include "BaseTexture.hpp"
#include "Generic.hpp"

namespace SH {

// Integer Power using template metaprogramming...
// TODO find a better way than this.
// sort on N-tuples needs to know at C++ compile time exactly 
template<int N, int M> 
struct _IntPow {
  static const int value = N * _IntPow<N, M-1>::value;
};

template<int N> 
struct _IntPow<N, 1> {
  static const int value = N;
};

// The foundation for the regular grid-based generators
template<int D, typename T>
struct GridGenFactory: public GeneratorFactory<_IntPow<3, D>::value, D, T> { 
  void operator()(const Generic<D, T> &p, Generator<D, T> result[]) const;
  
private:
  // given a Generator that has its cell and offset filled in, this function
  // generates the actual pos.
  virtual void makePos(Generator<D, T> &g) const = 0; 
};

// The default generator - uses a uniform integer grid with one
// generator point in each grid cell.
// If PointType is d-dimensional, it generates 3^d points from 
// p's cell and all adjacent grid cells.
template<int D, typename T>
struct DefaultGenFactory: public GridGenFactory<D, T> { 
  DefaultGenFactory(bool useTexture): m_useTexture(useTexture) {}
  
private:
  void makePos(Generator<D, T> &g) const;
  bool m_useTexture;
};

// a null point generator - just makes a non-jittered grid
// of generators (good for debugging, or certain kinds of patterns...)
template<int D, typename T>
struct NullGenFactory: public GridGenFactory<D, T> { 
private:
  void makePos(Generator<D, T> &g) const;
};

// An animating point generator - uses same uniform integer grid
// method as the default, except that it linearly interpolates
// between pairs of generator point sets, moving through a sequence
// of point sets as time increases. 
template<int D, typename T>
struct LerpGenFactory: GridGenFactory<D, T> {
  LerpGenFactory(const Generic<1, T> &time, bool useTexture);
  
private:
  void makePos(Generator<D, T> &g) const;
  const Generic<1, T> &m_time;
  bool m_useTexture;
};

// when Program objects can be called like functions, this will 
// no longer be necessary as it is analogous to the Algebra combine operator.
template<int N, int D, typename T, typename P1, typename P2>
struct CombinedPropFactory: public PropertyFactory<N, D, T> {
  CombinedPropFactory(const P1 *propFactory1, const P2 *propFactory2);
  
  Generic<N, T> operator()(const Generic<D, T> &p,
                             const Generator<D, T> &g) const;

private:
  const P1* m_propFactory1;
  const P2* m_propFactory2;
};


// standard distance based property factories 
// Re-write later to take function pointer (or Program object)
// to a distance function.
template<int D, typename T>
struct DistSqPropFactory: public PropertyFactory<1, D, T> {
  Generic<1, T> operator()(const Generic<D, T> &p,
                             const Generator<D, T> &g) const; 
};

template<int D, typename T>
struct Dist_1PropFactory: public PropertyFactory<1, D, T> {
  Generic<1, T> operator()(const Generic<D, T> &p,
                             const Generator<D, T> &g) const; 
};

template<int D, typename T>
struct Dist_InfPropFactory: public PropertyFactory<1, D, T> {
  Generic<1, T> operator()(const Generic<D, T> &p, 
                             const Generator<D, T> &g) const; 
};

template<int D, typename T>
struct DistSqGradientPropFactory: public PropertyFactory<D + 1, D, T> {
  Generic<D + 1, T> operator()(const Generic<D, T> &p, 
                                 const Generator<D, T> &g) const; 
};

template<int D, typename T>
struct Dist_1GradientPropFactory: public PropertyFactory<D + 1, D, T> {
  Generic<D + 1, T> operator()(const Generic<D, T> &p, 
                                 const Generator<D, T> &g) const; 
};

template<int D, typename T>
struct Dist_InfGradientPropFactory: public PropertyFactory<D + 1, D, T> {
  Generic<D + 1, T> operator()(const Generic<D, T> &p, 
                                 const Generator<D, T> &g) const;
};

template<int N, int D, typename T>
struct CellnoisePropFactory: public PropertyFactory<N, D, T> {
  CellnoisePropFactory(bool useTexture): m_useTexture(useTexture) {}
  Generic<N, T> operator()(const Generic<D, T> &p,
                             const Generator<D, T> &g) const; 

private:
  bool m_useTexture;
};

template<typename TexType, typename T>
struct Tex2DPropFactory: public PropertyFactory<TexType::typesize, 2, T> {
  Tex2DPropFactory(const BaseTexture2D<TexType> &tex, 
                   const Generic<1, T> &scale);
  Generic<TexType::typesize, T> operator()(const Generic<2, T> &p, 
                                             const Generator<2, T> &g) const
  {
    // Moved here from WorleyImpl.hpp because MSVC gets confused otherwise
    return m_tex(frac(g.cell * invScale * m_scale)) * ConstAttrib1f(1.0f);
  }

private:
  const BaseTexture2D<TexType> &m_tex;
  const Generic<1, T> &m_scale;
  ConstAttrib2f invScale;
  // TODO remove invScale and restrict to RECT textures later 
};

//----------------
// IMPLEMENTATION
//----------------

template<int D, typename T>
void GridGenFactory<D, T>::operator()(const Generic<D, T> &p, 
                                      Generator<D, T> result[]) const
{
  Attrib<D, SH_TEMP, T> pCell = floor(p);

  // each set of two bits represents offset along one dimension
  // if the bit value = 0, offset by -1, 1 = no offset, 2 = offset by 1 
  int i, j, offsetBits;
  ConstAttrib3f offsets(-1, 0, 1);
  int offsetSwiz[D];
  j = 0;
  for(offsetBits = 0; offsetBits < (1 << (D * 2)); ++offsetBits) {
    for(i = 0; i < D; ++i) {
      offsetSwiz[i] = ((offsetBits >> (i * 2)) & 3); 
      if(offsetSwiz[i] == 3) break;
    }
    if(i < D) continue;

    Generator<D, T> gen;
    result[j].offset = offsets.template swiz<D>(offsetSwiz); 
    SH_DEBUG_PRINT("Offset:" << result[j].offset); 
    result[j].cell = pCell + result[j].offset;
    makePos(result[j]);
    ++j;
  }
}

template<int D, typename T>
void DefaultGenFactory<D, T>::makePos(Generator<D, T> &g) const 
{
  g.pos = g.cell + cellnoise<D>(g.cell, m_useTexture);
}

template<int D, typename T>
void NullGenFactory<D, T>::makePos(Generator<D, T> &g) const 
{
  g.pos = g.cell + fillcast<D>(Attrib<1, SH_CONST, T>(0.5f)); 
}

template<int D, typename T>
LerpGenFactory<D, T>::LerpGenFactory(const Generic<1, T> &time, bool useTexture)
  : m_time(time), m_useTexture(useTexture)
{
}

template<int D, typename T>
void LerpGenFactory<D, T>::makePos(Generator<D, T> &g) const
{
  Attrib<1, SH_TEMP, T> lastTime = floor(m_time);
  Attrib<1, SH_TEMP, T> timeOffset = frac(m_time);
  Attrib<D + 1, SH_TEMP, T> offsetCell;

  offsetCell = fillcast<D+1>(g.cell);
  offsetCell(D) = lastTime;
  Attrib<D, SH_TEMP, T> p1 = cellnoise<D>(offsetCell, m_useTexture); 

  offsetCell(D) += 1;
  Attrib<D, SH_TEMP, T> p2 = cellnoise<D>(offsetCell, m_useTexture); 

  g.pos = g.cell + lerp(timeOffset, p2, p1);
}

template<int N, int D, typename T, typename P1, typename P2>
CombinedPropFactory<N, D, T, P1, P2>::CombinedPropFactory(const P1 *propFactory1, 
                                                          const P2 *propFactory2)
  : m_propFactory1(propFactory1), m_propFactory2(propFactory2)
{
}

template<int N, int D, typename T, typename P1, typename P2>
Generic<N, T>
CombinedPropFactory<N, D, T, P1, P2>::operator()(const Generic<D, T> &p,
                                                 const Generator<D, T> &g) const
{
  return join((*m_propFactory1)(p, g), (*m_propFactory2)(p, g));
}

template<typename P1, typename P2>
PropertyFactory<P1::NUM_PROPS + P2::NUM_PROPS, P1::DIM, typename P1::PropType>*
combine(const P1 *propFactory1, const P2 *propFactory2)
{
  const int N = P1::NUM_PROPS + P2::NUM_PROPS;
  return new CombinedPropFactory<N, P1::DIM, typename P1::PropType, P1, P2>(propFactory1, propFactory2);
}

template<int D, typename T>
Generic<1, T> 
DistSqPropFactory<D, T>::operator()(const Generic<D, T> &p,
                                    const Generator<D, T> &g) const
{
  Attrib<D, SH_TEMP, T> delta = p - g.pos;
  return delta | delta; 
}

template<int D, typename T>
Generic<1, T> 
Dist_1PropFactory<D, T>::operator() (const Generic<D, T> &p, 
                                     const Generator<D, T> &g) const
{
  Attrib<D, SH_TEMP, T> delta = abs(p - g.pos);
  return delta | fillcast<D>(ConstAttrib1f(1.0f)); 
}

template<int D, typename T>
Generic<1, T> 
Dist_InfPropFactory<D, T>::operator() (const Generic<D, T> &p,
                                       const Generator<D, T> &g) const
{
  Attrib<D, SH_TEMP, T> delta = abs(p - g.pos);

  // TODO replace with tuple max function when implemented 
  Attrib<1, SH_TEMP, T> result = 0; 
  for(int i = 0; i < D; ++i) result = max(result, delta(i));
  return result;
}

template<int D, typename T>
Generic<D + 1, T> 
DistSqGradientPropFactory<D, T>::operator() (const Generic<D, T> &p,
                                             const Generator<D, T> &g) const
{
  Attrib<D + 1, SH_TEMP, T> result;
  Attrib<D, SH_TEMP, T> delta = p - g.pos;

  result(0) = delta | delta; 
  for(int i = 1; i < D + 1; ++i) {
    result(i) = delta(i-1) * result(0);
  }
  return result;
}

template<int D, typename T>
Generic<D + 1, T> 
Dist_1GradientPropFactory<D, T>::operator() (const Generic<D, T> &p, 
                                             const Generator<D, T> &g) const
{

  Attrib<D + 1, SH_TEMP, T> result;
  Attrib<D, SH_TEMP, T> delta = p - g.pos;

  Attrib<1, SH_CONST, T> ONE(1);
  result(0) = abs(delta) | fillcast<D>(ONE);
  for(int i = 1; i < D + 1; ++i) { // TODO switch to a swizzled version
    result(i) = cond(delta(i-1), ONE, -ONE); 
  }
  return result;
}

template<int D, typename T>
Generic<D + 1, T> 
Dist_InfGradientPropFactory<D, T>::operator() (const Generic<D, T> &p, 
                                               const Generator<D, T> &g) const
{
  Attrib<D + 1, SH_TEMP, T> result;
  Attrib<D, SH_TEMP, T> delta = p - g.pos;
  Attrib<D, SH_TEMP, T> absDelta = abs(delta); 

  result(0) = 0;
  for(int i = 0; i < D; ++i) result(0) = max(result(0), absDelta(i)); 

  // TODO remove this when constant folding is available
  Attrib<1, SH_CONST, T> ONE(1);
  for(int i = 1; i < D + 1; ++i) { // TODO switch to a swizzled version
    result(i) = (absDelta(i-1) >= result(0)) * cond(delta(i-1), ONE, -ONE); 
  }
  return result;
}

template<int N, int D, typename T>
Generic<N, T>
CellnoisePropFactory<N, D, T>::operator() (const Generic<D, T> &p, 
                                           const Generator<D, T> &g) const
{
  return cellnoise<N>(g.cell, m_useTexture);
}

template<typename TexType, typename T>
Tex2DPropFactory<TexType, T>::Tex2DPropFactory(const BaseTexture2D<TexType> &tex, 
                                              const Generic<1, T> &scale)
  : m_tex(tex), m_scale(scale), 
    invScale(ConstAttrib2f(1.0f, 1.0f) / tex.size()) 
{
}

template<int N, int K, int P, typename T>
void kSelect(const Generic<P, T> vals[N], Generic<K, T> result[N], 
             float LARGE = 1e10)
{
  result[0] = fillcast<K>(ConstAttrib1f(LARGE));

  int i, j, k, l;
  int shiftswiz[K];
  Attrib1f c;
  for(i = 0; i < K; ++i) shiftswiz[i] = (i == 0 ? 0 : i - 1);

  // insert into result one by one
  for(i = 0; i < P; ++i) {
    // check if smaller than the first one
    c = vals[0](i) < result[0](0);
    for(j = 0; j < N; ++j) {
      result[0] = lerp(c, result[0].template swiz<K>(shiftswiz), result[0]); 
      result[0](0) = lerp(c, vals[0](i), result[0](0));
    }

    // check for the other ones
    for(j = 1; j < K; ++j) {
      c = (result[0](j-1) < vals[0](i)) * (vals[0](i) < result[0](j));
      for(k = 0; k < N; ++k) {
        for(l = 0; l < K; ++l) shiftswiz[l] = l + ( l >= j ? -1 : 0); 
        result[k] = lerp(c, result[k].template swiz<K>(shiftswiz), result[k]);
        result[k](j) = lerp(c, vals[k](i), result[k](j));
      }
    }
  }
}

template<int K, int L, int P, int D, typename T>
void worley(Generic<K, T> result[], const Generic<D, T> &p,
            const GeneratorFactory<P, D, T> *genFactory,
            const PropertyFactory<L, D, T> *propFactory)
{
  int i, j;
  Generator<D, T> generators[P];
  Attrib<P, SH_TEMP, T> props[L]; 
  Attrib<L, SH_TEMP, T> propTemp; 

  (*genFactory)(p, generators); // make generators
  for(i = 0; i < P; ++i) {
    propTemp = (*propFactory)(p, generators[i]);
    for(j = 0; j < L; ++j) props[j](i) = propTemp(j);
  }

  // sort points & gradients by distance
  groupsort<L>(props); 

  // weighted sum of basis function values to get final result
  for(j = 0; j < L; ++j) result[j] = cast<K>(props[j]);
}

template<int K, int D, typename T>
Generic<K, T> worley(const Generic<D, T> &p, 
#ifdef _WIN32
                       bool useTexture=true)
#else
                       bool useTexture)
#endif
{
  DefaultGenFactory<D, T> genFactory(useTexture);
  DistSqPropFactory<D, T> propFactory;
  Attrib<K, SH_TEMP, T> result;
  worley(&result, p, &genFactory, &propFactory);
  return result;
}

template<int K, int D, typename T>
Program worley(
#ifdef _WIN32
                   bool useTexture=true)
#else
                   bool useTexture)
#endif
{
  DefaultGenFactory<D, T> genFactory(useTexture);
  DistSqPropFactory<D, T> propFactory;
  return worley<K>(&genFactory, &propFactory); 
}

template<int K, int L, int P, int D, typename T>
Program worley(const GeneratorFactory<P, D, T> *genFactory, 
                   const PropertyFactory<L, D, T> *propFactory)
{
  Program program = SH_BEGIN_PROGRAM() {
    Attrib<D, SH_INPUT, T, SH_TEXCOORD> SH_DECL(texcoord);

    Attrib<K, SH_OUTPUT, T> result[L]; 
    worley(&result[0], texcoord, genFactory, propFactory); 
  } SH_END_PROGRAM;
  return program;
}

} // namespace SH

#endif // SHLIBWORLEYIMPL_HPP
