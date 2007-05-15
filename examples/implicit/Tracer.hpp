/* Derived from the wxWindows penguin example */

#ifndef SH_TRACER_H 
#define SH_TRACER_H 


#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>

#include <wx/glcanvas.h>
#include "Camera.hpp"
#include "PlotFunc.hpp"

enum DebugMode {
  DEBUG_START,
  NORMAL, // normal plotter
  FUNC, // shows function value at the midplane perpendicular to the viewer in the sphere
  HIT,  // shows hit/nohit, distance to hit
  GRAD, // shows normalized gradients at hitpoints
  PLOT, // plot evaluation at current loop cutoff and debug point  
  TRACE, // trace computation at a fixed point
  LOOP, // shows loop count
  CUTOFF, // cuts off after a fixed loop count
  DEBUG_END // marker
};

enum RangeMode {
  AA, /* affine arithmetic */
  AA_NOUC, /* affine arithmetic, no unique condensation */
  IA /* interval arithmetic */
}; 

class TracerFrame;
class TracerGLCanvas;

/* Define a new application type */
class TracerApp: public wxApp
{
  public:
    bool OnInit();

  private:
    TracerFrame* m_frame; 
};


class TracerFrame: public wxFrame
{
  public:
    TracerFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void initParams();


    void OnCharAll( wxKeyEvent& event);
    void MouseAll(wxMouseEvent& event, const wxSize& sz);
    void RefreshAll(); 

    /* some presets */
    void OnPreset(wxCommandEvent& event);
    void ShowSpheres();
    void ShowTorus();
    void ClearModels();
    void InitShaders(); 

    /* model/view parameters */
    SH::Matrix4x4f mv, inv_mv, mvd;
    SH::Point3f eyeposm;
    SH::Point3f lightPos;
    Camera camera;

    /* tracing parameters */
    Attrib1f level;
    Attrib1f eps;
    Attrib1f traceEps;
    Attrib1f traceDiff;
    Attrib1f loop_cutoff;
    Attrib1f loop_highlight;
    Attrib1f sphereRadius; 
    Attrib1f graphScale;
    Vector2f graphOffset;
    Attrib2f dbgCoords; // debug point in spherical coordinates 
    bool showTiming; 
    bool showAaDebug; 
    RangeMode rangeMode;

    /* the implicit functions */
    Color3f color[9];
    Vector3f trans[9];
    Attrib3f scl[9];
    Attrib1f fscl[9];
    PlotFuncPtr pfs[9];
    int selected; // 0 = no selection (mouse interacts with view), 1-9 means selected object i-1

private:
DECLARE_EVENT_TABLE()
    wxMenu* m_presetMenu;
    wxMenuBar* m_menuBar;
    wxSplitterWindow* m_split;
      wxSplitterWindow* m_rightSplit;
      static const int numCanvas = 3;
      TracerGLCanvas *m_canvas[numCanvas];

    int cur_x, cur_y; // mouse position
};


class TracerGLCanvas: public wxGLCanvas
{
public:
    TracerGLCanvas(DebugMode mode, wxWindow* parent, TracerFrame &tframe, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("TracerGLCanvas"));

    ~TracerGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse( wxMouseEvent& event );
    void OnChar( wxKeyEvent& event);
    void InitGL();
    void SetupView(); 
    void InitShaders();

    /* Sets both OpenGL & Sh contexts to this GL Canvas' */
    void GrabContext();  

    SH::Program vsh, fsh;
    SH::ProgramSetPtr shaders;

    DebugMode debugMode; 

private:
    DECLARE_EVENT_TABLE()
    bool do_init;
    TracerFrame& m_tframe; 
    Context* m_context;
    int cur_x, cur_y; // mouse position
};

#endif 

