IF EXIST "Debug\libshglsl_debug.dll" xcopy /D /Q /Y /K "Debug\libshglsl_debug.dll" "..\..\..\install"
IF EXIST "Debug\libshglsl_debug.pdb" xcopy /D /Q /Y /K "Debug\libshglsl_debug.pdb" "..\..\..\install"

IF EXIST "Release\libshglsl.dll" xcopy /D /Q /Y /K "Release\libshglsl.dll" "..\..\..\install"
IF EXIST "Release\libshglsl.pdb" xcopy /D /Q /Y /K "Release\libshglsl.pdb" "..\..\..\install"

IF EXIST "Debug\libshglsl_debug.dll" xcopy /D /Q /Y /K "Debug\libshglsl_debug.dll" "."
IF EXIST "Release\libshglsl.dll" xcopy /D /Q /Y /K "Release\libshglsl.dll" "."
