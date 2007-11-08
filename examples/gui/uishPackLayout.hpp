#ifndef UISH_PACKLAYOUT_HPP
#define UISH_PACKLAYOUT_HPP

#include <vector>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishContainer.hpp"

/* A basic container class, similar to  
 */
class uishPackLayout: public uishContainer {
  public:
    enum Direction {
      HORIZONTAL,
      VERTICAL
    };

    // could also have other stuff like stretch, align fields
    // but for now it just stretches children in both directions,
    // leaving pad between widgets 
    //
    // Also you cannot dynamically change any of these params right now.

    uishPackLayout(Direction dir = HORIZONTAL, double pad = -1);
    virtual ~uishPackLayout();

    /* returns the minimum requested width */ 
    virtual double width_request();
    virtual double height_request();

    virtual void addChild(uishWidgetPtr child);


  protected:
    typedef std::vector<double> OffsetVec;

    // holds the starting offsets for all child widgets
    // as well as the end of the last widget
    //
    // updated by the resize signal
    OffsetVec m_offsets; 

    Direction m_direction;

    double m_pad;

    double get_minwidth();
    double get_minheight();

    /* get the child at the given coordinates 
     * and the offset for that child */
    uishWidgetPtr getChild(double x, double y, uishMouseEvent& offset_event);

    virtual void on_render(); 
    virtual void on_resize(const uishResizeEvent& event); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual bool on_button_release(const uishButtonEvent& event); 
    virtual bool on_motion(const uishMouseEvent& event); 
};
typedef SH::ShPointer<uishPackLayout> uishPackLayoutPtr;

#endif
