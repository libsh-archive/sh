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
#include "FBOStreams.hpp"

/// Turn this on if you want timings on std::cerr
//#define DO_FBO_TIMING

// Turn this on to debug the fragment programs.
//#define SH_DEBUG_PBS_PRINTFP

#include <map>
#include <fstream>
#include <cstdlib>
#include <list>
#include <set>

#include "sh.hpp"
#include "ShOptimizations.hpp"
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "GlTextureStorage.hpp"
#include "PBufferContext.hpp"
#include "Utils.hpp"
#include "FBOCache.hpp"

#ifdef DO_FBO_TIMING
#include <sys/time.h>
#include <time.h>
#endif

namespace shgl {

using namespace SH;
using namespace std;

#ifdef DO_FBO_TIMING

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

class FBOStreamException : public ShException {
public:
  FBOStreamException(const std::string& message)
    : ShException("FBO Stream Execution: " + message)
  {
  }
};


FBOStreams::FBOStreams(void) :
  m_shaders(NULL), m_setup_vp(false), 
  m_draw_buffers_ext(NONE), m_max_draw_buffers(1)
{
}

FBOStreams::~FBOStreams()
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

struct UnflagWrite {
  bool operator()(const ShStoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) return false;
    
    t->write(false);
    
    // Make sure we iterate over all of them
    return false;
  }
};

struct PrintStorages {
  bool operator()(const ShStoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) return false;
    std::cerr << *t << std::endl;
    return false;
  }
};

static void draw_rectangle(float x, float y, float w, float h, float size)
{
  glTexCoord3f(x/size, y/size, 0.0);
  glVertex3f(2.0*x/size-1.0, 2.0*y/size-1.0, 0.0);
  glTexCoord3f((x+w)/size, y/size, 0.0);
  glVertex3f(2.0*(x+w)/size-1.0, 2.0*y/size-1.0, 0.0);
  glTexCoord3f((x+w)/size, (y+h)/size, 0.0);
  glVertex3f(2.0*(x+w)/size-1.0, 2.0*(y+h)/size-1.0, 0.0);
  glTexCoord3f(x/size, (y+h)/size, 0.0);
  glVertex3f(2.0*x/size-1.0, 2.0*(y+h)/size-1.0, 0.0);
}

void FBOStreams::execute(const ShProgramNodeCPtr& program_const,
                         ShStream& dest, TextureStrategy *texture)
{
  // Let's get rid of that constness... Yes, yes, I know...
  ShProgramNodePtr program = shref_const_cast<ShProgramNode>(program_const);
  
  // Make sure program has no inputs
  if (!program->inputs.empty()) {
    shError(FBOStreamException("Stream program has unbound inputs, and can hence not be executed."));
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

  // TODO: Check that the size of each output matches the size of each dest.

  int count = (*dest.begin())->count();

  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
    if (count != (*I)->count()) {
      shError(FBOStreamException("All stream outputs must be of the same size"));
    }
  }
  

  PBufferHandlePtr old_handle = 0;
#ifdef WIN32
  if (wglGetCurrentContext() == NULL) {
#else
  if (glXGetCurrentContext() == NULL) {
#endif
    PBufferFactory* factory = PBufferFactory::instance();
    PBufferContextPtr context = factory->get_context(1,1);
    old_handle = context->activate();
  }

  DECLARE_TIMER(vpsetup);

  // --- Set up the vertex program
  if (!m_setup_vp) {
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    if (extensions) {
      string extstr(reinterpret_cast<const char*>(extensions));

      if (
#ifdef ATI_draw_buffers
          (extstr.find("ATI_draw_buffers") != string::npos) ||
#endif
          (extstr.find("ARB_draw_buffers") != string::npos)) {
#ifdef ATI_draw_buffers
        if (extstr.find("ATI_draw_buffers") != string::npos) {
          SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &m_max_draw_buffers));
          m_draw_buffers_ext = ATI;
        } else {
#endif
          SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &m_max_draw_buffers));
          m_draw_buffers_ext = ARB;
#ifdef ATI_draw_buffers
        }
#endif
      }
    }
    SH_GL_CHECK_ERROR(glGenFramebuffersEXT(1, &m_framebuffer));

    // The (trivial) vertex program
    m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
    m_vp.node()->target() = get_target_backend(program_const) + "vertex";
    shCompile(m_vp);
    m_setup_vp = true;
  }
  
  TIMING_RESULT(vpsetup);

  GLint max_outputs;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_outputs));

  // --- Set up the fragment programs and such
  StreamCache* cache = program->get_info<StreamCache>();
  if (!cache) {
    // Pick a size for the texture that is as large as the largest output
    int tex_size = 1;
    for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
      ShHostStoragePtr hs = 
        shref_dynamic_cast<ShHostStorage>((*I)->memory()->findStorage("host"));
      // Dirty cast, but if length is too big we'll blow up anyways
      while (hs && tex_size * tex_size < 
             (int)hs->length()/hs->value_size()/(*I)->size())
        tex_size <<= 1;
    }

    cache = new StreamCache(program.object(), m_vp.node().object(), 
                            tex_size, max_outputs);
    program->add_info(cache);
  }
  
  cache->update_channels();
  cache->freeze_inputs(true);

  // If none of the inputs use strides/offsets, use a quicker 
  // simpler version of the stream program
  StreamCache::SetType program_type = StreamCache::NO_OFFSET_STRIDE;
  for (ShProgramNode::ChannelList::const_iterator I = program->begin_channels();
       I != program->end_channels(); ++I) {
    if ((*I)->stride() != 1 || (*I)->offset() != 0) {
      program_type = StreamCache::FULL;
      break;
    }
  }  

  // Check if all the outputs use the same stride and offsets. If they
  // differ we need do each output in a separate pass
  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
    if ((*I)->stride() != 1 || (*I)->offset() != 0) {
      program_type = StreamCache::FULL;
    }
    if ((*dest.begin())->stride() != (*I)->stride() || 
        (*dest.begin())->offset() != (*I)->offset()) {
      program_type = StreamCache::SINGLE_OUTPUT;
      break;
    }
  }
  if (program_type == StreamCache::SINGLE_OUTPUT)
    max_outputs = 1;

#ifdef SH_DEBUG_FBOS_PRINTTEX
  int num = 0;
  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I, ++num) {
    std::cerr << "output " << num << " memory time " 
              << (*I)->memory()->timestamp() << std::endl;
    (*I)->memory()->findStorage("opengl:texture", PrintStorages());
  }
  num = 0;
  for (ShProgramNode::ChannelList::const_iterator I = program->begin_channels();
       I != program->end_channels(); ++I, ++num) {
    std::cerr << "input " << num << " memory time "
              << (*I)->memory()->timestamp() << std::endl;
    (*I)->memory()->findStorage("opengl:texture", PrintStorages());
  }  
#endif

  SH_GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT));
  FBOCache::instance()->bindFramebuffer();

  ShStream::iterator dest_iter = dest.begin();
  // Run each fragment program
  for (StreamCache::set_iterator I = cache->sets_begin(program_type);
       I != cache->sets_end(program_type); ++I) {
    std::vector<GLuint> draw_buffers;
    int offset = 0, stride = 1, count = 0;
    for (int i = 0; i < max_outputs && dest_iter != dest.end(); ++i,++dest_iter) {
      offset = (*dest_iter)->offset();
      stride = (*dest_iter)->stride();
      count = (*dest_iter)->count();
    
      ShTextureNodePtr tex;
      ShTextureTraits traits(0, ShTextureTraits::SH_FILTER_NONE, 
                             ShTextureTraits::SH_WRAP_REPEAT);
      // TODO: are these always the right dims?
      ShTextureDims dims(SH_TEXTURE_2D);
      tex = new ShTextureNode(dims, (*dest_iter)->size(), 
                              (*dest_iter)->valueType(), traits,
                              cache->tex_size(), cache->tex_size(), 1, count);
      tex->memory((*dest_iter)->memory(), 0);
      texture->bindTexture(tex, GL_COLOR_ATTACHMENT0_EXT+i, true);
      draw_buffers.push_back(GL_COLOR_ATTACHMENT0_EXT+i);
    }
    if (m_draw_buffers_ext == ATI)
      glDrawBuffersATI(draw_buffers.size(), &draw_buffers.front());
    if (m_draw_buffers_ext == ARB)
      glDrawBuffersARB(draw_buffers.size(), &draw_buffers.front());

    FBOCache::instance()->check();
    
    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    shBind(**I);
    TIMING_RESULT(binding);    

#ifdef SH_DEBUG_FBOS_PRINTFP
    {
      ShProgramSet::NodeList::const_iterator i = (*I)->begin();
      (*i)->code()->print(std::cerr);
      ++i;
      (*i)->code()->print(std::cerr);
    }
#endif

    int size = cache->tex_size();
    glViewport(0, 0, size, size);
    
    DECLARE_TIMER(render);
    
    // Generate quad geometry
    glBegin(GL_QUADS); {
      int full_lines_start = offset / size;
      int full_lines_end = (offset+count) / size;
      int last_line_count = (offset+count) % size;
      int first_line_start = offset % size;
      
      if (first_line_start) {
        draw_rectangle(first_line_start, full_lines_start, 
                       size - first_line_start, 1, size);
        ++full_lines_start;
      }
      if (last_line_count) {
        draw_rectangle(0, full_lines_end, last_line_count, 1, size);
      }
      if (full_lines_end != full_lines_start) {
        draw_rectangle(0, full_lines_start, size, 
                       full_lines_end - full_lines_start, size);
      }
    } glEnd();
    
    TIMING_RESULT(render);
    
    DECLARE_TIMER(finish);
    glFinish();
    
    TIMING_RESULT(finish);
    
    // Unbind, just to be safe
    shUnbind(**I);
    
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(FBOStreamException("Could not render"));
      return;
    } 
  }
  
  cache->freeze_inputs(false);

  // Resetting the write flags needs to be done after all the passes
  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
    I->object()->memory()->findStorage("opengl:texture", UnflagWrite());
  }
    
  SH_GL_CHECK_ERROR(glPopAttrib());
  FBOCache::instance()->unbindFramebuffer();
  
  if (old_handle) {
    old_handle->restore();
  }
}

StreamStrategy* FBOStreams::create()
{
  return new FBOStreams();
}

}
