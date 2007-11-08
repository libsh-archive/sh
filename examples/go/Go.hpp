/* Derived from the wxWindows penguin example */

#ifndef GO_H 
#define GO_H 


#include <string>
#include <vector>
#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>
#include <wx/splitter.h>

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
    void OnChar( wxKeyEvent& event);
    GoManVec* stack() { return m_stack; }

private:
    friend class GoApp;

    /* domain eps threshold for future global optimization runs */ 
    float m_domain_eps;

    /* State - used for serialization of current configuration */
    struct StackOp {
      int id;
      std::string param; /* encoded parameters */
      StackOp() {}
      StackOp(int id, const std::string& param): id(id), param(param) {}
      StackOp(int id, float param);
      float getFloat() const;
      void setFloat(float param);
    };
    typedef std::vector<StackOp> StackHistory;
    StackHistory m_history;

    /* For each i in 0...m_stack.size() - 1, this stores the most recent
     * history index j where the stack was that size.
     * (so the indices are in strictly increasing order) 
     *
     * This way node i on the current stack corresponds to the sequence of ops 
     * between historyMap[i - 1] and historyMap[i] in the history stack. 
     */ 
    std::vector<int> historyMap; 

    /* Returns whether op should be pushed on stack (opening of stack history files for example is ignored, since recording those in history can lead to paranormal behaviour) */
    bool handleStackOp(const StackOp& op); 
    void updateHistoryMap();

    /* Uses history map to roll back after a stack popping event */
    void popHistory(); 

    /* Saves the whole stack history into filename, and saves transformation state of all nodes
     * on the stack */ 
    void save(const std::string& filename);

    /* Appends the stack ops in the file onto the current stack, and loads any saved transformations */  
    void load(const std::string& filename);

    //wxMenu* m_fileMenu;
    //wxMenuBar* m_menuBar;
    wxSplitterWindow* m_split;
      wxPanel* m_controlsPanel; 
        wxSizer* m_controlsSizer;
        
          wxBoxSizer* m_generatorBox;
            wxButton* m_openObjectBtn;
            wxButton* m_saveObjectBtn;
            wxButton* m_pointBtn; 
            wxButton* m_lineBtn; 
            wxButton* m_planeBtn; 
            wxButton* m_circleBtn; 
            wxButton* m_sphereBtn; 

          wxBoxSizer* m_curveopBox;
            //wxButton* m_offset2dBtn;
            wxButton* m_lerpBtn;
            wxButton* m_profileBtn;
            wxButton* m_wireBtn;

          wxBoxSizer* m_funcopBox;
            wxGridSizer* m_basicopSizer;
              wxButton* m_addBtn;
              wxButton* m_subBtn;
              wxButton* m_mulBtn;
              wxButton* m_sinBtn; 
              wxButton* m_squareBtn;
              wxButton* m_swiz0Btn;
              wxButton* m_swiz1Btn; 
            wxButton* m_noiseBtn;
            wxButton* m_distanceBtn;
            wxButton* m_plotdistBtn;

          wxBoxSizer* m_paramBox;
          wxGridSizer* m_paramSizer;
            wxButton* m_x0Btn;
            wxButton* m_x1Btn;
            wxButton* m_x2Btn;
          
          wxBoxSizer* m_transBox;
          wxGridSizer* m_transSizer;
            wxButton* m_rotxBtn;
            wxButton* m_rotyBtn;
            wxButton* m_rotzBtn;
            wxButton* m_transxBtn;
            wxButton* m_transyBtn;
            wxButton* m_transzBtn;

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
    void ResetView();
    void SetupView();
    void setClearColor(const SH::Color3f& color);

    bool do_init;
    int cur_x, cur_y; // mouse position

private:
    GoFrame* m_frame;
    float m_orthoWidth; 

    DECLARE_EVENT_TABLE()
};

/* Holds the stack of parametric functions and operators, and maintains which one is
 * currently active for manipulation in the UI */
class GoManVec: public wxPanel {
  public:
   GoManVec(GoFrame* frame, wxWindow* parent, wxWindowID id = wxID_ANY, 
           const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxDefaultSize, 
           const wxString& name = wxT("panel"));

   /* Pushes on a stack node, builds a corresponding widget, and selects result*/
   void push(RenderNodePtr node); 
   void pop(int count=1);
   

   /* Select a node */
   void select(int i) { select(node(i)); }
   void select(RenderNodePtr node) { 
     if(m_selected) m_selected->highlight(false);
     m_selected = node;
     if(m_selected) m_selected->highlight(true);
     Refresh(false);
   }
    
   RenderNodePtr selected() { return m_selected; }
   RenderNodePtr node(int i) { return m_stack.at(i >= 0 ? i : m_stack.size() + i); } 
   RenderNodePtr top() { return node(-1); }
   bool empty() { return m_stack.empty(); }
   size_t size() { return m_stack.size(); }

   /* Renders stack */
   void render();

  private:
    GoFrame* m_frame;
    wxBoxSizer* m_sizer;
    typedef std::vector<RenderNodePtr> Stack;
    Stack m_stack;
    RenderNodePtr m_selected;

    DECLARE_EVENT_TABLE()
};

#endif 

