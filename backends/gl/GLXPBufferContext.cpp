#include "GLXPBufferContext.hpp"
#include "ShError.hpp"

#ifndef GLX_RGBA_FLOAT_ATI_BIT
#define GLX_RGBA_FLOAT_ATI_BIT          0x00000100
#endif /* GLX_RGBA_FLOAT_ATI_BIT */

#ifndef GLX_FLOAT_COMPONENTS_NV
#define GLX_FLOAT_COMPONENTS_NV         0x20B0
#endif /* GLX_FLOAT_COMPONENTS_NV */

namespace shgl {

using namespace SH;

//////////////////////
// GLXPBufferHandle //
//////////////////////

GLXPBufferHandle::GLXPBufferHandle(Display* display,
                                   GLXDrawable drawable,
                                   GLXContext context)
  : m_display(display),
    m_drawable(drawable),
    m_context(context)
{
}

GLXPBufferHandle::~GLXPBufferHandle()
{
}

void GLXPBufferHandle::restore()
{
  if (!glXMakeCurrent(m_display, m_drawable, m_context)) {
    shError(ShException("Restoring GLX Context failed"));
  }
}

///////////////////////
// GLXPBufferContext //
///////////////////////

GLXPBufferContext::GLXPBufferContext(int width, int height,
                                     void* id,
                                     Display* display, GLXPbuffer pbuffer,
                                     GLXContext context)
  : PBufferContext(width, height, id),
    m_display(display),
    m_pbuffer(pbuffer),
    m_context(context)
{
}

GLXPBufferContext::~GLXPBufferContext()
{
  // TODO: Free context
}

PBufferHandlePtr GLXPBufferContext::activate()
{
  GLXContext old_context = glXGetCurrentContext();
  GLXDrawable old_drawable = glXGetCurrentDrawable();

  if (!glXMakeCurrent(m_display, m_pbuffer, m_context)) {
    shError(ShException("Activating GLXPBufferContext failed!"));
  }

  if (old_drawable && old_context) {
    return new GLXPBufferHandle(m_display, old_drawable, old_context);
  } else {
    return 0;
  }
}

///////////////////////
// GLXPBufferFactory //
///////////////////////

GLXPBufferFactory::GLXPBufferFactory()
  : m_display(0),
    m_fb_config(0),
    m_extension(SH_ARB_NO_FLOAT_EXT)
{
}

GLXPBufferFactory::~GLXPBufferFactory()
{
}

FloatExtension GLXPBufferFactory::get_extension()
{
  init_display();
  init_config();
  return m_extension;
}

void GLXPBufferFactory::init_display()
{
  if (!m_display) {
    m_display = glXGetCurrentDisplay();
  }
  if (!m_display) {
    m_display = XOpenDisplay(0);
  }
  if (!m_display) {
    shError(ShException("Could not open X display"));
  }
}

void GLXPBufferFactory::init_config()
{
  if (m_fb_config) return;
  
  int scrnum = DefaultScreen(m_display);

  std::vector<int> fb_base_attribs;
  fb_base_attribs.push_back(GLX_DOUBLEBUFFER); fb_base_attribs.push_back(False);
  fb_base_attribs.push_back(GLX_RED_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_GREEN_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_BLUE_SIZE); fb_base_attribs.push_back(32);
  fb_base_attribs.push_back(GLX_STENCIL_SIZE); fb_base_attribs.push_back(1);
  fb_base_attribs.push_back(GLX_DRAWABLE_TYPE); fb_base_attribs.push_back(GLX_PBUFFER_BIT);

  // Try NVIDIA
  if (!m_fb_config) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_BIT);
    fb_attribs.push_back(GLX_FLOAT_COMPONENTS_NV); fb_attribs.push_back(True);
    fb_attribs.push_back(None);
    
    int items = 0;
    m_fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
    if (m_fb_config) {
      m_extension = SH_ARB_NV_FLOAT_BUFFER;
    }
  }

  // Try ATI
  if (!m_fb_config) {
    std::vector<int> fb_attribs(fb_base_attribs);
    fb_attribs.push_back(GLX_RENDER_TYPE); fb_attribs.push_back(GLX_RGBA_FLOAT_ATI_BIT);
    fb_attribs.push_back(None);
    
    int items = 0;
    m_fb_config = glXChooseFBConfig(m_display, scrnum, &fb_attribs.front(), &items);
    if (m_fb_config) {
      m_extension = SH_ARB_ATI_PIXEL_FORMAT_FLOAT;
    }
  }

  if (!m_fb_config) {
    shError(ShException("Could not get GLX FB Config!\n"
                                      "Your card may not support the appropriate extensions."));
  }

  if (m_extension == SH_ARB_NO_FLOAT_EXT) {
    m_fb_config = 0;
    shError(ShException("Could not choose a floating-point extension!\n"
                        "Your card may not support the appropriate extensions."));
  }
}

PBufferContextPtr GLXPBufferFactory::get_context(int width, int height,
                                                 void* id)
{
  for (std::list<GLXPBufferContextPtr>::const_iterator I = m_contexts.begin();
       I != m_contexts.end(); ++I) {
    if ((*I)->width() == width && (*I)->height() == height && (*I)->id() == id) {
      return *I;
    }
  }
  GLXPBufferContextPtr p = create_context(width, height, id);

  if (p) {
    m_contexts.push_back(p);
  }

  return p;
}

GLXPBufferContextPtr GLXPBufferFactory::create_context(int width, int height,
                                                       void* id)
{
  init_display();
  init_config();

  // Set up the pbuffer
  int pbuffer_attribs[] = {
    GLX_PBUFFER_WIDTH, width,
    GLX_PBUFFER_HEIGHT, height,
    GLX_LARGEST_PBUFFER, False,
    None
  };

  GLXPbuffer pbuffer = glXCreatePbuffer(m_display, m_fb_config[0], pbuffer_attribs);
  if (!pbuffer) {
    shError(ShException("Could not make pbuffer!"));
    return 0;
  }

  GLXContext share = glXGetCurrentContext();
  
  GLXContext context = glXCreateNewContext(m_display, m_fb_config[0], GLX_RGBA_TYPE, share, True);
  if (!context) {
    // TODO: delete pbuffer
    shError(ShException("Could not create PBuffer context"));
    return 0;
  }
  
  return new GLXPBufferContext(width, height, id, m_display, pbuffer, context);
}


GLXPBufferFactory* GLXPBufferFactory::m_instance = 0;

GLXPBufferFactory* GLXPBufferFactory::instance()
{
  if (!m_instance) m_instance = new GLXPBufferFactory();

  return m_instance;
}

}
