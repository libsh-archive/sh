#include <cstdarg>
#include <cassert>
#include <sstream>
#include "ShVariableNode.hpp"
#include "ShError.hpp"
#include "ShAlgebra.hpp"
#include "ShManipulator.hpp"

namespace SH {
ShManipulator::ShManipulator() {} 

ShManipulator::ShManipulator(const ShManipulator &m)
  : m_ranges(m.m_ranges) {
} 

void ShManipulator::append(int i) {
  m_ranges.push_back(IndexRange(i, i));
}

void ShManipulator::append(int start, int end) {
  m_ranges.push_back(IndexRange(start, end));
}

void ShManipulator::append(IndexRange r) {
  m_ranges.push_back(r);
}

/* input permutation  */
ShProgram operator<<(const ShProgram &p, const ShManipulator &m) {
  int size = p->inputs.size();
  int i;

  ShProgram permuter = SH_BEGIN_PROGRAM() {
    /* Make shader outputs from p's inputs */
    std::vector<ShVariable> outputs;
    for(ShProgramNode::VarList::const_iterator inIt = p->inputs.begin();
        inIt != p->inputs.end(); ++inIt) {
      outputs.push_back(ShVariable(new ShVariableNode(SH_VAR_OUTPUT, 
              (*inIt)->size(), (*inIt)->specialType())));
    }

    int size = outputs.size();

    /* Make shader outputs from permuted ranges of inputs */
    std::vector<bool> used(size, false); //mark used inputs
    for(ShManipulator::IndexRangeVector::const_iterator irvIt = m.m_ranges.begin();
        irvIt != m.m_ranges.end(); ++irvIt) {
      int start = irvIt->first;
      if(start < 0) start = size + start;
      
      int end = irvIt->second;
      if(end < 0) end = size + end;

      if(start < 0 || start >= size || end < 0 || end >= size) {
        std::ostringstream os;
        os << "Invalid ShManipulator Range (" << irvIt->first << ", " <<
          irvIt->second << ") for an ShProgram with output size " << size;
         
        ShError(ShAlgebraException(os.str())); 
      }

      for(i = start; i <= end; ++i) {
        if(used[i]) {
          std::ostringstream os;
          os << "Duplicate index " << i << " in range (" << start 
            << ", " << end << ") not allowed for input manipulators"; 
          ShError(ShAlgebraException(os.str()));
        }
        used[i] = true;

        ShVariable input(new ShVariableNode(SH_VAR_INPUT, 
              outputs[i].node()->size(), outputs[i].node()->specialType()));

        ShStatement stmt(outputs[i], SH_OP_ASN, input);
        ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
      }
    }

    for(i = 0; i < size; ++i) {
      if(!used[i]) {
        ShVariable input(new ShVariableNode(SH_VAR_INPUT, 
              outputs[i].node()->size(), outputs[i].node()->specialType()));

        ShStatement stmt(outputs[i], SH_OP_ASN, input);
        ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
      }
    }
  } SH_END_PROGRAM;

  return connect(permuter, p); 
}

ShProgram operator>>(const ShManipulator &m, const ShProgram &p) {
  return p << m;
}

ShProgram operator<<(const ShManipulator &m, const ShProgram &p) {
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

      if(start < 0 || start >= size || end < 0 || end >= size) {
        std::ostringstream os;
        os << "Invalid ShManipulator Range (" << irvIt->first << ", " <<
          irvIt->second << ") for an ShProgram with output size " << size;
         
        ShError(ShAlgebraException(os.str())); 
      }

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

ShProgram operator>>(const ShProgram &p, const ShManipulator &m) {
  return m << p;
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

shPermute::shPermute(int outputSize, ...) { 
  va_list ap;
  va_start(ap, outputSize);
  for(int i = 0; i < outputSize; ++i) {
    int index = va_arg(ap, int);
    append(index);
  }
  va_end(ap);
}

shPermute::shPermute(std::vector<int> indices) {
  for(std::vector<int>::iterator it = indices.begin();
      it != indices.end(); ++it) {
    append(*it);
  }
}

shPermute shPermute::operator*(const shPermute &p) const {
  std::vector<int> resultIndices;
  for(IndexRangeVector::const_iterator it = p.m_ranges.begin();
      it != p.m_ranges.end(); ++it) {
    int index = it->first;
    assert(index == it->second);

    if(index < 0) index = m_ranges.size() + index;
    assert(index >= 0 && index < m_ranges.size()); 

    resultIndices.push_back(m_ranges[index].first); 
  }
  return shPermute(resultIndices);
}

shRange::shRange(int i) {
  append(i);
}

shRange::shRange(int start, int end) {
  append(start, end);
}

shRange shRange::operator()(int i) {
  shRange result(*this);
  result.append(i);
  return result;
}

shRange shRange::operator()(int start, int end) {
  shRange result(*this); 
  result.append(start, end);
  return result;
}

shExtract::shExtract(int k) {
  append(k);
  if( k != 0 ) append(0, k - 1);
  if( k != -1 ) append(k + 1, -1);
}

shDrop::shDrop(int k) {
  if(k >= 0) {
    append(k, -1); 
  } else {
    append(0, k - 1); 
  }
}

}
