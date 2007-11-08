#ifndef SHADERGRAPH_HPP
#define SHADERGRAPH_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include "uish.hpp"
#include "scanner.hpp"

/* frame containing a subgraph */
class ShaderGraphSection: public uishFrame {
  public:
    ShaderGraphSection(Scanner& scanner, SH::ShSectionNodePtr section);

  protected:
    Scanner& m_scanner;
    SH::ShSectionNodePtr m_section;

    uishFramePtr m_frame; 
    uishPackLayoutPtr m_pack;
      uishLabelPtr m_sectionLabel; 
      uishLabelPtr m_sectionError;

      uishFixedFramePtr m_graphFrame;
        uishGraphLayoutPtr m_graph;
};
typedef SH::ShPointer<ShaderGraphSection> ShaderGraphSectionPtr; 

/* frame containing a pack of statements */  
class ShaderGraphCfgNode: public uishFrame {
  public:
    ShaderGraphCfgNode(Scanner& scanner, SH::ShCtrlGraphNodePtr node);

  protected:
    Scanner& m_scanner;
    SH::ShCtrlGraphNodePtr m_node;

    uishPackLayoutPtr m_pack;
};
typedef SH::ShPointer<ShaderGraphCfgNode> ShaderGraphCfgNodePtr;

/* frame containing a single statement */
class ShaderGraphStmt: public uishFrame { 
  public:
    ShaderGraphStmt(Scanner& scanner, SH::ShStatement& stmt);

  protected:
    Scanner& m_scanner;
    SH::ShStatement& m_stmt;

    uishPackLayoutPtr m_pack;
      uishLabelPtr m_label;
      uishLabelPtr m_error;
};
typedef SH::ShPointer<ShaderGraphStmt> ShaderGraphStmtPtr; 


// A treeview representation of a shader 
class ShaderGraph : public uishBase { 
  public:
    ShaderGraph(Scanner& scanner);
    virtual ~ShaderGraph() {}

    void update();

  protected:
    Scanner& m_scanner;

};

#endif

