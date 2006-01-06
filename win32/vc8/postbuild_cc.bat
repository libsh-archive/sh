IF EXIST "Debug\libshcc_debug.dll" xcopy /D /Q /Y /K "Debug\libshcc_debug.dll" "..\..\..\install\vc8"
IF EXIST "Debug\libshcc_debug.pdb" xcopy /D /Q /Y /K "Debug\libshcc_debug.pdb" "..\..\..\install\vc8"

IF EXIST "Release\libshcc.dll" xcopy /D /Q /Y /K "Release\libshcc.dll" "..\..\..\install\vc8"
IF EXIST "Release\libshcc.pdb" xcopy /D /Q /Y /K "Release\libshcc.pdb" "..\..\..\install\vc8"
