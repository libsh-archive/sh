#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <set>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include "Camera.hpp"


/* Represents the state of a viewer
 * May be shared by several different views */
class Viewer;
struct ViewerState {
  ViewerState();
  void updateView(); ///< updates matrices
  void invalidate();  ///< invalidates all linked viewers

  void linkViewer(Viewer* viewer);
  void unlinkViewer(Viewer* viewer); 

  Camera camera;
  SH::ShMatrix4x4f mv, mvd; // @todo probably don't want mvd here unless always same aspect?
  SH::ShPoint3f lightPos;

  private:
    typedef std::set<Viewer*> ViewerSet;
    ViewerSet m_viewers;
};

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer(ViewerState& state);
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();

  void setFsh(SH::ShProgram fsh);
  
protected:
  ViewerState &m_state;

  double m_curx, m_cury;
  bool init;


  SH::ShProgram m_basevsh, m_vsh, m_lightsh, m_fsh;

  void updateView();
  void initShaders();

  Glib::RefPtr<Gdk::GL::Drawable> m_gldrawable;
  bool enterGL();
  void leaveGL();

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

private:
};

#endif
