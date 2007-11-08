#ifndef UISH_BORDER_HPP
#define UISH_BORDER_HPP

#include <sh/sh.hpp>

/* Empty box 
 */
class uishBorder: public SH::ShRefCountable {
  public:
    /* renders a border in the region between rectangles
     * [0,width]x[0,height] and [border,width-border]x[border,height-border]
     */
    virtual void operator()(double width, double height, double border) = 0;

    /* returns the default size of the border */
    virtual double default_size() = 0; 
};
typedef SH::ShPointer<uishBorder> uishBorderPtr;

/* simple rectangular border
 * @todo very easy to allow arbitrary polys...
 */
class uishBasicBorder: public uishBorder {
  public:
    uishBasicBorder(SH::ShColor4f color); 
    uishBasicBorder(SH::ShColor4f left, SH::ShColor4f right, 
                    SH::ShColor4f top, SH::ShColor4f bottom);
    virtual void operator()(double width, double height, double border);
    virtual double default_size(); 

  protected:
    float m_left[4], m_right[4], m_top[4], m_bottom[4];
};

#endif
