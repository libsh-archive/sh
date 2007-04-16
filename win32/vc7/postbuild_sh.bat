xcopy /D /Q /Y /K "..\..\src\sh\*.hpp" "..\..\..\install\include\sh"

IF EXIST "Debug\libsh_debug.dll" xcopy /D /Q /Y /K "Debug\libsh_debug.dll" "..\..\..\install\bin\vc7"
IF EXIST "Debug\libsh_debug.pdb" xcopy /D /Q /Y /K "Debug\libsh_debug.pdb" "..\..\..\install\bin\vc7"
IF EXIST "Debug\libsh_debug.lib" xcopy /D /Q /Y /K "Debug\libsh_debug.lib" "..\..\..\install\lib\vc7"

IF EXIST "Release\libsh.dll" xcopy /D /Q /Y /K "Release\libsh.dll" "..\..\..\install\bin\vc7"
IF EXIST "Release\libsh.pdb" xcopy /D /Q /Y /K "Release\libsh.pdb" "..\..\..\install\bin\vc7"
IF EXIST "Release\libsh.lib" xcopy /D /Q /Y /K "Release\libsh.lib" "..\..\..\install\lib\vc7"

