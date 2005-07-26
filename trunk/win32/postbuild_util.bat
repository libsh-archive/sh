copy ..\util\*.hpp ..\..\install\include\sh

IF EXIST Debug\libshutil_debug.lib copy Debug\libshutil_debug.lib ..\..\install\lib
IF EXIST Release\libshutil.lib copy Release\libshutil.lib ..\..\install\lib
