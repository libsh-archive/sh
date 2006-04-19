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
//#define DEBUG_PBS_PRINTFP

#include <map>
#include <fstream>
#include <cstdlib>
#include <list>
#include <set>

#include "sh.hpp"
#include "Optimizations.hpp"
#include "Exception.hpp"
#include "Error.hpp"
#include "TypeInfo.hpp"
#include "Variant.hpp"
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

class FBOStreamException : public Exception {
public:
  FBOStreamException(const std::string& message)
    : Exception("FBO Stream Execution: " + message)
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
  bool operator()(const StoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) return false;
    
    t->write(false);
    
    // Make sure we iterate over all of them
    return false;
  }
};

struct PrintStorages {
  bool operator()(const StoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) return false;
    std::cerr << *t << std::endl;
    return false;
  }
};

static void draw_rectangle(float x, float y, float w, float h, 
                           float tex_w, float tex_h, float coord_w, float coord_h)
{
  glTexCoord4f(x/coord_w, y/coord_h, 0.0, 1.0);
  glVertex3f(2.0*x/tex_w-1.0, 2.0*y/tex_h-1.0, 0.0);
  glTexCoord4f((x+w)/coord_w, y/coord_h, 0.0, 1.0);
  glVertex3f(2.0*(x+w)/tex_w-1.0, 2.0*y/tex_h-1.0, 0.0);
  glTexCoord4f((x+w)/coord_w, (y+h)/coord_h, 0.0, 1.0);
  glVertex3f(2.0*(x+w)/tex_w-1.0, 2.0*(y+h)/tex_h-1.0, 0.0);
  glTexCoord4f(x/coord_w, (y+h)/coord_h, 0.0, 1.0);
  glVertex3f(2.0*x/tex_w-1.0, 2.0*(y+h)/tex_h-1.0, 0.0);
}

static void draw_rectangle(float x, float y, float w, float h, float size, float coords)
{
  draw_rectangle(x, y, w, h, size, size, coords, coords);
}

static void draw_1d_stream(int size, const BaseTexture& tex, bool indexed)
{
  int coords = indexed ? 1 : size;
  glViewport(0, 0, size, size);

  DECLARE_TIMER(render);

  int offset, stride, count;
  tex.get_offset(&offset, 1);
  tex.get_stride(&stride, 1);
  tex.get_count(&count, 1);

  // Generate quad geometry
  glBegin(GL_QUADS); {
    int full_lines_start = offset / size;
    int full_lines_end = (offset+count*stride) / size;
    int last_line_count = (offset+count*stride) % size;
    int first_line_start = offset % size;
    
    if (first_line_start) {
      draw_rectangle(first_line_start, full_lines_start, 
                     size - first_line_start, 1, size, coords);
      ++full_lines_start;
    }
    if (last_line_count) {
      draw_rectangle(0, full_lines_end, last_line_count, 1, size, coords);
    }
    if (full_lines_end != full_lines_start) {
      draw_rectangle(0, full_lines_start, size, 
                     full_lines_end - full_lines_start, size, coords);
    }
  } glEnd();

  TIMING_RESULT(render);    
}

static void draw_2d_stream(int width, int height,
                           const BaseTexture& tex, int indexed)
{
  int coords_width = indexed ? 1 : width;
  int coords_height = indexed ? 1 : height;
  glViewport(0, 0, width, height);
  
  int offset[2], stride[2], count[2];
  tex.get_offset(offset, 2);
  tex.get_stride(stride, 2);
  tex.get_count(count, 2);
  
  glBegin(GL_QUADS);
  draw_rectangle(offset[0], offset[1], stride[0]*count[0], stride[1]*count[1],
                 width, height, coords_width, coords_height);  
  glEnd();
}

static void check_stream(const Stream& stream, TextureDims dims)
{
  for(Stream::const_iterator I = stream.begin(); I != stream.end(); ++I) {
    if (I->node()->dims() != dims) {
      error(FBOStreamException("All stream outputs must be of the same dimension"));
    }
  }
}

static int dimension(const BaseTexture& tex)
{
  switch (tex.node()->dims()) {
  case SH_TEXTURE_1D:
    return 1;
  case SH_TEXTURE_2D:
    return 2;
  case SH_TEXTURE_3D:
    return 3;
  default:
    error(FBOStreamException("Only SH_TEXTURE_{1,2,3}D can be used as a stream"));
    return 0;
  }
}

static int choose_program_version(const Stream& input, const Stream& output)
{
  int os_versions[3] = { StreamCache::OS_1D, 
                         StreamCache::OS_2D,
                         StreamCache::OS_3D };
  
  // TODO what about 3D?
  int version = StreamCache::OS_NONE_2D;
  bool single_output = false;

  int first_offset[3], first_stride[3], first_count[3];
  output.begin()->get_offset(first_offset, 1);
  output.begin()->get_stride(first_stride, 1);
  output.begin()->get_count(first_count, 1);
  for (Stream::const_iterator I = output.begin(); I != output.end(); ++I) {
    int offset[3], stride[3], count[3];
    I->get_offset(offset, 3);
    I->get_stride(stride, 3);
    I->get_count(count, 3);
    // If the outputs differ in any way, we have to render them separately
    if (I->node()->width() != output.begin()->node()->width() ||
        I->node()->height() != output.begin()->node()->height() ||
        I->node()->depth() != output.begin()->node()->depth()) {
      single_output = true;
    }
    for (int i = 0; i < dimension(*I); ++i) {
      if (offset[i] != first_offset[i] ||
          stride[i] != first_stride[i] ||
          count[i] != first_count[i]) {
        single_output = true;
      }
      if (offset[i] != 0 || stride[i] != 1) {
        version = os_versions[dimension(*I)-1];
      }
    }
  }
  
  for (Stream::const_iterator I = input.begin(); I != input.end(); ++I) {
    int stride[3], offset[3];
    I->get_offset(offset, 3);
    I->get_stride(stride, 3);
    for (int i = 0; i < dimension(*I); ++i) {
      if (offset[i] != 0 || stride[i] != 1) {
        version = os_versions[dimension(*I)-1];
      }
    }
    if (I->node()->dims() == SH_TEXTURE_1D) {
      for (Stream::const_iterator J = output.begin(); J != output.end(); ++J) {
        // If the input and output sizes don't match, the generated 2d textures
        // will be of different size, so we need to perform the index calculation
        // even if the stride and offset isn't used
        if (I->node()->width() != J->node()->width()) {
          version = StreamCache::OS_1D;
        }
      }
    }
  }
  
  if (single_output) {
    version |= StreamCache::SINGLE_OUTPUT;
  }
  return version;
}

void FBOStreams::execute(const Program& program,
                         Stream& dest, TextureStrategy *texture)
{
  // Get rid of const so that we can add the StreamCache to it
  ProgramNodePtr program_node = shref_const_cast<ProgramNode>(program.node());

  // Make sure all inputs are bound
  if (program_node->inputs.size() > program.stream_inputs().size()) {
    error(FBOStreamException("Stream program has unbound inputs, and can hence not be executed."));
    return;
  }

  if (dest.size() == 0) {
    SH_DEBUG_WARN("Stream program has no outputs?");
    return;
  }

  if (program.node()->outputs.size() != dest.size()) {
    SH_DEBUG_ERROR("Number of stream program outputs ("
                   << program_node->outputs.size()
                   << ") does not match number of destinations ("
                   << dest.size()
                   << ").");
    return;
  }
  
  check_stream(dest, dest.begin()->node()->dims());
  check_stream(program.stream_inputs(), dest.begin()->node()->dims());

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
          GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &m_max_draw_buffers));
          m_draw_buffers_ext = ATI;
        } else {
#endif
          GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &m_max_draw_buffers));
          m_draw_buffers_ext = ARB;
#ifdef ATI_draw_buffers
        }
#endif
      }
      if (extstr.find("ATI_texture_float") != string::npos) {
        m_float_extension = ARB_ATI_PIXEL_FORMAT_FLOAT;
      }
      else if (extstr.find("NV_float_buffer") != string::npos) {
        m_float_extension = ARB_NV_FLOAT_BUFFER;
      }
      else {
        m_float_extension = ARB_NO_FLOAT_EXT;
      }
    }
    GL_CHECK_ERROR(glGenFramebuffersEXT(1, &m_framebuffer));

    // The (trivial) vertex program
    m_vp = keep<Position4f>() & keep<TexCoord4f>();
    m_vp.node()->target() = get_target_backend(program_node) + "vertex";
    compile(m_vp);
    m_setup_vp = true;
  }
  
  if (m_float_extension == ARB_NO_FLOAT_EXT) {
    error(FBOStreamException("Cannot execute stream program, floating point texture support not found"));
    return;
  }
  
  TIMING_RESULT(vpsetup);

  GLint max_outputs;
  GL_CHECK_ERROR(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_outputs));

  // --- Set up the fragment programs and such
  StreamCache* cache = program_node->get_info<StreamCache>();
  if (!cache) {
    cache = new StreamCache(program_node.object(), m_vp.node().object(), 
                            max_outputs, m_float_extension);
    program_node->add_info(cache);
  }
  
  StreamCache::ProgramVersion program_version;
  program_version = choose_program_version(program.stream_inputs(), dest);
  if (program_version & StreamCache::SINGLE_OUTPUT)
    max_outputs = 1;

  cache->generate_programs(program_version);

  for (Stream::const_iterator I = program.stream_inputs().begin();
       I != program.stream_inputs().end(); ++I) {
    I->node()->memory(0)->freeze(true);
  }

#ifdef DEBUG_FBOS_PRINTTEX
  int num = 0;
  for (Stream::iterator I = dest.begin(); I != dest.end(); ++I, ++num) {
    std::cerr << "output " << num << " memory time " 
              << I->node()->memory(0)->timestamp() << std::endl;
    I->node()->memory(0)->findStorage("opengl:texture", PrintStorages());
  }
  num = 0;
  for (ProgramNode::ChannelList::const_iterator I = program->begin_channels();
       I != program->end_channels(); ++I, ++num) {
    std::cerr << "input " << num << " memory time "
              << (*I)->memory()->timestamp() << std::endl;
    (*I)->memory()->findStorage("opengl:texture", PrintStorages());
  }  
#endif

  GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT));
  FBOCache::instance()->bindFramebuffer();

  Stream::iterator dest_iter = dest.begin();
  // Run each fragment program
  for (StreamCache::set_iterator I = cache->sets_begin(program_version);
       I != cache->sets_end(program_version); ++I) {

    std::vector<GLuint> draw_buffers;    
    Stream::iterator dest_tex;
    int dest_width = 0, dest_height = 0, dest_depth = 0;
    for (int i = 0; i < max_outputs && dest_iter != dest.end(); ++i,++dest_iter) {
      dest_tex = dest_iter;
      TextureNodePtr dest_node = dest_iter->node();
      
      TextureDims dims = SH_TEXTURE_2D;
      switch (dest_node->dims()) {
      case SH_TEXTURE_1D:
      case SH_TEXTURE_2D:
        if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
          dims = SH_TEXTURE_RECT;
        } else {
          dims = SH_TEXTURE_2D;
        }
        break;
      case SH_TEXTURE_3D:
        dims = SH_TEXTURE_3D;
        break;
      default:
        SH_DEBUG_ASSERT(false);
        break;
      }
      
      if (dest_node->dims() == SH_TEXTURE_1D) {
        int tex_size = 1;
        while (tex_size * tex_size < dest_node->width())
          tex_size <<= 1;
        dest_width = tex_size;
        dest_height = tex_size;
        dest_depth = 1;
      }
      else {
        dest_width = dest_node->width();
        dest_height = dest_node->height();
        dest_depth = dest_node->depth();
      }
      
      TextureNodePtr tex = new TextureNode(dims, dest_node->size(), 
                              dest_node->valueType(), ArrayTraits(),
                              dest_width, dest_height, dest_depth);
      tex->memory(dest_node->memory(0), 0);
      texture->bindTexture(tex, GL_COLOR_ATTACHMENT0_EXT+i, true);
      draw_buffers.push_back(GL_COLOR_ATTACHMENT0_EXT+i);
    }
    if (m_draw_buffers_ext == ATI)
      glDrawBuffersATI(draw_buffers.size(), &draw_buffers.front());
    if (m_draw_buffers_ext == ARB)
      glDrawBuffersARB(draw_buffers.size(), &draw_buffers.front());

    FBOCache::instance()->check();
    
    cache->update_channels(program_version, program.stream_inputs(),
                           *dest_tex, dest_width, dest_height, dest_depth);

#ifdef DEBUG_FBOS_PRINTFP
    {
      ProgramSet::NodeList::const_iterator i = (*I)->begin();
      (*i)->code()->print(std::cerr);
      ++i;
      (*i)->code()->print(std::cerr);
    }
#endif

    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    bind(**I);
    TIMING_RESULT(binding);    

    switch (dest_tex->node()->dims()) {
    case SH_TEXTURE_1D:
      SH_DEBUG_ASSERT(dest_width == dest_height);
      draw_1d_stream(dest_width, *dest_tex,
                     m_float_extension == ARB_NV_FLOAT_BUFFER);
      break;
    case SH_TEXTURE_2D:
      draw_2d_stream(dest_width, dest_height, *dest_tex,
                     m_float_extension == ARB_NV_FLOAT_BUFFER);
      break;
/*
    case SH_TEXTURE_3D:
      draw_3d_stream(dest_node);
      break;
*/
    default:
      SH_DEBUG_ASSERT(false);
      break;
    }
 
    DECLARE_TIMER(finish);
    glFinish();
    
    TIMING_RESULT(finish);
    
    // Unbind, just to be safe
    unbind(**I);
    
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      error(FBOStreamException("Could not render"));
      return;
    } 
  }
  
  for (Stream::const_iterator I = program.stream_inputs().begin();
       I != program.stream_inputs().end(); ++I) {
    I->node()->memory(0)->freeze(false);
  }

  // Resetting the write flags needs to be done after all the passes
  for (Stream::iterator I = dest.begin(); I != dest.end(); ++I) {
    I->node()->memory(0)->findStorage("opengl:texture", UnflagWrite());
  }
    
  GL_CHECK_ERROR(glPopAttrib());
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
