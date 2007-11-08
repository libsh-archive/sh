#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "uishBase.hpp"
#include "uishGraphLayout.hpp"

using namespace SH;

uishGraphLayout::uishGraphLayout()
  : m_scale(1),
    m_offset(0, 0)
{}

uishGraphLayout::uishGraphLayout(double width, double height)
  : uishContainer(width, height),
    m_scale(1),
    m_offset(0, 0)
{ }

uishGraphLayout::~uishGraphLayout()
{ }

void uishGraphLayout::addChild(uishWidgetPtr child) {
  uishContainer::addChild(child);
}

double uishGraphLayout::width_request() {
  doLayout();
  ShAttrib4d bb = bounding_box();
  return (bb(1) - bb(0)).getValue(0);
}

double uishGraphLayout::height_request() {
  doLayout();
  ShAttrib4d bb = bounding_box();
  return (bb(3) - bb(2)).getValue(0);
}

ShAttrib4d uishGraphLayout::bounding_box() {
  doLayout();
  ShAttrib4d bb(0.0, 0.0, 0.0, 0.0);
  if(m_children.empty()) return bb;

  OffsetVec::iterator O = m_childOffsets.begin(); 
  for(iterator I = begin(); I != end(); ++I, ++O) {
    double minx, maxx, miny, maxy;
    minx = (*O).getValue(0);
    miny = (*O).getValue(1);
    maxx = minx + (*I)->width_request();
    maxy = miny + (*I)->height_request();

    if(I == begin()) {
      bb = ShConstAttrib4d(minx, maxx, miny, maxy);
    } else {
      bb(0) = min(bb(0), minx);
      bb(1) = max(bb(1), maxx);
      bb(2) = min(bb(2), miny);
      bb(3) = min(bb(3), maxy);
    }
  }
  return bb;
}

void uishGraphLayout::addEdge(uishWidgetPtr from, uishWidgetPtr to) {
  size_t fromIdx, toIdx, cur;
  cur = 0;
  for(iterator I = begin(); I != end(); ++I, ++cur) {
    if(*I == from) {
      fromIdx = cur; 
    }
    if(*I == to) {
      toIdx = cur;
    }
  }
  m_edges.push_back(Edge(fromIdx, toIdx));
}

bool uishGraphLayout::uses_stencil() {
  return true;
}

void uishGraphLayout::on_render()
{
//  std::cerr << "rendering" << std::endl;
  // do the layout for nodes that have not been done
  doLayout();
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  // use stencil test to restrict children to this' box 
  glEnable(GL_STENCIL_TEST);

  // @todo this won't work for nested graphs 
  glStencilFunc(GL_ALWAYS,0,1);
  glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, height());
    glVertex2f(width(), height());
    glVertex2f(width(),0); 
  glEnd();

  // find out what stencil value we are at currently 
  int stencilValue = 1;
  for(uishContainerPtr ancestor = parent(); ancestor; ancestor = ancestor->parent()) {
    if(ancestor->uses_stencil()) {
      stencilValue++;
    }
  }

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
  glStencilFunc(GL_EQUAL, stencilValue, 0xFFFFFFFF);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glPushMatrix();
    glTranslatef(m_offset.getValue(0), m_offset.getValue(1), 0);
    glScalef(m_scale, m_scale, 1);

    // draw edges
    glBegin(GL_LINES);
    glColor4f(0,0,0,1);
    for(EdgeList::iterator E = m_edges.begin(); E != m_edges.end(); ++E) {
      size_t from = E->first;
      size_t to = E->second;
      ShVector2f fromOffset = m_childOffsets[from] + m_children[from]->size() * 0.5;
      ShVector2f toOffset = m_childOffsets[to] + m_children[to]->size() * 0.5;
      
      glVertex2f(fromOffset.getValue(0), fromOffset.getValue(1));
      glVertex2f(toOffset.getValue(0), toOffset.getValue(1));
    }
    glEnd();

    // draw nodes
    OffsetVec::iterator O = m_childOffsets.begin();
    for(iterator I = begin(); I != end(); ++I, ++O) {
      glPushMatrix();
        glTranslatef((*O).getValue(0), (*O).getValue(1), 0);
        (*I)->render();
      glPopMatrix();
    }
  glPopMatrix();

  glStencilOp(GL_KEEP, GL_DECR, GL_DECR); 
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, height());
    glVertex2f(width(), height());
    glVertex2f(width(),0); 
  glEnd();

  glPopAttrib();
}


/* find the child where this is in */
bool uishGraphLayout::on_button_press(const uishButtonEvent& event) {

  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(child && child->button_press(offsetEvent)) {
    return true;
  }
  press = last = ShConstPoint2d(event.x, event.y);
  return true;
}

bool uishGraphLayout::on_button_release(const uishButtonEvent& event) {
  uishButtonEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(child && child->button_release(offsetEvent)) {
    return true;
  }
  return false; 
}

bool uishGraphLayout::on_motion(const uishMouseEvent& event) {
  //std::cerr << "motion in graph layout " << name() << std::endl;

  uishMouseEvent offsetEvent = event;
  uishWidgetPtr child = getChild(event.x, event.y, offsetEvent); 
  if(child && child->motion(offsetEvent)) {
    return true;
  } 
  ShPoint2f eventPos(event.x, event.y);
  ShVector2f delta = eventPos - last; 
  float mul = 5.0;
  float scale = 100.0f;
  last = eventPos; 

  /* scale around mouse position 
   * new offset = old offset*/
  if(event.state & GDK_BUTTON2_MASK) {
    float multiplier = 1.0f + mul * delta.getValue(1) / scale;

    // find mouse position 
    ShPoint2f origin = (press - m_offset)  / m_scale;

    m_scale *= multiplier; 
    m_offset = press - origin * m_scale; 
  }

  if(event.state & GDK_BUTTON1_MASK) {
    m_offset += delta; 
  }
  m_base->invalidate();
  return true;
}

uishWidgetPtr uishGraphLayout::getChild(double x, double y, uishMouseEvent& offsetEvent)
{
  reverse_iterator I = m_children.rbegin();
  OffsetVec::reverse_iterator O = m_childOffsets.rbegin();
  SH_DEBUG_ASSERT(m_children.size() == m_offsets.size());

  x -= m_offset.getValue(0);
  y -= m_offset.getValue(1);
  x /= m_scale;
  y /= m_scale;

  for(;I != m_children.rend(); ++I, ++O) {
    float off[2];
    (*O).getValues(off);

    double size[2];
    (*I)->size().getValues(size);

    if(off[0] < x && x < off[0] + size[0] &&
       off[1] < y && y < off[1] + size[1]) {
      offsetEvent.x = x - off[0];
      offsetEvent.y = y - off[1]; 
      return *I;
    }
  }
  return 0;
}

void uishGraphLayout::on_resize(const uishResizeEvent& event)
{
  //std::cerr << "resize" << std::endl;
  m_width = event.width;
  m_height = event.height;

  for(iterator I = begin(); I != end(); ++I) {
    (*I)->resize(uishResizeEvent((*I)->width_request(), (*I)->height_request()));
  }
}


// DOT layout copied over from GrLayout of shrike-graph branch
void uishGraphLayout::doLayout() {
  SH_DEBUG_ASSERT(m_childOffsets.size() <= m_children.size());
  if(m_childOffsets.size() == m_children.size()) {
    return;
  }
  char filename[] = {'/','t','m','p','/','u','i','s','h','_','X','X','X','X','X','X',0};
  if (!mktemp(filename)) {
    std::cerr << "Could not make temp file name" << std::endl;
    return;
  }

  std::ofstream out(filename);

  out << "digraph uish {" << std::endl;
  out << "rankdir=LR;" << std::endl;
  //out << "nodesep=20;" << std::endl;
  out << "ordering=out;" << std::endl;
  out << "center=1;" << std::endl;
  out << "edge [minlen=20];" << std::endl;
  OffsetVec::iterator O = m_childOffsets.begin();
  for (size_t i = 0; i < m_children.size(); ++i) {
    out << "n" << i << " [height=" << m_children[i]->height()
        << ",width=" << m_children[i]->width() << ",fixedsize=true,shape=box,";
    if(O != m_childOffsets.end()) {
      out << ",pos=" << O->getValue(0) << "," << O->getValue(1) << "!"; 
      ++O;
    }
    out << "label=\"" << i << "\"];" << std::endl;
  }

  for (EdgeList::iterator E = m_edges.begin(); E != m_edges.end(); ++E) {
    out << "n" << E->first << " -> n" << E->second <<";" << std::endl;
  }

  out << "}" << std::endl;

  out.close();

  char resfilename[] = {'/','t','m','p','/','u','i','s','h','_','X','X','X','X','X','X',0};
  if (!mktemp(resfilename)) {
    std::cerr << "Could not make temp file name for output" << std::endl;
    return;
  }

  
  std::string cmdline;

  cmdline = "dot -Tplain ";
  cmdline += filename;
  cmdline += " > ";
  cmdline += resfilename;
  
  system(cmdline.c_str());

  std::ifstream in(resfilename);

  m_childOffsets.resize(m_children.size());
  while (!in.eof()) {
    std::string type;
    in >> std::ws >> type;
    if (in.eof()) break;
    if (type != "node") { while (!in.eof() && in.get() != '\n'); continue; }

    std::string name;
    double x, y, width, height;

    in >> std::ws >> name;
    in >> std::ws >> x;
    in >> std::ws >> y;
    in >> std::ws >> width;
    in >> std::ws >> height;
    int index = -1;
    in >> std::ws >> index;

    while (!in.eof() && in.get() != '\n');

    if (index < 0) continue;

    //std::cerr << index << ": " << m_children[index]->name() << " at " << x << ", " << y << std::endl;

    m_childOffsets[index] = 
      ShVector2f(x - m_children[index]->width() / 2.0,
                 y - m_children[index]->height() / 2.0);

  }

//  unlink(filename);
//  unlink(resfilename);
}
