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
#include "PBufferStreams.hpp"

/// Turn this on if you want timings on std::cerr
//#define DO_PBUFFER_TIMING

// Turn this on to debug the fragment programs.
//#define SH_DEBUG_PBS_PRINTFP

#include <map>
#include <fstream>
#include <cstdlib>

#include "sh.hpp"
#include "ShOptimizations.hpp"
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "ShTransformer.hpp"

#ifdef DO_PBUFFER_TIMING
#include <sys/time.h>
#include <time.h>
#endif

namespace shgl {

using namespace SH;


#ifdef DO_PBUFFER_TIMING

class Timer {
public:
  Timer() { start(); }

  void start() { gettimeofday(&startval, 0); }

  long diff() {
    timeval endval;
    gettimeofday(&endval, 0);
    return (endval.tv_sec - startval.tv_sec)*1000
      + (endval.tv_usec/1000 - startval.tv_usec/1000);
  }

private:
  timeval startval;
};

#endif

class PBufferStreamException : public ShException {
public:
  PBufferStreamException(const std::string& message)
    : ShException("PBuffer Stream Execution: " + message)
  {
  }
};

typedef std::map<ShChannelNodePtr, ShTextureNodePtr> StreamInputMap;

class StreamInputGatherer {
public:
  StreamInputGatherer(StreamInputMap& input_map)
    : input_map(input_map)
  {
  }
  
  void operator()(const ShCtrlGraphNode* node)
  {
    if (!node->block) return;
    
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      const ShStatement& stmt = *I;
      if (stmt.op != SH_OP_FETCH) continue;

      // TODO: ought to complain here
      if (stmt.src[0].node()->kind() != SH_STREAM) continue;

      ShChannelNodePtr stream_node = shref_dynamic_cast<ShChannelNode>(stmt.src[0].node());
      input_map.insert(std::make_pair(stream_node, ShTextureNodePtr(0)));
    }
  }

private:
  StreamInputMap& input_map;
};

class TexFetcher {
public:
  TexFetcher(StreamInputMap& input_map,
             ShVariableNodePtr tc_node,
             bool indexed,
             ShVariableNodePtr width_var,
             ShProgramNodePtr program)
    : input_map(input_map),
      tc_node(tc_node),
      indexed(indexed),
      width_var(width_var),
      program(program)
  {
  }

  void operator()(ShCtrlGraphNode* node)
  {
    if (!node->block) return;
    for (ShBasicBlock::ShStmtList::iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      ShStatement& stmt = *I;
      if (stmt.op != SH_OP_FETCH && stmt.op != SH_OP_LOOKUP) continue;
      
      if (!stmt.src[0].node()) {
        SH_DEBUG_WARN("FETCH/LOOKUP from null stream");
        continue;
      }
      if (stmt.src[0].node()->kind() != SH_STREAM) {
        SH_DEBUG_WARN("FETCH/LOOKUP from non-stream");
        continue;
      }
      
      ShChannelNodePtr stream_node = shref_dynamic_cast<ShChannelNode>(stmt.src[0].node());
      StreamInputMap::const_iterator J = input_map.find(stream_node);
      if (J == input_map.end()) {
        SH_DEBUG_WARN("Stream node not found in input map");
        continue;
      }

      if (!J->second) {
        SH_DEBUG_WARN("No texture allocated for stream node");
        continue;
      }

      ShVariable texVar(J->second);

      if (stmt.op == SH_OP_FETCH) {
        ShVariable coordsVar(tc_node);
        if (indexed) {
          stmt = ShStatement(stmt.dest, texVar, SH_OP_TEXI, coordsVar);
        } else {
          stmt = ShStatement(stmt.dest, texVar, SH_OP_TEX, coordsVar);
        }
      } else {
        // Make sure our actualy index is a temporary in the program.
        ShContext::current()->enter(program);
        ShVariable coordsVar(new ShVariableNode(SH_TEMP, 2, SH_FLOAT));
        ShContext::current()->exit();
        
        ShBasicBlock::ShStmtList new_stmts;
        new_stmts.push_back(ShStatement(coordsVar(0), stmt.src[1], SH_OP_MOD, width_var));
        new_stmts.push_back(ShStatement(coordsVar(1), stmt.src[1], SH_OP_DIV, width_var));
        new_stmts.push_back(ShStatement(stmt.dest, texVar, SH_OP_TEXI, coordsVar));
        I = node->block->erase(I);
        node->block->splice(I, new_stmts);
        I--;
      }
      // The following is useful for debugging
      // stmt = ShStatement(stmt.dest, SH_OP_ASN, coordsVar);
    }
  }
  
private:
  StreamInputMap& input_map;
  ShVariableNodePtr tc_node;
  bool indexed;
  ShVariableNodePtr width_var;
  ShProgramNodePtr program;
};

PBufferStreams::PBufferStreams(void) :
  m_setup_vp(false)
{
}

PBufferStreams::~PBufferStreams()
{
}

#ifdef DO_PBUFFER_TIMING
int indent = 0;
Timer supertimer;

void fillin()
{
  long sd = supertimer.diff();
  supertimer.start();
  if (indent) for (int j = 0; j < sd; j++) {
    for (int i = 0; i < indent; i++) std::cerr << "| ";
    std::cerr << std::endl;
  }
}

#define DECLARE_TIMER(t) Timer pbtime_ ## t; do { fillin(); for (int i = 0; i < indent; i++) std::cerr << "| "; std::cerr << "^ " << # t << " starts" << std::endl; indent++;} while (0)
#define TIMING_RESULT(t) do {long d = pbtime_ ## t.diff(); fillin(); indent--; for (int i = 0; i < indent; i++) std::cerr << "| "; std::cerr << "v " << # t << " took " << d << " ms" << std::endl; supertimer.start(); } while (0)
#else
#define DECLARE_TIMER(t)
#define TIMING_RESULT(t) 
#endif

void PBufferStreams::execute(const ShProgramNodeCPtr& program,
                             ShStream& dest)
{
  DECLARE_TIMER(overhead);

  // Check program target
  if (program->target() != "gpu:stream") {
    shError(PBufferStreamException("This backend can only execute ``gpu:stream'' programs."));
    return;
  }

  // Make sure program has no inputs
  if (!program->inputs.empty()) {
    shError(PBufferStreamException("Stream program has unbound inputs, and can hence not be executed."));
    return;
  }

  if (dest.size() == 0) {
    SH_DEBUG_WARN("Stream program has no outputs?");
    return;
  }

  if ((int)program->outputs.size() != dest.size()) {
    SH_DEBUG_ERROR("Number of stream program outputs ("
                   << program->outputs.size()
                   << ") does not match number of destinations ("
                   << dest.size()
                   << ").");
    return;
  }
  TIMING_RESULT(overhead);
  
  if (dest.size() > 1) {
    DECLARE_TIMER(overall);
    // TODO: If any stream channels are used as both inputs and
    // outputs we will want to cache their new values elsewhere and
    // then overwrite them at the end.
    int i = 0;
    for (ShStream::NodeList::iterator I = dest.begin(); I != dest.end(); ++I, ++i) {
      ShStream s(*I);
      DECLARE_TIMER(specialize);
      ShProgram p = shSwizzle(i) << shref_const_cast<ShProgramNode>(program);
      TIMING_RESULT(specialize);
      execute(p.node(), s);
    }
    TIMING_RESULT(overall);
    return;
  }

  DECLARE_TIMER(onerun);
  // --- Set up the GLX context
  
  ShChannelNodePtr output = *dest.begin();
  int count = output->count();
  ShValueType valueType = output->valueType();

  // Pick a size for the texture that just fits the output data.
  int tex_size = 1;
  
  while (tex_size * tex_size < count) {
    tex_size <<= 1;
  }

  FloatExtension extension = setupContext(tex_size, tex_size);

  if (extension == SH_ARB_NO_FLOAT_EXT) return;

  DECLARE_TIMER(gather);
  
  StreamInputMap input_map;

  // Do a DFS through the program's control graph.
  StreamInputGatherer gatherer(input_map);
  program->ctrlGraph->dfs(gatherer);

  TIMING_RESULT(gather);
  
  if (input_map.empty()) {
    shError(PBufferStreamException("Stream program does not use any streams!"));
    return;
  }

  DECLARE_TIMER(texsetup);
  // First, allocate textures for each input stream.
  // Need to ensure that input stream sizes are the same.
  for (StreamInputMap::iterator I = input_map.begin(); I != input_map.end(); ++I) {
    if (I->first->count() != count) {
      SH_DEBUG_ERROR("Input lengths of stream program do not match ("
                     << I->first->count() << " != " << count << ")");
      return;
    }
    ShTextureNodePtr tex;
    ShTextureTraits traits = ShArrayTraits();
    traits.clamping(ShTextureTraits::SH_UNCLAMPED);

    // TODO!
    // We're copying a far larger amount of memory in here than we
    // should. Don't do this.
    switch (extension) {
    case SH_ARB_NV_FLOAT_BUFFER:
      tex = new ShTextureNode(SH_TEXTURE_RECT, I->first->size(),
                              I->first->valueType(), traits, tex_size, tex_size, 1, count);
      break;
    case SH_ARB_ATI_PIXEL_FORMAT_FLOAT:
      tex = new ShTextureNode(SH_TEXTURE_2D, I->first->size(),
                              I->first->valueType(), traits, tex_size, tex_size, 1, count);
      break;
    default:
      tex = 0;
      break;
    }

    tex->memory(I->first->memory());
    I->second = tex;
  }
  TIMING_RESULT(texsetup);
  
  DECLARE_TIMER(fpsetup);
  // Add in the texcoord variable
  ShProgram fp = ShProgram(shref_const_cast<ShProgramNode>(program))
    & lose<ShTexCoord2f>("streamcoord");

  // Handle affine conversion first
  ShContext::current()->enter(fp.node());
  ShTransformer transform(fp.node());
  transform.convertInputOutput();
  transform.convertAffineTypes();
  ShContext::current()->exit();

  // Make it a fragment program
  fp.node()->target() = "gpu:fragment";
  
  ShVariableNodePtr tc_node = fp.node()->inputs.back(); // there should be only one input anyways

  // Make a guaranteed uniform variable, by "pushing" the global scope
  ShContext::current()->enter(0);
  ShAttrib1f width = tex_size;
  ShContext::current()->exit();
  
  // replace FETCH with TEX
  TexFetcher texFetcher(input_map, tc_node, extension == SH_ARB_NV_FLOAT_BUFFER,
                        width.node(), fp.node());
  fp.node()->ctrlGraph->dfs(texFetcher);
  fp.node()->collectVariables(); // necessary to collect all the new textures

  // optimize
  optimize(fp);

  int gl_error;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not enable GL_VERTEX_PROGRAM_ARB"));
    return;
  }
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not enable GL_FRAGMENT_PROGRAM_ARB"));
    return;
  }
#ifdef SH_DEBUG_PBS_PRINTFP
  {
  std::ofstream fpgv("pb.dot");
  fp.node()->ctrlGraph->graphvizDump(fpgv);
  }
  system("dot -Tps -o pb.ps pb.dot");
#endif

  // generate code
  shCompile(fp);

#ifdef SH_DEBUG_PBS_PRINTFP
 {
  std::ofstream fpdbg("pbufferstream.fp");
  fp.code()->print(fpdbg);
 }
#endif

  TIMING_RESULT(fpsetup);

  DECLARE_TIMER(vpsetup);

  if (!m_setup_vp)
    {
    // The (trivial) vertex program
    m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
    m_vp.node()->target() = "gpu:vertex";
    shCompile(m_vp);
    m_setup_vp = true;
    }

  TIMING_RESULT(vpsetup);

  DECLARE_TIMER(binding);
  // Then, bind vertex (pass-through) and fragment program
  shBind(m_vp);
  shBind(fp);
  TIMING_RESULT(binding);

  DECLARE_TIMER(clear);
  glClear(GL_COLOR_BUFFER_BIT);
  TIMING_RESULT(clear);

  DECLARE_TIMER(rendersetup);
  glViewport(0, 0, tex_size, tex_size);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float tc_right;
  float tc_upper;

  if (extension == SH_ARB_NV_FLOAT_BUFFER) {
    tc_right = static_cast<float>(tex_size);
    tc_upper = static_cast<float>(tex_size);
  } else {
    tc_right = 1.0;
    tc_upper = 1.0;
  }
  TIMING_RESULT(rendersetup);

  DECLARE_TIMER(render);

  // Generate quad geometry
  glBegin(GL_QUADS); {
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glTexCoord2f(0.0, tc_upper);
    glVertex3f(-1.0,  1.0, 0.0);
    glTexCoord2f(tc_right, tc_upper);
    glVertex3f( 1.0,  1.0, 0.0);
    glTexCoord2f(tc_right, 0.0);
    glVertex3f( 1.0, -1.0, 0.0);
  } glEnd();

  TIMING_RESULT(render);

  DECLARE_TIMER(finish);
  glFinish();

  TIMING_RESULT(finish);

  
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not render"));
    return;
  }
  
  DECLARE_TIMER(findouthost);

  ShHostStoragePtr outhost
    = shref_dynamic_cast<ShHostStorage>(output->memory()->findStorage("host"));
  if (!outhost) {
    int datasize = shTypeInfo(valueType)->datasize(); 
    outhost = new ShHostStorage(output->memory().object(),
                                datasize * output->size() * output->count());
  }
  TIMING_RESULT(findouthost);

  DECLARE_TIMER(dirtyouthost);
  // Read back
  outhost->dirty();
  TIMING_RESULT(dirtyouthost);
  

  GLenum format;
  switch (output->size()) {
  case 1:
    format = GL_RED;
    break;
  case 2:
    SH_DEBUG_ASSERT(0 && "Sorry, 2-component outputs aren't working right now!");
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  default:
    SH_DEBUG_ASSERT(false);
    break;
  }

  DECLARE_TIMER(readback);

  // @todo half-float
  ShVariantPtr  resultBuffer; 
  int resultDatasize = output->size() * count;
  GLenum readpixelType;
  ShValueType convertedType; 
  readpixelType = shGlType(valueType, convertedType);
  if(convertedType != SH_VALUETYPE_END) {
      SH_DEBUG_WARN("ARB backend does not handle stream output type " << shValueTypeName(valueType) << " natively."
          << "  Using " << shValueTypeName(convertedType) << " temporary buffer.");
      resultBuffer = shVariantFactory(convertedType, SH_MEM)->generate(resultDatasize);
  } else {
      resultBuffer = shVariantFactory(valueType, SH_MEM)->generate(
          outhost->data(), resultDatasize, false);
  }

  glReadPixels(0, 0, tex_size, count / tex_size, format,
               readpixelType, resultBuffer->array());
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not do glReadPixels()"));
    return;
  }
  if (count % tex_size) {
    glReadPixels(0, count / tex_size, count % tex_size, 1, format, readpixelType,
                 (char*)(resultBuffer->array()) + (count - (count % tex_size)) * output->size() * resultBuffer->datasize());
    gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not do rest of glReadPixels()"));
      return;
    }
  }

  if(convertedType != SH_VALUETYPE_END) { // need to copy to outhoust->data()
    ShVariantPtr outhostVariant = shVariantFactory(valueType, SH_MEM)->generate(
          outhost->data(), resultDatasize, false);
    outhostVariant->set(resultBuffer);
  }

  TIMING_RESULT(readback);
  
  // TODO: I think this is necessary, but it doesn't seem to be. I assume
  // that GLUT (or whatever UI toolkit) is setting up its one context when
  // its about to redraw. -Kevin
  restoreContext();
  
  TIMING_RESULT(onerun);
}


}
