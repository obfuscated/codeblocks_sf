# Microsoft Developer Studio Project File - Name="test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "test.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "test.mak" CFG="test - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "test - Win32 DebugUniv" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 ReleaseUniv" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 DebugDev" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 ReleaseDev" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 DebugUnicode" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 ReleaseUnicode" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test - Win32 DebugUniv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUniv"
# PROP BASE Intermediate_Dir "DebugUniv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUniv"
# PROP Intermediate_Dir "DebugUniv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "..\..\include" /I "$(WXDEVEL)\include" /I "$(WXDEVEL)\lib\vc_lib\mswunivd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WXUNIVERSAL__" /D "__WXMSW__" /FR /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x807 /i "$(WXDEVEL)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wsock32.lib wxbase28d.lib wxbase28d_net.lib wxbase28d_odbc.lib wxbase28d_xml.lib wxexpatd.lib wxjpegd.lib wxmswuniv28d_adv.lib wxmswuniv28d_core.lib wxmswuniv28d_dbgrid.lib wxmswuniv28d_gl.lib wxmswuniv28d_html.lib wxpngd.lib wxregexd.lib wxtiffd.lib wxzlibd.lib wxscintillad_univ.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"$(WXDEVEL)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "test - Win32 ReleaseUniv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUniv"
# PROP BASE Intermediate_Dir "ReleaseUniv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUniv"
# PROP Intermediate_Dir "ReleaseUniv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /Gi /GR /GX /O2 /I "..\..\include" /I "$(WXDEVEL)\include" /I "$(WXDEVEL)\lib\vc_lib\mswuniv" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXUNIVERSAL__" /D "__WXMSW__" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG"
# ADD RSC /l 0x807 /i "$(WXDEVEL)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase28.lib wxbase28_net.lib wxbase28_odbc.lib wxbase28_xml.lib wxexpat.lib wxjpeg.lib wxmswuniv28_adv.lib wxmswuniv28_core.lib wxmswuniv28_dbgrid.lib wxmswuniv28_gl.lib wxmswuniv28_html.lib wxpng.lib wxregex.lib wxtiff.lib wxzlib.lib wxscintilla_univ.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT.lib" /libpath:"$(WXDEVEL)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "test - Win32 DebugDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDev"
# PROP BASE Intermediate_Dir "DebugDev"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDev"
# PROP Intermediate_Dir "DebugDev"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "..\..\include" /I "$(WXDEVEL)\include" /I "$(WXDEVEL)\lib\vc_lib\mswd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WXMSW__" /FR /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x807 /i "$(WXDEVEL)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wsock32.lib wxbase28d.lib wxbase28d_net.lib wxbase28d_odbc.lib wxbase28d_xml.lib wxexpatd.lib wxjpegd.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxmsw28d_dbgrid.lib wxmsw28d_gl.lib wxmsw28d_html.lib wxpngd.lib wxregexd.lib wxtiffd.lib wxzlibd.lib wxscintillad_dev.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"$(WXDEVEL)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "test - Win32 ReleaseDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDev"
# PROP BASE Intermediate_Dir "ReleaseDev"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDev"
# PROP Intermediate_Dir "ReleaseDev"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /Gi /GR /GX /O2 /I "..\..\include" /I "$(WXDEVEL)\include" /I "$(WXDEVEL)\lib\vc_lib\msw" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXMSW__" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG"
# ADD RSC /l 0x807 /i "$(WXDEVEL)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase28.lib wxbase28_net.lib wxbase28_odbc.lib wxbase28_xml.lib wxexpat.lib wxjpeg.lib wxmsw28_adv.lib wxmsw28_core.lib wxmsw28_dbgrid.lib wxmsw28_gl.lib wxmsw28_html.lib wxpng.lib wxregex.lib wxtiff.lib wxzlib.lib wxscintilla_dev.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT.lib" /libpath:"$(WXDEVEL)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "test - Win32 DebugUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUnicode"
# PROP BASE Intermediate_Dir "DebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "..\..\include" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_lib\mswud" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WXMSW__" /D "_UNICODE" /FR /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x807 /i "$(WXWIN)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wsock32.lib wxbase28ud.lib wxbase28ud_net.lib wxbase28ud_odbc.lib wxbase28ud_xml.lib wxexpatd.lib wxjpegd.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxmsw28ud_dbgrid.lib wxmsw28ud_gl.lib wxmsw28ud_html.lib wxpngd.lib wxregexud.lib wxtiffd.lib wxzlibd.lib wxscintillaud.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"$(WXWIN)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "test - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUnicode"
# PROP BASE Intermediate_Dir "ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /Gi /GR /GX /O2 /I "..\..\include" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_lib\mswu" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXMSW__" /D "_UNICODE" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG"
# ADD RSC /l 0x807 /i "$(WXWIN)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase28u.lib wxbase28u_net.lib wxbase28u_odbc.lib wxbase28u_xml.lib wxexpat.lib wxjpeg.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxmsw28u_dbgrid.lib wxmsw28u_gl.lib wxmsw28u_html.lib wxpng.lib wxregexu.lib wxtiff.lib wxzlib.lib wxscintillau.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT.lib" /libpath:"$(WXWIN)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /map /debug

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /ZI /Od /I "..\..\include" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_lib\mswd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WXMSW__" /FR /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x807 /i "$(WXWIN)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wsock32.lib wxbase28d.lib wxbase28d_net.lib wxbase28d_odbc.lib wxbase28d_xml.lib wxexpatd.lib wxjpegd.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxmsw28d_dbgrid.lib wxmsw28d_gl.lib wxmsw28d_html.lib wxpngd.lib wxregexd.lib wxtiffd.lib wxzlibd.lib wxscintillad.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"$(WXWIN)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /incremental:no

!ELSEIF  "$(CFG)" == "test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /Gi /GR /GX /O2 /I "..\..\include" /I "$(WXWIN)\include" /I "$(WXWIN)\lib\vc_lib\msw" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXMSW__" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG"
# ADD RSC /l 0x807 /i "$(WXWIN)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxbase28.lib wxbase28_net.lib wxbase28_odbc.lib wxbase28_xml.lib wxexpat.lib wxjpeg.lib wxmsw28_adv.lib wxmsw28_core.lib wxmsw28_dbgrid.lib wxmsw28_gl.lib wxmsw28_html.lib wxpng.lib wxregex.lib wxtiff.lib wxzlib.lib wxscintilla.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT.lib" /libpath:"$(WXWIN)\lib\vc_lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /profile /map /debug

!ENDIF

# Begin Target

# Name "test - Win32 DebugUniv"
# Name "test - Win32 ReleaseUniv"
# Name "test - Win32 DebugDev"
# Name "test - Win32 ReleaseDev"
# Name "test - Win32 DebugUnicode"
# Name "test - Win32 ReleaseUnicode"
# Name "test - Win32 Debug"
# Name "test - Win32 Release"
# Begin Source File

SOURCE=.\defsext.h
# End Source File
# Begin Source File

SOURCE=.\edit.cpp
# End Source File
# Begin Source File

SOURCE=.\edit.h
# End Source File
# Begin Source File

SOURCE=.\prefs.cpp
# End Source File
# Begin Source File

SOURCE=.\prefs.h
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=.\test.rc
# End Source File
# End Target
# End Project
