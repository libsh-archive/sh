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
#include "ShArb.hpp"
#include <map>

// All of this is Linux specific right now...
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#undef GL_GLEXT_PROTOTYPES

#include "sh.hpp"
#include "ShOptimizer.hpp"

// TODO: Improve this...

//#define SH_ARB_USING_ATI
#define SH_ARB_USING_NVIDIA

#ifdef SH_ARB_USING_ATI
// TODO: Include glxATI.h if it's available

#ifndef GLX_ATI_pixel_format_float
#define GLX_ATI_pixel_format_float  1

#define GLX_RGBA_FLOAT_ATI_BIT				0x00000100

#endif // GLX_ATI_pixel_format_float
#endif // SH_ARB_USING_ATI

#ifdef SH_ARB_USING_NVIDIA

#ifndef GLX_FLOAT_COMPONENTS_NV
#define GLX_FLOAT_COMPONENTS_NV         0x20B0
#endif // GLX_FLOAT_COMPONENTS_NV

#endif // SH_ARB_USING_NVIDIA

namespace ShArb {

using namespace SH;

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
      if (stmt.src[0].node()->kind() != SH_VAR_STREAM) continue;

      ShChannelNodePtr stream_node = stmt.src[0].node();
      input_map.insert(std::make_pair(stream_node, ShTextureNodePtr(0)));
    }
  }

private:
  StreamInputMap& input_map;
};

class TexFetcher {
public:
  TexFetcher(StreamInputMap& input_map,
             ShVariableNodePtr tc_node)
    : input_map(input_map),
      tc_node(tc_node)
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
      
      if (stmt.src[0].node()->kind() != SH_VAR_STREAM) {
        // TODO: complain
        continue;
      }
      
      ShChannelNodePtr stream_node = stmt.src[0].node();
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
      
      stmt = ShStatement(stmt.dest, texVar, SH_OP_TEX, coordsVar);
      // The following is useful for debugging
      // stmt = ShStatement(stmt.dest, SH_OP_ASN, coordsVar);
    }
  }
  
private:
  StreamInputMap& input_map;
  ShVariableNodePtr tc_node;
};

void ArbBackend::execute(const ShProgram& program,
                         ShStream& dest)
{
  // Check program target
  if (program->target() != "gpu:stream") {
    SH_DEBUG_ERROR("ShArb can only execute ``gpu:stream'' programs.");
    return;
  }

  // Make sure program has no inputs
  if (!program->inputs.empty()) {
    SH_DEBUG_ERROR("Stream program has unbound inputs, and can hence not be executed.");
    return;
  }

  if (dest.size() == 0) {
    SH_DEBUG_WARN("Stream program has no outputs?");
    return;
  }

  if (program->outputs.size() != dest.size()) {
    SH_DEBUG_ERROR("Number of stream program outputs does not match number of destinations.");
    return;
  }
  
  if (dest.size() > 1) {
    // TODO: If any stream channels are used as both inputs and
    // outputs we will want to cache their new values elsewhere and
    // then overwrite them at the end.
    int i = 0;
    for (ShStream::NodeList::iterator I = dest.begin(); I != dest.end(); ++I, ++i) {
      ShStream s(*I);
      execute(shPermute(i) << program, s);
    }
    return;
  }

  ShChannelNodePtr output = *dest.begin();

  StreamInputMap input_map;

  // Do a DFS through the program's control graph.
  StreamInputGatherer gatherer(input_map);
  program->ctrlGraph->dfs(gatherer);

  if (input_map.empty()) {
    SH_DEBUG_ERROR("Stream program does not use any streams!");
    return;
  }

  // First, allocate textures for each input stream.
  // Need to ensure that input stream sizes are the same.
  int input_count = -1;
  int tex_size;
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
#ifdef SH_ARB_USING_NVIDIA
    ShDataTextureNodePtr tex = new ShDataTextureNode(SH_TEXTURE_RECT, tex_size, tex_size, 1,
                                                     I->first->size());
#else
    ShDataTextureNodePtr tex = new ShDataTextureNode(SH_TEXTURE_2D, tex_size, tex_size, 1,
                                                     I->first->size());
#endif
    ShDataMemoryObjectPtr mem = new ShDataMemoryObject(tex_size, tex_size, 1,
                                                       I->first->size());
    mem->setPartialData(reinterpret_cast<const float*>(I->first->data()), input_count);
    tex->setMem(mem);
    I->second = tex;
  }

  if (output->count() != input_count) {
    SH_DEBUG_ERROR("Input data count does not match output data count ("
                   << input_count << " != " << output->count() << ")");
    return;
  }

  /*
  SH_DEBUG_PRINT("Debugging info:" << std::endl <<
                 "input_count = " << input_count << std::endl <<
                 "tex_size   = " << tex_size);
  */

  // Add in the texcoord variable
  ShProgram fp = program & lose<ShTexCoord2f>();

  // Make it a fragment program
  fp->target() = "gpu:fragment";
  
  ShVariableNodePtr tc_node = fp->inputs.back(); // there should be only one input anyways

  ShBackendPtr arbBackend(this);

  // replace FETCH with TEX
  TexFetcher texFetcher(input_map, tc_node);
  fp->ctrlGraph->dfs(texFetcher);
  fp->collectVariables(); // necessary to collect all the new textures
  
  // optimize
  ShOptimizer optimizer(fp->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
    
  // Fun Happy Happy PBuffer Rendering Excitement
  
  // This is glx specific for now
  char* dpyname = 0;
  Display* dpy = XOpenDisplay(dpyname);
  int scrnum;
  scrnum = DefaultScreen(dpy);

  int fb_attribs[] = {
    GLX_DOUBLEBUFFER, False,
    GLX_RED_SIZE, 32,
    GLX_GREEN_SIZE, 32,
    GLX_BLUE_SIZE, 32,
#ifdef SH_ARB_USING_ATI
    GLX_RENDER_TYPE, GLX_RGBA_FLOAT_ATI_BIT,
#else
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
#endif
#ifdef SH_ARB_USING_NVIDIA
    GLX_FLOAT_COMPONENTS_NV, True,
#endif
    GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
    None
  };

  int pbuffer_attribs[] = {
    GLX_PBUFFER_WIDTH, tex_size,
    GLX_PBUFFER_HEIGHT, tex_size,
    GLX_LARGEST_PBUFFER, False,
    None
  };

  int items;
  
  
  GLXFBConfig* fb_config = glXChooseFBConfig(dpy, scrnum, fb_attribs, &items);
  if (!fb_config) {
    SH_DEBUG_ERROR("Could not get GLX FB Config!");
    XCloseDisplay(dpy);
    return;
  }

  GLXPbuffer pbuffer = glXCreatePbuffer(dpy, fb_config[0], pbuffer_attribs);
  if (!pbuffer) {
    SH_DEBUG_ERROR("Could not make pbuffer!");
    XCloseDisplay(dpy);
    return;
  }
  
  GLXContext pbuffer_ctxt = glXCreateNewContext(dpy, fb_config[0], GLX_RGBA_TYPE, 0, True);
  if (!pbuffer_ctxt) {
    SH_DEBUG_ERROR("Could not create PBuffer context");
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }

  // SH_DEBUG_PRINT("context direct: " << glXIsDirect(dpy, pbuffer_ctxt));

  glXMakeCurrent(dpy, pbuffer, pbuffer_ctxt);

  // SH_DEBUG_PRINT(glGetString(GL_EXTENSIONS));
  
  int gl_error;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Could not enable GL_VERTEX_PROGRAM_ARB: " << gl_error);
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Could not enable GL_FRAGMENT_PROGRAM_ARB: " << gl_error);
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }

  // generate code
  fp->compile(arbBackend);

  /*{
    SH_DEBUG_PRINT("Optimized FP:");
    fp->code(arbBackend)->print(std::cerr);
    }*/
  
  // The (trivial) vertex program
  ShProgram vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
  vp->target() = "gpu:vertex";
  vp->compile(arbBackend);

  // Then, bind vertex (pass-through) and fragment program
  fp->code(arbBackend)->bind();
  vp->code(arbBackend)->bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, tex_size, tex_size);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
#ifdef SH_ARB_USING_NVIDIA
#  define SH_ARB_STREAM_TEXCOORD_FUNC glTexCoord2f
#  define SH_ARB_STREAM_TEXCOORD_LEFT 0.0
#  define SH_ARB_STREAM_TEXCOORD_RIGHT (float)tex_size
#  define SH_ARB_STREAM_TEXCOORD_LOWER 0.0
#  define SH_ARB_STREAM_TEXCOORD_UPPER (float)tex_size
#else
#  define SH_ARB_STREAM_TEXCOORD_FUNC glTexCoord2f
#  define SH_ARB_STREAM_TEXCOORD_LEFT 0.0
#  define SH_ARB_STREAM_TEXCOORD_RIGHT 1.0 
#  define SH_ARB_STREAM_TEXCOORD_LOWER 0.0
#  define SH_ARB_STREAM_TEXCOORD_UPPER 1.0
#endif // !SH_ARB_USING_NVIDIA

  // Generate quad geometry
  glBegin(GL_QUADS); {
    SH_ARB_STREAM_TEXCOORD_FUNC(SH_ARB_STREAM_TEXCOORD_LEFT, SH_ARB_STREAM_TEXCOORD_LOWER);
    glVertex3f(-1.0, -1.0, 0.0);
    SH_ARB_STREAM_TEXCOORD_FUNC(SH_ARB_STREAM_TEXCOORD_LEFT, SH_ARB_STREAM_TEXCOORD_UPPER);
    glVertex3f(-1.0,  1.0, 0.0);
    SH_ARB_STREAM_TEXCOORD_FUNC(SH_ARB_STREAM_TEXCOORD_RIGHT, SH_ARB_STREAM_TEXCOORD_UPPER);
    glVertex3f( 1.0,  1.0, 0.0);
    SH_ARB_STREAM_TEXCOORD_FUNC(SH_ARB_STREAM_TEXCOORD_RIGHT, SH_ARB_STREAM_TEXCOORD_LOWER);
    glVertex3f( 1.0, -1.0, 0.0);
  } glEnd();

  glFinish();
  
  // Render

  glReadPixels(0, 0, tex_size, input_count / tex_size, GL_RGB,
               GL_FLOAT, output->data());
  if (input_count % tex_size) {
    // SH_DEBUG_PRINT("Reading 1 row of " << (input_count % tex_size) << " pixels");
    glReadPixels(0, input_count / tex_size, input_count % tex_size, 1, GL_RGB, GL_FLOAT,
                 reinterpret_cast<float*>(output->data())
                   + (input_count - (input_count % tex_size)) * output->size());
  }
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Could not do glReadPixels(): " << gl_error);
    glXDestroyContext(dpy, pbuffer_ctxt);
    XFree(fb_config);
    XCloseDisplay(dpy);
    return;
  }

  // Clean up
  // TODO: Anything else?
  glXDestroyContext(dpy, pbuffer_ctxt);
  XFree(fb_config);
  XCloseDisplay(dpy);
  
  ShEnvironment::boundShaders().clear();
}

}
