#include "uishBase.hpp"
#include "uishPackLayout.hpp"

uishPackLayout::uishPackLayout(Direction dir, double pad) 
  : m_direction(dir), 
    m_pad(pad)
{ 
  uishStylePtr style = uishBase::current()->style();
  if(m_pad == -1) {
    m_pad = style->spacePad();
  }
}

uishPackLayout::~uishPackLayout()
{ }

double uishPackLayout::width_request() 
{
  if(m_direction == HORIZONTAL) {
    m_width_request = std::max(m_width_request, get_minwidth() + (m_children.size() + 1) * m_pad);  
  } else {
    for(iterator I = begin(); I != end(); ++I) {
      m_width_request = std::max(m_width_request, (*I)->width_request() + m_pad * 2);
    }
  }
  return m_width_request;
}

double uishPackLayout::height_request() 
{
  if(m_direction == VERTICAL) {
    m_height_request = std::max(m_height_request, get_minheight() + (m_children.size() + 1) * m_pad);  
  } else {
    for(iterator I = begin(); I != end(); ++I) {
      m_height_request = std::max(m_height_request, (*I)->height_request() + m_pad * 2);
    }
  }
  return m_height_request;
}

void uishPackLayout::addChild(uishWidgetPtr child)
{
  uishContainer::addChild(child);
}

double uishPackLayout::get_minwidth() {
  double total = 0;
  for(iterator I = begin(); I != end(); ++I) {
    total += (*I)->width_request();
  }
  return total;
}

double uishPackLayout::get_minheight() {
  double total = 0;
  for(iterator I = begin(); I != end(); ++I) {
    total += (*I)->height_request();
  }
  return total;
}

void uishPackLayout::on_render()
{
  SH_DEBUG_ASSERT(m_offsets.size() == m_children.size());
  glPushMatrix();
  if(m_direction == HORIZONTAL) {
    glTranslatef(0, m_pad, 0);
  } else {
    glTranslatef(m_pad, 0, 0);
  }
  double last = 0;
  OffsetVec::iterator O = m_offsets.begin();
  for(iterator I = begin(); I != end(); ++I, last = *O, ++O) {
    if(m_direction == HORIZONTAL) {
      glTranslatef(*O - last, 0, 0);
    } else {
      glTranslatef(0, *O - last, 0);
    }
    (*I)->render();
  }
  glPopMatrix();
}

/* find the child where this is in */
bool uishPackLayout::on_button_press(const uishButtonEvent& event) {
  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(!child) {
    std::cout << "no child" << std::endl;
    return false;
  }
  return child->button_press(offsetEvent);
}

bool uishPackLayout::on_button_release(const uishButtonEvent& event) {
  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(!child) {
    std::cout << "no child" << std::endl;
    return false;
  }
  return child->button_release(offsetEvent);
}

bool uishPackLayout::on_motion(const uishMouseEvent& event) {
  uishMouseEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(!child) {
    std::cout << "no child" << std::endl;
    return false;
  }
  return child->motion(offsetEvent);
}

uishWidgetPtr uishPackLayout::getChild(double x, double y, uishMouseEvent& offsetEvent)
{
  iterator I = begin();
  OffsetVec::iterator O = m_offsets.begin();
  SH_DEBUG_ASSERT(m_children.size() == m_offsets.size());

  if(m_direction == HORIZONTAL) {
    if(y < m_pad || y > height() - m_pad) {
      return 0;
    }
    for(;I != end(); ++I, ++O) {
      if(x < *O) return 0;
      if(x < *O + (*I)->width()) {
        offsetEvent.x -= *O;
        offsetEvent.y -= m_pad;
        return *I;
      }
    }
  } else {
    if(x < m_pad || x > width() - m_pad) {
      return 0;
    }
    for(;I != end(); ++I, ++O) {
      if(y < *O) return 0;
      if(y < *O + (*I)->height()) {
        offsetEvent.x -= m_pad;
        offsetEvent.y -= *O;
        return *I;
      }
    }
  }
  return 0;
}

void uishPackLayout::on_resize(const uishResizeEvent& event)
{
  m_width = event.width;
  m_height = event.height;

  double length, other, num, m, scale;
  m_offsets.clear();

  /* Get min necessary = m
   * n = # of widgets
   * length = (n+1) * pad + m * s 
   * s = (length - (n + 1) * pad) / m
   *
   * other is the length of the other dimension for child widgets
   */
  num = m_children.size();
  if(m_direction == HORIZONTAL) {
    length = width();
    m = get_minwidth();
    other = height() - 2 * m_pad;
  } else {
    length = height();
    m = get_minheight();
    other = width() - 2 * m_pad;
  }
  scale = (length - (num + 1) * m_pad) / m; 
//  std::cerr << "  num = " << num << " length = " << length 
//    << " m = " << m << " pad = " << m_pad << " other = " << other << std::endl;

  m_offsets.clear();
  double offset = m_pad;
  for(iterator I = begin(); I != end(); ++I) {
    std::cerr << "  offset = " << offset << std::endl;
    m_offsets.push_back(offset);
    if(m_direction == HORIZONTAL) {
      double child_width = (*I)->width_request() * scale;
      (*I)->resize(uishResizeEvent(child_width, other)); 
      offset += child_width;
    } else {
      double child_height = (*I)->height_request() * scale;
      (*I)->resize(uishResizeEvent(other, child_height)); 
      offset += child_height;
    }
    offset += m_pad;
  }
  m_offsets.push_back(offset);
}

