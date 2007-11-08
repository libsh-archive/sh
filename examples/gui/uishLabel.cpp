#include <GL/gl.h>
#include <GL/glu.h>
#include "uishBase.hpp"
#include "uishLabel.hpp"

uishLabel::uishLabel(const std::string& label)
  : uishBox(0, 0, 0),
    m_label(label)
{
  name(m_label); // set the default name to be the label

  uishStylePtr style = uishBase::current()->style();
  m_fontSize = style->fontSize();
  style->colorText().getValues(m_color);
  m_fontFamily = style->fontFamily();
}

uishLabel::~uishLabel()
{}

double uishLabel::width_request() {
  on_realize();
  return m_width_request;
}

double uishLabel::height_request() {
  on_realize();
  return m_height_request; 
}

void uishLabel::on_realize() {
  m_font = uishFontManager::getFont(m_fontFamily, m_fontSize);

  float llx, lly, llz, urx, ury, urz;
  m_font->BBox(m_label.c_str(), llx, lly, llz, urx, ury, urz);
  set_size_request(urx - llx + 2 * borderSize(), ury - lly + 2 * borderSize());
//  std::cerr << "label=" << m_label << " size = " << size()
//    << " dx = " << (urx - llx) << " dy = " << (ury - lly)
//    << " borderSize = " << borderSize() << std::endl;;
}

void uishLabel::on_render() {
  uishBox::on_render();
  glPushAttrib(GL_CURRENT_BIT);
  glPushMatrix();
    // centered alignment 
    // @todo make this arbitrary (i.e. compass directions)
    //
    float llx, lly, llz, urx, ury, urz;
    m_font->BBox(m_label.c_str(), llx, lly, llz, urx, ury, urz);
    float xoff = borderSize() + (interiorWidth() - (urx - llx)) * 0.5;
    float yoff = borderSize() + (interiorHeight() - (ury - lly)) * 0.5 - lly;

    glTranslatef(xoff, yoff, 0);
    glColor3fv(m_color);
    m_font->Render(m_label.c_str());
  glPopMatrix();
  glPopAttrib();
}


