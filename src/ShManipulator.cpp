#include <cstdarg>
#include "ShManipulator.hpp"
#include "ShVariableNode.hpp"
#include "ShAlgebra.hpp"

namespace SH {
ShManipulator::ShManipulator() {} 

void ShManipulator::append(int i) {
  m_ranges.push_back(IndexRange(i, i));
}

void ShManipulator::append(int start, int end) {
  m_ranges.push_back(IndexRange(start, end));
}

void ShManipulator::append(IndexRange r) {
  m_ranges.push_back(r);
}

ShProgram operator<<(const ShProgram &p, const ShManipulator &m) {
  int size = p->outputs.size();

  ShProgram permuter = SH_BEGIN_PROGRAM() {
    /* Make shader inputs from p's outputs */
    std::vector<ShVariable> inputs;
    for(ShProgramNode::VarList::const_iterator outIt = p->outputs.begin();
        outIt != p->outputs.end(); ++outIt) {
      inputs.push_back(ShVariable(new ShVariableNode(SH_VAR_INPUT, 
              (*outIt)->size(), (*outIt)->specialType())));
    }

    int size = inputs.size();

    /* Make shader outputs from permuted ranges of inputs */
    for(ShManipulator::IndexRangeVector::const_iterator irvIt = m.m_ranges.begin();
        irvIt != m.m_ranges.end(); ++irvIt) {
      int start = irvIt->first;
      if(start < 0) start = size + start;
      
      int end = irvIt->second;
      if(end < 0) end = size + end;

      for(int i = start; i <= end; ++i) {
        ShVariable output(new ShVariableNode(SH_VAR_OUTPUT, 
              inputs[i].node()->size(), inputs[i].node()->specialType()));

        ShStatement stmt(output, SH_OP_ASN, inputs[i]);
        ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
      }
    }
  } SH_END_PROGRAM;

  return connect(p, permuter); 
}

#if 0
ShFixedSizeManipulator::ShFixedSizeManipulator(int srcSize)
  : m_srcSize(srcSize) {}

ShFixedSizeManipulator ShFixedSizeManipulator::operator&(const ShFixedSizeManipulator &b) const {
  ShFixedSizeManipulator result(m_srcSize + b.m_srcSize);

  for(IndexRangeVector::const_iterator it = m_ranges.begin();
      it != m_ranges.end(); ++it) {
    result.append(*it); 
  }

  for(IndexRangeVector::const_iterator it = b.m_ranges.begin();
      it != b.m_ranges.end(); ++it) {
    result.append(m_srcSize + it->first, m_srcSize + it->second);
  }
}

keep::keep(int n)
  : ShFixedSizeManipulator(n) {
  append(0, n - 1);
}

lose::lose(int n) 
  : ShFixedSizeManipulator(n) {}
#endif

permute::permute(int outputSize, ...) { 
  va_list ap;
  va_start(ap, outputSize);
  for(int i = 0; i < outputSize; ++i) {
    int index = va_arg(ap, int);
    append(index);
  }
  va_end(ap);
}

permute::permute(std::vector<int> indices) {
  for(std::vector<int>::iterator it = indices.begin();
      it != indices.end(); ++it) {
    append(*it);
  }
}

permute permute::operator*(const permute &p) const {
  std::vector<int> resultIndices;
  for(IndexRangeVector::const_iterator it = p.m_ranges.begin();
      it != p.m_ranges.end(); ++it) {
    int index = it->first;
    assert(index == it->second);

    if(index < 0) index = m_ranges.size() + index;
    assert(index >= 0 && index < m_ranges.size()); 

    resultIndices.push_back(m_ranges[index].first); 
  }
  return permute(resultIndices);
}

range range::operator()(int i) {
  range result = *this;
  result.append(i);
}

range range::operator()(int start, int end) {
  range result = *this;
  result.append(start, end);
}

extract::extract(int k) {
  append(k);
  append(0, k - 1);
  append(k + 1, -1);
}

drop::drop(int k) {
  if(k >= 0) {
    append(k + 1, -1); 
  } else {
    append(0, k - 1); 
  }
}

}
