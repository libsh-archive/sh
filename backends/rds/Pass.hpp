#ifndef _PASS_H
#define _PASS_H

#include "DAG.hpp"
#include "ShProgramNode.hpp"

class Pass {
public:
  Pass(DAGNode* d, std::string target);

  SH::ShProgramNodePtr get_prog() { return m_prog; };

private:
  SH::ShProgramNodePtr m_prog;

  void alloc_shared_mem(SH::ShBasicBlockPtr bb);
};

#endif
