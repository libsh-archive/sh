#ifndef SHOPTIMIZER_HPP
#define SHOPTIMIZER_HPP

#include "ShCtrlGraph.hpp"

namespace SH {

class ShOptimizer {
public:
  ShOptimizer(ShCtrlGraphPtr graph);

  /// Optimize with the given agressiveness
  void optimize(int level);

private:
  ShCtrlGraphPtr m_graph;
  
  /// NOT IMPLEMENTED
  ShOptimizer(const ShOptimizer& other);
  /// NOT IMPLEMENTED
  ShOptimizer& operator=(const ShOptimizer& other);
};

}

#endif
