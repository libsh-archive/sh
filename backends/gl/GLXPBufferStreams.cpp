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
#include <iostream>
#include <sstream>

#include "ShError.hpp"
#include "ShEnvironment.hpp"
#include "GLXPBufferStreams.hpp"

// Extensions for ATI and Nvidia

#ifndef GLX_RGBA_FLOAT_ATI_BIT
#define GLX_RGBA_FLOAT_ATI_BIT          0x00000100
#endif /* GLX_RGBA_FLOAT_ATI_BIT */

#ifndef GLX_FLOAT_COMPONENTS_NV
#define GLX_FLOAT_COMPONENTS_NV         0x20B0
#endif /* GLX_FLOAT_COMPONENTS_NV */

namespace shgl {

  using namespace SH;

  class GLXPBufferStreamException : public ShException
    {
    public:
      GLXPBufferStreamException(const std::string& message) :
	ShException("GLX PBuffer Stream Execution: " + message)
	{
	}
    };

  GLXPBufferStreams::GLXPBufferStreams(void) :
    m_display(NULL),
    m_orig_drawable(0),
    m_orig_context(0)
    {
    }

  GLXPBufferStreams::~GLXPBufferStreams()
    {
    }

  StreamStrategy* GLXPBufferStreams::create(void)
    {
    return new GLXPBufferStreams;
    }

  FloatExtension GLXPBufferStreams::setupContext(int width, int height)
    {
    // initialize m_display if it hasn't already been done 
    if (!m_display)
      {
      m_display = glXGetCurrentDisplay();
      if (!m_display)
	{
	m_display = XOpenDisplay(0);
	if (!m_display)
	  {
	  shError(GLXPBufferStreamException("Could not open X display"));
	  }
	}
      }

    // grab the current GLX context/drawable, they will
    // be restore after the stream execution is finished
    m_orig_context = glXGetCurrentContext();
    m_orig_drawable = glXGetCurrentDrawable();
    
    // search the crrent list of pbuffers for an applicable one
    ShGLXPBufferInfo match;
    for(std::list<ShGLXPBufferInfo>::iterator itr = m_infos.begin();
	itr != m_infos.end();
	itr++)
      {
      if ((*itr).valid() && (*itr).width == width && (*itr).height == height)
	{
	match = (*itr);
	break;
	}
      }

    // if a valid match wasn't found then create a new
    // context and add it to the list of infos
    if (!match.valid())
      {
      match = createContext(width, height);
      m_infos.push_back(match);
      }

    // Activate the pbuffer/context 
    if (!glXMakeCurrent(m_display, match.pbuffer, match.context))
      {
      std::stringstream msg;
      msg << "glXMakeCurrent failed";
      shError(GLXPBufferStreamException(msg.str()));
      }

    return match.extension;
    }

  void GLXPBufferStreams::restoreContext(void)
    {
    if (m_display && m_orig_drawable && m_orig_context)
      {
      if (!glXMakeCurrent(m_display, m_orig_drawable, m_orig_context))
	{
	std::stringstream msg;
	msg << "glXMakeCurrent failed";
	shError(GLXPBufferStreamException(msg.str()));
	}
      else
	{
	m_orig_context = 0;
	m_orig_drawable = 0;
	}
      }
    }

  ShGLXPBufferInfo GLXPBufferStreams::createContext(int width, int height)
    {
    ShGLXPBufferInfo ret;
    ret.width = width;
    ret.height = height;

    // Figure out what extension we're using
    int scrnum = DefaultScreen(m_display);
    
    std::vector<int> fb_base_attribs;
    fb_base_attribs.push_back(GLX_DOUBLEBUFFER); fb_base_attribs.push_back(False);
    fb_base_attribs.push_back(GLX_RED_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_GREEN_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_BLUE_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_DRAWABLE_TYPE); fb_base_attribs.push_back(GLX_PBUFFER_BIT);
  
    GLXFBConfig* fb_config = 0;

    // Try NVIDIA
    if (!fb_config)
      {
      std::vector<int> fb_attribs(fb_base_attribs);
      fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_BIT);
      fb_attribs.push_back(GLX_FLOAT_COMPONENTS_NV); fb_attribs.push_back(True);
      fb_attribs.push_back(None);
    
      int items = 0;
      fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
      if (fb_config)
	{
	ret.extension = SH_ARB_NV_FLOAT_BUFFER;
	}
      }

    // Try ATI
    if (!fb_config)
      {
      std::vector<int> fb_attribs(fb_base_attribs);
      fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_FLOAT_ATI_BIT);
      fb_attribs.push_back(None);
    
      int items = 0;
      fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
      if (fb_config)
	{
	ret.extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
	}
      }

    if (!fb_config)
      {
      shError(GLXPBufferStreamException("Could not get GLX FB Config!\n"
					"Your card may not support the appropriate extensions."));
      }

    if (ret.extension == SH_ARB_NO_FLOAT_EXT)
      {
      shError(GLXPBufferStreamException("Could not choose a floating-point extension!\n"
					"Your card may not support the appropriate extensions."));
      }

    // Set up the pbuffer
    int pbuffer_attribs[] = {
      GLX_PBUFFER_WIDTH, width,
      GLX_PBUFFER_HEIGHT, height,
      GLX_LARGEST_PBUFFER, False,
      None
    };

    ret.pbuffer = glXCreatePbuffer(m_display, fb_config[0], pbuffer_attribs);
    if (!ret.pbuffer)
      {
      shError(GLXPBufferStreamException("Could not make pbuffer!"));
      }
  
    ret.context = glXCreateNewContext(m_display, fb_config[0], GLX_RGBA_TYPE, 0, True);
    if (!ret.context)
      {
      // TODO: delete pbuffer
      shError(GLXPBufferStreamException("Could not create PBuffer context"));
      }

    return ret;
    }

}
