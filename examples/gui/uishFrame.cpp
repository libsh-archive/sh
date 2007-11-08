#include "uishFrame.hpp"

uishFrame::uishFrame() 
  : m_box(new uishBox(0,0))
{ }

uishFrame::uishFrame(double width, double height) 
  : uishContainer(width, height), 
    m_box(new uishBox(width, height))
{ }

uishFrame::~uishFrame()
{ }

double uishFrame::width_request() {
  m_width_request = 0;
  for(iterator I = begin(); I != end(); ++I) {
    m_width_request = std::max(m_width_request, 
        (*I)->width_request() + 2 * m_box->borderSize());
  }
  return m_width_request;
}

double uishFrame::height_request() {
  m_height_request = 0;
  for(iterator I = begin(); I != end(); ++I) {
    m_height_request = std::max(m_height_request, 
        (*I)->height_request() + 2 * m_box->borderSize());
  }
  return m_height_request;
}

uishBoxPtr uishFrame::box() {
  return m_box;
}

void uishFrame::on_render()
{
  m_box->render();
  glPushMatrix();
    glTranslatef(m_box->borderSize(), m_box->borderSize(), 0);
    for(iterator I = begin(); I != end(); ++I) {
      (*I)->render();
    }
  glPopMatrix();
}


void uishFrame::on_resize(const uishResizeEvent& event) {
  m_width = std::max(event.width, width_request()); 
  m_height = std::max(event.height, height_request()); 
  m_box->resize(uishResizeEvent(m_width, m_height));
  uishResizeEvent childEvent(m_width - 2 * m_box->borderSize(),
                             m_height - 2 * m_box->borderSize());
  for(iterator I = begin(); I != end(); ++I) {
    (*I)->resize(childEvent);
  }
}

bool uishFrame::on_button_press(const uishButtonEvent& event) {
  uishButtonEvent childEvent = event;
  childEvent.x -= m_box->borderSize();
  childEvent.y -= m_box->borderSize();
  for(reverse_iterator I = rbegin(); I != rend(); ++I) {
    if((*I)->is_active()) {
      return (*I)->button_press(childEvent);
    }
  }
  return false;
}

bool uishFrame::on_button_release(const uishButtonEvent& event) {
  uishButtonEvent childEvent = event;
  childEvent.x -= m_box->borderSize();
  childEvent.y -= m_box->borderSize();
  for(reverse_iterator I = rbegin(); I != rend(); ++I) {
    if((*I)->is_active()) {
      return (*I)->button_release(childEvent);
    }
  }
  return false;
}

bool uishFrame::on_motion(const uishMouseEvent& event) {
  uishMouseEvent childEvent = event;
  childEvent.x -= m_box->borderSize();
  childEvent.y -= m_box->borderSize();
  for(reverse_iterator I = rbegin(); I != rend(); ++I) {
    if((*I)->is_active()) {
      return (*I)->motion(childEvent);
    }
  }
  return false;
}

