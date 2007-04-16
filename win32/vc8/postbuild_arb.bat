IF EXIST "Debug\libsharb_debug.dll" xcopy /D /Q /Y /K "Debug\libsharb_debug.dll" "..\..\..\install\bin\vc8"
IF EXIST "Debug\libsharb_debug.pdb" xcopy /D /Q /Y /K "Debug\libsharb_debug.pdb" "..\..\..\install\bin\vc8"

IF EXIST "Release\libsharb.dll" xcopy /D /Q /Y /K "Release\libsharb.dll" "..\..\..\install\bin\vc8"
IF EXIST "Release\libsharb.pdb" xcopy /D /Q /Y /K "Release\libsharb.pdb" "..\..\..\install\bin\vc8"
