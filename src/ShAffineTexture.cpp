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
#include <valarray>
#include "ShMath.hpp"
#include "ShSyntax.hpp"
#include "ShStatement.hpp"
#include "ShTexCoord.hpp"
#include "ShAttrib.hpp"
#include "ShLib.hpp"
#include "ShLp.hpp"
#include "ShAffineTexture.hpp"

//#define SH_DBG_AAT

#ifdef SH_DBG_AAT
#define SH_DEBUG_PRINT_AAT(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_AAT(x) {}
#endif

namespace {

// @todo range -- this is turning into a mess since valarrays do some things
// well, and others not so well.  Clean this up (go write/find
// a multidimensional array class)

using namespace SH;

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::valarray<T>& v)
{
  out << "(";
  for(size_t i = 0; i < v.size(); ++i) {
    if(i != 0) out << ", ";
    out << v[i];
  }
  out << ")";
  return out;
}

typedef std::valarray<size_t> size_t_array;
typedef std::valarray<float> float_array;

template<typename T>
T product(const std::valarray<T>& w) {
  T result = w[0];
  for(size_t i = 1; i < w.size(); result *= w[i], ++i);
  return result;
}

/* Represents a sequence of multidimensional indices 
 * (slices in each dimension at the given stride) */
struct ArrayIndex {
  ArrayIndex(const size_t_array& start, const size_t_array& lengths, 
      const size_t_array& strides) 
    : m_dims(start.size()), m_start(start), 
      m_cur(static_cast<size_t>(0), start.size()), 
      m_lengths(lengths), 
      m_strides(strides)
  {
    SH_DEBUG_ASSERT(m_lengths.size() == m_dims && m_strides.size() == m_dims);
  }

  // prefix ++ operator
  ArrayIndex& operator++()
  {
    for(int i = m_dims - 1; i >= 0; --i) {
      if(m_cur[i] == m_lengths[i] - 1) {
        m_cur[i] = 0;
      } else {
        m_cur[i]++; 
        break;
      }
    }
    return *this;
  }

  // tests whether this has reached the end
  bool end() 
  {
    std::valarray<bool> done = (m_cur == m_lengths - static_cast<size_t>(1));
    return product(done);
  }

  size_t_array index() const {
    return m_start + m_cur * m_strides; 
  }

  size_t length() {
    return product(m_lengths);
  }

  friend std::ostream& operator<<(std::ostream& out, const ArrayIndex& idx)
  {
    out << idx.index() << " cur=" << idx.m_cur;
    return out;
  }

 private:
  size_t m_dims;
  size_t_array m_start, m_cur; 
  size_t_array m_lengths, m_strides;
};


// makes a copy of a multidimensional data array stored in a 1d array and 
// allows convenient indexing. 
//
// The higher the index in widths, the faster it increments in the 1D array
// (i.e. consecutive widths[widths.size()-1] are consecutive in the 1D array) 
// This may seem backwards for typical 1D packing of x,y,z... 
// but matches the way std::gslice_array works.
template<typename T>
struct ArrayAdaptor {
  ArrayAdaptor() {}

  ArrayAdaptor(T* data, size_t_array widths) 
    : m_dims(widths.size()), 
      m_size(product(widths)),
      m_data(data, m_size),
      m_widths(widths)
  {
    init();
  }

  ArrayAdaptor(size_t_array widths)
    : m_dims(widths.size()), 
      m_size(product(widths)),
      m_data(product(widths)),
      m_widths(widths)
  {
    init();
  }

  void init() {
    m_strides.resize(m_dims);
    m_strides[m_dims - 1] = 1;
    for(int i = m_dims - 1; i > 0;  --i) {
      m_strides[i - 1] = m_strides[i] * m_widths[i];
    }
  }

  T& operator[](int idx) {
    return m_data[idx];
  }

  /* Conversions between nD <-> 1D indices 
   * @{ */
  int indexOf(size_t_array idx) {
    SH_DEBUG_ASSERT(idx.size() == m_dims);
    return (idx * m_strides).sum();
  }

  size_t size() const {
    return m_size;
  }

  size_t_array indexOf(int idx) {
    size_t_array result(m_dims);
    for(int i = m_dims - 1; i >= 0; --i) {
      result[i] = idx % m_widths[i];
      idx /= m_widths[i];
    }
    return result;
  }
  /* @} */

  T& operator[](size_t_array idx) {
    return m_data[indexOf(idx)];
  }

  ArrayAdaptor& operator=(const ArrayAdaptor& other)
  {
    /* vararrays have undefined behaviour on assignment with a different sized
     * vararray */ 
    m_data.resize(other.m_data.size());
    m_widths.resize(other.m_widths.size());
    m_strides.resize(other.m_strides.size());

    m_dims = other.m_dims;
    m_size = other.m_size;
    m_data = other.m_data;
    m_widths = other.m_widths;
    m_strides = other.m_strides;

    return *this;
  }



  ArrayAdaptor& operator=(const T& value)
  {
    m_data = value;
    return *this;
  }

  /* copies out values to data, which must be large enough to hold them */ 
  void copyTo(T* data)
  {
    for(int i = 0; i < m_size; ++i) {
      data[i] = m_data[i];
    }
  }

  /* returns a copy of the data using the given ranges */
  std::valarray<T> rangeSlice(const size_t_array& start, const size_t_array& lengths)
  {
    SH_DEBUG_PRINT_AAT("dims = " << m_dims << " start = " << start << " lengths = " << lengths << " strides = " << m_strides);
    SH_DEBUG_ASSERT(start.size() == m_dims && lengths.size() == m_dims);
    return m_data[std::gslice(indexOf(start), lengths, m_strides)];
  }

  /* assigns other to the range slice */ 
  void rangeSliceAsn(const size_t_array& start, const size_t_array& lengths, const std::valarray<T>& other)
  {
    SH_DEBUG_ASSERT(start.size() == m_dims && lengths.size() == m_dims);
    SH_DEBUG_PRINT_AAT("start = " << indexOf(start) << " lengths = " << lengths << " strides = " << m_strides);
    m_data[std::gslice(indexOf(start), lengths, m_strides)] = other;
  }

  size_t m_dims;
  int m_size; // total size 
  std::valarray<T> m_data;
  size_t_array m_widths;
  size_t_array m_strides;
};


template<typename T>
std::ostream& operator<<(std::ostream& out, const ArrayAdaptor<T>& a)
{
  out << "{ widths = " << a.m_widths;
  out << ", strides = " << a.m_strides;
  out << ", data = " << a.m_data << " }";
  return out;
}

}

namespace SH {

ShAffineTexture::ShAffineTexture(ShTextureNodePtr other) 
  : m_other(other),
    m_width(other->width() * 3)
{
  SH_DEBUG_ASSERT(shIsRegularValueType(other->valueType()));

  switch (other->dims()) {
    case SH_TEXTURE_1D:   m_dims = 1; break; 

    case SH_TEXTURE_2D:
    case SH_TEXTURE_RECT: m_dims = 2; break; 

    case SH_TEXTURE_3D:   m_dims = 3; break; 
    default:
      SH_DEBUG_PRINT_AAT("Cannot handle this texture dim");
  }

  m_node = new ShTextureNodePtr[m_dims + 2];
  for(size_t i = 0; i < m_dims + 2; ++i) {
    m_node[i] = new ShTextureNode(other->dims(), other->size(), SH_FLOAT, 
        other->traits(), m_width, other->height(), other->depth());
  }

  m_other->memory()->add_dep(this);
  m_other->memory()->flush(); // update the data
}

ShAffineTexture::~ShAffineTexture()
{
  delete[] m_node;
}

// create a new texture memory
// save the original texture and all the mipmap levels on its right
void ShAffineTexture::memory_update() {
  size_t i, j;

  size_t_array widths(m_dims + 1);
  // index of the first non-tuple element dimension.  
  // This is the dimension we will expand the texture in to fit the new "mip-map" levels of data 
  size_t FIRST = m_dims - 1; 
  size_t ELEM = m_dims; 
  if(m_dims > 2) widths[m_dims - 3] = m_other->depth();
  if(m_dims > 1) widths[m_dims - 2] = m_other->height();
  m_width = widths[m_dims - 1] = m_other->width();
  size_t tupleSize = widths[m_dims] = m_other->size();
  size_t count = m_other->count(); // basically all the dimensions multiplied together, except tuple size 

  // cannot handle nD rectangles for now
  for(i = 0; i < FIRST; ++i) {
    SH_DEBUG_ASSERT(widths[i] == widths[FIRST]);
  }

  int typesize = shTypeInfo(m_other->valueType())->datasize();
  SH_DEBUG_ASSERT(m_other->valueType() == SH_FLOAT); // @todo range - can only handle this for now

  // access the original data
  ShHostStoragePtr oldmem = shref_dynamic_cast<ShHostStorage>(m_other->memory()->findStorage("host"));
  // @todo range handle non-float
  ArrayAdaptor<float> olddata(reinterpret_cast<float*>(oldmem->data()), widths);

  // create a new memory for the new texture
  
  // We need to store the original texture 
  // + mip-map requirements * 2^(m_dims)
  // for the 2^(m_dims) 1/2 offsets in each dimension for each mip-map level. 
  size_t_array newWidths = widths;
  newWidths[FIRST] *= 3;

  size_t CONST_FACTOR = m_dims; // index of the constant factor in newdata
  size_t DELTA = m_dims + 1; // index of the approximation factor
  size_t OUTPUTS = m_dims + 2; // total number of outputs - m_dims affine scaling factors, 1 constant, 1 approx error 
  ArrayAdaptor<float> *newdata = new ArrayAdaptor<float>[OUTPUTS];

  // Generate a zeroed out array for each new texture
  for(i = 0; i < OUTPUTS; ++i) {
    newdata[i] = ArrayAdaptor<float>(newWidths);
    newdata[i] = 0;
  }

  // copy over the original data into the constant factor texture 
  // (basically slice off a range on widths[1])
  size_t_array lo(static_cast<size_t>(0), m_dims + 1);
  newdata[CONST_FACTOR].rangeSliceAsn(lo, widths, olddata.m_data);

  SH_DEBUG_PRINT_AAT("newdata[CONST_FACTOR] " << newdata[CONST_FACTOR]);

  // Prepare for minimax approximation
  // Initialize LP constraints A, b, objective function c, and bounded variables (only delta)
  int constr_size = widths.size();
  size_t_array constr_widths(widths); 
  constr_widths[ELEM] = 2;
  SH_DEBUG_PRINT_AAT("constr_widths = " << constr_widths);

  size_t_array Awidths(constr_size + 1);
  Awidths[std::slice(0,constr_size,1)] = constr_widths; 
  Awidths[constr_size] = OUTPUTS;

  ArrayAdaptor<float> A(Awidths);
  SH_DEBUG_PRINT_AAT("A= " << A);

  // A = [ texcoord indices, 1, +-1 ] (with two rows for each texel texcoord in olddata
  size_t constr_length = product(constr_widths);
  for(i = 0; i < constr_length; ++i) {
    size_t base = i * OUTPUTS;
    size_t_array idx(A.indexOf(base));
    float sign = 1 - 2.0 * idx[ELEM]; 
    for(j = 0; j < m_dims; ++j) {
      A[base + j] = sign * idx[j];
    }
    A[base + CONST_FACTOR] = sign;
    A[base + DELTA] = -1; 
  }

  ArrayAdaptor<float> *b = new ArrayAdaptor<float>[tupleSize];
  for(i = 0; i < tupleSize; ++i) {
    b[i] = ArrayAdaptor<float>(constr_widths);
    float_array olddata_i(olddata.m_data[std::slice(i, count, tupleSize)]); 
    b[i].m_data[std::slice(0, count, 2)] = olddata_i; 
    b[i].m_data[std::slice(1, count, 2)] = -olddata_i; 
  }

  float_array c(0.0f, OUTPUTS); 
  c[DELTA] = -1;

  std::valarray<bool> bounded(false, OUTPUTS);
  bounded[DELTA] = true;

  SH_DEBUG_PRINT_AAT("A:\n" << A);
  for(i = 0; i < tupleSize; ++i) {
    SH_DEBUG_PRINT_AAT("b[" << i << "]: " << b[i]);
  }
  SH_DEBUG_PRINT_AAT("c: " << c);
  SH_DEBUG_PRINT_AAT("bounded: " << bounded);

  // fill in the data.  Use one multidimensional iterator (R) to generate ranges of
  // the original data over which we need to do minimax approx.
  // Use another iterator (O) to iterate over the index in the output data where
  // the result goes.
  size_t cur_width = 2; // widths of the ranges
  size_t cur_length = widths[FIRST];
  for(; cur_width <= widths[FIRST]; cur_width *= 2, cur_length /= 2) {
    size_t_array output_start(static_cast<size_t>(0), m_dims + 1);
    output_start[FIRST] = widths[FIRST] + cur_length; 
    size_t_array output_strides(static_cast<size_t>(1), m_dims + 1);

    size_t_array lengths(cur_length - 1, m_dims + 1); 
    lengths[ELEM] = 1;

    size_t_array range_start(static_cast<size_t>(0), m_dims + 1);
    size_t_array range_strides(cur_width / 2, m_dims + 1); // sample one level more finely

    SH_DEBUG_PRINT("cur_width = " << cur_width << " cur_length= " << cur_length); 
    SH_DEBUG_PRINT("range_start = " << range_start << " lengths = " << lengths << " range_strides = " << range_strides);

    for(i = 0; i < tupleSize; ++i) {
      SH_DEBUG_PRINT_AAT("  element=" << i);
      output_start[ELEM] = i;

      // break out slices of A and b and the result
      
      SH_DEBUG_PRINT_AAT("output_start = " << output_start << " lengths = " << lengths << " output_strides = " << output_strides);

      ArrayIndex R(range_start, lengths, range_strides); 
      ArrayIndex O(output_start, lengths, output_strides); 

      for(;; ++R, ++O) { // assuming this happens at least once...else bad
        SH_DEBUG_PRINT_AAT("    R = " << R << " O = " << O);
        size_t_array start(R.index());
        size_t_array length(cur_width, m_dims + 1);
        length[ELEM] = 2;
        SH_DEBUG_PRINT_AAT("    start = " << start << " length = " << length);

        size_t_array Astart(start.size() + 1);
        size_t_array Alength(length.size() + 1);
        Astart[std::slice(0, start.size(), 1)] = start;
        Astart[start.size()] = 0;
        Alength[std::slice(0, length.size(), 1)] = length;
        Alength[length.size()] = OUTPUTS;

        SH_DEBUG_PRINT_AAT("    Astart = " << Astart << " Alength = " << Alength);

        size_t M = product(length);

        float_array x( shLpSimplexRelaxed(M, OUTPUTS, 
                                           A.rangeSlice(Astart, Alength),
                                           b[i].rangeSlice(start, length),
                                           c, bounded) );

        SH_DEBUG_PRINT_AAT("    Result x = " << x );

        for(j = 0; j < m_dims; ++j) {
          newdata[m_dims - j - 1][O.index()] = x[j];
        }
        newdata[CONST_FACTOR][O.index()] = x[CONST_FACTOR];
        newdata[DELTA][O.index()] = x[DELTA];

        if(R.end()) break;
      }
    }
  }

  // Stick the data into textures
  for(i = 0; i < OUTPUTS; ++i) {
    ShHostMemoryPtr newmem = new ShHostMemory(product(newWidths) * typesize);
    newdata[i].copyTo(reinterpret_cast<float*>(newmem->hostStorage()->data()));

    // update the new texture parameters
    m_node[i]->memory(newmem, 0);
    switch(m_dims) {
      case 1: m_node[i]->setTexSize(newWidths[0]); break;
      case 2: m_node[i]->setTexSize(newWidths[1], newWidths[0]); break;
      case 3: m_node[i]->setTexSize(newWidths[2], newWidths[1], newWidths[0]);
    }
  }

  delete[] newdata;
  delete[] b;
}


ShVariableNodePtr ShAffineTexture::makeTemp(const ShVariable &v, const std::string& name, int size) 
{
  if(size == 0) size = v.size();
  ShVariableNodePtr result = v.node()->clone(SH_TEMP, size, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false); 
  result->name(name);
  return result;
}

ShRecord ShAffineTexture::rect_lookup(const ShVariable &lo, const ShVariable &hi) const 
{
  ShRecord result;

  // find out which level we should be looking at
  // * first get a potential level (that may have ranges one level too small),
  // * check if it works (special case level 0, since the original texels do not
  //   have an overlapping sampling offsets)
  // * decide on actual level
  // * get lookup coordinates
  // * lookup & translate constant value from center of 0,0 to center of lookup range

  size_t size = lo.size();
  ShVariable width(makeTemp(lo, "width"));
  shADD(width, hi, -lo);

  ShConstAttrib1f ZERO(0.0f);
  ShConstAttrib1f ONE(1.0f);
  ShVariable level(makeTemp(lo, "level")); // start with a guess about the level
  shLOG2(level, width);
  shCEIL(level, level);
  shMAX(level, level, ZERO.repeat(size));

  ShVariable gt0(makeTemp(lo, "level > 0"));
  shSGT(gt0, level, ZERO.repeat(size));

  ShVariable scale(makeTemp(lo, "scale"));
  ShConstAttrib1f HALF(0.5f);
  shADD(scale, level, -gt0);
  shPOW(scale, HALF.repeat(size), scale); // we have (1/2)^s where s = level - (level == 0 ? 0 : 1);

  ShVariable diff(makeTemp(lo, "diff")); // difference threshold between cells to force a level increase
  shADD(diff, HALF.repeat(size), gt0);

  ShVariable cell_lo(makeTemp(lo, "cell_lo"));
  ShVariable cell_hi(makeTemp(hi, "cell_hi"));

  shMUL(cell_lo, lo, scale);
  shFLR(cell_lo, cell_lo); 

  shMUL(cell_hi, hi, scale);
  shFLR(cell_hi, cell_hi); 

  ShVariable inc_level(makeTemp(lo, "inc_level")); // should be bool - whether to increase level
  shADD(inc_level, cell_hi, -cell_lo);
  shSGT(inc_level, inc_level, diff);

  shADD(level, level, inc_level);

  ShVariable max_level(makeTemp(lo, "max_level", 1));
  if(level.size() == 1) {
    shASN(max_level, level);
  } else {
    // slow...
    shMAX(max_level, level(0), level(1));
    for(int i = 2; i < level.size(); ++i) {
      shMAX(max_level, max_level, level(i));
    }
  }

  ShVariable offset(makeTemp(lo, "offset"));
  ShVariable texscale(makeTemp(lo, "texscale"));

  shADD(texscale, max_level, -ONE); 
  shPOW(texscale, HALF, texscale);
  
  shASN(offset, texscale);
  shADD(offset, offset, ONE);
  shMUL(offset, offset, ShConstAttrib1f(m_width));

  // handle level 0 case
  shCOND(offset, max_level, offset, ZERO); 
  shCOND(texscale, max_level, texscale, ONE);

  ShVariable texcoord(makeTemp(lo, "texcoord"));
  shMAD(texcoord, lo, scale, offset);

  for(size_t i = 0; i < m_dims + 2; ++i) {
    ShVariable resulti(m_node[i]->clone(SH_TEMP, 0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false));
    ShVariable texVar(m_node[i]);

    ShStatement stmt(resulti, texVar, SH_OP_TEXI, texcoord);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);

    result.append(resulti);
  }
  return result;
}
    
ShRecord ShAffineTexture::lookup(const ShVariable& lo, const ShVariable& hi) const {
  ShVariable scaled_lo(makeTemp(lo, "aatex_scl_lo"));
  ShVariable scaled_hi(makeTemp(hi, "aatex_scl_hi"));

  // turn it into a rect lookup
  ShVariable texsize = m_other->texSizeVar();
  shMUL(scaled_lo, lo, texsize); 
  shMUL(scaled_hi, hi, texsize); 

  ShRecord result = rect_lookup(scaled_lo, scaled_hi);

  // scale slopes 
  ShRecord::iterator I = result.begin();
  for(size_t i = 0; i < m_dims; ++i, ++I) {
    shMUL(*I, *I, texsize(i));  
  }
  return result;
}


ShTextureNodePtr ShAffineTexture::node(size_t i) {
  return m_node[i];
}

ShAffineTexture::TexAaTexMap ShAffineTexture::tatmap;

ShAffineTexturePtr ShAffineTexture::convert(ShTextureNodePtr other) 
{
  TexAaTexMap::iterator I = tatmap.find(other);
  if(I != tatmap.end()) {
    return I->second;
  }
  return (tatmap[other] = new ShAffineTexture(other));
}

}
