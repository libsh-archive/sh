IF EXIST "Debug\libshcc_debug.dll" xcopy /D /Q /Y /K "Debug\libshcc_debug.dll" "..\..\..\install"
IF EXIST "Debug\libshcc_debug.pdb" xcopy /D /Q /Y /K "Debug\libshcc_debug.pdb" "..\..\..\install"

IF EXIST "Release\libshcc.dll" xcopy /D /Q /Y /K "Release\libshcc.dll" "..\..\..\install"
IF EXIST "Release\libshcc.pdb" xcopy /D /Q /Y /K "Release\libshcc.pdb" "..\..\..\install"

IF EXIST "Debug\libshcc_debug.dll" xcopy /D /Q /Y /K "Debug\libshcc_debug.dll" "."
IF EXIST "Release\libshcc.dll" xcopy /D /Q /Y /K "Release\libshcc.dll" "."
