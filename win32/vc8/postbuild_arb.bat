IF EXIST "Debug\libsharb_debug.dll" xcopy /D /Q /Y /K "Debug\libsharb_debug.dll" "..\..\..\install\vc8"
IF EXIST "Debug\libsharb_debug.pdb" xcopy /D /Q /Y /K "Debug\libsharb_debug.pdb" "..\..\..\install\vc8"

IF EXIST "Release\libsharb.dll" xcopy /D /Q /Y /K "Release\libsharb.dll" "..\..\..\install\vc8"
IF EXIST "Release\libsharb.pdb" xcopy /D /Q /Y /K "Release\libsharb.pdb" "..\..\..\install\vc8"

