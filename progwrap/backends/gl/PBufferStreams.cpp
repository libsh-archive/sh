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

#include <map>

// Extensions for ATI and Nvidia

#ifndef GLX_ATI_pixel_format_float
#define GLX_ATI_pixel_format_float  1

#define GLX_RGBA_FLOAT_ATI_BIT				0x00000100

#endif // GLX_ATI_pixel_format_float

#ifndef GLX_FLOAT_COMPONENTS_NV
#define GLX_FLOAT_COMPONENTS_NV         0x20B0
#endif // GLX_FLOAT_COMPONENTS_NV

#include "sh.hpp"
#include "ShOptimizer.hpp"
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShContext.hpp"

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
             bool indexed)
    : input_map(input_map),
      tc_node(tc_node),
      indexed(indexed)
  {
  }

  void operator()(ShCtrlGraphNode* node)
  {
    ShVariable coordsVar(tc_node);
    if (!node->block) return;
    for (ShBasicBlock::ShStmtList::iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      ShStatement& stmt = *I;
      if (stmt.op != SH_OP_FETCH) continue;
      
      if (stmt.src[0].node()->kind() != SH_STREAM) {
        // TODO: complain
        continue;
      }
      
      ShChannelNodePtr stream_node = shref_dynamic_cast<ShChannelNode>(stmt.src[0].node());
      StreamInputMap::const_iterator I = input_map.find(stream_node);
      if (I == input_map.end()) {
        // TODO: complain
        continue;
      }

      if (!I->second) {
        // TODO: complain
        continue;
      }

      ShVariable texVar(I->second);

      if (indexed) {
        stmt = ShStatement(stmt.dest, texVar, SH_OP_TEXI, coordsVar);
      } else {
        stmt = ShStatement(stmt.dest, texVar, SH_OP_TEX, coordsVar);
      }
      // The following is useful for debugging
      // stmt = ShStatement(stmt.dest, SH_OP_ASN, coordsVar);
    }
  }
  
private:
  StreamInputMap& input_map;
  ShVariableNodePtr tc_node;
  bool indexed;
};

class GlxState {
public:
  GlxState()
    : display(0), drawable(0), context(0)
  {
    display = glXGetCurrentDisplay();
    if (display) {
      drawable = glXGetCurrentDrawable();
      context = glXGetCurrentContext();
    }
  }

  ~GlxState()
  {
    if (display) {
      glXMakeCurrent(display, drawable, context);
    }
  }

private:
  Display* display;
  GLXDrawable drawable;
  GLXContext context;
};

PBufferStreams::PBufferStreams(int context)
  : m_context(context),
    m_setup_vp(-1),
    m_display(0)
{
}

PBufferStreams::~PBufferStreams()
{
  // TODO: destroy our GLX information.
}

StreamStrategy* PBufferStreams::create(int context)
{
  return new PBufferStreams(context);
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

FloatExtension PBufferStreams::setupContext(int width, int height)
{
  if (m_info.valid()
      && m_info.width == width
      && m_info.height == height) {
    DECLARE_TIMER(activatecontext);
    shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(m_info.shcontext);
    glXMakeCurrent(m_display, m_info.pbuffer, m_info.context);
    TIMING_RESULT(activatecontext);
    return m_info.extension;
  }
  DECLARE_TIMER(makecontext);
  if (m_info.shcontext >= 0) {
    shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(m_info.shcontext);
    shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
  }
  // Figure out what extension we're using
  m_info.extension = SH_ARB_NO_FLOAT_EXT;
  m_info.width = width;
  m_info.height = height;
  m_info.pbuffer = 0;
  m_info.context = 0;

  m_info.shcontext
    = shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->newContext();
  
  // This is glx specific for now

  if (!m_display) {
    m_display = glXGetCurrentDisplay();
    if (!m_display) m_display = XOpenDisplay(0);
    if (!m_display) {
      shError(PBufferStreamException("Could not open X display"));
      return m_info.extension;
    }
  }
  
  int scrnum;
  scrnum = DefaultScreen(m_display);

  std::vector<int> fb_base_attribs;
  fb_base_attribs.push_back(GLX_DOUBLEBUFFER); fb_base_attribs.push_back(False);
  fb_base_attribs.push_back(GLX_RED_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_GREEN_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_BLUE_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_DRAWABLE_TYPE); fb_base_attribs.push_back(GLX_PBUFFER_BIT);
  
  int items;

  GLXFBConfig* fb_config = 0;

  // Try NVIDIA
  if (!fb_config) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_BIT);
    fb_attribs.push_back(GLX_FLOAT_COMPONENTS_NV); fb_attribs.push_back(True);
    fb_attribs.push_back(None);
    
    fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
    if (fb_config) {
      m_info.extension = SH_ARB_NV_FLOAT_BUFFER;
    }
  }
  // Try ATI
  if (!fb_config) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_FLOAT_ATI_BIT);
    fb_attribs.push_back(None);
    
    fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
    if (fb_config) {
      m_info.extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
    }
  }

  if (!fb_config) {
    shError(PBufferStreamException("Could not get GLX FB Config!\n"
                                   "Your card may not support the appropriate extensions."));
    return SH_ARB_NO_FLOAT_EXT;
  }

  if (m_info.extension == SH_ARB_NO_FLOAT_EXT) {
    shError(PBufferStreamException("Could not choose a floating-point extension!\n"
                                   "Your card may not support the appropriate extensions."));
    return m_info.extension;
  }

  // Set up the pbuffer
  int pbuffer_attribs[] = {
    GLX_PBUFFER_WIDTH, width,
    GLX_PBUFFER_HEIGHT, height,
    GLX_LARGEST_PBUFFER, False,
    None
  };

  m_info.pbuffer = glXCreatePbuffer(m_display, fb_config[0], pbuffer_attribs);
  if (!m_info.pbuffer) {
    shError(PBufferStreamException("Could not make pbuffer!"));
    return SH_ARB_NO_FLOAT_EXT;
  }
  
  m_info.context = glXCreateNewContext(m_display, fb_config[0], GLX_RGBA_TYPE, 0, True);
  if (!m_info.context) {
    shError(PBufferStreamException("Could not create PBuffer context"));
    XFree(fb_config);
    return SH_ARB_NO_FLOAT_EXT;
  }
  glXMakeCurrent(m_display, m_info.pbuffer, m_info.context);
  
  TIMING_RESULT(makecontext);
  return m_info.extension;
}

void PBufferStreams::execute(const ShProgramNodeCPtr& program,
                             ShStream& dest)
{
  DECLARE_TIMER(overhead);
  int prev = shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->context();

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

  // Pick a size for the texture that just fits the output data.
  int tex_size = 1;
  
  while (tex_size * tex_size < count) {
    tex_size <<= 1;
  }

  GlxState prevstate;
  
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
                              traits, tex_size, tex_size, 1);
      break;
    case SH_ARB_ATI_PIXEL_FORMAT_FLOAT:
      tex = new ShTextureNode(SH_TEXTURE_2D, I->first->size(),
                              traits, tex_size, tex_size, 1);
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
  ShProgram fp = ShProgram(shref_const_cast<ShProgramNode>(program)) & lose<ShTexCoord2f>();

  // Make it a fragment program
  fp.node()->target() = "gpu:fragment";
  
  ShVariableNodePtr tc_node = fp.node()->inputs.back(); // there should be only one input anyways

  // replace FETCH with TEX
  TexFetcher texFetcher(input_map, tc_node, extension == SH_ARB_NV_FLOAT_BUFFER);
  fp.node()->ctrlGraph->dfs(texFetcher);
  fp.node()->collectVariables(); // necessary to collect all the new textures

  
  // optimize
  ShOptimizer optimizer(fp.node()->ctrlGraph);
  optimizer.optimize(ShContext::current()->optimization());

  int gl_error;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not enable GL_VERTEX_PROGRAM_ARB"));
    //glXDestroyContext(m_display, pbuffer_ctxt);
    //XFree(fb_config);
    return;
  }
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not enable GL_FRAGMENT_PROGRAM_ARB"));
    //glXDestroyContext(m_display, pbuffer_ctxt);
    //XFree(fb_config);
    return;
  }

  // generate code
  shCompile(fp);

  TIMING_RESULT(fpsetup);

  DECLARE_TIMER(vpsetup);

  int curcontext = shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->context();
  if (m_setup_vp != curcontext) {
    // The (trivial) vertex program
    if (m_setup_vp < 0) {
      m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
      m_vp.node()->target() = "gpu:vertex";
    }
    shCompile(m_vp);
    m_setup_vp = curcontext;
  }

  TIMING_RESULT(vpsetup);

  DECLARE_TIMER(binding);
  // Then, bind vertex (pass-through) and fragment program
  shBind(m_vp);
  shBind(fp);
  TIMING_RESULT(binding);

  DECLARE_TIMER(clear);
  //glClear(GL_COLOR_BUFFER_BIT);
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
    //glXDestroyContext(m_display, pbuffer_ctxt);
    //XFree(fb_config);
    return;
  }
  
  DECLARE_TIMER(findouthost);

  ShHostStoragePtr outhost
    = shref_dynamic_cast<ShHostStorage>(output->memory()->findStorage("host"));
  if (!outhost) {
    outhost = new ShHostStorage(output->memory().object(),
                                sizeof(float) * output->size() * output->count());
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
  
  glReadPixels(0, 0, tex_size, count / tex_size, format,
               GL_FLOAT, outhost->data());
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    shError(PBufferStreamException("Could not do glReadPixels()"));
    //glXDestroyContext(m_display, pbuffer_ctxt);
    //XFree(fb_config);
    return;
  }
  if (count % tex_size) {
    glReadPixels(0, count / tex_size, count % tex_size, 1, format, GL_FLOAT,
                 reinterpret_cast<float*>(outhost->data())
                 + (count - (count % tex_size)) * output->size());
    gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not do rest of glReadPixels()"));
      //glXDestroyContext(m_display, pbuffer_ctxt);
      //XFree(fb_config);
      return;
    }
  }

  TIMING_RESULT(readback);
  
  // Clean up
  shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(prev);
  //glXDestroyContext(m_display, pbuffer_ctxt);
  //XFree(fb_config);

  // TODO: This just seems wrong.
  // ShEnvironment::boundShaders().clear();
  
  TIMING_RESULT(onerun);
}


}
