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
  m_bb = start->block = new ShBasicBlock();
  SH_DEBUG_PRINT("Adding statements from node" << node);
  m_bb->m_statements = node->get_statements();
  SH_DEBUG_PRINT("Got statements" << node);
#ifdef RDS_DEBUG
  m_bb->print(std::cout,2);
  node->cuts();
#endif
  m_prog->ctrlGraph = new ShCtrlGraph(start, start);
  this->target = new ShStream();
}

void Schedule::make_channel(ShVariableNodePtr v)
{
  if (channels.find(v) != channels.end()) return;

  channels[v] = new ShChannelNode(v->specialType(),v->size(),v->valueType());
#ifdef RDS_DEBUG
  std::cout << "Made channel for " << v->name() << std::endl;
#endif
}

Schedule::Schedule(RDS::PassVector p, RDS::VarVector v)
{
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Making passes");
#endif
  for (RDS::PassVector::iterator I = p.begin(); I != p.end(); ++I) {
    m_passes.push_back(new Pass(*I,"gpu:stream"));
  }

#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Making channels/allocing memory");
#endif
  for (RDS::VarVector::iterator I = v.begin(); I != v.end(); ++I) {
    make_channel( (*I)->node().object() );
  }

#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Adding instructions");
#endif
  int i = 0;
  for (std::vector<Pass*>::iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
    (*I)->id = ++i;
    fix_instructions( *I );
  }

}

void Schedule::fix_instructions(Pass* p)
{
  ShContext::current()->enter(p->get_prog());
  ShBasicBlockPtr bb = p->get_bb();

  for (ShBasicBlock::ShStmtList::iterator I = bb->begin(); I != bb->end(); ++I) {
    std::set<ShVariableNodePtr> temp_inputs, temp_outputs;
    temp_inputs.clear(); temp_outputs.clear();

    if (I->dest.node()) {
      ShVariableNode* vnode = I->dest.node().object();
      if (vnode->kind() == SH_TEMP && temp_outputs.find(vnode) == temp_outputs.end()
          && channels.find(vnode) != channels.end() ) {
        temp_outputs.insert(vnode); 
#ifdef RDS_DEBUG
        std::cout << "Shared variable " << vnode->name() 
                  << " defined in pass " << p->id << std::endl;
#endif
      }
    }

    for (int i = 0; i < opInfo[I->op].arity; i++) {
      if (!I->src[i].node()) continue;
      ShVariableNodePtr vnode = I->src[i].node().object();

      if (vnode->kind() == SH_TEMP && temp_inputs.find(vnode) == temp_inputs.end()
          && channels.find(vnode) != channels.end() ) {
        temp_inputs.insert(vnode); 
#ifdef RDS_DEBUG
        std::cout << "Shared variable " << vnode->name() 
                  << " used in pass " << p->id << std::endl;
#endif
      }
    }

    for (std::set<ShVariableNodePtr>::const_iterator I = temp_inputs.begin();
         I != temp_inputs.end(); ++I) {
#ifdef RDS_DEBUG
      std::cout << "Trying to fix " << (*I)->name() << std::endl;
#endif
      ShVariable *dst = new ShVariable(*I);
      ShVariable *src = new ShVariable(channels[*I]);
      bb->prependStatement(ShStatement(*dst, SH_OP_FETCH, *src));
#ifdef RDS_DEBUG
        std::cout << "Added fetch " << dst->name() << " from " << src->name() << std::endl;
#endif
    }

    for (std::set<ShVariableNodePtr>::const_iterator I = temp_outputs.begin();
         I != temp_outputs.end(); ++I) {
#ifdef RDS_DEBUG
      std::cout << "Trying to fix " << (*I)->name() << std::endl;
#endif
      ShVariable *src = new ShVariable(*I);
      ShVariableNodePtr outnode = new ShVariableNode(SH_OUTPUT, (*I)->size(), (*I)->valueType());
      ShVariable *dst = new ShVariable(outnode);
      bb->addStatement(ShStatement(*dst, SH_OP_ASN, *src));
      p->target->append(channels[*I]);
#ifdef RDS_DEBUG
        std::cout << "Added " << (*I)->name() << " to stream" << std::endl;
#endif
    }

  }
  p->get_prog()->collectVariables();
  
  ShContext::current()->exit();
}


/* Dead code lies below */

/*
void Schedule::mk_def_use()
{
  for (std::vector<Pass*>::iterator P = m_passes.begin(); P != m_passes.end(); ++P) {
    ShBasicBlockPtr bb = (*P)->get_bb();
    for (ShBasicBlock::ShStmtList::iterator I = bb->begin(); I != bb->end(); ++I) {

      if (I->dest.node()) {
        ShVariableNodePtr vnode = I->dest.node().object();
        if (def.find(vnode) == def.end()) {
          def[vnode] = *P;
        }
        else {
          def[vnode] = NULL;
#ifdef RDS_DEBUG
          std::cout << vnode->name() << " defined in multiple passes" << std::endl;
#endif
        }

        if (use.find(vnode) != use.end() && (use[vnode] != *P))
          make_channel(vnode);
      }

      for (int i = 0; i < opInfo[I->op].arity; i++) {
        if (!I->src[i].node()) continue;
        ShVariableNodePtr vnode = I->src[i].node().object();
        if (use.find(vnode) == use.end())
          use[vnode] = *P;
        else {
          use[vnode] = NULL;
#ifdef RDS_DEBUG
          std::cout << vnode->name() << " used in multiple passes" << std::endl;
#endif
        }

        if (def.find(vnode) != def.end() && (def[vnode] != *P))
          make_channel(vnode);
      }
    }
  }
}
*/
void Pass::alloc_shared_mem(DAGNode* dag) {

  std::set<ShVariableNode*> handled_output;
  std::set<ShVariableNode*> handled_input;

  typedef std::list<std::pair<ShVariableNode*, ShVariableNode*> > TempList;
  TempList output_temps, input_temps;
  
  ShContext::current()->enter(m_prog);
    
  for (ShBasicBlock::ShStmtList::iterator I = m_bb->begin();
       I != m_bb->end(); ++I) {
    if (I->dest.node()) {
      ShVariableNode* vnode = I->dest.node().object();
      if (vnode->kind() == SH_TEMP
          && handled_output.find(vnode) == handled_output.end()) {
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
    m_bb->prependStatement(ShStatement(dst, SH_OP_FETCH, src));
  }
  /*
  for (TempList::const_iterator I = output_temps.begin(); I != output_temps.end(); ++I){
    m_bb->addStatement(ShStatement(ShVariable(I->second), SH_OP_ASN,
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
  m_bb->print(std::cout,2);
  /*
  for (ShProgramNode::VarList::const_iterator I = m_prog->inputs_begin(); 
       I != m_prog->inputs_end(); ++I) {
    std::cout << "Input here " << (*I)->name() << std::endl;
  }*/
#endif
}
