#include <vector>
#include <cassert>
#include <cstdarg>
#include "Man.hpp"
#include "ManMatrix.hpp"

using namespace SH;
using namespace std;

void ManMatrix::setCount() {
  /* update count */
  switch(free_input_count()) {
    case 0: m_in = 0; 
    break;
    case 1: 
      m_in = (*begin_free_inputs())->size();
      break;
    default:
      m_in = -1;
      std::cerr << "Too many free inputs: " << free_input_count() << std::endl; 
  }
}

ManMatrix ManMatrix::resize_inputs(int size) const {
  assert(size >= m_in);
  if(size > m_in) {
    int* swiz = new int[m_in];
    for(int i = 0; i < m_in; ++i) swiz[i] = i; 
    SH::Program swizzer = SH_BEGIN_PROGRAM() {
      Variable in = var(SH_INPUT, size);
      if(m_in > 0) {
        Variable out = insize_var(SH_OUTPUT);
        shASN(out, in(m_in, swiz));
      }
    } SH_END;
    delete[] swiz;
    return *this << swizzer;
  } else {
    return *this;
  }
}
