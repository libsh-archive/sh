#include "ShSchedule.hpp"
#include <utility>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include "ShOperation.hpp"
#include "ShVariableNode.hpp"

namespace {
using namespace SH;

struct TempCounter {
  TempCounter(std::map<ShVariableNode*, int>& register_count,
              std::map<ShVariableNode*, int>& register_map,
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
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (!I->dest.node() && I->dest.node()->kind() == SH_TEMP) {
        ShVariableNode* node = I->dest.node().object();

        if (inblock.find(node) == inblock.end()) {
          if (++register_count[node] == 2) {
            register_map[node] = max_register++;
          }
          inblock.insert(node);
        }
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        if (I->src[i].node() && I->src[i].node()->kind() == SH_TEMP) {
          ShVariableNode* node = I->src[i].node().object();
          
          if (inblock.find(node) == inblock.end()) {
            if (++register_count[node] == 2) {
              register_map[node] = max_register++;
            }
            inblock.insert(node);
          }
        }
      }
    }
  }

  std::map<ShVariableNode*, int>& register_count;
  std::map<ShVariableNode*, int>& register_map;
  int max_register;
};

struct ScheduleBuilder {
  ScheduleBuilder(ShSchedule::PassList& passes)
    : passes(passes)
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
        pass.predicate = ShVariable();
        pass.predicate_pass = 0;
      } else {
        pass.predicate = node->successors.front().cond;
        pass.predicate_pass = node_to_pass[node->successors.front().node.object()];
        SH_DEBUG_ASSERT(pass.predicate_pass);
      }
      pass.default_pass = node_to_pass[node->follower.object()];
    }
  }
  
  ShSchedule::PassList& passes;
  std::map<ShCtrlGraphNode*, ShPass*> node_to_pass;
  std::map<ShPass*, ShCtrlGraphNode*> pass_to_node;
};

}

namespace SH {

ShSchedule::ShSchedule(const ShProgramNodePtr& program)
  : m_root(m_passes.end()),
    m_inputs(program->inputs),
    m_outputs(program->outputs)
{
  // El cheapo register allocator
  // Just make registers for those temporaries used in more than one
  // block.
  // Aka. the "Tibi register allocator"
  std::map<ShVariableNode*, int> register_count;
  std::map<ShVariableNode*, int> register_map;
  int max_register = 0;
  TempCounter count(register_count, register_map, max_register);
  program->ctrlGraph->dfs(count);

  // Now, break each block into its own program.

  ScheduleBuilder builder(m_passes);
  program->ctrlGraph->dfs(builder);
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
    out << "  \"" << &pass << "\" [label=\"\"]";
    out << ";" << std::endl;
    if (pass.predicate_pass) {
      out << "  \"" << &pass << "\" -> \"" << pass.predicate_pass << "\""
          << " [label=\"" << pass.predicate.name() << "\"];" << std::endl;
      out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\""
          << " [label=\"!" << pass.predicate.name() << "\"];" << std::endl;
    } else {
      if (pass.default_pass) {
        out << "  \"" << &pass << "\" -> \"" << pass.default_pass << "\";" << std::endl;
      }
    }
  }
  out << "}" << std::endl;
}

}


