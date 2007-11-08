#ifndef UISH_GRAPHLAYOUT_HPP
#define UISH_GRAPHLAYOUT_HPP

#include <vector>
#include <list>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishContainer.hpp"

/* A basic container class, similar to  
 */
class uishGraphLayout: public uishContainer {
  public:
    uishGraphLayout();
    uishGraphLayout(double width, double height);
    virtual ~uishGraphLayout();

    virtual void addChild(uishWidgetPtr child);

    /* expands to fit children, but resize can 
     * make it smaller, in which case the panning helps */
    virtual double width_request();
    virtual double height_request();

    /* Returns the bounding box surrounding the graph 
     * (x.min, x.max, y.min, y.max)*/ 
    SH::ShAttrib4d bounding_box();


    /* very simple edges right now (only between direct children)
     * later may want to allow edges between any descendents
     */
    virtual void addEdge(uishWidgetPtr from, uishWidgetPtr to);

  protected:
    // pan and zoom parameters
    SH::ShPoint2d press;
    SH::ShPoint2d last;
    float m_scale;
    SH::ShVector2f m_offset;

    //ShMatrix3x3f trans;
    //ShMatrix3x3f inv_trans;

    // offsets for each widget
    typedef std::vector<SH::ShVector2f> OffsetVec;
    OffsetVec m_childOffsets;

    // edges in the graph 
    // just goes from center of nodes for now
    typedef std::pair<size_t, size_t> Edge;
    typedef std::list<Edge> EdgeList;
    EdgeList m_edges;


    /* get the child at the given coordinates 
     * and the offset for that child */
    uishWidgetPtr getChild(double x, double y, uishMouseEvent& offset_event);

    virtual bool uses_stencil(); 

    virtual void on_render(); 
    virtual void on_resize(const uishResizeEvent& event); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual bool on_button_release(const uishButtonEvent& event); 

    // left/right pans
    // middle mouse y-axis zooms centered at mouse point
    virtual bool on_motion(const uishMouseEvent& event); 

    virtual void doLayout();
};
typedef SH::ShPointer<uishGraphLayout> uishGraphLayoutPtr;

#endif
