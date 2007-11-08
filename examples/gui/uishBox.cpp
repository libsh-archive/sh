#include "uishBox.hpp"
#include "uishBase.hpp"
#include "uishConstants.hpp"

uishBox::uishBox() {
  init(-1, 0);
}

uishBox::uishBox(double width, double height, double border_size, uishBorderPtr border) 
  : uishWidget(width, height),
    m_border(border)
{
  init(border_size, border);
  m_background = m_base->style()->colorBackground();
}

uishBox::~uishBox()
{}


double uishBox::borderSize() {
  return m_borderSize;
}

void uishBox::borderSize(double borderSize) {
  if(borderSize == -1) {
    m_borderSize = m_border->default_size();
  }
  m_borderSize = std::max(borderSize, 0.0);
  update_size();
}

double uishBox::interiorWidth() {
  return width() - 2 * borderSize();
}

double uishBox::interiorHeight() {
  return height() - 2 * borderSize();
}

void uishBox::set_background(SH::ShColor4f color) {
  m_background = color;
}


void uishBox::on_render() {
  float background[4];
  m_background.getValues(background);

  // fill in background
  glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(background);
    glBegin(GL_QUADS);
      glVertex2f(0, 0);
      glVertex2f(width(), 0); 
      glVertex2f(width(), height()); 
      glVertex2f(0, height()); 
    glEnd();
  glPopAttrib();

  // draw border (over edge of background)
  (*m_border)(width(), height(), m_borderSize);
}

void uishBox::init(double border_size, uishBorderPtr border) {
  uishStylePtr style = uishBase::current()->style();
  if(!style) {
    std::cerr << "Unable to get style" << std::endl;
  }
  if(!m_border) {
    m_border = style->border(); 
  }
  m_borderSize = border_size == -1 ? m_border->default_size() : border_size;
  update_size();
}

void uishBox::update_size() {
  m_width_request = std::max(m_width_request, m_borderSize * 2);
  m_height_request = std::max(m_height_request, m_borderSize * 2);
}
