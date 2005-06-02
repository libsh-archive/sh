copy ..\src\*.hpp ..\..\install\include\sh

IF EXIST Debug\libsh_debug.dll copy Debug\libsh_debug.dll ..\..\install
IF EXIST Debug\libsh_debug.pdb copy Debug\libsh_debug.pdb ..\..\install
IF EXIST Debug\libsh_debug.lib copy Debug\libsh_debug.lib ..\..\install\lib

IF EXIST Release\libsh.dll copy Release\libsh.dll ..\..\install
IF EXIST Release\libsh.pdb copy Release\libsh.pdb ..\..\install
IF EXIST Release\libsh.lib copy Release\libsh.lib ..\..\install\lib
