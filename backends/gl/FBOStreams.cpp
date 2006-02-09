// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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


static void check_framebuffer()
{
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  char *error = 0;
  switch (status) {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    return;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_FORMATS";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    break;
  default:
    error = "Unknown error";
    break;
  }
  SH_DEBUG_PRINT("GL Framebuffer error " << error);
}

// Extract the backend name from the target if there is one (including the colon)
static string get_target_backend(const ShProgramNodeCPtr& program)
{
  const string& target = program->target();
  string::size_type colon_pos = target.find(":");

  if (target.npos == colon_pos) {
    return "";
  } else {
    return target.substr(0, colon_pos+1); // includes the colon
  }
}

class FBOStreamException : public ShException {
public:
  FBOStreamException(const std::string& message)
    : ShException("FBO Stream Execution: " + message)
  {
  }
};


class FBOStreamCache : public ShInfo {
public:
  FBOStreamCache(ShProgramNode* stream_program,
                 ShProgramNode* vertex_program,
                 int max_outputs);

  ShInfo* clone() const;

  void update_channels(int width, int height);

  void build_sets(ShProgramNode* vertex_program);
  
  void freeze_inputs(bool state);

  typedef std::list<ShProgramSetPtr>::iterator set_iterator;
  typedef std::list<ShProgramSetPtr>::const_iterator set_const_iterator;
  typedef std::list<ShProgramNodePtr>::iterator program_iterator;
  typedef std::list<ShProgramNodePtr>::const_iterator program_const_iterator;

  set_iterator sets_begin() { return m_program_sets.begin(); }
  set_iterator sets_end() { return m_program_sets.end(); }
  set_const_iterator sets_begin() const { return m_program_sets.begin(); }
  set_const_iterator sets_end() const { return m_program_sets.end(); }

  program_iterator programs_begin() { return m_programs.begin(); }
  program_iterator programs_end() { return m_programs.end(); }
  program_const_iterator programs_begin() const { return m_programs.begin(); }
  program_const_iterator programs_end() const { return m_programs.end(); }
  
private:
  ShProgramNode* m_stream_program;
  ShProgramNode* m_vertex_program;
  ChannelMap m_channel_map;
  std::list<ShProgramNodePtr> m_programs;
  std::list<ShProgramSetPtr> m_program_sets;
  int m_max_outputs;

  FBOStreamCache(const FBOStreamCache& other);
  FBOStreamCache& operator=(const FBOStreamCache& other);
};

FBOStreamCache::FBOStreamCache(ShProgramNode* stream_program,
                               ShProgramNode* vertex_program,
                               int max_outputs)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program),
    m_max_outputs(max_outputs)
{
  ShTextureDims dims(SH_TEXTURE_2D);

  ChannelGatherer gatherer(m_channel_map, dims);
  stream_program->ctrlGraph->dfs(gatherer);

  split_program(stream_program, m_programs, 
                get_target_backend(stream_program) + "fragment",
                max_outputs);

  for (std::list<ShProgramNodePtr>::iterator I = m_programs.begin();
       I != m_programs.end(); ++I) {
    ShProgram with_tc = ShProgram(*I) & lose<ShTexCoord2f>("streamcoord");
    
    TexFetcher fetcher(m_channel_map, with_tc.node()->inputs.back(),
                       dims == SH_TEXTURE_RECT, with_tc.node());
    with_tc.node()->ctrlGraph->dfs(fetcher);

    optimize(with_tc);
    
    *I = with_tc.node();
  }

  build_sets(vertex_program);
}

ShInfo* FBOStreamCache::clone() const
{
  return new FBOStreamCache(m_stream_program, m_vertex_program, m_max_outputs);
}

void FBOStreamCache::update_channels(int width, int height)
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ShChannelNode* channel = I->first.object();
    ShTextureNode* texture = I->second.object();

    SH_DEBUG_ASSERT(channel);
    SH_DEBUG_ASSERT(texture);

    texture->memory(channel->memory(), 0);
    texture->setTexSize(width, height);
    texture->count(channel->count());
  }
}

void FBOStreamCache::freeze_inputs(bool state)
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    I->second->memory(0)->freeze(state);
  }
}

void FBOStreamCache::build_sets(ShProgramNode* vertex_program)
{
  m_vertex_program = vertex_program;
  m_program_sets.clear();
  for (std::list<ShProgramNodePtr>::iterator I = m_programs.begin(); I != m_programs.end(); ++I) {
    m_program_sets.push_back(new ShProgramSet(ShProgram(vertex_program),
                                              ShProgram(*I)));
  }
}

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

void FBOStreams::execute(const ShProgramNodeCPtr& program_const,
                         ShStream& dest, TextureStrategy *texture)
{
  // Let's get rid of that constness... Yes, yes, I know...
  ShProgramNodePtr program = shref_const_cast<ShProgramNode>(program_const);
  
  DECLARE_TIMER(overhead);

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
  TIMING_RESULT(overhead);

  // TODO: Check that the size of each output matches the size of each dest.

  int count = (*dest.begin())->count();

  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
    if (count != (*I)->count()) {
      shError(FBOStreamException("All stream outputs must be of the same size"));
    }
  }
  
  // Pick a size for the texture that just fits the output data.
  int tex_size = 1;
  {
    // The texture size needs to be as large as the area that will be
    // read.  In the case of 2-component output, we need to read an
    // extra element.
    int tex_elem_count = count;
    if (2 == count) tex_elem_count = 3;
    
    while (tex_size * tex_size < tex_elem_count) {
      tex_size <<= 1;
    }
  }

  PBufferFactory* factory = PBufferFactory::instance();
  PBufferContextPtr context = factory->get_context(1,1);
  PBufferHandlePtr old_handle = context->activate();

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
  FBOStreamCache* cache = program->get_info<FBOStreamCache>();
  if (!cache) {
    cache = new FBOStreamCache(program.object(), m_vp.node().object(), max_outputs);
    program->add_info(cache);
  }
  
  cache->update_channels(tex_size, tex_size);
  cache->freeze_inputs(true);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLint prevFramebuffer;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &prevFramebuffer));
  SH_GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_framebuffer));

  ShStream::iterator dest_iter = dest.begin();
  // Run each fragment program
  for (FBOStreamCache::set_iterator I = cache->sets_begin();
       I != cache->sets_end(); ++I) {
    std::vector<GLuint> draw_buffers;
    for (int i = 0; i < m_max_draw_buffers && dest_iter != dest.end(); ++i,++dest_iter) {
      // TODO: Should we cache the texture node?
      ShTextureNodePtr tex;
      ShTextureTraits traits = ShArrayTraits();
      // TODO: are these always the right dims?
      ShTextureDims dims(SH_TEXTURE_2D);
      tex = new ShTextureNode(dims, (*dest_iter)->size(), 
                              (*dest_iter)->valueType(),
                              traits, tex_size, tex_size, 1, count);
      tex->memory((*dest_iter)->memory(), 0);
      texture->bindTexture(tex, GL_COLOR_ATTACHMENT0_EXT+i, true);
      draw_buffers.push_back(GL_COLOR_ATTACHMENT0_EXT+i);
    }
    if (m_draw_buffers_ext == ATI)
      glDrawBuffersATI(draw_buffers.size(), &draw_buffers.front());
    if (m_draw_buffers_ext == ARB)
      glDrawBuffersARB(draw_buffers.size(), &draw_buffers.front());

    check_framebuffer();                     

    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    shBind(**I);
    TIMING_RESULT(binding);    
    
#ifdef SH_DEBUG_PBS_PRINTFP
    {
      ShProgramSet::NodeList::const_iterator i = (*I)->begin();
      (*i)->code()->print(std::cerr);
      ++i;
      (*i)->code()->print(std::cerr);
    }
#endif

    DECLARE_TIMER(clear);
    glClear(GL_COLOR_BUFFER_BIT);
    TIMING_RESULT(clear);

    DECLARE_TIMER(rendersetup);
    glViewport(0, 0, tex_size, tex_size);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
        
    TIMING_RESULT(rendersetup);

    DECLARE_TIMER(render);
    
    // Generate quad geometry
    glBegin(GL_QUADS); {
      glTexCoord2f(0.0, 0.0);
      glVertex3f(-1, -1, 0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f( 1, -1, 0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f( 1,  1, 0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-1,  1, 0);
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
    
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prevFramebuffer);
  
  if (old_handle) {
    old_handle->restore();
  }
}

StreamStrategy* FBOStreams::create()
{
  return new FBOStreams();
}

}
