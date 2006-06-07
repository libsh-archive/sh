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
//#define SH_DO_FBO_TIMING

// Turn this on to debug the fragment programs.
//#define SH_DEBUG_PBS_PRINTFP

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

#ifdef SH_DO_FBO_TIMING
#include <sys/time.h>
#include <time.h>
#endif

namespace shgl {

using namespace SH;
using namespace std;

#ifdef SH_DO_FBO_TIMING

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


FBOStreams::FBOStreams(string name)
  : m_name(name), m_setup_vp(false),
    m_draw_buffers_ext(NONE), m_max_draw_buffers(1)
{
}

FBOStreams::~FBOStreams()
{
}

void FBOStreams::init()
{
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
    // The (trivial) vertex program
    m_vp = keep<Position4f>() & keep<TexCoord4f>();
    m_vp.meta("opengl:matching", "generic");
    m_vp.node()->target() = m_name + ":vertex";
    compile(m_vp);
    m_setup_vp = true;
  }
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

static void gl_attribs(const vector<float>& coords)
{
  vector<float>::const_iterator i = coords.begin();
  for (int n = 1; i != coords.end(); ++n) {
    switch (coords.end() - i) {
    case 1:
      glVertexAttrib1f(n, *i);
      ++i;
      break;
    case 2:
      glVertexAttrib2fv(n, &*i);
      i += 2;
      break;
    case 3:
      glVertexAttrib3fv(n, &*i);
      i += 3;
      break;
    default:
      glVertexAttrib4fv(n, &*i);
      i += 4;
      break;
    }
  }
}

static void draw_rectangle(int x, int y, int w, int h, 
                           const vector<float> coords[3])
{
  SH_GL_CHECK_ERROR(glScissor(x, y, w, h));

  glBegin(GL_TRIANGLES);
  gl_attribs(coords[0]);
  glVertex3f(-1.0, -1.0, 0.0);
  gl_attribs(coords[1]);
  glVertex3f(3.0, -1.0, 0.0);
  gl_attribs(coords[2]);
  glVertex3f(-1.0, 3.0, 0.0);
  glEnd();
}

static void draw_1d_stream(int size, const BaseTexture& tex, bool indexed,
                           const ProgramVersion& version, const Stream& inputs)
{
  int dest_width = size;
  int dest_height = size;
  
  SH_GL_CHECK_ERROR(glEnable(GL_SCISSOR_TEST));
  SH_GL_CHECK_ERROR(glViewport(0, 0, dest_width, dest_height));

  int dest_offset, dest_stride, dest_repeat, dest_count;
  tex.get_offset(&dest_offset, 1);
  tex.get_stride(&dest_stride, 1);
  tex.get_repeat(&dest_repeat, 1);
  tex.get_count(&dest_count, 1);

  vector<float> coords[3];
  if (version.index_recalculation) {
    for (int i = 0; i < 3; ++i) {
      coords[i].reserve(inputs.size());
    }
    for (Stream::const_iterator in = inputs.begin(); in != inputs.end(); ++in) {
      int offset, stride, repeat, count;
      in->get_offset(&offset, 1);
      in->get_stride(&stride, 1);
      in->get_repeat(&repeat, 1);
      in->get_count(&count, 1);

      //
      // What we'd like to do here is have i=(width*y+x)/count as the input to the
      // fragment shader so that a simple count*frac(i) would give us the integer
      // position in the stream. frac() however isn't floating point error tolerant
      // so we have to bias the output by 1.0/(2*count). We remove this bias during
      // the offset/stride computation in SplitProgram1DRecompute
      //

      // Rasterization is a linear transformation of the form
      //
      // |r|    |a11 a12 a13| |x + bias|
      // |s|  = |a21 a22 a23|.|y + bias|
      // |t|    |a31 a32 a33| |   1    |
      // |q|    |a41 a42 a43|
      //
      // Each input uses one component of the vertex attribute. For one input
      // the calculation is
      //
      // i = a1(x + bias) + a2(y + bias) + a3
      //
      // hence if the desired computation is 
      //
      // i = (dest_width*x + dest_width*dest_height*y - dest_offset) / count
      //
      // the coefficients are
      //
      float bias = 1.0/(2*dest_width); // note that dest_width == dest_height
      float a1 = (float)dest_width / count;
      float a2 = (float)dest_width * dest_height / count;
      float a3 = -dest_offset/(float)count - (a1 + a2) * bias + 1.0/(2*count);
      //
      // The vertices of our triangle are at (0,0), (2,0) and (0,2) hence 
      // the coordinate at the vertices are
      //
      // (0,0) -> a3
      // (2,0) -> 2*a1 + a3
      // (0,2) -> 2*a2 + a3
      //
      coords[0].push_back(a3);
      coords[1].push_back(2*a1 + a3);
      coords[2].push_back(2*a2 + a3);
    }
  }
  else {
    for (int i = 0; i < 3; ++i) {
      coords[i].reserve(2);
    }
    coords[0].push_back(0);
    coords[0].push_back(0);
    coords[1].push_back(indexed ? 2*dest_width : 2);
    coords[1].push_back(0);
    coords[2].push_back(0);
    coords[2].push_back(indexed ? 2*dest_height : 2);
  }

  DECLARE_TIMER(render);

  // Generate geometry
  int full_lines_start = dest_offset / size;
  int full_lines_end = (dest_offset+dest_count*dest_stride) / size;
  int last_line_count = (dest_offset+dest_count*dest_stride) % size;
  int first_line_start = dest_offset % size;
  int first_line_count = dest_count*dest_stride > size-first_line_start ?
                         size-first_line_start : dest_count*dest_stride;
    
  if (first_line_start) {
    draw_rectangle(first_line_start, full_lines_start, 
                   first_line_count, 1, coords);
    ++full_lines_start;
  }
  if (full_lines_end >= full_lines_start && last_line_count) {
    draw_rectangle(0, full_lines_end, last_line_count, 1, coords);
  }
  if (full_lines_end > full_lines_start) {
    draw_rectangle(0, full_lines_start, size,
                   full_lines_end - full_lines_start, coords);
  }

  TIMING_RESULT(render);    
}

static void draw_2d_stream(int dest_width, int dest_height, const BaseTexture& tex,
                           const ProgramVersion& version, int indexed,
                           const Stream& inputs)
{
  int dest_offset[2], dest_stride[2], dest_count[2];
  tex.get_offset(dest_offset, 2);
  tex.get_stride(dest_stride, 2);
  tex.get_count(dest_count, 2);

  vector<float> coords[3];
  if (version.index_recalculation) {
    for (int i = 0; i < 3; ++i) {
      coords[i].reserve(2*inputs.size());
    }
    for (Stream::const_iterator in = inputs.begin(); in != inputs.end(); ++in) {
      int count[2];
      in->get_count(count, 2);

      //
      // See above about viewing rasterization as a linear transformation
      //
      float x_bias = 1.0/(2*dest_width);
      float x_a1 = (float)dest_width / count[0];
      float x_a3 = -dest_offset[0]/(float)count[0] - x_a1*x_bias + 1.0/(2*count[0]);

      float y_bias = 1.0/(2*dest_height);
      float y_a2 = (float)dest_height/ count[1];
      float y_a3 = -dest_offset[1]/(float)count[1] - y_a2 * y_bias + 1.0/(2*count[1]);

      coords[0].push_back(x_a3);
      coords[1].push_back(2*x_a1 + x_a3);
      coords[2].push_back(x_a3);
      
      coords[0].push_back(y_a3);
      coords[1].push_back(y_a3);
      coords[2].push_back(2*y_a2 + y_a3);
    }
  }
  else {
    for (int i = 0; i < 3; ++i) {
      coords[i].reserve(2);
    }
    coords[0].push_back(0);
    coords[0].push_back(0);
    coords[1].push_back(indexed ? 2*dest_width : 2);
    coords[1].push_back(0);
    coords[2].push_back(0);
    coords[2].push_back(indexed ? 2*dest_height : 2);
  }

  SH_GL_CHECK_ERROR(glViewport(0, 0, dest_width, dest_height));
  SH_GL_CHECK_ERROR(glEnable(GL_SCISSOR_TEST));

  draw_rectangle(dest_offset[0], dest_offset[1],
                 dest_stride[0]*dest_count[0], dest_stride[1]*dest_count[1], coords);
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

static void stream_to_texture(const TextureNodePtr& tex, TextureDims& dims,
                              int& width, int& height, int& depth, bool indexed)
{
  switch (tex->dims()) {
  case SH_TEXTURE_1D:
  case SH_TEXTURE_2D:
    if (indexed) {
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
      
  if (tex->dims() == SH_TEXTURE_1D) {
    int tex_size = 1;
    while (tex_size * tex_size < tex->width())
      tex_size <<= 1;
    width = tex_size;
    height = tex_size;
    depth = 1;
  }
  else {
    width = tex->width();
    height = tex->height();
    depth = tex->depth();
  }
}


static ProgramVersion choose_program_version(const Stream& input, const Stream& output)
{
  ProgramVersion version;
  version.dimension = dimension(*output.begin());
  version.index_recalculation = false;
  version.single_output = false;

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
        I->node()->depth() != output.begin()->node()->depth() ||
        I->node()->size() != output.begin()->node()->size()) {
      version.single_output = true;
    }
    for (int i = 0; i < dimension(*I); ++i) {
      if (offset[i] != first_offset[i] ||
          stride[i] != first_stride[i] ||
          count[i] != first_count[i]) {
        version.single_output = true;
      }
      if (offset[i] != 0 || stride[i] != 1) {
        version.index_recalculation = true;
      }
    }
  }
  
  for (Stream::const_iterator I = input.begin(); I != input.end(); ++I) {
    int stride[3], offset[3];
    I->get_offset(offset, 3);
    I->get_stride(stride, 3);
    for (int i = 0; i < dimension(*I); ++i) {
      if (offset[i] != 0 || stride[i] != 1) {
        version.index_recalculation = true;
      }
    }
    if (I->node()->dims() == SH_TEXTURE_1D) {
      for (Stream::const_iterator J = output.begin(); J != output.end(); ++J) {
        // If the input and output sizes don't match, the generated 2d textures
        // will be of different size, so we need to perform the index calculation
        // even if the stride and offset isn't used
        if (I->node()->width() != J->node()->width()) {
          version.index_recalculation = true;
        }
      }
    }
  }
  
  return version;
}

void FBOStreams::execute(const Program& program,
                         Stream& dest, TextureStrategy *texture)
{
  // Get rid of const so that we can add the StreamCache to it
  ProgramNodePtr program_node = shref_const_cast<ProgramNode>(program.node());

  // Make sure all inputs are bound
  if (program_node->inputs.size() > program.binding_spec.size()) {
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
  check_stream(program.stream_inputs, dest.begin()->node()->dims());

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

  // Setup the vertex program and query relevant extensions
  init();

  if (m_float_extension == ARB_NO_FLOAT_EXT) {
    error(FBOStreamException("Cannot execute stream program, floating point texture support not found"));
    return;
  }
  
  TIMING_RESULT(vpsetup);

  GLint max_outputs;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_outputs));

  // --- Set up the fragment programs and such
  StreamCache* cache = program_node->get_info<StreamCache>();
  if (!cache) {
    cache = new StreamCache(program_node.object(), m_vp.node().object(), 
                            max_outputs, m_float_extension);
    program_node->add_info(cache);
  }
    
  ProgramVersion program_version;
  program_version = choose_program_version(program.stream_inputs, dest);
  if (program_version.single_output)
    max_outputs = 1;

  const ProgramVersionCache& program_cache = 
    cache->get_program_cache(program_version, program.binding_spec);

  for (Stream::const_iterator I = program.stream_inputs.begin();
       I != program.stream_inputs.end(); ++I) {
    I->node()->memory(0)->freeze(true);
  }

#ifdef SH_DEBUG_FBOS_PRINTTEX
  int num = 0;
  for (Stream::iterator I = dest.begin(); I != dest.end(); ++I, ++num) {
    std::cerr << "output " << num << " memory time " 
              << I->node()->memory(0)->timestamp() << std::endl;
    I->node()->memory(0)->findStorage("opengl:texture", PrintStorages());
  }
  num = 0;
  for (Stream::const_iterator I = program.stream_inputs.begin();
       I != program.stream_inputs.end(); ++I, ++num) {
    std::cerr << "input " << num << " memory time "
              << I->node()->memory(0)->timestamp() << std::endl;
    I->node()->memory(0)->findStorage("opengl:texture", PrintStorages());
  }  
#endif

  SH_GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT | GL_SCISSOR_BIT | GL_ENABLE_BIT));
  FBOCache::instance()->bindFramebuffer();

  Stream::iterator dest_iter = dest.begin();
  ProgramVersionCache::iterator I;
  // Run each fragment program
  for (I = program_cache.begin(); I != program_cache.end(); ++I) {

    std::vector<GLuint> draw_buffers;    
    Stream::iterator dest_tex;
    int dest_width = 0, dest_height = 0, dest_depth = 0;
    for (int i = 0; i < max_outputs && dest_iter != dest.end(); ++i,++dest_iter) {
      dest_tex = dest_iter;
      TextureNodePtr dest_node = dest_iter->node();
      
      TextureDims dims;
      stream_to_texture(dest_node, dims, dest_width, dest_height, dest_depth,
                        m_float_extension == ARB_NV_FLOAT_BUFFER);
            
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
    
    (*I)->update_uniforms(program.uniform_inputs);
    (*I)->update_channels(program.stream_inputs, *dest_tex);

#ifdef SH_DEBUG_FBOS_PRINTFP
    {
      ProgramSet::NodeList::const_iterator i = (*I)->program_set()->begin();
      (*i)->code()->print(std::cerr);
      ++i;
      (*i)->code()->print(std::cerr);
    }
#endif

    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    bind(*(*I)->program_set());
    TIMING_RESULT(binding);    

    switch (dest_tex->node()->dims()) {
    case SH_TEXTURE_1D:
      SH_DEBUG_ASSERT(dest_width == dest_height);
      draw_1d_stream(dest_width, *dest_tex,
                     m_float_extension == ARB_NV_FLOAT_BUFFER,
                     program_version, program.stream_inputs);
      break;
    case SH_TEXTURE_2D:
      draw_2d_stream(dest_width, dest_height, *dest_tex, program_version,
                     m_float_extension == ARB_NV_FLOAT_BUFFER,
                     program.stream_inputs);
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
 
    // Unbind, just to be safe
    unbind(*(*I)->program_set());
    
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      error(FBOStreamException("Could not render"));
      return;
    } 
  }
  
  for (Stream::const_iterator I = program.stream_inputs.begin();
       I != program.stream_inputs.end(); ++I) {
    I->node()->memory(0)->freeze(false);
  }

  // Resetting the write flags needs to be done after all the passes
  for (Stream::iterator I = dest.begin(); I != dest.end(); ++I) {
    I->node()->memory(0)->findStorage("opengl:texture", UnflagWrite());
  }

  SH_GL_CHECK_ERROR(glPopAttrib());
  FBOCache::instance()->unbindFramebuffer();
  
  if (old_handle) {
    old_handle->restore();
  }
}

FBOStreams::GatherData& 
FBOStreams::get_gather_data(TextureDims src_dims, bool src_two_comp,
                            TextureDims idx_dims, bool idx_two_comp)
{
  std::pair<TexSpec, TexSpec> ver = make_pair(make_pair(src_dims, src_two_comp),
                                              make_pair(idx_dims, idx_two_comp));
  GatherCache::iterator cached = m_gather_cache.find(ver);
  if (cached != m_gather_cache.end()) {
    return cached->second;
  }

  GatherData& data = m_gather_cache[ver];
  if (src_two_comp)
    data.src = new TextureNode(src_dims, 2, SH_FLOAT, ArrayTraits(), 1, 1, 1);
  else 
    data.src = new TextureNode(src_dims, 4, SH_FLOAT, ArrayTraits(), 1, 1, 1);

  data.index = new TextureNode(idx_dims, 1, SH_FLOAT, ArrayTraits(), 1, 1, 1);

  Program gather_vsh = keep<Position4f>() & keep<TexCoord2f>();
  gather_vsh.target() = m_name + ":vertex";

  Program gather_fsh = SH_BEGIN_PROGRAM(m_name + ":fragment") {
    InputTexCoord2f SH_DECL(input);
    TexCoord3f SH_DECL(coord);
    
    Statement stmt(coord(2), Variable(data.index), OP_TEX, input);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    
    coord(2) = mad(coord(2), -data.size(0), data.size(3));
    coord(0) = frac(coord(2));
    coord(1) = (coord | data.size(0,1,2)) + data.size(3);
    
    if (src_two_comp) {
      Attrib2f tex_data;
      Statement stmt2(tex_data, Variable(data.src), OP_TEX, coord);
      Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt2);
      OutputAttrib4f out;
      out = tex_data(0, 1, 0, 1);
    }
    else {
      OutputAttrib4f out;
      Statement stmt2(out, Variable(data.src), OP_TEX, coord);
      Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt2);
    }    
  } SH_END_PROGRAM

  data.program = new ProgramSet(gather_vsh, gather_fsh);

  return data;
}

void FBOStreams::gather(const BaseTexture& dest_stream,
                        const BaseTexture& src_stream, 
                        const BaseTexture& index_stream, 
                        TextureStrategy* texture_strategy)
{
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
  
  // Setup the vertex program and query relevant extensions
  init();

  // Convert the index and src stream textures into texture nodes we 
  // can actually use (i.e. 1D -> 2D)  
  int dest_size[3] = {0, 0, 0};
  int src_size[3] = {0, 0, 0};
  TextureDims src_dims, dest_dims;
  stream_to_texture(src_stream.node(), src_dims, 
                    src_size[0], src_size[1], src_size[2],
                    m_float_extension == ARB_NV_FLOAT_BUFFER);  
  stream_to_texture(index_stream.node(), dest_dims,
                    dest_size[0], dest_size[1], dest_size[2],
                    m_float_extension == ARB_NV_FLOAT_BUFFER);

  // Create the texture nodes we are going to use
  TextureNodePtr src = new TextureNode(src_dims,
                                       src_stream.node()->size(),
                                       src_stream.node()->valueType(),
                                       src_stream.node()->traits(), 
                                       src_size[0], src_size[1], src_size[2]);
  src->memory(src_stream.node()->memory(0), 0);
  
  TextureNodePtr index = new TextureNode(dest_dims,
                                         index_stream.node()->size(),
                                         index_stream.node()->valueType(),
                                         index_stream.node()->traits(), 
                                         dest_size[0], dest_size[1], dest_size[2]);
  index->memory(index_stream.node()->memory(0), 0);

  TextureNodePtr dest = new TextureNode(dest_dims,
                                        src_stream.node()->size(),
                                        src_stream.node()->valueType(),
                                        src_stream.node()->traits(), 
                                        dest_size[0], dest_size[1], dest_size[2]);
  dest->memory(dest_stream.node()->memory(0), 0);
  
  // HACK ALERT! binding the texture here ensures that there is an upto
  // date opengl storage. The generic gathering program will then simply
  // use the storage even though it doesn't know the correct texture
  // parameters
  texture_strategy->bindTexture(src, GL_TEXTURE0, false);
  texture_strategy->bindTexture(index, GL_TEXTURE0, false);
  
  // Generate the gathering program
  GatherData& gather_data = 
    get_gather_data(src_dims, src_stream.node()->size() == 2,
                    dest_dims, index_stream.node()->size() == 2);

  // Setup the gethering program parameters
  gather_data.src->memory(src->memory(0), 0);
  gather_data.src->setTexSize(src->width(), src->height());
  gather_data.index->memory(index->memory(0), 0);
  gather_data.index->setTexSize(index->width(), index->height());
  
  float size[4];
  size[0] = -1.0/src_size[0];
  size[1] = 0;
  size[2] = 1.0/src_size[1];
  size[3] = 1.0/2/src_size[0];
  gather_data.size.setValues(size);

#if 0
  {
    ProgramSet::NodeList::const_iterator i = gather_data.program->begin();
    (*i)->code()->print(std::cerr);
    ++i;
    (*i)->code()->print(std::cerr);
  }
#endif

  // Setup the rendering destination
  SH_GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT));
  FBOCache::instance()->bindFramebuffer();
  
  texture_strategy->bindTexture(dest, GL_COLOR_ATTACHMENT0_EXT, true);
    
  FBOCache::instance()->check();

  // Gather.
  bind(*gather_data.program);
  
  glViewport(0, 0, dest_size[0], dest_size[1]);
  glBegin(GL_TRIANGLES);
  glTexCoord2f(0, 0);
  glVertex3f(-1.0, -1.0, 0);
  glTexCoord2f(m_float_extension == ARB_NV_FLOAT_BUFFER ? 2*dest_size[0] : 2, 0);
  glVertex3f(3.0, -1.0, 0);
  glTexCoord2f(0, m_float_extension == ARB_NV_FLOAT_BUFFER ? 2*dest_size[1] : 2);
  glVertex3f(-1.0, 3.0, 0);
  glEnd();
  
  unbind(*gather_data.program);
  
  SH_GL_CHECK_ERROR(glPopAttrib());
  FBOCache::instance()->unbindFramebuffer();

  dest->memory(0)->findStorage("opengl:texture", UnflagWrite());
  
  if (old_handle) {
    old_handle->restore();
  }
}

StreamStrategy* FBOStreams::create()
{
  SH_DEBUG_ASSERT(false);
  return 0;
}

}
