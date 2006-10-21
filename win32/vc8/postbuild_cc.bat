IF EXIST "Debug\libshcc_debug.dll" xcopy /D /Q /Y /K "Debug\libshcc_debug.dll" "..\..\..\install\bin\vc8"
IF EXIST "Debug\libshcc_debug.pdb" xcopy /D /Q /Y /K "Debug\libshcc_debug.pdb" "..\..\..\install\bin\vc8"

IF EXIST "Release\libshcc.dll" xcopy /D /Q /Y /K "Release\libshcc.dll" "..\..\..\install\bin\vc8"
IF EXIST "Release\libshcc.pdb" xcopy /D /Q /Y /K "Release\libshcc.pdb" "..\..\..\install\bin\vc8"
