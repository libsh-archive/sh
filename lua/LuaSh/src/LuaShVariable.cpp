#include "LuaShVariable.hpp"
using namespace std;
using namespace SH;

namespace LuaSh {
//-----------------------------------------------------------------------------
// Static variables
const char LuaShVariable::ClassName[] = "ShVariable";

const LuaBind<LuaShVariable>::tMethod LuaShVariable::Methods[] =
{
    {"assign",       &LuaShVariable::Assign},
    {"nameOfType",   &LuaShVariable::nameOfType},
    {"size",         &LuaShVariable::size},
    {NULL, NULL}     // Denotes the end of the list
};

const LuaBind<LuaShVariable>::tMethod LuaShVariable::Operators[] =
{
    {"__mutate",     &LuaShVariable::Assign},
    {"__add",        &LuaShVariable::Add},
    {NULL, NULL}     // Denotes the end of the list
};
//-----------------------------------------------------------------------------
LuaShVariable::LuaShVariable(lua_State *L)
{    
    // Extract the parameters from the stack
    int N = lua_gettop(L);
    ShVar = new ShVariable(
        new ShVariableNode(SH_TEMP, N, SH_FLOAT, SH_ATTRIB));

    for (int i = 1; i <= N; ++i)
    {
        lua_Number Num = lua_tonumber(L, i);
        
    }
}
//-----------------------------------------------------------------------------
LuaShVariable::~LuaShVariable()
{
    delete ShVar;
}
//-----------------------------------------------------------------------------
int LuaShVariable::nameOfType(lua_State *L)
{
    lua_pushstring(L, GetTypeName().c_str());
    return 1;
}
//-----------------------------------------------------------------------------
int LuaShVariable::size(lua_State *L)
{
    lua_pushnumber(L, static_cast<lua_Number>(ShVar->size()));
    return 1;
}
//-----------------------------------------------------------------------------
int LuaShVariable::Assign(lua_State *L)
{
    const LuaShVariable *v = GetInstance(L, 1);
    if (v)                   // Assignment from ShVariable/promoted scalar
    {
        shASN(*ShVar, *(v->ShVar));
    }
    return 0;
}
//-----------------------------------------------------------------------------
int LuaShVariable::Add(lua_State *L)
{
    return 0;
}
//-----------------------------------------------------------------------------
} // namespace LuaSh
