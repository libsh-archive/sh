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
using namespace std;

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
                             ShStream& dest, TextureStrategy* texture)
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

  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
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
  
  DECLARE_TIMER(vpsetup);

  // --- Set up the vertex program
  if (!m_setup_vp) {
    // The (trivial) vertex program
    m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
    m_vp.node()->target() = get_target_backend(program_const) + "vertex";
    shCompile(m_vp);
    m_setup_vp = true;
  }
  
  TIMING_RESULT(vpsetup);

  // --- Set up the fragment programs and such
  StreamCache* cache = program->get_info<StreamCache>();
  if (!cache) {
    cache = new StreamCache(program.object(), m_vp.node().object(), tex_size, 1);
    program->add_info(cache);
  }
  
  cache->update_channels();

  // --- Set up the GLX context
  PBufferContextPtr context = 
    factory->get_context(cache->tex_size(), cache->tex_size());

  PBufferHandlePtr old_handle = context->activate();

  // Check whether some channels are both being read and written to
  bool rw_channels = false;
  if (dest.size() > 1) {
    std::set<ShMemory*> input_channels;
    for (ShProgramNode::ChannelList::const_iterator i = program->begin_channels();
         i != program->end_channels(); i++) {
      input_channels.insert((*i)->memory().object());
    }

    for (ShStream::const_iterator i = dest.begin(); i != dest.end(); i++) {
      if (input_channels.find((*i)->memory().object()) != input_channels.end()) {
        static bool warning_seen = false;
        if (!warning_seen) {
          SH_DEBUG_WARN("Using an intermediate stream during execution since some of the channels are both being read from and written to.");
          warning_seen = true;
        }
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
							node->valueType(), channel_mem, count);
      intermediate_stream->append(channel_copy);
    }
    dest_iter = intermediate_stream->begin();
  } else {
    // Use the output stream directly
    dest_iter = dest.begin();
  }

  // Run each fragment program
  for (StreamCache::set_iterator I = cache->sets_begin();
       I != cache->sets_end(); ++I, ++dest_iter) {

    ShChannelNode* output = dest_iter->object();
    
    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    ShProgramSetPtr program_set = *I;
    shBind(*program_set);
    TIMING_RESULT(binding);

//#ifdef SH_DEBUG_PBS_PRINTFP
    {
      ShProgramSet::NodeList::const_iterator i = program_set->begin();
      ++i;
      ShProgramNodePtr program_node = *i;
      std::cerr << program_node->describe_interface() << std::endl;
      program_node->code()->print(std::cerr);
    }
//#endif

    DECLARE_TIMER(clear);
    glClear(GL_COLOR_BUFFER_BIT);
    TIMING_RESULT(clear);

    DECLARE_TIMER(rendersetup);
    glViewport(0, 0, cache->tex_size(), cache->tex_size());
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float tc_right;
    float tc_upper;
    
    if (extension == SH_ARB_NV_FLOAT_BUFFER) {
      tc_right = static_cast<float>(cache->tex_size());
      tc_upper = static_cast<float>(cache->tex_size());
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
    
    
    GLenum format = GL_RGB;
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
      {
	std::stringstream s;
	s << "Output size of " << output->size() << " is not currently supported.";
	shError(PBufferStreamException(s.str()));
	break;
      }
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
    
    // actual copy of the results to the stream
    glReadPixels(0, 0, cache->tex_size(), count / cache->tex_size(), format,
                 readpixelType, resultBuffer->array());
    gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not do glReadPixels()"));
      return;
    }
    if (count % cache->tex_size()) {
      glReadPixels(0, count / cache->tex_size(), 
                   count % cache->tex_size(), 1, format, readpixelType,
                   (char*)(resultBuffer->array()) 
                   + (count - (count % cache->tex_size())) * 
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
