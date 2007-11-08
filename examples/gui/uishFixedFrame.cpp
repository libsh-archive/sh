#include "uishFixedFrame.hpp"

using namespace SH;

uishFixedFrame::uishFixedFrame(double width, double height) 
  : uishContainer(width, height),
    m_box(new uishBox(width, height))
{ 
  std::cerr << "width = " << m_width_request << " height = " << m_height_request << std::endl;
}

uishFixedFrame::~uishFixedFrame()
{ }

uishBoxPtr uishFixedFrame::box() {
  return m_box;
}


void uishFixedFrame::on_render()
{
  m_box->render();
  /* should put this somewhere else */
  m_childScales.clear();
  m_childTranslates.clear();
  std::cerr << "on_render" << name() << " num children " << m_children.size() << " size=" << size() << std::endl; 

  for(iterator I = begin(); I != end(); ++I) {
    std::cerr << "fixed frame child " << (*I)->name() << std::endl;
    double child_width = (*I)->width_request();
    double child_height = (*I)->height_request();
    uishResizeEvent childEvent(child_width, child_height);
    (*I)->resize(childEvent);

    // push scaling, translation onto  
    double width_scale = m_box->interiorWidth() / child_width;
    double height_scale = m_box->interiorHeight() / child_height;
    std::cerr << "  width_scale " << width_scale << " height_scale " << height_scale << std::endl;
    if(width_scale < height_scale) { // use width_scale
      m_childTranslates.push_back(
          ShConstVector2f(0.0, m_box->interiorHeight() * (1.0 - width_scale / height_scale) * 0.5f));
      m_childScales.push_back(width_scale);
    } else {
      m_childTranslates.push_back(
          ShConstVector2f(m_box->interiorWidth() * (1.0 - height_scale / width_scale) * 0.5f, 0.0));
      m_childScales.push_back(height_scale);
    }
    std::cerr << "   t" << m_childTranslates.back() << std::endl
              << "   s" << m_childScales.back() << std::endl;
  }

  TranslateVec::iterator T = m_childTranslates.begin();
  DoubleVec::iterator S = m_childScales.begin();
  for(iterator I = begin(); I != end(); ++I, ++T, ++S) {
    std::cerr << "fixed rendering " << (*I)->name() << std::endl;
    glPushMatrix();
      glTranslated((*T).getValue(0) + m_box->borderSize(),
                   (*T).getValue(1) + m_box->borderSize(), 0);
      glScaled((*S), (*S), 1);
      (*I)->render();
    glPopMatrix();
  }
}


void uishFixedFrame::on_resize(const uishResizeEvent& event) {
  m_box->resize(event);
  m_width = m_width_request = event.width; 
  m_height = m_height_request = event.height; 
}

uishWidgetPtr uishFixedFrame::getChild(double x, double y, uishMouseEvent& offset_event) {
  // @todo assumes opacity in overlapping widgets...but should not
  TranslateVec::reverse_iterator T = m_childTranslates.rbegin();
  DoubleVec::reverse_iterator S = m_childScales.rbegin();

  for(reverse_iterator I = rbegin(); I != rend(); ++I, ++T, ++S) {
    if((*I)->is_active()) {
      double tx, ty;
      tx = (x - m_box->borderSize() - (*T).getValue(0)) / (*S);
      ty = (y - m_box->borderSize() - (*T).getValue(1)) / (*S);

      if(tx < 0 || ty < 0 || tx > (*I)->width() || ty > (*I)->height()) {
        continue;
      }
      offset_event.x = tx;
      offset_event.y = ty;
      return (*I);
    }
  }
  return 0;
}

bool uishFixedFrame::on_button_press(const uishButtonEvent& event) {
  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent);
  return (child && child->button_press(offsetEvent));
}

bool uishFixedFrame::on_button_release(const uishButtonEvent& event) {
  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent);
  return (child && child->button_release(offsetEvent));
}

bool uishFixedFrame::on_motion(const uishMouseEvent& event) {
  uishMouseEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent);
  return (child && child->motion(offsetEvent));
}

