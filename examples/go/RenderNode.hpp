#ifndef STACKNODE_HPP
#define STACKNODE_HPP

#include <vector>
#include <string>
#include <sh/sh.hpp>
#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>
#include "man.hpp"
#include "Camera.hpp"

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

  /* Reacts to mouse motion.
   *
   * This is called when this node has been selected, and it may optionally
   * react to mouse input 
   *
   * Returns whether something changed */
  virtual bool OnMotion(const wxMouseEvent& e); 

  /* Toggles visual highlight */
  virtual void highlight(bool on=false);

  virtual std::string name() const = 0;

  /* Global parameters common to all stack nodes */
  static Camera camera;
  static SH::Matrix4x4f mv, mvd;
  static SH::Point3f lightPos;
  static SH::Color3f diffuseColor;
  static wxWindow* parentWindow; 

  protected:
    SH::Matrix4x4f m; /* additional model transformation (does not affect function - for separating objects only) */
    wxBoxSizer* m_sizer;
    int cur_x, cur_y;
};
typedef SH::Pointer<RenderNode> RenderNodePtr; 
typedef std::vector<RenderNodePtr> RenderNodeVec;

/* Nodes for ManRenderNodes and HostManRenderNodes */

struct ManRenderNode: public RenderNode {
  /* Sets up a man render node and builds a basic widget (label only) for the node */
  ManRenderNode(const Man& man);
  virtual ~ManRenderNode() {}
  virtual void render();
  const Man& man() { return m_man; }
  Man transformed_man() { 
    ManMatrix mm = m_(m);
    std::cout << "fooyeah!" << std::endl;
    mm.node()->dump("mm");
    Man result = mul<SH::Point3f>(mm, m_man);
    result.node()->dump("tman");
    return result; 
  }
  std::string name() const { return m_man.name(); }

  private:
    void init(); /* initializes shaders and display list */
    void eval_point(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_curve(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_surface(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_function(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);
    void eval_function2d(const SH::TexCoord2f& texcoord, SH::Generic<3, float>& p, SH::Generic<3, float>& n);

    void drawPoint();
    void drawCurve(int n = 10000);
    void drawSurface(int n = 1000);

    const Man m_man;
    GLuint m_display_list;
    bool m_init;

    SH::Program vsh, fsh;
    SH::ProgramSetPtr m_shaders;
};
typedef SH::Pointer<ManRenderNode> ManRenderNodePtr;

/* Function node that keeps a number of subnodes as arguments */ 
struct FuncRenderNode: public ManRenderNode {
  FuncRenderNode(const Man& man, RenderNodePtr a0): ManRenderNode(man) 
  { m_args.push_back(a0); }

  FuncRenderNode(const Man& man, RenderNodePtr a0, RenderNodePtr a1): ManRenderNode(man) 
  { m_args.push_back(a0); m_args.push_back(a1); }

  FuncRenderNode(const Man& man, RenderNodePtr a0, RenderNodePtr a1, RenderNodePtr a2): ManRenderNode(man) 
  { m_args.push_back(a0); m_args.push_back(a1); m_args.push_back(a2); }

  virtual ~FuncRenderNode() {}
  protected:
    RenderNodeVec m_args;
    static Man toman(RenderNodePtr n) { return SH::shref_dynamic_cast<ManRenderNode>(n)->man(); }
};

/* Some shape op function nodes */
struct ProfileRenderNode: public FuncRenderNode {
  ProfileRenderNode(RenderNodePtr cross, RenderNodePtr profile)
    : FuncRenderNode(m_profile(toman(cross), toman(profile)), cross, profile) {}
};

struct WireRenderNode: public FuncRenderNode {
  WireRenderNode(RenderNodePtr cross, RenderNodePtr wire)
    : FuncRenderNode(m_wire(toman(cross), toman(wire)), cross, wire) {}
};

struct HostManRenderNode: public ManRenderNode { 
  HostManRenderNode(const HostMan& man): ManRenderNode(man), m_hostman(man) {} 
  HostMan m_hostman;
};

#endif
