IF EXIST "Debug\libsharb_debug.dll" xcopy /D /Q /Y /K "Debug\libsharb_debug.dll" "..\..\..\install"
IF EXIST "Debug\libsharb_debug.pdb" xcopy /D /Q /Y /K "Debug\libsharb_debug.pdb" "..\..\..\install"

IF EXIST "Release\libsharb.dll" xcopy /D /Q /Y /K "Release\libsharb.dll" "..\..\..\install"
IF EXIST "Release\libsharb.pdb" xcopy /D /Q /Y /K "Release\libsharb.pdb" "..\..\..\install"

IF EXIST "Debug\libsharb_debug.dll" xcopy /D /Q /Y /K "Debug\libsharb_debug.dll" "."
IF EXIST "Release\libsharb.dll" xcopy /D /Q /Y /K "Release\libsharb.dll" "."
