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
#include "WGLPBufferContext.hpp"
#include "ShError.hpp"

namespace shgl {

using namespace SH;

//////////////////////
// WGLPBufferHandle //
//////////////////////

WGLPBufferHandle::WGLPBufferHandle(HDC dc,
				   HGLRC hglrc)
  : m_dc(dc),
    m_hglrc(hglrc)
{
}

WGLPBufferHandle::~WGLPBufferHandle()
{
}

void WGLPBufferHandle::restore()
{
  if (!wglMakeCurrent(m_dc, m_hglrc)) {
    shError(ShException("Restoring WGL Context failed"));
  }
}

///////////////////////
// WGLPBufferContext //
///////////////////////

WGLPBufferContext::WGLPBufferContext(int width, int height,
                                     void* id,
				     HPBUFFERARB pbuffer,
                                     HDC dc, HGLRC hglrc)
  : PBufferContext(width, height, id),
    m_pbuffer(pbuffer),
    m_dc(dc),
    m_hglrc(hglrc)
{
}

WGLPBufferContext::~WGLPBufferContext()
{
  // TODO: Free context
}

PBufferHandlePtr WGLPBufferContext::activate()
{
  HDC old_dc = wglGetCurrentDC();
  HGLRC old_hglrc = wglGetCurrentContext();

  if (!wglMakeCurrent(m_dc, m_hglrc)) {
    shError(ShException("Activating WGLPBufferContext failed!"));
  }

  if (old_dc && old_hglrc) {
    return new WGLPBufferHandle(old_dc, old_hglrc);
  } else {
    return 0;
  }
}

///////////////////////
// WGLPBufferFactory //
///////////////////////

WGLPBufferFactory::WGLPBufferFactory()
  : m_dc(0),
    m_format(0),
    m_extension(SH_ARB_NO_FLOAT_EXT)
{
}

WGLPBufferFactory::~WGLPBufferFactory()
{
}

FloatExtension WGLPBufferFactory::get_extension()
{
  init_dc();
  init_config();
  return m_extension;
}

void WGLPBufferFactory::init_dc()
{
  if (!m_dc) {
    m_dc = wglGetCurrentDC();
  }
  if (!m_dc) {
    m_dc = CreateDC("DISPLAY", NULL, NULL, NULL);
  }
  if (!m_dc) {
    shError(ShException("Could create device context"));
  }
}

void WGLPBufferFactory::init_config()
{
  if (m_format) return;

  init_dc();

  std::vector<int> fb_base_attribs;
  fb_base_attribs.push_back(WGL_DOUBLE_BUFFER_ARB); fb_base_attribs.push_back(false);
  fb_base_attribs.push_back(WGL_RED_BITS_ARB); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(WGL_GREEN_BITS_ARB); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(WGL_BLUE_BITS_ARB); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(WGL_DRAW_TO_PBUFFER_ARB); fb_base_attribs.push_back(true);
  fb_base_attribs.push_back(WGL_SUPPORT_OPENGL_ARB); fb_base_attribs.push_back(true);
  fb_base_attribs.push_back(WGL_STENCIL_BITS_ARB); fb_base_attribs.push_back(1);

  // Try ATI
  if (!m_format) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(WGL_PIXEL_TYPE_ARB); fb_attribs.push_back(WGL_TYPE_RGBA_FLOAT_ATI);
    fb_attribs.push_back(0);
    
    unsigned int items = 0;
    if (wglChoosePixelFormatARB(m_dc, &fb_attribs[0], NULL, 1, &m_format, &items)
	&& items > 0) {
      m_extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
    }
  }

  // Try NVIDIA
  if (!m_format) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(WGL_PIXEL_TYPE_ARB); fb_attribs.push_back(WGL_TYPE_RGBA_ARB);
    fb_attribs.push_back(WGL_FLOAT_COMPONENTS_NV); fb_attribs.push_back(true);
    fb_attribs.push_back(0);
    
    unsigned int items = 0;
    if (wglChoosePixelFormatARB(m_dc, &fb_attribs[0], NULL, 1, &m_format, &items)
	&& items > 0) {
      m_extension = SH_ARB_NV_FLOAT_BUFFER;
    }
  }

  if (!m_format) {
    shError(ShException("Could not get WGL Pixelformat!\n"
			"Your card may not support the appropriate extensions."));
  }

  if (m_extension == SH_ARB_NO_FLOAT_EXT) {
    m_format = 0;
    shError(ShException("Could not choose a floating-point extension!\n"
                        "Your card may not support the appropriate extensions."));
  }
}

PBufferContextPtr WGLPBufferFactory::get_context(int width, int height,
                                                 void* id)
{
  for (std::list<WGLPBufferContextPtr>::const_iterator I = m_contexts.begin();
       I != m_contexts.end(); ++I) {
    if ((*I)->width() == width && (*I)->height() == height && (*I)->id() == id) {
      return *I;
    }
  }
  WGLPBufferContextPtr p = create_context(width, height, id);

  if (p) {
    m_contexts.push_back(p);
  }

  return p;
}

WGLPBufferContextPtr WGLPBufferFactory::create_context(int width, int height,
                                                       void* id)
{
  init_dc();
  init_config();

  // Set up the pbuffer
  int pbuffer_attribs[] = {
    WGL_PBUFFER_LARGEST_ARB, false,
    0
  };

  HPBUFFERARB pbuffer = wglCreatePbufferARB(m_dc, m_format, width, height, pbuffer_attribs);
  if (!pbuffer) {
    shError(ShException("Could not make pbuffer!"));
    return 0;
  }

  HDC pbuffer_dc = wglGetPbufferDCARB(pbuffer);

  HGLRC pbuffer_hglrc = wglCreateContext(pbuffer_dc);

  // TODO: Sharelists?
  
  return new WGLPBufferContext(width, height, id, pbuffer, pbuffer_dc, pbuffer_hglrc);
}


WGLPBufferFactory* WGLPBufferFactory::m_instance = 0;

WGLPBufferFactory* WGLPBufferFactory::instance()
{
  if (!m_instance) m_instance = new WGLPBufferFactory();

  return m_instance;
}

}
