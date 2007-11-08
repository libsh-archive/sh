#include <wx/wx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sh/sh.hpp>
#include "Go.hpp"
#include "RenderNode.hpp"
#include "ColorFinder.hpp"
#include "Trackball.hpp"

using namespace SH;
using namespace std;

namespace {

/* A relatively light Yellow-Red ramp for on-screen display */
ConstColor3f SCREEN_MIN(189/255.0, 0/255.0, 38/255.0);
ConstColor3f SCREEN_MID(253/255.0, 141/255.0, 60/255.0);
ConstColor3f SCREEN_MAX(255/255.0, 255/255.0, 178/255.0);

/* slightly darker version for paper display */ 
ConstColor3f PAPER_MIN(SCREEN_MIN * 0.8f); 
ConstColor3f PAPER_MID(SCREEN_MID * 0.8);
ConstColor3f PAPER_MAX(SCREEN_MAX * 0.8f);

Color3f GLOBAL_MIN;
}

Camera RenderNode::camera;
Matrix4x4f RenderNode::inv_mv;
Matrix4x4f RenderNode::mv;
Matrix4x4f RenderNode::mvd;
Point3f RenderNode::lightPos;
Color3f RenderNode::diffuseColor(0.75f, 0.75f, 0.75f);
Color3f RenderNode::lineColor(0.5f, 0.5f, 0.5f);
Color3f RenderNode::globalMinColor;
Color3f RenderNode::minColor;
Color3f RenderNode::midColor;
Color3f RenderNode::maxColor;
bool RenderNode::paperMode = false;
GoManVec* RenderNode::parentWindow = 0;
wxWindow* RenderNode::canvasWindow = 0;
bool RenderNode::twodRotation = false;

BEGIN_EVENT_TABLE(RenderNode, wxPanel)
    EVT_MOUSE_EVENTS(RenderNode::OnMouse)
END_EVENT_TABLE()

RenderNode::RenderNode()
  : wxPanel(parentWindow), m_sizer(new wxBoxSizer(wxVERTICAL)), cur_x(-1000000), cur_y(-1000000), m_changed(false) {
  if(!parentWindow) {
    cerr << "Error 59132623195: Please set the static RenderNode parent window!" << endl;
  }
  GLOBAL_MIN = ColorFinder::color(ColorFinder::Set2, 8, 2);
} 

void RenderNode::OnMouse(wxMouseEvent& event) {
  if(event.LeftDown()) { // click!
    parentWindow->select(this);
  }
}

bool RenderNode::OnMotion(const wxMouseEvent& event) {
  int x = event.GetX();
  int y = event.GetY();
  bool changed = false;
  if(cur_x != -1000000) {
    const double factor = 20.0;
    double dx = (x - cur_x)/factor;
    double dy = (cur_y - y)/factor;
    if(event.LeftIsDown()) {
      int width, height;
      canvasWindow->GetSize(&width, &height);
      Trackball trackball;
      trackball.resize(width, height);
      Matrix4x4f rot = trackball.rotate(cur_x, cur_y, x, y, twodRotation);
      Matrix4x4f mymv = mv | m;
      Point3f view_origin = mymv | ConstPoint3f(0,0,0);  
      Matrix4x4f recenter = translate(-view_origin);
      Matrix4x4f inv_recenter = translate(view_origin);
      m = inv_mv | inv_recenter | rot | recenter | mv | m;
      changed = true;
    }
    if (event.MiddleIsDown()) {
      float s = 1 + dx;
      m = m | scale(ConstAttrib3f(s, s, s)); 
      changed = true;
    }
    if (event.RightIsDown()) {
      m = inv_mv | translate(ConstVector3f(dx, dy, 0)) | mv | m;
      changed = true;
    }
  }
  m_changed |= changed;
  cur_x = x;
  cur_y = y;

  if((event.LeftUp() || event.MiddleUp() || event.RightUp()) && m_changed) { // propagate changes!
    for(size_t i = 0; i < m_dependents.size(); ++i) {
      m_dependents[i]->update();
    }
  }
  return changed;
}

void RenderNode::dependsOn(RenderNodePtr other) {
  other->m_dependents.push_back(this);
  m_parents.push_back(other);
}

void RenderNode::dependsOnNothing() {
  for(NodeVec::iterator N = m_parents.begin(); N != m_parents.end(); ++N) {
    NodeVec& Ndep = (*N)->m_dependents;
    NodeVec::iterator me = find(Ndep.begin(), Ndep.end(), this);
    assert(me != Ndep.end()); 
    Ndep.erase(me);
  }
  m_parents.clear();
}

void RenderNode::highlight(bool on) {
  if(on) {
    SetBackgroundColour(wxColour(255, 158, 9));
  } else {
    SetBackgroundColour(wxNullColour);
  }
}

void RenderNode::saveState(ostream& out) {
  float values[4];
  for(int i = 0; i < 4; ++i) {
    m[i].getValues(values);
    out.write(reinterpret_cast<char*>(values), sizeof(float) * 4);
  }
}
void RenderNode::readState(istream& in) {
  float values[4];
  for(int i = 0; i < 4; ++i) {
    in.read(reinterpret_cast<char*>(values), sizeof(float) * 4);
    m[i].setValues(values);
  }
}

ManRenderNode::ManRenderNode(const Man& man, size_t sampleRate)
  : m_man(man), m_init(false), m_sampleRate(sampleRate) {
  m_sizer->Add(new wxStaticText(this, wxID_ANY,
      wxString(man.name().c_str(), wxConvUTF8)),
      wxSizerFlags().Expand().Left());
}

void ManRenderNode::render() {
  if(!m_init) init();

  m_mv = mv | m;
  m_mvd = mvd | m;
  if(paperMode) {
    minColor = PAPER_MIN;
    midColor = PAPER_MID;
    maxColor = PAPER_MAX;
  } else {
    minColor = SCREEN_MIN;
    midColor = SCREEN_MID;
    maxColor = SCREEN_MAX;
  }
  globalMinColor = GLOBAL_MIN;

  bind(*m_shaders);
  glCallList(m_display_list);
  unbind(*m_shaders);
}

void ManRenderNode::init() {
  if(m_init) return;

  Color3f highlight = ColorFinder::color(ColorFinder::Set2, 8, 0);
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    InOutPosition4f SH_DECL(pos);
    InputTexCoord2f SH_DECL(texcoord);
    OutputNormal3f SH_DECL(normal);
    OutputVector3f SH_DECL(lightv);
    OutputAttrib1f SH_DECL(use_lighting); 
    OutputColor3f SH_DECL(color);

    use_lighting = 0.0f;
  /* set up the display list and evaluate geometry */
    Point3f SH_DECL(evalp);
    color = diffuseColor;
    switch(m_man.output_size()) {
      case 1: // function
        switch(m_man.input_size()) {
          case 0: eval_point(texcoord, evalp, normal); color = lineColor; break;
          case 1: eval_function(texcoord, evalp, normal); color = lineColor; break;
          case 2: eval_function2d(texcoord, evalp, normal); use_lighting = 1.0; break;
          default: assert(false); break;
        }
        break;
      case 2: // 2D point, curve (expanded to 3D)
      case 3: // 3D point, curve, surface
        switch(m_man.input_size()) {
          case 0: eval_point(texcoord, evalp, normal); color = lineColor; break;
          case 1: eval_curve(texcoord, evalp, normal); color = lineColor; break;
          case 2: eval_surface(texcoord, evalp, normal); use_lighting = 1.0; break;
          default: assert(false); break;
        }
        break;

      case 4: // 3D point, curve, surface, with coloring parameter
        color = ConstColor3f(0,0,0); 
        switch(m_man.input_size()) {
          case 1: eval_curve_color(texcoord, evalp, color, normal); break;
          case 2: eval_surface_color(texcoord, evalp, color, normal); use_lighting = 0.25; break;
          default: assert(false);
        }
        //color = lerp(color(0), highlight, diffuseColor);
        SH::Color3f loColor = lerp(color(0) * 2, midColor, minColor); 
        SH::Color3f hiColor = lerp(color(0) * 2 - 1.0f, maxColor, midColor); 
        SH::Color3f ratioColor = cond(color(0) < 0.5f, loColor, hiColor); 
        SH::Color3f resultColor = cond(color(0) < 1e-3, globalMinColor, ratioColor); 
        color = resultColor;
        break;
    };
    pos(0,1,2) = evalp;
    pos(3) = 1.0f;

    Point3f posv = (m_mv | pos)(0,1,2); // Compute viewspace position
    lightv = normalize(lightPos - posv); // Compute light direction
    
    pos = m_mvd | pos; // Project position
    ConstAttrib1f splits(2.0);
    normal = normalize(m_mv | normal); // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    InputPosition4f SH_DECL(pos);
    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(lightv);
    InputAttrib1f SH_DECL(use_lighting);
    InOutColor3f SH_DECL(color);

    normal = normalize(normal);
    lightv = normalize(lightv);
    normal = faceforward(lightv, normal);
    
    SH::Color3f lighted_color = (normal | lightv) * color; 
    color = lerp(use_lighting, lighted_color, color);
  } SH_END;
  //lock_in_uniforms(fsh);
  
  vsh.node()->dump("vsh");
#if 0
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif

  m_shaders = new ProgramSet(vsh, fsh);
  m_init = true;
}

void ManRenderNode::eval_point(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  drawPoint();
  Man m3 = m_man.fill(3);
  p = m3;
  n = ConstNormal3f(0,0,1);
}


/* Plots a curve at z=0 */ 
void ManRenderNode::eval_curve(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  std::cout << "drawing eval curve" << endl;
  drawCurve();
  Man m3 = m_man.fill(3);
  p = m3(texcoord(0)); 
  n = ConstNormal3f(0,0,1);
}

/* Plots a surfaces */
void ManRenderNode::eval_surface(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  drawSurface();
  Man m3 = m_man.fill(3);
  p = m3(texcoord);
  Man dm3 = m_normal3d(m3);
  n = dm3(texcoord); 
}

/* Plots a curve at z=0 */ 
void ManRenderNode::eval_curve_color(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& color, Generic<3, float>& n) {
  std::cout << "drawing eval curve" << endl;
  drawCurve();
  assert(m_man.output_size() == 4);
  Attrib<4, SH_TEMP> result;
  result = m_man(texcoord(0));
  p = result(0,1,2); 
  color = result(3,3,3);
  n = ConstNormal3f(0,0,1);
}

/* Plots a surfaces */
void ManRenderNode::eval_surface_color(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& color, Generic<3, float>& n) {
  drawSurface();
  assert(m_man.output_size() == 4);
  Attrib<4, SH_TEMP> result;
  result = m_man(texcoord);
  p = result(0,1,2); 
  color = result(3,3,3);
  Man dm3 = m_normal3d(m_man(0,1,2));
  n = dm3(texcoord); 
}

/* plots a function y = f(u), x = u*/ 
void ManRenderNode::eval_function(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  std::cout << "drawing eval function" << endl;
  drawCurve();
  p(0) = texcoord(0) - 0.5f;
  p(1) = m_man(texcoord(0));
  p(2) = 0.0f;
  n = ConstNormal3f(0,0,1);
}

void ManRenderNode::eval_function2d(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  std::cout << "drawing eval_function2d" << endl;
  m_sampleRate = 100;
  drawSurface();
  Man m3 = m_combine(m_u(0) - 0.5f, m_u(1) - 0.5f, m_man);
  p = m3(texcoord);
  Man dm3 = m_normal3d(m3);
  n = dm3(texcoord);
}

void ManRenderNode::drawPoint() {
  m_display_list = glGenLists(1);
  glPointSize(3.0);
  glNewList(m_display_list, GL_COMPILE);
  glBegin(GL_POINTS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0,0,0);
  glEnd();
  glEndList();
}

void ManRenderNode::drawCurve() {
  m_display_list = glGenLists(1);
  glNewList(m_display_list, GL_COMPILE);
  glBegin(GL_LINE_STRIP);
  size_t n = m_sampleRate == 0 ? DEFAULT_SAMPLERATE : m_sampleRate;
  float step = 1.0f / n;
  for(size_t i = 0; i <= n; ++i) {
    glTexCoord2f(i * step, 0.0f);
    glVertex3f(0,0,0);
  }
  glEnd();
  glEndList();
}

void ManRenderNode::drawSurface() {
  m_display_list = glGenLists(1);
  glNewList(m_display_list, GL_COMPILE);
  size_t n = m_sampleRate == 0 ? DEFAULT_SAMPLERATE : m_sampleRate;
  float step = 1.0f / n;
  for(size_t i = 0; i < n; ++i) {
    glBegin(GL_QUAD_STRIP);
    for(size_t j = 0; j <= n; ++j) {
      glTexCoord2f(i * step, j * step);
      glVertex3f(0,0,0);
    
      glTexCoord2f((i+1) * step, j * step);
      glVertex3f(0,0,0);
    }
    glEnd();
  }
  glEndList();
}

void HostManRenderNode::update() {
  m_hostman.update();
  canvasWindow->Refresh(false);
}
