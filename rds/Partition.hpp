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
#include "sh.hpp"			 // necessary?
#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShRefCount.hpp"

namespace SH {

  /**  
   *	Partitions a flowgraph. Marked nodes are the roots of the partitions.
   */
  class
  SH_DLLEXPORT Partition {
  public:
  	// this may accept a ShCtrlGraphNodePtr instead
    Partition(ShProgramNodePtr program);	
 
 	// root nodes of each partition
    typedef std::vector<ShCtrlGraphNodePtr> PassList;
    PassList passes;
 
    void debugDump();
	  
  private:
    void partition(ShCtrlGraphNodePtr v);
	
	ShCtrlGraphPtr m_graph;
	
	// true if node already visited
    typedef std::map<ShCtrlGraphNodePtr, bool> VisitMap;
    VisitMap m_visit;
  };
}

#endif
