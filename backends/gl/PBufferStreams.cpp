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

namespace shgl {

using namespace SH;

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

PBufferStreams::PBufferStreams(int context)
  : m_context(context)
{
}

StreamStrategy* PBufferStreams::create(int context)
{
  return new PBufferStreams(context);
}

void PBufferStreams::execute(const ShProgram& program,
                             ShStream& dest)
{
  // Check program target
  if (program->target() != "gpu:stream") {
    ShError(PBufferStreamException("This backend can only execute ``gpu:stream'' programs."));
    return;
  }

  // Make sure program has no inputs
  if (!program->inputs.empty()) {
    ShError(PBufferStreamException("Stream program has unbound inputs, and can hence not be executed."));
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
  
  if (dest.size() > 1) {
    // TODO: If any stream channels are used as both inputs and
    // outputs we will want to cache their new values elsewhere and
    // then overwrite them at the end.
    int i = 0;
    for (ShStream::NodeList::iterator I = dest.begin(); I != dest.end(); ++I, ++i) {
      ShStream s(*I);
      execute(shSwizzle(i) << program, s);
    }
    return;
  }

  // --- Set up the GLX context
  
  // Figure out what extension we're using
  enum {
    SH_ARB_NV_FLOAT_BUFFER,
    SH_ARB_ATI_PIXEL_FORMAT_FLOAT,
    SH_ARB_NO_FLOAT_EXT
  } extension = SH_ARB_NO_FLOAT_EXT;

  // This is glx specific for now
  char* dpyname = 0;
  Display* dpy = XOpenDisplay(dpyname);
  int scrnum;
  scrnum = DefaultScreen(dpy);

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
    
    fb_config = glXChooseFBConfig(dpy, scrnum, &fb_attribs.front(), &items);
    if (fb_config) extension = SH_ARB_NV_FLOAT_BUFFER;
  }
  // Try ATI
  if (!fb_config) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_FLOAT_ATI_BIT);
    fb_attribs.push_back(None);
    
    fb_config = glXChooseFBConfig(dpy, scrnum, &fb_attribs.front(), &items);
    if (fb_config) extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
  }

  if (!fb_config) {
    SH_DEBUG_ERROR("Could not get GLX FB Config!" << std::endl <<
                   "Your card may not support the appropriate extensions.");
    XCloseDisplay(dpy);
    return;
  }

  if (extension == SH_ARB_NO_FLOAT_EXT) {
    SH_DEBUG_ERROR("Could not choose a floating-point extension!" << std::endl <<
                   "Your card may not support the appropriate extensions.");
    XCloseDisplay(dpy);
    return;
  }
  
  ShChannelNodePtr output = *dest.begin();

  StreamInputMap input_map;

  // Do a DFS through the program's control graph.
  StreamInputGatherer gatherer(input_map);
  program->ctrlGraph->dfs(gatherer);

  if (input_map.empty()) {
    ShError(PBufferStreamException("Stream program does not use any streams!"));
    return;
  }

  // First, allocate textures for each input stream.
  // Need to ensure that input stream sizes are the same.
  int input_count = -1;
  int tex_size = 0;
  for (StreamInputMap::iterator I = input_map.begin(); I != input_map.end(); ++I) {
    if (input_count < 0) {
      input_count = I->first->count();

      // Pick a size for the texture that just fits the data.
      int t = 1;
      while (t * t < input_count) {
        t <<= 1;
      }
      tex_size = t;
    }
    if (input_count != I->first->count()) {
      SH_DEBUG_ERROR("Input lengths of stream program do not match ("
                     << input_count << " != " << I->first->count() << ")");
      return;
    }
    ShTextureNodePtr tex;
    ShTextureTraits traits = ShArrayTraits();
    traits.clamping(ShTextureTraits::SH_UNCLAMPED);
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


  // Add in the texcoord variable
  ShProgram fp = program & lose<ShTexCoord2f>();

  // Make it a fragment program
  fp->target() = "gpu:fragment";
  
  ShVariableNodePtr tc_node = fp->inputs.back(); // there should be only one input anyways

  // replace FETCH with TEX
  TexFetcher texFetcher(input_map, tc_node, extension == SH_ARB_NV_FLOAT_BUFFER);
  fp->ctrlGraph->dfs(texFetcher);
  fp->collectVariables(); // necessary to collect all the new textures
  
  // optimize
  ShOptimizer optimizer(fp->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
    
  // Set up the pbuffer
  int pbuffer_attribs[] = {
    GLX_PBUFFER_WIDTH, tex_size,
    GLX_PBUFFER_HEIGHT, tex_size,
    GLX_LARGEST_PBUFFER, False,
    None
  };

  GLXPbuffer pbuffer = glXCreatePbuffer(dpy, fb_config[0], pbuffer_attribs);
  if (!pbuffer) {
    ShError(PBufferStreamException("Could not make pbuffer!"));
    XCloseDisplay(dpy);
    return;
  }
  
  GLXContext pbuffer_ctxt = glXCreateNewContext(dpy, fb_config[0], GLX_RGBA_TYPE, 0, True);
  if (!pbuffer_ctxt) {
    ShError(PBufferStreamException("Could not create PBuffer context"));
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }

  glXMakeCurrent(dpy, pbuffer, pbuffer_ctxt);

  shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->newContext();
  
  if (output->count() != input_count) {
    SH_DEBUG_ERROR("Input data count does not match output data count ("
                   << input_count << " != " << output->count() << ")");
    return;
  }
  
  int gl_error;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    ShError(PBufferStreamException("Could not enable GL_VERTEX_PROGRAM_ARB"));
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    ShError(PBufferStreamException("Could not enable GL_FRAGMENT_PROGRAM_ARB"));
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }

  // generate code
  shCompileShader(fp);
  
  // The (trivial) vertex program
  ShProgram vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
  vp->target() = "gpu:vertex";
  shCompileShader(vp);

  // Then, bind vertex (pass-through) and fragment program
  shBindShader(vp);
  shBindShader(fp);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  glFinish();

  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    ShError(PBufferStreamException("Could not render"));
    shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }
  
  // Render

  ShHostStoragePtr outhost
    = shref_dynamic_cast<ShHostStorage>(output->memory()->findStorage("host"));
  if (!outhost) {
    outhost = new ShHostStorage(output->memory().object(),
                                sizeof(float) * output->size() * output->count());
  }

  outhost->dirty();
  
  glReadPixels(0, 0, tex_size, input_count / tex_size, GL_RGB,
               GL_FLOAT, outhost->data());
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    ShError(PBufferStreamException("Could not do glReadPixels()"));
    shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }
  if (input_count % tex_size) {
    glReadPixels(0, input_count / tex_size, input_count % tex_size, 1, GL_RGB, GL_FLOAT,
                 reinterpret_cast<float*>(outhost->data())
                 + (input_count - (input_count % tex_size)) * output->size());
    gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      ShError(PBufferStreamException("Could not do rest of glReadPixels()"));
      shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
      glXDestroyContext(dpy, pbuffer_ctxt);
      XFree(fb_config);
      XCloseDisplay(dpy);
      return;
    }
  }

  // Clean up
  shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
  glXDestroyContext(dpy, pbuffer_ctxt);
  XFree(fb_config);
  XCloseDisplay(dpy);

  ShEnvironment::boundShaders().clear();
}


}
