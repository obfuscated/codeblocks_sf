# Microsoft Developer Studio Project File - Name="CompilerGCC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CompilerGCC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CompilerGCC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CompilerGCC.mak" CFG="CompilerGCC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CompilerGCC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CompilerGCC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CompilerGCC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./devel/vc6/CompilerGCC/release"
# PROP Intermediate_Dir "./devel/vc6/CompilerGCC/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CompilerGCC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "e:\devcode\wxWindows-2.4.2\lib\mswdll" /I "e:\myprojects\CodeBlocks\CodeBlocks\src\sdk" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "_USRDLL" /D "CompilerGCC_EXPORTS" /D "BUILDING_PLUGIN" /D "WXUSINGDLL" /YX"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ./devel/managers.lib ./devel/sdk.lib ./devel/TinyXML.lib wxxrc.lib stc.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib /nologo /dll /machine:I386 /out:"./devel/share/codeblocks/plugins/CompilerGCC.dll"

!ELSEIF  "$(CFG)" == "CompilerGCC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./devel/vc6/CompilerGCC/debug"
# PROP Intermediate_Dir "./devel/vc6/CompilerGCC/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CompilerGCC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "e:\devcode\wxWindows-2.4.2\lib\mswdlld" /I "e:\myprojects\CodeBlocks\CodeBlocks\src\sdk" /D "_USRDLL" /D "CompilerGCC_EXPORTS" /D "BUILDING_PLUGIN" /D "WXUSINGDLL" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "__WX__" /YX"wx\wxprec.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ./devel/managersd.lib ./devel/sdkd.lib ./devel/TinyXMLd.lib wxxrcd.lib stcd.lib wxmsw24d.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"./devel/share/codeblocks/plugins/CompilerGCC.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CompilerGCC - Win32 Release"
# Name "CompilerGCC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\plugins\compilergcc\advancedcompileroptionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilererrors.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilergcc.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilermessages.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilerMINGW.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilerMSVC.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compileroptionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\customvars.cpp
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\makefilegenerator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\plugins\compilergcc\advancedcompileroptionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilererrors.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilergcc.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilermessages.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilerMINGW.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compilerMSVC.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\compileroptionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\customvars.h
# End Source File
# Begin Source File

SOURCE=.\plugins\compilergcc\makefilegenerator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
