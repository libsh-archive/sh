#include "uishContainer.hpp"

uishContainer::uishContainer() 
{ }

uishContainer::uishContainer(double width, double height) 
  : uishWidget(width, height)
{ 
  std::cerr << "container width = " << m_width_request << " height = " << m_height_request << std::endl;
}

uishContainer::~uishContainer()
{ }

void uishContainer::addChild(uishWidgetPtr child)
{
  m_children.push_back(child);
  child->setParent(this);
}

void uishContainer::on_realize()
{
  for(iterator I = begin(); I != end(); ++I) {
    (*I)->realize();
  }
}
