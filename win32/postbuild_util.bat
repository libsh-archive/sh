copy ..\util\*.hpp ..\..\install\include\sh

IF EXIST Debug\libshutild.lib copy Debug\libshutild.lib ..\..\install\lib
IF EXIST Release\libshutil.lib copy Release\libshutil.lib ..\..\install\lib
