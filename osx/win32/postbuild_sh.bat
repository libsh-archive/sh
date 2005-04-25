copy ..\src\*.hpp ..\..\install\include\sh

IF EXIST Debug\libshd.dll copy Debug\libshd.dll ..\..\install
IF EXIST Debug\libshd.pdb copy Debug\libshd.pdb ..\..\install
IF EXIST Debug\libshd.lib copy Debug\libshd.lib ..\..\install\lib

IF EXIST Release\libsh.dll copy Release\libsh.dll ..\..\install
IF EXIST Release\libsh.pdb copy Release\libsh.pdb ..\..\install
IF EXIST Release\libsh.lib copy Release\libsh.lib ..\..\install\lib
