#include <iostream>
#include "LuaSh.hpp"
using namespace std;
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
	   cerr << "Usage: " << argv[0] << " luafile" << endl;
	   return 1;
	}

	// Start up Lua and load the basic libraries
	lua_State *L = lua_open();
    luaL_openlibs(L);

    // Open the Sh library
    LuaSh::luaopen_sh(L);
    
	// Execute the given Lua file
	if (luaL_loadfile(L, argv[1]) || lua_pcall(L, 0, 0, 0))
	{
	   cerr << lua_tostring(L, -1) << endl;
	   return 1;
	}

	// Cleanup
	lua_close(L);

    #ifdef _WIN32
      system("pause");
    #endif
	return 0;
}
//-----------------------------------------------------------------------------
