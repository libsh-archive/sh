#ifndef UISH_CONTAINER_HPP
#define UISH_CONTAINER_HPP

#include <vector>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include "uishWidget.hpp"

/* A container class.
 * Subclasses of this class are responsible for
 * implementing a event handlers tha deal with children appropriately. 
 */
class uishContainer: public uishWidget {
  public:
    uishContainer();
    uishContainer(double width, double height);
    virtual ~uishContainer();

    typedef std::vector<uishWidgetPtr> WidgetVec;
    typedef WidgetVec::iterator iterator;
    typedef WidgetVec::reverse_iterator reverse_iterator;

    /** Access children
     * @{ */
    iterator begin() { return m_children.begin(); }
    iterator end() { return m_children.end(); }

    reverse_iterator rbegin() { return m_children.rbegin(); }
    reverse_iterator rend() { return m_children.rend(); }
    /* @} */

    virtual void addChild(uishWidgetPtr child);

  protected:
    WidgetVec m_children;

    /* Containers should implement the following event handlers
     * to deal with their children as appropriate */
    virtual void on_realize();
    virtual void on_render() = 0; 
    virtual void on_resize(const uishResizeEvent& event) = 0; 
    virtual bool on_button_press(const uishButtonEvent& event) = 0; 
    virtual bool on_button_release(const uishButtonEvent& event) = 0; 
    virtual bool on_motion(const uishMouseEvent& event) = 0; 
};
typedef SH::ShPointer<uishContainer> uishContainerPtr;

#endif
