/* Derived from the wxWindows penguin example */

#ifndef SH_GO_H 
#define SH_GO_H 


#include <string>
#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>

#include <wx/glcanvas.h>
#include "Camera.hpp"


/* Define a new application type */
class GoApp: public wxApp
{
  public:
    bool OnInit();
};

/* Define a new frame type */
class GoGLCanvas;

class GoFrame: public wxFrame
{
  public:
    GoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnButton( wxCommandEvent &event );
    void OnExit(wxCommandEvent& event);

private:
    //wxMenu* m_fileMenu;
    //wxMenuBar* m_menuBar;
    wxSplitterWindow* m_split;
      wxPanel* m_controlsPanel; 
        wxSizer* m_controlsSizer;
        
        wxSizer* m_buttonSizer;
          wxButton* m_openSvgBtn;
          wxButton* m_openSurfaceBtn;
          wxButton* m_profileBtn;
          wxButton* m_wireBtn;
          wxButton* m_offsetBtn;
          wxButton* m_distanceBtn;
        wxScrolledWindow* m_stackWnd;
      GoGLCanvas *m_canvas;
DECLARE_EVENT_TABLE()
};


class GoGLCanvas: public wxGLCanvas
{
public:
    GoGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("GoGLCanvas"));

    ~GoGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse( wxMouseEvent& event );
    void OnChar( wxKeyEvent& event);
    void InitGL();
    void SetupView();
    void InitShaders();

    bool do_init;

    SH::Matrix4x4f mv, mvd;
    SH::Point3f lightPos;
    SH::Program vsh, fsh;
    SH::ProgramSetPtr shaders;
    Camera camera;

    SH::Color3f diffusecolor;

    int cur_x, cur_y; // mouse position

private:
    DECLARE_EVENT_TABLE()
};

/* Holds the stack of parametric functions and operators */
class GoStack: public wxScrolledWindow {
   GoStack(wxWindow* parent, wxWindowID id = wxID_ANY, 
           const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, 
           const wxString& name = wxT("scrolledWindow"));

  private:
    DECLARE_EVENT_TABLE()
};

#endif 

