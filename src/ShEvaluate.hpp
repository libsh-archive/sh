#ifndef SHEVALUATE_HPP
#define SHEVALUATE_HPP

#include "ShStatement.hpp"

namespace SH {

// Evaluate the given statement.
// This calls an appropriate function from ShInstructions.
// Be sure to keep this synchronized with the operations in ShStatement.
void evaluate(ShStatement& stmt);

}

#endif
