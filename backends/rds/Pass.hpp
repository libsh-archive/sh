#ifndef _PASS_H
#define _PASS_H

#include <map>
#include <set>

#include "DAG.hpp"
#include "RDS.hpp"
#include "ShProgramNode.hpp"
#include "ShStream.hpp"

class Pass {
public:
  Pass(DAGNode* d, std::string target);

  SH::ShProgramNodePtr get_prog() { return m_prog; };
  SH::ShBasicBlockPtr get_bb() { return m_bb; };
  int id;
  SH::ShStream* target;

private:
  SH::ShProgramNodePtr m_prog;
  SH::ShBasicBlockPtr m_bb;

  void alloc_shared_mem(DAGNode* dag);
};

class Schedule {
public:
  Schedule::Schedule(RDS::PassVector p, RDS::VarVector v);

  typedef std::map<SH::ShVariableNodePtr,Pass* > VarMap;
  VarMap def,use;
  typedef std::map<SH::ShVariableNodePtr,SH::ShChannelNodePtr> ChanMap;
  ChanMap channels;

  std::vector<Pass*>* get_passes() { return &m_passes; }

private:
  void fix_instructions(Pass* p);
  void make_channel(SH::ShVariableNodePtr v);
  std::vector<Pass*> m_passes;
};

#endif
