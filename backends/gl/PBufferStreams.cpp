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
#include "PBufferStreams.hpp"

/// Turn this on if you want timings on std::cerr
//#define DO_PBUFFER_TIMING

// Turn this on to debug the fragment programs.
//#define SH_DEBUG_PBS_PRINTFP

#include <map>
#include <fstream>
#include <cstdlib>
#include <list>
#include <iostream>
#include <set>
#include <vector>

#include "sh.hpp"
#include "ShOptimizations.hpp"
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "PBufferContext.hpp"
#include "Utils.hpp"
#include "GlTextureStorage.hpp"
#include "GlTextureName.hpp"
#include "GlTextures.hpp"

#ifdef DO_PBUFFER_TIMING
#include <sys/time.h>
#include <time.h>
#endif

#define FBO

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
//FIXME: temporary change	  
    //SH_TEXTURE_RECT;
    dims = SH_TEXTURE_2D;
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
    ShProgram with_tc = ShProgram(*I) & lose<ShTexCoord2f>("streamcoord");
    
    TexFetcher fetcher(m_channel_map, with_tc.node()->inputs.back(),
                       dims == SH_TEXTURE_2D/*RECT*/, with_tc.node());
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

    texture->memory(channel->memory(), 0);
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

  std::vector<GlTextureStoragePtr> outtexs;  
  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I) {
    if (count != (*I)->count()) {
      shError(PBufferStreamException("All stream outputs must be of the same size"));
    }
#ifdef FBO
    // find/create texturestorages for outputs 
    ShChannelNode* output = &(**I);
    ShTextureNodePtr tex;
    ShTextureTraits traits = ShArrayTraits();
    
    tex = new ShTextureNode(SH_TEXTURE_2D, output->size(), output->valueType(),
                         traits, 1, 1, 1, 0);
    tex->memory(output->memory(), 0);
    tex->setTexSize(tex_size, tex_size);
    tex->count(output->count());
   
    StorageFinder strictWriteFinder(tex, true, true, false);
    StorageFinder writeFinder(tex, true, false, false);

    GlTextureStoragePtr outtex = shref_dynamic_cast<GlTextureStorage>(output->memory()->findStorage("opengl:texture", strictWriteFinder));
    //if(outtex) printf("found an appropriate tex storage for writting\n");
    /*if(!outtex) GlTextureStoragePtr outtex = shref_dynamic_cast<GlTextureStorage>(output->memory()->findStorage("opengl:texture", writeFinder));*/

    if(!outtex){
      		
	// the newly created texstorage must be compatible with a texture storage 
	// used for stream inputs, so have to create it with the same attributes

     	printf("FYI: creating a texture storage for stream output\n");    
   
        GlTextureNamePtr name = new GlTextureName(GL_TEXTURE_2D);
	name->params(traits);
	outtex = new GlTextureStorage(tex->memory(0).object(),
                                     GL_TEXTURE_2D,
                                     shGlFormat(tex),
                                     shGlInternalFormat(tex),
                                     tex->valueType(),
                                     tex->width(), tex->height(), 
                                     tex->depth(), tex->size(),
                                     tex->count(), name, 0);

	outtex->allocate();
	/*outtex->sync(); //possible optimization would be to just allocate space for the texture,
	//not allocate and copy from other up-to-date storage, since we are going to write to it anyway
	*/
		
	

    }//if

    outtex->setWrite();
    outtexs.push_back(outtex);

#endif //FBO    
  }//for
  
  
  DECLARE_TIMER(onerun);

#ifndef FBO  
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
#endif//fbo

  DECLARE_TIMER(vpsetup);

  // --- Set up the vertex program
  if (!m_setup_vp) {
    // The (trivial) vertex program
    m_vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
    m_vp.node()->target() = "gpu:vertex";
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

  ShStream::iterator dest_iter= dest.begin();
   
#ifdef FBO
  //check if texstorage has been created
      //create an FBO
     //FIXME: dont create and destroy an FBO every time, rather have a global one
     GLuint fb, depth_rb;
     SH_GL_CHECK_ERROR(glGenFramebuffersEXT(1, &fb));
     SH_GL_CHECK_ERROR(glGenRenderbuffersEXT(1, &depth_rb));
 
     SH_GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb));
     // initialize depth renderbuffer
     SH_GL_CHECK_ERROR(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb));
     SH_GL_CHECK_ERROR(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                                 GL_DEPTH_COMPONENT24, tex_size, tex_size));
     SH_GL_CHECK_ERROR(glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_rb));

    
#endif//FBO

  // Run each fragment program
  int i=0; 
  for (PBufferStreamCache::set_iterator I = cache->sets_begin();
       I != cache->sets_end(); ++I, ++dest_iter, ++i) {

    ShChannelNode* output = dest_iter->object();
    
#ifdef FBO
 
    GlTextureStoragePtr outtex = outtexs[i];
        
    SH_GL_CHECK_ERROR(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, outtex->name(), 0));

    SH_GL_CHECK_ERROR(GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));
    if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
       printf("FBO IS INCOMPLETE!\n");
       
#endif //FBO
    
    DECLARE_TIMER(binding);
    // Then, bind vertex (pass-through) and fragment program
    shBind(**I);
    TIMING_RESULT(binding);

#ifdef SH_DEBUG_PBS_PRINTFP
    {
      ShProgramSet::NodeList::const_iterator i = (*I)->begin();
      ++i;
      (*i)->code()->print(std::cerr);
    }
#endif

    DECLARE_TIMER(clear);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    TIMING_RESULT(clear);

    DECLARE_TIMER(rendersetup);
    
    
    int old_viewport[4];
    glGetIntegerv(GL_VIEWPORT, old_viewport);
    glViewport(0, 0, tex_size, tex_size);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    float tc_right;
    float tc_upper;

    tc_right = static_cast<float>(tex_size);
    tc_upper = static_cast<float>(tex_size);
/*
#ifndef FBO    
    if (extension == SH_ARB_NV_FLOAT_BUFFER) {
      tc_right = static_cast<float>(tex_size);
      tc_upper = static_cast<float>(tex_size);
    } else {
#endif	    
      tc_right = 1.0;
      tc_upper = 1.0;
#ifndef FBO
    }
#endif
*/
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
    
// return state
    glViewport(0,0,old_viewport[2], old_viewport[3]);
 
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
      shError(PBufferStreamException("Could not render"));
      return;
    }
  
    DECLARE_TIMER(findouthost);

    ShValueType valueType = output->valueType();
    
    //FIXME// check for the backend type, if GL use pbuffer binding as opposed to copying back on the host
    
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
    }//switch

     
    /*if(output->memory()->findStorage("host")){
    	output->memory()->removeStorage(output->memory()->findStorage("host"));
    }*/
       
#ifndef FBO
	// use host memory for now until texture storage is created    
//	printf("PBufferStreams.cpp: Could not find texture storages in memory: %s, copying results to the host\n", output->memory()->getTag());
	ShHostStoragePtr outhost = shref_dynamic_cast<ShHostStorage>(output->memory()->findStorage("host"));
    	if (!outhost) {
    	  int datasize = shTypeInfo(valueType)->datasize(); 
    	  outhost = new ShHostStorage(output->memory().object(), datasize * output->size() * output->count(), valueType);
	}
    
   // Read back
   // FIXME check for backend
      TIMING_RESULT(findouthost);
      SH_DEBUG_ASSERT(outhost->value_type() == valueType);
      DECLARE_TIMER(dirtyouthost);
      outhost->dirtyall();
      TIMING_RESULT(dirtyouthost);
   
            
    // do the copy: pbuffer->texture    
    // Bind texture name for this scope.
    //GlTextureName::Binding binding(outGPU->texName());
    
     
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
    
    glReadPixels(0, 0, tex_size, count / tex_size, format, // actual copy of the
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
#endif //FBO
    
  }//for

  int k = 0;
  for (ShStream::iterator I = dest.begin(); I != dest.end(); ++I, k++) {

     outtexs[k]->clearWrite();
     outtexs[k]->dirtyall();
  }//for

#ifdef FBO
// delete fb, rb
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glDeleteFramebuffersEXT(1, &fb);
   glDeleteRenderbuffersEXT(1, &depth_rb);

#endif //FBO

#ifndef FBO
   if (old_handle) {
    old_handle->restore();
  }
#endif //FBO
}

StreamStrategy* PBufferStreams::create()
{
  return new PBufferStreams();
}

}
