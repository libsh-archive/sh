IF EXIST "Debug\libshglsl_debug.dll" xcopy /D /Q /Y /K "Debug\libshglsl_debug.dll" "..\..\..\install\bin\vc8"
IF EXIST "Debug\libshglsl_debug.pdb" xcopy /D /Q /Y /K "Debug\libshglsl_debug.pdb" "..\..\..\install\bin\vc8"

IF EXIST "Release\libshglsl.dll" xcopy /D /Q /Y /K "Release\libshglsl.dll" "..\..\..\install\bin\vc8"
IF EXIST "Release\libshglsl.pdb" xcopy /D /Q /Y /K "Release\libshglsl.pdb" "..\..\..\install\bin\vc8"
