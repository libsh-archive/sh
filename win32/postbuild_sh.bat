xcopy /D /Q /Y /K "..\src\*.hpp" "..\..\install\include\sh"

IF EXIST "Debug\libsh_debug.dll" xcopy /D /Q /Y /K "Debug\libsh_debug.dll" "..\..\install"
IF EXIST "Debug\libsh_debug.pdb" xcopy /D /Q /Y /K "Debug\libsh_debug.pdb" "..\..\install"
IF EXIST "Debug\libsh_debug.lib" xcopy /D /Q /Y /K "Debug\libsh_debug.lib" "..\..\install\lib"

IF EXIST "Release\libsh.dll" xcopy /D /Q /Y /K "Release\libsh.dll" "..\..\install"
IF EXIST "Release\libsh.pdb" xcopy /D /Q /Y /K "Release\libsh.pdb" "..\..\install"
IF EXIST "Release\libsh.lib" xcopy /D /Q /Y /K "Release\libsh.lib" "..\..\install\lib"

IF EXIST "Debug\libsh_debug.dll" xcopy /D /Q /Y /K "Debug\libsh_debug.dll" "."
IF EXIST "Release\libsh.dll" xcopy /D /Q /Y /K "Release\libsh.dll" "."
