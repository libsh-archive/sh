#ifndef UISH_BASE_HPP
#define UISH_BASE_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishWidget.hpp"
#include "uishStyle.hpp"


/* The OpenGL canvas inside which uish operates.  
 * This sets up the initial orthographic projection
 *
 * Some stuff copied from CS488's Viewer class
 */
class uishBase: public Gtk::GL::DrawingArea {
  public:
    uishBase(uishStylePtr style = 0); 

    virtual ~uishBase();

    virtual void invalidate();

    virtual void setRoot(uishWidgetPtr root);

    // enters GL context 
    bool enterGL(); 
    void leaveGL(); 

    // returns active style
    uishStylePtr style();

    static uishBase* current();

  protected:
    uishWidgetPtr m_root;
    uishStylePtr m_style;

    float m_clearColor[4];
    
    static uishBase* m_cur; 

    Glib::RefPtr<Gdk::GL::Drawable> m_gldrawable;

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

};

#endif
