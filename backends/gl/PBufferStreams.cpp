// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <list>
#include <set>

#include "sh.hpp"
#include "ShOptimizations.hpp"
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "PBufferContext.hpp"
#include "Utils.hpp"

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

class PBufferStreamCache : public ShInfo {
public:
  PBufferStreamCache(ShProgramNode* stream_program,
                     ShProgramNode* vertex_program);

  ShInfo* clone() const;

  void update_channels(int width, int height);

  void build_sets(ShProgramNode* vertex_program);

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

  PBufferStreamCache(const PBufferStreamCache& other);
  PBufferStreamCache& operator=(const PBufferStreamCache& other);
};

PBufferStreamCache::PBufferStreamCache(ShProgramNode* stream_program,
                                       ShProgramNode* vertex_program)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program)
{
  FloatExtension extension = PBufferFactory::instance()->get_extension();
  ShTextureDims dims(SH_TEXTURE_2D);
  
  switch (extension) {
  case SH_ARB_NV_FLOAT_BUFFER:
    dims = SH_TEXTURE_RECT;
    break;
  case SH_ARB_ATI_PIXEL_FORMAT_FLOAT:
    dims = SH_TEXTURE_2D;
    break;
  case SH_ARB_NO_FLOAT_EXT:
    throw PBufferStreamException("No floating point rendering target.\n"
                                 "Ensure your system supports floating point pbuffers.");
    break;
  } 

  ChannelGatherer gatherer(m_channel_map, dims);
  stream_program->ctrlGraph->dfs(gatherer);

  split_program(stream_program, m_programs, "gpu:fragment");

  for (std::list<ShProgramNodePtr>::iterator I = m_programs.begin();
       I != m_programs.end(); ++I) {
    //ShProgram with_tc = ShProgram(*I) & lose<ShTexCoord2f>("streamcoord");
    ShProgram with_tc = ShProgram(*I);

    for(int i = 0; i< m_channel_map.size(); i++)
      with_tc = with_tc & lose<ShTexCoord2f>("streamcoord");

    /*TexFetcher fetcher(m_channel_map, with_tc.node()->inputs.back(),
      dims == SH_TEXTURE_RECT, with_tc.node()); */
    std::list<ShVariableNodePtr>::iterator myit = with_tc.node()->inputs.begin();
    for( ShProgramNode::VarList::size_type i = 0;
	 i < (with_tc.node()->inputs.size() - m_channel_map.size()); i++, ++myit);
    std::list<ShVariableNodePtr> mylist(myit, with_tc.node()->inputs.end());
    TexFetcher fetcher(m_channel_map, mylist, // probably should just pass the iterator
                       dims == SH_TEXTURE_RECT, with_tc.node());
    with_tc.node()->ctrlGraph->dfs(fetcher);

    optimize(with_tc);
    
    *I = with_tc.node();
  }

  build_sets(vertex_program);
}

ShInfo* PBufferStreamCache::clone() const
{
  return new PBufferStreamCache(m_stream_program, m_vertex_program);
}

void PBufferStreamCache::update_channels(int width, int height)
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ShChannelNode* channel = I->first.object();
    ShTextureNode* texture = I->second.object();

    SH_DEBUG_ASSERT(channel);
    SH_DEBUG_ASSERT(texture);

    texture->memory(channel->memory());
    texture->setTexSize(width, height);
    texture->count(channel->count());
  }
}

void PBufferStreamCache::build_sets(ShProgramNode* vertex_program)
{
  m_vertex_program = vertex_program;
  m_program_sets.clear();
  for (std::list<ShProgramNodePtr>::iterator I = m_programs.begin(); I != m_programs.end(); ++I) {
    m_program_sets.push_back(new ShProgramSet(ShProgram(vertex_program),
                                              ShProgram(*I)));
  }
}

PBufferStreams::PBufferStreams(void) :
  m_shaders(NULL), m_setup_vp(false)
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

void PBufferStreams::execute(const ShProgramNodeCPtr& program_const,
                             ShStream& dest)
{
  // Let's get rid of that constness... Yes, yes, I know...
  ShProgramNodePtr program = shref_const_cast<ShProgramNode>(program_const);
  
  DECLARE_TIMER(overhead);

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

  // TODO: Check that the size of each output matches the size of each dest.

  int count = (*dest.begin())->count();

  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I ) {
    if (count != (*I)->count()) {
      shError(PBufferStreamException("All stream outputs must be of the same size"));
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
  
  DECLARE_TIMER(onerun);

  PBufferFactory* factory = PBufferFactory::instance();

  if (!factory) {
    throw PBufferStreamException("No PBuffer support found.\n"
                                 "Ensure your system supports floating point pbuffers.");
    return;
  }

  FloatExtension extension = factory->get_extension();
  
  // --- Set up the GLX context
  PBufferContextPtr context = factory->get_context(tex_size, tex_size);

  PBufferHandlePtr old_handle = context->activate();

  DECLARE_TIMER(vpsetup);

  // --- Set up the vertex program
  if (!m_setup_vp) {
    //m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();   // The identity vertex program
    //m_vp.node()->target() = "gpu:vertex";
    float delta = 1.0 / (float)tex_size * 2.0f;
    m_vp = SH_BEGIN_PROGRAM("gpu:vertex") {
      ShInputPosition4f ipos;
      ShOutputPosition4f opos;
      ShInputTexCoord2f tc;
      ShOutputTexCoord2f otc;
      ShOutputTexCoord2f otc2;
      //ShOutputTexCoord2f otc2;
      //ShAttrib2f SH_DECL(offset) = ShAttrib2f(delta, 0.0);
      ShAttrib2f SH_DECL(offset) = ShAttrib2f(2.0 - delta /10., 1.0);

      opos = ipos;
      otc = tc * offset;
      otc2 = otc;
    } SH_END;

    shCompile(m_vp);
    m_setup_vp = true;
  }
  
  TIMING_RESULT(vpsetup);

  // --- Set up the fragment programs and such
  PBufferStreamCache* cache = program->get_info<PBufferStreamCache>();
  if (!cache) {
    cache = new PBufferStreamCache(program.object(), m_vp.node().object());
    program->add_info(cache);
  }  

  cache->update_channels(tex_size, tex_size);

  // Check whether some channels are both being read and written to
  bool rw_channels = false;
  if (dest.size() > 1) {
    std::set<ShChannelNode*> input_channels;
    for (ShProgramNode::ChannelList::const_iterator i = program->channels_begin();
	 i != program->channels_end(); i++) {
      input_channels.insert(i->object());
    }

    for (ShStream::const_iterator i = dest.begin(); i != dest.end(); i++) {
      if (input_channels.find(i->object()) != input_channels.end()) {
	SH_DEBUG_WARN("Using an intermediate stream during execution since some of the channels are both being read from and written to.");
	rw_channels = true;
	break;
      }
    }
  }

  ShStream::iterator dest_iter;
  ShStream* intermediate_stream = NULL;
  if (rw_channels) {
    // Make an intermediate stream so that updates take effect only
    // once all of the programs have been executed.
    intermediate_stream = new ShStream();
    for (ShStream::const_iterator i = dest.begin(); i != dest.end(); i++) {
      ShChannelNode* node = i->object();
      int count = (*i)->count();
      int tuplesize = node->size();
      int valuesize = shTypeInfo(node->valueType())->datasize();
      int length = count * tuplesize * valuesize;

      ShHostMemoryPtr channel_mem = new ShHostMemory(length, node->valueType());
      ShChannelNodePtr channel_copy = new ShChannelNode(node->specialType(), tuplesize, 
							node->valueType(), channel_mem, count, /*offset*/ 0, /*stride*/1);
      intermediate_stream->append(channel_copy);
    }
    dest_iter = intermediate_stream->begin();
  } else {
    // Use the output stream directly
    dest_iter = dest.begin();
  }

  // Run each fragment program
  for (PBufferStreamCache::set_iterator I = cache->sets_begin();
       I != cache->sets_end(); ++I, ++dest_iter) {

    ShChannelNode* output = dest_iter->object();

    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    shBind(**I);
    TIMING_RESULT(binding);

    // #ifdef SH_DEBUG_PBS_PRINTFP // XXX
    for (ShProgramSet::NodeList::const_iterator i = (*I)->begin(); i != (*I)->end(); i++) {
      (*i)->code()->print(std::cerr);  
    }
    // #endif // XXX

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
    
    // Unbind, just to be safe
    shUnbind(**I);
    
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not render"));
      return;
    }
  
    DECLARE_TIMER(findouthost);

    ShValueType valueType = output->valueType();
    
    ShHostStoragePtr outhost
      = shref_dynamic_cast<ShHostStorage>(output->memory()->findStorage("host"));
    if (!outhost) {
      int datasize = shTypeInfo(valueType)->datasize(); 
      outhost = new ShHostStorage(output->memory().object(),
                                  datasize * output->size() * output->count(),
				  valueType);
    }
    TIMING_RESULT(findouthost);
    
    SH_DEBUG_ASSERT(outhost->value_type() == valueType);

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
      format = GL_RGB; // glReadPixels doesn't support 2-component output
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
    GLenum readpixelType;
    ShValueType convertedType; 
    readpixelType = shGlType(valueType, convertedType);
    if (convertedType != SH_VALUETYPE_END) {
      SH_DEBUG_WARN("GL backend does not handle stream output type " 
		    << shValueTypeName(valueType) << " natively."
                    << "  Using " << shValueTypeName(convertedType) 
		    << " temporary buffer.");
    }
    
    ShVariantPtr  resultBuffer;
    int resultDatasize = output->size() * count;
    bool using_temporary_buffer = false;
    if ((output->size() != 2) && (SH_VALUETYPE_END == convertedType)) {
      // Use the output buffer directly
      resultBuffer = shVariantFactory(valueType, SH_MEM)->
	generate(resultDatasize, outhost->data(), false);
    } else {
      if (2 == output->size()) {
	// Hack to support 2-component outputs (add an extra component)
	resultDatasize = 3 * count;
	if (SH_VALUETYPE_END == convertedType) convertedType = valueType;
      }
      // Use a temporary buffer
      resultBuffer = shVariantFactory(convertedType, SH_MEM)->
	generate(resultDatasize);
      using_temporary_buffer = true;
    }
    
    int freq = 2.0; // this should be from stride
    glReadPixels(0, 0, tex_size / freq, count / tex_size, format, // actual copy of the
                 readpixelType, resultBuffer->array());    // results to the stream
    gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not do glReadPixels()"));
      return;
    }
    if (count % tex_size) {
      glReadPixels(0, count / tex_size, count % tex_size, 1, format, readpixelType,
                   (char*)(resultBuffer->array()) + (count - (count % tex_size)) * 
		   output->size() * resultBuffer->datasize());
      gl_error = glGetError();
      if (gl_error != GL_NO_ERROR) {
        shError(PBufferStreamException("Could not do rest of glReadPixels()"));
        return;
      }
    }
    
    if (using_temporary_buffer) { // need to copy to outhoust->data()
      if (2 == output->size()) {
	// Hack to support 2-component output (discard third component)
	resultBuffer = resultBuffer->get(false, ShSwizzle(3,0,1), count);
	resultDatasize = 2 * count;
      }
      ShVariantPtr outhostVariant = shVariantFactory(valueType, SH_MEM)->
	generate(resultDatasize, outhost->data(), false);
      outhostVariant->set(resultBuffer);
    }
    
    TIMING_RESULT(readback);
  }

  if (intermediate_stream) {
    // Once all programs have been executed, update the values in the
    // output stream using the values from the intermediate stream.
    ShStream::const_iterator j = intermediate_stream->begin();
    for (ShStream::const_iterator i = dest.begin(); i != dest.end(); j++, i++) {
      ShChannelNode* intermediate_node = j->object();
      ShChannelNode* real_node = i->object();

      ShValueType valueType = real_node->valueType();
      int datasize = shTypeInfo(valueType)->datasize(); 
      int size = real_node->size();
      int count = real_node->count();
    
      ShHostStoragePtr real_host 
	= shref_dynamic_cast<ShHostStorage>(real_node->memory()->findStorage("host"));
      if (!real_host) {
	real_host = new ShHostStorage(real_node->memory().object(), datasize * size * count, valueType);
      }
      ShVariantPtr real_variant 
	= shVariantFactory(valueType, SH_MEM)->generate(size * count, real_host->data(), false);

      ShHostStoragePtr intermediate_host 
	= shref_dynamic_cast<ShHostStorage>(intermediate_node->memory()->findStorage("host"));
      if (!intermediate_host) {
	intermediate_host = new ShHostStorage(intermediate_node->memory().object(), datasize * size * count, valueType);
      }
      ShVariantPtr intermediate_variant 
	= shVariantFactory(valueType, SH_MEM)->generate(size * count, intermediate_host->data(), false);

      real_variant->set(intermediate_variant); // copy channel data
      real_host->dirty();
    }
    delete intermediate_stream;
  }

  if (old_handle) {
    old_handle->restore();
  }
}

StreamStrategy* PBufferStreams::create()
{
  return new PBufferStreams();
}

}
