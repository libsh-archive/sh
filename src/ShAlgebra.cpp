#include <map>
#include <string>
#include <sstream>
#include "ShAlgebra.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShOptimizer.hpp"
#include "ShInternals.hpp"
#include "ShEnvironment.hpp"
#include "ShManipulator.hpp"
#include "ShFixedManipulator.hpp"

namespace {

using namespace SH;

typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> CtrlGraphCopyMap;

struct CtrlGraphCopier {
  CtrlGraphCopier(CtrlGraphCopyMap& copyMap)
    : copyMap(copyMap)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShCtrlGraphNodePtr newNode = new ShCtrlGraphNode(*node);
    copyMap[node] = newNode;
  }

  CtrlGraphCopyMap& copyMap;
};

void copyCtrlGraph(ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail,
                   ShCtrlGraphNodePtr& newHead, ShCtrlGraphNodePtr& newTail)
{
  CtrlGraphCopyMap copyMap;
  copyMap[0] = 0;
  
  CtrlGraphCopier copier(copyMap);
  SH_DEBUG_ASSERT(tail); // catch empty tails
  head->clearMarked();
  head->dfs(copier);

  // Replace the successors and followers in the new graph with their new equivalents
  for (CtrlGraphCopyMap::iterator I = copyMap.begin(); I != copyMap.end(); ++I) {
    ShCtrlGraphNodePtr node = I->second; // Get the new node
    if (!node) continue;
    for (std::vector<ShCtrlGraphBranch>::iterator J = node->successors.begin(); J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
    if (node->block) {
      ShBasicBlockPtr new_block = new ShBasicBlock(*node->block);
      node->block = new_block;
    }
  }
  newHead = copyMap[head];
  newTail = copyMap[tail];

  head->clearMarked();
}

}

namespace SH {

ShProgram connect(const ShProgram& a, const ShProgram& b)
{
  int aosize, bisize;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
  if( !a ) return b;
  if( !b ) return a;
  aosize = a->outputs.size();
  bisize = b->inputs.size();
  std::string rtarget;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else {
      rtarget = ""; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

  taila->append(headb);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }

// push back extra inputs from b if aosize < bisize
  if(aosize < bisize) {
    ShProgramNode::VarList::const_iterator II = b->inputs.begin();
    for(int i = 0; i < aosize; ++i, ++II); 
    for(; II != b->inputs.end(); ++II) {
      program->inputs.push_back(*II);
    }
  }

  for (ShProgramNode::VarList::const_iterator II = b->outputs.begin(); II != b->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }

  // push back extra outputs from a if aosize > bisize
  if(aosize > bisize) { 
    ShProgramNode::VarList::const_iterator II = a->outputs.begin();
    for(int i = 0; i < bisize; ++i, ++II); 
    for(; II != a->outputs.end(); ++II) {
      program->outputs.push_back(*II);
    }
  }
  
  ShVariableReplacer::VarMap varMap;

  
  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  
  ShProgramNode::VarList::const_iterator I, J;  
  for (I = a->outputs.begin(), J = b->inputs.begin(); I != a->outputs.end() && J != b->inputs.end(); ++I, ++J) {
    if((*I)->size() != (*J)->size()) {
      ShError(ShAlgebraException("Cannot smash variables " + 
            (*I)->nameOfType() + " " + (*I)->name() + " and " + 
            (*J)->nameOfType() + " " + (*J)->name() + " with different sizes."));
    }
    ShVariableNodePtr n = new ShVariableNode(SH_VAR_TEMP, (*I)->size());
    varMap[*I] = n;
    varMap[*J] = n;
  }

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  ShVariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();
  return program;
}

ShProgram combine(const ShProgram& a, const ShProgram& b)
{
  std::string rtarget;
  if( !a || !b ) SH_DEBUG_WARN( "Connecting with a null ShProgram" );
  if(!a) return b;
  if(!b) return a;

  if (a->target().empty()) {
    rtarget = b->target(); // A doesn't have a target. Use b's.
  } else {
    if (b->target().empty() || a->target() == b->target()) {
      rtarget = a->target(); // A has a target, b doesn't
    } else { 
      rtarget = ""; // Connecting different targets.
    }
  }

  ShProgram program = new ShProgramNode(rtarget);

  ShCtrlGraphNodePtr heada, taila, headb, tailb;

  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);
  copyCtrlGraph(b->ctrlGraph->entry(), b->ctrlGraph->exit(), headb, tailb);

  taila->append(headb);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, tailb);
  program->ctrlGraph = new_graph;

  for (ShProgramNode::VarList::const_iterator I = a->inputs.begin(); I != a->inputs.end(); ++I) {
    program->inputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = a->outputs.begin(); I != a->outputs.end(); ++I) {
    program->outputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = b->inputs.begin(); I != b->inputs.end(); ++I) {
    program->inputs.push_back(*I);
  }
  for (ShProgramNode::VarList::const_iterator I = b->outputs.begin(); I != b->outputs.end(); ++I) {
    program->outputs.push_back(*I);
  }

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel); 
 
  program->collectVariables();
  
  return program;
}

ShProgram namedCombine(const ShProgram &a, const ShProgram &b) {
  ShProgram ab = combine(a, b);

  typedef std::pair<std::string, int> InputType;
  typedef std::map< InputType, int > FirstOccurenceMap;  // position of first occurence of an input type
  typedef std::vector< std::vector<int> > Duplicates;
  FirstOccurenceMap firsts;
  // dups[i] stores the set of positions that have matching input types with position i.
  // The whole set is stored in the smallest i position.
  Duplicates dups( ab->inputs.size(), std::vector<int>()); 

  int i = 0;
  for(ShProgramNode::VarList::const_iterator I = ab->inputs.begin();
      I != ab->inputs.end(); ++I, ++i) {
    InputType it( (*I)->name(), (*I)->size() );
    if( firsts.find( it ) != firsts.end() ) { // duplicate
      dups[ firsts[it] ].push_back(i); 
    } else {
      firsts[it] = i;
      dups[i].push_back(i);
    }
  }
  std::vector<int> swizzle;
  ShFixedManipulator duplicator;
  for(i = 0; i < dups.size(); ++i) {
    if( dups[i].empty() ) continue;
    for(int j = 0; j < dups[i].size(); ++j) swizzle.push_back(dups[i][j]);
    if( duplicator ) duplicator = duplicator & shDup(dups[i].size());
    else duplicator = shDup(dups[i].size());
  }
  return ab << shSwizzle(swizzle) << duplicator; 
}

ShProgram namedConnect(const ShProgram &a, const ShProgram &b, bool keepExtra) {
  // positions of a pair of matched a output and b input 
  typedef std::pair<int, int> MatchedChannels; 
  typedef std::vector<MatchedChannels> MatchedChannelVec;

  std::vector<bool> aMatch(a->outputs.size(), false);
  std::vector<bool> bMatch(b->inputs.size(), false);
  MatchedChannelVec mcv;
  int i, j;

  i = 0;
  for(ShProgramNode::VarList::const_iterator I = a->outputs.begin();
      I != a->outputs.end(); ++I, ++i) {
    j = 0;
    for(ShProgramNode::VarList::const_iterator J = b->inputs.begin();
      J != b->inputs.end(); ++J, ++j) {
      if(bMatch[j] || (*I)->name() != (*J)->name() || (*I)->size() != (*J)->size()) continue; 
      mcv.push_back(MatchedChannels(i,j));
      aMatch[i] = true;
      bMatch[j] = true;
    }
  }

  std::vector<int> aSwiz, bSwiz;
  for(i = 0; i < mcv.size(); ++i) {
    aSwiz.push_back(mcv[i].first);
    bSwiz.push_back(mcv[i].second);
  }
  if( keepExtra ) {
    for(i = 0; i < aMatch.size(); ++i) {
      if( !aMatch[i] ) aSwiz.push_back(i);
    }
  }
  for(i = 0; i < bMatch.size(); ++i) {
    if( !bMatch[i] ) bSwiz.push_back(i);
  }
  return b << shSwizzle(bSwiz) << ( shSwizzle(aSwiz) << a );
}

ShProgram operator<<(const ShProgram& a, const ShProgram& b)
{
  return connect(b,a);
}

ShProgram operator>>(const ShProgram& a, const ShProgram& b)
{
  return connect(a,b);
}

ShProgram operator&(const ShProgram& a, const ShProgram& b)
{
  return combine(a, b);
}

ShProgram operator<<(const ShVariable &var, const ShProgram &p) { 
  return replaceUniform(p, var);
}

ShProgram replaceUniform(const ShProgram& a, const ShVariable& v)
{
  if(!v.uniform()) {
    ShError(ShAlgebraException("Cannot replace non-uniform variable"));
  }

  ShProgram program = new ShProgramNode(a->target());

  ShCtrlGraphNodePtr heada, taila;
  
  copyCtrlGraph(a->ctrlGraph->entry(), a->ctrlGraph->exit(), heada, taila);

  ShCtrlGraphPtr new_graph = new ShCtrlGraph(heada, taila);
  program->ctrlGraph = new_graph;

  for (ShProgramNode::VarList::const_iterator II = a->inputs.begin(); II != a->inputs.end(); ++II) {
    program->inputs.push_back(*II);
  }
  for (ShProgramNode::VarList::const_iterator II = a->outputs.begin(); II != a->outputs.end(); ++II) {
    program->outputs.push_back(*II);
  }
  
  ShVariableReplacer::VarMap varMap;

  ShEnvironment::shader = program;
  ShEnvironment::insideShader = true;
  // make a new input
  ShVariableNodePtr newInput = new ShVariableNode(SH_VAR_INPUT, v.size(), v.node()->specialType()); 
  varMap[v.node()] = newInput;

  ShEnvironment::shader = 0;
  ShEnvironment::insideShader = false;

  ShVariableReplacer replacer(varMap);
  program->ctrlGraph->dfs(replacer);

  ShOptimizer optimizer(program->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);
  
  program->collectVariables();

  return program;
}

}
