/* modified from wx penguin example */  
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/splitter.h>


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
#include "RenderNode.hpp"


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
    
    // generator 
    ID_OPEN_SVG_CRV = 100, 
    ID_OPEN_SURFACE,
    ID_POINT,
    ID_LINE,
    ID_CIRCLE,
    ID_SIN,
    ID_PLANE,

    ID_OP_PROFILE_PROD,
    ID_OP_WIRE_PROD,
    ID_OP_ADD,
    ID_OP_MUL,
    ID_OP_DISTANCE,
    ID_OP_PLOTDIST, /* only valid for 2 curves */

    ID_STACKNODE_START = 1337 /* start of stack node ids */
};



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
  EVT_BUTTON    (ID_POINT,                 GoFrame::OnButton)
  EVT_BUTTON    (ID_LINE,                 GoFrame::OnButton)
  EVT_BUTTON    (ID_PLANE,                GoFrame::OnButton)
  EVT_BUTTON    (ID_CIRCLE,               GoFrame::OnButton)
  EVT_BUTTON    (ID_SIN,               GoFrame::OnButton)

  EVT_BUTTON    (ID_OP_PROFILE_PROD,      GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_WIRE_PROD,         GoFrame::OnButton)

  EVT_BUTTON    (ID_OP_ADD,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_MUL,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_DISTANCE,          GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_PLOTDIST,          GoFrame::OnButton)
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
      m_controlsSizer->Add(
        m_generatorBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("Generators")), wxVERTICAL ), 0, wxEXPAND);
        m_generatorBox->Add(m_openSvgBtn = new wxButton(m_controlsPanel, ID_OPEN_SVG_CRV, _T("&Open Curve..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_openSurfaceBtn = new wxButton(m_controlsPanel, ID_OPEN_SURFACE, _T("Open &Surface..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_pointBtn= new wxButton(m_controlsPanel, ID_POINT, _T("&Point"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_lineBtn= new wxButton(m_controlsPanel, ID_LINE, _T("&Line"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_planeBtn= new wxButton(m_controlsPanel, ID_PLANE, _T("Plane"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_circleBtn= new wxButton(m_controlsPanel, ID_CIRCLE, _T("&Circle"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_sinBtn= new wxButton(m_controlsPanel, ID_SIN, _T("&sine"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);

      m_controlsSizer->Add(
        m_curveopBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("2D Curve Tools")), wxVERTICAL ), 0, wxEXPAND);
        m_curveopBox->Add(m_profileBtn = new wxButton(m_controlsPanel, ID_OP_PROFILE_PROD, _T("&Profile Product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                             0, wxEXPAND);
        m_curveopBox->Add(m_wireBtn = new wxButton(m_controlsPanel, ID_OP_WIRE_PROD, _T("&Wire Product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                             0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);

      m_controlsSizer->Add(
        m_funcopBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("Operators")), wxVERTICAL ), 0, wxEXPAND);
        m_funcopBox->Add(m_addBtn = new wxButton(m_controlsPanel, ID_OP_ADD, _T("&+"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_funcopBox->Add(m_mulBtn = new wxButton(m_controlsPanel, ID_OP_MUL, _T("&*"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_funcopBox->Add(m_distanceBtn = new wxButton(m_controlsPanel, ID_OP_DISTANCE, _T("&Distance"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_funcopBox->Add(m_plotdistBtn = new wxButton(m_controlsPanel, ID_OP_PLOTDIST, _T("Plot Distance Function"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);

      m_controlsSizer->AddSpacer(30);

      m_controlsSizer->Add(m_stack = new GoManVec(this, m_controlsPanel), wxSizerFlags().Expand().Proportion(1));
      RenderNode::parentWindow = m_stack;
    m_canvas = new GoGLCanvas(this, m_split, wxID_ANY, wxDefaultPosition,
        wxSize(768, 200), wxSUNKEN_BORDER);

    m_split->SplitVertically(m_controlsPanel, m_canvas, 200);
}

/* Handle button presses */
void GoFrame::OnButton( wxCommandEvent &event ) {
  std::cout << event.GetId() <<endl;
  switch(event.GetId()) {
    case ID_OPEN_SVG_CRV: {
      wxString svg_filename = wxFileSelector(_T("Open SVG curve"),_T(""),_T(""),_T(""),
                                     _T("SVG files (*.svg)|*.svg"), wxOPEN); 
      if(!svg_filename.empty()) {
        Man curve = m_svg_crv(static_cast<const char*>(svg_filename.mb_str())) * 0.01; 
        curve.name(svg_filename.AfterLast('/').mb_str());
        m_stack->push(new ManRenderNode(curve));
      } 
      break;
    }
    case ID_OPEN_SURFACE: {
      wxString bezier_filename = wxFileSelector(_T("Open BEZIER patch"),_T(""),_T(""),_T(""),
                                     _T("Bezier patch (*.bezier)|*.bezier"), wxOPEN); 
      if(!bezier_filename.empty()) {
        Surface3D surface = m_read_tea_surface(static_cast<const char*>(bezier_filename.mb_str()));
        surface.name(bezier_filename.AfterLast('/').mb_str());
        m_stack->push(new ManRenderNode(surface));
      }
      break;
    }

    case ID_POINT: {
      Point2D p = m_(ConstPoint2f(0.0f, 0.0f)); 
      m_stack->push(new ManRenderNode(p));
      break;
    }

    case ID_LINE: {
      Curve2D line = m_combine(m_u(0), m_(0.0f));
      m_stack->push(new ManRenderNode(line));
      break;
    }

    case ID_PLANE: {
      Surface3D plane = m_combine(m_u(0), m_u(1), m_(0.0f));
      m_stack->push(new ManRenderNode(plane));
      break;
    }

    case ID_CIRCLE: {
      Attrib1f SH_DECL(TWOPI) = 2 * M_PI;
      Curve2D circle = m_circle(m_u(0) * (TWOPI));
      m_stack->push(new ManRenderNode(circle));
      break;
    }
    case ID_SIN: {
      Attrib1f SH_DECL(TWOPI) = 2 * M_PI;
      Man sinu0 = sin(m_u(0) * (4 * TWOPI));
      m_stack->push(new ManRenderNode(sinu0));
      break;
    }

    case ID_OP_PROFILE_PROD: {
      ProfileRenderNode* prn = new ProfileRenderNode(m_stack->node(-2), m_stack->node(-1));
      prn->man().node()->dump("profile");
      m_stack->pop(2);
      m_stack->push(prn);
      break;
    }
    case ID_OP_WIRE_PROD: {
      WireRenderNode* wrn = new WireRenderNode(m_stack->node(-2), m_stack->node(-1));
      wrn->man().node()->dump("wire");
      m_stack->pop(2);
      m_stack->push(wrn);
      break;
    }
    case ID_OP_ADD: {
      ManRenderNodePtr a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-2));   
      ManRenderNodePtr b = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));
      m_stack->pop(2);
      m_stack->push(new ManRenderNode(a->man() +b->man()));
      break;
    }
    case ID_OP_MUL: {
      ManRenderNodePtr a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-2));   
      ManRenderNodePtr b = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));
      m_stack->pop(2);
      m_stack->push(new ManRenderNode(a->man() * b->man()));
      break;
    }
    case ID_OP_DISTANCE: 
    case ID_OP_PLOTDIST: {
      cout << "woo!" << endl;
      ManRenderNodePtr a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-2));   
      ManRenderNodePtr b = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));
      Man am = a->transformed_man();
      am.node()->dump("am");
      Man bm = b->transformed_man();
      am.node()->dump("bm");
      int A = am.input_size();
      int B = bm.input_size();
      int N = std::max(am.output_size(), bm.output_size());
      am = am.fill(N);
      bm = bm.fill(N);
      if(event.GetId() == ID_OP_PLOTDIST) {
        assert(A == 1 && B == 1);
        Man diffab = am(m_u(0)) - bm(m_u(1)); 
        Man dist = sqrt(dot(diffab, diffab));
        dist.node()->dump("plotdist");
        m_stack->push(new ManRenderNode(dist));
        break;
      } 
      HostMan dist; 

      switch(A) {
        case 0:
          switch(B) {
            case 0: assert(false); break;
            case 1: switch(N) { case 2: dist = nearest_points<0, 1, 2>(am, bm); break; case 3: dist = nearest_points<0, 1, 3>(am, bm); break; } break;
            case 2: switch(N) { case 2: dist = nearest_points<0, 2, 2>(am, bm); break; case 3: dist = nearest_points<0, 2, 3>(am, bm); break; } break;
          }
          break;
        case 1:
          switch(B) {
            case 0: switch(N) { case 2: dist = nearest_points<1, 0, 2>(am, bm); break; case 3: dist = nearest_points<1, 0, 3>(am, bm); break; } break;
            case 1: switch(N) { case 2: dist = nearest_points<1, 1, 2>(am, bm); break; case 3: dist = nearest_points<1, 1, 3>(am, bm); break; } break;
            case 2: switch(N) { case 2: dist = nearest_points<1, 2, 2>(am, bm); break; case 3: dist = nearest_points<1, 2, 3>(am, bm); break; } break;
          } break;
          break;
        case 2:
          switch(B) {
            case 0: switch(N) { case 2: dist = nearest_points<2, 0, 2>(am, bm); break; case 3: dist = nearest_points<2, 0, 3>(am, bm); break; } break;
            case 1: switch(N) { case 2: dist = nearest_points<2, 1, 2>(am, bm); break; case 3: dist = nearest_points<2, 1, 3>(am, bm); break; } break;
            case 2: switch(N) { case 2: dist = nearest_points<2, 2, 2>(am, bm); break; case 3: dist = nearest_points<2, 2, 3>(am, bm); break; } break;
          }
          break;
      }
      m_stack->push(new HostManRenderNode(dist));
      break;
    }
  }
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

GoGLCanvas::GoGLCanvas(GoFrame* frame, wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
      do_init(true),
      m_frame(frame)
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
    m_frame->stack()->render();


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

  if(event.ShiftDown()) { /* pass along to active object on GoManVec */
    if(!m_frame->stack()->empty()) {
      changed |= m_frame->stack()->node(-1)->OnMotion(event);
    }
  } else {
    if (event.LeftIsDown()) {
      RenderNode::camera.orbit(cur_x, cur_y, x, y, sz.x, sz.y);
      changed = true;
    }
    if (event.MiddleIsDown()) {
      RenderNode::camera.move(0, 0, (y - cur_y)/factor);
      changed = true;
    }
    if (event.RightIsDown()) {
      RenderNode::camera.move((x - cur_x)/factor, (cur_y - y)/factor, 0);
      changed = true;
    }
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
      case WXK_DELETE:
       m_frame->stack()->pop();
       break;
    }
}

void GoGLCanvas::InitGL()
{
    if(!do_init) return;
    do_init = false;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    //setBackend("glsl");
    setBackend("arb");
    SetupView();
    //Context::current()->set_flag("aa_disable_debug", false);

    // Initial values for the uniforms
    RenderNode::RenderNode::camera.move(0.0, 0.0, -30.0);
    RenderNode::lightPos = Point3f(5.0, 5.0, 5.0);
    //diffusecolor = ColorFinder::color(ColorFinder::Set1, 5, 0); 
    RenderNode::diffuseColor = Color3f(0.75, 0.75, 0.75);

#if 0
    cout << "Vertex Unit:" << endl;
    vsh.node()->code()->print(cout);
    cout << "--" << endl;
    cout << "Fragment Unit:" << endl;
    fsh.node()->code()->print(cout);
    cout << "--" << endl;
#endif

  Man plane = m_combine(m_u(0), m_u(1), m_(0.0f));
  Man foo = m_normal3d(plane);
}

void GoGLCanvas::SetupView()
{
  int w, h;
  GetClientSize(&w, &h);
  float aspect = static_cast<float>(w) / h;
  RenderNode::camera.glProjection(aspect);
  RenderNode::mv = RenderNode::camera.modelView();
  RenderNode::mvd = RenderNode::camera.modelViewProjection(Matrix4x4f());
}

BEGIN_EVENT_TABLE(GoManVec, wxPanel)
END_EVENT_TABLE()

GoManVec::GoManVec(GoFrame* frame, wxWindow* parent, wxWindowID id,
           const wxPoint& pos, 
           const wxSize& size, 
           const wxString& name) 
  : wxPanel(parent, id, pos, size, wxSUNKEN_BORDER, name), m_frame(frame),
    m_sizer(new wxBoxSizer(wxVERTICAL)) {
  SetSizer(m_sizer);
}

void GoManVec::push(RenderNodePtr node) {
   cout << "Adding" << endl;
   m_frame->Refresh(false);
   m_stack.push_back(node);
   m_sizer->Add(node.object(), wxSizerFlags().Expand());
   m_sizer->Layout();
   cout << "sizer children: " << m_sizer->GetChildren().GetCount() << endl;
}

void GoManVec::pop(int count) {
  for(int i = 0; i < count; ++i) {
    m_sizer->Remove(m_stack.back().object());
    m_stack.pop_back();
  }
  m_sizer->Layout();
  m_frame->Refresh(false);
  cout << "sizer children: " << m_sizer->GetChildren().GetCount() << endl;
}

void GoManVec::render() {
  for(Stack::iterator I = m_stack.begin(); I != m_stack.end(); ++I) {
    (*I)->render();
  }
}
