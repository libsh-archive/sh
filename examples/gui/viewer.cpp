#include <iostream>

#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#undef GL_GLEXT_PROTOTYPES
#include <GL/glu.h>
#include "viewer.hpp"

using namespace SH;
using namespace ShUtil;

ViewerState::ViewerState() {
  lightPos.name("lightPos");
  lightPos = ShConstPoint3f(5.0f, 5.0f, 5.0f);

  /* default view */
  camera.move(0.0, 0.0, -10.0);
}

void ViewerState::updateView()
{
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
}

void ViewerState::invalidate() {
  // invalidate all linked m_viewers
  for(ViewerSet::iterator I = m_viewers.begin(); I != m_viewers.end(); ++I) {
    (*I)->invalidate();
  }
}

void ViewerState::linkViewer(Viewer* viewer) {
  m_viewers.insert(viewer);
}

void ViewerState::unlinkViewer(Viewer* viewer) {
  m_viewers.erase(viewer);
}

Viewer::Viewer(ViewerState& state)
  : m_state(state), 
    init(false)
{
  m_state.linkViewer(this);
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig.is_null()) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);
}

Viewer::~Viewer()
{
  m_state.unlinkViewer(this);
  // Nothing to do here right now.
}

void Viewer::setFsh(ShProgram fsh)
{
  if(!enterGL()) return;

  try {
    initShaders();
    m_fsh = namedConnect(m_lightsh, fsh);
    m_vsh = namedAlign(m_basevsh, m_fsh);
    shBind(m_vsh);
    shBind(m_fsh);

    m_vsh.node()->dump("vsh");
    m_fsh.node()->dump("fsh");
  } catch(const ShException &e) {
    std::cerr << "Error: " << e.message() << std::endl;
  } catch(...) {
    std::cerr << "Caught unknown error: " << std::endl;
  }
  leaveGL();

  invalidate();
}



void Viewer::initShaders()
{
  if(init) return;

  m_basevsh = ShKernelLib::shVsh(m_state.mv, m_state.mvd);
  m_basevsh = m_basevsh << shExtract("lightPos") << m_state.lightPos;

  m_lightsh = ShKernelLight::pointLight<ShColor3f>() << ShConstColor3f(1.0f, 1.0f, 1.0f);
  init = true;
}

bool Viewer::enterGL() {
  m_gldrawable = get_gl_drawable();
  
  if (!m_gldrawable)  {
    std::cerr << "could not get_gl_drawable" << std::endl;
    return false;
  }

  if (!m_gldrawable->gl_begin(get_gl_context())) {
    m_gldrawable = 0;
    std::cerr << "could not gl_begin" << std::endl;
    return false;
  }

  return true;
}

void Viewer::leaveGL() {

  if(!m_gldrawable) {
    std::cerr << "null gldrawable" << std::endl;
  }
  m_gldrawable->gl_end();
  m_gldrawable = 0;
}


void Viewer::invalidate()
{
  // Force a rerender
  //GtkAllocation allocation = get_allocation();
  //get_window()->invalidate_rect(Gdk::Rectangle(&allocation), true);

  queue_draw();
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  if(!enterGL()) return;
  shInit();
  shSetBackend("arb");

  initShaders();

  glClearColor( 0.0, 0.0, 0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_VERTEX_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  std::cout << gluErrorString(glGetError()) << std::endl;

  // Place the camera at its initial position
  m_state.updateView();

  leaveGL();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  if(!enterGL()) return false;

  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_state.updateView();

  // Draw stuff
  Gdk::GL::Drawable::draw_teapot(true, 1.0f); 

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  m_gldrawable->swap_buffers();

  leaveGL();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  if(!enterGL()) return false;

  glViewport(0, 0, event->width, event->height);
  m_state.camera.glProjection((float)get_width() / get_height());
  m_state.updateView();

  leaveGL();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  m_curx = event->x;
  m_cury = event->y;
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  double factor = 20.0;
  bool changed = false;

  if(event->state & GDK_BUTTON1_MASK) {
    m_state.camera.orbit((int)m_curx, (int)m_cury, (int)event->x, (int)event->y,  
                 get_width(),
                 get_height()); 
    changed = true;
  }

  if(event->state & GDK_BUTTON2_MASK) {
    m_state.camera.move(0, 0, (event->y - m_cury)/factor);
    changed = true;
  }

  if(event->state & GDK_BUTTON3_MASK) {
    m_state.camera.move((event->x - m_curx)/factor, -(event->y - m_cury)/factor, 0);
    changed = true;
  }

  m_curx = event->x;
  m_cury = event->y;
  
  if(changed) {
    m_state.updateView();
    m_state.invalidate();
  }

  return true;
}
