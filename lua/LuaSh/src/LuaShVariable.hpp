#ifndef LUASHVARIABLE_HPP
#define LUASHVARIABLE_HPP
//-----------------------------------------------------------------------------
// LuaShVariable
//   Provides a Lua-ready wrapper around the ShAttrib type.
//-----------------------------------------------------------------------------
#include "ShInclude.hpp"
#include "LuaBind.hpp"

namespace LuaSh {
//-----------------------------------------------------------------------------
class LuaShVariable
{
private:
    // Handy functions
    static LuaShVariable * GetInstance(lua_State *L, int StackPos)
    {
        return LuaBind<LuaShVariable>::GetInstance(L, StackPos);
    }
    // Same as above, but allow scalars to be promoted to Attrib1's
    static LuaShVariable * GetInstanceScalarPromotion(
        lua_State *L, int StackPos)
    {
        LuaShVariable *v = GetInstance(L, StackPos);
        /*if (!v && lua_isnumber(L, StackPos))
          v = new LuaShVariable(*/
    }
    
    // Various checks needed for ensuring compatible operand types
    bool SizeMatches(const LuaShVariable *Other) const
    {
        return (ShVar->size() == Other->ShVar->size());
    }
    
protected:
    // The actual internal Sh variable
    SH::Variable *ShVar;

public:
    // Variables needed by the autobind template class
    static const char ClassName[];
    static const LuaBind<LuaShVariable>::tMethod Methods[];
    static const LuaBind<LuaShVariable>::tMethod Operators[];

    // Internal constructors
    LuaShVariable(const SH::VariableNodePtr &node,
                  const SH::Swizzle &swizzle, bool neg)
    {
        ShVar = new SH::Variable(node, swizzle, neg);
    }
    
    
    // Return the type name of this specific variable
    std::string GetTypeName() { return ShVar->node()->nameOfType(); }
        
    // Every Lua-callable function must have the prototype:
    //   int luafunc(lua_State *L);
    //
    // Each function is passed the current Lua state, from which it can
    // retrieve arguments. Return values are pushed onto the Lua stack and
    // the (C++) function must return the number of values returned (to Lua).
    LuaShVariable(lua_State *L);
    ~LuaShVariable();
    
    int nameOfType(lua_State *L);
    int size(lua_State *L);

    // Specially bound functions (operators, etc)
    int Assign(lua_State *L);
    int Add(lua_State *L);
};
//-----------------------------------------------------------------------------
} // namespace LuaSh
#endif
