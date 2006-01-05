xcopy /D /Q /Y /K "..\..\util\*.hpp" "..\..\..\install\include\sh"

IF EXIST "Debug\libshutil_debug.lib" xcopy /D /Q /Y /K "Debug\libshutil_debug.lib" "..\..\..\install\lib"
IF EXIST "Release\libshutil.lib" xcopy /D /Q /Y /K "Release\libshutil.lib" "..\..\..\install\lib"
