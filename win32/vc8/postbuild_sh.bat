xcopy /D /Q /Y /K "..\..\src\*.hpp" "..\..\..\install\vc8\include\sh"

IF EXIST "Debug\libsh_debug.dll" xcopy /D /Q /Y /K "Debug\libsh_debug.dll" "..\..\..\install\vc8"
IF EXIST "Debug\libsh_debug.pdb" xcopy /D /Q /Y /K "Debug\libsh_debug.pdb" "..\..\..\install\vc8"
IF EXIST "Debug\libsh_debug.lib" xcopy /D /Q /Y /K "Debug\libsh_debug.lib" "..\..\..\install\vc8\lib"

IF EXIST "Release\libsh.dll" xcopy /D /Q /Y /K "Release\libsh.dll" "..\..\..\install\vc8"
IF EXIST "Release\libsh.pdb" xcopy /D /Q /Y /K "Release\libsh.pdb" "..\..\..\install\vc8"
IF EXIST "Release\libsh.lib" xcopy /D /Q /Y /K "Release\libsh.lib" "..\..\..\install\vc8\lib"

