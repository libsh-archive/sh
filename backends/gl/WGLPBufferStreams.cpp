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
#include "WGLPBufferStreams.hpp"

namespace shgl {

  using namespace SH;

  class WGLPBufferStreamException: public ShException
    {
    public:
      WGLPBufferStreamException(const std::string& message) :
        ShException("WGL PBuffer Stream Execution: " + message)
        {
        }
    };

  WGLPBufferStreams::WGLPBufferStreams(int context) :
    PBufferStreams(context),
    m_orig_hdc(NULL),
    m_orig_hglrc(NULL)
    {
    }

  WGLPBufferStreams::~WGLPBufferStreams()
    {
    }

  StreamStrategy* WGLPBufferStreams::create(int context)
    {
    return new WGLPBufferStreams(context);
    }

  FloatExtension WGLPBufferStreams::setupContext(int width, int height)
    {
    if (m_info.valid() && m_info.width == width && m_info.height == height)
      {
      shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->setContext(m_info.shcontext);

      if (!wglMakeCurrent(m_info.pbuffer_hdc, m_info.pbuffer_hglrc))
        {
        wglReleasePbufferDCARB(m_info.pbuffer, m_info.pbuffer_hdc);
        wglDestroyPbufferARB(m_info.pbuffer);
        wglDeleteContext(m_info.pbuffer_hglrc);

        m_info.pbuffer = NULL;
        m_info.pbuffer_hdc = NULL;
        m_info.pbuffer_hglrc = NULL;

        std::stringstream msg;
        msg << "wglMakeCurrent failed (" << GetLastError() << ")";
        shError(WGLPBufferStreamException(msg.str()));
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

    m_info.shcontext = shref_dynamic_cast<GlBackend>(ShEnvironment::backend)->newContext();
  
    // Grab the current DC and context, this will need to be
    // restored after the pbuffer work has been completed. If
    // there is no DC then we need to create one, the subsequent
    // WGL calls need a suitable DC to bootstrap the pbuffer
    // creation. 
    // TODO: cleanup this hackery. I assume the a that "DISPLAY"
    // DC is suitable enough for the subsequent WGL stuffs, but
    // the Win32 documentation doesn't say one way or another if
    // this is the "right" thing. Should the HWND/HDC/HGLRC that
    // is being used in GlBackend::GlBackend be kept around so
    // this code doesn't need any special cases (see comments
    // in GlBackend.cpp)?
    bool delete_dc = false;
    m_orig_hdc = wglGetCurrentDC();
    if (m_orig_hdc == NULL)
      {
      delete_dc = true;
      m_orig_hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
      if (m_orig_hdc == NULL)
        {
        std::stringstream msg;
        msg << "CreateDC failed (" << GetLastError() << ")";
        shError(WGLPBufferStreamException(msg.str()));
        }
      }
    else
      {
      m_orig_hglrc = wglGetCurrentContext();
      }

    int nfattribs = 0;
    int niattribs = 0;

    std::vector<int> fb_base_attribs;
    fb_base_attribs.push_back(WGL_DOUBLE_BUFFER_ARB); fb_base_attribs.push_back(false);
    fb_base_attribs.push_back(WGL_RED_BITS_ARB); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(WGL_GREEN_BITS_ARB); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(WGL_BLUE_BITS_ARB); fb_base_attribs.push_back(32);
    fb_base_attribs.push_back(WGL_DRAW_TO_PBUFFER_ARB); fb_base_attribs.push_back(true);
    fb_base_attribs.push_back(WGL_SUPPORT_OPENGL_ARB); fb_base_attribs.push_back(true);
  
    bool found_pixelformat = false;
    int format = 0;

    // Try NVIDIA
    if (!found_pixelformat)
      {
      std::vector<int> fb_attribs(fb_base_attribs);

      fb_attribs.push_back(WGL_PIXEL_TYPE_ARB); fb_attribs.push_back(WGL_TYPE_RGBA_ARB);
      fb_attribs.push_back(WGL_FLOAT_COMPONENTS_NV); fb_attribs.push_back(true);
      fb_attribs.push_back(0);
    
      unsigned int nformats = 0;
      if (!wglChoosePixelFormatARB(m_orig_hdc, &fb_attribs[0], NULL, 1, &format, &nformats))
        {
        SH_DEBUG_WARN("wglChoosePixelFormatARB failed (" << GetLastError() << ")");
        }

      if (nformats > 0)
        {
        found_pixelformat = true;
        m_info.extension = SH_ARB_NV_FLOAT_BUFFER;
        }
      }

    // Try ATI
    if (!found_pixelformat)
      {
      std::vector<int> fb_attribs(fb_base_attribs);

      fb_attribs.push_back(WGL_PIXEL_TYPE_ARB); fb_attribs.push_back(WGL_TYPE_RGBA_FLOAT_ATI);
      fb_attribs.push_back(0);
    
      unsigned int nformats = 0;
      if (!wglChoosePixelFormatARB(m_orig_hdc, &fb_attribs[0], NULL, 1, &format, &nformats))
        {
        SH_DEBUG_WARN("wglChoosePixelFormatARB failed (" << GetLastError() << ")");
        }

      if (nformats > 0)
        {
        found_pixelformat = true;
        m_info.extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
        }
      }

    if (!found_pixelformat)
      {
      shError(WGLPBufferStreamException("Could not find appropriate pixel format!"));
      return SH_ARB_NO_FLOAT_EXT;
      }

    if (m_info.extension == SH_ARB_NO_FLOAT_EXT)
      {
      shError(WGLPBufferStreamException("Could not choose a floating-point extension!"));
      return m_info.extension;
      }

    // Set up the pbuffer
    int pbuffer_attribs[] = {
      WGL_PBUFFER_LARGEST_ARB, false,
      0
    };

    // TODO: It seems that on windows with ATI cards small pbuffers produce
    // bad results in the first few pixels of the image. I've found that making
    // atleast an 8x8 pbuffer alleviates this. Some further testing is needed
    // and possible a better work around. -Kevin
    int temp_width = std::max(m_info.width, 8);
    int temp_height = std::max(m_info.height, 8);

    m_info.pbuffer = wglCreatePbufferARB(m_orig_hdc, format, temp_width, temp_height, pbuffer_attribs);
    if (m_info.pbuffer == NULL)
      {
      std::stringstream msg;
      msg << "wglCreatePbufferARB failed (" << GetLastError() << ")";
      shError(WGLPBufferStreamException(msg.str()));
      }

    m_info.pbuffer_hdc = wglGetPbufferDCARB(m_info.pbuffer);
    if (m_info.pbuffer_hdc == NULL)
      {
      wglDestroyPbufferARB(m_info.pbuffer);

      m_info.pbuffer = NULL;

      std::stringstream msg;
      msg << "wglGetPbufferDCARB failed (" << GetLastError() << ")";
      shError(WGLPBufferStreamException(msg.str()));
      }

    // Create a new gl context for the p-buffer.
    m_info.pbuffer_hglrc = wglCreateContext(m_info.pbuffer_hdc);
    if (m_info.pbuffer_hglrc == NULL)
      {
      wglReleasePbufferDCARB(m_info.pbuffer, m_info.pbuffer_hdc);
      wglDestroyPbufferARB(m_info.pbuffer);

      m_info.pbuffer = NULL;
      m_info.pbuffer_hdc = NULL;

      std::stringstream msg;
      msg << "wglCreateContext failed (" << GetLastError() << ")";
      shError(WGLPBufferStreamException(msg.str()));
      }

    if (!wglMakeCurrent(m_info.pbuffer_hdc, m_info.pbuffer_hglrc))
      {
      wglReleasePbufferDCARB(m_info.pbuffer, m_info.pbuffer_hdc);
      wglDestroyPbufferARB(m_info.pbuffer);
      wglDeleteContext(m_info.pbuffer_hglrc);

      m_info.pbuffer = NULL;
      m_info.pbuffer_hdc = NULL;
      m_info.pbuffer_hglrc = NULL;

      std::stringstream msg;
      msg << "wglMakeCurrent failed (" << GetLastError() << ")";
      shError(WGLPBufferStreamException(msg.str()));
      }

    // If we had to create the DC (see above) then delete it.
    // TODO: cleanup this hackery (see above)
    if (delete_dc)
      {
      DeleteDC(m_orig_hdc);
      }

    return m_info.extension;
    }

  void WGLPBufferStreams::restoreContext(void)
    {
    if (m_orig_hdc != NULL && m_orig_hglrc != NULL)
      {
      if (!wglMakeCurrent(m_orig_hdc, m_orig_hglrc))
        {
        std::stringstream msg;
        msg << "wglMakeCurrent failed (" << GetLastError() << ")";
        shError(WGLPBufferStreamException(msg.str()));
        }
      }
    }

}
