IF EXIST "Debug\libshglsl_debug.dll" xcopy /D /Q /Y /K "Debug\libshglsl_debug.dll" "..\..\..\install\vc8"
IF EXIST "Debug\libshglsl_debug.pdb" xcopy /D /Q /Y /K "Debug\libshglsl_debug.pdb" "..\..\..\install\vc8"

IF EXIST "Release\libshglsl.dll" xcopy /D /Q /Y /K "Release\libshglsl.dll" "..\..\..\install\vc8"
IF EXIST "Release\libshglsl.pdb" xcopy /D /Q /Y /K "Release\libshglsl.pdb" "..\..\..\install\vc8"
