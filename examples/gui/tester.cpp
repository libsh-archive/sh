#include <sstream>
#include "uish.hpp"
#include "shaderGraph.hpp"

#if 0

ShaderGraphSection::ShaderGraphSection(Scanner& scanner, SH::ShSectionNodePtr section) 
  : m_scanner(scanner), m_section(section)
{
  m_graph = new uishGraphLayout();

  ShSectionNode::iterator S = section->begin(); 
  // associate data flow with edges
  for(; S != section->end(); ++S) {
    m_graph->addChild(new ShaderGraphSection((*S)));
  }

  ShSectionNode::cfg_iterator C = section->cfg_begin();
  for(; C != section->cfg_end(); ++C) {
    m_graph->addChild(new ShaderGraphCfgNode(*C));
  }
}

ShaderGraphCfgNode::ShaderGraphCfgNode(Scanner& scanner, SH::ShCtrlGraphNodePtr node) 
  : m_scanner(scanner), m_node(node)
{
  m_pack = new uishPackLayout(uishPackLayout::VERTICAL);
  if(!node->block) return; 
  for(ShBasicBlock::iterator I = node->block->begin();
      I != node->block->end(); ++I) {
    m_pack->addChild(new ShaderGraphStmt(*I));
  }
}

ShaderGraphStmt::ShaderGraphStmt(Scanner& scanner, SH::Statement& stmt)
  : m_scanner(scanner), m_stmt(stmt)
{
  m_pack = new uishPackLayout(uishPackLayout::HORIZONTAL);

  std::ostringstream out;
  out << stmt;
  m_label = new uishLabel(out.str());
  m_pack->addChild(m_label);

  out.clear();
  out << m_scanner.getError(m_stmt);
  m_error = new uishLabel(out.str());
  m_pack->addChild(m_error);
}
#endif

ShaderGraph::ShaderGraph(Scanner& scanner) 
  : m_scanner(scanner)
{
#if 0
  //m_root = new uishBox(10, 10); 
  
#endif 

#if 0
  m_root = new uishLabel("hello");
#endif

  uishPackLayoutPtr m_packfoo = new uishPackLayout(uishPackLayout::VERTICAL);
  uishPackLayoutPtr m_pack2foo = new uishPackLayout(uishPackLayout::HORIZONTAL);

  m_packfoo->addChild(new uishLabel("hello"));
  m_packfoo->name("m_packfoo");
  m_pack2foo->name("m_pack2foo");
  m_packfoo->addChild(new uishBox(50, 10));
  m_packfoo->addChild(new uishLabel("hello2"));

  m_packfoo->addChild(m_pack2foo);
    m_pack2foo->addChild(new uishBox(10, 10));
    m_pack2foo->addChild(new uishBox(20, 10));
    m_pack2foo->addChild(new uishBox(30, 10));
    m_pack2foo->addChild(new uishLabel("yeah"));

#if 1
  uishFixedFramePtr m_fixed(new uishFixedFrame(50, 50));
    m_fixed->name("fixed frame");
  std::cerr << "fixed requests " << m_fixed->width_request() << " " 
    << m_fixed->height_request() << std::endl;
     uishBoxPtr m_fixedbox(new uishBox(50, 100)); 
     m_fixedbox->name("fixedbox");
     m_fixed->addChild(m_fixedbox);
     m_fixed->addChild(new hidingLabel("toggle yeah"));
     m_fixed->addChild(new hidingLabel("toggle bleah bleah bleah"));
  std::cerr << "fixed requests " << m_fixed->width_request() << " " 
    << m_fixed->height_request() << std::endl;
 // m_root = m_fixed;
#endif

#if 1
   uishPackLayoutPtr m_pack = new uishPackLayout(uishPackLayout::VERTICAL); { m_pack->name("m_pack");

     uishGraphLayoutPtr m_graph = new uishGraphLayout(); { m_graph->name("m_graph");
       m_graph->set_size_request(0, 400); 
       uishLabelPtr m_hello = new uishLabel("hello"); 
       m_hello->borderSize(2.0);

       uishLabelPtr m_world = new uishLabel("world"); 
       m_world->borderSize(3.0);

       uishLabelPtr m_foo = new uishLabel("poohbear");
       m_foo->borderSize(2.0);

       uishBoxPtr m_box = new uishBox(20, 20); m_box->name("m_box");
       //m_graph->addChild(m_hello);
       //m_graph->addChild(m_world);
       //m_graph->addChild(m_foo);
       //m_graph->addChild(m_box);
       m_graph->addChild(m_fixed);

       //m_graph->addEdge(m_hello, m_world);
       //m_graph->addEdge(m_world, m_foo);
       //m_graph->addEdge(m_foo, m_hello);
       //m_graph->addEdge(m_world, m_box);

       uishGraphLayoutPtr m_graph2 = new uishGraphLayout(); { m_graph2->name("m_graph2");
         m_graph2->set_size_request(200, 200);

         uishLabelPtr m_foo = new uishLabel("foo"); 
         m_foo->borderSize(2.0);

         uishLabelPtr m_foo2 = new uishLabel("foo2");
         m_foo2->borderSize(2.0);

         m_graph2->addChild(m_foo);
         m_graph2->addChild(m_foo2);
         m_graph2->addChild(m_packfoo);

         m_graph2->addEdge(m_foo, m_foo2);
         m_graph2->addEdge(m_packfoo, m_foo);

       }
       uishFramePtr m_graph2Frame = new uishFrame();
       m_graph2Frame->addChild(m_graph2);
       m_graph->addChild(m_graph2Frame);
       //m_graph->addEdge(m_hello, m_graph2);
     }

     m_pack->addChild(new uishLabel("foo"));
     m_pack->addChild(m_graph);
   }
   m_root = m_pack; 
#endif
}

void ShaderGraph::update() {
  invalidate();
}
