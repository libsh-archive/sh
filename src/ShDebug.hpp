#ifndef SHDEBUG_HPP
#define SHDEBUG_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SH_DEBUG
#include <iostream>
#include <cstdlib>

#define SH_DEBUG_PRINT(x) { std::cerr << __FILE__ << ":" << __LINE__ << ": " << x << std::endl; }
#define SH_DEBUG_WARN(x) { SH_DEBUG_PRINT("Warning: " << x) }
#define SH_DEBUG_ERROR(x) { SH_DEBUG_PRINT("Error: " << x) }
#define SH_DEBUG_ASSERT(cond) { if (!(cond)) { SH_DEBUG_ERROR("Assertion failed: " << # cond); abort(); } }

#else

#define SH_DEBUG_PRINT(x)
#define SH_DEBUG_WARN(x)
#define SH_DEBUG_ERROR(x)
#define SH_DEBUG_ASSERT(cond)

#endif // SH_DEBUG

#endif
