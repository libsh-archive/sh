#ifndef SHSCHEDULE_HPP
#define SHSCHEDULE_HPP

#include <list>
#include <set>
#include <iosfwd>
#include "ShProgramNode.hpp"
#include "ShVariable.hpp"

namespace SH {

enum MappingType {
  MAPPING_NULL,
  MAPPING_TEMP,
  MAPPING_INPUT,
  MAPPING_OUTPUT
};

// All mappings are assumed to point to 4-colour floating point
// buffers [may want to allow differently sized buffers in the future].
// Offset and length are used to address subparts of these buffers.
struct Mapping {
  Mapping()
    : type(MAPPING_NULL), id(0), offset(0), length(0)
  {
  }
  
  Mapping(MappingType type, int id, int offset, int length)
    : type(type), id(id), offset(offset), length(length)
  {
  }
  
  MappingType type;
  int id;
  unsigned int offset;
  unsigned int length;
};

std::ostream& operator<<(std::ostream& out, const Mapping& mapping);

struct ShPass {
  ShProgramNodePtr program;
  std::list<Mapping> inputs, outputs;

  // Either a predicated branch or an unconditional branch.

  Mapping predicate;
  ShPass* predicate_pass; // Taken if predicate is true
  ShPass* default_pass; // Taken if no predicate, or predicate is false

  ShVoidPtr backend_data;
  
  // Number of elements waiting on this pass.
  int count;
};

class ShSchedule {
public:
  typedef std::list<ShPass> PassList;

  // Build a schedule from the given program's control flow graph.
  explicit ShSchedule(const ShProgramNodePtr& program);

  // Construct a schedule from simple components
  ShSchedule(const PassList& list,
             PassList::const_iterator root,
             const ShProgramNode::VarList& inputs,
             const ShProgramNode::VarList& outputs);

  void prepare();
  void execute();
  
  void dump_graphviz(std::ostream& out) const;
  
  // Access to the pass list
  PassList::iterator begin() { return m_passes.begin(); }
  PassList::iterator end() { return m_passes.end(); }
  PassList::const_iterator begin() const { return m_passes.begin(); }
  PassList::const_iterator end() const { return m_passes.end(); }

  PassList::iterator root() { return m_root; }
  PassList::const_iterator root() const { return m_root; }

  std::size_t num_passes() { return m_passes.size(); }

  // Number of temporary buffers used.
  std::size_t num_temps() { return m_num_temps; }
  
private:
  // The program for which this schedule is generated.
  ShProgramNodePtr m_program;
  
  PassList m_passes;
  
  // The root of the graph
  PassList::iterator m_root;

  // Passes with count > 0
  std::set<ShPass*> m_eligible_passes;

  // The original inputs and outputs
  ShProgramNode::VarList m_inputs, m_outputs;

  ShVoidPtr m_backend_data;

  std::size_t m_num_temps;

  // NOT IMPLEMENTED
  ShSchedule(const ShSchedule&);
  ShSchedule& operator=(const ShSchedule&);
};

}

#endif
