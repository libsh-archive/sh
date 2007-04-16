copy ..\..\src\lauxlib.h ..\..\..\install\lua\include\
copy ..\..\src\luaconf.h ..\..\..\install\lua\include\
copy ..\..\src\lua.h     ..\..\..\install\lua\include\
copy ..\..\src\lualib.h  ..\..\..\install\lua\include\

IF EXIST Release\liblua.lib copy Release\liblua.lib ..\..\..\install\lua\lib
