#include "Pass.hpp"
#include "ShProgramNode.hpp"
#include "ShContext.hpp"
#include "ShRefCount.hpp"

#include <set>
#include <map>

#define RDS_DEBUG

#ifdef RDS_DEBUG
#include <iostream>
#endif

using namespace SH;

Pass::Pass(DAGNode* node, std::string target)
{
  m_prog = new ShProgramNode(target);
  ShCtrlGraphNodePtr start = new ShCtrlGraphNode();
  start->block = new ShBasicBlock();
  start->block->m_statements = node->get_statements();
#ifdef RDS_DEBUG
  start->block->print(std::cout,2);
#endif
  m_prog->ctrlGraph = new ShCtrlGraph(start, start);
  alloc_shared_mem(start->block);
}

void Pass::alloc_shared_mem(ShBasicBlockPtr bb) {

  // We need to turn shared temporaries into inputs/outputs

  std::set<ShVariableNode*> handled_output;
  std::set<ShVariableNode*> handled_input;

  typedef std::list<std::pair<ShVariableNode*, ShVariableNode*> > TempList;
  TempList output_temps, input_temps;
  
  ShContext::current()->enter(m_prog);
    
  for (ShBasicBlock::ShStmtList::iterator I = bb->begin();
       I != bb->end(); ++I) {
    if (I->dest.node()) {
      ShVariableNode* vnode = I->dest.node().object();
      if (vnode->kind() == SH_TEMP
          && handled_output.find(vnode) == handled_output.end()) {
        // Need to add an output for this node
        ShVariableNodePtr outnode = new ShVariableNode(SH_STREAM,
                                                       vnode->size(),
                                                       vnode->valueType());
      
        {
          std::ostringstream os;
          os << "T[" << vnode->name() << "]_sout";
          outnode->name(os.str());
        }
        handled_output.insert(vnode);
        output_temps.push_back(std::make_pair(vnode, outnode.object()));
      } else if (vnode->kind() == SH_OUTPUT
                 && handled_output.find(vnode) == handled_output.end()) {
        
        handled_output.insert(vnode);
        m_prog->outputs.push_back(vnode);
      }
    }
    
    for (int i = 0; i < opInfo[I->op].arity; i++) {
      if (!I->src[i].node()) continue;
      ShVariableNode* vnode = I->src[i].node().object();

      if (vnode->kind() == SH_TEMP
          && handled_input.find(vnode) == handled_input.end() ) {

        // Need to add an input for this node
        ShVariableNodePtr innode = new ShVariableNode(SH_STREAM,
                                                      vnode->size(),
                                                      vnode->valueType());
        {
          std::ostringstream os;
          os << "T[" << vnode->name() << "]_sin";
          innode->name(os.str());
        }
        handled_input.insert(vnode);
        input_temps.push_back(std::make_pair(vnode, innode.object()));
      } else if (vnode->kind() == SH_INPUT
                 && handled_input.find(vnode) == handled_input.end()) {

 /*
        ShTextureNodePtr tex;
        ShTextureTraits traits = ShArrayTraits();
        traits.clamping(ShTextureTraits::SH_UNCLAMPED);
        tex = new ShTextureNode(SH_TEXTURE_2D, I->first->size(),
                                I->first->valueType(), traits, tex_size,
                                tex_size, 1, count);*/

        handled_input.insert(vnode);
      }
    }
  }

  // Add the assignment statements connecting the temporaries to inputs/outputs.
  for (TempList::const_iterator I = input_temps.begin(); I != input_temps.end(); ++I){
    ShVariable dst(I->first);
    ShChannelNodePtr str = new ShChannelNode(I->first->specialType(),
                                             dst.size(),dst.valueType());
    ShVariable src(str);
    bb->prependStatement(ShStatement(dst, SH_OP_FETCH, src));
  }
  /*
  for (TempList::const_iterator I = output_temps.begin(); I != output_temps.end(); ++I){
    bb->addStatement(ShStatement(ShVariable(I->second), SH_OP_ASN,
                                              ShVariable(I->first)));
  }*/

  m_prog->collectVariables();
  
  /*
  passes.push_back(pass);
  node_to_pass[node.object()] = &passes.back();
  pass_to_node[&passes.back()] = node.object();*/
  
  ShContext::current()->exit();

#ifdef RDS_DEBUG
  /*
  ShProgramNode::VarList::const_iterator I = m_prog->inputs_begin();
  for (int i=0; i<I.size(); ++i)
    std::cout << "Input " << I[i]->name() << std::endl;*/
  std::cout << m_prog->describe_interface() << std::endl;
  bb->print(std::cout,2);
  /*
  for (ShProgramNode::VarList::const_iterator I = m_prog->inputs_begin(); 
       I != m_prog->inputs_end(); ++I) {
    std::cout << "Input here " << (*I)->name() << std::endl;
  }*/
#endif
}
