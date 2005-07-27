#include <iostream>
#include "LuaSh.hpp"
#include "LuaShVariable.hpp"
using namespace std;
using namespace LuaSh;
//-----------------------------------------------------------------------------
int LuaSh::luaopen_sh(lua_State *L)
{
    // Register all of our library classes and functions
    LuaBind<LuaShVariable>::Register(L);
    
    // We don't return any results to Lua
    return 0;
}
//-----------------------------------------------------------------------------
