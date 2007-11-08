#include <sstream>
#include "shaderGraph.hpp"

using namespace SH;

ShaderGraphSection::ShaderGraphSection(Scanner& scanner, SH::ShSectionNodePtr section) 
  : m_scanner(scanner), m_section(section)
{
  std::cerr << "Handling Section " << section->name() << " {" << std::endl; 
  m_frame = new uishFrame(); {
    m_pack = new uishPackLayout(uishPackLayout::VERTICAL); {
      m_sectionLabel = new uishLabel(section->name());
      m_pack->addChild(m_sectionLabel);

      ShStatement* endStmt = section->getEnd();
      if(endStmt) {
        std::ostringstream out;
        m_sectionError = new uishLabel(out.str());
        m_sectionError->set_background(uishColor::LightRed);
        m_pack->addChild(m_sectionError);
      }

      
      m_graphFrame = new uishFixedFrame(100, 100); {
        m_graph = new uishGraphLayout(); {

          ShSectionNode::iterator S = section->begin(); 
          // associate data flow with edges
          for(; S != section->end(); ++S) {
            m_graph->addChild(new ShaderGraphSection(scanner, *S));
          }

          typedef std::map<ShCtrlGraphNodePtr, ShaderGraphCfgNodePtr> CfgMap;
          CfgMap cfgmap; 

          ShSectionNode::cfg_iterator C = section->cfgBegin();
          for(; C != section->cfgEnd(); ++C) {
            if((*C)->block && !(*C)->block->empty()) {
              cfgmap[*C] = new ShaderGraphCfgNode(scanner, *C);
              m_graph->addChild(cfgmap[*C]); 
            }
          }

          for(; C != section->cfgEnd(); ++C) {
            if((*C)->follower && cfgmap.find((*C)->follower) != cfgmap.end()) {
              m_graph->addEdge(cfgmap[*C], cfgmap[(*C)->follower]);
            }
          }

        }
        m_graph->name(section->name() + "_graph");
      }
      m_graphFrame->name(section->name() + "_graphFrame");
      
      if(section->isRoot()) {
        m_pack->addChild(m_graph);
      } else {
        m_graphFrame->addChild(m_graph);
        m_pack->addChild(m_graphFrame);
      }
    } 
    m_pack->name(section->name() + "_pack");
    m_frame->addChild(m_pack);
  }
  m_frame->name(section->name() + "_frame");
  m_frame->box()->set_background(uishColor::LightBlue);
  addChild(m_frame);

  name(section->name());

  std::cerr << "} Done Section " << section->name() << std::endl; 
}

ShaderGraphCfgNode::ShaderGraphCfgNode(Scanner& scanner, SH::ShCtrlGraphNodePtr node) 
  : m_scanner(scanner), m_node(node)
{
  std::cerr << "  Handling cfg node " << std::endl; 
  m_pack = new uishPackLayout(uishPackLayout::VERTICAL);
  if(!node->block) return; 
  for(ShBasicBlock::iterator I = node->block->begin();
      I != node->block->end(); ++I) {
    m_pack->addChild(new ShaderGraphStmt(scanner, *I));
  }

  m_pack->name("cfg_pack");
  addChild(m_pack);

  name("cfg node");

  std::cerr << "  Done cfg node " << std::endl; 
}

ShaderGraphStmt::ShaderGraphStmt(Scanner& scanner, ShStatement& stmt)
  : m_scanner(scanner), m_stmt(stmt)
{
  std::cerr << "    Handling stmt " << stmt << std::endl; 

  m_pack = new uishPackLayout(uishPackLayout::HORIZONTAL);

  std::ostringstream out;
  out << stmt;
  m_label = new uishLabel(out.str());
  m_label->borderSize(0);
  m_pack->addChild(m_label);

  std::ostringstream out2;
  out2 << m_scanner.getError(m_stmt);
  m_error = new uishLabel(out2.str());
  m_error->set_background(uishColor::LightRed);
  m_pack->addChild(m_error);

  box()->borderSize(0);
  addChild(m_pack);
  name(out.str());
}

ShaderGraph::ShaderGraph(Scanner& scanner) 
  : m_scanner(scanner)
{}

void ShaderGraph::update() {

  ShSectionTree& stree = m_scanner.getSectionTree();

  m_root = new ShaderGraphSection(m_scanner, stree.root);
  enterGL();
  m_root->realize();
  m_root->resize(uishResizeEvent(get_width(), get_height()));
  leaveGL();
  invalidate();
}
