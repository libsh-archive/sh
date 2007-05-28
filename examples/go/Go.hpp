/* Derived from the wxWindows penguin example */

#ifndef SH_GO_H 
#define SH_GO_H 


#include <string>
#include <vector>
#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>

#include <wx/glcanvas.h>
#include "RenderNode.hpp"
#include "Camera.hpp"


/* Define a new application type */
class GoApp: public wxApp
{
  public:
    bool OnInit();
};

/* Define a new frame type */
class GoGLCanvas;
class GoManVec;

class GoFrame: public wxFrame
{
  public:
    GoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnButton( wxCommandEvent &event );
    void OnExit(wxCommandEvent& event);
    GoManVec* stack() { return m_stack; }

private:
    //wxMenu* m_fileMenu;
    //wxMenuBar* m_menuBar;
    wxSplitterWindow* m_split;
      wxPanel* m_controlsPanel; 
        wxSizer* m_controlsSizer;
        
          wxBoxSizer* m_generatorBox;
            wxButton* m_openSvgBtn;
            wxButton* m_openSurfaceBtn;
            wxButton* m_pointBtn; 
            wxButton* m_lineBtn; 
            wxButton* m_planeBtn; 
            wxButton* m_circleBtn; 
            wxButton* m_sinBtn; 

          wxBoxSizer* m_curveopBox;
            wxButton* m_profileBtn;
            wxButton* m_wireBtn;

          wxBoxSizer* m_funcopBox;
            wxButton* m_addBtn;
            wxButton* m_mulBtn;
            wxButton* m_distanceBtn;
            wxButton* m_plotdistBtn;

        GoManVec* m_stack;
      GoGLCanvas *m_canvas;
DECLARE_EVENT_TABLE()
};


class GoGLCanvas: public wxGLCanvas
{
public:
    GoGLCanvas(GoFrame* frame, wxWindow *parent, wxWindowID id = wxID_ANY,
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

    bool do_init;
    int cur_x, cur_y; // mouse position

private:
    GoFrame* m_frame;

    DECLARE_EVENT_TABLE()
};

/* Holds the stack of parametric functions and operators */
class GoManVec: public wxPanel {
  public:
   GoManVec(GoFrame* frame, wxWindow* parent, wxWindowID id = wxID_ANY, 
           const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, 
           const wxString& name = wxT("panel"));

   /* Pushes on a stack node and builds a corresponding widget */
   void push(RenderNodePtr node); 

   void pop(int count=1);

   RenderNodePtr node(int i) { return m_stack.at(i >= 0 ? i : m_stack.size() + i); } 
   bool empty() { return m_stack.empty(); }

   /* Renders stack */
   void render();

  private:
    GoFrame* m_frame;
    typedef std::vector<RenderNodePtr> Stack;
    Stack m_stack;
    wxBoxSizer* m_sizer;

    DECLARE_EVENT_TABLE()
};

#endif 

