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

  GLXPBufferStreams::GLXPBufferStreams(int context) :
    PBufferStreams(context),
    m_display(NULL),
    m_orig_drawable(0),
    m_orig_context(0)
    {
    }

  GLXPBufferStreams::~GLXPBufferStreams()
    {
    }

  StreamStrategy* GLXPBufferStreams::create(int context)
    {
    return new GLXPBufferStreams(context);
    }

  FloatExtension GLXPBufferStreams::setupContext(int width, int height)
    {
    if (m_info.valid() && m_info.width == width && m_info.height == height)
      {
      shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(m_info.shcontext);
      if (!glXMakeCurrent(m_display, m_info.pbuffer, m_info.context))
	{
	// TODO: cleanup
        std::stringstream msg;
        msg << "glXMakeCurrent failed";
        shError(GLXPBufferStreamException(msg.str()));
        }

      return m_info.extension;
      }

    if (m_info.shcontext >= 0)
      {
      shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(m_info.shcontext);
      shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->destroyContext();
      }

    // Figure out what extension we're using
    m_info.extension = SH_ARB_NO_FLOAT_EXT;
    m_info.width = width;
    m_info.height = height;
    m_info.pbuffer = 0;
    m_info.context = 0;
    
    m_info.shcontext = shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->newContext();
    
    if (!m_display)
      {
      m_display = glXGetCurrentDisplay();
      if (!m_display)
	{
	m_display = XOpenDisplay(0);
	if (!m_display)
	  {
	  shError(GLXPBufferStreamException("Could not open X display"));
	  return m_info.extension;
	  }
	}
      else
	{
	m_orig_context = glXGetCurrentContext();
	m_orig_drawable = glXGetCurrentDrawable();
	}
      }
    
    int scrnum;
    scrnum = DefaultScreen(m_display);
    
    std::vector<int> fb_base_attribs;
    fb_base_attribs.push_back(GLX_DOUBLEBUFFER); fb_base_attribs.push_back(False);
    fb_base_attribs.push_back(GLX_RED_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_GREEN_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_BLUE_SIZE); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(GLX_DRAWABLE_TYPE); fb_base_attribs.push_back(GLX_PBUFFER_BIT);
  
    int items;

    GLXFBConfig* fb_config = 0;

    // Try NVIDIA
    if (!fb_config)
      {
      std::vector<int> fb_attribs(fb_base_attribs);
      fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_BIT);
      fb_attribs.push_back(GLX_FLOAT_COMPONENTS_NV); fb_attribs.push_back(True);
      fb_attribs.push_back(None);
    
      fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
      if (fb_config)
	{
	m_info.extension = SH_ARB_NV_FLOAT_BUFFER;
	}
      }

    // Try ATI
    if (!fb_config)
      {
      std::vector<int> fb_attribs(fb_base_attribs);
      fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_FLOAT_ATI_BIT);
      fb_attribs.push_back(None);
    
      fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
      if (fb_config)
	{
	m_info.extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
	}
      }

    if (!fb_config)
      {
      shError(GLXPBufferStreamException("Could not get GLX FB Config!\n"
					"Your card may not support the appropriate extensions."));
      return SH_ARB_NO_FLOAT_EXT;
      }

    if (m_info.extension == SH_ARB_NO_FLOAT_EXT)
      {
      shError(GLXPBufferStreamException("Could not choose a floating-point extension!\n"
					"Your card may not support the appropriate extensions."));
      return m_info.extension;
      }

    // Set up the pbuffer
    int pbuffer_attribs[] = {
      GLX_PBUFFER_WIDTH, width,
      GLX_PBUFFER_HEIGHT, height,
      GLX_LARGEST_PBUFFER, False,
      None
    };

    m_info.pbuffer = glXCreatePbuffer(m_display, fb_config[0], pbuffer_attribs);
    if (!m_info.pbuffer)
      {
      shError(GLXPBufferStreamException("Could not make pbuffer!"));
      return SH_ARB_NO_FLOAT_EXT;
      }
  
    m_info.context = glXCreateNewContext(m_display, fb_config[0], GLX_RGBA_TYPE, 0, True);
    if (!m_info.context)
      {
      // TODO: delete pbuffer
      shError(GLXPBufferStreamException("Could not create PBuffer context"));
      return SH_ARB_NO_FLOAT_EXT;
      }

    if (!glXMakeCurrent(m_display, m_info.pbuffer, m_info.context))
      {
      // TODO: delete pbuffer, context
      shError(GLXPBufferStreamException("glXMakeCurrent failed"));
      return SH_ARB_NO_FLOAT_EXT;
      }
  
    return m_info.extension;
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
      }
    }

}
