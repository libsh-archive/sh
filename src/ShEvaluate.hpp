#ifndef SHEVALUATE_HPP
#define SHEVALUATE_HPP

#include "ShStatement.hpp"
#include "ShProgramNode.hpp"

namespace SH {
SH_DLLEXPORT
void evaluate(ShStatement& stmt);

SH_DLLEXPORT
void evaluate(const ShProgramNodePtr& p);

}

#endif
