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
#include "ShAti.hpp"
#include <iostream>
#include <sstream>

//TODO remove
#ifdef WIN32

#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include "ShArrayData.hpp"

//TODO uber buffer stuff

PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = NULL;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = NULL;

#else


//#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
//#include <GL/glext.h>
#include <GL/glx.h>
//#undef GL_GLEXT_PROTOTYPES
//
#include "glati.h"
#include "extensions.h"

#endif /* WIN32 */

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShEnvironment.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"

// TODO replace with proper error handling
#define CHECK_GL_ERROR() { if( int error = glGetError() ) { SH_DEBUG_WARN( "Unexpected GL error: " << error ); } }
#define PRINT_GL_ERROR( message ) { \
  int error = glGetError();\
  if (error != GL_NO_ERROR) {\
    SH_DEBUG_ERROR("Error description: " << message << " error: " << error << " " );\
    switch(error) {\
    case GL_INVALID_ENUM:\
      SH_DEBUG_ERROR("INVALID_ENUM");\
      break;\
    case GL_INVALID_VALUE:\
      SH_DEBUG_ERROR("INVALID_VALUE");\
      break;\
    case GL_INVALID_OPERATION:\
      SH_DEBUG_ERROR("INVALID_OPERATION");\
      break;\
    }\
  } \
}


namespace ShAti {

static SH::ShRefCount<ShAti::AtiBackend> instance = new ShAti::AtiBackend();
  
const unsigned int shGlCubeMapTargets[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

const unsigned int shGlTextureType[] = {
  GL_TEXTURE_1D,
  GL_TEXTURE_2D,
  GL_TEXTURE_3D,
  GL_TEXTURE_CUBE_MAP,
};

using namespace SH;

#define shGlProgramStringARB glProgramStringARB
#define shGlActiveTextureARB glActiveTextureARB
#define shGlProgramLocalParameter4fvARB glProgramLocalParameter4fvARB
#define shGlProgramEnvParameter4fvARB glProgramEnvParameter4fvARB
#define shGlGetProgramivARB glGetProgramivARB
#define shGlGenProgramsARB glGenProgramsARB
#define shGlDeleteProgramsARB glDeleteProgramsARB
#define shGlBindProgramARB glBindProgramARB

unsigned int shAtiTargets[] = {
  GL_VERTEX_PROGRAM_ARB,
  GL_FRAGMENT_PROGRAM_ARB
};

/** All the possible operations in the ARB spec.
 */
enum AtiOp {
  // VERTEX AND FRAGMENT
  
  // Vector
  SH_ATI_ABS,
  SH_ATI_FLR,
  SH_ATI_FRC,
  SH_ATI_LIT,
  SH_ATI_MOV,

  // Scalar
  SH_ATI_EX2,
  SH_ATI_LG2,
  SH_ATI_RCP,
  SH_ATI_RSQ,

  // Binary scalar
  SH_ATI_POW,

  // Binary vector
  SH_ATI_ADD,
  SH_ATI_DP3,
  SH_ATI_DP4,
  SH_ATI_DPH,
  SH_ATI_DST,
  SH_ATI_MAX,
  SH_ATI_MIN,
  SH_ATI_MUL,
  SH_ATI_SGE,
  SH_ATI_SLT,
  SH_ATI_SUB,
  SH_ATI_XPD,

  // Trinary
  SH_ATI_MAD,

  // Swizzling
  SH_ATI_SWZ,

  // VERTEX ONLY
  // Scalar
  SH_ATI_EXP,
  SH_ATI_LOG,
  
  // FRAGMENT ONLY
  // Scalar
  SH_ATI_COS,
  SH_ATI_SIN,
  SH_ATI_SCS,

  // Trinary
  SH_ATI_CMP,
  SH_ATI_LRP,

  // Sampling
  SH_ATI_TEX,
  SH_ATI_TXP,
  SH_ATI_TXB,

  // KIL
  SH_ATI_KIL
};

/** Information about the operations from AtiOp.
 */
static struct {
  char* name;
  bool vp, fp;
  int arity;
  bool vector;
  bool collectingOp;
} arbOpInfo[] = {
  // VERTEX AND FRAGMENT
  // Vector
  {"ABS", true, true, 1, true, false},
  {"FLR", true, true, 1, true, false},
  {"FRC", true, true, 1, true, false},
  {"LIT", true, true, 1, true, false},
  {"MOV", true, true, 1, true, false},

  // Scalar
  {"EX2", true, true, 1, false, false},
  {"LG2", true, true, 1, false, false},
  {"RCP", true, true, 1, false, false},
  {"RSQ", true, true, 1, false, false},

  // Binary scalar
  {"POW", true, true, 2, false, false},

  // Binary vector
  {"ADD", true, true, 2, true, false},
  {"DP3", true, true, 2, true, true},
  {"DP4", true, true, 2, true, true},
  {"DPH", true, true, 2, true, true},
  {"DST", true, true, 2, true, true},
  {"MAX", true, true, 2, true, false},
  {"MIN", true, true, 2, true, false},
  {"MUL", true, true, 2, true, false},
  {"SGE", true, true, 2, true, false},
  {"SLT", true, true, 2, true, false},
  {"SUB", true, true, 2, true, false},
  {"XPD", true, true, 2, true, true},

  // Trinary
  {"MAD", true, true, 3, true, false},

  // Swizzling
  {"SWZ", true, true, 2, true, true}, // should this really be a collectingOp?

  // VERTEX ONLY
  // Scalar
  {"EXP", true, false, 2, false, false},
  {"LOG", true, false, 2, false, false},
  
  // FRAGMENT ONLY
  // Scalar
  {"COS", false, true, 1, false, false},
  {"SIN", false, true, 1, false, false},
  {"SCS", false, true, 1, false, false},

  // Trinary
  {"CMP", false, true, 3, true, false},
  {"LRP", false, true, 3, true, false},

  // Sampling
  {"TEX", false, true, 3, true, false},
  {"TXP", false, true, 3, true, false},
  {"TXB", false, true, 3, true, false},

  // KIL
  {"KIL", false, true, 0, false, false}
};

/** An ARB instruction.
 */
struct AtiInst {
  AtiInst(AtiOp op, const ShVariable& dest)
    : op(op), dest(dest)
  {
  }

  AtiInst(AtiOp op, const ShVariable& dest, const ShVariable& src0)
    : op(op), dest(dest)
  {
    src[0] = src0;
  }

  AtiInst(AtiOp op, const ShVariable& dest, const ShVariable& src0,
          const ShVariable& src1)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
  }
  AtiInst(AtiOp op, const ShVariable& dest, const ShVariable& src0,
          const ShVariable& src1, const ShVariable& src2)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  
  AtiOp op;
  ShVariable dest;
  ShVariable src[3];
};

/** Information about ShAtiRegType members.
 */
struct {
  char* name;
  char* estName;
} shAtiRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Possible bindings for a register (see ARB spec).
 */
enum ShAtiRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ATI_REG_PARAMLOC,
  SH_ATI_REG_PARAMENV,
  // Output
  SH_ATI_REG_RESULTCOL,

  // VERTEX
  // Input
  SH_ATI_REG_VERTEXPOS,
  SH_ATI_REG_VERTEXWGT,
  SH_ATI_REG_VERTEXNRM,
  SH_ATI_REG_VERTEXCOL,
  SH_ATI_REG_VERTEXFOG,
  SH_ATI_REG_VERTEXTEX,
  SH_ATI_REG_VERTEXMAT,
  SH_ATI_REG_VERTEXATR,
  // Output
  SH_ATI_REG_RESULTPOS,
  SH_ATI_REG_RESULTFOG,
  SH_ATI_REG_RESULTPTS, ///< Result point size
  SH_ATI_REG_RESULTTEX,

  // FRAGMENT
  // Input
  SH_ATI_REG_FRAGMENTCOL,
  SH_ATI_REG_FRAGMENTTEX,
  SH_ATI_REG_FRAGMENTFOG,
  SH_ATI_REG_FRAGMENTPOS,
  // Output
  SH_ATI_REG_RESULTDPT,

  SH_ATI_REG_NONE
};

/** Information about the ShAtiRegBinding members.
 */
struct {
  ShAtiRegType type;
  char* name;
  bool indexed;
} shAtiRegBindingInfo[] = {
  {SH_ATI_REG_PARAM, "program.local", true},
  {SH_ATI_REG_PARAM, "program.env", true},
  {SH_ATI_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {SH_ATI_REG_ATTRIB, "vertex.position", false},
  {SH_ATI_REG_ATTRIB, "vertex.weight", true},
  {SH_ATI_REG_ATTRIB, "vertex.normal", false},
  {SH_ATI_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {SH_ATI_REG_ATTRIB, "vertex.fogcoord", false},
  {SH_ATI_REG_ATTRIB, "vertex.texcoord", true},
  {SH_ATI_REG_ATTRIB, "vertex.matrixindex", true},
  {SH_ATI_REG_ATTRIB, "vertex.attrib", true},
  {SH_ATI_REG_OUTPUT, "result.position", false},
  {SH_ATI_REG_OUTPUT, "result.fogcoord", false},
  {SH_ATI_REG_OUTPUT, "result.pointsize", false},
  {SH_ATI_REG_OUTPUT, "result.texcoord", true},

  {SH_ATI_REG_ATTRIB, "fragment.color", false}, // TODO: Special case?
  {SH_ATI_REG_ATTRIB, "fragment.texcoord", true},
  {SH_ATI_REG_ATTRIB, "fragment.fogcoord", false},
  {SH_ATI_REG_ATTRIB, "fragment.position", false},
  {SH_ATI_REG_OUTPUT, "result.depth", false},

  {SH_ATI_REG_ATTRIB, "<nil>", false},
};

struct AtiBindingSpecs {
  ShAtiRegBinding binding;
  int maxBindings;
  ShVariableSpecialType specialType;
  bool allowGeneric;
};

AtiBindingSpecs shAtiVertexAttribBindingSpecs[] = {
  {SH_ATI_REG_VERTEXPOS, 1, SH_VAR_POSITION, false},
  {SH_ATI_REG_VERTEXNRM, 1, SH_VAR_NORMAL, false},
  {SH_ATI_REG_VERTEXCOL, 1, SH_VAR_COLOR, false},
  {SH_ATI_REG_VERTEXTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ATI_REG_VERTEXFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ATI_REG_NONE, 0, SH_VAR_ATTRIB, true}
};

AtiBindingSpecs shAtiFragmentAttribBindingSpecs[] = {
  {SH_ATI_REG_FRAGMENTPOS, 1, SH_VAR_POSITION, false},
  {SH_ATI_REG_FRAGMENTCOL, 1, SH_VAR_COLOR, false},
  {SH_ATI_REG_FRAGMENTTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ATI_REG_FRAGMENTFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ATI_REG_NONE, 0, SH_VAR_ATTRIB, true}
};

AtiBindingSpecs* shAtiAttribBindingSpecs[] = {
  shAtiVertexAttribBindingSpecs,
  shAtiFragmentAttribBindingSpecs
};

AtiBindingSpecs shAtiVertexOutputBindingSpecs[] = {
  {SH_ATI_REG_RESULTPOS, 1, SH_VAR_POSITION, false},
  {SH_ATI_REG_RESULTCOL, 1, SH_VAR_COLOR, false},
  {SH_ATI_REG_RESULTTEX, 8, SH_VAR_TEXCOORD, true},
  {SH_ATI_REG_RESULTFOG, 1, SH_VAR_ATTRIB, true},
  {SH_ATI_REG_RESULTPTS, 1, SH_VAR_ATTRIB, true},
  {SH_ATI_REG_NONE, 0, SH_VAR_ATTRIB}
};

AtiBindingSpecs shAtiFragmentOutputBindingSpecs[] = {
  {SH_ATI_REG_RESULTCOL, 1, SH_VAR_COLOR, false},
  {SH_ATI_REG_RESULTDPT, 1, SH_VAR_ATTRIB, false},
  {SH_ATI_REG_NONE, 0, SH_VAR_ATTRIB}
};

AtiBindingSpecs* shAtiOutputBindingSpecs[] = {
  shAtiVertexOutputBindingSpecs,
  shAtiFragmentOutputBindingSpecs
};

/** An ARB register.
 */
struct AtiReg {
  AtiReg()
    : type(SH_ATI_REG_TEMP), index(-1), binding(SH_ATI_REG_NONE), bindingIndex(-1)
  {
  }
  
  AtiReg(ShAtiRegType type, int index)
    : type(type), index(index), binding(SH_ATI_REG_NONE), bindingIndex(-1)
  {
  }

  ShAtiRegType type;
  int index;

  union {
    struct {
      ShAtiRegBinding binding;
      int bindingIndex;
    };
    float values[4];
  };

  friend std::ostream& operator<<(std::ostream& out, const AtiReg& reg);

  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const
  {
    out << shAtiRegTypeInfo[type].name << " " << *this;
    if (type == SH_ATI_REG_CONST) {
      out << " = " << "{";
      for (int i = 0; i < 4; i++) {
        if (i) out << ", ";
        out << values[i];
      }
      out << "}";
    } else if (binding != SH_ATI_REG_NONE) {
      out << " = " << shAtiRegBindingInfo[binding].name;
      if (shAtiRegBindingInfo[binding].indexed) {
        out << "[" << bindingIndex << "]";
      }
    }
    out << ";";
    return out;
  }
};

/** Output a use of an arb register.
 */
std::ostream& operator<<(std::ostream& out, const AtiReg& reg)
{
  out << shAtiRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

using namespace SH;

AtiCode::AtiCode(AtiBackendPtr backend, const ShProgram& shader, int kind)
  : m_backend(backend), m_shader(shader), m_kind(kind),
    m_numTemps(0), m_numInputs(0), m_numOutputs(0), m_numParams(0), m_numConsts(0),
    m_numTextures(0), m_programId(0)
{
}

AtiCode::~AtiCode()
{
}

void AtiCode::generate()
{
  m_shader->ctrlGraph->entry()->clearMarked();
  genNode(m_shader->ctrlGraph->entry());
  m_shader->ctrlGraph->entry()->clearMarked();
  allocRegs();
}

bool AtiCode::allocateRegister(const ShVariableNodePtr& var)
{
  if (!var) return true;
  if (var->kind() != SH_VAR_TEMP) return true;
  if (var->uniform()) return true;

  if (m_tempRegs.empty()) {
    SH_DEBUG_ERROR("Out of temporaries!");
    return false;
  }

  int idx = m_tempRegs.front();
  m_tempRegs.pop_front();
  if (idx + 1 > m_numTemps) m_numTemps = idx + 1;
  m_registers[var] = AtiReg(SH_ATI_REG_TEMP, idx);
  
  return true;
}

void AtiCode::freeRegister(const ShVariableNodePtr& var)
{
  if (!var) return;
  if (var->kind() != SH_VAR_TEMP) return;
  if (var->uniform()) return;

  SH_DEBUG_ASSERT(m_registers.find(var) != m_registers.end());
  m_tempRegs.push_front(m_registers[var].index);
}

void AtiCode::upload()
{
  /// clean-up previous errors
  int error;
  if( error = glGetError() ) SH_DEBUG_WARN( "Unexpected GL error" << error  );

  if (!m_programId)
    shGlGenProgramsARB(1, &m_programId);

  shGlBindProgramARB(shAtiTargets[m_kind], m_programId);
  
  std::ostringstream out;
  print(out);
  std::string text = out.str();
  shGlProgramStringARB(shAtiTargets[m_kind], GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)text.size(), text.c_str());
  error = glGetError();
  if (error == GL_INVALID_OPERATION) {
    int pos = -1;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos);
    const unsigned char* message = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
    SH_DEBUG_WARN("Error at character " << pos);
    SH_DEBUG_WARN("Message: " << message);
    if( pos >= 0 ) {
      SH_DEBUG_WARN("Code (30 chars): " << text.substr(pos, 30));
    }
  }
  if (error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Error uploading ARB program: " << error);
  }
}

void AtiCode::bind()
{
  if (!m_programId) {
    upload();
  }
  
  shGlBindProgramARB(shAtiTargets[m_kind], m_programId);
  
  SH::ShEnvironment::boundShader[m_kind] = m_shader;

  // Initialize constants
  for (RegMap::const_iterator I = m_registers.begin(); I != m_registers.end(); ++I) {
    ShVariableNodePtr node = I->first;
    AtiReg reg = I->second;
    if (node->hasValues() && reg.type == SH_ATI_REG_PARAM) {
      updateUniform(node);
    }
  }
  for (ShProgramNode::VarList::const_iterator I = m_shader->textures.begin(); I != m_shader->textures.end();
       ++I) {
    loadTexture(*I);
  }
}

void AtiCode::loadTexture(ShTextureNodePtr texture) 
{
  if (!texture) return;
  
  RegMap::const_iterator texRegIterator = m_registers.find(texture);
  
  SH_DEBUG_ASSERT(texRegIterator != m_registers.end());
  
  const AtiReg& texReg = texRegIterator->second;
  
  shGlActiveTextureARB(GL_TEXTURE0 + texReg.index);
  
  glBindTexture(shGlTextureType[texture->dims()], texReg.bindingIndex);
  
  if (1) {
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  if (1) {
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
    
  ShDataTextureNodePtr datatex = texture;
  if (datatex) {
    loadDataTexture(datatex);
    return;
  }

  ShCubeTextureNodePtr cubetex = texture;
  if (cubetex) {
    loadCubeTexture(cubetex);
    return;
  }
    
  SH_DEBUG_WARN(texture->name() << " is not a valid texture!");
}

void AtiCode::loadDataTexture(ShDataTextureNodePtr texture, unsigned int type)
{

  /// reset the error OpenGL counter
  int error;
  if( error = glGetError() ) SH_DEBUG_WARN( "Unexpected GL error" << error );

  if (!type) {
    switch (texture->dims()) {
    case SH_TEXTURE_1D:
      type = GL_TEXTURE_1D;
      break;
    case SH_TEXTURE_2D:
      type = GL_TEXTURE_2D;
      break;
    case SH_TEXTURE_3D:
      type = GL_TEXTURE_3D;
      break;
    default:
      SH_DEBUG_ERROR("No type specified and no known type for this texture.");
      break;
    }
  }

  // TODO: Other types of textures.
  // TODO: Element Format
  // TODO: sampling/filtering
  // TODO: wrap/clamp
  unsigned int internalFormat;
  unsigned int format;


  // TODO handle 1D/3D for uber buffer attaches
  ShDataMemoryObjectPtr dataMem = texture->mem();
  if( dataMem ) { // 
    glAttachMemATI( GL_TEXTURE_2D, 0 );

    //TODO fix this hard-coded float stuff
    switch (texture->elements()) {
    case 1:
      format = GL_LUMINANCE;
      internalFormat = GL_LUMINANCE_FLOAT32_ATI;
      break;
    case 2:
      format = GL_LUMINANCE_ALPHA;
      internalFormat = GL_LUMINANCE_ALPHA_FLOAT32_ATI;
      break;
    case 3:
      format = GL_RGB;
      internalFormat = GL_RGB_FLOAT32_ATI;
      break;
    case 4:
      format = GL_RGBA;
      internalFormat = GL_RGBA_FLOAT32_ATI;
      break;
    default:
      format = 0;
      break;
    }

    switch(type) {
    case GL_TEXTURE_1D:
      glTexImage1D(type, 0, internalFormat, texture->width(), 0, format, GL_FLOAT, dataMem->data());
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
      glTexImage2D(type, 0, internalFormat, texture->width(), texture->height(), 0, format, GL_FLOAT,
                   dataMem->data());
      break;
    case GL_TEXTURE_3D:
      glTexImage3D(type, 0, internalFormat, texture->width(), texture->height(), texture->depth(),
                   0, format, GL_FLOAT, dataMem->data());
      break;
    default:
      SH_DEBUG_WARN("Texture type not handled by ARB backend");
      break;
    }
  } else {
    ShUberbufferPtr ub = texture->mem(); 
    if( ub ) {
      m_backend->allocUberbuffer( ub );
      glAttachMemATI( GL_TEXTURE_2D, ub->mem() );

      m_backend->printUbErrors();
    } else {
      glAttachMemATI( GL_TEXTURE_2D, 0 );
      m_backend->printUbErrors();
    }
  } 

  error = glGetError();
  if (error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Error loading texture: " << error);
    switch(error) {
    case GL_INVALID_ENUM:
      SH_DEBUG_ERROR("INVALID_ENUM");
      break;
    case GL_INVALID_VALUE:
      SH_DEBUG_ERROR("INVALID_VALUE");
      break;
    case GL_INVALID_OPERATION:
      SH_DEBUG_ERROR("INVALID_OPERATION");
      break;
    }
  }
}

void AtiCode::loadCubeTexture(ShCubeTextureNodePtr cube)
{
  for (int i = 0; i < 6; i++) {
    loadDataTexture(cube->face(static_cast<ShCubeDirection>(i)), shGlCubeMapTargets[i]);
  }
}

void AtiCode::updateUniform(const ShVariableNodePtr& uniform)
{
  if (!uniform) return;

  RegMap::const_iterator I = m_registers.find(uniform);
  if (I == m_registers.end()) return;

  ShTextureNodePtr tex = uniform;
  if (tex) {
    loadTexture(tex);
    return;
  }
    
  const AtiReg& reg = I->second;
  
  float values[4];
  int i;
  for (i = 0; i < uniform->size(); i++) {
    values[i] = (float)uniform->getValue(i);
  }
  for (; i < 4; i++) {
    values[i] = 0.0;
  }
  
  if (reg.type != SH_ATI_REG_PARAM) return;
  switch(reg.binding) {
  case SH_ATI_REG_PARAMLOC:
    shGlProgramLocalParameter4fvARB(shAtiTargets[m_kind], reg.bindingIndex, values);
    break;
  case SH_ATI_REG_PARAMENV:
    shGlProgramEnvParameter4fvARB(shAtiTargets[m_kind], reg.bindingIndex, values);
    break;
  default:
    return;
  }
}

std::ostream& AtiCode::printVar(std::ostream& out, bool dest, const ShVariable& var,
                                bool collectingOp, const ShSwizzle& destSwiz = ShSwizzle(4)) const
{
  RegMap::const_iterator I = m_registers.find(var.node());
  if (I == m_registers.end()) {
    out << "<no reg for " << var.name() << ">";
    return out;
  }
  const AtiReg& reg = I->second;

  // Negation
  if (var.neg()) out << '-';

  // Register name
  out << reg;

  // Swizzling
  const char* swizChars = "xyzw";
  out << ".";
  if (dest || var.swizzle().size() == 1) {
    for (int i = 0; i < std::min(var.swizzle().size(), 4); i++) {
       out << swizChars[var.swizzle()[i]];
    }
  } else if (collectingOp) {
    for (int i = 0; i < 4; i++) {
       out << swizChars[i < var.swizzle().size() ? var.swizzle()[i] : i];
    }
  } else {
    for (int i = 0; i < 4; i++) {
      int j;
      for (j = 0; j < destSwiz.size(); j++) {
        if (destSwiz[j] == i) break;
      }
      if (j == destSwiz.size()) j = i;
      out << swizChars[j < var.size() ? var.swizzle()[j] : j];
    }
  }

  return out;
}

struct LineNumberer {
  LineNumberer() { line = 0; }
  int line;
};

std::ostream& operator<<(std::ostream& out, LineNumberer& l)
{
  out << " # " << ++l.line << std::endl;
  return out;
}

bool AtiCode::printSamplingInstruction(std::ostream& out, const AtiInst& instr) const
{
  if (instr.op != SH_ATI_TEX && instr.op != SH_ATI_TXP && instr.op != SH_ATI_TXB)
    return false;

  ShTextureNodePtr texture = instr.src[1].node();
  RegMap::const_iterator texRegIt = m_registers.find(instr.src[1].node());
  SH_DEBUG_ASSERT(texRegIt != m_registers.end());

  const AtiReg& texReg = texRegIt->second;
  
  out << "  ";
  out << arbOpInfo[instr.op].name << " ";
  printVar(out, true, instr.dest, false) << ", ";
  printVar(out, false, instr.src[0], true, instr.dest.swizzle()) << ", ";
  out << "texture[" << texReg.index << "], ";
  switch (texture->dims()) {
  case SH_TEXTURE_1D:
    out << "1D";
    break;
  case SH_TEXTURE_2D:
    out << "2D"; // TODO: RECT
    break;
  case SH_TEXTURE_3D:
    out << "3D";
    break;
  case SH_TEXTURE_CUBE:
    out << "CUBE";
    break;
  }
  out << ";" << std::endl;
  return true;
}

std::ostream& AtiCode::print(std::ostream& out)
{
  LineNumberer endl;

  // Print version header
  switch(m_kind) {
  case 0:
    out << "!!ARBvp1.0" << endl;
    break;
  case 1:
    out << "!!ARBfp1.0" << endl;
    break;
  }

  // Print register declarations
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    out << "  ";
    m_registers[*I].printDecl(out);
    out << endl;
  }

  if (m_numTemps) {
    out << "  TEMP ";
    for (int i = 0; i < m_numTemps; i++) {
      if (i > 0) out << ", ";
      out << AtiReg(SH_ATI_REG_TEMP, i);
    }
    out << ";" << endl;
  }

  out << endl;
  
  // Print instructions
  for (AtiInstList::const_iterator I = m_instructions.begin();
       I != m_instructions.end(); ++I) {
    if (!printSamplingInstruction(out, *I)) {
      out << "  ";
      out << arbOpInfo[I->op].name << " ";
      printVar(out, true, I->dest, arbOpInfo[I->op].collectingOp);
      for (int i = 0; i < arbOpInfo[I->op].arity; i++) {
        out << ", ";
        printVar(out, false, I->src[i], arbOpInfo[I->op].collectingOp, I->dest.swizzle());
      }
      out << ';';
      out << endl;
    }
  }

  out << "END" << endl;
  return out;
}


void AtiCode::genNode(ShCtrlGraphNodePtr node)
{
  if (!node || node->marked()) return;
  node->mark();

  if (node->block) for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    const ShStatement& stmt = *I;
    switch (stmt.op) {
    case SH_OP_ASN:
      m_instructions.push_back(AtiInst(SH_ATI_MOV, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_NEG:
      m_instructions.push_back(AtiInst(SH_ATI_MOV, stmt.dest, -stmt.src[0]));
      break;
    case SH_OP_ADD:
      m_instructions.push_back(AtiInst(SH_ATI_ADD, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_MUL:
      if (stmt.src[0].size() != 1 || stmt.src[1].size() != 1) {
        if (stmt.src[0].size() == 1) {
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0; 
          m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, 
                stmt.src[0](stmt.src[1].size(), swizzle), stmt.src[1]));
          delete [] swizzle;
          break;
        } else if (stmt.src[1].size() == 1) {
          int* swizzle = new int[stmt.src[0].size()];
          for (int i = 0; i < stmt.src[0].size(); i++) swizzle[i] = 0;
          m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, stmt.src[0],
                                                 stmt.src[1](stmt.src[0].size(), swizzle)));
          delete [] swizzle;
          break;
        }
      } 
      m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, stmt.src[0], stmt.src[1])); 
      break;
    case SH_OP_DIV:
      genDiv(stmt.dest, stmt.src[0], stmt.src[1]);
      /*
      {
      ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src[1].size()));
      m_instructions.push_back(AtiInst(SH_ATI_RCP, rcp, stmt.src[1]));
      m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, stmt.src[0], rcp));
      }
      */
      break;
    case SH_OP_ABS:
      m_instructions.push_back(AtiInst(SH_ATI_ABS, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_CEIL:
      {
        m_instructions.push_back(AtiInst(SH_ATI_FLR, stmt.dest, -stmt.src[0])); 
        m_instructions.push_back(AtiInst(SH_ATI_MOV, stmt.dest, -stmt.dest));
      }
      break;
    case SH_OP_COS:
      if( m_kind == SH_VERTEX_PROGRAM ) 
        SH_DEBUG_ERROR("cosine is not implemented for ARB vertex program ");
      m_instructions.push_back(AtiInst(SH_ATI_COS, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_DOT:
      {
        ShVariable left = stmt.src[0];
        ShVariable right = stmt.src[1];

        // expand left/right if they are scalar
        if( left.size() < right.size() ) {
          int *swizzle = new int[ right.size() ];
          for( int i = 0; i < right.size(); ++i ) swizzle[i] = 0; 
          left = left( right.size(), swizzle ); 
          delete swizzle;
        } else if( right.size() < left.size() ) {
          int *swizzle = new int[ left.size() ];
          for( int i = 0; i < left.size(); ++i ) swizzle[i] = 0; 
          right = right( left.size(), swizzle ); 
          delete swizzle;
        }

        if (left.size() == 3) {
          m_instructions.push_back(AtiInst(SH_ATI_DP3, stmt.dest, left, right));
        } else if (left.size() == 4) {
          m_instructions.push_back(AtiInst(SH_ATI_DP4, stmt.dest, left, right));
        } else if (left.size() == 1) {
          m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, left, right));
        } else {
          ShVariable mul(new ShVariableNode(SH_VAR_TEMP, left.size()));
          m_instructions.push_back(AtiInst(SH_ATI_MUL, mul, left, right));
          m_instructions.push_back(AtiInst(SH_ATI_ADD, stmt.dest, mul(0), mul(1)));
          for (int i = 2; i < left.size(); i++) {
            m_instructions.push_back(AtiInst(SH_ATI_ADD, stmt.dest, stmt.dest, mul(i)));
          }
        }
      }
      break;
    case SH_OP_FLR:
      m_instructions.push_back(AtiInst(SH_ATI_FLR, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_FMOD:
      {
        // TODO - is this really optimal?
        ShVariable t1(new ShVariableNode(SH_VAR_TEMP, stmt.src[0].size()));
        ShVariable t2(new ShVariableNode(SH_VAR_TEMP, stmt.src[0].size()));
        
        // result = x - sign(x/y)*floor(abs(x/y))*y
        genDiv(t1, stmt.src[0], stmt.src[1]);
        m_instructions.push_back(AtiInst(SH_ATI_ABS, t2, t1));

        genDiv(t1, t1, t2);
        m_instructions.push_back(AtiInst(SH_ATI_FLR, t2, t2)); 
        m_instructions.push_back(AtiInst(SH_ATI_MUL, t1, t1, t2)); 
        m_instructions.push_back(AtiInst(SH_ATI_MUL, t1, t1, stmt.src[1])); 
        m_instructions.push_back(AtiInst(SH_ATI_SUB, stmt.dest, stmt.src[0], t1)); 
      }
      break;
    case SH_OP_FRAC:
      m_instructions.push_back(AtiInst(SH_ATI_FRC, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_LRP:
      if(m_kind == SH_VERTEX_PROGRAM) {
        ShVariable t(new ShVariableNode(SH_VAR_TEMP, stmt.src[1].size()));
        // lerp(f,a,b)=f*a + (1-f)*b = f*(a-b) + b 
        m_instructions.push_back(AtiInst(SH_ATI_ADD, t, stmt.src[1], -stmt.src[2]));

        if (stmt.src[0].size() == 1 && stmt.src[1].size() != 1) { 
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;

          m_instructions.push_back(AtiInst(SH_ATI_MAD, stmt.dest, 
                stmt.src[0](stmt.src[1].size(), swizzle), t, stmt.src[2]));
          delete [] swizzle;
        } else {
          m_instructions.push_back(AtiInst(SH_ATI_MAD, stmt.dest, 
                stmt.src[0], t, stmt.src[2]));
        }
      } else {
        if(stmt.src[0].size() == 1 && stmt.src[1].size() != 1) {
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;

          m_instructions.push_back(AtiInst(SH_ATI_LRP, stmt.dest,
                stmt.src[0](stmt.src[1].size(), swizzle), stmt.src[1], stmt.src[2]));
          delete [] swizzle;
        } else {
          m_instructions.push_back(AtiInst(SH_ATI_LRP, stmt.dest,
                stmt.src[0], stmt.src[1], stmt.src[2]));
        }
      }
      break;
    case SH_OP_MAD:
      if (stmt.src[0].size() != 1 || stmt.src[1].size() != 1) {
        if (stmt.src[0].size() == 1) {
          int* swizzle = new int[stmt.src[1].size()];
          for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0; 
          m_instructions.push_back(AtiInst(SH_ATI_MAD, stmt.dest, 
                stmt.src[0](stmt.src[1].size(), swizzle), stmt.src[1], stmt.src[2]));
          delete [] swizzle;
          break;
        } else if (stmt.src[1].size() == 1) {
          int* swizzle = new int[stmt.src[0].size()];
          for (int i = 0; i < stmt.src[0].size(); i++) swizzle[i] = 0;
          m_instructions.push_back(AtiInst(SH_ATI_MAD, stmt.dest, stmt.src[0],
                stmt.src[1](stmt.src[0].size(), swizzle), stmt.src[2]));
          delete [] swizzle;
          break;
        }
      } 
      m_instructions.push_back(AtiInst(SH_ATI_MAD, stmt.dest, stmt.src[0], 
            stmt.src[1], stmt.src[2])); 
      break;

    case SH_OP_MAX:
      m_instructions.push_back(AtiInst(SH_ATI_MAX, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_MIN:
      m_instructions.push_back(AtiInst(SH_ATI_MIN, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_NORM:
      { // TODO: other than 3 components
        ShVariable mul(new ShVariableNode(SH_VAR_TEMP, 1));
        m_instructions.push_back(AtiInst(SH_ATI_DP3, mul, stmt.src[0], stmt.src[0]));
        m_instructions.push_back(AtiInst(SH_ATI_RSQ, mul, mul));
        m_instructions.push_back(AtiInst(SH_ATI_MUL, stmt.dest, mul, stmt.src[0]));
      }
      break;
    case SH_OP_SIN:
      // TODO fix this
      if( m_kind == SH_VERTEX_PROGRAM ) 
        SH_DEBUG_ERROR("sin is not implemented for ARB vertex program ");
      m_instructions.push_back(AtiInst(SH_ATI_SIN, stmt.dest, stmt.src[0]));
      break;
    case SH_OP_SLT:
      m_instructions.push_back(AtiInst(SH_ATI_SLT, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_SGT:
      m_instructions.push_back(AtiInst(SH_ATI_SLT, stmt.dest, stmt.src[1], stmt.src[0]));
      break;
    case SH_OP_SLE:
      m_instructions.push_back(AtiInst(SH_ATI_SGE, stmt.dest, stmt.src[1], stmt.src[0]));
      break;
    case SH_OP_SGE:
      m_instructions.push_back(AtiInst(SH_ATI_SGE, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_SQRT:
      {
        ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, stmt.src[0].size()));
        m_instructions.push_back(AtiInst(SH_ATI_RSQ, rcp, stmt.src[0]));
        m_instructions.push_back(AtiInst(SH_ATI_RCP, stmt.dest, rcp));
      break;
      }
    case SH_OP_POW:
      // TODO: scalarize?
      m_instructions.push_back(AtiInst(SH_ATI_POW, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_TEX:
      m_instructions.push_back(AtiInst(SH_ATI_TEX, stmt.dest, stmt.src[1], stmt.src[0]));
      break;
    case SH_OP_XPD:
      m_instructions.push_back(AtiInst(SH_ATI_XPD, stmt.dest, stmt.src[0], stmt.src[1]));
      break;
    case SH_OP_COND:
      if (stmt.src[0].size() == 1 && stmt.src[1].size() != 1) {
        int* swizzle = new int[stmt.src[1].size()];
        for (int i = 0; i < stmt.src[1].size(); i++) swizzle[i] = 0;
        m_instructions.push_back(AtiInst(SH_ATI_CMP, stmt.dest, -stmt.src[0](stmt.src[1].size(), swizzle),
                                         stmt.src[1], stmt.src[2]));
        delete [] swizzle;
      } else {
        m_instructions.push_back(AtiInst(SH_ATI_CMP, stmt.dest, -stmt.src[0], stmt.src[1], stmt.src[2]));
      }
      break;
    case SH_OP_KIL:
      m_instructions.push_back(AtiInst(SH_ATI_KIL, -stmt.dest));
      break;
    default:
      SH_DEBUG_WARN(opInfo[stmt.op].name << " not handled by ARB backend");
    }
  }

  genNode(node->follower);
}

void AtiCode::genDiv(ShVariable dest, ShVariable op1, ShVariable op2) {
  if (op2.size() == 1 && op1.size() != 1) {
    ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, op2.size())); 
    m_instructions.push_back(AtiInst(SH_ATI_RCP, rcp, op2)); 

    int* swizzle = new int[op1.size()];
    for (int i = 0; i < op1.size(); i++) swizzle[i] = 0;
    m_instructions.push_back(AtiInst(SH_ATI_MUL, dest, op1,
          rcp(op1.size(), swizzle)));
    delete [] swizzle;
  } else {
    ShVariable rcp(new ShVariableNode(SH_VAR_TEMP, 1));

    // TODO arg...component-wise div is ugly, ARB RCP only works on scalars
    for(int i = 0; i < op2.size(); ++i) {
      m_instructions.push_back(AtiInst(SH_ATI_RCP, rcp, op2(i)));
      m_instructions.push_back(AtiInst(SH_ATI_MUL, dest(i), op1(i), rcp));
    }
  }
}

void AtiCode::allocRegs()
{
  allocInputs();
  
  allocOutputs();
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->uniforms.begin();
       I != m_shader->uniforms.end(); ++I) {
    allocParam(*I);
  }

  allocConsts();
  
  allocTemps();

  allocTextures();
}

void AtiCode::bindSpecial(const ShProgramNode::VarList::const_iterator& begin,
                          const ShProgramNode::VarList::const_iterator& end,
                          const AtiBindingSpecs& specs, 
                          std::vector<int>& bindings,
                          ShAtiRegType type, int& num)
{
  bindings.push_back(0);
  
  if (specs.specialType == SH_VAR_ATTRIB) return;
  
  for (ShProgramNode::VarList::const_iterator I = begin; I != end; ++I) {
    ShVariableNodePtr node = *I;
    
    if (m_registers.find(node) != m_registers.end()) continue;
    if (node->specialType() != specs.specialType) continue;
    
    m_registers[node] = AtiReg(type, num++);
    m_registers[node].binding = specs.binding;
    m_registers[node].bindingIndex = bindings.back();
    
    bindings.back()++;
    if (bindings.back() == specs.maxBindings) break;
  }    
}

void AtiCode::allocInputs()
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; shAtiAttribBindingSpecs[m_kind][i].binding != SH_ATI_REG_NONE; i++) {
    bindSpecial(m_shader->inputs.begin(), m_shader->inputs.end(),
                shAtiAttribBindingSpecs[m_kind][i], m_inputBindings,
                SH_ATI_REG_ATTRIB, m_numInputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->inputs.begin();
       I != m_shader->inputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = AtiReg(SH_ATI_REG_ATTRIB, m_numInputs++);

    // Binding
    for (int i = 0; shAtiAttribBindingSpecs[m_kind][i].binding != SH_ATI_REG_NONE; i++) {
      const AtiBindingSpecs& specs = shAtiAttribBindingSpecs[m_kind][i];

      if (specs.allowGeneric && m_inputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_inputBindings[i];
        m_inputBindings[i]++;
        break;
      }
    }
  }
}

void AtiCode::allocOutputs()
{
  // First, try to assign some "special" output register bindings
  for (int i = 0; shAtiOutputBindingSpecs[m_kind][i].binding != SH_ATI_REG_NONE; i++) {
    bindSpecial(m_shader->outputs.begin(), m_shader->outputs.end(),
                shAtiOutputBindingSpecs[m_kind][i], m_outputBindings,
                SH_ATI_REG_OUTPUT, m_numOutputs);
  }
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->outputs.begin();
       I != m_shader->outputs.end(); ++I) {
    ShVariableNodePtr node = *I;
    if (m_registers.find(node) != m_registers.end()) continue;
    m_registers[node] = AtiReg(SH_ATI_REG_OUTPUT, m_numOutputs++);

    // Binding
    for (int i = 0; shAtiOutputBindingSpecs[m_kind][i].binding != SH_ATI_REG_NONE; i++) {
      const AtiBindingSpecs& specs = shAtiOutputBindingSpecs[m_kind][i];

      if (specs.allowGeneric && m_outputBindings[i] < specs.maxBindings) {
        m_registers[node].binding = specs.binding;
        m_registers[node].bindingIndex = m_outputBindings[i];
        m_outputBindings[i]++;
        break;
      }
    }
  }
}

void AtiCode::allocParam(ShVariableNodePtr node)
{
  // TODO: Check if we reached maximum
  if (m_registers.find(node) != m_registers.end()) return;
  m_registers[node] = AtiReg(SH_ATI_REG_PARAM, m_numParams);
  m_registers[node].binding = SH_ATI_REG_PARAMLOC;
  m_registers[node].bindingIndex = m_numParams;
  m_numParams++;
}


void AtiCode::allocConsts()
{
  for (ShProgramNode::VarList::const_iterator I = m_shader->constants.begin();
       I != m_shader->constants.end(); ++I) {
    ShVariableNodePtr node = *I;
    m_registers[node] = AtiReg(SH_ATI_REG_CONST, m_numConsts);
    for (int i = 0; i < 4; i++) {
      m_registers[node].values[i] = (float)(i < node->size() ? node->getValue(i) : 0.0);
    }
    m_numConsts++;
  }
}

void AtiCode::allocTextures()
{
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int binding, index;
    index = m_numTextures;
    glGenTextures(1, (GLuint*)&binding);
    m_registers[node] = AtiReg(SH_ATI_REG_TEXTURE, index);
    m_registers[node].bindingIndex = binding;
    m_numTextures++;
  }
}


void mark(ShLinearAllocator& allocator, ShVariableNodePtr node, int i)
{
  if (!node) return;
  if (node->kind() != SH_VAR_TEMP) return;
  if (node->hasValues()) return;
  allocator.mark(node, i);
}

void AtiCode::allocTemps()
{
  ShLinearAllocator allocator(this);

  for (std::size_t i = 0; i < m_instructions.size(); i++) {
    AtiInst instr = m_instructions[i];
    mark(allocator, instr.dest.node(), (int)i);
    for (int j = 0; j < 3; j++) {
      mark(allocator, instr.src[j].node(), (int)i);
    }
  }
  
  m_tempRegs.clear();
  m_numTemps = 0;
  for (int i = 0; i < m_backend->temps(m_kind); i++) {
    m_tempRegs.push_back(i);
  }
  
  allocator.allocate();
  
  m_tempRegs.clear();
}

AtiBackend::AtiBackend()
{
#ifdef WIN32
	DWORD err;
	if ((glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB")) == NULL)
		err = GetLastError();
	if ((glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB")) == NULL)
		err = GetLastError();
	if ((glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB")) == NULL)
		err = GetLastError();
	if ((glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB")) == NULL)
		err = GetLastError();
	if ((glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D")) == NULL)
		err = GetLastError();
	if ((glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)wglGetProcAddress("glProgramEnvParameter4fvARB")) == NULL)
		err = GetLastError();
	if ((glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)wglGetProcAddress("glProgramLocalParameter4fvARB")) == NULL)
		err = GetLastError();
	if ((glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)wglGetProcAddress("glGetProgramivARB")) == NULL)
		err = GetLastError();
#endif /* WIN32 */
}

void AtiBackend::init2(void)
{
  InitExtensions();
  // TODO Max TEX instructions, texture indirections
  for (int i = 0; i < 2; i++) {
    unsigned int target = shAtiTargets[i];
    m_instrs[i] = (!i ? 128 : 48);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &m_instrs[i]);
    SH_DEBUG_PRINT("instrs[" << i << "] = " << m_instrs[i]);
    m_temps[i] = (!i ? 12 : 16);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_TEMPORARIES_ARB, &m_temps[i]);
    SH_DEBUG_PRINT("temps[" << i << "] = " << m_temps[i]);
    m_attribs[i] = (!i ? 16 : 10);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_ATTRIBS_ARB, &m_attribs[i]);
    SH_DEBUG_PRINT("attribs[" << i << "] = " << m_attribs[i]);
    m_params[i] = (!i ? 96 : 24);
    shGlGetProgramivARB(target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &m_params[i]);
    SH_DEBUG_PRINT("params[" << i << "] = " << m_params[i]);
    m_texs[i] = (!i ? 0 : 24);
    if (i) shGlGetProgramivARB(target, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, &m_texs[i]);
    SH_DEBUG_PRINT("texs[" << i << "] = " << m_texs[i]);
  }

}

AtiBackend::~AtiBackend()
{
}

std::string AtiBackend::name() const
{
  return "ati";
}

ShBackendCodePtr AtiBackend::generateCode(int kind, const ShProgram& shader)
{

  SH_DEBUG_ASSERT(shader.object());
  AtiCodePtr code = new AtiCode(this, shader, kind);
  code->generate();
  return code;
}

void AtiBackend::allocFramebuffer(ShFramebufferPtr fb) 
{
  if( !fb ) {
    SH_DEBUG_WARN( "No framebuffer object (This is probably okay)" );
    return;
  }
  ShUberbufferPtr ub = fb->getUberbuffer();

  allocUberbuffer( ub );
}

void AtiBackend::allocUberbuffer( ShUberbufferPtr ub) {
  if( !ub ) return;

  CHECK_GL_ERROR();

  GLmem mem = ub->mem();
  if( mem ) return;

  SH_DEBUG_PRINT("Allocating uber buffer ");

  ShUberbuffer::PropertyMap props = ub->properties();
  int numProps = props.size();
  GLint* propsArray = new GLint[ numProps * 2 + 4 ];

  int i = 0;
  for( ShUberbuffer::PropertyMap::iterator pmit = props.begin();
      pmit != props.end(); ++pmit, i += 2 ) {
    propsArray[i ] = pmit->first;
    propsArray[i + 1] = pmit->second;
  }

  // TODO fix this hack (give user way of specifying properties)
  // Currently hardcoded to allow ubuf to be used as a 2D texture,
  // and attach to a color buffer if it has 3-4 components.
  propsArray[i++] = GL_TEXTURE_2D;
  propsArray[i++] = GL_TRUE;
  if( ub->elements() > 2 ) { 
    propsArray[i++] = GL_COLOR_BUFFER_ATI;
    propsArray[i++] = GL_TRUE;
  }

  GLenum format = ub->format();
  if( format == 0 ) {
    switch( ub->elements() ) {
      case 1: format = GL_LUMINANCE_FLOAT32_ATI; break;
      case 2: format = GL_LUMINANCE_ALPHA_FLOAT32_ATI; break;
      case 3: format = GL_RGB_FLOAT32_ATI; break;
      case 4: format = GL_RGBA_FLOAT32_ATI; break;
      default: format = 0;
    }
  }
  ub->setFormat( format );

  SH_DEBUG_PRINT(ub->format()<<"; "<<GL_RGBA);

  ub->setMem( glAllocMem2DATI(format, ub->width(), ub->height(), 
			  i/2, propsArray ) );
  
  SH_DEBUG_PRINT("Alocated: "<<ub->mem());

  delete[] propsArray;

  PRINT_GL_ERROR( "Allocating ubuf" );
}

void AtiBackend::printUbErrors() {
  // TODO fix when this function is documented
  //const GLcharARB *msg = glGetMemInfoLogATI(0);
  //if(msg) SH_DEBUG_WARN( msg ); 
}

void AtiBackend::bindFramebuffer() {
  ShFramebufferPtr fb = ShEnvironment::framebuffer;
  allocFramebuffer( fb );

  CHECK_GL_ERROR();

  if( fb ) {
    ShUberbufferPtr ub = fb->getUberbuffer();
    if( ub ) {
      allocUberbuffer( ub );
      //TODO may not be necessary
      glAttachMemATI( GL_AUX0, ub->mem() );

      SH_DEBUG_PRINT( "Attach GL_AUX0 to " << ub->mem() );

      //TODO other buffers 
      glDrawBuffer( GL_AUX0 ); 
    } else {
      glAttachMemATI( GL_AUX0, 0 );

      SH_DEBUG_PRINT( "Detach GL_AUX0" ); 

      glDrawBuffer( GL_BACK );
    }
  } else {
      glAttachMemATI( GL_AUX0, 0 );

      SH_DEBUG_PRINT( "Detach GL_AUX0" ); 

      glDrawBuffer( GL_BACK );
  }

  PRINT_GL_ERROR( "Binding Framebuffer" );
}

void AtiBackend::setUberbufferData(ShUberbufferPtr ub, const float *data) {
  allocUberbuffer( ub );
  unsigned int mem = ub->mem();
  if( !mem ) return; 
  //TODO replace all this junk with a glMemImage(...) call when it's implemented


  CHECK_GL_ERROR();

  //bind the texture 
  GLuint texBinding;
  glGenTextures(1, &texBinding);
  shGlActiveTextureARB(GL_TEXTURE0);

  //TODO handle other dimensions
  glBindTexture(GL_TEXTURE_2D, texBinding); 
  
  //attach the uber buffer
  glAttachMemATI( GL_TEXTURE_2D, mem ); 

  //use glTexImage to upload data
  // TODO support other dimensions
  GLenum format;
  switch (ub->elements()) {
    case 1: format = GL_LUMINANCE; break;
    case 2: format = GL_LUMINANCE_ALPHA; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default: format = 0; break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, ub->format(), ub->width(), ub->height(), 0, format, GL_FLOAT,
               data);

  //detach uber buffer
  glAttachMemATI( GL_TEXTURE_2D, 0 );

  //delete texture
  glDeleteTextures(1, (GLuint*)&texBinding);

  PRINT_GL_ERROR( "Setting data for ubuf " << mem  );
}

//TODO use glGetImage on the super buffer when it's implemented
//instead of rendering to a buffer and reading pixels
float* AtiBackend::getUberbufferData(const ShUberbuffer *ub) {
  unsigned int mem = ub->mem();
  if( !mem ) {
    SH_DEBUG_WARN( "Cannot get uber buffer data from unininitialized uber buffer\n" );
    return 0; 
  }
  SH_DEBUG_WARN( "Getting ubuf data (Warning - experimental - bindShader again before accessing textures)" ); 

  CHECK_GL_ERROR();

  GLint oldReadBuf;
  glGetIntegerv( GL_READ_BUFFER, &oldReadBuf );

  float* data = new float[ ub->width() * ub->height() * ub->elements() ];


  glAttachMemATI( GL_AUX0, 0 );
  SH_DEBUG_PRINT( "Detach GL_AUX0" );

  glAttachMemATI( GL_AUX0, mem );
  SH_DEBUG_PRINT( "Attach GL_AUX0 to " << mem );

  glReadBuffer( GL_AUX0 );

  GLenum format;
  switch (ub->elements()) {
    case 1: format = GL_LUMINANCE; break;
    case 2: format = GL_LUMINANCE_ALPHA; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default: format = 0; break;
  }
  glReadPixels( 0, 0, ub->width(), ub->height(), format, GL_FLOAT, data ); 

  /*
  for( int i = 0; i < ub->height(); ++i ) {
    std::cout << "Row " << i << ": ";
    for( int j = 0; j < ub->width(); ++j ) {
      std::cout << "( ";
      for( int k = 0; k < ub->elements(); ++k ) {
        std::cout << data[ ub->elements() * ( ub->width() * i + j ) + k ] << " ";
      }
      std::cout << " )";
    }
    std::cout << std::endl;
  }
  */


  glAttachMemATI( GL_AUX0, 0 );
  SH_DEBUG_PRINT( "Detach GL_AUX0" ); 

  glReadBuffer( oldReadBuf );

  PRINT_GL_ERROR( "Getting data from ubuf " << mem );

  return data;
}

void AtiBackend::render(SH::ShVertexArray& data){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_DOUBLE, 0, data.getData(0));
  glNormalPointer(GL_DOUBLE, 0, data.getData(1));

  glDrawElements(GL_TRIANGLES, data.getIndexSize(0), GL_UNSIGNED_SHORT, data.getIndex(0));

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glFinish();
  glutSwapBuffers();

}

void AtiBackend::render_planar(SH::ShVertexArray& data){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glVertexPointer(2, GL_DOUBLE, 0, data.getData(3));
  glNormalPointer(GL_DOUBLE, 0, data.getData(1));
  
  glDrawElements(GL_TRIANGLES, data.getIndexSize(0), GL_UNSIGNED_SHORT, data.getIndex(0));

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glFinish();
  //glutSwapBuffers();

}


void AtiBackend::deleteUberbuffer(const ShUberbuffer* ub) {
  CHECK_GL_ERROR();

  if( !ub->mem() ) return; 
  glDeleteMemATI(ub->mem());

  PRINT_GL_ERROR( "Deleting Ubuf " << ub->mem() );
}

}
