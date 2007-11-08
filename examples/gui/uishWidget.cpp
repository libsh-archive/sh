#include "uishContainer.hpp"
#include "uishBase.hpp"
#include "uishWidget.hpp"

uishWidget::uishWidget()
  : m_width_request(0), m_height_request(0), 
    m_width(0), m_height(0),
    m_active(true),
    m_realized(false)

{
  init();
}

uishWidget::uishWidget(double width_request, double height_request)
  : m_width_request(width_request), m_height_request(height_request),
    m_width(width_request), m_height(height_request),
    m_active(true),
    m_realized(false)
{
  std::cerr << "widget width = " << m_width_request << " height = " << m_height_request << std::endl;
  init();
}

uishWidget::~uishWidget()
{
}

uishContainerPtr uishWidget::parent()
{
  return m_parent;
}

void uishWidget::setParent(SH::ShPointer<uishContainer> parent) {
  m_parent = parent;
}

double uishWidget::width_request()
{
  return m_width_request;
}

double uishWidget::height_request()
{
  return m_height_request;
}

double uishWidget::width()
{
  m_width = std::max(m_width, width_request());
  return m_width;
}

double uishWidget::height()
{
  m_height = std::max(m_height, height_request());
  return m_height;
}

SH::ShAttrib2d uishWidget::size() {
  return SH::ShConstAttrib2d(width(), height());
}


void uishWidget::set_size_request(double width, double height)
{
  m_width_request = width;
  m_height_request = height;
}

void uishWidget::set_active(bool active)
{
  if(m_active != active) {
    m_active = active;
    m_base->invalidate();
  }
}

bool uishWidget::is_active()
{
  return m_active;
}

void uishWidget::trigger_render()
{
  if(m_active) {
    trigger_realize();
    on_render();
  }
}

void uishWidget::trigger_realize() {
  if(m_realized) return;
  on_realize();
  m_realized = true;
}

void uishWidget::on_resize(const uishResizeEvent& event)
{
  m_width = std::max(width_request(), event.width);
  m_height = std::max(height_request(), event.height);
}

void uishWidget::resize_debug(const uishResizeEvent& event)
{
  //std::cerr << "resize " << event.width 
  //          << ", " << event.height << " name = " << name() << std::endl;
}

bool uishWidget::on_button_press(const uishButtonEvent& event) {
  //std::cerr << "widget button press at " << event.x << " " << event.y << std::endl << " name = " << name() << std::endl; 
  return false;
}

bool uishWidget::on_button_release(const uishButtonEvent& event) {
  //std::cerr << "widget button release at " << event.x << " " << event.y << std::endl << " name = " << name() << std::endl; 
  return false;
}
 
bool uishWidget::on_motion(const uishMouseEvent& event) {
  //std::cerr << "widget mouse motion at " << event.x << " " << event.y << " name= " << name() << std::endl; 
  return false;
}

bool uishWidget::button_debug(const uishButtonEvent& event) {
  //std::cerr << "widget button event at " << event.x << " " << event.y << " name= " << name() << std::endl; 
  return false;
}

bool uishWidget::motion_debug(const uishMouseEvent& event) {
  //std::cerr << "widget motion event at " << event.x << " " << event.y << " name= " << name() << std::endl; 
  return false;
}

bool uishWidget::uses_stencil() {
  return false;
}


void uishWidget::init()
{
  m_base = uishBase::current();

  render.connect(SigC::slot(*this, &uishWidget::trigger_render)); 

  realize.connect(SigC::slot(*this, &uishWidget::trigger_realize)); 

  button_press.connect(SigC::slot(*this, &uishWidget::button_debug)); 
  button_release.connect(SigC::slot(*this, &uishWidget::button_debug)); 
  motion.connect(SigC::slot(*this, &uishWidget::motion_debug)); 

  button_press.connect(SigC::slot(*this, &uishWidget::on_button_press)); 
  button_release.connect(SigC::slot(*this, &uishWidget::on_button_release)); 
  motion.connect(SigC::slot(*this, &uishWidget::on_motion)); 


  resize.connect(SigC::slot(*this, &uishWidget::resize_debug)); 
  resize.connect(SigC::slot(*this, &uishWidget::on_resize)); 
}

