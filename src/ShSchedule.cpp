#include "ShSchedule.hpp"
#include <utility>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include "ShOperation.hpp"
#include "ShVariableNode.hpp"
#include "ShContext.hpp"
#include "ShTransformer.hpp"
#include "ShOptimizations.hpp"

namespace {
using namespace SH;

struct TempCounter {
  TempCounter(std::map<ShVariableNode*, int>& register_count,
              std::map<ShVariableNode*, Mapping>& register_map,
              int& max_register)
    : register_count(register_count),
      register_map(register_map),
      max_register(max_register)
  {
  }
      
  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    std::set<ShVariableNode*> inblock;

    SH_DEBUG_ASSERT(node->successors.size() < 2);

    if (!node->successors.empty()) {
      
      // Make sure that predicates are one-tuples.
      // Could have used a transformer for this.
      if (node->successors.front().cond.size() > 1) {
        const ShVariable& var = node->successors.front().cond;
        ShVariableNodePtr new_node = new ShVariableNode(SH_TEMP, 1, var.node()->valueType());
        ShVariable new_var(new_node);

        block->addStatement(ShStatement(new_var, SH_OP_ASN, var(0)));
        for (int i = 1; i < var.size(); i++) {
          block->addStatement(ShStatement(new_var, new_var, SH_OP_MAX, var(i)));
        }
      
        node->successors.front().cond = new_var;
      }

      // Branch predicates must have buffers allocated to them.
      
      ShVariableNode* vnode = node->successors.front().cond.node().object();
      if (register_count[vnode] < 2) {
        register_count[vnode] = 2;
        register_map[vnode] = Mapping(MAPPING_TEMP, max_register++,
                                      0, vnode->size());
        inblock.insert(vnode);
      }
    }
    
    // Go find registers
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->dest.node() && I->dest.node()->kind() == SH_TEMP) {
        ShVariableNode* vnode = I->dest.node().object();

        if (inblock.find(vnode) == inblock.end()) {
          if (++register_count[vnode] == 2) {
            register_map[vnode] = Mapping(MAPPING_TEMP, max_register++,
                                          0, vnode->size());
          }
          inblock.insert(vnode);
        }
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        if (I->src[i].node() && I->src[i].node()->kind() == SH_TEMP) {
          ShVariableNode* vnode = I->src[i].node().object();
          
          if (inblock.find(vnode) == inblock.end()) {
            if (++register_count[vnode] == 2) {
              register_map[vnode] = Mapping(MAPPING_TEMP, max_register++,
                                            0, vnode->size());
            }
            inblock.insert(vnode);
          }
        }
      }
    }
  }

  std::map<ShVariableNode*, int>& register_count;
  std::map<ShVariableNode*, Mapping>& register_map;
  int max_register;
};

struct ScheduleBuilder {
  ScheduleBuilder(std::map<ShVariableNode*, Mapping>& register_map,
                  ShSchedule::PassList& passes)
    : register_map(register_map),
      passes(passes)
  {
  }

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShPass pass;

    ShCtrlGraphNodePtr new_head = new ShCtrlGraphNode();
    ShCtrlGraphNodePtr new_body = new ShCtrlGraphNode();
    ShCtrlGraphNodePtr new_tail = new ShCtrlGraphNode();

    if (node->block) {
      new_body->block = new ShBasicBlock(*node->block);
    }
    new_head->append(new_body);
    new_body->append(new_tail);

    // TODO: Target?
    pass.program = new ShProgramNode("");
    pass.program->ctrlGraph = new ShCtrlGraph(new_head, new_tail);
    
    pass.count = 0;
    

    // We need to turn shared temporaries into inputs/outputs

    std::set<ShVariableNode*> handled_output;
    std::set<ShVariableNode*> handled_input;

    typedef std::list<std::pair<ShVariableNode*, ShVariableNode*> > TempList;
    TempList output_temps, input_temps;
    
    ShContext::current()->enter(pass.program);
    if (new_body->block) {
      
      
      for (ShBasicBlock::ShStmtList::iterator I = new_body->block->begin();
           I != new_body->block->end(); ++I) {
        if (I->dest.node()) {
          ShVariableNode* vnode = I->dest.node().object();
          if (vnode->kind() == SH_TEMP
              && handled_output.find(vnode) == handled_output.end()
              && register_map.find(vnode) != register_map.end()) {
            // Need to add an output for this node
            ShVariableNodePtr outnode = new ShVariableNode(SH_OUTPUT,
                                                           vnode->size(),
                                                           vnode->valueType());
          
            {
              std::ostringstream os;
              os << "T[" << register_map[vnode].id << "]_out";
              outnode->name(os.str());
            }
            pass.outputs.push_back(register_map[vnode]);
            handled_output.insert(vnode);
            output_temps.push_back(std::make_pair(vnode, outnode.object()));
          } else if (vnode->kind() == SH_OUTPUT
                     && handled_output.find(vnode) == handled_output.end()) {
            SH_DEBUG_ASSERT(register_map.find(vnode) != register_map.end());
            
            pass.outputs.push_back(register_map[vnode]);
            handled_output.insert(vnode);
          }
        }
        
        for (int i = 0; i < opInfo[I->op].arity; i++) {
          if (!I->src[i].node()) continue;
          ShVariableNode* vnode = I->src[i].node().object();

          if (vnode->kind() == SH_TEMP
              && handled_input.find(vnode) == handled_input.end() 
              && register_map.find(vnode) != register_map.end()) {

            // Need to add an input for this node
            ShVariableNodePtr innode = new ShVariableNode(SH_INPUT,
                                                          vnode->size(),
                                                          vnode->valueType());
            {
              std::ostringstream os;
              os << "T[" << register_map[vnode].id << "]_in";
              innode->name(os.str());
            }
            pass.inputs.push_back(register_map[vnode]);
            handled_input.insert(vnode);
            input_temps.push_back(std::make_pair(vnode, innode.object()));
          } else if (vnode->kind() == SH_INPUT
                     && handled_input.find(vnode) == handled_input.end()) {
            SH_DEBUG_ASSERT(register_map.find(vnode) != register_map.end());
            
            pass.inputs.push_back(register_map[vnode]);
            handled_input.insert(vnode);
          }
        }
      }

      // Add the assignment statements connecting the temporaries to inputs/outputs.
      for (TempList::const_iterator I = input_temps.begin(); I != input_temps.end(); ++I){
        new_body->block->prependStatement(ShStatement(ShVariable(I->first), SH_OP_ASN,
                                                      ShVariable(I->second)));
      }
      for (TempList::const_iterator I = output_temps.begin(); I != output_temps.end(); ++I){
        new_body->block->addStatement(ShStatement(ShVariable(I->second), SH_OP_ASN,
                                                  ShVariable(I->first)));
      }

      passes.push_back(pass);
      
      // We'll fill in the edges later, using these maps.
      // Not efficient, but simple.
      node_to_pass[node.object()] = &passes.back();
      pass_to_node[&passes.back()] = node.object();
    }
    
    ShContext::current()->exit();
  }

  void connect_passes()
  {
    for (ShSchedule::PassList::iterator I = passes.begin();
         I != passes.end(); ++I) {
      ShPass& pass = *I;
      ShCtrlGraphNode* node = pass_to_node[&pass];
      SH_DEBUG_ASSERT(node);

      // Won't handle these for now. Would be easy though with dummy
      // passes or something.
      SH_DEBUG_ASSERT(node->successors.size() < 2);

      if (node->successors.empty()) {
        pass.predicate = Mapping();
        pass.predicate_pass = 0;
      } else {
        ShVariableNode* cnode = node->successors.front().cond.node().object();
        SH_DEBUG_ASSERT(register_map.find(cnode) != register_map.end());
        SH_DEBUG_ASSERT(node->successors.front().cond.size() == 1);
        
        pass.predicate = register_map[cnode];
        pass.predicate_pass = node_to_pass[node->successors.front().node.object()];
        SH_DEBUG_ASSERT(pass.predicate_pass);
      }
      pass.default_pass = node_to_pass[node->follower.object()];
    }
  }
  
  std::map<ShVariableNode*, Mapping> register_map;
  ShSchedule::PassList& passes;
  std::map<ShCtrlGraphNode*, ShPass*> node_to_pass;
  std::map<ShPass*, ShCtrlGraphNode*> pass_to_node;
};

}

namespace SH {

std::ostream& operator<<(std::ostream& out, const Mapping& mapping)
{
  switch (mapping.type) {
  case MAPPING_NULL:
    out << "[null]";
    break;
  case MAPPING_TEMP:
    out << "T[" << mapping.id << "]";
    break;
  case MAPPING_INPUT:
    out << "I[" << mapping.id << "]";
    break;
  case MAPPING_OUTPUT:
    out << "O[" << mapping.id << "]";
    break;
  }
  return out;
}

ShSchedule::ShSchedule(const ShProgramNodePtr& program_orig)
  : m_program(program_orig->clone()),
    m_root(m_passes.end()),
    m_inputs(m_program->inputs),
    m_outputs(m_program->outputs)
{
  ShTransformer xform(m_program);

  xform.convertInputOutput();

  optimize(m_program);
  
  // El cheapo register allocator
  // Just make registers for those temporaries used in more than one
  // block.
  // Aka. the "Tibi register allocator"
  std::map<ShVariableNode*, int> register_count;
  std::map<ShVariableNode*, Mapping> register_map;
  int max_register = 0;
  ShContext::current()->enter(m_program);
  TempCounter count(register_count, register_map, max_register);
  m_program->ctrlGraph->dfs(count);
  ShContext::current()->exit();

  int i = 0;
  for (ShProgramNode::VarList::const_iterator I = m_program->inputs_begin();
       I != m_program->inputs_end(); ++I, ++i) {
    ShVariableNode* node = I->object();
    register_map[node] = Mapping(MAPPING_INPUT, i, 0, node->size());
  }
  i = 0;
  for (ShProgramNode::VarList::const_iterator I = m_program->outputs_begin();
       I != m_program->outputs_end(); ++I, ++i) {
    ShVariableNode* node = I->object();
    register_map[node] = Mapping(MAPPING_OUTPUT, i, 0, node->size());
  }


  // Now, break each block into its own program.

  ScheduleBuilder builder(register_map, m_passes);
  m_program->ctrlGraph->dfs(builder);
  builder.connect_passes();
  
  // Set the root.
  m_root = m_passes.begin();
}

ShSchedule::ShSchedule(const ShSchedule::PassList& passes,
                       ShSchedule::PassList::const_iterator root,
                       const ShProgramNode::VarList& inputs,
                       const ShProgramNode::VarList& outputs)
  : m_passes(passes),
    m_root(m_passes.begin()),
    m_inputs(inputs),
    m_outputs(outputs)
{
  std::advance(m_root, std::distance(passes.begin(), root));
  m_eligible_passes.insert(&(*m_root));
}

void ShSchedule::dump_graphviz(std::ostream& out)
{
  out << "digraph schedule { " << std::endl;
  for (PassList::const_iterator I = m_passes.begin(); I != m_passes.end(); ++I) {
    const ShPass& pass = *I;
    
    out << "  \"" << &pass << "\" [label=\"";
    out << "{";
    for (std::list<Mapping>::const_iterator J = pass.inputs.begin();
         J != pass.inputs.end(); ++J) {
      if (J != pass.inputs.begin()) out << ", ";
      out << *J;
    }
    out << "}\\n";
    
    ShCtrlGraphNodePtr body_node = pass.program->ctrlGraph->entry()->follower;
    if (body_node->block) {
      body_node->block->graphvizDump(out);
    }
    out << "{";
    for (std::list<Mapping>::const_iterator J = pass.outputs.begin();
         J != pass.outputs.end(); ++J) {
      if (J != pass.outputs.begin()) out << ", ";
      out << *J;
    }
    out << "}";
    out << "\", shape=box]";
    out << ";" << std::endl;
    
    if (pass.predicate_pass) {
      out << "  \"" << &pass << "\" -> \"" << pass.predicate_pass << "\""
          << " [label=\"buf[" << pass.predicate.id << "]\"];" << std::endl;
      out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\""
          << " [label=\"not buf[" << pass.predicate.id << "]\"];" << std::endl;
    } else {
      if (pass.default_pass) {
        out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\";" << std::endl;
      }
    }
  }

  out << "}" << std::endl;
}

}


