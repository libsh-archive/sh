#include <GL/gl.h>
#include "uishBorder.hpp"

uishBasicBorder::uishBasicBorder(SH::ShColor4f color) {
  color.getValues(m_left);
  memcpy(m_right, m_left, 4 * sizeof(float));
  memcpy(m_top, m_left, 4 * sizeof(float));
  memcpy(m_bottom, m_left, 4 * sizeof(float));
}

uishBasicBorder::uishBasicBorder(SH::ShColor4f left, SH::ShColor4f right, 
                    SH::ShColor4f top, SH::ShColor4f bottom) {
  left.getValues(m_left);
  right.getValues(m_right);
  top.getValues(m_top);
  bottom.getValues(m_bottom);
}

void uishBasicBorder::operator()(double width, double height, double border) {
  glPushAttrib(GL_CURRENT_BIT);
  glBegin(GL_QUADS);
    glColor4fv(m_bottom);
    glVertex2f(border, border);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width - border, border);

    glColor4fv(m_right);
    glVertex2f(width - border, border);
    glVertex2f(width, 0);
    glVertex2f(width, height);
    glVertex2f(width - border, height - border);

    glColor4fv(m_top);
    glVertex2f(width - border, height - border);
    glVertex2f(width, height);
    glVertex2f(0, height);
    glVertex2f(border, height - border);

    glColor4fv(m_left);
    glVertex2f(border, height - border);
    glVertex2f(0, height);
    glVertex2f(0, 0);
    glVertex2f(border, border);
  glEnd();
  glPopAttrib();
}

double uishBasicBorder::default_size() {
  return 2;
}


