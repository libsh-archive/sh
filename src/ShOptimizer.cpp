#include "ShOptimizer.hpp"

namespace SH {

ShOptimizer::ShOptimizer(ShCtrlGraphPtr graph)
  : m_graph(graph)
{
}

void ShOptimizer::optimize(int level)
{
  if (level == 0) return; // Level 0 means no optimizations at all.
}

}
