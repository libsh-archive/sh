#ifndef STACKNODE_HPP
#define STACKNODE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>
#include "man.hpp"
#include "Camera.hpp"

struct GoManVec; // forward declaration

/* A node for composing parametric functions 
 *
 * Each node encapsulates the following
 *  - the description of the parametric function it represents
 *  - how to render it in the 3D display
 *  - how to render it in the 2D UI
 *    - each node returns a panel 
 *
 * The basic RenderNode automatically builds a wxBoxSizer
 * to stick widgets into.
 */
struct RenderNode: public wxPanel, SH::RefCountable { 
  RenderNode();
  virtual ~RenderNode() {}

  /* Renders the stack node.
   * Node sets up its own transformation and shaders */
  virtual void render() = 0;

  /** Called when the stack item has been clicked */
  virtual void OnMouse(wxMouseEvent& event );

  /* Reacts to mouse motion.
   *
   * This is called when this node has been selected, and it may optionally
   * react to mouse input 
   *
   * Returns whether something changed */
  virtual bool OnMotion(const wxMouseEvent& e); 

  /* Links this to another render node (whenever uniform updates are not enough)
   * (e.g. if updating some uniforms requires re-running an optimization).
   *
   * When the parameters in the other node are changed, this->update() is called */
  void dependsOn(SH::Pointer<RenderNode> other); 

  /* Removes links to everything this depends on */ 
  void dependsOnNothing();  

  virtual void update() {}

  /* Toggles visual highlight */
  virtual void highlight(bool on=false);

  virtual std::string name() const = 0;

  /* Saves node state (currently just the model transformation) */ 
  virtual void saveState(std::ostream& out);
  virtual void readState(std::istream& in);

  /* Global parameters common to all stack nodes */
  static Camera camera;
  static SH::Matrix4x4f inv_mv, mv, mvd;
  static SH::Point3f lightPos;
  static SH::Color3f diffuseColor;
  static SH::Color3f lineColor;
  static SH::Color3f globalMinColor, minColor, midColor, maxColor; /* tri-color ramp used for distance plots */ 
  static bool paperMode;
  static GoManVec* parentWindow;
  static wxWindow* canvasWindow;
  static bool twodRotation; /* only rotate around current view axis */ 

  protected:
    /* hmm.. this probably isn't necessary with uniform lifting */
    SH::Matrix4x4f m_mv, m_mvd; /* modelview matrix, modelview-project matrix based on my m */

    SH::Matrix4x4f m; /* additional model transformation (does not affect function - for separating objects only) */
    wxBoxSizer* m_sizer;
    int cur_x, cur_y;

    bool m_changed;
    typedef std::vector<SH::Pointer<RenderNode> > NodeVec;
    NodeVec m_dependents; /* umm make sure nobody builds cyclic dependencies... */
    NodeVec m_parents; /* Things this depends on */

    DECLARE_EVENT_TABLE()
};
typedef SH::Pointer<RenderNode> RenderNodePtr; 
typedef std::vector<RenderNodePtr> RenderNodeVec;

/* Nodes for ManRenderNodes and HostManRenderNodes */

struct ManRenderNode: public RenderNode {
  /* Sets up a man render node and builds a basic widget (label only) for the node */
  ManRenderNode(const Man& man, size_t sampleRate=0);
  virtual ~ManRenderNode() {}
  virtual void render();
  const Man& man() { return m_man; }
  Man transformed_man() { 
    ManMatrix mm = m_(m);
    return mm.mul<SH::Point3f>(m_man).name(name());
  }
  std::string name() const { return m_man.name(); }

  private:
    void init(); /* initializes shaders and display list */
    void eval_point(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_curve(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_surface(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_function(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_function2d(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);

    void eval_curve_color(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& color, SH::Generic<3, float>& n);
    void eval_surface_color(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& color, SH::Generic<3, float>& n);

    void drawPoint();
    void drawCurve();
    void drawSurface();

    const Man m_man;
    GLuint m_display_list;
    bool m_init;
    size_t m_sampleRate;
    static const int DEFAULT_SAMPLERATE = 500;

    SH::Program vsh, fsh;
    SH::ProgramSetPtr m_shaders;
};
typedef SH::Pointer<ManRenderNode> ManRenderNodePtr;

/* Function node that keeps a number of subnodes as arguments */ 
struct FuncRenderNode: public ManRenderNode {
  FuncRenderNode(const Man& man, RenderNodePtr a0, size_t sampleRate=0): ManRenderNode(man, sampleRate) 
  { m_args.push_back(a0); }

  FuncRenderNode(const Man& man, RenderNodePtr a0, RenderNodePtr a1, size_t sampleRate=0): ManRenderNode(man, sampleRate) 
  { m_args.push_back(a0); m_args.push_back(a1); }

  FuncRenderNode(const Man& man, RenderNodePtr a0, RenderNodePtr a1, RenderNodePtr a2, size_t sampleRate=0): ManRenderNode(man, sampleRate) 
  { m_args.push_back(a0); m_args.push_back(a1); m_args.push_back(a2); }

  virtual ~FuncRenderNode() {}
  protected:
    RenderNodeVec m_args;
    static Man toman(RenderNodePtr n) { return SH::shref_dynamic_cast<ManRenderNode>(n)->man(); }
    static Man tomant(RenderNodePtr n) { return SH::shref_dynamic_cast<ManRenderNode>(n)->transformed_man(); }
};

/* Some shape op function nodes */
struct ProfileRenderNode: public FuncRenderNode {
  ProfileRenderNode(RenderNodePtr cross, RenderNodePtr profile, size_t sampleRate=0)
    : FuncRenderNode(m_profile(toman(cross), toman(profile)), cross, profile, sampleRate) {}
};

struct WireRenderNode: public FuncRenderNode {
  WireRenderNode(RenderNodePtr cross, RenderNodePtr wire, size_t sampleRate=0)
    : FuncRenderNode(m_wire(toman(cross), toman(wire)), cross, wire, sampleRate) {}
};

struct HostManRenderNode: public ManRenderNode { 
  HostManRenderNode(const HostMan& man, size_t sampleRate=0): ManRenderNode(man, sampleRate), m_hostman(man) {} 
  virtual void update(); 
  HostMan m_hostman;
};

#endif
