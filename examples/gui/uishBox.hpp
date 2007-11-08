#ifndef UISH_BOX_HPP
#define UISH_BOX_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishStyle.hpp"
#include "uishBorder.hpp"
#include "uishWidget.hpp"

/* Empty box 
 */
class uishBox: public uishWidget { 
  public:
    // set up a zero sized box, default borders
    uishBox();

    // set up a box with the requested width and height and borders
    // if border = 0, uses Base's default borders
    // if border_size = -1, uses the border's default size
    uishBox(double width, double height, double border_size=-1, uishBorderPtr border = 0); 
    virtual ~uishBox();

    double borderSize(); 
    void borderSize(double borderSize);

    double interiorWidth();
    double interiorHeight();

    void set_background(SH::ShColor4f color); 

  protected:
    /* initializes using the given border_size and border (same options as)*/ 
    void init(double border_size, uishBorderPtr border); 

    void update_size();
    virtual void on_render(); 

    SH::ShColor4f m_background;
    uishBorderPtr m_border;
    double m_borderSize; 

};
typedef SH::ShPointer<uishBox> uishBoxPtr;

#endif
