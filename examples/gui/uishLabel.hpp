#ifndef UISH_LABEL_HPP
#define UISH_LABEL_HPP

#include <string>
#include "uishFontManager.hpp"
#include "uishBox.hpp"

/* Empty box 
 */
class uishLabel: public uishBox { 
  public:
    // set up a label using the uishBase's current style 
    uishLabel(const std::string& label);
    virtual ~uishLabel();

    virtual double width_request();
    virtual double height_request();

  protected:
    int m_fontSize;
    float m_color[4];
    std::string m_fontFamily;
    FTFont* m_font;

    std::string m_label;

    virtual void on_realize(); 
    virtual void on_render(); 
};
typedef SH::ShPointer<uishLabel> uishLabelPtr;

#endif
