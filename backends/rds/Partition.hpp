// Partitioning v 1.0
//
// Last Revised: Nov. 28 by Cynthia
//
//
// Makes a vector of flowgraph partitions at the moment (ShCtrlGraphNodePtr).
// Will probably then have to convert each into its own ShProgram.
//////////////////////////////////////////////////////////////////////////////

#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "sh.hpp"      // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"

  /**  
   *  Partitions a flowgraph. Marked nodes are the roots of the partitions.
   */
  class
  SH_DLLEXPORT Partition {
    public:
      // this may accept a ShCtrlGraphNodePtr instead
      Partition(SH::ShProgramNodePtr progPtr);  
 
      // root nodes of each partition
      typedef std::vector<SH::ShCtrlGraphNodePtr> PassList;
      PassList passes;
 
      void debugDump();
    
    private:
      void partition(SH::ShCtrlGraphNodePtr v);
  
      SH::ShCtrlGraphPtr m_graph;
  
      // true if node already visited
      typedef std::map<SH::ShCtrlGraphNodePtr, bool> VisitMap;
      VisitMap visit; // Most of Partition.cpp used visit[] so I renamed it
  };

#endif
