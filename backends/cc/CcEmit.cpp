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
#include <map>
#include <vector>
#include <sstream>
#include "Cc.hpp" 
#include "Debug.hpp" 
#include "Stream.hpp" 
#include "Variant.hpp"
#include "Operation.hpp"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef CC_DEBUG
#  define CC_DEBUG_PRINT(x) DEBUG_PRINT(x)
#else
#  define CC_DEBUG_PRINT(x) do { } while(0)
#endif

namespace Cc {

using namespace SH;

/** @file CcEmit.cpp
 * Implements code emission for a single Statement.  This is a table-driven approach
 * patterned on ArbEmit.cpp in the GL backend.
 */

// @todo type use different functions depending on type
// e.g. std::abs for int types, funcf versions of func for float

// handles linear ops that require up to 4 src arguments (may not be independent) 
struct CcOpCode 
{
  Operation op;
  char *code;
};

struct CcOpCodeVecs
{
  CcOpCodeVecs() {}

  // Dices up the code string into references #i or $i to 
  // src variables and the code fragments between references. 
  //
  // after construction, frag.size() == (index.size() + 1) 
  // and index.size() = scalar.size()
  CcOpCodeVecs(const CcOpCode &op);

  bool operator<(const CcOpCodeVecs &other) {
    return op < other.op;
  }

  std::string encode() const;

  Operation op;

  std::vector<int> index;
  std::vector<bool> scalar;
  std::vector<std::string> frag; 
};


typedef std::map<SH::Operation, CcOpCodeVecs> CcOpCodeMap;

CcOpCodeVecs::CcOpCodeVecs(const CcOpCode &op) {
  std::string code = op.code; 

  std::string::size_type i, j;
  i = j = 0;
  for(; (j = code.find_first_of("#$", i)) != std::string::npos;) {
    frag.push_back(code.substr(i, j - i));
    scalar.push_back(code[j] == '$');
    i = j + 1;
    j = code.find_first_not_of("012345689", i);
    index.push_back(atoi(code.substr(i, j - i).c_str()));
    i = j;
  }
  if(i == std::string::npos) {
    frag.push_back("");
  } else {
    frag.push_back(code.substr(i));
  }

}

std::string CcOpCodeVecs::encode() const 
{
  std::ostringstream out;
  for(unsigned int i = 0; i < index.size(); ++i) {
    out << frag[i];
    out << "src[" << index[i] << "]";
    if(scalar[i]) out << ".scalar";
  }
  out << frag.back();
  return out.str();
}

// Table of replacement macros holding C++ code corresponding to SH op 
//
// This table is parsed into a static map in CcBackendCode.  Although
// it may be possible to classify the ops depending on the kinds of C++
// code output, this is a bit trickier than with the gl backend. Since
// the end result is not assmebly, the variety of syntax makes categorizing
// things more difficult, so here's the easy way out. 
//
// Each entry represents the following macro expansion
// for j = 0 to dest.size() 
//    insert code for resolve(dest, j) = code string in table with 
//        #i in rhs replaced by resolve(src[i], j)
//        $i in rhs replaced by resolve(src[i], src[i].size() == 1 ? 0 : j)
//        where i is an non-negative integer
const CcOpCode opCodeTable[] = {
  {OP_ASN,   "#0" },
  {OP_NEG,   "-#0" },  
  {OP_ADD,   "$0 + $1"},
  {OP_MUL,   "$0 * $1"},
  {OP_DIV,   "$0 / $1"},

  {OP_SLT,   "($0 < $1 ? 1 : 0)"},
  {OP_SLE,   "($0 <= $1 ? 1 : 0)"},
  {OP_SGT,   "($0 > $1 ? 1 : 0)"},
  {OP_SGE,   "($0 >= $1 ? 1 : 0)"},
  {OP_SEQ,   "($0 == $1 ? 1 : 0)"},
  {OP_SNE,   "($0 != $1 ? 1 : 0)"},

  {OP_ABS,   "fabs(#0)"}, 
  {OP_ACOS,  "acos(#0)"},
  {OP_ASIN,  "asin(#0)"},
  {OP_ATAN,  "atan(#0)"},
  {OP_ATAN2, "atan2(#0, #1)"},
#ifdef _WIN32
  {OP_ACOSH, "log(#0 + sqrt(#0 * #0 - 1.0))"},
  {OP_ASINH, "log(#0 + sqrt(#0 * #0 + 1.0))"},
  {OP_ATANH, "log((1.0 + #0)/(1.0 - #0)) / 2.0"},
#else
  {OP_ACOSH, "acosh(#0)"},
  {OP_ASINH, "asinh(#0)"},
  {OP_ATANH, "atanh(#0)"},
#endif
  {OP_CBRT,  "pow(#0, 1 / 3.0)"},
  {OP_CEIL,  "ceil(#0)"},
  {OP_COS,   "cos(#0)"},
  {OP_COSH,  "cosh(#0)"},
  {OP_EXP,   "exp(#0)"},
  {OP_EXP2,  "exp2(#0)"},
  {OP_EXP10, "exp10(#0)"},
  {OP_FLR,   "floor(#0)"},
  {OP_FRAC,  "#0 - floor(#0)"},
  {OP_LOG,   "log(#0)"},
  {OP_LOG2,  "log2(#0)"},
  {OP_LOG10, "log10(#0)"},
  {OP_LRP,   "$0 * ($1 - $2) + $2"},
  {OP_MAD,   "$0 * $1 + $2"},
  {OP_MAX,   "($0 > $1 ? $0 : $1)"},
  {OP_MIN,   "($0 < $1 ? $0 : $1)"}, 
  {OP_MOD,   "($0 - $1 * floor((double)$0 / $1))"},
  {OP_POW,   "pow($0, $1)"},
  {OP_RCP,   "1 / #0"},
  {OP_RND,   "floor(#0 + 0.5)"},
  {OP_RSQ,   "1 / sqrt(#0)"},
  {OP_SIN,   "sin(#0)"},
  {OP_SINH,  "sinh(#0)"},
  {OP_SGN,   "(#0 < 0 ? -1 : (#0 > 0 ? 1 : 0))"},
  {OP_SQRT,  "sqrt(#0)"},
  {OP_TAN,   "tan(#0)"},
  {OP_TANH,  "tanh(#0)"},
  {OP_COND,  "($0 > 0 ? $1 : $2)"},
  {OP_FETCH, "#0"},

  {OPERATION_END,  0} 
  // @todo LO, HI, SETLO, SETHI
};

// @todo type these are still implemented in the switch statement below
// fix them later or maybe just leave them 
#if 0
  {OP_CMUL,             
  {OP_CSUM,             
  {OP_DOT,             
  {OP_NORM,             
  {OP_XPD,              
  {OP_TEX,              
  {OP_TEXI,             
  {OP_TEXD,             
  {OP_KIL,              
  {OP_OPTBRA,           
#endif

// @todo type implement emit
void CcBackendCode::emit(const Statement& stmt) {
  static CcOpCodeMap opcodeMap;

  // @todo type should really move this to the CcBackendCode constructor 
  // @todo type should handle other types properly
  
  // fill in opcodeMap from the above table
  if(opcodeMap.empty()) {
    CC_DEBUG_PRINT("Operation -> C++ code mappings");
    for(int i = 0; opCodeTable[i].op != OPERATION_END; ++i) {

      opcodeMap[opCodeTable[i].op] = CcOpCodeVecs(opCodeTable[i]); 
      CC_DEBUG_PRINT(opInfo[opCodeTable[i].op].name << " -> " 
          << opcodeMap[opCodeTable[i].op].encode());
    }
  }

  // output SH intermediate m_code for reference
  m_code << "  // " << stmt << std::endl;

  // generate C m_code from statement

  // @todo get rid of warnings for assignment of different types 
  // (e.g. when float to int cast required)

  // handle ops in the table first 
  if(opcodeMap.find(stmt.op) != opcodeMap.end()) {
    CcOpCodeVecs codeVecs = opcodeMap[stmt.op]; 
    for(int i = 0; i < stmt.dest.size(); ++i) {
      m_code << "  " << resolve(stmt.dest, i) << " = (" 
        << ctype(stmt.dest.valueType()) << ")(";
      unsigned int j;
      for(j = 0; j < codeVecs.index.size(); ++j) { 
        const Variable& src = stmt.src[codeVecs.index[j]];
        m_code << codeVecs.frag[j];
        if(codeVecs.scalar[j]) {
          m_code << resolve(src, src.size() > 1 ? i : 0); 
        } else {
          m_code << resolve(src, i); 
        }
      }
      m_code << codeVecs.frag[j] << ");" << std::endl;
    }
    return;
  }

  // handle remaining ops with some custom code
  // @todo improve collecting ops
  switch(stmt.op) {
    case OP_DOT:
      {
        DEBUG_ASSERT(stmt.dest.size() == 1);
        m_code << "  " << resolve(stmt.dest, 0) << " = " 
          << resolve(stmt.src[0], 0) 
          << " * "
          << resolve(stmt.src[1], 0)
          << ";" << std::endl;

        int inc0 = stmt.src[0].size() == 1 ? 0 : 1;
        int inc1 = stmt.src[1].size() == 1 ? 0 : 1;
        int size = std::max(stmt.src[0].size(), stmt.src[1].size());

        int i, s0, s1;
        for(i = s0 = s1 = 1; i < size; ++i, s0 += inc0, s1 += inc1) {
          m_code << "  " << resolve(stmt.dest, 0) << " += " 
            << resolve(stmt.src[0], s0) 
            << " * "
            << resolve(stmt.src[1], s1)
            << ";" << std::endl;
        }
        break;
      }

    case OP_CSUM:
      {
        DEBUG_ASSERT(stmt.dest.size() == 1);
        m_code << "  " << resolve(stmt.dest, 0) << " = " 
          << resolve(stmt.src[0], 0) 
          << ";" << std::endl;

        int size = stmt.src[0].size();
        for(int i = 1; i < size; ++i) {
          m_code << "  " << resolve(stmt.dest, 0) << " += " 
            << resolve(stmt.src[0], i) 
            << ";" << std::endl;
        }
        break;
      }

    case OP_CMUL:
      {
        DEBUG_ASSERT(stmt.dest.size() == 1);
        m_code << "  " << resolve(stmt.dest, 0) << " = " 
          << resolve(stmt.src[0], 0) 
          << ";" << std::endl;

        int size = stmt.src[0].size();
        for(int i = 1; i < size; ++i) {
          m_code << "  " << resolve(stmt.dest, 0) << " *= " 
            << resolve(stmt.src[0], i) 
            << ";" << std::endl;
        }
        break;
      }

    case OP_LIT:
      {
        m_code << "  {" << std::endl;
	
	// Clamp to zero the first two arguments
	m_code << "    " << resolve(stmt.src[0], 0) << " = (" 
	       << resolve(stmt.src[0], 0) << " > 0) ? " 
	       << resolve(stmt.src[0], 0) << " : 0;" << std::endl;
	m_code << "    " << resolve(stmt.src[0], 1) << " = (" 
	       << resolve(stmt.src[0], 1) << " > 0) ? " 
	       << resolve(stmt.src[0], 1) << " : 0;" << std::endl;

	// Clamp to -128, 128 the third argument
	m_code << "    " << resolve(stmt.src[0], 2) << " = (" 
	       << resolve(stmt.src[0], 2) << " > -128.0) ? " 
	       << resolve(stmt.src[0], 2) << " : -128.0;" << std::endl;
	m_code << "    " << resolve(stmt.src[0], 2) << " = (" 
	       << resolve(stmt.src[0], 2) << " < 128.0) ? " 
	       << resolve(stmt.src[0], 2) << " : 128.0;" << std::endl;

	// Result according to OpenGL spec
	m_code << "    " << resolve(stmt.dest, 0) << " = 1;" << std::endl;
	m_code << "    " << resolve(stmt.dest, 1) << " = "
	       << resolve(stmt.src[0], 0) << ";" << std::endl;
	m_code << "    " << resolve(stmt.dest, 2) << " = (" 
	       << resolve(stmt.src[0], 0) << " > 0)" << " * pow(" 
	       << resolve(stmt.src[0], 1) << ", " << resolve(stmt.src[0], 2)
	       << ");" << std::endl;
	m_code << "    " << resolve(stmt.dest, 3) << " = 1;" << std::endl;

	m_code << "  }" << std::endl;
	break;
      }

    case OP_NORM:
      {
        m_code << "  {" << std::endl;
        m_code << "    float len = 1.0/sqrt(";
        for(int i = 0; i < stmt.dest.size(); i++)
        {
          if (i != 0) m_code << " + ";
          m_code << resolve(stmt.src[0], i)
           << " * "
           << resolve(stmt.src[0], i);
        }
        m_code << ");" << std::endl;
        for(int i = 0; i < stmt.dest.size(); i++)
        {
          m_code << "    "
           << resolve(stmt.dest, i)
           << " = len*"
           << resolve(stmt.src[0], i)
           << ";" << std::endl;
        }
        m_code << "  }" << std::endl;

        break;
      }
    case OP_XPD:
      {
        for(int i = 0; i < stmt.dest.size(); i++)
        {
          int i0 = (i+1)%3;
          int i1 = (i+2)%3;
          m_code << "  "
           << resolve(stmt.dest, i)
           << " = "
           << resolve(stmt.src[0], i0)
           << " * "
           << resolve(stmt.src[1], i1)
           << " - "
           << resolve(stmt.src[1], i0)
           << " * "
           << resolve(stmt.src[0], i1)
           << ";" << std::endl;
        }

        break;
      }
    case OP_TEX:
      emitTexLookup(stmt, "sh_cc_backend_lookup");
      break;

    case OP_TEXI:
      emitTexLookup(stmt, "sh_cc_backend_lookupi");
      break;

    case OP_KIL:
      {
      // TODO: maintain prior output values 
      m_code << "  if (";
      for(int i = 0; i < stmt.src[0].size(); i++)
      {
        if (i != 0) m_code << " || ";
        m_code << "("
         << resolve(stmt.src[0], i) 
         << " > 0)";
      }
      m_code << ")" << std::endl;
      m_code << "    return;" << std::endl;
      break;
      }
    case OP_OPTBRA:
      {
        DEBUG_ASSERT(false);
        break;
      }
    default:
      {
        // TODO: once FETCH is implemented, we should be able to
        // return an error here and not break the unit tests.

        // std::stringstream s;
        // s << "CC Code: Unknown operation " << opInfo[stmt.op].name;
        // error(s.str());
        // break;

        m_code << "  // *** unhandled operation " << opInfo[stmt.op].name
               << " ***" << std::endl;
        break;
      }
  }
}

void CcBackendCode::emitTexLookup(const Statement& stmt, const char* texfunc) {
  TextureNodePtr node = shref_dynamic_cast<TextureNode>(stmt.src[0].node());
  int dims = 0; 
  switch(node->dims()) {
    case SH_TEXTURE_1D: dims = 1; break;   
    case SH_TEXTURE_2D: dims = 2; break; 
    case SH_TEXTURE_RECT: dims = 2; break;
    case SH_TEXTURE_3D: dims = 3; break; 
    case SH_TEXTURE_CUBE: 
      DEBUG_ERROR("Cube maps not handled"); 
    default:
      DEBUG_ERROR("Unhandled texture dim");
  }

  // names of the functors to use for different texture lookup types 
  std::string srcInterp, srcFilter, srcWrap;

  if(node->traits().interpolation() != 0) {
      //error(BackendException("cc backend supports only nearest-neighbour texture lookup."));
      //DEBUG_WARN("cc backend supports only nearest-neighbour texture lookup.");
  }

  if (node->traits().filtering() != TextureTraits::FILTER_NONE) {
      //error(BackendException("cc backend does not support texture filtering."));
      DEBUG_WARN("cc backend does not support texture filtering.");
  }

  switch(node->traits().wrapping()) {
    case TextureTraits::WRAP_CLAMP:
    case TextureTraits::WRAP_CLAMP_TO_EDGE:
      srcWrap = "sh_gcc_backend_wrap_clamp";
      break;
    case TextureTraits::WRAP_REPEAT:
      srcWrap = "sh_gcc_backend_wrap_repeat";
      break;
    default:
      error(BackendException("cc backend does not support requested texture wrapping mode."));
      break;
  }

  m_code << "  {" << std::endl;
  std::string destvar; 
  std::string srcvar;
  bool tempdest = !stmt.dest.swizzle().identity();
  bool tempsrc = (!stmt.src[1].swizzle().identity()) || stmt.src[1].neg();

  if(tempdest) {
    m_code << "    " << ctype(stmt.dest.valueType()) <<  
      " result[" << stmt.dest.size() << "];" << std::endl;
    destvar = "result";
  } else destvar = resolve(stmt.dest);

  if(tempsrc) {
    m_code << "    " << ctype(stmt.src[1].valueType()) 
      << " input[" << stmt.src[1].size() << "];" << std::endl;

    for(int i = 0; i < stmt.src[1].size(); i++) {
      m_code << "    input[" << i << "] = " 
        << resolve(stmt.src[1], i) << ";" << std::endl;
    }

    srcvar = "input";
  } else srcvar = resolve(stmt.src[1]);

  m_code << "    " << texfunc << "<"
     << dims << ", "
     << node->size() << ", "
     << node->width() << ", "
     << node->height() << ", "
     << node->depth() <<  ", "
     << ctype(node->valueType()) << "," 
     << srcWrap << ">("
   << resolve(stmt.src[0])
   << ", "
   << srcvar
   << ", "
   << destvar
   << ");" << std::endl;
  
  if(tempdest) {
    for(int i = 0; i < stmt.dest.size(); i++) {
      m_code << "    "
       << resolve(stmt.dest, i)
       << " = result[" << i << "];" << std::endl;
    }
  }
  m_code << "  }" << std::endl;
}

}

