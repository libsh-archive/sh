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

#define DISTFUNC GaussianPF
//#define DISTFUNC InvPF 


#include <cstdlib>
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
bool showDot = false;
bool testMode = false; /* complete test of all cases */
bool testMode2 = false; /* test case specified from command line, run numTestFrames, then quit */
bool showGradient = true;
bool screenCap = false;
string caseName;
int twistCount = 0;
int bumpCount = 0;
int objCount = 0;
int testRun = 0;
int testCase = 0;
int testRange = AAUC;
int testFrame = 0;
DebugMode mainDebugMode = NORMAL; 
float testFpsTotal = 0;
const int numTestFrames = 4;
const int numRuns = 3;
const int testSize = 512;

map<string, float> savedLoopCount; /* used to compute giga-instructions per second, only on the AAUC case */     

vector<string> split(const string& str, char delim) {
  vector<string> result;
  size_t s, e;
  size_t len = str.length();
  for(s = 0; s < len; s = e) {
    for(;s < len && str[s] == delim; ++s);
    if(s == len) break;
    for(e = s + 1; e < len && str[e] != delim; ++e); 
    result.push_back(str.substr(s, e - s));
  }
  return result;
}

struct ProgramStats {
  string prefix;
  int temps, scalar_temps;
  int instr_count, scalar_instr_count;
  int live, scalar_live;
  float avg_live, avg_scalar_live;
  ProgramStats(string prefix=""): prefix(prefix), temps(-1), scalar_temps(-1), instr_count(-1), scalar_instr_count(-1),
    live(-1), scalar_live(-1), avg_live(-1), avg_scalar_live(-1) {}

  void update() {
      instr_count = Context::current()->get_stat((defaultBackend + "_instr_count").c_str());
      scalar_instr_count = Context::current()->get_stat((defaultBackend + "_scalar_instr_count").c_str());
      temps = Context::current()->get_stat((defaultBackend + "_num_temps").c_str());
      scalar_temps = Context::current()->get_stat((defaultBackend + "_num_scalar_temps").c_str());
      live = Context::current()->get_stat((defaultBackend + "_num_live").c_str());
      scalar_live = Context::current()->get_stat((defaultBackend + "_num_scalar_live").c_str());
      avg_live = Context::current()->get_stat((defaultBackend + "_avg_num_live").c_str());
      avg_scalar_live = Context::current()->get_stat((defaultBackend + "_avg_num_scalar_live").c_str());
  }

  string header() {
    ostringstream sout;
    sout << "," << prefix << "instr_count," << prefix << "scalar_instr_count," << prefix << "temps," << prefix << "scalar_temps," << prefix << "live," << prefix << "scalar_live," << prefix 
         << "avg_live," << prefix << "avg_scalar_live";
    return sout.str();
  }

  string csv() {
    ostringstream sout;
    sout << "," <<instr_count<< "," <<scalar_instr_count<< "," <<temps<< "," <<scalar_temps<< "," <<live<< "," <<scalar_live<< "," <<avg_live<< "," <<avg_scalar_live;
    return sout.str();
  }
};
ProgramStats tracer_stats("tracer");
ProgramStats firsthit_stats("firsthit");
ProgramStats fsh_stats;


ConstColor3f TufteOrange = ConstColor3f(1.0f, 0.62f, .035f);
string timingCase; 
string arithmeticName;

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

const char* RangeModeShortName[] = {
  "AAUC",
  "AA",
  "IA"
};

ostream& operator<<(ostream & out, const wxString & str) {
  out << str.mb_str();
  return out;
} 



Timer timer;

void startTimer(string msg) {
  timer = Timer::now();
}

double printElapsed(string msg) {
  double elapsed = (Timer::now() - timer).value() / 1000; 
  if(showTiming) {
    cout << "timing:" << msg << "," << timingCase << "," << arithmeticName << "," << elapsed << "," << (1.0 / elapsed) << endl;
    cout.flush();
  }
  return elapsed;
}
double getElapsed() {
  return (Timer::now() - timer).value() / 1000; 
}

// IDs for the controls and the menu commands
enum
{
    // our menu items
    Preset_Sphere1 = 100,
    Preset_Sphere2,
    Preset_Sphere3,
    Preset_Sphere4,
    Preset_Sphere5,
    Preset_Sphere6,
    Preset_Sphere7,
    Preset_Sphere8,
    Preset_Line1,
    Preset_Line2,
    Preset_Line3,
    Preset_Line4,
    Preset_Line5,
    Preset_Line6,
    Preset_Line7,
    Preset_Line8,
    Preset_Torus1,
    Preset_Torus2,
    Preset_Torus3,
    Preset_Torus4,
    Preset_Torus5,
    Preset_Torus6,
    Preset_Torus7,
    Preset_Torus8,
    Preset_H2O,

    // standard menu items
    Preset_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Preset_About = wxID_ABOUT
};

// `Main program' equivalent, creating windows and returning main app frame
bool TracerApp::OnInit()
{

  bool singleCanvas = false;
  bool noBackground = false;
  wxSize framesize(1280, 768); 

  /*  read in saved loop count */
  char buf[1000];
  ifstream fin("loop.csv");
  for(;fin.getline(buf, 1000);) {
    if(fin.eof()) break;
    vector<string> toks = split(buf, ',');
    string name = toks[0].substr(5, toks[0].length() - 9); /* ignore loop_ prefix, .png suffix */
    float avg_count;
    sscanf(toks[1].c_str(), "%f", &avg_count);
    savedLoopCount[name] = avg_count;
    cerr << name << "=" << avg_count << endl; 
  }

  /* Handle cmd line args */
  for(int i = 0; i < argc; ++i) {
    string foo = static_cast<const char*>(wxString(argv[i]).mb_str());
    if(foo == "-single") {
      framesize = wxSize(testSize, testSize);
      singleCanvas = true;
    }
    if(foo == "-white")  noBackground = true;
    if(foo == "-test") {
      testMode2 = true; 
      framesize = wxSize(testSize, testSize);
      singleCanvas = noBackground = true;
    }
    if(foo == "-cap") {
      screenCap = true; 
      framesize = wxSize(testSize, testSize);
      singleCanvas = noBackground = true;
    }
    if(foo == "-nograd") {
      showGradient = false;
    }
    if(foo.substr(0,7) == "-sphere") caseName = foo.substr(1); 
    if(foo.substr(0,6)  == "-torus") caseName = foo.substr(1); 
    if(foo == "-debug") {
      showAaDebug = true;
      cerr << "debug on!" << endl;
    }
    if(foo == "-loop") {
      mainDebugMode = LOOP;
    }
  }

    // Create the main frame window
    m_frame = new TracerFrame(NULL, wxT("shimpl"),
        wxDefaultPosition, framesize, wxDEFAULT_FRAME_STYLE, singleCanvas, noBackground);

  if(caseName != "") {
    int count = caseName[caseName.length() - 1] - '1'; 
    int preset = (caseName[0] == 's' ? Preset_Sphere1 : Preset_Torus1) + count;
    wxCommandEvent evt(0, preset);
    m_frame->OnPreset(evt);
  }

  for(int i = 0; i < argc; ++i) {
    string foo = static_cast<const char*>(wxString(argv[i]).mb_str());
    if(foo == "-twistx" || foo == "-twisty" || foo == "-twistz") {
      int axis = foo == "-twistx" ? 0 : foo == "-twisty" ? 1 : 2;
      for(int i = 0; i < 9; ++i) {
        if(m_frame->pfs[i]) m_frame->pfs[i] = new TwistPF(m_frame->pfs[i], axis, 1);
      }
      caseName += foo; 
      twistCount++;
    }
    if(foo == "-bumpx" || foo == "-bumpy" || foo == "-bumpz") {
      int axis = foo == "-bumpx" ? 0 : foo == "-bumpy" ? 1 : 2;
      for(int i = 0; i < 9; ++i) {
        if(m_frame->pfs[i]) m_frame->pfs[i] = new SinPF(m_frame->pfs[i], axis, 0.2, 4);
      }
      caseName += foo; 
      bumpCount++;
    }
  }
  if(mainDebugMode == LOOP) {
    caseName = "loop_" + caseName;
  }

    /* Show the m_frame */
    m_frame->Show(true);
    return true;
}

IMPLEMENT_APP(TracerApp)



BEGIN_EVENT_TABLE(TracerFrame, wxFrame)
    EVT_MENU(Preset_Sphere1, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere2, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere3, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere4, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere5, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere6, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere7, TracerFrame::OnPreset)
    EVT_MENU(Preset_Sphere8, TracerFrame::OnPreset)

    EVT_MENU(Preset_Line1, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line2, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line3, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line4, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line5, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line6, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line7, TracerFrame::OnPreset)
    EVT_MENU(Preset_Line8, TracerFrame::OnPreset)

    EVT_MENU(Preset_Torus1, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus2, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus3, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus4, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus5, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus6, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus7, TracerFrame::OnPreset)
    EVT_MENU(Preset_Torus8, TracerFrame::OnPreset)

    EVT_MENU(Preset_H2O, TracerFrame::OnPreset)
END_EVENT_TABLE()

/* Tracer frame constructor */
TracerFrame::TracerFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, bool singleCanvas, bool noBackground)
    : wxFrame(frame, wxID_ANY, title, pos, size, style),
      backgroundColor(noBackground ? ConstColor3f(1, 1, 1) : ConstColor3f(.25, .25, .25))
{

    /* Make a menubar */
    m_presetMenu = new wxMenu;

    m_presetMenu->Append(Preset_Sphere1, wxT("&1 Sphere"));
    m_presetMenu->Append(Preset_Sphere2, wxT("&2 Spheres"));
    m_presetMenu->Append(Preset_Sphere3, wxT("&3 Spheres"));
    m_presetMenu->Append(Preset_Sphere4, wxT("&4 Spheres"));
    m_presetMenu->Append(Preset_Sphere5, wxT("&5 Spheres"));
    m_presetMenu->Append(Preset_Sphere6, wxT("&6 Spheres"));
    m_presetMenu->Append(Preset_Sphere7, wxT("&7 Spheres"));
    m_presetMenu->Append(Preset_Sphere8, wxT("&8 Spheres"));
    m_presetMenu->Append(Preset_Line1, wxT("1 &Line"));
    m_presetMenu->Append(Preset_Line2, wxT("2 &Lines"));
    m_presetMenu->Append(Preset_Line3, wxT("3 &Lines"));
    m_presetMenu->Append(Preset_Line4, wxT("4 &Lines"));
    m_presetMenu->Append(Preset_Line5, wxT("5 &Lines"));
    m_presetMenu->Append(Preset_Line6, wxT("6 &Lines"));
    m_presetMenu->Append(Preset_Line7, wxT("7 &Lines"));
    m_presetMenu->Append(Preset_Line8, wxT("8 &Lines"));
    m_presetMenu->Append(Preset_Torus1, wxT("1 &Torus"));
    m_presetMenu->Append(Preset_Torus2, wxT("2 &Torii"));
    m_presetMenu->Append(Preset_Torus3, wxT("3 &Torii"));
    m_presetMenu->Append(Preset_Torus4, wxT("4 &Torii"));
    m_presetMenu->Append(Preset_Torus5, wxT("5 &Torii"));
    m_presetMenu->Append(Preset_Torus6, wxT("6 &Torii"));
    m_presetMenu->Append(Preset_Torus7, wxT("7 &Torii"));
    m_presetMenu->Append(Preset_Torus8, wxT("8 &Torii"));
    m_presetMenu->Append(Preset_H2O, wxT("H2O"));
    m_menuBar = new wxMenuBar;
    m_menuBar->Append(m_presetMenu, wxT("&Preset"));
    if(!(screenCap||testMode2)) {
      SetMenuBar(m_menuBar);
    }

    if(singleCanvas) {
      numCanvas = 1;
      m_canvas[0] = new TracerGLCanvas(mainDebugMode, this, *this, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024), wxNO_BORDER); 
    } else {
      numCanvas = 3;
      m_split = new wxSplitterWindow(this); 
      m_rightSplit = new wxSplitterWindow(m_split); 
      for(int i = 0; i < numCanvas; ++i) {
        DebugMode debugMode = mainDebugMode;
        switch(i) {
          case 0: debugMode = mainDebugMode; break;
          case 1: debugMode = PLOT; break;
          case 2: debugMode = TRACE; break;
        }
        wxSize size = i == 0 ? wxSize(768, 1024) : wxSize(512, 512); 
        m_canvas[i] = new TracerGLCanvas(debugMode, i == 0 ? m_split : m_rightSplit, 
            *this, wxID_ANY, wxDefaultPosition,
            size, /*wxSUNKEN_BORDER*/ wxNO_BORDER);
      }
      m_split->SplitVertically(m_canvas[0], m_rightSplit); 
      m_rightSplit->SplitHorizontally(m_canvas[1], m_canvas[2]);
    }
    initParams();
    RefreshAll();

    //this->SetCanvas(m_canvas);
}

void TracerFrame::initParams() {
  selected = 0;
  ClearModels();

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
  traceEps = 1e-3;

  traceDiff = 0.0f;
  traceDiff.name("traceDiff");

  loop_cutoff.name("loop_cutoff");
  loop_cutoff = 255.0f;

  loop_highlight.name("loop_highlight");
  loop_highlight = 0.0f;

  eyeposm.name("eyeposm");

  sphereRadius = 10.0;
  graphScale = sphereRadius * 2; 
  graphOffset = ConstVector2f(0.0, 0.0);

  dbgCoords = Attrib2f(0.0, 0.0);

  camera.move(0.0, 0.0, -7.0);
  lightPos = Point3f(5.0, 5.0, 5.0);
  lightPos.name("lightPos");

  showTiming = true;
  showDot = false;

  rangeMode = AAUC;
  arithmeticName = RangeModeShortName[rangeMode]; 
}

/* Intercept menu commands */
void TracerFrame::OnPreset( wxCommandEvent& event )
{
  ClearModels();
  switch(event.GetId()) {
    case Preset_Sphere1: 
    case Preset_Sphere2: 
    case Preset_Sphere3: 
    case Preset_Sphere4: 
    case Preset_Sphere5: 
    case Preset_Sphere6: 
    case Preset_Sphere7: 
    case Preset_Sphere8: 
        ShowSphere(event.GetId() - Preset_Sphere1 + 1); break;

    case Preset_Line1: 
    case Preset_Line2: 
    case Preset_Line3: 
    case Preset_Line4: 
    case Preset_Line5: 
    case Preset_Line6: 
    case Preset_Line7: 
    case Preset_Line8: 
        ShowLine(event.GetId() - Preset_Line1 + 1); break;

    case Preset_Torus1: 
    case Preset_Torus2: 
    case Preset_Torus3: 
    case Preset_Torus4: 
    case Preset_Torus5: 
    case Preset_Torus6: 
    case Preset_Torus7: 
    case Preset_Torus8: 
        ShowTorus(event.GetId() - Preset_Torus1 + 1); break;

    case Preset_H2O: ShowH2O(); break;
    default:
      cout << "Unknown preset" << event.GetId() << endl; 
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
    pfs[0] = new DISTFUNC(new Dist2SpherePF(color[0]));
  }
  level = 1.0;
}

void TracerFrame::InitTrans(int numObj) {
  objCount = numObj;
  float side = 1.0;
  if(numObj > 1) {
    float foo = side * sqrt(3);
    trans[0] = Vector3f(-side, -0.5 * foo, 0);
    trans[1] = Vector3f(side, -0.5 * foo, 0);
    trans[2] = Vector3f(0, 0.5 * foo, 0);

    trans[3] = Vector3f(-side, 0.75 * foo, side);
    trans[4] = Vector3f(side, 0.75 * foo, side);
    trans[5] = Vector3f(0, -0.75 * foo, side);

    trans[6] = Vector3f(foo, 0, 0);
    trans[7] = Vector3f(-foo, 0, 0);
  } else {
    scl[0] = ConstAttrib3f(0.8, 0.8, 0.8);
  }
}

void TracerFrame::ShowSphere(int numSpheres) {
  for(int i = 0; i < numSpheres; ++i) {
      pfs[i] = new DISTFUNC(new Dist2SpherePF(color[i]));
  }
  InitTrans(numSpheres);
  ostringstream sout;
  sout << "sphere" << numSpheres;
  timingCase = sout.str();
}

void TracerFrame::ShowLine(int numLines) {
  for(int i = 0; i < numLines; ++i) {
    pfs[i] = new DISTFUNC(new Dist2LinePF(color[i]));
    fscl[i] = 0.52; 
  }
  //fscl[0] = ; 
  InitTrans(numLines);
  ostringstream sout;
  sout << "line" << numLines;
  timingCase = sout.str();
}

void TracerFrame::ShowTorus(int numTorus) {
  for(int i = 0; i < numTorus; ++i) {
      pfs[i] = new DISTFUNC(new Dist2TorusPF(color[i]));
      if(numTorus >= 2) fscl[i] = 0.7; 
  }
  InitTrans(numTorus);
  ostringstream sout;
  sout << "torus" << numTorus;
  timingCase = sout.str();
}

void TracerFrame::ShowH2O() {
  /* umm, should just read in some pdb or cml file */ 
  /* bond angles of water = 109.47 */
  ConstVector3f trans(0, 1, 0); 
  ConstVector3f rotAxis(0, 0, 1); 
  ConstAttrib1f bondAngle(0.95530596607909624);

  pfs[0] = new DISTFUNC(new Dist2TorusPF(color[0]));

  pfs[1] = new TransformPF(new DISTFUNC(new Dist2SpherePF(color[1])),
               translate(4 * trans) * rotate(rotAxis, -bondAngle)); 
  pfs[2] = new TransformPF(new DISTFUNC(new Dist2SpherePF(color[1])),
               translate(4 * trans) * rotate(rotAxis, bondAngle)); 

  pfs[3] = new TransformPF(new DISTFUNC(new Dist2LinePF(color[2], 3)), 
               translate(trans) * rotate(rotAxis, bondAngle)); 
  pfs[4] = new TransformPF(new DISTFUNC(new Dist2LinePF(color[2], 3)),
               translate(trans) * rotate(rotAxis, -bondAngle)); 
  level = 1.9;  
  timingCase = "h2o";
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
    case ',': break;
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


    case 's': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new Dist2SpherePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'i': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new Dist2LinePF(color[idx])); 
       reinit = true;
     }
     break;

    case 'c': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new Dist2CylinderPF(color[idx])); 
       reinit = true;
     }
     break;

    case 't': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new Dist2TorusPF(color[idx])); 
       //pfs[idx] = new Dist2TorusPF(color[idx]); 
       reinit = true;
     }
     break;


    case 'p': 
     if(selected > 0) {
       int idx = selected - 1;
       /*
       pfs[idx] = new DISTFUNC(new DistPlanePF(color[idx])); 
       */
       pfs[idx] = new NoisePF(pfs[idx], ConstAttrib1f(0.1f));
       reinit = true;
     }
     break;

    case 'T':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new TwistPF(pfs[idx], 1, 1);  
       reinit = true;
     }
     break;

    case 'S':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new SinPF(pfs[idx], 1, 0.1, 4);  
       reinit = true;
     }
     break;

/*
    case 'd':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new DropPF(color[idx])); 
       reinit = true;
     }
     break;
     */

    case 'g': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new GumdropTorusPF(color[idx]); 
       reinit = true;
     }
     break;

    case 'q': 
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new SuperquadricPF(color[idx], 0.75); 
       reinit = true;
     }
     break;

    case 'j':
     if(selected > 0) {
       int idx = selected - 1;
       // some constants taken from shrike;
       Attrib1f maxlvl=5;
       Attrib1f julia_max = 2.0;
       Attrib4f c(-0.08,0.0,-0.8,-0.03); /* from pbourke's site*/
       pfs[idx] = new JuliaPF(maxlvl, julia_max, c, color[idx]);
       reinit = true;
     }
     break;

/*
    case 'h':
     if(selected > 0) {
       int idx = selected - 1;
       pfs[idx] = new DISTFUNC(new HeightFieldPF("heightfield.png", color[idx]));
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
     camera.move(0.0, 0.0, -7.0);
     for(int i = 0; i < numCanvas; ++i) m_canvas[i]->SetupView();
     break;

    
    case 'z':
     if(rangeMode == AAUC) rangeMode = IA;
     else rangeMode = static_cast<RangeMode>(rangeMode - 1);
     arithmeticName = RangeModeShortName[rangeMode]; 
     reinit = true;
     break;

    case 'Z':
     if(rangeMode == IA) rangeMode = AAUC;
     else rangeMode = static_cast<RangeMode>(rangeMode + 1);
     arithmeticName = RangeModeShortName[rangeMode]; 
     reinit = true;
     break;

    case 'x':
     showTiming = !showTiming;
     break;

    case 'w':
      showAaDebug = !showAaDebug;
      Context::current()->set_flag("aa_disable_debug", !showAaDebug);
      reinit = true;
      break;

    case 'd':
      showDot = !showDot;
      cout << "Showing dot " << showDot << endl;
      //Context::current()->set_flag("aa_disable_debug", !showAaDebug);
      reinit = true;
      break;

    case '`': {
        testMode = true;
        // initialize test mode
        testRun = 0;
        testCase = Preset_Sphere1;
        testRange = AAUC;
        testFrame = 0;
        testFpsTotal = 0;
        wxCommandEvent evt(0, testCase);
        OnPreset(evt);
        break;
    }
      

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
     cout << "  m/M           Change rendering mode" << endl;
     cout << "  z/Z           Change range arithmetic mode" << endl;
     cout << "  k/K           Change bounding sphere radius" << endl;
     cout << "  v/V           Change value of level set to intersect" << endl; 
     cout << "  e/E           Change epsilon (determines when to end tracing iterations)" << endl;
     cout << "  f/F           Change trace epsilon (determines how close to level set value roots must be)" << endl; 
     cout << "  l/L           Change loop count loop_cutoff in Loop Cutoff mode" << endl; 
     //cout << "  g/G           Change graph y scale in Trace mode" << endl; 
     cout << "  w             Show debug" << endl;
     cout << "  x             Show timings" << endl;
     cout << "  d             Show dot" << endl;
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
     cout << "  i             Make line segment (line segment generator) " << endl;
     cout << "  c             Make object a cylinder (line generator)" << endl;
     cout << "  t             Make object a torus (circle generator)" << endl;
     //cout << "  d             Make object a drop" << endl;
     cout << "  g             Make gumdrop torus function" << endl;
     cout << "  q             Make superquadric function" << endl;
     cout << "  p             Make object a plane (plane generator)" << endl;
     cout << "  D             Distort with a twist" << endl; 
     //cout << "  h             Make object a texture mapped heightfield (SLOW)" << endl;
     cout << "  j             Make object a julia fractal (SLOW!!)" << endl;
     cout << "  n             Make object empty" << endl;
     cout << "  Middle Button Translate object x/y" << endl;
     cout << "  Right Button  Translate object z" << endl;
     cout << "  Shift-Left/Mddle/Right Button    Change object x/y/z scaling" << endl;
     cout << "  Ctrl-Left Button Change function scaling" << endl;
      cout << "Sphere Radius: " << sphereRadius.getValue(0) << endl;
      cout << "Level: " << level.getValue(0) << endl;
      cout << "Graph scale: " << graphScale << endl; 
      cout << "Epsilon : " << eps.getValue(0) << " Trace Epsilon: " << traceEps.getValue(0) << endl;
      cout << "Trace Extra : " << traceDiff.getValue(0) << endl;
      cout << "Loop Cutoff : " << loop_cutoff.getValue(0) << endl;
      cout << "Loop Highlight: " << loop_highlight.getValue(0) << endl;
      cout << "Arithmetic Type : " << RangeModeName[rangeMode] << endl; 
      break;
  }


  if(reinit) {
    InitShaders();
    for(int i = 0; i < 9; ++i) {
      if(pfs[i]) {
        cout << "PF " << i << ": " << pfs[i]->name() << endl; 
      }
    }
  }

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

int attribList[] = {
  WX_GL_DOUBLEBUFFER, 1,
  0
};

TracerGLCanvas::TracerGLCanvas(DebugMode mode, wxWindow* parent, TracerFrame& tframe, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name, attribList),
      debugMode(mode), do_init(true), m_tframe(tframe),
      m_context(Context::createContext())
{
  //m_context->disable_optimization("propagation");
  //m_context->optimization(0);
}

TracerGLCanvas::~TracerGLCanvas()
{
}

void TracerGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    static int frameCount = 0;

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
    glFinish();

    startTimer("render");
    //cout << "Debug mode=" << DebugModeName[debugMode] << endl;
    if(debugMode == TRACE || debugMode == PLOT) {
      glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 0.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
      glEnd();
    } else {
      GLUquadric* sph;
      sph = gluNewQuadric();
      gluSphere(sph, m_tframe.sphereRadius.getValue(0), 32, 32);
    }

    glFinish();
    if(testMode2 || testMode) {
      if(testFrame == 0) {
        cout << "timing:render_headers,case,framecount,objCount,bumpCount,twistCount,GFLOPS,avg_loops" << fsh_stats.header() << firsthit_stats.header() << tracer_stats.header() << ",shape,arithmetic,time,fps" << endl;
      }
    }
    double elapsed = getElapsed();
    double fps = 1.0 / elapsed;
    double gflops = 0.0;
    float savedLoops = 0;
    if(savedLoopCount.find(caseName) != savedLoopCount.end() && (m_tframe.rangeMode == AAUC || m_tframe.rangeMode == AA)) {
      savedLoops = savedLoopCount[caseName];
      int instr_count = savedLoops * firsthit_stats.scalar_instr_count + (fsh_stats.scalar_instr_count - firsthit_stats.scalar_instr_count); 
      gflops = fps * testSize * testSize * instr_count; 
      gflops /= 1e9; 
    }
    cout << "timing:render," << caseName << "," << frameCount++ << "," << objCount << "," << bumpCount << "," << twistCount 
         << "," << gflops << "," << savedLoops 
         << fsh_stats.csv() << firsthit_stats.csv() << tracer_stats.csv()
         << "," << timingCase << "," << arithmeticName << "," << elapsed << "," <<  (1.0 / elapsed) << endl; 
  
    // Swap
    SwapBuffers();

    if(testMode) {
      testFpsTotal += 1.0 / elapsed;
      if(++testFrame == numTestFrames) { /* next case */ 
        //cout << "avg_timing:render," << timingCase << "," << arithmeticName << "," << instr_count << "," << scalar_instr_count << "," << (testFpsTotal / numTestFrames) << "," << temps << "," << scalar_temps << endl; 
        testFrame = 0;
        testFpsTotal = 0;
        if(testRange == IA) { /* next preset */
          testRange = AAUC;
          if(++testCase > Preset_H2O) {
            if(++testRun >= numRuns) {
              testMode = false;
            } else {
              testCase = Preset_Sphere1;
              wxCommandEvent evt(0, testCase);
              m_tframe.OnPreset(evt);
            }
          } else {
            wxCommandEvent evt(0, testCase);
            m_tframe.OnPreset(evt);
          }
        } else { /* next arithmetic mode */
          testRange++; 
        }
        m_tframe.rangeMode = static_cast<RangeMode>(testRange);
        arithmeticName = RangeModeShortName[m_tframe.rangeMode]; 
        InitShaders();
      } 
      m_tframe.RefreshAll();
    }
    if(testMode2) {
      if(++testFrame == numTestFrames) {
        testFrame = 0;
        if(++testRange > IA) {  
          m_tframe.Close(true);
        } else {
          m_tframe.rangeMode = static_cast<RangeMode>(testRange);
          arithmeticName = RangeModeShortName[m_tframe.rangeMode]; 
          InitShaders();
        }
      }
      m_tframe.RefreshAll();
    }
    if(screenCap && testFrame++ == 1) {
      ostringstream cmdout;
      cmdout << "/usr/bin/import -window shimpl " << caseName << ".png";
      system(cmdout.str().c_str()); 
      m_tframe.Close(true);
    }
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
    setBackend(defaultBackend);
    do_init = false;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glEnable(GL_CULL_FACE);
    // stupid hack - we only need one surface of the sphere anyway, so let's
    // enable front face culling (more chance we'll get part of the sphere, even if viewpoint is inside */ 
    glCullFace(GL_FRONT);
    //glCullFace(GL_BACK);

    float bgcolor[3];
    m_tframe.backgroundColor.getValues(bgcolor);
    glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], 1.0);

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

    Context::current()->set_flag("aa_disable_debug", !showAaDebug);

    startTimer("build_vsh");
    if(debugMode == TRACE || debugMode == PLOT) {
      Matrix4x4f id;
      vsh = KernelLib::vsh(id, id);
      vsh = vsh << extract("lightPos") << ConstAttrib3f(0, 0, 0);
    } else {
      vsh = KernelLib::vsh(m_tframe.mv, m_tframe.mvd);
      vsh = vsh << extract("lightPos") << m_tframe.lightPos;
    }

    printElapsed("build_vsh");

    Color3f SH_DECL(ks) = Color3f(0.3, 0.3, 0.3);
    Attrib1f SH_DECL(specexp) = 64.0f;

    ConstColor3f lowColor = ColorFinder::reverseColor(ColorFinder::YlGnBu, 3, 0); 
    ConstColor3f highColor= ColorFinder::reverseColor(ColorFinder::YlGnBu, 3, 2); 

    /* Set flags */
    switch(m_tframe.rangeMode) {
      case AAUC: Context::current()->set_flag("aa_enable_um", true); break; 
      case AA: Context::current()->set_flag("aa_enable_um", false); break; 
      case IA: Context::current()->set_flag("aa_enable_um", true); break; 
    }

    // Build the plot function 
    SumPFPtr plotfunc = new SumPF(m_tframe.level);
    for(int i = 0; i < 9; ++i) {
      if(m_tframe.pfs[i]) {
        plotfunc->add(new FuncScalePF(new TranslatePF(new ScalePF(m_tframe.pfs[i], m_tframe.scl[i]), m_tframe.trans[i]), m_tframe.fscl[i]));
      }
    }

    string shapeName = plotfunc->name(); 
    if(caseName != "") {
      shapeName = caseName;
    } else if(timingCase != "") {
      shapeName = timingCase;
    }

    Program func = plotfunc->funcProgram("posm");
    func.name(shapeName);
    Program gradient;
    if(showGradient) {
      gradient = plotfunc->gradProgram("posm");
    } else {
      gradient = plotfunc->nullProgram("posm", ConstAttrib3f(0, 0, 1)); 
    }
    Program colorFunc = plotfunc->colorProgram("posm");

    printElapsed("build_plotfunc");

    Color3f SH_DECL(lightColor) = ConstAttrib3f(0.75, .75, .75);

    Program tracer, hitter;

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
      tracer = trace(func)(m_tframe.eyeposm, dir);
      tracer = add<Attrib1f>() << -m_tframe.level << tracer;
      tracer.name("tracer_" + shapeName);


      cout << "Building firsthit" << endl;
      hitter = firsthit(tracer, use_aa, debugMode, m_tframe, texcoord);

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
        if(showDot) {
          kill(!hashit && !inDbg);
        } else {
          kill(!hashit); 
        }
        Point3f SH_DECL(hitp) = mad(hit, dir, m_tframe.eyeposm);

        OutputNormal3f SH_DECL(normal);
        OutputVector3f SH_DECL(lightVec);
        OutputVector3f SH_DECL(halfVec);
        normal = gradient(hitp);
        normal = m_tframe.mv | normal;
        normal = normalize(normal);
        //lightVec = m_tframe.mv | (m_tframe.lightPos - hitp); 
        lightVec = m_tframe.lightPos - (m_tframe.mv | hitp); 
        lightVec = normalize(lightVec);
        viewVec = normalize(viewVec);
        halfVec = normalize(lightVec + viewVec);

        ConstAttrib1f ONE(1.0f);
        normal = faceforward(lightVec, normal); 

        OutputColor3f SH_DECL(kd) = colorFunc(hitp);
        if(showDot) {
          kd = lerp(inDbg, TufteOrange, kd);
          normal = lerp(inDbg, lightVec, normal);
        }
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
              //color = lerp(hashit, lerp(hit, lowColor, highColor), ConstColor3f(0.5, 0.5, 0.5));
              color = hit(0,0,0);
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
      case AAUC: fsh.name("fsh_aa_" + shapeName); break;
      case AA: fsh.name("fsh_aa_nouc_" + shapeName); break;
      case IA: fsh.name("fsh_ia_" + shapeName); break;
    }
    printElapsed("build_fsh" + fsh.name());

    vsh = namedAlign(vsh, fsh);
    vsh.name("vsh");

    /* dump out various pieces */
    if(showAaDebug || testMode2) {
      tracer.node()->dump("tracer_" + shapeName);
      Program foo;
      if(m_tframe.rangeMode == AAUC || m_tframe.rangeMode == AA) {
          foo = affine_inclusion_syms(tracer.clone(true));
          string fooName = (m_tframe.rangeMode == AAUC ? "aa_tracer_" : "aa_nouc_tracer") + shapeName;
          foo.name(fooName);
          foo.node()->dump(fooName);
          compile(foo, "gpu:fragment");
      } else {
          foo = inclusion(tracer.clone(true));
          foo.name("ia_tracer_" + shapeName);
          foo.node()->dump("ia_tracer_" + shapeName);
          compile(foo, "gpu:fragment");
      }
      tracer_stats.update();

      foo = hitter.clone(true);
      foo.node()->dump("firsthit_" + shapeName);
      compile(foo, "gpu:fragment");
      firsthit_stats.update();

      dump(fsh, fsh.name());
      dump(vsh, "vsh");
    }

    startTimer("Binding Shaders");
    lock_in_uniforms(vsh);
    lock_in_uniforms(fsh);
    shaders = new ProgramSet(vsh, fsh);
    bind(*shaders);
    fsh_stats.update();
    printElapsed("bind_shaders");
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

