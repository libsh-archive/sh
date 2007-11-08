#ifndef UISH_FIXEDFRAME_HPP
#define UISH_FIXEDFRAME_HPP

#include <vector>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishBox.hpp"
#include "uishContainer.hpp"

/* A container class.
 * Scales children to fit within this
 */
class uishFixedFrame: public uishContainer {
  public:
    uishFixedFrame(double width, double height);
    virtual ~uishFixedFrame();

    uishBoxPtr box();

  protected:
    uishBoxPtr m_box;

    typedef std::vector<double> DoubleVec;
    typedef std::vector<SH::ShVector2f> TranslateVec;

    /* should probably just use a 3x3 matrix, but this is faster
     * to code up for now */
    TranslateVec m_childTranslates;
    DoubleVec m_childScales;

    /* get the child at the given coordinates                                           
     *      * and the offset for that child */                                                 
    uishWidgetPtr getChild(double x, double y, uishMouseEvent& offset_event);    

    /* FixedFrames should implement the following event handlers
     * to deal with their children as appropriate */
    virtual void on_render(); 
    virtual void on_resize(const uishResizeEvent& event); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual bool on_button_release(const uishButtonEvent& event); 
    virtual bool on_motion(const uishMouseEvent& event); 
};
typedef SH::ShPointer<uishFixedFrame> uishFixedFramePtr;

#endif
