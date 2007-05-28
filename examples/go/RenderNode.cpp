#include <wx/wx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sh/sh.hpp>
#include "RenderNode.hpp"

using namespace SH;
using namespace std;

Camera RenderNode::camera;
Matrix4x4f RenderNode::mv;
Matrix4x4f RenderNode::mvd;
Point3f RenderNode::lightPos;
Color3f RenderNode::diffuseColor(0.75f, 0.75f, 0.75f);
wxWindow* RenderNode::parentWindow = 0;

RenderNode::RenderNode()
  : wxPanel(parentWindow), m_sizer(new wxBoxSizer(wxVERTICAL)), cur_x(-1000000), cur_y(-1000000) {
  if(!parentWindow) {
    cerr << "Error 59132623195: Please set the static RenderNode parent window!" << endl;
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
    if (event.MiddleIsDown()) {
      float s = 1 + dx;
      m = m | scale(ConstAttrib3f(s, s, s)); 
      changed = true;
    }
    if (event.RightIsDown()) {
      m = translate(ConstAttrib3f(dx, dy, 0)) | m;
      changed = true;
    }
  }
  cur_x = x;
  cur_y = y;
  return changed;
}

void RenderNode::highlight(bool on) {
  if(on) {
    SetBackgroundColour(wxColour(255, 158, 9));
  } else {
    SetBackgroundColour(wxNullColour);
  }
}

ManRenderNode::ManRenderNode(const Man& man)
  : m_man(man), m_init(false) {
  m_sizer->Add(new wxStaticText(this, wxID_ANY,
      wxString(man.name().c_str(), wxConvUTF8)),
      wxSizerFlags().Expand().Left());
}

void ManRenderNode::render() {
  if(!m_init) init();

  bind(*m_shaders);
  glCallList(m_display_list);
  unbind(*m_shaders);
}

void ManRenderNode::init() {
  if(m_init) return;

  vsh = SH_BEGIN_VERTEX_PROGRAM {
    InOutPosition4f SH_DECL(pos);
    InputTexCoord2f SH_DECL(texcoord);
    OutputNormal3f SH_DECL(normal);
    OutputVector3f SH_DECL(lightv);
    OutputColor3f SH_DECL(color);

  /* set up the display list and evaluate geometry */
    Point3f evalp;
    switch(m_man.output_size()) {
      case 1: // function
        switch(m_man.input_size()) {
          case 1: eval_function(texcoord, evalp, normal); break;
          case 2: eval_function2d(texcoord, evalp, normal); break;
          default: assert(false); break;
        }
        break;
      case 2: // 2D point, curve
        switch(m_man.input_size()) {
          case 0: eval_point(texcoord, evalp, normal); break;
          case 1: eval_curve(texcoord, evalp, normal); break;
          default: assert(false); break;
        }
        break;
      case 3: // 3D point, curve, surface
        switch(m_man.input_size()) {
          case 0: eval_point(texcoord, evalp, normal); break;
          case 1: eval_curve(texcoord, evalp, normal); break;
          case 2: eval_surface(texcoord, evalp, normal); break;
          default: assert(false);
        }
        break;
    };
    pos(0,1,2) = evalp;
    pos(3) = 1.0f;

    Matrix4x4f my_mv = mv | m;
    Matrix4x4f my_mvd = mvd | m;
    Point3f posv = (my_mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = my_mvd | pos; // Project position
    color = normal;
    normal = my_mv | normal; // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    InputColor3f SH_DECL(kd); 
    InputPosition4f SH_DECL(pos);
    InputNormal3f SH_DECL(normal);
    InputVector3f SH_DECL(lightv);
    InOutColor3f SH_DECL(color);

    normal = normalize(normal);
    lightv = normalize(lightv);
    normal = faceforward(lightv, normal);
    
    color = (normal | lightv) * kd; 
  } SH_END;
  fsh = fsh << diffuseColor;
  lock_in_uniforms(fsh);
  
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

/* plots a function y = f(u), x = u*/ 
void ManRenderNode::eval_function(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  std::cout << "drawing eval function" << endl;
  drawCurve();
  p(0) = texcoord(0);
  p(1) = m_man(texcoord(0));
  p(2) = 0.0f;
  n = ConstNormal3f(0,0,1);
}

void ManRenderNode::eval_function2d(const TexCoord2f& texcoord, Generic<3, float>& p, Generic<3, float>& n) {
  std::cout << "drawing eval_function2d" << endl;
  drawSurface();
  Man m3 = m_combine(m_u(0), m_u(1), m_man);
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

void ManRenderNode::drawCurve(int n) {
  m_display_list = glGenLists(1);
  glNewList(m_display_list, GL_COMPILE);
  glBegin(GL_LINE_STRIP);
  float step = 1.0f / n;
  for(int i = 0; i <= n; ++i) {
    glTexCoord2f(i * step, 0.0f);
    glVertex3f(0,0,0);
  }
  glEnd();
  glEndList();
}

void ManRenderNode::drawSurface(int n) {
  m_display_list = glGenLists(1);
  glNewList(m_display_list, GL_COMPILE);
  float step = 1.0f / n;
  for(int i = 0; i < n; ++i) {
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j <= n; ++j) {
      glTexCoord2f(i * step, j * step);
      glVertex3f(0,0,0);
    
      glTexCoord2f((i+1) * step, j * step);
      glVertex3f(0,0,0);
    }
    glEnd();
  }
  glEndList();
}
