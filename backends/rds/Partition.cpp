// Parition v 1.0
//
// Last Revised: Nov. 29 by Cynthia
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "ShDebug.hpp"
#include "ShUtility.hpp"
#include "Partition.hpp"

using namespace SH;

  Partition::Partition(ShProgramNodePtr progPtr)
        :   m_graph(progPtr->ctrlGraph)
  { 
    // assumption: visit should default to false for each node
    // (if my assumption is incorrect, set visit for each node
    //  to false manually here)

    partition(m_graph->entry());
  }

  // still requires debug output
  void Partition::debugDump()
  {
#ifdef SH_DEBUG
      SH_DEBUG_PRINT("Debug Dump");
      //DebugDumper d(*this);
      //or (PassList::iterator I = passes.begin(); I != passes.end(); ++I) {
      //  ShCtrlGraphNodePtr w = *I;
      //}
      SH_DEBUG_PRINT("Debug Dump End");
#endif
  }

  // partitions graph; roots of partitions based on marked nodes
  void Partition::partition(ShCtrlGraphNodePtr v)
  {
    if (!v) return;
    
    visit[v] = true;
    
    // partition each of v's children
    // would not compile with const_iterator, just using iterator for now
    for (std::vector<ShCtrlGraphBranch>::iterator I = v->successors.begin();
         I != v->successors.end(); ++I) 
    {
      ShCtrlGraphNodePtr w = I->node;
      
      if (!visit[w]) {
        partition(w);
      }
      
      visit[w] = true;
      
      // cut w off from v if it's marked
      if (w->marked()) {
        v->successors.erase(I);   // not 100 % sure about this statement
      }
    }
    
    ShCtrlGraphNodePtr w = v->follower;
        if (w) {
          if (!visit[w]) {
        partition(w);
      }
      
      visit[w] = true;
      
      /*  From ShCtrlGraph.hpp: 
       *
       *  Only the exit node of a control graph will have an unconditional
       *  successor of 0. 
       *
       *  ---> Can we just cut the follower off like this?
       **/
      
      // cut w off from v if it's marked
      if (w->marked()) {
        v->follower = 0;
      }
        }
    
    // check if v is a partition root
    if (v->marked()) {
      passes.push_back(v);
    }   
  }
