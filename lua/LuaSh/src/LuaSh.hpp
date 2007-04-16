#ifndef LUASH_HPP
#define LUASH_HPP
//-----------------------------------------------------------------------------
// LuaSh
//   Constants and high-level functions for the Lua Sh wrapper.
//-----------------------------------------------------------------------------
#include "LuaInclude.hpp"

namespace LuaSh {
//-----------------------------------------------------------------------------
int luaopen_sh(lua_State *L);
//-----------------------------------------------------------------------------
} // namespace LuaSh
#endif
