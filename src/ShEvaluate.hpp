#ifndef SHEVALUATE_HPP
#define SHEVALUATE_HPP

#include "ShStatement.hpp"
#include "ShProgramNode.hpp"

// @todo merge this with ShEval.hpp, ShEval.cpp
namespace SH {
// Evaluate the given statement.
SH_DLLEXPORT
void evaluate(ShStatement& stmt);

SH_DLLEXPORT
void evaluate(const ShProgramNodePtr& p);

}

#endif
