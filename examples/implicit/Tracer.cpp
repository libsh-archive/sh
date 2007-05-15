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


#include <cassert>
#include <complex>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sh/sh.hpp>
#include <sh/Optimizations.hpp>
#include <shutil/shutil.hpp>
#include "Camera.hpp"
#include "ColorFinder.hpp"
#include "Timer.hpp"
#include "ScalarStack.hpp"
#include "Plot.hpp"
#include "Tracer.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil; 

#include "Bb.hpp"


string defaultBackend = "arb";
bool showTiming = true;
bool showAaDebug = false;

ConstColor3f TufteOrange = ConstColor3f(1.0f, 0.62f, .035f);

const char* DebugModeName[] = {
  "debug_start",
  "Normal",
  "Function Value",
  "Hit Distance",
  "Gradient",
  "Plot",
  "Trace",
  "Loop Count",
  "Loop Cutoff",
  "debug_end"
};

const char* RangeModeName[] = {
  "Affine Arithmetic",
  "Affine Arithmetic (Unique Condensation Off)",
  "Interval Arithmetic"
};



Timer timer;

void startTimer(string msg) {
  timer = Timer::now();
  if(showTiming) {
    std::cout << "Timer " << msg << " "; 
  }
}

void printElapsed(string msg) {
  double elapsed = (Timer::now() - timer).value() / 1000; 
  if(showTiming) {
    cout << " elapsed " << msg << " " << elapsed << " = " << (1.0 / elapsed) << "Hz" << endl;
  }
}

// `Main program' equivalent, creating windows and returning main app frame
bool TracerApp::OnInit()
{
    // Create the main frame window
    m_frame = new TracerFrame(NULL, wxT("Implicit Ray-Tracer"),
        wxDefaultPosition, wxSize(1280, 1024));


    /* Show the m_frame */
    m_frame->Show(true);
    return true;
}

IMPLEMENT_APP(TracerApp)

// IDs for the controls and the menu commands
enum
{
    // our menu items
    Preset_Spheres = 100,
    Preset_Torus,

    // standard menu items
    Preset_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Preset_About = wxID_ABOUT
};


BEGIN_EVENT_TABLE(TracerFrame, wxFrame)
    EVT_MENU(Preset_Spheres, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus, TracerFrame::OnPreset)
END_EVENT_TABLE()

/* Tracer frame constructor */
TracerFrame::TracerFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
    initParams();

    /* Make a menubar */
    m_presetMenu = new wxMenu;

    m_presetMenu->Append(Preset_Spheres, wxT("&Spheres"));
    m_presetMenu->Append(Preset_Torus, wxT("&Torus"));
    m_menuBar = new wxMenuBar;
    m_menuBar->Append(m_presetMenu, wxT("&Preset"));
    SetMenuBar(m_menuBar);

    m_split = new wxSplitterWindow(this); 
    m_rightSplit = new wxSplitterWindow(m_split); 
    for(int i = 0; i < numCanvas; ++i) {
      DebugMode debugMode = NORMAL;
      switch(i) {
        case 0: debugMode = NORMAL; break;
        case 1: debugMode = LOOP; break;
        case 2: debugMode = TRACE; break;
      }
      wxSize size = i == 0 ? wxSize(768, 1024) : wxSize(512, 512); 
      m_canvas[i] = new TracerGLCanvas(debugMode, i == 0 ? m_split : m_rightSplit, 
          *this, wxID_ANY, wxDefaultPosition,
          size, /*wxSUNKEN_BORDER*/ wxNO_BORDER);
    }
    m_split->SplitVertically(m_canvas[0], m_rightSplit); 
    m_rightSplit->SplitHorizontally(m_canvas[1], m_canvas[2]);

    //this->SetCanvas(m_canvas);
}

void TracerFrame::initParams() {
  selected = 0;
  ShowTorus();

  // set up names
  for(int i = 0; i < 9; ++i) {
    ostringstream out;
    out << "trans_" << (i+1);
    trans[i].name(out.str());

    ostringstream out2;
    out2 << "scl_" << (i+1);
    scl[i].name(out2.str());
    scl[i] = ConstAttrib3f(1,1,1);

    ostringstream out3;
    out2 << "fscl_" << (i+1);
    fscl[i].name(out3.str());
    fscl[i] = ConstAttrib1f(1);
  }

  /* set up colors */
  for(int i = 0; i < 9; ++i) {
    color[i] = ColorFinder::color(ColorFinder::Set1, 9, i); 
  }

  level.name("level");
  level = 1.0;

  eps.name("epsilon");
  eps = 1e-5;

  traceEps.name("traceEpsilon");
  traceEps = 1e-5;

  traceDiff = 0.0f;
  traceDiff.name("traceDiff");

  loop_cutoff.name("loop_cutoff");
  loop_cutoff = 64.0f;

  loop_highlight.name("loop_highlight");
  loop_highlight = 0.0f;

  eyeposm.name("eyeposm");

  sphereRadius = 8.0;
  graphScale = sphereRadius * 2; 
  graphOffset = ConstVector2f(0.0, 0.0);

  dbgCoords = Attrib2f(0.0, 0.0);

  camera.move(0.0, 0.0, -15.0);
  lightPos = Point3f(5.0, 5.0, 5.0);
  lightPos.name("lightPos");

  showTiming = true;
  showAaDebug = false;

  rangeMode = AA;
}

/* Intercept menu commands */
void TracerFrame::OnPreset( wxCommandEvent& event )
{
  ClearModels();
  switch(event.GetId()) {
    case Preset_Spheres: ShowSpheres(); break;
    case Preset_Torus: ShowTorus(); break;
    default:
      cerr << "Unknown preset" << event.GetId() << endl; 
  }
  InitShaders();
}

void TracerFrame::MouseAll(wxMouseEvent& event, const wxSize& sz) 
{
  int x = event.GetX();
  int y = event.GetY();

  const double factor = 20.0;
  bool changed = false;
  bool viewchanged = false;
  double dx = (x - cur_x)/factor;
  double dy = (cur_y - y)/factor;
  double dz = (y - cur_y)/factor;
  
  if (event.LeftIsDown()) {
    if(event.ShiftDown()) {
      if(selected != 0) 
        scl[selected-1](0) += dx;
    } else if (event.ControlDown()) {
      if(selected != 0) 
        fscl[selected-1] += dx;
    } else {
      camera.orbit(cur_x, cur_y, x, y, sz.x, sz.y);
      viewchanged = true;
    }
    changed = true;
  }
  if (event.MiddleIsDown()) {
    if(selected == 0) {
      camera.move(0, 0, dz); 
      viewchanged = true;
    } else {
      if(event.ShiftDown()) {
        scl[selected-1](1) += dx;
      } else {
        trans[selected-1](2) += dz;
      }
    }
    changed = true;
  }
  if (event.RightIsDown()) {
    if(selected == 0) {
      camera.move(dx, dy, 0);
      viewchanged = true;
    } else {
      if(event.ShiftDown()) {
        scl[selected-1](2) += dx;
      } else {
        trans[selected-1](0) += dx;
        trans[selected-1](1) += dy;
      }
    }
    changed = true;
  }

  if(viewchanged) {
    for(int i = 0; i < numCanvas; ++i) m_canvas[i]->SetupView();
  }

  if (changed) {
    RefreshAll();
  }

  cur_x = x;
  cur_y = y;
}


void TracerFrame::RefreshAll() {
  for(int i = 0; i < numCanvas; ++i) {
    m_canvas[i]->GrabContext();
    for(int j = 0; j < 9; ++j) { /* stupid hack to update uniforms */
      if(pfs[j]) {
        trans[j] = trans[j];
        scl[j] = scl[j];
        fscl[j] = fscl[j];
        color[j] = color[j];
      }
    }
    graphScale = graphScale;
    graphOffset = graphOffset;
    sphereRadius = sphereRadius;
    level = level;
    traceDiff = traceDiff;
    eps = eps;
    traceEps = traceEps;
    loop_cutoff = loop_cutoff;
    loop_highlight = loop_highlight;
    dbgCoords = dbgCoords;
    m_canvas[i]->SetupView();
    m_canvas[i]->Refresh(false);
  }
}

void TracerFrame::ClearModels() {
  for(int i = 0; i < numCanvas; ++i) {
    m_canvas[i]->GrabContext();
    for(int j = 0; j < 9; ++j) {
      trans[j] = ConstAttrib3f(0, 0, 0);
      scl[j] = ConstAttrib3f(1, 1, 1);
      fscl[j] = ConstAttrib1f(1);
      pfs[j] = 0; 
    }
    pfs[0] = new GaussianPF(new Dist2SpherePF(color[0]));
  }
}

void TracerFrame::ShowSpheres() {
  float side = 1.25;
  pfs[0] = new GaussianPF(new Dist2SpherePF(color[0]));
  trans[0] = Vector3f(-side, -side * 0.5 * sqrt(3), 0);
  pfs[1] = new GaussianPF(new Dist2SpherePF(color[1]));
  trans[1] = Vector3f(side, -side * 0.5 * sqrt(3), 0);
  pfs[2] = new GaussianPF(new Dist2SpherePF(color[2]));
  trans[2] = Vector3f(0, side * 0.5 * sqrt(3), 0);
}

void TracerFrame::ShowTorus() {
  float side = 1.69;
  pfs[0] = new GaussianPF(new Dist2TorusPF(color[0]));
  /*
  trans[0] = Vector3f(-side, -side * 0.5 * sqrt(3), 0);
  pfs[1] = new GaussianPF(new Dist2TorusPF(color[1]));
  trans[1] = Vector3f(side, -side * 0.5 * sqrt(3), 0);
  pfs[2] = new GaussianPF(new Dist2TorusPF(color[2]));
  trans[2] = Vector3f(0, side * 0.5 * sqrt(3), 0);
  */
}

void TracerFrame::InitShaders() {
  for(int i = 0; i < numCanvas; ++i) {
    m_canvas[i]->InitShaders();
    m_canvas[i]->Refresh(false);
  }
}


void TracerFrame::OnCharAll(wxKeyEvent& event) 
{
  long k = event.GetKeyCode();
  bool ctrlDown = event.ControlDown();
  if('0' <= k && k <= '9') {
    selected = k - '0'; 
    cout << "Selected ";
    if(selected) {
      cout << (selected - 1);
    } else {
      cout << "None";
    }
    cout << endl;
    return;
  }

  bool reinit = false;
  switch(k)
    {
    case 'q':
    case 'Q':
      exit(0);
      break;

    case 'k': sphereRadius -= 0.1f; break;
    case 'K': sphereRadius += 0.1f; break;

    case 'v': level -= 0.1f; break;
    case 'V': level += 0.1f; break;

/*
    case 'd': traceDiff -= 0.1f; break;
    case 'D': traceDiff += 0.1f; break;
    */

    case 'e': eps *= 0.5f; break;
    case 'E': eps *= 2.0f; break;

    case 'f': traceEps *= 0.5f; break;
    case 'F': traceEps *= 2.0f; break;


    case 'l': loop_cutoff -= 1; 
      if(loop_cutoff.getValue(0) < 0) loop_cutoff = 0.0f;
      if(loop_highlight.getValue(0) > loop_cutoff.getValue(0)) loop_highlight = loop_cutoff;
      break;
    case 'L': loop_cutoff += 1; break;

    case 'h': 
      loop_highlight -= 1; 
      if(loop_highlight.getValue(0) < 0) loop_highlight = 0.0f;
      break;
    case 'H': loop_highlight += 1; 
      if(loop_highlight.getValue(0) > loop_cutoff.getValue(0)) loop_highlight = loop_cutoff;
      break;

    case 'g': graphScale /= 1.1; break;
    case 'G': graphScale *= 1.1; break;

    case 's': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new Dist2SpherePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'c': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new Dist2CylinderPF(color[idx])); 
       reinit = true;
     }
     break;

    case 't': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new Dist2TorusPF(color[idx])); 
       //pfs[idx] = new Dist2TorusPF(color[idx]); 
       reinit = true;
     }
     break;

    case 'p': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new DistPlanePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'd':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new DropPF(color[idx])); 
       reinit = true;
     }
     break;

    case 'i': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new MitchellPF(color[idx]); 
       reinit = true;
     }
     break;
#if 0
     if(selected > 0) {
       int idx = selected - 1;
       // some constants taken from shrike;
       Attrib1f maxlvl=5;
       Attrib1f julia_max = 2.0;
       Attrib2f c(0.54,-0.51);
       pfs[idx] = new JuliaPF(maxlvl, julia_max, c, color[idx]);
       reinit = true;
     }
#endif

/*
    case 'h':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GaussianPF(new HeightFieldPF("heightfield.png", color[idx]));
       reinit = true;
     }
     break;
     */

    case 'n':
     if(selected > 0) {
       pfs[selected - 1] = 0;
       reinit = true;
     }
     break;

    case 'y': // resets view
     camera.reset();
     camera.move(0.0, 0.0, -15.0);
     for(int i = 0; i < numCanvas; ++i) m_canvas[i]->SetupView();
     break;

    
    case 'z':
     if(rangeMode == AA) rangeMode = IA;
     else rangeMode = static_cast<RangeMode>(rangeMode - 1);
     reinit = true;
     break;

    case 'Z':
     if(rangeMode == IA) rangeMode = AA;
     else rangeMode = static_cast<RangeMode>(rangeMode + 1);
     reinit = true;
     break;

    case 'x':
     showTiming = !showTiming;
     break;

    case 'w':
      showAaDebug = !showAaDebug;
      Context::current()->set_flag("aa_disable_debug", !showAaDebug);
      break;

    case WXK_LEFT:  dbgCoords(0) -= 0.01; break;
    case WXK_RIGHT: dbgCoords(0) += 0.01; break;
    case WXK_UP:    dbgCoords(1) += ctrlDown ? 0.001 : 0.01; break;
    case WXK_DOWN:  dbgCoords(1) -= ctrlDown ? 0.001 : 0.01; break;

    case '.':
      for(int i = 0; i < 9; ++i) {
        if(pfs[i]) {
          cout << "i=" << i << endl;
          cout << "  scl=" << scl[i] << endl;
          cout << "  fscl=" << fscl[i] << endl;
          cout << "  tran=" << trans[i] << endl;
        }
      }
      break;

    default:
     // print out help
     cout << "Available Interaction:" << endl;
     cout << endl; 
     cout << "  q             Quit" << endl;
     cout << "  m/M           Change rendering mode" << endl;
     cout << "  z/Z           Change range arithmetic mode" << endl;
     cout << "  k/K           Change bounding sphere radius" << endl;
     cout << "  v/V           Change value of level set to intersect" << endl; 
     cout << "  e/E           Change epsilon (determines when to end tracing iterations)" << endl;
     cout << "  f/F           Change trace epsilon (determines how close to level set value roots must be)" << endl; 
     cout << "  l/L           Change loop count loop_cutoff in Loop Cutoff mode" << endl; 
     cout << "  g/G           Change graph y scale in Trace mode" << endl; 
     cout << "  w             Show debug" << endl;
     cout << "  x             Show timings" << endl;
     cout << "  up/down/left/right  Move trace point" << endl;
     cout << "  y             Reset view" << endl;
     cout << "  0             Change to view mode (deselect object)" << endl;
     cout << "  1-9           Change to object manipulation mode (plus select an object)" << endl;
     cout << "  Left Button   Rotate" << endl;
     cout << endl;
     cout << "View Mode" << endl;
     cout << "  Middle Button Translate x/y" << endl;
     cout << "  Right Button  Translate z" << endl;
     cout << endl;
     cout << "Object Manipulation Mode" << endl;
     cout << "  s             Make object a sphere (point generator)" << endl;
     cout << "  c             Make object a cylinder (line generator)" << endl;
     cout << "  t             Make object a torus (circle generator)" << endl;
     cout << "  d             Make object a drop" << endl;
     cout << "  i             Make Mitchell function" << endl;
     cout << "  p             Make object a plane (plane generator)" << endl;
     //cout << "  h             Make object a texture mapped heightfield (SLOW)" << endl;
     //cout << "  a             Make object a julia fractal (SLOW!!)" << endl;
     cout << "  n             Make object empty" << endl;
     cout << "  Middle Button Translate object x/y" << endl;
     cout << "  Right Button  Translate object z" << endl;
     cout << "  Shift-Left/Mddle/Right Button    Change object x/y/z scaling" << endl;
     cout << "  Ctrl-Left Button Change function scaling" << endl;
  }


  if(reinit) {
    InitShaders();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        cout << "PF " << i << ": " << pfs[i]->name() << endl; 
      }
    }
  }
  cout << "Sphere Radius: " << sphereRadius.getValue(0) << endl;
  cout << "Level: " << level.getValue(0) << endl;
  cout << "Graph scale: " << graphScale << endl; 
  cout << "Epsilon : " << eps.getValue(0) << " Trace Epsilon: " << traceEps.getValue(0) << endl;
  cout << "Trace Extra : " << traceDiff.getValue(0) << endl;
  cout << "Loop Cutoff : " << loop_cutoff.getValue(0) << endl;
  cout << "Loop Highlight: " << loop_highlight.getValue(0) << endl;
  cout << "Arithmetic Type : " << RangeModeName[rangeMode] << endl; 

  RefreshAll();
}

/* This is called once on each canvas, since uniform state is separate,
 * even though we are re-using the same variables */
void TracerGLCanvas::SetupView() {
  GrabContext();
  int w, h;
  GetClientSize(&w, &h);
  float aspect = static_cast<float>(w) / h;
  glPushMatrix();
    m_tframe.camera.glProjection(aspect);
  glPopMatrix();
  m_tframe.mv = m_tframe.camera.modelView();
  m_tframe.inv_mv = inverse(m_tframe.mv);
  m_tframe.eyeposm = m_tframe.inv_mv | ConstPoint3f(0.0f, 0.0f, 0.0f); 
  m_tframe.mvd = m_tframe.camera.modelViewProjection(Matrix4x4f());
}

BEGIN_EVENT_TABLE(TracerGLCanvas, wxGLCanvas)
    EVT_SIZE(TracerGLCanvas::OnSize)
    EVT_PAINT(TracerGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TracerGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(TracerGLCanvas::OnMouse)
    EVT_CHAR(TracerGLCanvas::OnChar)
END_EVENT_TABLE()

TracerGLCanvas::TracerGLCanvas(DebugMode mode, wxWindow* parent, TracerFrame& tframe, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
      debugMode(mode), do_init(true), m_tframe(tframe),
      m_context(Context::createContext())
{
  //m_context->disable_optimization("propagation");
  //m_context->optimization(1);
}

TracerGLCanvas::~TracerGLCanvas()
{
}

void TracerGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    GrabContext();

    // Initialize OpenGL
    if (do_init)
    {
        InitGL();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    startTimer("Rendering");
    cout << "Debug mode=" << DebugModeName[debugMode] << endl;
    if(debugMode == TRACE || debugMode == PLOT) {
      glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 0.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
      glEnd();
    } else {
      GLUquadric* sph;
      sph = gluNewQuadric();
      gluSphere(sph, m_tframe.sphereRadius.getValue(0), 32, 32);
    }

    glFinish();
  
    // Swap
    SwapBuffers();
    printElapsed("Done Render");
}

void TracerGLCanvas::OnSize(wxSizeEvent& event)
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
      GrabContext();
      glViewport(0, 0, (GLint) w, (GLint) h);
      SetupView();
      Update();
    }
}

void TracerGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    /* Do nothing, to avoid flashing on MSW */
}

void TracerGLCanvas::OnMouse( wxMouseEvent& event ) {
  wxSize sz( GetClientSize() );
  if(debugMode == PLOT) {
    const double factor = 100.0;
    const double sfactor = 100.0;
    bool changed = false;
    int x = event.GetX();
    int y = event.GetY();
    
    if (event.LeftIsDown()) {
      m_tframe.graphOffset(0) -= (x - cur_x) / factor * m_tframe.graphScale;
      m_tframe.graphOffset(1) += (y - cur_y) / factor * m_tframe.graphScale;
      changed = true;
    }
    if (event.MiddleIsDown()) {
      m_tframe.graphScale *= 1.0 + (y - cur_y) / sfactor; 
      changed = true;
    }
    if(changed) {
      m_tframe.RefreshAll();
    }
    cur_x = x;
    cur_y = y;
  } else {
    m_tframe.MouseAll(event, sz);
  }
}

void TracerGLCanvas::OnChar(wxKeyEvent& event) {
  m_tframe.OnCharAll(event);
  long k = event.GetKeyCode();
  bool newMode = false;
  switch(k) {
    case 'm': 
     debugMode = static_cast<DebugMode>(debugMode - 1); 
     if(debugMode == DEBUG_START) {
       debugMode = static_cast<DebugMode>(DEBUG_END - 1);
     }
     newMode = true;
     break;

    case 'M': 
     debugMode = static_cast<DebugMode>(debugMode + 1); 
     if(debugMode == DEBUG_END) {
       debugMode = NORMAL;
     }
     newMode = true;
     break;
    case '\\': {
       Program foo(fsh.node()->clone());
       placeAaSyms(foo.node(), true);
       foo.node()->dump("aa_fsh_syms"); 
       handleAaOps(foo.node());
       foo.node()->dump("aa_fsh_full"); 
       break;
    }
  }
  if(newMode) { /* update shaders, just for this view */
    InitShaders(); 
    cout << "Initializing Debug Mode: " << DebugModeName[debugMode] << endl; 
  }
}


void TracerGLCanvas::InitGL()
{
    Context::setContext(m_context);
    if(!do_init) return;
    do_init = false;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glEnable(GL_CULL_FACE);
    // stupid hack - we only need one surface of the sphere anyway, so let's
    // enable front face culling (more chance we'll get part of the sphere, even if viewpoint is inside */ 
    //glCullFace(GL_FRONT);
    glCullFace(GL_BACK);

    glClearColor(0.25, 0.25, 0.25, 1.0);

    try {
      InitShaders();
    }
    catch (const std::exception &e) {
      cerr << e.what() << endl;
    }

}


void TracerGLCanvas::InitShaders()
{
  try {
    GrabContext();
    bool use_aa = (m_tframe.rangeMode != IA);

    startTimer("Building Shaders");

    if(debugMode == TRACE || debugMode == PLOT) {
      Matrix4x4f id;
      vsh = KernelLib::vsh(id, id);
      vsh = vsh << extract("lightPos") << ConstAttrib3f(0, 0, 0);
    } else {
      vsh = KernelLib::vsh(m_tframe.mv, m_tframe.mvd);
      vsh = vsh << extract("lightPos") << m_tframe.lightPos;
    }

    printElapsed("Build vsh");

    Color3f SH_DECL(ks) = Color3f(0.3, 0.3, 0.3);
    Attrib1f SH_DECL(specexp) = 64.0f;

    ConstColor3f lowColor = ColorFinder::reverseColor(ColorFinder::YlGnBu, 3, 0); 
    ConstColor3f highColor= ColorFinder::reverseColor(ColorFinder::YlGnBu, 3, 2); 

    /* Set flags */
    switch(m_tframe.rangeMode) {
      case AA: Context::current()->set_flag("aa_enable_um", true); break; 
      case AA_NOUC: Context::current()->set_flag("aa_enable_um", false); break; 
      case IA: Context::current()->set_flag("aa_enable_um", true); break; 
    }

    // Build the plot function 
    SumPFPtr plotfunc = new SumPF(m_tframe.level);
    for(int i = 0; i < 9; ++i) {
      if(m_tframe.pfs[i]) {
        plotfunc->add(new FuncScalePF(new TranslatePF(new ScalePF(m_tframe.pfs[i], m_tframe.scl[i]), m_tframe.trans[i]), m_tframe.fscl[i]));
      }
    }

    Program func = plotfunc->funcProgram("posm");
    Program gradient = plotfunc->gradProgram("posm");
    Program colorFunc = plotfunc->colorProgram("posm");

    printElapsed("Build plot functions (function, gradient, color)");

    Color3f SH_DECL(lightColor) = ConstAttrib3f(0.75, .75, .75);

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      InputPoint3f SH_DECL(posm);
      InputVector3f SH_DECL(viewVec);
      InputTexCoord2f SH_DECL(texcoord);
      InputPosition4f SH_DECL(posh);


      Vector3f SH_DECL(dir) = posm - m_tframe.eyeposm;
      Attrib1f SH_DECL(dist) = length(dir);
      dir = normalize(dir);

      Attrib1f SH_DECL(theta) = m_tframe.dbgCoords(0);
      Attrib1f SH_DECL(phi) = m_tframe.dbgCoords(1);
      Point3f SH_DECL(debugPosm);
      debugPosm(0) = cos(theta) * sin(phi);
      debugPosm(1) = sin(theta) * sin(phi);
      debugPosm(2) = cos(phi);
      debugPosm *= m_tframe.sphereRadius;
      //Vector3f SH_DECL(debugDir) = normalize(debugPosm - m_tframe.eyeposm);
      //Attrib1f inDbg = (normalize(dir) | debugDir) > 0.99999; 
      Attrib1f inDbg = length(debugPosm - posm) < m_tframe.sphereRadius / 200.0; 
      if(debugMode == TRACE || debugMode == PLOT) {
        posm = debugPosm;
        dir = posm - m_tframe.eyeposm;
        dist = length(dir);
        dir = normalize(dir);
      } 

      cout << "Building tracer" << endl;
      Program tracer = trace(func)(m_tframe.eyeposm, dir);
      tracer = add<Attrib1f>() << -m_tframe.level << tracer;
      if(showAaDebug) {
        tracer.node()->dump("tracer");
      }
      Program foo = affine_inclusion_syms(tracer);
      placeAaSyms(foo.node(), true);
      if(showAaDebug) {
        foo.node()->dump("aa_tracer");
      }

      cout << "Building firsthit" << endl;
      Program hitter = firsthit(tracer, use_aa, debugMode, m_tframe, texcoord);

      Attrib1f SH_DECL(trace_start); 
      Attrib1f SH_DECL(trace_end) = dist + 2.0  * (dir | -posm) + m_tframe.traceDiff;

      // handle if we're inside the sphere
      trace_start = SH::min(dist, trace_end);
      trace_end = SH::max(dist, trace_end);

      Attrib1i_f SH_DECL(range) = make_interval(trace_start, trace_end);  // @todo figure out where it exits the sphere
      Attrib1f SH_DECL(hashit);
      Attrib1f SH_DECL(hit);
      if(debugMode != FUNC) {
        (hashit & hit) = hitter(range);
      }


      if(debugMode == NORMAL) {
        kill(!hashit && !inDbg);
        Point3f SH_DECL(hitp) = mad(hit, dir, m_tframe.eyeposm);

        OutputNormal3f SH_DECL(normal);
        OutputVector3f SH_DECL(lightVec);
        OutputVector3f SH_DECL(halfVec);
        normal = gradient(hitp);
        normal = m_tframe.mv | normal;
        normal = normalize(normal);
        lightVec = m_tframe.mv | (m_tframe.lightPos - hitp); 
        lightVec = normalize(lightVec);
        viewVec = normalize(viewVec);
        halfVec = normalize(lightVec + viewVec);

        ConstAttrib1f ONE(1.0f);
        normal = faceforward(lightVec, normal); 

        OutputColor3f SH_DECL(kd) = colorFunc(hitp);
        kd = lerp(inDbg, TufteOrange, kd);
        normal = lerp(inDbg, lightVec, normal);
      } else {
        OutputColor3f SH_DECL(color);
        switch(debugMode) {
          case FUNC:
            {
              Attrib1f SH_DECL(le) = length(m_tframe.eyeposm);
              Attrib1f SH_DECL(t) = le * le / (dir | -m_tframe.eyeposm);
              Attrib1f SH_DECL(funcval);
              Point3f SH_DECL(pos) = mad(t, dir, m_tframe.eyeposm);
              funcval = tracer(t);
              color = colorFunc(pos); 
              color *= funcval;
            }
            break;
          case GRAD:
            {
              kill(!hashit && !inDbg);
              Point3f SH_DECL(hitp) = mad(hit, dir, m_tframe.eyeposm);
              color = gradient(hitp);
              color = normalize(color);
            }
            break;
          case HIT:
            color(0) = hashit;  // hit or not
            color(1) = hashit * (hit - dist) / abs(trace_end); // distance to hit
            break;

          case PLOT:
            color = plot(tracer, texcoord, make_interval(hashit, hit), m_tframe.graphScale, m_tframe.graphOffset); 
            break;

          case LOOP:
          case CUTOFF:
            {
              color = lerp(hashit, lerp(hit, lowColor, highColor), ConstColor3f(0.5, 0.5, 0.5));
            }
            break;

          default:
            break;
        }
        color = lerp(inDbg, TufteOrange, color);
      }

    } SH_END;
    fsh.name("fsh");
    std::cout << "OptimizationLevel: " << Context::current()->optimization() << std::endl;

    if(debugMode == NORMAL) {
      Program light = KernelLight::pointLight<Color3f>() << lightColor; 

      // kd comes from fsh, pass in the rest of the phong params 
      Program lightNShade = KernelSurface::phong<Color3f>() << swizzle("ks", "specExp", "irrad", "kd", "normal", "halfVec", "lightVec", "posh") << ks << specexp << light;
      lightNShade.name("lightnshade");
      if(showAaDebug) {
        dump(lightNShade, "lightnshade");
      }

      fsh = namedConnect(fsh, lightNShade); 
    }

    switch(m_tframe.rangeMode) {
      case AA: fsh.name("fsh_aa"); break;
      case AA_NOUC: fsh.name("fsh_aa_nouc"); break;
      case IA: fsh.name("fsh_ia"); break;
    }
    printElapsed("Build fsh");

    vsh = namedAlign(vsh, fsh);
    vsh.name("vsh");

    if(showAaDebug) {
      dump(fsh, fsh.name());
      dump(vsh, "vsh");
    }

    startTimer("Binding Shaders");
    lock_in_uniforms(vsh);
    lock_in_uniforms(fsh);
    shaders = new ProgramSet(vsh, fsh);
    bind(*shaders);
    printElapsed("Bind vsh, fsh");
  } catch(const Exception &e) {
    cout << "Error: " << e.message() << endl;
  } catch(...) {
    cout << "Caught unknown error: " << endl;
  }
}

void TracerGLCanvas::GrabContext() {
  SetCurrent();
  Context::setContext(m_context);
}

