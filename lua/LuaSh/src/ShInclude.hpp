#ifndef SHINCLUDE_HPP
#define SHINCLUDE_HPP
//-----------------------------------------------------------------------------
// We don't really want to see the spew of Sh's current warnings ;)
#pragma warning(push)
#pragma warning(disable : 4003)			// Not enough actual params for macro
#pragma warning(disable : 4251)			// DLL-interface
#pragma warning(disable : 4244)			// Double-float conversion
#pragma warning(disable : 4099)			// Type first seen using "x", now "y"
#pragma warning(disable : 4800)			// Autoconvert to bool
#pragma warning(disable : 4267)			// Conversion from size_t to int

// Include any necessary Sh files
#include <sh/sh.hpp>

#pragma warning(pop)
//-----------------------------------------------------------------------------
// Constants
const std::string ShTexIDMetaName = "opengl:texid";
const std::string ShTexUnitMetaName = "opengl:texunit";
//-----------------------------------------------------------------------------
#endif
