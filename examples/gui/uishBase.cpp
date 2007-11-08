#include "uishBase.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

uishBase* uishBase::m_cur = 0;

/* uishBase 
 *
 * Lots of stuff copied from CS488's uishBase class
 */
uishBase::uishBase(uishStylePtr style)
  : m_style(style)
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

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
  m_cur = this;

  if(!m_style) {
    m_style = new uishDefaultStyle();
  }
  m_style->colorClear().getValues(m_clearColor);
}

uishBase::~uishBase()
{
  // Nothing to do here right now.
}

void uishBase::invalidate()
{
  // Force a rerender
  //GtkAllocation allocation = get_allocation();
  //get_window()->invalidate_rect(Gdk::Rectangle(&allocation), true);

  queue_draw();
}

void uishBase::setRoot(uishWidgetPtr root)
{
  m_root = root;
}

bool uishBase::enterGL() {
  //std::cout << "enter gl base { " << std::endl;

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

uishStylePtr uishBase::style() {
  return m_style;
}

uishBase* uishBase::current()
{
  if(!m_cur) {
    std::cerr << "Warning - null uishBase" << std::endl;
  }
  return m_cur;
}

void uishBase::leaveGL() {
  //std::cout << "} leave gl base " << std::endl;

  if(!m_gldrawable) {
    std::cerr << "null gldrawable" << std::endl;
  }
  m_gldrawable->gl_end();
  m_gldrawable = 0;
}

void uishBase::on_realize()
{
  std::cerr << "on_realize" << std::endl;
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();

  enterGL();

  // disable depth test - we will render in back to front order 
  glDisable(GL_DEPTH_TEST);
  //glEnable(GL_POLYGON_SMOOTH);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //glHint(GL_POLYGON_SMOOTH, GL_NICEST);
  
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

  if(m_root) {
    m_root->realize();
  }

  leaveGL();
  std::cerr << "done realize" << std::endl;
}

bool uishBase::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if(!enterGL()) {
    return false;
  }

  // Clear the screen

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

  // Modify the current projection matrix so that we move the 
  // camera away from the origin.  We'll draw the game at the
  // origin, and we need to back up to see it.

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if(m_root) {
    m_root->render();
  }

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  leaveGL();

  return true;
}

bool uishBase::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!enterGL()) {
    return false;
  }

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);

  glOrtho(0, event->width, 0, event->height, -1000, 1000);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  if(m_root) {
    m_root->resize(uishResizeEvent(event->width, event->height));
  }

  leaveGL();

  return true;
}

bool uishBase::on_button_press_event(GdkEventButton* event)
{
  std::cerr << "Stub: Button " << event->button << " pressed" << std::endl;

  if(m_root) {
    m_root->button_press(uishButtonEvent(event->time, event->x, get_height() - event->y, 
          event->state, event->button));
  }

  return true;
}

bool uishBase::on_button_release_event(GdkEventButton* event)
{
  std::cerr << "Stub: Button " << event->button << " released" << std::endl;

  if(m_root) {
    m_root->button_release(uishButtonEvent(event->time, event->x, get_height() - event->y, 
          event->state, event->button));
  }
  return true;
}

bool uishBase::on_motion_notify_event(GdkEventMotion* event)
{
  std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;

  if(m_root) {
    m_root->motion(uishMouseEvent(event->time, event->x, get_height() - event->y, event->state));
  }
  return true;
}
