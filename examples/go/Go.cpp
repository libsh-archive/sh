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


#include <cctype>
#include <unistd.h>
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
#include "Case.hpp"


using namespace std;
using namespace SH;

ostream& operator<<(ostream & out, const wxString & str) {
  out << str.mb_str();
  return out;
} 

namespace {
bool use_aa = true;

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

string lower(const string& str) {
  string result = str;
  for(size_t i = 0; i < result.size(); ++i) {
    result[i] = tolower(result[i]); 
  }
  return result;
}

bool ends_with(const string& str, const string& suffix) {
  return str.length() >= suffix.length() && str.rfind(suffix) == str.length() - suffix.length(); 
}

template<typename T>
void write(ostream& out, const T& value) {
  out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void writeString(ostream& out, const char* value) {
  int len = strlen(value);
  write(out, len); 
  out.write(value, len + 1); 
}

void writeString(ostream& out, const string& s) {
  writeString(out, s.c_str());
}

template<typename T>
T read(istream& in) {
  T result;
  in.read(reinterpret_cast<char*>(&result), sizeof(T));
  return result;
}

string readString(istream& in) {
  int len = read<int>(in);
  char* buf = new char[len + 1];
  in.read(buf, len + 1);
  string result = buf;
  delete[] buf;
  return result;
}

}

// Event IDs 
enum
{
    // our menu items
    
    // note that for the loading of old objects to work, this list can only be appended to
    // NO deletions or insertions before the end.
    // (Saved objects record how they're constructed from basic ops). 
    // (yeah, it's stupid...but change some of the IDs and you rewrite history.
    // Hitler becomes Mother Teresa, and your functions suddenly exist in an alternate universe) 
    
    // generator 
    ID_OPEN_OBJECT = 100, /* currently supports .svg, .goo */
    ID_SAVE_OBJECT,
    ID_POINT,
    ID_LINE,
    ID_CIRCLE,
    ID_SIN,
    ID_PLANE,

    ID_OP_LERP,
    ID_OP_PROFILE_PROD,
    ID_OP_WIRE_PROD,
    ID_OP_ADD,
    ID_OP_SUB,
    ID_OP_MUL,
    ID_OP_NOISE,
    ID_OP_FQERIUSDPJFPQORPOSPDOFUIPOQUR, // not used for the moment 
    ID_OP_DISTANCE,
    ID_OP_PLOTDIST, /* only valid for 2 curves */

    ID_CONSTANT,    /* numerical constant */
    ID_X0,
    ID_X1,
    ID_X2,
    ID_OP_ROTX,
    ID_OP_ROTY,
    ID_OP_ROTZ,
    ID_OP_TRANSX,
    ID_OP_TRANSY,
    ID_OP_TRANSZ,
    ID_SQUARE,
    ID_OP_SWIZ0,
    ID_OP_SWIZ1,
    ID_SPHERE,
    ID_OP_OFFSET2D,

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

    string mode = "aa";
    string filename = "";
    for(size_t i = 0; i < argc; ++i) {
      string foo = static_cast<const char*>(wxString(argv[i]).mb_str());
      GoFrame::StackOp op;
      if(split(foo, '.').back() == "goo") {
        op.id = ID_OPEN_OBJECT;
        op.param = foo; 
        filename = split(op.param, '/').back();
      } else if (foo == "dist") {
        op.id = ID_OP_DISTANCE; 
        caseName = mode + "_" + filename;
      } else if (foo == "noum") {
        Context::current()->set_flag("aa_enable_um", false); 
        cout << "UM enabled = " << Context::current()->get_flag("aa_enable_um") << endl;
        mode = "aanoum";
        continue;
      } else if (foo == "ia") {
        use_aa = false;
        cout << "Using IA" << endl;
        mode = "ia";
        continue;
      } else if (foo == "quit") {
        frame->Close(true);
        break;
      } else {
        continue;
      }
      if(frame->handleStackOp(op)) { /* opening and saving are left out */ 
        frame->m_history.push_back(op);
        frame->updateHistoryMap();
      }
    }

    return true;
}

IMPLEMENT_APP(GoApp)

BEGIN_EVENT_TABLE(GoFrame, wxFrame)
  EVT_MENU      (wxID_EXIT,               GoFrame::OnExit)

  EVT_BUTTON    (ID_OPEN_OBJECT,         GoFrame::OnButton)
  EVT_BUTTON    (ID_SAVE_OBJECT,         GoFrame::OnButton)
  EVT_BUTTON    (ID_POINT,                 GoFrame::OnButton)
  EVT_BUTTON    (ID_LINE,                 GoFrame::OnButton)
  EVT_BUTTON    (ID_PLANE,                GoFrame::OnButton)
  EVT_BUTTON    (ID_CIRCLE,               GoFrame::OnButton)
  EVT_BUTTON    (ID_SPHERE,               GoFrame::OnButton)

  EVT_BUTTON    (ID_OP_LERP,      GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_OFFSET2D,      GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_PROFILE_PROD,      GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_WIRE_PROD,         GoFrame::OnButton)

  EVT_BUTTON    (ID_OP_ADD,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_SUB,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_MUL,               GoFrame::OnButton)
  EVT_BUTTON    (ID_SIN,               GoFrame::OnButton)
  EVT_BUTTON    (ID_SQUARE,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_SWIZ0,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_SWIZ1,               GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_NOISE,             GoFrame::OnButton)
  //EVT_BUTTON    (ID_OP_NOISE3D,           GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_DISTANCE,          GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_PLOTDIST,          GoFrame::OnButton)

  EVT_BUTTON    (ID_X0,                   GoFrame::OnButton)
  EVT_BUTTON    (ID_X1,                   GoFrame::OnButton)
  EVT_BUTTON    (ID_X2,                   GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_ROTX,              GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_ROTY,              GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_ROTZ,              GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_TRANSX,              GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_TRANSY,              GoFrame::OnButton)
  EVT_BUTTON    (ID_OP_TRANSZ,              GoFrame::OnButton)

  EVT_CHAR      (GoFrame::OnChar)
END_EVENT_TABLE()

/* Go frame constructor */
GoFrame::GoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style), m_domain_eps(1e-4)
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
        m_generatorBox->Add(m_openObjectBtn = new wxButton(m_controlsPanel, ID_OPEN_OBJECT, _T("&Open Object..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_saveObjectBtn = new wxButton(m_controlsPanel, ID_SAVE_OBJECT, _T("&Save Object..."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_pointBtn= new wxButton(m_controlsPanel, ID_POINT, _T("&point"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_lineBtn= new wxButton(m_controlsPanel, ID_LINE, _T("&line"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_planeBtn= new wxButton(m_controlsPanel, ID_PLANE, _T("plane"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_circleBtn= new wxButton(m_controlsPanel, ID_CIRCLE, _T("&circle"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_generatorBox->Add(m_sphereBtn= new wxButton(m_controlsPanel, ID_SPHERE, _T("&sphere"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);

      m_controlsSizer->Add(
        m_curveopBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("2D Curve Tools")), wxVERTICAL ), 0, wxEXPAND);
        m_curveopBox->Add(m_lerpBtn = new wxButton(m_controlsPanel, ID_OP_LERP, _T("lerp"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                             0, wxEXPAND);
        //m_curveopBox->Add(m_offset2dBtn = new wxButton(m_controlsPanel, ID_OP_OFFSET2D, _T("offset2d"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
        //                     0, wxEXPAND);
        m_curveopBox->Add(m_profileBtn = new wxButton(m_controlsPanel, ID_OP_PROFILE_PROD, _T("profile product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                             0, wxEXPAND);
        m_curveopBox->Add(m_wireBtn = new wxButton(m_controlsPanel, ID_OP_WIRE_PROD, _T("wire product"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                             0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);

      m_controlsSizer->Add(
        m_funcopBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("Operators")), wxVERTICAL ), 0, wxEXPAND);
        m_funcopBox->Add(m_basicopSizer= new wxGridSizer(3, 0), 0, wxEXPAND);
            m_basicopSizer->Add(m_addBtn = new wxButton(m_controlsPanel, ID_OP_ADD, _T("+"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                               0, wxEXPAND);
            m_basicopSizer->Add(m_subBtn = new wxButton(m_controlsPanel, ID_OP_SUB, _T("-"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                               0, wxEXPAND);
            m_basicopSizer->Add(m_mulBtn = new wxButton(m_controlsPanel, ID_OP_MUL, _T("*"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                               0, wxEXPAND);
            m_basicopSizer->Add(m_sinBtn= new wxButton(m_controlsPanel, ID_SIN, _T("sine"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                               0, wxEXPAND);
            m_basicopSizer->Add(m_squareBtn= new wxButton(m_controlsPanel, ID_SQUARE, _T("^2"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                               0, wxEXPAND);
            //m_basicopSizer->Add(m_swiz0Btn= new wxButton(m_controlsPanel, ID_OP_SWIZ0, _T("v[0]"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
            //                   0, wxEXPAND);
            //m_basicopSizer->Add(m_swiz1Btn= new wxButton(m_controlsPanel, ID_OP_SWIZ1, _T("v[1]"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
            //                  0, wxEXPAND);
        m_funcopBox->Add(m_noiseBtn = new wxButton(m_controlsPanel, ID_OP_NOISE, _T("add noise"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_funcopBox->Add(m_distanceBtn = new wxButton(m_controlsPanel, ID_OP_DISTANCE, _T("&minimum distance"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);
        m_funcopBox->Add(m_plotdistBtn = new wxButton(m_controlsPanel, ID_OP_PLOTDIST, _T("plot distance function"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
                           0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);
      m_controlsSizer->Add(
        m_paramBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("Parametric Coords")), wxVERTICAL) ,0, wxEXPAND);
        m_paramBox->Add(m_paramSizer = new wxGridSizer(0, 3), 0, wxEXPAND);
        m_paramSizer->Add(m_x0Btn = new wxButton(m_controlsPanel, ID_X0, _T("x0"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_paramSizer->Add(m_x1Btn = new wxButton(m_controlsPanel, ID_X1, _T("x1"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_paramSizer->Add(m_x2Btn = new wxButton(m_controlsPanel, ID_X2, _T("x2"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);

      m_controlsSizer->AddSpacer(20);
      m_controlsSizer->Add(
        m_transBox = new wxStaticBoxSizer(new wxStaticBox(m_controlsPanel, wxID_ANY, _T("Affine Transforms")), wxVERTICAL) ,0, wxEXPAND);
        m_transBox->Add(m_transSizer = new wxGridSizer(3, 0), 0, wxEXPAND);
        m_transSizer->Add(m_rotxBtn = new wxButton(m_controlsPanel, ID_OP_ROTX, _T("rotx"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_transSizer->Add(m_rotyBtn = new wxButton(m_controlsPanel, ID_OP_ROTY, _T("roty"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_transSizer->Add(m_rotzBtn = new wxButton(m_controlsPanel, ID_OP_ROTZ, _T("rotz"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_transSizer->Add(m_transxBtn = new wxButton(m_controlsPanel, ID_OP_TRANSX, _T("transx"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_transSizer->Add(m_transyBtn = new wxButton(m_controlsPanel, ID_OP_TRANSY, _T("transy"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);
        m_transSizer->Add(m_transzBtn = new wxButton(m_controlsPanel, ID_OP_TRANSZ, _T("transz"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER), 0, wxEXPAND);

      m_controlsSizer->AddSpacer(30);

      m_controlsSizer->Add(m_stack = new GoManVec(this, m_controlsPanel), wxSizerFlags().Expand().Proportion(1));
      RenderNode::parentWindow = m_stack;
    m_canvas = new GoGLCanvas(this, m_split, wxID_ANY, wxDefaultPosition,
        wxSize(768, 200), wxSUNKEN_BORDER);
    RenderNode::canvasWindow = m_canvas; 

    m_split->SplitVertically(m_controlsPanel, m_canvas, 200);
}

/* Handle button presses */
void GoFrame::OnButton( wxCommandEvent &event ) {
  StackOp op;
  op.id = event.GetId();
  switch(op.id) { 
    case ID_OPEN_OBJECT: {
      wxString obj_filename = wxFileSelector(_T("Open Object"),_T(""),_T(""),_T(""),
                                     _T("Saved Object (*.goo)|*.goo|SVG files (*.svg)|*.svg|Bezier patch (*.bezier)|*.bezier"), wxOPEN); 
      op.param = static_cast<const char*>(obj_filename.mb_str());
      caseName = split(op.param, '/').back();

      break;
    }
    case ID_SAVE_OBJECT: { 
      wxString obj_filename = wxFileSelector(_T("Save Object"),_T(""),_T(""),_T(""),
                                     _T("Saved Object (*.goo)|*.goo"), wxSAVE); 
      op.param = static_cast<const char*>(obj_filename.mb_str());
      break;
    }
    default: break;
  }
  /* strip current working directory from param, if found
   * Makes stored paths more portable when switching systems */
  char* cwdbuf = getcwd(0, 0);
  //cout << "CWD=" << cwdbuf << endl;
  if(op.param.find(cwdbuf, 0) == 0) {
    op.param = op.param.substr(strlen(cwdbuf) + 1);
  }
  free(cwdbuf);
  
  if(handleStackOp(op)) { /* opening and saving are left out */ 
    m_history.push_back(op);
    updateHistoryMap();
  }
}

bool GoFrame::handleStackOp(const StackOp& op) { 
  switch(op.id) {
    case ID_OPEN_OBJECT: {
      const string& filename = op.param;
      if(filename.empty()) break;

      if(ends_with(lower(filename), ".svg")) {
        Man curve = m_svg_crv(filename.c_str()) * 0.01; 
        curve.name(split(filename, '/').back());
        m_stack->push(new ManRenderNode(curve));
      } else if (ends_with(lower(filename), ".bezier")) {
        Surface3D surface = m_read_tea_surface(filename.c_str());
        surface.name(split(filename, '/').back());
        m_stack->push(new ManRenderNode(surface));
      } else if (ends_with(lower(filename), ".goo")) {
        ifstream in(filename.c_str(), ifstream::binary | ifstream::in); 

        int count = read<int>(in);
        //cout << "Reading in " << count << " ops" << endl;
        for(int i = 0; i < count; ++i) {
          StackOp op;
          op.id = read<int>(in); 
          op.param = readString(in);
          //cout << "i=" << i << " param=" << op.param << endl;
          handleStackOp(op);
          m_history.push_back(op);
          updateHistoryMap();
        }

        int stacksize = read<int>(in); 
        for(int i = -1; i >= -stacksize; --i) { 
          m_stack->node(i)->readState(in); 
        }
        return false;
      } else {
        cerr << "Unrecognized file extension" << endl;
        return false;
      }
      break;
    }

    case ID_SAVE_OBJECT: {
      const string& filename = op.param;
      if(filename.empty()) break;
      ofstream out(filename.c_str(), ofstream::binary | ofstream::out); 

      int count = m_history.size();
      write(out, count); 
      //cout << "Writing out " << count << " ops" << endl;
      for(int i = 0; i < count; ++i) {
        write(out, m_history[i].id);
        writeString(out, m_history[i].param);
      }

      int stacksize = m_stack->size();
      write(out, stacksize);
      for(int i = -1; i >= -stacksize; --i) { 
        m_stack->node(i)->saveState(out); 
      }
      break;
      out.close();
      return false;
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

    case ID_SPHERE: {
      ConstAttrib1f SH_DECL(PI) =  M_PI;
      ConstAttrib1f SH_DECL(TWOPI) = 2 * M_PI;
      m_stack->push(new ManRenderNode(m_sphere(TWOPI * m_u(0), PI * m_u(1))));
      break;
    }

    case ID_OP_PROFILE_PROD: {
      ProfileRenderNode* prn = new ProfileRenderNode(m_stack->node(-2), m_stack->node(-1));
      //prn->man().node()->dump("profile");
      m_stack->pop(2);
      m_stack->push(prn);
      break;
    }

    case ID_OP_WIRE_PROD: {
      WireRenderNode* wrn = new WireRenderNode(m_stack->node(-2), m_stack->node(-1));
      //wrn->man().node()->dump("wire");
      m_stack->pop(2);
      m_stack->push(wrn);
      break;
    }

    case ID_OP_LERP: 
    case ID_OP_OFFSET2D:
    case ID_OP_ADD: 
    case ID_OP_SUB: 
    case ID_OP_MUL: 
    case ID_OP_NOISE: 
    case ID_OP_ROTX:
    case ID_OP_ROTY:
    case ID_OP_ROTZ: 
    case ID_OP_TRANSX:
    case ID_OP_TRANSY:
    case ID_OP_TRANSZ: {
      ManRenderNodePtr a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-2));   
      ManRenderNodePtr b = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));
      m_stack->pop(2);
      switch(op.id) {
        case ID_OP_LERP:{
            ManRenderNodePtr t = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));   
            m_stack->pop(1);
            m_stack->push(new ManRenderNode(lerp(t->man(), a->man(), b->man()))); break;
        }
        case ID_OP_OFFSET2D: m_stack->push(new ManRenderNode(m_offset2d(a->man(), b->man()))); break;
        case ID_OP_ADD: m_stack->push(new ManRenderNode(a->man() + b->man())); break;
        case ID_OP_SUB: m_stack->push(new ManRenderNode(a->man() - b->man())); break;
        case ID_OP_MUL: m_stack->push(new ManRenderNode(a->man() * b->man())); break;
        case ID_OP_NOISE: m_stack->push(new ManRenderNode(m_add_noise(a->man(), b->man()))); break;
        case ID_OP_ROTX: m_stack->push(new ManRenderNode(m_rotatex(a->man(), b->man()))); break;
        case ID_OP_ROTY: m_stack->push(new ManRenderNode(m_rotatey(a->man(), b->man()))); break;
        case ID_OP_ROTZ: m_stack->push(new ManRenderNode(m_rotatez(a->man(), b->man()))); break;
        case ID_OP_TRANSX: m_stack->push(new ManRenderNode(m_translatex(a->man(), b->man()))); break;
        case ID_OP_TRANSY: m_stack->push(new ManRenderNode(m_translatey(a->man(), b->man()))); break;
        case ID_OP_TRANSZ: m_stack->push(new ManRenderNode(m_translatez(a->man(), b->man()))); break;
      }
      break;
    }

    case ID_SIN: 
    case ID_SQUARE: 
    case ID_OP_SWIZ0:
    case ID_OP_SWIZ1: {
      Man a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1))->man();   
      m_stack->pop(1);
      switch(op.id) {
        case ID_SIN: m_stack->push(new ManRenderNode(sin(a))); break;
        case ID_OP_SWIZ0: m_stack->push(new ManRenderNode(a(0))); break;
        case ID_OP_SWIZ1: m_stack->push(new ManRenderNode(a(1))); break;
        case ID_SQUARE: {
          Program f = SH_BEGIN_PROGRAM() {
            Variable x = a.outsize_var(SH_INOUT);
            shMUL(x, x, x);
          } SH_END;
          Man result = f << a;
          result.name("(" + a.name() + ")^2");
          m_stack->push(new ManRenderNode(result));
          break;
        }
      }
      break;
    }

    case ID_OP_DISTANCE: 
    case ID_OP_PLOTDIST: {
      ManRenderNodePtr a, b; 
      int i;

      /* Get two nodes, ignore host mans */
      for(i = -1; shref_dynamic_cast<HostManRenderNode>(m_stack->node(i)); --i);
      b = shref_dynamic_cast<ManRenderNode>(m_stack->node(i));
      for(--i; shref_dynamic_cast<HostManRenderNode>(m_stack->node(i)); --i);
      a = shref_dynamic_cast<ManRenderNode>(m_stack->node(i));

      Man am = a->transformed_man();
      //am.node()->dump("am");
      Man bm = b->transformed_man();
      //am.node()->dump("bm");
      int A = am.input_size();
      int B = bm.input_size();
      int N = std::max(am.output_size(), bm.output_size());
      am = am.fill(N);
      bm = bm.fill(N);
      HostMan dist; 
      if(op.id == ID_OP_PLOTDIST) {
        switch(A) {
          case 0:
            switch(B) {
              case 0: assert(false); break;
              case 1: switch(N) { case 2: dist = distance_graph<0, 1, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = distance_graph<0, 1, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 2: switch(N) { case 2: dist = distance_graph<0, 2, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = distance_graph<0, 2, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            }
            break;
          case 1:
            switch(B) {
              case 0: switch(N) { case 2: dist = distance_graph<1, 0, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = distance_graph<1, 0, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 1: switch(N) { case 2: dist = distance_graph<1, 1, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = distance_graph<1, 1, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            } break;
            break;
          case 2:
            switch(B) {
              case 0: switch(N) { case 2: dist = distance_graph<2, 0, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = distance_graph<2, 0, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            }
            break;
        }
      } else {
        switch(A) {
          case 0:
            switch(B) {
              case 0: assert(false); break;
              case 1: switch(N) { case 2: dist = nearest_points<0, 1, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<0, 1, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 2: switch(N) { case 2: dist = nearest_points<0, 2, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<0, 2, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            }
            break;
          case 1:
            switch(B) {
              case 0: switch(N) { case 2: dist = nearest_points<1, 0, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<1, 0, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 1: switch(N) { case 2: dist = nearest_points<1, 1, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<1, 1, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 2: switch(N) { case 2: dist = nearest_points<1, 2, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<1, 2, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            } break;
            break;
          case 2:
            switch(B) {
              case 0: switch(N) { case 2: dist = nearest_points<2, 0, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<2, 0, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 1: switch(N) { case 2: dist = nearest_points<2, 1, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<2, 1, 3>(am, bm, m_domain_eps, use_aa); break; } break;
              case 2: switch(N) { case 2: dist = nearest_points<2, 2, 2>(am, bm, m_domain_eps, use_aa); break; case 3: dist = nearest_points<2, 2, 3>(am, bm, m_domain_eps, use_aa); break; } break;
            }
            break;
        }
      }
      size_t sampleRate = op.id == ID_OP_DISTANCE ? 2 : 0;
      RenderNodePtr distnode = new HostManRenderNode(dist, sampleRate);
      m_stack->push(distnode);
      distnode->dependsOn(a);
      distnode->dependsOn(b);
      break;
    }

    case ID_CONSTANT: {
      m_stack->push(new ManRenderNode(m_(op.getFloat())));
      /*
        ManRenderNodePtr a = shref_dynamic_cast<ManRenderNode>(m_stack->node(-1));   
        m_stack->pop(1);
        m_stack->push(new ManRenderNode(a->man() * m_(op.getFloat())));
      */
      break;
    }

    case ID_X0: 
    case ID_X1:
    case ID_X2: {
      m_stack->push(new ManRenderNode(m_u(op.id - ID_X0)));
      break;
    }

  }
  return true;
}

void GoFrame::updateHistoryMap() {
  /* Shrink or expand if necessary */
  while(historyMap.size() > m_stack->size()) historyMap.pop_back(); 
  int curHistoryIdx = m_history.size() - 1;
  while(historyMap.size() < m_stack->size()) historyMap.push_back(curHistoryIdx);
  if(!historyMap.empty()) {
    historyMap.back() = curHistoryIdx;
  }
}

void GoFrame::popHistory() {
  historyMap.pop_back();
  if(historyMap.empty()) {
    m_history.clear();
  } else {
    m_history.resize(historyMap.back() + 1);
  }
}

/* Intercept menu commands */
void GoFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

void GoFrame::OnChar( wxKeyEvent& event)
{
    switch( event.GetKeyCode() ) {
      case 'q': case 'Q':
        Close(1);
        break;

      case 'p': case 'P': /* paper mode - change background and diffuse color */
        cout << "Paper mode" << endl;
        m_canvas->setClearColor(Color3f(1, 1, 1));
        RenderNode::lineColor = Color3f(0.0, 0.0, 0.0);
        RenderNode::diffuseColor = Color3f(0.5, 0.5, 0.5);
        RenderNode::paperMode = true;
        Refresh(false);
        break;

      case 's': case 'S': /* screen mode */
        cout << "Screen mode" << endl;
        m_canvas->setClearColor(Color3f(0.2, 0.2, 0.2));
        RenderNode::lineColor = Color3f(0.9, 0.9, 0.9);
        RenderNode::diffuseColor = Color3f(0.75, 0.75, 0.75);
        RenderNode::paperMode = false;
        Refresh(false);
        break;

      case 'd': case 'D': 
        cout << "Enabling debug" << endl;
        Context::current()->set_flag("aa_disable_debug", false);
        break;

      case 'e': m_domain_eps *= 0.1; cout << "GO domain epsilon=" << m_domain_eps << endl; break;
      case 'E': m_domain_eps *= 10; cout << "GO domain epsilon=" << m_domain_eps << endl; break;

      case 'o': case 'O':
        cout << "Enabling op handler debug" << endl;
        Context::current()->set_flag("aho_disable_debug", false);
        break;
        

      case 'r': case 'R':
        cout << "Resetting view" << endl;
        m_canvas->ResetView();
        m_canvas->SetupView();
        Refresh(false);
        break;

      case 'x': case 'X':
      case 'y': case 'Y':
      case 'z': case 'Z': {
            char axis = event.GetKeyCode();
            float ax = axis == 'x' ? 1 : axis == 'X' ? -1 : 0;
            float ay = axis == 'y' ? 1 : axis == 'Y' ? -1 : 0;
            float az = axis == 'z' ? 1 : axis == 'Z' ? -1 : 0;
            RenderNode::RenderNode::camera.rotate(90.0, ax, ay, az); 
            m_canvas->SetupView();
            Refresh(false);
            break;
        }

      case 'u':
        Context::current()->set_flag("aa_enable_um", !Context::current()->get_flag("aa_enable_um"));
        cout << "UM enabled = " << Context::current()->get_flag("aa_enable_um") << endl;

        break;

      case 't':
        RenderNode::twodRotation = !RenderNode::twodRotation; 
        cout << "2D rotation set to " << RenderNode::twodRotation << endl;
        break;

      case '1': case  '2': case '3': case'4': case '5': case '6': case '7': case '8': case '9':
        {
          StackOp op(ID_CONSTANT, event.GetKeyCode() - '0'); 
          if(event.GetKeyCode() == '1') op.setFloat(M_PI);
          if(handleStackOp(op)) { /* opening and saving are left out */ 
            m_history.push_back(op);
            updateHistoryMap();
          }
        }
        break;

      case '!': case  '@': case '#': case'$': case '%': case '^': case '&': case '*': case '(':
        {
          StackOp op(ID_CONSTANT, 1.0f); 
          string nummap = "!@#$%^&*()";
          for(size_t i = 0; i < 9; ++i) {
            if(nummap[i] == event.GetKeyCode()) {
              op.setFloat(powf(0.5f, i)); 
              break;
            }
          }
          if(handleStackOp(op)) { /* opening and saving are left out */ 
            m_history.push_back(op);
            updateHistoryMap();
          }
        }
        break;

      case WXK_DELETE:
       m_stack->top()->dependsOnNothing();
       m_stack->pop();
       popHistory();
       break;
    }
}

GoFrame::StackOp::StackOp(int id, float param) 
  : id(id)
{
  setFloat(param);
}

float GoFrame::StackOp::getFloat() const {
  istringstream pin(param);
  float result;
  pin >> result;
  return result;
}

void GoFrame::StackOp::setFloat(float param) {
  ostringstream pout;
  pout << param;
  this->param = pout.str(); 
}

BEGIN_EVENT_TABLE(GoGLCanvas, wxGLCanvas)
    EVT_SIZE(GoGLCanvas::OnSize)
    EVT_PAINT(GoGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(GoGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(GoGLCanvas::OnMouse)
    EVT_CHAR(GoGLCanvas::OnChar)
END_EVENT_TABLE()

int attribList[] = {
  WX_GL_DOUBLEBUFFER, 1,
  0
};

GoGLCanvas::GoGLCanvas(GoFrame* frame, wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name, attribList),
      do_init(true),
      m_frame(frame),
      m_orthoWidth(10.0)
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
      changed |= m_frame->stack()->selected()->OnMotion(event);
    }
  } else {
    if (event.LeftIsDown()) {
      RenderNode::camera.orbit(cur_x, cur_y, x, y, sz.x, sz.y);
      changed = true;
    }
    if (event.MiddleIsDown()) {
      //RenderNode::camera.move(0, 0, (y - cur_y)/factor);
      float scale = 1 + (y - cur_y) / factor;
      m_orthoWidth /= scale;
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
  m_frame->OnChar(event);
}


void GoGLCanvas::InitGL()
{
    if(!do_init) return;
    do_init = false;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    setBackend(defaultBackend);
    //setBackend("arb");
    //setBackend("gl");
    SetupView();
    //Context::current()->set_flag("aa_disable_debug", false);
    //Context::current()->set_flag("aho_disable_debug", false);
    

    // anti-aliased lines
#if 0
    glEnable (GL_LINE_SMOOTH);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
#endif
    glLineWidth (4.0);

    // Initial values for the uniforms
    ResetView();
    RenderNode::lightPos = Point3f(250.0, 250.0, 500.0);
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

void GoGLCanvas::ResetView() 
{
  RenderNode::camera.reset(); 
  RenderNode::camera.move(0.0, 0.0, -30.0);
  m_orthoWidth = 10.0;
}

void GoGLCanvas::SetupView()
{
  int w, h;
  GetClientSize(&w, &h);
  //float aspect = static_cast<float>(w) / h;
  //RenderNode::camera.glProjection(aspect);
  RenderNode::camera.glOrtho(m_orthoWidth, m_orthoWidth * h / w);
  RenderNode::mv = RenderNode::camera.modelView();
  RenderNode::inv_mv = inverse(RenderNode::mv); 
  RenderNode::mvd = RenderNode::camera.modelViewProjection(Matrix4x4f());
}

void GoGLCanvas::setClearColor(const Color3f& color) {
  glClearColor(color.getValue(0), color.getValue(1), color.getValue(2), 1.0);
  Refresh(false);
}

BEGIN_EVENT_TABLE(GoManVec, wxPanel)
END_EVENT_TABLE()

GoManVec::GoManVec(GoFrame* frame, wxWindow* parent, wxWindowID id,
           const wxPoint& pos, 
           const wxSize& size, 
           const wxString& name) 
  : wxPanel(parent, id, pos, size, wxSUNKEN_BORDER, name), m_frame(frame),
    m_sizer(new wxBoxSizer(wxVERTICAL)), m_selected(0) {
  SetSizer(m_sizer);
}

void GoManVec::push(RenderNodePtr node) {
   //cout << "Adding" << endl;
   m_frame->Refresh(false);
   m_stack.push_back(node);
   m_sizer->Add(node.object(), wxSizerFlags().Expand());
   m_sizer->Layout();
   //cout << "sizer children: " << m_sizer->GetChildren().GetCount() << endl;
   select(node);
}

void GoManVec::pop(int count) {
  if(!m_stack.empty() && m_selected == m_stack.back()) {
    /* note this works just fine because it will use the select(RenderNodePtr) version of the overloaded function */
    select(m_stack.size() <= 1 ? 0 : node(-2)); 
  }
  for(int i = 0; i < count; ++i) {
    m_sizer->Remove(m_stack.back().object());
    m_stack.pop_back();
  }
  m_sizer->Layout();
  m_frame->Refresh(true);
  //cout << "sizer children: " << m_sizer->GetChildren().GetCount() << endl;
}

void GoManVec::render() {
  for(Stack::iterator I = m_stack.begin(); I != m_stack.end(); ++I) {
    (*I)->render();
  }
}
