/* modified from wx penguin example */  
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/splitter.h"


#include <map>
#include <vector>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sh/sh.hpp>
#include "Camera.hpp"
#include "Go.hpp"
#include "ColorFinder.hpp"
#include "man.hpp"


using namespace std;
using namespace SH;

ostream& operator<<(ostream & out, const wxString & str) {
  out << str.mb_str();
  return out;
} 

// Event IDs 
enum
{
    // our menu items
    ID_OPEN_SVG_CRV = 100, 
    ID_OPEN_SURFACE,


    ID_OP_PROFILE_PROD,
    ID_OP_WIRE_PROD,
    ID_OP_OFFSET_PROD,
    ID_OP_DISTANCE
};


/* Maps shapes to their display lists */
map<ProgramNodeCPtr, GLuint> display_list; 
vector<Curve3D> curves;
vector<Surface3D> surfaces;

template<int N>
void plotVert(const Generic<N, float>& p) {
  static float val[N];
  p.getValues(val);
  switch(N) {
    case 2: glVertex2fv(val); break;
    case 3: glVertex3fv(val); break;
    case 4: glVertex4fv(val); break;
  }
}

void plotNormal(const Generic<3, float>& n) {
  static float val[3];
  n.getValues(val);
  glNormal3fv(val); 
}

/* Plots a curve at z=0 */ 
void plot(const Curve3D& c, int n = 1000) {
  if(display_list.find(c.node()) == display_list.end()) {
    TessCurve3D tessc = m_uniform_tess(c, n);
    GLuint dl = glGenLists(1);
    display_list[c.node()] = dl;
    glNewList(dl, GL_COMPILE_AND_EXECUTE);
    glBegin(GL_LINE_STRIP);
    plotNormal(ConstNormal3f(0, 0, 1));
    for(int i = 0; i <= n; ++i) { 
        plotVert(tessc[i]);
    }
    glEnd();
    glEndList();
  } else {
    glCallList(display_list[c.node()]);
  }
}

Normal3f getNormal(const TessSurface3D &tess, int i, int j, int n) {
  return normalize(-cross(tess[i + 1][j] - tess[i][j], tess[i][j+1] - tess[i][j]));
}

Normal3f getAvgNormal(const TessSurface3D &tess, int i, int j, int n) {
  Normal3f result(0,0,0); 
  int count = 0;
  if(i < n && j < n) { result += getNormal(tess, i, j, n); count++; }
  if(i < n && j > 0) { result += getNormal(tess, i, j - 1, n); count++; }
  if(i > 0 && j > 0) { result += getNormal(tess, i - 1, j - 1, n); count++; }
  if(i > 0 && j < n) { result += getNormal(tess, i - 1, j, n); count++; }
  return result / static_cast<float>(count); 
}

/* Plots a surfaces */
void plot(const Surface3D& s, int n = 100) {
  if(display_list.find(s.node()) == display_list.end()) {
    TessSurface3D tess = m_uniform_tess(s, n);
    GLuint dl = glGenLists(1);
    display_list[s.node()] = dl;
    glNewList(dl, GL_COMPILE_AND_EXECUTE);
    glBegin(GL_QUADS);
    for(int i = 0; i < n; ++i) { 
      for(int j = 0; j < n; ++j) {
        plotNormal(getAvgNormal(tess, i, j, n));
        plotVert(tess[i][j]);
        plotNormal(getAvgNormal(tess, i+1, j, n));
        plotVert(tess[i+1][j]);
        plotNormal(getAvgNormal(tess, i+1, j+1, n));
        plotVert(tess[i+1][j+1]);
        plotNormal(getAvgNormal(tess, i, j+1, n));
        plotVert(tess[i][j+1]);
      }
    }
    glEnd();
    glEndList();
  } else {
    glCallList(display_list[s.node()]);
  }
}

// `Main program' equivalent, creating windows and returning main app frame
bool GoApp::OnInit()
{
    // Create the main frame window
    GoFrame *frame = new GoFrame(NULL, wxT("Global Optimization"),
        wxDefaultPosition, wxSize(1280, 1024));

    /* Show the frame */
    frame->Show(true);

    return true;
}

IMPLEMENT_APP(GoApp)

BEGIN_EVENT_TABLE(GoFrame, wxFrame)
  EVT_MENU      (wxID_EXIT,               GoFrame::OnExit)
  EVT_BUTTON    (ID_OPEN_SVG_CRV,         GoFrame::OnButton)
  EVT_BUTTON    (ID_OPEN_SURFACE,         GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_PROFILE_PROD,      GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_WIRE_PROD,         GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_OFFSET_PROD,       GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_DISTANCE,          GoFrame::OnButton)
END_EVENT_TABLE()

/* Go frame constructor */
GoFrame::GoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
#if 0
    /* Make a menubar */
    m_fileMenu = new wxMenu();

    m_fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    m_menuBar = new wxMenuBar();
    m_menuBar->Append(m_fileMenu, wxT("&File"));
    SetMenuBar(m_menuBar);
#endif

    m_split = new wxSplitterWindow(this);

    m_controlsPanel = new wxPanel(m_split, wxID_ANY); 
    m_controlsPanel->SetSizer(m_controlsSizer = new wxBoxSizer (wxVERTICAL));
      m_controlsSizer->Add(m_buttonSizer = new wxBoxSizer (wxVERTICAL),
                           0, wxEXPAND);
        m_buttonSizer->Add(m_openSvgBtn = new wxButton(m_controlsPanel, ID_OPEN_SVG_CRV, _T("Open SVG &Curve..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_buttonSizer->Add(m_openSurfaceBtn = new wxButton(m_controlsPanel, ID_OPEN_SURFACE, _T("Open &Bezier Surface..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_buttonSizer->AddSpacer(20);
        m_buttonSizer->Add(m_profileBtn = new wxButton(m_controlsPanel, ID_OP_PROFILE_PROD, _T("&Profile Product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_buttonSizer->Add(m_wireBtn = new wxButton(m_controlsPanel, ID_OP_WIRE_PROD, _T("&Wire Product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_buttonSizer->Add(m_offsetBtn = new wxButton(m_controlsPanel, ID_OP_OFFSET_PROD, _T("&Offset Product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_buttonSizer->Add(m_distanceBtn = new wxButton(m_controlsPanel, ID_OP_DISTANCE, _T("&Distance"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);

      m_controlsSizer->Add(m_stackWnd = new wxScrolledWindow(m_controlsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL));
    m_canvas = new GoGLCanvas(m_split, wxID_ANY, wxDefaultPosition,
        wxSize(768, 200), wxSUNKEN_BORDER);

    m_split->SplitVertically(m_controlsPanel, m_canvas, 200);
}

/* Handle button presses */
void GoFrame::OnButton( wxCommandEvent &event ) {
  switch(event.GetId()) {
    case ID_OPEN_SVG_CRV: {
      wxString svg_filename = wxFileSelector(_T("Open SVG curve"),_T(""),_T(""),_T(""),
                                     _T("SVG files (*.svg)|*.svg"), wxOPEN); 
      if(!svg_filename.empty()) {
        cout << svg_filename << endl;
      }
      break;
    }
    case ID_OPEN_SURFACE: {
      wxString bezier_filename = wxFileSelector(_T("Open BEZIER patch"),_T(""),_T(""),_T(""),
                                     _T("Bezier patch (*.bezier)|*.bezier"), wxOPEN); 
      if(!bezier_filename.empty()) {
        cout << bezier_filename << endl;
      }
      break;
    }
      break;
    case ID_OP_PROFILE_PROD:
      break;
    case ID_OP_WIRE_PROD:
      break;
    case ID_OP_OFFSET_PROD:
      break;
    case ID_OP_DISTANCE:
      break;
  };
}

/* Intercept menu commands */
void GoFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

BEGIN_EVENT_TABLE(GoGLCanvas, wxGLCanvas)
    EVT_SIZE(GoGLCanvas::OnSize)
    EVT_PAINT(GoGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(GoGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(GoGLCanvas::OnMouse)
    EVT_CHAR(GoGLCanvas::OnChar)
END_EVENT_TABLE()

GoGLCanvas::GoGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
      do_init(true)
{
}

GoGLCanvas::~GoGLCanvas()
{
}

void GoGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

    // Initialize OpenGL
    if (do_init)
    {
        InitGL();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bind(*shaders);

/*
    glFrontFace(GL_CW);
    glutSolidTeapot(2.5);
    glFrontFace(GL_CCW);
    */

    for(int i = 0; i < curves.size(); ++i) {
      plot(curves[i]);
    }
    for(int i = 0; i < surfaces.size(); ++i) {
      plot(surfaces[i]);
    }

    unbind(*shaders);

    glFlush();
  
    // Swap
    SwapBuffers();
}

void GoGLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if ( GetContext() )
#endif
    {
      SetCurrent();
      glViewport(0, 0, (GLint) w, (GLint) h);
      SetupView();
      Update();
    }
}

void GoGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    /* Do nothing, to avoid flashing on MSW */
}

void GoGLCanvas::OnMouse( wxMouseEvent& event )
{
  int x = event.GetX();
  int y = event.GetY();
  wxSize sz( GetClientSize() );

  const double factor = 20.0;
  bool changed = false;
  
  if (event.LeftIsDown()) {
    camera.orbit(cur_x, cur_y, x, y, sz.x, sz.y);
    changed = true;
  }
  if (event.MiddleIsDown()) {
    camera.move(0, 0, (y - cur_y)/factor);
    changed = true;
  }
  if (event.RightIsDown()) {
    camera.move((x - cur_x)/factor, (cur_y - y)/factor, 0);
    changed = true;
  }

  if (changed) {
    SetupView();
    Refresh(false);
  }

  cur_x = x;
  cur_y = y;
}

void GoGLCanvas::OnChar( wxKeyEvent& event)
{
   switch( event.GetKeyCode() ) {
     case 'q': case 'Q':
       break;
   }
}

void GoGLCanvas::InitGL()
{
    if(!do_init) return;
    do_init = false;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    SetupView();

    // Initial values for the uniforms
    camera.move(0.0, 0.0, -30.0);
    lightPos = Point3f(5.0, 5.0, 5.0);
    //diffusecolor = ColorFinder::color(ColorFinder::Set1, 5, 0); 
    diffusecolor = Color3f(0.75, 0.75, 0.75);

    try {
      InitShaders();
      bind(*shaders);
    }
    catch (const std::exception &e) {
      cerr << e.what() << endl;
    }

#if 0
    cout << "Vertex Unit:" << endl;
    vsh.node()->code()->print(cout);
    cout << "--" << endl;
    cout << "Fragment Unit:" << endl;
    fsh.node()->code()->print(cout);
    cout << "--" << endl;
#endif

}

void GoGLCanvas::SetupView()
{
  int w, h;
  GetClientSize(&w, &h);
  float aspect = static_cast<float>(w) / h;
  camera.glProjection(aspect);
  mv = camera.modelView();
  mvd = camera.modelViewProjection(Matrix4x4f());
}

void GoGLCanvas::InitShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    InOutPosition4f pos;
    InOutNormal3f normal;
    OutputVector3f lightv;

    Point3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    InputColor3f SH_DECL(kd); 
    InputPosition4f pos;
    InputNormal3f normal;
    InputVector3f lightv;

    OutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    normal = faceforward(lightv, normal);
    
    color = (normal | lightv) * kd; 
  } SH_END;
  fsh = fsh << diffusecolor;
  lock_in_uniforms(fsh);
  cout << fsh.describe_binding_spec() << endl;

  shaders = new ProgramSet(vsh, fsh);

  ConstAttrib1f SH_DECL(radius) = 1.0f;
  Curve2D circle = m_circle(mx_<1>(0) * (2 * M_PI)) * radius;
  circle.node()->dump("circle");

  Curve2D dcircle = m_differentiate(circle, 0);
  dcircle.node()->dump("dcircle");

  Curve2D svgCurve = m_svg_crv("foo.svg") * 0.01; 
  svgCurve.node()->dump("svgcurve");

  curves.push_back(circle);
  curves.push_back(svgCurve); 

  float twopi = 2 * M_PI;
  Surface3D sphere = m_sphere(mx_<2>(0) * twopi, mx_<2>(1) * M_PI) * radius;
  //surfaces.push_back(sphere);
 
  Curve2D pot_profile = m_svg_crv("profile.svg") * 0.01; 
  curves.push_back(pot_profile);

  Curve2D blob = m_svg_crv("blob.svg") * 0.01; 
  curves.push_back(blob);

  Curve2D wineglass_profile = m_svg_crv("wineglass_profile.svg") * 0.01; 
  curves.push_back(wineglass_profile);

  cout << "PReparing glass" << endl;
  Surface3D wineglass = m_profile(circle, wineglass_profile(mx_<2>(1)));
  surfaces.push_back(wineglass);
  wineglass.node()->dump("wineglass");
}

BEGIN_EVENT_TABLE(GoStack, wxScrolledWindow)
END_EVENT_TABLE()

GoStack::GoStack(wxWindow* parent, wxWindowID id,
           const wxPoint& pos, 
           const wxSize& size, 
           const wxString& name) 
  : wxScrolledWindow(parent, id, pos, size, wxVSCROLL, name) {
}

