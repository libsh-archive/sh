#ifndef SHDEBUG_HPP
#define SHDEBUG_HPP

#ifdef DEBUG
#include <iostream>

#define DEBUG_PRINT(x) { std::cerr << __FILE__ << ":" << __LINE__ << ": " << x << std::endl; }
#define DEBUG_WARN(x) { DEBUG_PRINT("Warning: " << x) }
#define DEBUG_ERROR(x) { DEBUG_PRINT("Error: " << x) }
#define DEBUG_ASSERT(cond) { if (!cond) { DEBUG_ERROR("Assertion failed: " << # cond); } }

#else

#define DEBUG_PRINT(x)
#define DEBUG_WARN(x)
#define DEBUG_ERROR(x)
#define DEBUG_ASSERT(cond)

#endif // DEBUG

#endif
