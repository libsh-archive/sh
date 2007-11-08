#ifndef UISH_WIDGET_HPP
#define UISH_WIDGET_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishConstants.hpp"
#include "uishEvents.hpp"

struct HandlerMarshaller {
  typedef bool OutType;
  typedef bool InType;
  
  HandlerMarshaller()
    : m_handled(false) {} 

  OutType value() { return m_handled; } 

  static OutType default_value() { return false; }

  bool marshal(InType newval)
  { 
      m_handled |= newval;
      return false;     // continue emittion process
  };

  private:
    bool m_handled; 
};


class uishBase;
class uishContainer;


/* 
 * @todo Event processing needs lots of work
 *  -should use picking?
 *  -mouse events with button down should go to widget that handled
 *  most recent button press
 *  -should not pass events on to inactive widgets
 */

/* The fundamental base class of all uish classes.  
 */
class uishWidget: public virtual SH::ShRefCountable, public virtual SigC::Object, public virtual SH::ShMeta {
  public:
    uishWidget();
    uishWidget(double width_request, double height_request);
    virtual ~uishWidget();

    SH::ShPointer<uishContainer> parent();
    void setParent(SH::ShPointer<uishContainer> parent);

    /* Signals for the various events  
     */
    SigC::Signal0<void> realize; 
    SigC::Signal0<void> render; 
    SigC::Signal1<void, const uishResizeEvent&> resize; 
    SigC::Signal1<bool, const uishButtonEvent&, HandlerMarshaller> button_press; 
    SigC::Signal1<bool, const uishButtonEvent&, HandlerMarshaller> button_release; 
    SigC::Signal1<bool, const uishMouseEvent&, HandlerMarshaller> motion; 

    /* Returns requested width/height >= minimum widt/height required by this
     * widget (similar methods to gtkmm) */
    virtual double width_request();
    virtual double height_request();

    /* Returns actual current width/height >= width_request/height_request
     * May be set by resize 
     */
    virtual double width();
    virtual double height();

    virtual SH::ShAttrib2d size();
    
    /* Sets requested size (clamped >= minium size of the widget) */
    virtual void set_size_request(double width, double height);

    /* Returns whether this widget should be rendered */
    void set_active(bool active);
    bool is_active(); 

    /* Returns whether this uses the stencil buffer during rendering.
     * This is useful for widgets whose children may exceed its boundaries.
     * Then the stencil buffer may be used to clamp rendering */
    virtual bool uses_stencil(); 

  protected:
    uishBase* m_base; 

    SH::ShPointer<uishContainer> m_parent;

    /* widget should be able to request a minumum size */
    double m_width_request, m_height_request;

    /* widget should have actual size */
    double m_width, m_height;

    /* whether the widget should be rendered 
     * defaults to true */
    bool m_active;

    /* realized flag */
    bool m_realized;

    /* checks if active, then calls on_realize if necessary,
     * then on_render */
    void trigger_render();
    void trigger_realize();
    void resize_debug(const uishResizeEvent& event); 
    bool button_debug(const uishButtonEvent& event); 
    bool motion_debug(const uishMouseEvent& event); 

    virtual void on_realize() {} 
    virtual void on_render() {} 
    virtual void on_resize(const uishResizeEvent& event); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual bool on_button_release(const uishButtonEvent& event); 
    virtual bool on_motion(const uishMouseEvent& event); 


    void init();
};
typedef SH::ShPointer<uishWidget> uishWidgetPtr;

#endif
