#ifndef UISH_FRAME_HPP
#define UISH_FRAME_HPP

#include <vector>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishBox.hpp"
#include "uishContainer.hpp"

/* A container class.
 * Basically expands to fit its largest child and
 * stretches smaller children to its borders.
 * (Really intended to hold only one child widget,
 * or a set of widgets where only one is visible at a time)
 */
class uishFrame: public uishContainer {
  public:
    uishFrame();
    uishFrame(double width, double height);
    virtual ~uishFrame();

    virtual double width_request();
    virtual double height_request();

    uishBoxPtr box();

  protected:
    uishBoxPtr m_box;

    /* Frames should implement the following event handlers
     * to deal with their children as appropriate */
    virtual void on_render(); 
    virtual void on_resize(const uishResizeEvent& event); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual bool on_button_release(const uishButtonEvent& event); 
    virtual bool on_motion(const uishMouseEvent& event); 
};
typedef SH::ShPointer<uishFrame> uishFramePtr;

#endif
