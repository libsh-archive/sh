#ifndef FULLSEARCH_HPP
#define FULLSEARCH_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include "ShDllExport.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "ShBasicBlock.hpp"
#include "DAG.hpp"
#include "CallBack.hpp"

/***
 * A brute force approach to partitioning (for comparison purposes only)
 */
class SH_DLLEXPORT NextKSubset {
  public:
	NextKSubset();
	void init(int n, int k);
	int *next();
	bool more() { return ksub_mtc; }
  private:
	int m_n;	// number of elements 
	int m_k;	// size of subset
	int *m_a;	// array of elements
	int  ksub_h, ksub_m2;
	bool ksub_mtc;
};

#endif
