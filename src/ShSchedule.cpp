#include "ShSchedule.hpp"
#include <utility>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include "ShOperation.hpp"
#include "ShVariableNode.hpp"
#include "ShContext.hpp"

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
    passes.push_back(pass);
    
    // We'll fill in the edges later, using these maps.
    // Not efficient, but simple.
    node_to_pass[node.object()] = &passes.back();
    pass_to_node[&passes.back()] = node.object();

    // TODO: We need to turn shared temporaries into inputs/outputs

    std::set<ShVariableNode*> handled_output;
    std::set<ShVariableNode*> handled_input;

    ShContext::current()->enter(pass.program);
    if (new_body->block) for (ShBasicBlock::ShStmtList::iterator I = new_body->block->begin();
                              I != new_body->block->end(); ++I) {
      if (I->dest.node() && I->dest.node()->kind() == SH_TEMP) {
        ShVariableNode* vnode = I->dest.node().object();
        if (handled_output.find(vnode) == handled_output.end() &&
            register_map.find(vnode) != register_map.end()) {
          // Need to add an output for this node
          ShVariableNodePtr outnode = new ShVariableNode(SH_OUTPUT,
                                                         vnode->size(),
                                                         vnode->valueType());
          
          // TODO: Add an assignment statement at end of pass
          
          pass.outputs.push_back(register_map[vnode]);
          handled_output.insert(vnode);
        }
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        if (!I->src[i].node() || I->src[i].node()->kind() != SH_TEMP) continue;
        ShVariableNode* vnode = I->src[i].node().object();

        if (handled_input.find(vnode) != handled_input.end() ||
            register_map.find(vnode) == register_map.end()) continue;

        // Need to add an input for this node
        ShVariableNodePtr innode = new ShVariableNode(SH_INPUT,
                                                      vnode->size(),
                                                      vnode->valueType());
        
        // TODO: Add an assignment statement at beginning of pass
        pass.inputs.push_back(register_map[vnode]);
        handled_output.insert(vnode);
      }
    }

    // TODO: Add assignment statements
    
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

ShSchedule::ShSchedule(const ShProgramNodePtr& program_orig)
  : m_program(program_orig->clone()),
    m_root(m_passes.end()),
    m_inputs(m_program->inputs),
    m_outputs(m_program->outputs)
{
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
    ShCtrlGraphNodePtr body_node = pass.program->ctrlGraph->entry()->follower;
    if (body_node->block) {
      body_node->block->graphvizDump(out);
    }
    out << "\", shape=box]";
    out << ";" << std::endl;
    if (pass.predicate_pass) {
      out << "  \"" << &pass << "\" -> \"" << pass.predicate_pass << "\""
          << " [label=\"b" << pass.predicate.id << "\"];" << std::endl;
      out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\""
          << " [label=\"!b" << pass.predicate.id << "\"];" << std::endl;
    } else {
      if (pass.default_pass) {
        out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\";" << std::endl;
      }
    }
  }

  out << "}" << std::endl;
}

}


