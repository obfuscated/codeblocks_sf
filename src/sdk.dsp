# Microsoft Developer Studio Project File - Name="sdk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sdk - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdk.mak" CFG="sdk - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdk - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sdk - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdk - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./devel/vc6/sdk/release"
# PROP Intermediate_Dir "./devel/vc6/sdk/release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "e:\devcode\wxWindows-2.4.2\lib\msw" /D "NDEBUG" /D PLUGIN_EXPORT= /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "_LIB" /YX"wx\wxprec.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"./devel/sdk.lib"

!ELSEIF  "$(CFG)" == "sdk - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sdk___Win32_Debug0"
# PROP BASE Intermediate_Dir "sdk___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./devel/vc6/sdk/debug"
# PROP Intermediate_Dir "./devel/vc6/sdk/debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "e:\devcode\wxWindows-2.4.2\lib\mswd" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D PLUGIN_EXPORT= /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "_LIB" /D "__WX__" /YX"wx\wxprec.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"./devel/sdkd.lib"

!ENDIF 

# Begin Target

# Name "sdk - Win32 Release"
# Name "sdk - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sdk\cbeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\cbplugin.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\cbproject.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\compileoptionsbase.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\compilerfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\compileroptions.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\compiletargetbase.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\configmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\configuretoolsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\confirmreplacedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editarrayfiledlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editarrayorderdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editarraystringdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editorcolorset.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editorconfigurationdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\editormanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\edittooldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\filegroupsandmasks.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\finddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\incrementalselectlistdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\macrosmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\manager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\menuitemsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\messagelog.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\messagemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\newfromtemplatedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\pipedprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\pluginmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\pluginsconfigurationdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectbuildtarget.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectfileoptionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectlayoutloader.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectloader.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectoptionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projectsfilemasksdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\projecttemplateloader.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\replacedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\sdk_events.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\selecttargetdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\simplelistlog.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\simpletextlog.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\templatemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\toolsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\workspaceloader.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\sdk\cbeditor.h
# End Source File
# Begin Source File

SOURCE=.\sdk\cbplugin.h
# End Source File
# Begin Source File

SOURCE=.\sdk\cbproject.h
# End Source File
# Begin Source File

SOURCE=.\sdk\compileoptionsbase.h
# End Source File
# Begin Source File

SOURCE=.\sdk\compiler.h
# End Source File
# Begin Source File

SOURCE=.\sdk\compilerfactory.h
# End Source File
# Begin Source File

SOURCE=.\sdk\compileroptions.h
# End Source File
# Begin Source File

SOURCE=.\sdk\compiletargetbase.h
# End Source File
# Begin Source File

SOURCE=.\sdk\configmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\configuretoolsdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\confirmreplacedlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editarrayfiledlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editarrayorderdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editarraystringdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editorcolorset.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editorconfigurationdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\editormanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\edittooldlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\filegroupsandmasks.h
# End Source File
# Begin Source File

SOURCE=.\sdk\finddlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\findreplacebase.h
# End Source File
# Begin Source File

SOURCE=.\sdk\globals.h
# End Source File
# Begin Source File

SOURCE=.\sdk\incrementalselectlistdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\licenses.h
# End Source File
# Begin Source File

SOURCE=.\sdk\macrosmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\manager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\menuitemsmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\messagelog.h
# End Source File
# Begin Source File

SOURCE=.\sdk\messagemanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\newfromtemplatedlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\pipedprocess.h
# End Source File
# Begin Source File

SOURCE=.\sdk\pluginmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\pluginsconfigurationdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectbuildtarget.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectfileoptionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectlayoutloader.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectloader.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectoptionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projectsfilemasksdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\projecttemplateloader.h
# End Source File
# Begin Source File

SOURCE=.\sdk\replacedlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\sdk_events.h
# End Source File
# Begin Source File

SOURCE=.\sdk\selecttargetdlg.h
# End Source File
# Begin Source File

SOURCE=.\sdk\settings.h
# End Source File
# Begin Source File

SOURCE=.\sdk\simplelistlog.h
# End Source File
# Begin Source File

SOURCE=.\sdk\simpletextlog.h
# End Source File
# Begin Source File

SOURCE=.\sdk\templatemanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\toolsmanager.h
# End Source File
# Begin Source File

SOURCE=.\sdk\workspaceloader.h
# End Source File
# End Group
# End Target
# End Project
