# Microsoft Developer Studio Project File - Name="sh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sh.mak" CFG="sh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sh - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "sh - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /I "../src" /I "../../include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /D "NOMINMAX" /D "_MBCS" /Gm /GR PRECOMP_VC7_TOBEREMOVED /GZ /c /GX 
# ADD CPP /nologo /MDd /I "../src" /I "../../include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /D "NOMINMAX" /D "_MBCS" /Gm /GR PRECOMP_VC7_TOBEREMOVED /GZ /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\sh.lib" 
# ADD LIB32 /nologo /out:"Debug\sh.lib" 

!ELSEIF  "$(CFG)" == "sh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /ML /Zi /W3 /D "WIN32" /D "NDEBUG" /D "_LIB" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c /GX 
# ADD CPP /nologo /ML /Zi /W3 /D "WIN32" /D "NDEBUG" /D "_LIB" /D "_MBCS" PRECOMP_VC7_TOBEREMOVED /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\sh.lib" 
# ADD LIB32 /nologo /out:"Release\sh.lib" 

!ENDIF

# Begin Target

# Name "sh - Win32 Debug"
# Name "sh - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\ShAlgebra.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBackend.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBasicBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBitSet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShCtrlGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShDomTree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShEnvironment.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShError.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShException.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShImage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShInit.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLinearAllocator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShOptimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShParser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShProgram.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShStatement.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShSwizzle.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShSyntax.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTextureNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShToken.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShUtility.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShVariable.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ShVariableNode.cpp
# End Source File
# End Group
# Begin Group "backends"

# PROP Default_Filter ""
# Begin Group "arb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\backends\arb\ShArb.cpp
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sh.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShAlgebra.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShAttrib.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShAttribImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBackend.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBasicBlock.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBitSet.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShBlock.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShColor.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShConstant.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShConstantImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShCtrlGraph.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShDebug.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShDomTree.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShEnvironment.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShError.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShException.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShImage.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLib.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibAttrib.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibColor.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibNormal.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibPoint.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibPosition.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibTexCoord.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLibVector.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShLinearAllocator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShMatrix.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShMatrixImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShNormal.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShOptimizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShParser.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShPoint.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShPosition.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShProgram.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShRefCount.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShSpecialType.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShStatement.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShSwizzle.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShSyntax.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTexCoord.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTexture.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTextureImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTextureNode.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShToken.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShTokenizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShUtility.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShVariable.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShVariableNode.hpp
# End Source File
# Begin Source File

SOURCE=..\src\ShVector.hpp
# End Source File
# End Group
# Begin Group "backends"

# PROP Default_Filter ""
# Begin Group "arb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\backends\arb\ShArb.hpp
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx"
# End Group
# End Target
# End Project

