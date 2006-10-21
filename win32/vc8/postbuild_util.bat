xcopy /D /Q /Y /K "..\..\src\shutil\*.hpp" "..\..\..\install\include\shutil"

IF EXIST "Debug\libshutil_debug.lib" xcopy /D /Q /Y /K "Debug\libshutil_debug.lib" "..\..\..\install\lib\vc8"
IF EXIST "Release\libshutil.lib" xcopy /D /Q /Y /K "Release\libshutil.lib" "..\..\..\install\lib\vc8"
