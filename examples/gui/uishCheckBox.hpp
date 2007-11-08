#ifndef UISH_CHECKBOX_HPP
#define UISH_CHECKBOX_HPP

#include <string>
#include "uishFrame.hpp"

/* Empty box 
 */
class uishCheckBox: public uishFrame { 
  public:
    // set up a label using the uishBase's current style 
    uishCheckBox(const std::string& label, bool initial_value);
    virtual ~uishCheckBox();

    virtual double width_request();
    virtual double height_request();

    SigC::Signal0<uishToggleEvent> toggle; 

  protected:
    uishLabelPtr m_label;

    bool m_active;

    virtual void on_realize(); 
    virtual void on_render(); 
    virtual bool on_button_press(const uishButtonEvent& event); 
    virtual void on_toggle(const uishToggleEvent& event);
};
typedef SH::ShPointer<uishCheckBox> uishCheckBoxPtr;

#endif
