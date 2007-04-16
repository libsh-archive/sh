#ifndef LUABIND_HPP
#define LUABIND_HPP
//-----------------------------------------------------------------------------
// LuaBind
//   Helper classes for Lua binding.
//-----------------------------------------------------------------------------
#include "LuaInclude.hpp"
#include <exception>
#include <string>
#include <iostream>
//-----------------------------------------------------------------------------
class LuaException : public std::exception
{
private:
    std::string m_message;
    
public:
    LuaException(const std::string &message) throw()
        : m_message(message)
    {}
    virtual ~LuaException() throw() {}
    virtual const char * what() const throw() { return m_message.c_str(); }
};
//-----------------------------------------------------------------------------
struct StderrLuaException
{
    static void HandleException(const LuaException &e)
    {
        std::cerr << e.what() << std::endl;
    }
};
//-----------------------------------------------------------------------------
template <class T, class ExceptionPolicy = StderrLuaException>
class LuaBind : public ExceptionPolicy
{
private:
    // We'll store full Lua userdata (so we can have a metatable), but our
    // userdata will merely contain a pointer to the real class.
    struct tUserData { T *Instance; };
    
public:
    // All Lua-accessable member functions must match this pointer type
    typedef int (T::*MemFuncPtr)(lua_State *L);
    // A type mapping member function name to the actual pointer
    struct tMethod
    {
        const char *Name;
        MemFuncPtr MemFunc;
    };

    // Register the class "T" with Lua
    static void Register(lua_State *L)
    {
        // Create a new methods table
        lua_newtable(L);
        int MethodsTable = lua_gettop(L);

        // Create a new metatable for the new type
        luaL_newmetatable(L, T::ClassName);
        int TypeMetatable = lua_gettop(L);

        // Store the methods table in the globals table
        lua_pushstring(L, T::ClassName);
        lua_pushvalue(L, MethodsTable);
        lua_settable(L, LUA_GLOBALSINDEX);

        // Hide metatable (mt.__metatable = methods)
        lua_pushliteral(L, "__metatable");
        lua_pushvalue(L, MethodsTable);
        lua_settable(L, TypeMetatable);

        // Read access checks the methods table
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, MethodsTable);
        lua_settable(L, TypeMetatable);

        // Custom function to display type info
        lua_pushliteral(L, "__tostring");
        lua_pushcfunction(L, ToString_T);
        lua_settable(L, TypeMetatable);

        // Garbage collection needs to call the destructor
        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, GarbageCollect_T);
        lua_settable(L, TypeMetatable);

        // Add any user-defined operator overloads
        for (const tMethod *m = T::Operators; m->Name; ++m)
        {
            lua_pushstring(L, m->Name);
            lua_pushlightuserdata(L, (void*)m);
            lua_pushcclosure(L, CallMember_T, 1);
            lua_settable(L, TypeMetatable);
        }

        // Now we need a metatable for the methods table to allow class
        // instantiation (among other things).
        lua_newtable(L);
        int MethodsMetatable = lua_gettop(L);

        // "Calling" the methods table should call the constructor.
        // We'll as well provide the constructor method as "new".
        lua_pushliteral(L, "__call");
        lua_pushcfunction(L, Construct_T);
        lua_pushliteral(L, "new");
        lua_pushvalue(L, -2);
        lua_settable(L, MethodsTable);       // type.new()
        lua_settable(L, MethodsMetatable);   // type()        
        // MethodsMetatable now on top of stack
        lua_setmetatable(L, MethodsTable);

        // Finally fill in the given methods
        for (const tMethod *m = T::Methods; m->Name; ++m)
        {
            // We'll store the method structure pointers as light userdata,
            // stored with each member function call.
            lua_pushstring(L, m->Name);
            lua_pushlightuserdata(L, (void*)m);
            lua_pushcclosure(L, CallMember_T, 1);
            lua_settable(L, MethodsTable);
        }

        // Pop our two remaining tables
        lua_pop(L, 2);
    }
    
    // Type-safe convert of userdata to our template type
    // Pass the stack location that the "self" pointer can be found at
    static T * GetInstance(lua_State *L, int SelfLoc)
    {
        tUserData *d = static_cast<tUserData *>(
            luaL_checkudata(L, SelfLoc, T::ClassName));
        
        if (!d)
            return NULL;
        else
            return d->Instance;
    }

private:
    // Cannot instantiate this class
    LuaBind() {}

    // Dispatch a member function call to the right instance
    static int CallMember_T(lua_State *L)
    {
        // First stack value is "self", rest are arguments
        T *Instance = GetInstance(L, 1);
        if (Instance)
        {
            // Make sure real arguments begin at stack index 1
            lua_remove(L, 1);
            
            // Get the member function table from the upvalue
            const tMethod *m = static_cast<const tMethod *>(
                lua_touserdata(L, lua_upvalueindex(1)));
            
            int ReturnVal = 0;
            try
            {
                // Call the member function with the instance pointer, "this"
                ReturnVal = (Instance->*(m->MemFunc))(L);
            }
            catch (LuaException &e) { HandleException(e); }
            return ReturnVal;
        }
        else
        {
            luaL_typerror(L, 1, T::ClassName);
            return 0;
        }
    }

    // Create a new T instance
    static int Construct_T(lua_State *L)
    {
        // Drop the self pointer to the type metatable
        lua_remove(L, 1);

        // Create a new instance and new userdata pointing to it
        T *Instance = new T(L);
        tUserData *d = static_cast<tUserData *>(
            lua_newuserdata(L, sizeof(tUserData)));
        d->Instance = Instance;

        // Grab our methods metatable and set it as the metatable for our new
        // instance of T (down to stack position -2 after get pushes the new
        // metatable on the top).
        luaL_getmetatable(L, T::ClassName);
        lua_setmetatable(L, -2);

        // Now return our new userdata (now back on top of stack) => 1 return
        return 1;
    }

    // Delete an instance of T
    static int GarbageCollect_T(lua_State *L)
    {
        // Userdata will be the first stack element
        tUserData *d = static_cast<tUserData *>(lua_touserdata(L, 1));
        delete d->Instance;
        return 0;
    }

    static int ToString_T(lua_State *L)
    {
        // Userdata will be the first stack element
        tUserData *d = static_cast<tUserData *>(lua_touserdata(L, 1));

        // Get the pointer address (as an int)
        char Address[32];
        sprintf(Address, "%p", d->Instance);

        // Output a formatted string
        lua_pushfstring(L, "%s (%s)",
                        d->Instance->GetTypeName().c_str(), Address);
        return 1;
    }
};
//-----------------------------------------------------------------------------
#endif
