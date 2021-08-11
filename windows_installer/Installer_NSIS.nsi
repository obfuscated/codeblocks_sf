# Debugging:
;!define BUILD_TYPE 64
;!define NIGHTLY_BUILD_SVN 12492_PLUS

#####################################################################
# The installer is divided into 5 main sections (section groups):   #
# "Default"           -> includes C::B core and core plugins        #
# "Lexers"            -> includes C::B lexers for different langs   #
# "Contrib plugins"   -> includes C::B contrib plugins              #
# "C::B share config" -> includes the C::B Share Config tool        #
# "C::B Launcher"     -> includes CbLauncher                        #
#####################################################################
# What to do to add a new installer section (e.g. a new plugin):    #
# 1.) Add Installer section                                         #
# 2.) Add Uninstaller section (files in reverse order of installer) #
# 3.) Add macro to uninstaller functions                            #
# 4.) Add section description                                       #
# --> Basically you need to add stuff at 4 places! :-)              #
#####################################################################
# To compile this script:                                           #
#                                                                   #
# 1) Download and install NSIS (v3) from here:                      #
#      http://nsis.sourceforge.net/Download                         #
#                                                                   #
# 2) Download and install the Ultra-Modern UI:                      #
#      https://github.com/SuperPat45/UltraModernUI                  #
#                                                                   #
# 3) Update the following or check out the Build_NSIS_64bit.bat     #
#       file:                                                       #
#    * BUILD_TYPE for 32 or 64 bit                                  #
#    * BUILD_TYPE for 32 or 64 bit                                  #
#                                                                   #
# 4) run NSIS using this command line or via the MakeNSISW.exe GUI  #
#   C:\PATH_TO\NSIS\makensis.exe setup.nsi                          #
# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  #
# You may also need to adjust "RequestExecutionLevel admin/user" ,  #
# see below.                                                        #
# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  #
# NOTE: The latest 32 bit wget zip is available from :              #
#   https://eternallybored.org/misc/wget/                           #
#####################################################################

Name CodeBlocks
XPStyle on
Unicode True

# Enable logging when using the NSIS special build
!define ENABLE_LOGGING

#########################################################
# Room for adjustments of most important settings BEGIN #
#########################################################
# The following line defined if the build is for 32 or 64 bits
!ifdef BUILD_TYPE
  !if ${BUILD_TYPE} == 32
    !undef BUILD_TYPE
    !define BUILD_TYPE 32
  !else
    !undef BUILD_TYPE
    !define BUILD_TYPE 64
  !endif
!else
    !define BUILD_TYPE 64
!endif


# The following line defined if the build is a nightly build and it's the SVN number
# if not defined the version will default to YY.MM (year:month)
!ifdef NIGHTLY_BUILD_SVN
  !if ${NIGHTLY_BUILD_SVN} == "False"
    !undef NIGHTLY_BUILD_SVN
  !endif
!else
  !define NIGHTLY_BUILD_SVN 12492
!endif

# Possibly required to adjust manually:
# Note: a) These files are only required for the installer.
#       b) These are in the ${CB_INSTALL_GRAPHICS_DIR} directory
!ifdef NIGHTLY_BUILD_SVN
    !define CB_SPLASH_FILENAME  setup_splash_nightly.bmp
!else
    !define CB_SPLASH_FILENAME  setup_splash_2003.bmp
!endif
!define CB_LOGO_FILENAME        setup_logo_2003.bmp

#########################################################
# Room for adjustments of most important settings END   #
#########################################################

##############################
# Included NSIS script files #
##############################
!include LogicLib.nsh
!include "UMUI.nsh"             # UMUI - Ultra Modern UI
!include Sections.nsh
!include x64.nsh
!include WinVer.nsh

# WARNING: This is very SLOW if enabled, but it reduces the output exe by about 20%!!
#SetCompressor /SOLID LZMA

###########################
# CB BUILD Folder DEFINES #
###########################
!if ${BUILD_TYPE} == 32
    !define CB_BASE          .\..\src\output31
    !define WX_BASE          .\..\src\output31
    #!define WX_BASE          D:\Devel\CodeBlocks\Releases\CodeBlocks_2003
!else
    !define CB_BASE          .\..\src\output31_64
    !define WX_BASE          .\..\src\output31_64
    #!define WX_BASE          D:\Devel\CodeBlocks\Releases\CodeBlocks_2003
!endif
!define CB_SHARE         \share
!define CB_SHARE_CB      ${CB_SHARE}\CodeBlocks
!define CB_DOCS          ${CB_SHARE_CB}\docs
!define CB_LEXERS        ${CB_SHARE_CB}\lexers
!define CB_PLUGINS       ${CB_SHARE_CB}\plugins
!define CB_SCRIPTS       ${CB_SHARE_CB}\scripts
!define CB_SCTESTS       ${CB_SCRIPTS}\tests
!define CB_TEMPLATES     ${CB_SHARE_CB}\templates
!define CB_WIZARD        ${CB_TEMPLATES}\wizard
!define CB_IMAGES        ${CB_SHARE_CB}\images
!define CB_IMG_SETTINGS  ${CB_IMAGES}\settings
!define CB_XML_COMPILERS ${CB_SHARE_CB}\compilers

###########################
#   NSIS Folder DEFINES   #
###########################
# These are the NSIS source directories
!define CB_INSTALL_DICTIONARIES_DIR     .\Dictionaries
!define CB_INSTALL_DOCUMENTATION_DIR    .\Documentation
!define CB_INSTALL_GRAPHICS_DIR         .\Graphics
!define CB_INSTALL_LICENSES_DIR         .\Licenses
!define CB_INSTALL_URLS_DIR             .\URLS

###########################
#        BRANDING         #
###########################
BrandingText "Code::Blocks"

###########################
#       DATE Defines      #
###########################
# Get Current Date into CURRENT_DATESTAMP variable
!define /date CURRENT_DATESTAMP "%d%b%Y"
!define /date CURRENT_DATE_YEAR "%Y"
!define /date CURRENT_DATE_YEAR_NO_CENTURY "%y"
!define /date CURRENT_DATE_MONTH "%m"
!define /date CURRENT_DATE_DAY "%d"

################################################
# CB SPECIFIC INSTALLER CONFIGURATION  Defines #
################################################
!define REGKEY           "SOFTWARE\$(^Name)"
!ifdef NIGHTLY_BUILD_SVN
    !define VERSION      "SVN-${NIGHTLY_BUILD_SVN}"
!else
    #!define VERSION     ${CURRENT_DATE_YEAR_NO_CENTURY}.${CURRENT_DATE_MONTH}
!endif
!define COMPANY          "The Code::Blocks Team"
!define URL              http://www.codeblocks.org
!define CB_SPLASH        ${CB_INSTALL_GRAPHICS_DIR}\${CB_SPLASH_FILENAME}
!define CB_LOGO          ${CB_INSTALL_GRAPHICS_DIR}\${CB_LOGO_FILENAME}
!define CB_LICENSE       ${CB_INSTALL_LICENSES_DIR}\gpl-3.0.txt

# Installer attributes (usually these do not change)
# Note: We can't always use "Code::Blocks" as the "::" conflicts with the file system.
!if ${BUILD_TYPE} == 32
    OutFile             CodeBlocks-${VERSION}-32bit-setup-${CURRENT_DATESTAMP}-NSIS.exe
!else
    OutFile             CodeBlocks-${VERSION}-64bit-setup-${CURRENT_DATESTAMP}-NSIS.exe
!endif

Caption           "Code::Blocks ${VERSION} ${CURRENT_DATE_YEAR}.${CURRENT_DATE_MONTH}.${CURRENT_DATE_DAY}.0 Installation"
CRCCheck          on
XPStyle           on
ShowInstDetails   show
VIProductVersion  ${CURRENT_DATE_YEAR}.${CURRENT_DATE_MONTH}.${CURRENT_DATE_DAY}.0
VIAddVersionKey   ProductName     "Code::Blocks"
VIAddVersionKey   ProductVersion  "${VERSION}"
VIAddVersionKey   CompanyName     "${COMPANY}"
VIAddVersionKey   CompanyWebsite  "${URL}"
VIAddVersionKey   FileVersion     "${VERSION}"
VIAddVersionKey   FileDescription "Code::Blocks cross-platform IDE"
VIAddVersionKey   LegalCopyright  "Code::Blocks Team"
UninstallCaption  "Code::Blocks Uninstallation"
ShowUninstDetails show

###############################################################
#   NSIS PAGE GUI Defines (SEE NSIS and Ultra-Modern UI docs) #
###############################################################
# Interface configuration
!define MUI_ICON                     "${CB_INSTALL_GRAPHICS_DIR}\setup_icon.ico"
#!define MUI_HEADERIMAGE
#!define MUI_HEADERIMAGE_BITMAP       "${CB_LOGO}"
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define UMUI_USE_INSTALLOPTIONSEX


!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN           "$INSTDIR\codeblocks.exe"
; !define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME    "$INSTDIR${CB_DOCS}\manual_codeblocks_en.pdf"
!define MUI_FINISHPAGE_SHOWREADME_TEXT   "Open Code::Blocks manual"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_LINK          "${URL}"
!define MUI_FINISHPAGE_LINK_LOCATION "${URL}"
!define MUI_UNICON                   "${CB_INSTALL_GRAPHICS_DIR}\setup_icon.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

!define UMUI_LEFTIMAGE_BMP "${CB_INSTALL_GRAPHICS_DIR}\setup_1.bmp"
!define UMUI_HEADERBGIMAGE_BMP  "${CB_LOGO}"
!define UMUI_SKIN "SoftBlue"
!define UMUI_PARAMS_REGISTRY_ROOT HKCU
!define UMUI_PARAMS_REGISTRY_KEY  ${REGKEY}
!define UMUI_INSTALLDIR_REGISTRY_VALUENAME "InstallDir"    ;Replace the InstallDirRegKey instruction and automatically save the $INSTDIR variable
!define UMUI_VERSION ${VERSION}
!define /date UMUI_VERBUILD "${VERSION}_%Y-%m-%d HH:MM"
!define UMUI_VERSION_REGISTRY_VALUENAME "Version"
!define UMUI_VERBUILD_REGISTRY_VALUENAME "VerBuild"
!define UMUI_UNINSTALLPATH_REGISTRY_VALUENAME "uninstallpath"
!define UMUI_INSTALLERFULLPATH_REGISTRY_VALUENAME "installpath"
!define UMUI_UNINSTALL_FULLPATH "$INSTDIR\Uninstall.exe"
!define UMUI_PREUNINSTALL_FUNCTION preuninstall_function
!define UMUI_ABORTPAGE_LINK          "${URL}"
!define UMUI_ABORTPAGE_LINK_LOCATION "${URL}"
!define UMUI_TEXT_SETUPTYPE_MINIMAL_TITLE  "Minimal: Important plugins, important lexers"
!define UMUI_TEXT_SETUPTYPE_STANDARD_TITLE "Standard: Core plugins, core tools, and core lexers"
!define UMUI_TEXT_SETUPTYPE_COMPLETE_TITLE "Full: All plugins, all tools, just everything"

# Multiuser mode defines
!define MULTIUSER_INSTALLMODE_INSTDIR "$(^Name)"
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_INSTALLMODE_FUNCTION onMultiUserModeChanged
#!define MULTIUSER_INSTALLMODE_UNFUNCTION  onMultiUserModeChanged
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY ${REGKEY}
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME "MultiUserInstallMode"
#!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY ${REGKEY}
#!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME "MultiUserInstallMode"
!include MultiUser.nsh

# Reserved Files
ReserveFile "${NSISDIR}\Plugins\x86-unicode\AdvSplash.dll"

# Installer pages
Var STARTMENU_FOLDER_INSTALL
Var STARTMENU_FOLDER_UNINSTALL

!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_WELCOME
    !define UMUI_UPDATEPAGE_REMOVE
    !define UMUI_UPDATEPAGE_CONTINUE_SETUP
!insertmacro UMUI_PAGE_UPDATE
!insertmacro MUI_PAGE_LICENSE    ${CB_LICENSE}
    !define UMUI_SETUPTYPEPAGE_MINIMAL "$(UMUI_TEXT_SETUPTYPE_MINIMAL_TITLE)"
    !define UMUI_SETUPTYPEPAGE_STANDARD "$(UMUI_TEXT_SETUPTYPE_STANDARD_TITLE)"
    !define UMUI_SETUPTYPEPAGE_COMPLETE "$(UMUI_TEXT_SETUPTYPE_COMPLETE_TITLE)"
    !define UMUI_SETUPTYPEPAGE_DEFAULTCHOICE ${UMUI_COMPLETE}
#    !define UMUI_SETUPTYPE_REGISTRY_VALUENAME "SetupType"
!insertmacro UMUI_PAGE_SETUPTYPE
!insertmacro MUI_PAGE_COMPONENTS

!insertmacro MUI_PAGE_DIRECTORY

    ; DO NOT USE !define UMUI_ALTERNATIVESTARTMENUPAGE_USE_TREEVIEW
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
    !define UMUI_ALTERNATIVESTARTMENUPAGE_SETSHELLVARCONTEXT
    !define MUI_STARTMENUPAGE_DEFAULTFOLDER $(^Name)
!insertmacro UMUI_PAGE_ALTERNATIVESTARTMENU Application $STARTMENU_FOLDER_INSTALL

  !define UMUI_CONFIRMPAGE_TEXTBOX confirm_function
!insertmacro UMUI_PAGE_CONFIRM

!insertmacro MUI_PAGE_INSTFILES
Page Custom CompilerDownloadPage_Show CompilerDownloadPage_Leave
!insertmacro MUI_PAGE_FINISH

!insertmacro UMUI_PAGE_ABORT

# Un-Installer pages
!insertmacro MUI_UNPAGE_WELCOME
    !define UMUI_MAINTENANCEPAGE_MODIFY
    !define UMUI_MAINTENANCEPAGE_REPAIR
    !define UMUI_MAINTENANCEPAGE_REMOVE
    # !define UMUI_MAINTENANCEPAGE_CONTINUE_SETUP
    !define UMUI_MAINTENANCEPAGE_DEFAULTCHOICE ${UMUI_REMOVE}
!insertmacro UMUI_UNPAGE_MAINTENANCE
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro UMUI_UNPAGE_ABORT
# Installer languages
!insertmacro MUI_LANGUAGE "English" # first language is the default language

##########################################
# NSIS Installer Privilege CONFIGURATION #
##########################################
# Specifies the requested execution level for Windows Vista, 7, 8, 10 & 11
# The value is embedded in the installer and uninstaller's XML manifest
# and tells Vista/7/10, and probably future versions of Windows, what privileges
# level the installer requires.
# -> user  requests the a normal user's level with no administrative privileges
# -> admin requests administrator level and will cause Windows to prompt the
#    user to verify privilege escalation.
RequestExecutionLevel user

####################################################
# NSIS CUSTOM COMPILER DOWNLOAD PAGE CONFIGURATION #
####################################################
ReserveFile NSIS_CompilerDownload.ini
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

# ========================================================================================================================
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# Changes are usually NOT required below # Changes are usually NOT required below # Changes are usually NOT required below #
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# ========================================================================================================================

############################
# Check for Fortran Plugin #
############################
# This is required iof you are building from github instead of source forge source repo trunk/branch/tag!!

; See http://nsis.sourceforge.net/Check_if_a_file_exists_at_compile_time for documentation
!macro !defineifexist _VAR_NAME _FILE_NAME
	!tempfile _TEMPFILE
    !if /FileExists "${_FILE_NAME}"
       !echo "!define ${_VAR_NAME}" > "${_TEMPFILE}"
    !endif
;	!ifdef NSIS_WIN32_MAKENSIS
;		; Windows - cmd.exe
;		!system 'if exist "${_FILE_NAME}" echo !define ${_VAR_NAME} > "${_TEMPFILE}"'
;	!else
;		; Posix - sh
;		!system 'if [ -e "${_FILE_NAME}" ]; then echo "!define ${_VAR_NAME}" > "${_TEMPFILE}"; fi'
;	!endif
	!include '${_TEMPFILE}'
	!delfile '${_TEMPFILE}'
	!undef _TEMPFILE
!macroend
!define !defineifexist "!insertmacro !defineifexist"

${!defineifexist} FORTRAN_PLUGIN_FOUND ${CB_BASE}${CB_SHARE_CB}\FortranProject.zip

################################################################################
# Logging macro - from https://nsis.sourceforge.io/Logging:Enable_Logs_Quickly #
################################################################################
!define LogSet "!insertmacro LogSetMacro"
!macro LogSetMacro SETTING
  !ifdef ENABLE_LOGGING
    LogSet ${SETTING}
  !endif
!macroend
 
!define LogText "!insertmacro LogTextMacro"
!macro LogTextMacro INPUT_TEXT
  !ifdef ENABLE_LOGGING
    LogText ${INPUT_TEXT}
  !endif
!macroend
######################
# Installer sections #
######################

# These are the installer types.
# They basically wrap different selections of components.
# To use a component in section "Full" (=1) and "Edit" (=4) choose
# "SectionIn 1 4" in the section(s) accordingly (see how it's done below).
InstType "$(UMUI_TEXT_SETUPTYPE_COMPLETE_TITLE)"               # 1
InstType "$(UMUI_TEXT_SETUPTYPE_STANDARD_TITLE)"               # 2 
InstType "$(UMUI_TEXT_SETUPTYPE_MINIMAL_TITLE)"                # 3
InstType "Editor: Code::Blocks as editor only (all lexers)"    # 4 Select custom then this in the drop down.

# Now the installer sections and section groups start.
# They basically define the tree of components available.
SectionGroup "!Default install" SECGRP_DEFAULT

    # Short explanation:
    # Section    "!Name" -> Section is bold
    # Section /o "Name"  -> Section is optional and not selectd by default
    # Section    "-Name" -> Section is hidden an cannot be unselected

    # C::B core begin

    Section "!Core Files (required)" SEC_CORE
        SectionIn 1 2 3 4 RO
        SetOutPath $INSTDIR
        # Verify if creating/accessing the target folder succeeded.
        # If not, issue an error message and abort installation
        IfErrors 0 accessOK
            MessageBox MB_OK|MB_ICONEXCLAMATION \
                "Cannot create the target folder.$\r$\nInstallation cannot continue.$\r$\nMake sure that you have the required file access permissions." \
                /SD IDOK
            DetailPrint "Aborting installation."
            Abort
        accessOK:
            SetOverwrite on
            File ${WX_BASE}\wxmsw*u_gcc_cb.dll
            File ${WX_BASE}\wxmsw*u_gl_gcc_cb.dll
            File ${CB_BASE}\Addr2LineUI.exe
            File ${CB_BASE}\cb_console_runner.exe
            File ${CB_BASE}\CbLauncher.exe
            #File ${CB_BASE}\cctest.exe   # CB developer testing only
            File ${CB_BASE}\codeblocks.dll
            File ${CB_BASE}\codeblocks.exe
            # MinGW DLL's for thread handling etc.
!if ${BUILD_TYPE} == 64
            File ${CB_BASE}\libgcc_s_seh-1.dll
!else
            File ${CB_BASE}\libgcc_s_dw2-1.dll
!endif
            File ${CB_BASE}\libstdc++-6.dll
            File ${CB_BASE}\libwinpthread-1.dll
            
            ${If} ${IsWinXP}
                # crash handler for Windows XP!!!!
                ; File ${CB_BASE}\dbgcore.dll - N/A for XP
                !if ${BUILD_TYPE} == 64
                    File exchndl_xp\win64\dbghelp.dll
                    File exchndl_xp\win64\exchndl.dll
                    File exchndl_xp\win64\mgwhelp.dll
                    File exchndl_xp\win64\symsrv.dll
                    File exchndl_xp\win64\symsrv.yes
                !else
                    File exchndl_xp\win32\dbghelp.dll
                    File exchndl_xp\win32\exchndl.dll
                    File exchndl_xp\win32\mgwhelp.dll
                    File exchndl_xp\win32\symsrv.dll
                    File exchndl_xp\win32\symsrv.yes
                !endif
            ${Else}
                # crash handler for Win 7+  (fails on XP!!!!)
                File ${CB_BASE}\dbgcore.dll
                File ${CB_BASE}\dbghelp.dll
                File ${CB_BASE}\exchndl.dll
                File ${CB_BASE}\mgwhelp.dll
                File ${CB_BASE}\symsrv.dll
                File ${CB_BASE}\symsrv.yes
            ${EndIf}
            # Licenses
            File ${CB_INSTALL_LICENSES_DIR}\gpl-3.0.txt
            File ${CB_INSTALL_LICENSES_DIR}\lgpl-3.0.txt
            # WGET
            File wget.exe
            SetOutPath $INSTDIR${CB_SHARE_CB}
            File ${CB_BASE}${CB_SHARE_CB}\start_here.zip
            File ${CB_BASE}${CB_SHARE_CB}\tips.txt
            File ${CB_BASE}${CB_SHARE_CB}\manager_resources.zip
            File ${CB_BASE}${CB_SHARE_CB}\resources.zip
            SetOutPath $INSTDIR${CB_DOCS}
            File ${CB_INSTALL_DOCUMENTATION_DIR}\index.ini
            File ${CB_INSTALL_DOCUMENTATION_DIR}\manual_codeblocks_en.chm
            File ${CB_INSTALL_DOCUMENTATION_DIR}\manual_codeblocks_en.pdf
            File ${CB_INSTALL_DOCUMENTATION_DIR}\manual_codeblocks_fr.chm
            File ${CB_INSTALL_DOCUMENTATION_DIR}\manual_codeblocks_fr.pdf
            File ${CB_INSTALL_DOCUMENTATION_DIR}\Manual_wxPBGuide.pdf
            SetOutPath $INSTDIR${CB_SCRIPTS}
            File ${CB_BASE}${CB_SCRIPTS}\*.script
            SetOutPath $INSTDIR${CB_SCTESTS}
            File ${CB_BASE}${CB_SCTESTS}\*.script
            SetOutPath $INSTDIR${CB_TEMPLATES}
            File ${CB_BASE}${CB_TEMPLATES}\*.*
            SetOutPath $INSTDIR${CB_IMAGES}
            File ${CB_BASE}${CB_IMAGES}\*.png
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\*.png
            WriteRegStr HKCU "${REGKEY}\Components" "Core Files (required)" 1
    SectionEnd

    # C::B core end

    # C::B shortcuts begin

    SectionGroup "Shortcuts" SECGRP_SHORTCUTS

        Section "Program Shortcut" SEC_PROGRAMSHORTCUT
            SectionIn 1 2 3 4
            SetOutPath $SMPROGRAMS\$STARTMENU_FOLDER_INSTALL
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) Share Config.lnk"         $INSTDIR\cb_share_config.exe
            #CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) Code Completion Test.lnk" $INSTDIR\cctest.exe # CB developer testing only
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) Address to Line GUI.lnk"  $INSTDIR\Addr2LineUI.exe

            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) PDF Manual English.lnk" $INSTDIR${CB_DOCS}\manual_codeblocks_en.pdf "" "" 0 SW_SHOWNORMAL  "" "The Code::Blocks PDF User Manual in English"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) CHM Manual English.lnk" $INSTDIR${CB_DOCS}\manual_codeblocks_en.chm "" "" 0 SW_SHOWNORMAL  "" "The Code::Blocks CHM User Manual in English"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) PDF Manual French.lnk"  $INSTDIR${CB_DOCS}\manual_codeblocks_fr.pdf "" "" 0 SW_SHOWNORMAL  "" "The Code::Blocks PDF User Manual in French"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) CHM Manual French.lnk"  $INSTDIR${CB_DOCS}\manual_codeblocks_fr.chm "" "" 0 SW_SHOWNORMAL  "" "The Code::Blocks CHM User Manual in French"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\PBs wxWidgets Guide.lnk" $INSTDIR${CB_DOCS}\Manual_wxPBGuide.pdf "" "" 0 SW_SHOWNORMAL  "" "PBs GuiDe to Starting with wxWidgets with MinGW and Code::Blocks"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) License.lnk"                 "$INSTDIR\gpl-3.0.txt" "" "" 0 SW_SHOWNORMAL  "" "Code::Blocks license"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) SDK License.lnk"             "$INSTDIR\lgpl-3.0.txt" "" "" 0 SW_SHOWNORMAL  "" "Code::Blocks SDK license"
            SetOutPath $SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links\$(^Name) Web Site.lnk"              "http://www.codeblocks.org" "" "" 0 SW_SHOWNORMAL  "" "Go to Code::Blocks IDE website"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links\$(^Name) Forums.lnk"                "http://forums.codeblocks.org" "" "" 0 SW_SHOWNORMAL  "" "Go to Code::Blocks IDE discussion forums"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links\$(^Name) WiKi.lnk"                  "http://wiki.codeblocks.org" "" "" 0 SW_SHOWNORMAL  "" "Go to Code::Blocks IDE WiKi site"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links\$(^Name) Tickets.lnk"               "https://sourceforge.net/p/codeblocks/tickets/" "" "" 0 SW_SHOWNORMAL  "" "Report bugs/enhancements for Code::Blocks"
            CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Links\$(^Name) beginner instructions.lnk" "http://www.sci.brooklyn.cuny.edu/~goetz/codeblocks/codeblocks-instructions.pdf" "" "" 0 SW_SHOWNORMAL  "" "Code::Blocks beginner install and user guide"

            WriteRegStr HKCU "${REGKEY}\Components" "Program Shortcut" 1
        SectionEnd

        Section "Desktop Shortcut" SEC_DESKTOPSHORTCUT
            SectionIn 1
            CreateShortCut "$DESKTOP\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            WriteRegStr HKCU "${REGKEY}\Components" "Desktop Shortcut" 1
        SectionEnd

        Section "Quick Launch Shortcut" SEC_QUICKLAUNCHSHORTCUT
            SectionIn 1
            CreateShortCut "$QUICKLAUNCH\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            WriteRegStr HKCU "${REGKEY}\Components" "Quick Launch Shortcut" 1
        SectionEnd

    SectionGroupEnd

    # C::B shortcuts end

    # C::B lexers begin

    SectionGroup "Lexers" SECGRP_LEXERS

        SectionGroup "Compiler Languages"
            Section "C/C++"
                SectionIn 1 2 3 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_cpp.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_cpp.xml
                File ${CB_BASE}${CB_LEXERS}\lexer_rc.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_rc.xml
                WriteRegStr HKCU "${REGKEY}\Components" "C/C++" 1
            SectionEnd

            Section "Ada"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_ada.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_ada.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Ada" 1
            SectionEnd

            Section "The D Language"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_d.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_d.xml
                WriteRegStr HKCU "${REGKEY}\Components" "The D Language" 1
            SectionEnd

            Section "Fortran"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_f77.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_f77.xml
                File ${CB_BASE}${CB_LEXERS}\lexer_fortran.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_fortran.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Fortran" 1
            SectionEnd

            Section "Java"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_java.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_java.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Java" 1
            SectionEnd

            Section "JavaScript"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_javascript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_javascript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "JavaScript" 1
            SectionEnd

            Section "Objective-C"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_objc.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_objc.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Objective-C" 1
            SectionEnd

            Section "Pascal"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_pascal.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_pascal.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Pascal" 1
            SectionEnd

            Section "Smalltalk"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_smalltalk.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_smalltalk.xml
                WriteRegStr HKCU "${REGKEY}\Components" "SmallTalk" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Script Languages"
            Section "Angelscript"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_angelscript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_angelscript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Angelscript" 1
            SectionEnd

            Section "AutoTools"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_autotools.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_autotools.xml
                WriteRegStr HKCU "${REGKEY}\Components" "AutoTools" 1
            SectionEnd

            Section "Caml"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_caml.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_caml.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Caml" 1
            SectionEnd

            Section "Coffee"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_coffee.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_coffee.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Coffee" 1
            SectionEnd

            Section "Game Monkey"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_gm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_gm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Game Monkey" 1
            SectionEnd

            Section "Haskell"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_haskell.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_haskell.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Haskell" 1
            SectionEnd

            Section "Lisp"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_lisp.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_lisp.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Lisp" 1
            SectionEnd

            Section "Lua"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_lua.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_lua.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Lua" 1
            SectionEnd

            Section "Nim"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_nim.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_nim.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Nim" 1
            SectionEnd

            Section "Perl"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_perl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_perl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Perl" 1
            SectionEnd

            Section "Postscript"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_postscript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_postscript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Postscript" 1
            SectionEnd

            Section "Python"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_python.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_python.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Python" 1
            SectionEnd

            Section "Ruby"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_ruby.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_ruby.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Ruby" 1
            SectionEnd

            Section "Squirrel"
                SectionIn 1 2 3 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_squirrel.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_squirrel.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Squirrel" 1
            SectionEnd

            Section "VB Script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_vbscript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_vbscript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "VB Script" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Markup Languages"
            Section "BiBTeX"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_bibtex.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_bibtex.xml
                WriteRegStr HKCU "${REGKEY}\Components" "BiBTeX" 1
            SectionEnd

            Section "CSS"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_css.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_css.xml
                WriteRegStr HKCU "${REGKEY}\Components" "CSS" 1
            SectionEnd

            Section "HTML"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_html.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_html.xml
                WriteRegStr HKCU "${REGKEY}\Components" "HTML" 1
            SectionEnd

            Section "LaTeX"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_latex.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_latex.xml
                WriteRegStr HKCU "${REGKEY}\Components" "LaTeX" 1
            SectionEnd

            Section "Markdown"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_markdown.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_markdown.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Markdown" 1
            SectionEnd

            Section "XML"
                SectionIn 1 2 3 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_xml.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_xml.xml
                WriteRegStr HKCU "${REGKEY}\Components" "XML" 1
            SectionEnd

            Section "YAML"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_yaml.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_yaml.xml
                WriteRegStr HKCU "${REGKEY}\Components" "YAML" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Graphics Programming"
            Section "CUDA"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_cu.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_cu.xml
                WriteRegStr HKCU "${REGKEY}\Components" "CUDA" 1
            SectionEnd

            Section "GLSL (GLslang)"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_glsl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_glsl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "GLSL (GLslang)" 1
            SectionEnd

            Section "nVidia Cg"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_cg.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_cg.xml
                WriteRegStr HKCU "${REGKEY}\Components" "nVidia Cg" 1
            SectionEnd

            Section "Ogre"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_OgreCompositor.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_OgreCompositor.xml
                File ${CB_BASE}${CB_LEXERS}\lexer_OgreMaterial.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_OgreMaterial.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Ogre" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Embedded development"
            Section "A68k Assembler"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_A68k.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_A68k.xml
                WriteRegStr HKCU "${REGKEY}\Components" "A68k Assembler" 1
            SectionEnd

            Section "Hitachi Assembler"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_hitasm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_hitasm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Hitachi Assembler" 1
            SectionEnd

            Section "Intel HEX"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_ihex.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_ihex.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Intel HEX" 1
            SectionEnd

            Section "Motorola S-Record"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_srec.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_srec.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Motorola S-Record" 1
            SectionEnd

            Section "Tektronix extended HEX"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_tehex.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_tehex.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Tektronix extended HEX" 1
            SectionEnd

            Section "Verilog"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_verilog.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_verilog.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Verilog" 1
            SectionEnd

            Section "VHDL"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_vhdl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_vhdl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "VHDL" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Shell / Binutils"
            Section "bash script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_bash.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_bash.xml
                WriteRegStr HKCU "${REGKEY}\Components" "bash script" 1
            SectionEnd

            Section "DOS batch files"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_batch.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_batch.xml
                WriteRegStr HKCU "${REGKEY}\Components" "DOS batch files" 1
            SectionEnd

            Section "Windows registry file"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_registry.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_registry.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Windows registry file" 1
            SectionEnd

            Section "Cmake"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_cmake.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_cmake.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Cmake" 1
            SectionEnd

            Section "diff"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_diff.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_diff.xml
                WriteRegStr HKCU "${REGKEY}\Components" "diff" 1
            SectionEnd

            Section "Makefile"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_make.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_make.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Makefile" 1
            SectionEnd

            Section "PowerShell"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_powershell.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_powershell.xml
                WriteRegStr HKCU "${REGKEY}\Components" "PowerShell" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Others"
            Section "Google Protocol Buffer"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_proto.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Google Protocol Buffer" 1
            SectionEnd

            Section "InnoSetup"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_inno.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_inno.xml
                WriteRegStr HKCU "${REGKEY}\Components" "InnoSetup" 1
            SectionEnd

            Section "MASM"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_masm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_masm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "MASM" 1
            SectionEnd

            Section "Matlab"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_matlab.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_matlab.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Matlab" 1
            SectionEnd

            Section "NSIS installer script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_nsis.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_nsis.xml
                WriteRegStr HKCU "${REGKEY}\Components" "NSIS installer script" 1
            SectionEnd

            Section "Plain file"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_plain.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Plain file" 1
            SectionEnd

            Section "Property file"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_properties.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_properties.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Property file" 1
            SectionEnd

            Section "Sql"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_sql.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_sql.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Sql" 1
            SectionEnd

            Section "XBase"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_prg.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_prg.xml
                WriteRegStr HKCU "${REGKEY}\Components" "XBase" 1
            SectionEnd
        SectionGroupEnd
    SectionGroupEnd

    # C::B lexers end

    # C::B core plugins begin

    SectionGroup "Core Plugins" SECGRP_CORE_PLUGINS

        Section "Abbreviations plugin" SEC_ABBREV
            SectionIn 1 2 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\abbreviations.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\abbreviations.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\abbrev.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\abbrev-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "Abbreviations plugin" 1
        SectionEnd


        Section "AStyle plugin" SEC_ASTYLE
            SectionIn 1 2 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\astyle.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\astyle.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\astyle-plugin.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\astyle-plugin-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "AStyle plugin" 1
        SectionEnd

        Section "Autosave plugin" SEC_AUTOSAVE
            SectionIn 1 2 3 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\autosave.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\autosave.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\autosave.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\autosave-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "Autosave plugin" 1
        SectionEnd

        Section "Class Wizard plugin" SEC_CLASSWIZARD
            SectionIn 1 2
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\classwizard.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\classwizard.dll
            WriteRegStr HKCU "${REGKEY}\Components" "Class Wizard plugin" 1
        SectionEnd

        Section "Code Completion plugin" SEC_CODECOMPLETION
            SectionIn 1 2
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\codecompletion.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\codecompletion.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\codecompletion.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\codecompletion-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "Code Completion plugin" 1
        SectionEnd

        Section "Compiler plugin" SEC_COMPILER
            SectionIn 1 2 3
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\compiler.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\compiler.dll
            SetOutPath $INSTDIR${CB_XML_COMPILERS}
            File ${CB_BASE}${CB_XML_COMPILERS}\*.xml
            SetOutPath $INSTDIR${CB_IMAGES}
            File ${CB_BASE}${CB_IMAGES}\compile.png
            File ${CB_BASE}${CB_IMAGES}\compilerun.png
            File ${CB_BASE}${CB_IMAGES}\rebuild.png
            File ${CB_BASE}${CB_IMAGES}\stop.png
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\compiler.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\compiler-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "Compiler plugin" 1
        SectionEnd

        Section "Debugger plugin" SEC_DEBUGGER
            SectionIn 1 2 3
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\debugger.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\debugger.dll
            SetOutPath $INSTDIR${CB_IMAGES}
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\debugger.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\debugger-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "Debugger plugin" 1
        SectionEnd

        Section "MIME Handler plugin" SEC_MIMEHANDLER
            SectionIn 1 2 3 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\defaultmimehandler.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\defaultmimehandler.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\extensions.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\extensions-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "MIME Handler plugin" 1
        SectionEnd

        Section "Open Files List plugin" SEC_OPENFILESLIST
            SectionIn 1 2 3 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\openfileslist.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\openfileslist.dll
            WriteRegStr HKCU "${REGKEY}\Components" "Open Files List plugin" 1
        SectionEnd

        Section "Projects Importer plugin" SEC_PROJECTSIMPORTER
            SectionIn 1 2
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\projectsimporter.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\projectsimporter.dll
            WriteRegStr HKCU "${REGKEY}\Components" "Projects Importer plugin" 1
        SectionEnd

        Section "RND Generator plugin" SEC_RNDGEN
            SectionIn 1 2
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\rndgen.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\rndgen.dll
            WriteRegStr HKCU "${REGKEY}\Components" "RND Generator plugin" 1
        SectionEnd

        Section "Scripted Wizard plugin" SEC_SCRIPTEDWIZARD
            SectionIn 1 2 3
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\scriptedwizard.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\scriptedwizard.dll
            SetOutPath $INSTDIR${CB_WIZARD}
            File /r ${CB_BASE}${CB_WIZARD}\*.*
            WriteRegStr HKCU "${REGKEY}\Components" "Scripted Wizard plugin" 1
        SectionEnd

        Section "SmartIndent plugin" SEC_SMARTINDENT
            SectionIn 1 2 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentCpp.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentFortran.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentHDL.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentLua.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentPascal.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentPython.zip
            File ${CB_BASE}${CB_SHARE_CB}\SmartIndentXML.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentCpp.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentFortran.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentHDL.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentLua.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentPascal.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentPython.dll
            File ${CB_BASE}${CB_PLUGINS}\SmartIndentXML.dll
            WriteRegStr HKCU "${REGKEY}\Components" "SmartIndent plugin" 1
        SectionEnd

        Section "ToDo List plugin" SEC_TODOLIST
            SectionIn 1 2 3 4
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\todo.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\todo.dll
            SetOutPath $INSTDIR${CB_IMG_SETTINGS}
            File ${CB_BASE}${CB_IMG_SETTINGS}\todo.png
            File ${CB_BASE}${CB_IMG_SETTINGS}\todo-off.png
            WriteRegStr HKCU "${REGKEY}\Components" "ToDo List plugin" 1
        SectionEnd

        Section "XP Look And Feel plugin" SEC_XPLOOKANDFEEL
            SectionIn 1 2
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\xpmanifest.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\xpmanifest.dll
            WriteRegStr HKCU "${REGKEY}\Components" "XP Look And Feel plugin" 1
        SectionEnd

    SectionGroupEnd

    # C::B core plugins end

SectionGroupEnd

SectionGroup "Contrib Plugins" SECGRP_CONTRIB_PLUGINS

    # C::B contrib plugins begin

    Section "Auto Versioning plugin" SEC_AUTOVERSIONING
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\AutoVersioning.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\AutoVersioning.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Auto Versioning plugin" 1
    SectionEnd

    Section "Browse Tracker plugin" SEC_BROWSETRACKER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\BrowseTracker.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\BrowseTracker.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Browse Tracker plugin" 1
    SectionEnd

    Section "Byo Games plugin" SEC_BYOGAMES
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\byogames.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\byogames.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Byo Games plugin" 1
    SectionEnd

    Section "Cccc plugin" SEC_CCCC
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\Cccc.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\Cccc.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Cccc plugin" 1
    SectionEnd

    Section "Code Snippets plugin" SEC_CODESNIPPETS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\codesnippets.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\codesnippets.dll
        SetOutPath $INSTDIR${CB_IMAGES}\codesnippets
        File ${CB_BASE}${CB_IMAGES}\codesnippets\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "Code Snippets plugin" 1
    SectionEnd

    Section "Code Statistics plugin" SEC_CODESTAT
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\codestat.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\codestat.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\codestats.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\codestats-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Code Statistics plugin" 1
    SectionEnd

    Section "Copy Strings plugin" SEC_COPYSTRINGS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\copystrings.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\copystrings.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Copy Strings plugin" 1
    SectionEnd

    Section "CppCheck plugin" SEC_CPPCHECK
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\CppCheck.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\CppCheck.dll
        WriteRegStr HKCU "${REGKEY}\Components" "CppCheck plugin" 1
    SectionEnd

    Section "Cscope plugin" SEC_CSCOPE
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\Cscope.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\Cscope.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Cscope plugin" 1
    SectionEnd

    Section "DevPak plugin" SEC_DEVPAK
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\devpakupdater.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\devpakupdater.dll
        WriteRegStr HKCU "${REGKEY}\Components" "DevPak plugin" 1
    SectionEnd

    Section "DoxyBlocks plugin" SEC_DOXYBLOCKS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\DoxyBlocks.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\DoxyBlocks.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\DoxyBlocks.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\DoxyBlocks-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "DoxyBlocks plugin" 1
    SectionEnd


    Section "Drag Scroll plugin" SEC_DRAGSCROLL
        SectionIn 1 4
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\DragScroll.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\DragScroll.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\dragscroll.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\dragscroll-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Drag Scroll plugin" 1
    SectionEnd

    Section "EditorConfig plugin" SEC_EDITORCONFIG
        SectionIn 1 4
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\EditorConfig.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\EditorConfig.dll
        WriteRegStr HKCU "${REGKEY}\Components" "EditorConfig plugin" 1
    SectionEnd

    Section "Editor Tweaks plugin" SEC_EDITORTWEAKS
        SectionIn 1 4
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\EditorTweaks.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\EditorTweaks.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Editor tweaks plugin" 1
    SectionEnd

    Section "EnvVars plugin" SEC_ENVVARS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\envvars.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\envvars.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\envvars.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\envvars-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "EnvVars plugin" 1
    SectionEnd

    Section "File Manager plugin" SEC_FILEMANAGER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\FileManager.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\FileManager.dll
        WriteRegStr HKCU "${REGKEY}\Components" "File Manager plugin" 1
    SectionEnd

!ifdef FORTRAN_PLUGIN_FOUND
    Section "Fortran Project plugin" SEC_FORTRANPROJECT
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\FortranProject.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\FortranProject.dll
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject
        File ${CB_BASE}${CB_IMAGES}\fortranproject\*.dem
        File ${CB_BASE}${CB_IMAGES}\fortranproject\*.f90
        File ${CB_BASE}${CB_IMAGES}\fortranproject\*.py
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\16x16
        File ${CB_BASE}${CB_IMAGES}\fortranproject\16x16\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\20x20
        File ${CB_BASE}${CB_IMAGES}\fortranproject\20x20\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\24x24
        File ${CB_BASE}${CB_IMAGES}\fortranproject\24x24\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\28x28
        File ${CB_BASE}${CB_IMAGES}\fortranproject\28x28\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\32x32
        File ${CB_BASE}${CB_IMAGES}\fortranproject\32x32\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\40x40
        File ${CB_BASE}${CB_IMAGES}\fortranproject\40x40\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\48x48
        File ${CB_BASE}${CB_IMAGES}\fortranproject\48x48\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\56x56
        File ${CB_BASE}${CB_IMAGES}\fortranproject\56x56\*.png
        SetOutPath $INSTDIR${CB_IMAGES}\fortranproject\64x64
        File ${CB_BASE}${CB_IMAGES}\fortranproject\64x64\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "Fortran Project plugin" 1
    SectionEnd
!endif

    Section "HeaderFixUp plugin" SEC_HEADERFIXUP
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\headerfixup.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\headerfixup.dll
        WriteRegStr HKCU "${REGKEY}\Components" "HeaderFixUp plugin" 1
    SectionEnd

    Section "Help plugin" SEC_HELP
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\help_plugin.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\help_plugin.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\help-plugin.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\help-plugin-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Help plugin" 1
    SectionEnd

    Section "HexEditor plugin" SEC_HEXEDITOR
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\HexEditor.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\HexEditor.dll
        WriteRegStr HKCU "${REGKEY}\Components" "HexEditor plugin" 1
    SectionEnd

    Section "IncrementalSearch plugin" SEC_INCREMENTALSEARCH
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\IncrementalSearch.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\IncrementalSearch.dll
        WriteRegStr HKCU "${REGKEY}\Components" "IncrementalSearch plugin" 1
    SectionEnd

    Section "Key Binder plugin" SEC_KEYBINDER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\keybinder.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\keybinder.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\onekeytobindthem.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\onekeytobindthem-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Key Binder plugin" 1
    SectionEnd

    Section "Koders plugin" SEC_KODERS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\cb_koders.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\cb_koders.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Koders plugin" 1
    SectionEnd

    Section "Lib Finder plugin" SEC_LIBFINDER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\lib_finder.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\lib_finder.dll
        SetOutPath $INSTDIR${CB_SHARE_CB}\lib_finder
        File ${CB_BASE}${CB_SHARE_CB}\lib_finder\*.xml
        WriteRegStr HKCU "${REGKEY}\Components" "Lib Finder plugin" 1
    SectionEnd

    Section "MouseSap plugin" SEC_MOUSESAP
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\MouseSap.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\MouseSap.dll
        WriteRegStr HKCU "${REGKEY}\Components" "MouseSap plugin" 1
    SectionEnd

    Section "Nassi Shneiderman plugin" SEC_NASSI
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\NassiShneiderman.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\NassiShneiderman.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Nassi Shneiderman plugin" 1
    SectionEnd

    Section "Occurrences Highlighting plugin" SEC_OCC_HIGHLIGHTING
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\OccurrencesHighlighting.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\OccurrencesHighlighting.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Occurrences Highlighting plugin" 1
    SectionEnd

    Section "Tools+ plugin" SEC_TOOLSPLUS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\ToolsPlus.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\ToolsPlus.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Tools+ plugin" 1
    SectionEnd

    Section "Profiler plugin" SEC_PROFILER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\Profiler.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\Profiler.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\profiler.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\profiler-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Profiler plugin" 1
    SectionEnd

    Section "Project Options Manipulator plugin" SEC_PRJOPTSMANIP
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\ProjectOptionsManipulator.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\ProjectOptionsManipulator.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        WriteRegStr HKCU "${REGKEY}\Components" "Project Options Manipulator plugin" 1
    SectionEnd

    Section "RegEx Testbed plugin" SEC_REGEXTESTBED
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\RegExTestbed.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\RegExTestbed.dll
        WriteRegStr HKCU "${REGKEY}\Components" "RegEx Testbed plugin" 1
    SectionEnd

    Section "Reopen Editor plugin" SEC_REOPEN
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\ReopenEditor.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\ReopenEditor.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Reopen Editor plugin" 1
    SectionEnd

    Section "Exporter plugin" SEC_EXPORTER
        SectionIn 1 4
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\Exporter.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\Exporter.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Exporter plugin" 1
    SectionEnd

    Section "SpellChecker plugin" SEC_SPELLCHECKER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\SpellChecker.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\spellchecker.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\spellchecker-off.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\OnlineSpellChecking.xml
        File ${CB_INSTALL_DICTIONARIES_DIR}\en_GB.aff
        File ${CB_INSTALL_DICTIONARIES_DIR}\en_GB.dic
        File ${CB_INSTALL_DICTIONARIES_DIR}\en_US.aff
        File ${CB_INSTALL_DICTIONARIES_DIR}\en_US.dic
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\16x16
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\16x16\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\20x20
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\20x20\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\24x24
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\24x24\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\28x28
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\28x28\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\32x32
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\32x32\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\40x40
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\40x40\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\48x48
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\48x48\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\56x56
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\56x56\*.png
        SetOutPath $INSTDIR${CB_SHARE_CB}\SpellChecker\64x64
        File ${CB_BASE}${CB_SHARE_CB}\SpellChecker\64x64\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "SpellChecker plugin" 1
    SectionEnd

    Section "SymTab plugin" SEC_SYMTAB
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\SymTab.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\SymTab.dll
        WriteRegStr HKCU "${REGKEY}\Components" "SymTab plugin" 1
    SectionEnd

    Section "ThreadSearch plugin" SEC_THREADSEARCH
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\ThreadSearch.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\ThreadSearch.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\ThreadSearch.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\ThreadSearch-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "ThreadSearch plugin" 1
    SectionEnd

    Section "wxSmith plugin" SEC_WXSMITH
        SectionIn 1 2
        SetOutPath $INSTDIR
        SetOverwrite on
        File ${CB_BASE}\wxsmithlib.dll
        File ${CB_BASE}\wxchartctrl.dll
        File ${CB_BASE}\wxcustombutton.dll
        File ${CB_BASE}\wxflatnotebook.dll
        File ${CB_BASE}\wximagepanel.dll
        File ${CB_BASE}\wxkwic.dll
        File ${CB_BASE}\wxled.dll
        File ${CB_BASE}\wxmathplot.dll
        File ${CB_BASE}\wxspeedbutton.dll
        SetOutPath $INSTDIR${CB_SHARE_CB}
        File ${CB_BASE}${CB_SHARE_CB}\wxsmith.zip
        File ${CB_BASE}${CB_SHARE_CB}\wxSmithAui.zip
        File ${CB_BASE}${CB_SHARE_CB}\wxSmithContribItems.zip

        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\wxsmith.dll
        File ${CB_BASE}${CB_PLUGINS}\wxSmithAui.dll
        File ${CB_BASE}${CB_PLUGINS}\wxSmithContribItems.dll

        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\wxsmith.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\wxsmith-off.png
        SetOutPath $INSTDIR${CB_IMAGES}\wxsmith
        File ${CB_BASE}${CB_IMAGES}\wxsmith\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "wxSmith plugin" 1
    SectionEnd

    # C::B contrib plugins end

SectionGroupEnd

Section "C::B CBP2Make" SEC_CBP2MAKE
    SectionIn 1
    SetOutPath $INSTDIR
    SetOverwrite on
    File ${CB_BASE}\cbp2make.exe
    SetOutPath $SMPROGRAMS\$STARTMENU_FOLDER_INSTALL
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) CBP2Make.lnk" $INSTDIR\cbp2make.exe
    WriteRegStr HKCU "${REGKEY}\Components" "C::B CBP2Make" 1
SectionEnd

Section "C::B Share Config" SEC_SHARECONFIG
    SectionIn 1
    SetOutPath $INSTDIR
    SetOverwrite on
    File ${CB_BASE}\cb_share_config.exe
    SetOutPath $SMPROGRAMS\$STARTMENU_FOLDER_INSTALL
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) Share Config.lnk" $INSTDIR\cb_share_config.exe
    WriteRegStr HKCU "${REGKEY}\Components" "C::B Share Config" 1
SectionEnd

Section "C::B Launcher" SEC_LAUNCHER
    SectionIn 1
    SetOutPath $INSTDIR
    SetOverwrite on
    File ${CB_BASE}\CbLauncher.exe
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\$(^Name) (Launcher).lnk" $INSTDIR\CbLauncher.exe
    WriteRegStr HKCU "${REGKEY}\Components" "C::B Launcher" 1
SectionEnd


Section -post SEC_MISC
    WriteRegStr HKCU "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    SetOutPath $SMPROGRAMS\$STARTMENU_FOLDER_INSTALL
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER_INSTALL\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr   HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
    
    LogText "SEC_MISC"
    LogText "MultiUser.InstallMode : $MultiUser.InstallMode"
    LogText "SMPROGRAMS : $SMPROGRAMS"
    LogText "STARTMENU_FOLDER_UNINSTALL : $STARTMENU_FOLDER_UNINSTALL"

    ${If} $MultiUser.InstallMode == AllUsers
        WriteRegStr HKLM "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY}" "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME}" $MultiUser.InstallMode 
    ${Else}
        WriteRegStr HKCU "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY}" "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME}" $MultiUser.InstallMode 
    ${EndIf}
SectionEnd

# ========================================================================================================================
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# ========================================================================================================================

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKCU "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

########################
# Uninstaller sections #
########################

##############################################################################
# Note: Files in the uninstall section in REVERSE ORDER of the installation! #
##############################################################################

Section "-un.C::B Launcher" UNSEC_LAUNCHER
    Delete /REBOOTOK $INSTDIR\CbLauncher.exe
    Delete /REBOOTOK "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) (Launcher).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "C::B Launcher"
SectionEnd

Section "-un.C::B Share Config" UNSEC_SHARECONFIG
    Delete /REBOOTOK $INSTDIR\cb_share_config.exe
    Delete /REBOOTOK "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) Share Config.lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "C::B Share Config"
SectionEnd

Section "-un.C::B CBP2Make" UNSEC_CBP2MAKE
    Delete /REBOOTOK $INSTDIR\cbp2make.exe
    Delete /REBOOTOK "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) CBP2Make.lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "C::B CBP2Make"
SectionEnd

# C::B contrib plugins begin

Section "-un.Auto Versioning plugin" UNSEC_AUTOVERSIONING
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\AutoVersioning.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\AutoVersioning.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Auto Versioning plugin"
SectionEnd

Section "-un.Browse Tracker plugin" UNSEC_BROWSETRACKER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\BrowseTracker.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\BrowseTracker.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Browse Tracker plugin"
SectionEnd

Section "-un.Byo Games plugin" UNSEC_BYOGAMES
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\byogames.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\byogames.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Byo Games plugin"
SectionEnd

Section "-un.Cccc plugin" UNSEC_CCCC
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Cccc.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Cccc.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Cccc plugin"
SectionEnd

Section "-un.Code Snippets plugin" UNSEC_CODESNIPPETS
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\codesnippets\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\codesnippets
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codesnippets.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codesnippets.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Snippets plugin"
SectionEnd

Section "-un.Code Statistics plugin" UNSEC_CODESTAT
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codestats-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codestats.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codestat.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codestat.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Statistics plugin"
SectionEnd

Section "-un.Copy Strings plugin" UNSEC_COPYSTRINGS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\copystrings.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\copystrings.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Copy Strings plugin"
SectionEnd

Section "-un.CppCheck plugin" UNSEC_CPPCHECK
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\CppCheck.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\CppCheck.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "CppCheck plugin"
SectionEnd

Section "-un.Cscope plugin" UNSEC_CSCOPE
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Cscope.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Cscope.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Cscope plugin"
SectionEnd

Section "-un.DevPak plugin" UNSEC_DEVPAK
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\devpakupdater.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\devpakupdater.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "DevPak plugin"
SectionEnd

Section "-un.DoxyBlocks plugin" UNSEC_DOXYBLOCKS
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\DoxyBlocks\16x16\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\DoxyBlocks\16x16
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\DoxyBlocks\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\DoxyBlocks
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\DoxyBlocks-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\DoxyBlocks.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\DoxyBlocks.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\DoxyBlocks.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "DoxyBlocks plugin"
SectionEnd

Section "-un.Drag Scroll plugin" UNSEC_DRAGSCROLL
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\dragscroll-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\dragscroll.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\DragScroll.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\DragScroll.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Drag Scroll plugin"
SectionEnd

Section "-un.EditorConfig plugin" UNSEC_EDITORCONFIG
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\EditorConfig.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\EditorConfig.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "EditorConfig plugin"
SectionEnd

Section "-un.Editor tweaks plugin" UNSEC_EDITORTWEAKS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\EditorTweaks.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\EditorTweaks.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Editor tweaks plugin"
SectionEnd

Section "-un.EnvVars plugin" UNSEC_ENVVARS
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\envvars-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\envvars.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\envvars.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\envvars.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "EnvVars plugin"
SectionEnd

Section "-un.File Manager plugin" UNSEC_FILEMANAGER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\FileManager.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\FileManager.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "File Manager plugin"
SectionEnd

!ifdef FORTRAN_PLUGIN_FOUND
Section /o "-un.Fortran Project plugin" UNSEC_FORTRANPROJECT
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\*.dem
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\*.f90
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\*.py
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\16x16\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\16x16
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\20x20\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\20x20
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\24x24\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\24x24
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\28x28\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\28x28
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\32x32\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\32x32
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\40x40\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\40x40
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\48x48\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\48x48
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\56x56\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\56x56
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\64x64\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject\64x64
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\fortranproject
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\FortranProject.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\FortranProject.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Fortran Project plugin"
SectionEnd
!endif
Section /o "-un.HeaderFixUp plugin" UNSEC_HEADERFIXUP
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\headerfixup.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\headerfixup.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "HeaderFixUp plugin"
SectionEnd

Section "-un.Help plugin" UNSEC_HELP
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\help-plugin-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\help-plugin.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\help_plugin.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\help_plugin.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Help plugin"
SectionEnd

Section "-un.HexEditor plugin" UNSEC_HEXEDITOR
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\HexEditor.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\HexEditor.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "HexEditor plugin"
SectionEnd

Section "-un.IncrementalSearch plugin" UNSEC_INCREMENTALSEARCH
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\IncrementalSearch.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\IncrementalSearch.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "IncrementalSearch plugin"
SectionEnd

Section "-un.Key Binder plugin" UNSEC_KEYBINDER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\onekeytobindthem-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\onekeytobindthem.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\keybinder.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\keybinder.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Key Binder plugin"
SectionEnd

Section "-un.Koders plugin" UNSEC_KODERS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\cb_koders.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\cb_koders.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Koders plugin"
SectionEnd

Section "-un.Lib Finder plugin" UNSEC_LIBFINDER
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder\*.xml
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\lib_finder.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Lib Finder plugin"
SectionEnd

Section "-un.MouseSap plugin" UNSEC_MOUSESAP
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\MouseSap.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\MouseSap.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "MouseSap plugin"
SectionEnd

Section "-un.Nassi Shneiderman plugin" UNSEC_NASSI
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\NassiShneiderman.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\NassiShneiderman.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Nassi Shneiderman plugin"
SectionEnd

Section "-un.Occurrences Highlighting plugin" UNSEC_OCC_HIGHLIGHTING
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\OccurrencesHighlighting.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\OccurrencesHighlighting.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Occurrences Highlighting plugin"
SectionEnd

Section "-un.Tools+ plugin" UNSEC_TOOLSPLUS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\ToolsPlus.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\ToolsPlus.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Tools+ plugin"
SectionEnd

Section "-un.Profiler plugin" UNSEC_PROFILER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\profiler-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\profiler.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Profiler.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Profiler.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Profiler plugin"
SectionEnd

Section "-un.Project Options Manipulator plugin" UNSEC_PRJOPTSMANIP
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\ProjectOptionsManipulator.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\ProjectOptionsManipulator.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Project Options Manipulator plugin"
SectionEnd

Section "-un.RegEx Testbed plugin" UNSEC_REGEXTESTBED
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\RegExTestbed.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\RegExTestbed.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "RegEx Testbed plugin"
SectionEnd

Section "-un.Reopen Editor plugin" UNSEC_REOPEN
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\ReopenEditor.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\ReopenEditor.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Reopen Editor plugin"
SectionEnd

Section "-un.Exporter plugin" UNSEC_EXPORTER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Exporter.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Exporter.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Exporter plugin"
SectionEnd

Section "-un.SymTab plugin" UNSEC_SYMTAB
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SymTab.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SymTab.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "SymTab plugin"
SectionEnd

Section "-un.SpellChecker plugin" UNSEC_SPELLCHECKER
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\*.aff
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\*.dic
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\*.png
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\OnlineSpellChecking.xml
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\16x16\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\16x16
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\20x20\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\20x20
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\24x24\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\24x24
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\28x28\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\28x28
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\32x32\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\32x32
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\40x40\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\40x40
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\48x48\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\48x48
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\56x56\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\56x56
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\64x64\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker\64x64
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\SpellChecker-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\SpellChecker.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SpellChecker.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SpellChecker.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "SpellChecker plugin"
SectionEnd

Section "-un.ThreadSearch plugin" UNSEC_THREADSEARCH
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\ThreadSearch-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\ThreadSearch.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\ThreadSearch.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\ThreadSearch.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "ThreadSearch plugin"
SectionEnd

Section "-un.wxSmith plugin" UNSEC_WXSMITH
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\wxsmith\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\wxsmith
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\wxsmith-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\wxsmith.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\wxsmith.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\wxSmithAui.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\wxSmithContribItems.dll
    Delete /REBOOTOK $INSTDIR\wxspeedbutton.dll
    Delete /REBOOTOK $INSTDIR\wxmathplot.dll
    Delete /REBOOTOK $INSTDIR\wxled.dll
    Delete /REBOOTOK $INSTDIR\wxkwic.dll
    Delete /REBOOTOK $INSTDIR\wximagepanel.dll
    Delete /REBOOTOK $INSTDIR\wxflatnotebook.dll
    Delete /REBOOTOK $INSTDIR\wxdatetimepicker.dll
    Delete /REBOOTOK $INSTDIR\wxcustombutton.dll
    Delete /REBOOTOK $INSTDIR\wxchartctrl.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\wxsmith.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\wxSmithAui.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\wxSmithContribItems.zip
    Delete /REBOOTOK $INSTDIR\wxsmithlib.dll
    DeleteRegValue HKCU "${REGKEY}\Components" "wxSmith plugin"
SectionEnd

# C::B contrib plugins end

# C::B core plugins begin

Section "-un.Abbreviations plugin" UNSEC_ABBREV
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\abbrev-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\abbrev.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\abbreviations.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\abbreviations.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Abbreviations plugin"
SectionEnd

Section "-un.AStyle plugin" UNSEC_ASTYLE
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\astyle-plugin-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\astyle-plugin.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\astyle.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\astyle.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "AStyle plugin"
SectionEnd

Section "-un.Autosave plugin" UNSEC_AUTOSAVE
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\autosave-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\autosave.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\autosave.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\autosave.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Autosave plugin"
SectionEnd

Section "-un.Class Wizard plugin" UNSEC_CLASSWIZARD
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\classwizard.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\classwizard.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Class Wizard plugin"
SectionEnd

Section "-un.Code Completion plugin" UNSEC_CODECOMPLETION
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codecompletion-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codecompletion.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codecompletion.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codecompletion.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Completion plugin"
SectionEnd

Section "-un.Compiler plugin" UNSEC_COMPILER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\compiler-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\compiler.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\stop.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\rebuild.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\compilerun.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\compile.png
    Delete /REBOOTOK $INSTDIR${CB_XML_COMPILERS}\*.xml
    RMDir /r /REBOOTOK $INSTDIR${CB_XML_COMPILERS}
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\compiler.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\compiler.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Compiler plugin"
SectionEnd

Section "-un.Debugger plugin" UNSEC_DEBUGGER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\debugger-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\debugger.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\debugger.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\debugger.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Debugger plugin"
SectionEnd

Section "-un.MIME Handler plugin" UNSEC_MIMEHANDLER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\extensions-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\extensions.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\defaultmimehandler.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\defaultmimehandler.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "MIME Handler plugin"
SectionEnd

Section "-un.Open Files List plugin" UNSEC_OPENFILESLIST
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\openfileslist.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\openfileslist.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Open Files List plugin"
SectionEnd

Section "-un.Projects Importer plugin" UNSEC_PROJECTSIMPORTER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\projectsimporter.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\projectsimporter.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Projects Importer plugin"
SectionEnd

Section "-un.RND Generator plugin" UNSEC_RNDGEN
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\rndgen.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\rndgen.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "RND Generator plugin"
SectionEnd

Section "-un.Scripted Wizard plugin" UNSEC_SCRIPTEDWIZARD
    Delete /REBOOTOK $INSTDIR${CB_WIZARD}\*.*
    RMDir  /r /REBOOTOK $INSTDIR${CB_WIZARD}
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\scriptedwizard.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\scriptedwizard.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Scripted Wizard plugin"
SectionEnd

Section "-un.SmartIndent plugin" UNSEC_SMARTINDENT
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentCpp.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentFortran.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentHDL.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentLua.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentPascal.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentPython.dll
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SmartIndentXML.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentCpp.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentFortran.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentHDL.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentLua.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentPascal.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentPython.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SmartIndentXML.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "SmartIndent plugin"
SectionEnd

Section "-un.ToDo List plugin" UNSEC_TODOLIST
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\todo-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\todo.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\todo.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\todo.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "ToDo List plugin"
SectionEnd

Section "-un.XP Look And Feel plugin" UNSEC_XPLOOKANDFEEL
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\xpmanifest.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\xpmanifest.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "XP Look And Feel plugin"
SectionEnd

# C::B core plugins end

# C::B lexers begin

# "Compiler Languages"

Section "-un.C/C++" UNSEC_CPP
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_rc.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_rc.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cpp.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cpp.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "C/C++"
SectionEnd

Section "-un.Ada" UNSEC_ADA
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ada.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ada.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Ada"
SectionEnd

Section "-un.The D Language" UNSEC_D
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_d.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_d.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "The D Language"
SectionEnd

Section "-un.Fortran" UNSEC_F
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_f77.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_f77.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_fortran.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_fortran.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Fortran"
SectionEnd

Section "-un.Java" UNSEC_JAVA
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_java.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_java.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Java"
SectionEnd

Section "-un.JavaScript" UNSEC_JAVA_SCRIPT
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_javascript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_javascript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "JavaScript"
SectionEnd

Section "-un.Objective-C" UNSEC_OBJECTIVE_C
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_objc.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_objc.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Objective-C"
SectionEnd

Section "-un.Pascal" UNSEC_PASCAL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_pascal.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_pascal.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Pascal"
SectionEnd

Section "-un.Smalltalk" UNSEC_SMALLTALK
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_smalltalk.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_smalltalk.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Smalltalk"
SectionEnd

# "Script Languages"

Section "-un.Angelscript" UNSEC_AS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_angelscript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_angelscript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Angelscript"
SectionEnd

Section "-un.AutoTools" UNSEC_AT
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_autotools.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_autotools.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "AutoTools"
SectionEnd

Section "-un.Caml" UNSEC_CAML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_caml.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_caml.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Caml"
SectionEnd

Section "-un.Coffee" UNSEC_COFFEE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_coffee.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_coffee.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Coffee"
SectionEnd

Section "-un.Game Monkey" UNSEC_GM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_gm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_gm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Game Monkey"
SectionEnd

Section "-un.Haskell" UNSEC_HASKELL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_haskell.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_haskell.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Haskell"
SectionEnd

Section "-un.Lisp" UNSEC_LISP
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lisp.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lisp.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Lisp"
SectionEnd

Section "-un.Lua" UNSEC_LUA
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lua.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lua.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Lua"
SectionEnd

Section "-un.Nim" UNSEC_NIM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nim.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nim.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Nim"
SectionEnd

Section "-un.Perl" UNSEC_PERL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_perl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_perl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Perl"
SectionEnd

Section "-un.Postscript" UNSEC_POSTSCRIPT
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_postscript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_postscript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Postscript"
SectionEnd

Section "-un.Python" UNSEC_PYTHON
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_python.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_python.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Python"
SectionEnd

Section "-un.Ruby" UNSEC_RUBY
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ruby.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ruby.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Ruby"
SectionEnd

Section "-un.Squirrel" UNSEC_SQ
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_squirrel.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_squirrel.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Squirrel"
SectionEnd

Section "-un.VB Script" UNSEC_VB
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vbscript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vbscript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "VB Script"
SectionEnd

# "Shell / Binutils"

Section "-un.bash script" UNSEC_BASH
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bash.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bash.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "bash script"
SectionEnd

Section "-un.DOS batch files" UNSEC_DOS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_batch.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_batch.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "DOS batch files"
SectionEnd

Section "-un.Windows registry file" UNSEC_REGISTRY
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_registry.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_registry.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Windows registry file"
SectionEnd

Section "-un.Cmake" UNSEC_CMAKE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cmake.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cmake.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Cmake"
SectionEnd

Section "-un.diff" UNSEC_DIFF
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_diff.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_diff.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "diff"
SectionEnd

Section "-un.Makefile" UNSEC_MAKE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_make.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_make.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Makefile"
SectionEnd

Section "-un.PowerShell" UNSEC_PS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_powershell.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_powershell.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "PowerShell"
SectionEnd

# "Markup Languages"

Section "-un.BiBTeX" UNSEC_BIBTEX
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bibtex.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bibtex.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "BiBTeX"
SectionEnd

Section "-un.CSS" UNSEC_CSS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_css.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_css.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "CSS"
SectionEnd

Section "-un.HTML" UNSEC_HTML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_html.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_html.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "HTML"
SectionEnd

Section "-un.LaTeX" UNSEC_LATEX
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_latex.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_latex.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "LaTeX"
SectionEnd

Section "-un.Markdown" UNSEC_MARKDOWN
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_markdown.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_markdown.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Markdown"
SectionEnd

Section "-un.XML" UNSEC_XML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_xml.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_xml.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "XML"
SectionEnd

Section "-un.YAML" UNSEC_YAML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_yaml.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_yaml.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "YAML"
SectionEnd

# "Graphics Programming"

Section "-un.CUDA" UNSEC_CUDA
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cu.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cu.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "CUDA"
SectionEnd

Section "-un.GLSL (GLslang)" UNSEC_GLSL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_glsl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_glsl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "GLSL (GLslang)"
SectionEnd

Section "-un.nVidia Cg" UNSEC_CG
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cg.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cg.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "nVidia Cg"
SectionEnd

Section "-un.Ogre" UNSEC_OGRE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreMaterial.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreMaterial.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreCompositor.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreCompositor.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Ogre"
SectionEnd

# "Embedded development"

Section "-un.A68k Assembler" UNSEC_A68K
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_A68k.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_A68k.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "A68k Assembler"
SectionEnd

Section "-un.Hitachi Assembler" UNSEC_HITASM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_hitasm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_hitasm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Hitachi Assembler"
SectionEnd

Section "-un.Intel HEX" UNSEC_IHEX
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ihex.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_ihex.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Intel HEX"
SectionEnd

Section "-un.Motorola S-Record" UNSEC_SREC
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_srec.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_srec.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Motorola S-Record"
SectionEnd

Section "-un.Tektronix extended HEX" UNSEC_TEHEX
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_tehex.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_tehex.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Tektronix extended HEX"
SectionEnd

Section "-un.Verilog" UNSEC_VERILOG
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_verilog.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_verilog.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Verilog"
SectionEnd

Section "-un.VHDL" UNSEC_VHDL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vhdl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vhdl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "VHDL"
SectionEnd

# "Others"

Section "-un.Google Protocol Buffer" UNSEC_PROTO
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_proto.xml
    DeleteRegValue HKCU "${REGKEY}\Components" "Google Protocol Buffer"
SectionEnd

Section "-un.InnoSetup" UNSEC_INNO
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_inno.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_inno.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "InnoSetup"
SectionEnd

Section "-un.MASM" UNSEC_MASM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_masm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_masm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "MASM"
SectionEnd

Section "-un.Matlab" UNSEC_MATLAB
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_matlab.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_matlab.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Matlab"
SectionEnd

Section "-un.NSIS installer script" UNSEC_NSIS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nsis.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nsis.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "NSIS installer script"
SectionEnd

Section "-un.Plain file" UNSEC_PLAIN
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_plain.xml
    DeleteRegValue HKCU "${REGKEY}\Components" "Plain file"
SectionEnd

Section "-un.Property file" UNSEC_PROPERTY
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_properties.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_properties.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Property file"
SectionEnd

Section "-un.Sql" UNSEC_SQL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_sql.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_sql.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Sql"
SectionEnd

Section "-un.XBase" UNSEC_XBASE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_prg.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_prg.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "XBase"
SectionEnd

# C::B lexers end

# C::B shortcuts begin

Section "-un.Program Shortcut" UNSEC_PROGRAMSHORTCUT
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name).lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) Share Config.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) Code Completion Test.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) Address to Line GUI.lnk"

    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) PDF Manual English.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) CHM Manual English.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) PDF Manual French.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) CHM Manual French.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\PBs wxWidgets Guide.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) License.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\$(^Name) SDK License.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links\$(^Name) Web Site.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links\$(^Name) Forums.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links\$(^Name) WiKi.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links\$(^Name) Tickets.lnk"
    Delete /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links\$(^Name) beginner instructions.lnk"
    RMDir  /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Links"
    RMDir  /REBOOTOK  "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL"
    DeleteRegValue HKCU "${REGKEY}\Components" "Program Shortcut"
SectionEnd

Section "-un.Desktop Shortcut" UNSEC_DESKTOPSHORTCUT
    Delete /REBOOTOK "$DESKTOP\$(^Name).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Desktop Shortcut"
SectionEnd

Section "-un.Quick Launch Shortcut" UNSEC_QUICKLAUNCHSHORTCUT
    Delete /REBOOTOK "$QUICKLAUNCH\$(^Name).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Quick Launch Shortcut"
SectionEnd

# C::B shortcuts end

# C::B core begin

Section "-un.Core Files (required)" UNSEC_CORE
    # Just use RMDir following, if it fails that's ok
    # cause the post section will handle this.
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}
    Delete /REBOOTOK $INSTDIR${CB_TEMPLATES}\*.*
    RMDir  /REBOOTOK $INSTDIR${CB_TEMPLATES}
    Delete /REBOOTOK $INSTDIR${CB_SCTESTS}\*.script
    RMDir  /REBOOTOK $INSTDIR${CB_SCTESTS}
    Delete /REBOOTOK $INSTDIR${CB_SCRIPTS}\*.script
    RMDir  /REBOOTOK INSTDIR${CB_SCRIPTS}
    # Documentation
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\index.ini
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\manual_codeblocks_en.chm
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\manual_codeblocks_en.pdf
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\manual_codeblocks_fr.chm
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\manual_codeblocks_fr.pdf
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\Manual_wxPBGuide.pdf
    RMDir  /REBOOTOK $INSTDIR${CB_DOCS}
    # Shared Directory
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\resources.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\manager_resources.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\tips.txt
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\start_here.zip
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE}
    # WGET
    Delete /REBOOTOK $INSTDIR\wget.exe
    # Licens files
    Delete /REBOOTOK $INSTDIR\gpl-3.0.txt
    Delete /REBOOTOK $INSTDIR\lgpl-3.0.txt
    # crash handler
    ${If} ${IsWinXP}
        # crash handler for Windows XP!!!!
        Delete /REBOOTOK $INSTDIR\symsrv.yes
        Delete /REBOOTOK $INSTDIR\symsrv.dll
        Delete /REBOOTOK $INSTDIR\mgwhelp.dll
        Delete /REBOOTOK $INSTDIR\exchndl.dll
        Delete /REBOOTOK $INSTDIR\dbghelp.dll
        ; Delete /REBOOTOK $INSTDIR\dbgcore.dll - N/A for XP
    ${Else}
        # crash handler for Win 7+  (fails on XP!!!!)
        Delete /REBOOTOK $INSTDIR\symsrv.yes
        Delete /REBOOTOK $INSTDIR\symsrv.dll
        Delete /REBOOTOK $INSTDIR\mgwhelp.dll
        Delete /REBOOTOK $INSTDIR\exchndl.dll
        Delete /REBOOTOK $INSTDIR\dbghelp.dll
        Delete /REBOOTOK $INSTDIR\dbgcore.dll
    ${EndIf}
    # MinGW DLL's for thread handling etc.
    Delete /REBOOTOK $INSTDIR\libwinpthread-1.dll
    Delete /REBOOTOK $INSTDIR\libstdc++-6.dll
!if ${BUILD_TYPE} == 64
    Delete /REBOOTOK $INSTDIR\libgcc_s_seh-1.dll
!else
    Delete /REBOOTOK $INSTDIR\libgcc_s_dw2-1.dll
!endif
    # CodeBlock Files
    Delete /REBOOTOK $INSTDIR\codeblocks.exe
    Delete /REBOOTOK $INSTDIR\codeblocks.dll
    #Delete /REBOOTOK $INSTDIR\cctest.exe # CB developer testing only
    Delete /REBOOTOK $INSTDIR\CbLauncher.exe
    Delete /REBOOTOK $INSTDIR\cb_console_runner.exe
    Delete /REBOOTOK $INSTDIR\Addr2LineUI.exe
    Delete /REBOOTOK $INSTDIR\wxmsw*u_gl_gcc_cb.dll
    Delete /REBOOTOK $INSTDIR\wxmsw*u_gcc_cb.dll
    DeleteRegValue HKCU "${REGKEY}\Components" "Core Files (required)"
    
SectionEnd

# C::B core end

Section -un.post UNSEC_MISC
    Delete "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL\Uninstall $(^Name).lnk"
    RMDir /REBOOTOK "$SMPROGRAMS\$STARTMENU_FOLDER_UNINSTALL"

    ${If} $MultiUser.InstallMode == AllUsers
        DeleteRegKey HKLM "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY}\{MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME}"
    ${Else}
        DeleteRegKey HKCU "${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY}\${MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME}"
    ${EndIf}

    DeleteRegValue HKCU "${REGKEY}" Path
    DeleteRegKey HKCU "${REGKEY}\Components"
    DeleteRegKey HKCU "${REGKEY}"


    LogText "un.onUninstSuccess"
    LogText "MultiUser.InstallMode : $MultiUser.InstallMode"
    LogText "SMPROGRAMS : $SMPROGRAMS"
    LogText "STARTMENU_FOLDER_UNINSTALL : $STARTMENU_FOLDER_UNINSTALL"


    # ===================================================================================================

    # Start Unregister CodeBlocks associated files - see FileAssocation.cpp
    # or check out the registry "Computer\HKEY_CURRENT_USER\SOFTWARE\Classes\CodeBlocks.*" entries.
    
    ${If} ${RunningX64}
        ; ${LogText} "SetRegView 64"
        SetRegView 64
        StrCpy $0 0 ; Registry key index
        ; Length of "CodeBlocks" = 10
        enumunkey64:
            EnumRegKey $1 HKCU64 "SOFTWARE\Classes" $0
            ;${LogText} "Read HKCU\SOFTWARE\Classes\$1"
            IntOp $0 $0 + 1
            StrCmp $1 "" done64
            StrCpy $2 $1 10  0
            ;${LogText} "Read32 0 = $0 , 1 = $1 , 2 = $2"
            StrCmp $2 "CodeBlocks" 0 enumunkey64
            StrLen $3 $1
            IntOp $3 $3 - 10    ; Includes .
            StrCpy $4 $1 $3 10  ; Includes .
            StrCmp $4 "" enumunkey64
            ReadRegStr $5 HKCU64 "SOFTWARE\Classes\$4" ""
            ;${LogText} "L2761 REG64 1 = $1 , 2 = $2 , 3 = $3 , 4 = $4 , 5 = $5"
            StrCmp $5 $1 0 DelCodeBlocksEntry64   ; If file extension no for Codeblocsk then goto DelCodeBlocksEntry
            ;${LogText} "DeleteRegValue HKCU64 'SOFTWARE\Classes\$4' ''"
            DeleteRegValue HKCU64 "SOFTWARE\Classes\$4" ""    ; Delete default as it is codeblocks
            ;${LogText} "DeleteRegKey /IfEmpty HKCU64 'SOFTWARE\Classes\$4'"
            DeleteRegKey /IfEmpty HKCU64 "SOFTWARE\Classes\$4"
        DelCodeBlocksEntry64:
            DeleteRegKey HKCU64 "SOFTWARE\Classes\$1"
            Goto enumunkey64

        done64:    
        # Finish Unregister CodeBlocks associated files - see FileAssocation.cpp
    ${EndIf}
        
    ;${LogText} "SetRegView 32"
    SetRegView 32
    StrCpy $0 0 ; Registry key index
    ; Length of "CodeBlocks" = 10
    enumunkey:
        EnumRegKey $1 HKCU32 "SOFTWARE\Classes" $0
        ;${LogText} "Read HKCU32\SOFTWARE\Classes\$1"
        IntOp $0 $0 + 1
        StrCmp $1 "" done
        StrCpy $2 $1 10  0
        ;${LogText} "Read32 0 = $0 , 1 = $1 , 2 = $2"
        StrCmp $2 "CodeBlocks" 0 enumunkey
        StrLen $3 $1
        IntOp $3 $3 - 10    ; Includes .
        StrCpy $4 $1 $3 10  ; Includes .
        StrCmp $4 "" enumunkey
        ReadRegStr $5 HKCU32 "SOFTWARE\Classes\$4" ""
        ;${LogText} "L2792 REG32   1 = $1 , 2 = $2 , 3 = $3 , 4 = $4 , 5 = $5"
        StrCmp $5 $1 0 DelCodeBlocksEntry   ; If file extension no for Codeblocsk then goto DelCodeBlocksEntry
        ;${LogText} "DeleteRegValue HKCU32 'SOFTWARE\Classes\$4' ''"
        DeleteRegValue HKCU32 "SOFTWARE\Classes\$4" ""    ; Delete default as it is codeblocks
        ;${LogText} "DeleteRegKey HKCU32 'SOFTWARE\Classes\$4'"
        DeleteRegKey HKCU32 "SOFTWARE\Classes\$4"
    DelCodeBlocksEntry:
        DeleteRegKey HKCU32 "SOFTWARE\Classes\$1"
        Goto enumunkey

    done:    
    # Finish Unregister CodeBlocks associated files - see FileAssocation.cpp
    
    ; Delete Uninstall Entries and uninstall.exe
    DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK $INSTDIR\uninstall.exe

    # ===================================================================================================
    RMDir /r /REBOOTOK $INSTDIR
    # If $INSTDIR was already removed, skip these next steps
    IfFileExists $INSTDIR 0 instDirAllreadyRemoved
        # It appears $INSTDIR is not empty, thus it could not be removed.
        # Ask the user to probably force the removal.
        MessageBox MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION \
            "Remove all files in your Code::Blocks directory?$\r$\n(If you have anything you created that you want to keep, click No.)" \
            /SD IDNO IDYES yesRMINSTDIR IDNO noRMDINSTDIR
yesRMINSTDIR:
        # Try to delete all remaining files and finally $INSTDIR recursively
        RMDir /r /REBOOTOK $INSTDIR
        IfFileExists $INSTDIR 0 instDirAllreadyRemoved
            MessageBox MB_OK|MB_ICONEXCLAMATION \
                "Warning: $INSTDIR could not be removed.$\r$\n(Probably missing access rights?)" \
                /SD IDOK
noRMDINSTDIR:
        # If user selected "no" -> skip the next steps
instDirAllreadyRemoved:
    
    # ===================================================================================================
    # Check for User data and ask if the user wants to delete the data
    IfFileExists "$APPDATA\codeblocks" 0 appDataDirAllreadyRemoved
        MessageBox MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION  "Do you want to delete the existing configuration files and directory? ($APPDATA\codeblocks)" \
        /SD IDNO IDYES yesRMAPPDATADIR IDNO noRMAPPDATADIR
yesRMAPPDATADIR:
        # Try to delete all APPDATA\codeblocks files and directory.
        RMDir /r "$APPDATA\codeblocks"
        IfFileExists "$APPDATA\codeblocks" 0 appDataDirAllreadyRemoved
            MessageBox MB_OK|MB_ICONEXCLAMATION \
                "Warning: $APPDATA\codeblocks could not be removed.$\r$\n(Probably missing access rights?)" \
                /SD IDOK
noRMAPPDATADIR:
        # If user selected "no" -> skip the next steps
    
appDataDirAllreadyRemoved:
    # ===================================================================================================
    

SectionEnd

#######################
# Installer functions #
#######################

Function .onInit
    LogSet on
    LogText "HERE"
    ;${LogSet} on
    InitPluginsDir
    Push $R1
    File /oname=$PLUGINSDIR\spltmp.bmp ${CB_SPLASH}
    advsplash::show 1000 600 400 -1 $PLUGINSDIR\spltmp
    Pop $R1
    Pop $R1

    !insertmacro MULTIUSER_INIT    
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "NSIS_CompilerDownload.ini"

FunctionEnd


#########################
# Uninstaller functions #
#########################

Function un.onInit
    LogSet on
    
    ; Load multiuser data. NOTE:  keep the .onInstSuccess WriteRegStr...
    !insertmacro MULTIUSER_UNINIT

    ; Delete start menu entries
    !insertmacro MUI_STARTMENU_GETFOLDER Application $STARTMENU_FOLDER_UNINSTALL

    LogText "un.onInit after "
    LogText "MultiUser.InstallMode : $MultiUser.InstallMode"
    LogText "SMPROGRAMS : $SMPROGRAMS"
    LogText "STARTMENU_FOLDER_UNINSTALL : $STARTMENU_FOLDER_UNINSTALL"
    LogText "INSTDIR : $INSTDIR"
    
    ReadRegStr $INSTDIR HKCU "${REGKEY}" Path
    !insertmacro SELECT_UNSECTION "Core Files (required)"              ${UNSEC_CORE}

    !insertmacro SELECT_UNSECTION "Program Shortcut"                   ${UNSEC_PROGRAMSHORTCUT}
    !insertmacro SELECT_UNSECTION "Desktop Shortcut"                   ${UNSEC_DESKTOPSHORTCUT}
    !insertmacro SELECT_UNSECTION "Quick Launch Shortcut"              ${UNSEC_QUICKLAUNCHSHORTCUT}

    # "Compiler Languages"
    !insertmacro SELECT_UNSECTION "C/C++"                              ${UNSEC_CPP}
    !insertmacro SELECT_UNSECTION "Ada"                                ${UNSEC_ADA}
    !insertmacro SELECT_UNSECTION "The D Language"                     ${UNSEC_D}
    !insertmacro SELECT_UNSECTION "Fortran"                            ${UNSEC_F}
    !insertmacro SELECT_UNSECTION "Java"                               ${UNSEC_JAVA}
    !insertmacro SELECT_UNSECTION "JavaScript"                         ${UNSEC_JAVA_SCRIPT}
    !insertmacro SELECT_UNSECTION "Objective-C"                        ${UNSEC_OBJECTIVE_C}
    !insertmacro SELECT_UNSECTION "Pascal"                             ${UNSEC_PASCAL}
    !insertmacro SELECT_UNSECTION "Smalltalk"                          ${UNSEC_SMALLTALK}
    # "Script Languages"
    !insertmacro SELECT_UNSECTION "Angelscript"                        ${UNSEC_AS}
    !insertmacro SELECT_UNSECTION "AutoTools"                          ${UNSEC_AT}
    !insertmacro SELECT_UNSECTION "Caml"                               ${UNSEC_CAML}
    !insertmacro SELECT_UNSECTION "Coffee"                             ${UNSEC_COFFEE}
    !insertmacro SELECT_UNSECTION "Game Monkey"                        ${UNSEC_GM}
    !insertmacro SELECT_UNSECTION "Haskell"                            ${UNSEC_HASKELL}
    !insertmacro SELECT_UNSECTION "Lisp"                               ${UNSEC_LISP}
    !insertmacro SELECT_UNSECTION "Lua"                                ${UNSEC_LUA}
    !insertmacro SELECT_UNSECTION "Nim"                                ${UNSEC_NIM}
    !insertmacro SELECT_UNSECTION "Perl"                               ${UNSEC_PERL}
    !insertmacro SELECT_UNSECTION "Postscript"                         ${UNSEC_POSTSCRIPT}
    !insertmacro SELECT_UNSECTION "Python"                             ${UNSEC_PYTHON}
    !insertmacro SELECT_UNSECTION "Ruby"                               ${UNSEC_RUBY}
    !insertmacro SELECT_UNSECTION "Squirrel"                           ${UNSEC_SQ}
    !insertmacro SELECT_UNSECTION "VB Script"                          ${UNSEC_VB}
    # "Markup Languages"
    !insertmacro SELECT_UNSECTION "BiBTeX"                             ${UNSEC_BIBTEX}
    !insertmacro SELECT_UNSECTION "CSS"                                ${UNSEC_CSS}
    !insertmacro SELECT_UNSECTION "HTML"                               ${UNSEC_HTML}
    !insertmacro SELECT_UNSECTION "LaTeX"                              ${UNSEC_LATEX}
    !insertmacro SELECT_UNSECTION "Markdown"                           ${UNSEC_MARKDOWN}
    !insertmacro SELECT_UNSECTION "XML"                                ${UNSEC_XML}
    !insertmacro SELECT_UNSECTION "YAML"                               ${UNSEC_YAML}
    # "Graphics Programming"
    !insertmacro SELECT_UNSECTION "CUDA"                               ${UNSEC_CUDA}
    !insertmacro SELECT_UNSECTION "GLSL (GLSlang)"                     ${UNSEC_GLSL}
    !insertmacro SELECT_UNSECTION "nVidia Cg"                          ${UNSEC_CG}
    !insertmacro SELECT_UNSECTION "Ogre"                               ${UNSEC_OGRE}
    # "Embedded development"
    !insertmacro SELECT_UNSECTION "A68k Assembler"                     ${UNSEC_A68K}
    !insertmacro SELECT_UNSECTION "Hitachi Assembler"                  ${UNSEC_HITASM}
    !insertmacro SELECT_UNSECTION "Intel HEX"                          ${UNSEC_IHEX}
    !insertmacro SELECT_UNSECTION "Motorola S-Record"                  ${UNSEC_SREC}
    !insertmacro SELECT_UNSECTION "Tektronix extended HEX"             ${UNSEC_TEHEX}
    !insertmacro SELECT_UNSECTION "Verilog"                            ${UNSEC_VERILOG}
    !insertmacro SELECT_UNSECTION "VHDL"                               ${UNSEC_VHDL}
    # "Shell / Binutils"
    !insertmacro SELECT_UNSECTION "bash script"                        ${UNSEC_BASH}
    !insertmacro SELECT_UNSECTION "DOS batch files"                    ${UNSEC_DOS}
    !insertmacro SELECT_UNSECTION "Windows registry file"              ${UNSEC_REGISTRY}
    !insertmacro SELECT_UNSECTION "Cmake"                              ${UNSEC_CMAKE}
    !insertmacro SELECT_UNSECTION "diff"                               ${UNSEC_DIFF}
    !insertmacro SELECT_UNSECTION "Makefile"                           ${UNSEC_MAKE}
    !insertmacro SELECT_UNSECTION "PowerShell"                         ${UNSEC_PS}
    # "Others"
    !insertmacro SELECT_UNSECTION "Google Protocol Buffer"             ${UNSEC_PROTO}
    !insertmacro SELECT_UNSECTION "InnoSetup"                          ${UNSEC_INNO}
    !insertmacro SELECT_UNSECTION "MASM"                               ${UNSEC_MASM}
    !insertmacro SELECT_UNSECTION "MATLAB"                             ${UNSEC_MATLAB}
    !insertmacro SELECT_UNSECTION "NSIS installer script"              ${UNSEC_NSIS}
    !insertmacro SELECT_UNSECTION "Plain file"                         ${UNSEC_PLAIN}
    !insertmacro SELECT_UNSECTION "Property file"                      ${UNSEC_PROPERTY}
    !insertmacro SELECT_UNSECTION "Sql"                                ${UNSEC_SQL}
    !insertmacro SELECT_UNSECTION "XBase"                              ${UNSEC_XBASE}

    !insertmacro SELECT_UNSECTION "Abbreviations plugin"               ${UNSEC_ABBREV}
    !insertmacro SELECT_UNSECTION "AStyle plugin"                      ${UNSEC_ASTYLE}
    !insertmacro SELECT_UNSECTION "Autosave plugin"                    ${UNSEC_AUTOSAVE}
    !insertmacro SELECT_UNSECTION "Class Wizard plugin"                ${UNSEC_CLASSWIZARD}
    !insertmacro SELECT_UNSECTION "Code Completion plugin"             ${UNSEC_CODECOMPLETION}
    !insertmacro SELECT_UNSECTION "Compiler plugin"                    ${UNSEC_COMPILER}
    !insertmacro SELECT_UNSECTION "Debugger plugin"                    ${UNSEC_DEBUGGER}
    !insertmacro SELECT_UNSECTION "MIME Handler plugin"                ${UNSEC_MIMEHANDLER}
    !insertmacro SELECT_UNSECTION "Open Files List plugin"             ${UNSEC_OPENFILESLIST}
    !insertmacro SELECT_UNSECTION "Projects Importer plugin"           ${UNSEC_PROJECTSIMPORTER}
    !insertmacro SELECT_UNSECTION "SmartIndent plugin"                 ${UNSEC_SMARTINDENT}
    !insertmacro SELECT_UNSECTION "Scripted Wizard plugin"             ${UNSEC_SCRIPTEDWIZARD}
    !insertmacro SELECT_UNSECTION "RND Generator plugin"               ${UNSEC_RNDGEN}
    !insertmacro SELECT_UNSECTION "ToDo List plugin"                   ${UNSEC_TODOLIST}
    !insertmacro SELECT_UNSECTION "XP Look And Feel plugin"            ${UNSEC_XPLOOKANDFEEL}

    !insertmacro SELECT_UNSECTION "Auto Versioning plugin"             ${UNSEC_AUTOVERSIONING}
    !insertmacro SELECT_UNSECTION "Browse Tracker plugin"              ${UNSEC_BROWSETRACKER}
    !insertmacro SELECT_UNSECTION "Byo Games plugin"                   ${UNSEC_BYOGAMES}
    !insertmacro SELECT_UNSECTION "Cccc plugin"                        ${UNSEC_CCCC}
    !insertmacro SELECT_UNSECTION "Code Snippets plugin"               ${UNSEC_CODESNIPPETS}
    !insertmacro SELECT_UNSECTION "Code Statistics plugin"             ${UNSEC_CODESTAT}
    !insertmacro SELECT_UNSECTION "Copy Strings plugin"                ${UNSEC_COPYSTRINGS}
    !insertmacro SELECT_UNSECTION "CppCheck plugin"                    ${UNSEC_CPPCHECK}
    !insertmacro SELECT_UNSECTION "Cscope plugin"                      ${UNSEC_CSCOPE}
    !insertmacro SELECT_UNSECTION "DevPak plugin"                      ${UNSEC_DEVPAK}
    !insertmacro SELECT_UNSECTION "DoxyBlocks plugin"                  ${UNSEC_DOXYBLOCKS}
    !insertmacro SELECT_UNSECTION "Drag Scroll plugin"                 ${UNSEC_DRAGSCROLL}
    !insertmacro SELECT_UNSECTION "EditorConfig plugin"                ${UNSEC_EDITORCONFIG}
    !insertmacro SELECT_UNSECTION "Editor tweaks plugin"               ${UNSEC_EDITORTWEAKS}
    !insertmacro SELECT_UNSECTION "EnvVars plugin"                     ${UNSEC_ENVVARS}
    !insertmacro SELECT_UNSECTION "File Manager plugin"                ${UNSEC_FILEMANAGER}
!ifdef FORTRAN_PLUGIN_FOUND
    !insertmacro SELECT_UNSECTION "Fortran Project plugin"             ${UNSEC_FORTRANPROJECT}
!endif    
    !insertmacro SELECT_UNSECTION "HeaderFixUp plugin"                 ${UNSEC_HEADERFIXUP}
    !insertmacro SELECT_UNSECTION "Help plugin"                        ${UNSEC_HELP}
    !insertmacro SELECT_UNSECTION "HexEditor plugin"                   ${UNSEC_HEXEDITOR}
    !insertmacro SELECT_UNSECTION "IncrementalSearch plugin"           ${UNSEC_INCREMENTALSEARCH}
    !insertmacro SELECT_UNSECTION "Key Binder plugin"                  ${UNSEC_KEYBINDER}
    !insertmacro SELECT_UNSECTION "Koders plugin"                      ${UNSEC_KODERS}
    !insertmacro SELECT_UNSECTION "Lib Finder plugin"                  ${UNSEC_LIBFINDER}
    !insertmacro SELECT_UNSECTION "MouseSap plugin"                    ${UNSEC_MOUSESAP}
    !insertmacro SELECT_UNSECTION "Nassi Shneiderman plugin"           ${UNSEC_NASSI}
    !insertmacro SELECT_UNSECTION "Occurrences Highlighting plugin"    ${UNSEC_OCC_HIGHLIGHTING}
    !insertmacro SELECT_UNSECTION "Tools+ plugin"                      ${UNSEC_TOOLSPLUS}
    !insertmacro SELECT_UNSECTION "Profiler plugin"                    ${UNSEC_PROFILER}
    !insertmacro SELECT_UNSECTION "Project Options Manipulator plugin" ${UNSEC_PRJOPTSMANIP}
    !insertmacro SELECT_UNSECTION "RegEx Testbed plugin"               ${UNSEC_REGEXTESTBED}
    !insertmacro SELECT_UNSECTION "ReOpen Editor plugin"               ${UNSEC_REOPEN}
    !insertmacro SELECT_UNSECTION "Exporter plugin"                    ${UNSEC_EXPORTER}
    !insertmacro SELECT_UNSECTION "SpellChecker plugin"                ${UNSEC_SPELLCHECKER}
    !insertmacro SELECT_UNSECTION "SymTab plugin"                      ${UNSEC_SYMTAB}
    !insertmacro SELECT_UNSECTION "ThreadSearch plugin"                ${UNSEC_THREADSEARCH}
    !insertmacro SELECT_UNSECTION "wxSmith plugin"                     ${UNSEC_WXSMITH}

    !insertmacro SELECT_UNSECTION "C::B CBP2Make"                      ${UNSEC_CBP2MAKE}
    !insertmacro SELECT_UNSECTION "C::B Share Config"                  ${UNSEC_SHARECONFIG}
    !insertmacro SELECT_UNSECTION "C::B Launcher"                      ${UNSEC_LAUNCHER}

FunctionEnd

########################
# Section Descriptions #
########################

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_DEFAULT}          "The default install which consists of the Code::Blocks core components and the core plugins."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE}                "Code::Blocks core components (without these, Code::Blocks will not work properly)."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_SHORTCUTS}        "Shortcuts to be created."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROGRAMSHORTCUT}     "Creates a shortcut to Code::Blocks in the startmenu."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DESKTOPSHORTCUT}     "Creates a shortcut to Code::Blocks on the desktop."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_QUICKLAUNCHSHORTCUT} "Creates a shortcut to Code::Blocks in the quick lauch bar."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_LEXERS}           "Lexer files provide syntax styling and delimiter matching for different programming languages and others."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_CORE_PLUGINS}     "Core plugins that are most likely desired. This includes the compiler and debugger plugin."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ABBREV}              "Speeds up the code typing with configurable abbreviations."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ASTYLE}              "Source code formatter. Uses AStyle to reformat your sources."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_AUTOSAVE}            "Saves your work in regular intervals."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CLASSWIZARD}         "Provides an easy way to create a new C++ class file pair."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODECOMPLETION}      "Provides a symbols browser for your projects and code-completion inside the editor."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPILER}            "Provides an interface to various compilers, including GNU compiler suite, Microsoft, Borland, etc."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEBUGGER}            "Provides interfaces to the GNU GDB and MS CDB debuggers."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_MIMEHANDLER}         "Provides a (default) files extension handler."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_OPENFILESLIST}       "Shows all currently open files (editors) in a list."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROJECTSIMPORTER}    "Imports foreign projects/workspaces (Dev-C++, MSVC6, MSVS7, MSVS8, MSVC10)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_RNDGEN}              "Provides a random number generator to be used for developing applications with intensive use of random numbers."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SCRIPTEDWIZARD}      "Provides a generic platform for creating project wizards (already includes a lot of wizards)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SMARTINDENT}         "Provided smart indention options for several languages (C++, Fortran, HDL, Lua, Pascal, Python, XML)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_TODOLIST}            "Provides a To-Do list and collects items accoringly from source files of a file/project/workspace."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_XPLOOKANDFEEL}       "Creates a manifest file that makes use of common controls 6.0 under Windows XP."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_CONTRIB_PLUGINS}  "Contributed plugins by the Code::Blocks user/developer community. These plugins extend the IDE nicely."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_AUTOVERSIONING}      "Auto increments the version and build number of your application every time a change has been made."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BROWSETRACKER}       "Browse to previous source positions / editors comfortable."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BYOGAMES}            "Provides a collection of games inside C::B for fun."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CCCC}                "A plugin for code analysis based on Cccc (C and C++ Code Counter)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EDITORCONFIG}        "Allow different editor configurations (like tabs, spaces etc.) per project."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EDITORTWEAKS}        "Several source code editor tweaks and bits like quick access to settings or alignment of source code."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODESNIPPETS}        "Allows to create and save small pieces of code (snippets) for later use."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODESTAT}            "A plugin for counting code, comments and empty lines of a project."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_COPYSTRINGS}         "Copies all the strings in the current editor into the clipboard."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CPPCHECK}            "A plugin for code analysis based on CppCheck."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CSCOPE}              "A plugin for code analysis based on Cscope."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEVPAK}              "Installs selected DevPaks from the internet."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DOXYBLOCKS}          "Add Doxygen documentation generator support for Code::Blocks."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DRAGSCROLL}          "Mouse drag and scroll using right or middle mouse key."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ENVVARS}             "Sets up environment variables within the focus of Code::Blocks."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_FILEMANAGER}         "Browses folders and files directly inside Code::Blocks (Explorer-like)."
!ifdef FORTRAN_PLUGIN_FOUND
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_FORTRANPROJECT}      "Extension for Code::Blocks to develop Fortran applications (compiler, CodeCompletion...)."
!endif
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_HEADERFIXUP}         "Provides analysis of header files according a customisable setup. C::B and wxWidgets are included by default."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_HELP}                "Add a list of help/MAN files to the help menu so you can have them handy to launch."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_HEXEDITOR}           "Provides an embedded very powerful hex editor to Code::Blocks (supports large binary files, too)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_INCREMENTALSEARCH}   "Searches and highlights a marked text incrementally within the open editor."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_KEYBINDER}           "Provides the user an ability to bind custom key combinations to the menu items."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_KODERS}              "Provides an interface to search for code snippets at the Koders webpage."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_LIBFINDER}           "Tool which automatically searches for installed libraries and adds them to global variables and projects."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_MOUSESAP}            "Plugin to provide middle mouse select and paste functionality."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_NASSI}               "Generate and use source code with Nassi Shneiderman diagrams."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_OCC_HIGHLIGHTING}    "Highlight occurrences of specific characters, words or phrases accross documents."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_TOOLSPLUS}           "Provides a refined Code::Blocks Tools menu."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROFILER}            "Provides a simple graphical interface to the GNU GProf profiler."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PRJOPTSMANIP}        "Provides powerful mechanisms to mass set/edit/delete project options wrt compiler, linker, etc."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_REGEXTESTBED}        "Provides a regular expressions testbed."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_REOPEN}              "Helper plugin to quickly re-open recently closed editors."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXPORTER}            "Provides the ability to export syntax highlighted source files to HTML, RTF, ODT or PDF."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SPELLCHECKER}        "Online spell checker for Code::Blocks (required additional free dictionaries to be downloaded e.g. from OpenOffice.org)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SYMTAB}              "Provides a simple graphical interface to the GNU symbol table displayer (nm)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_THREADSEARCH}        "Multi-threaded 'Search in files' with preview window."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_WXSMITH}             "RAD tool used to create wxWidgets based GUI applications, forms, dialogs and other."

!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CBP2MAKE}            "Allow to convert Code::Blocks Project (*.cbp) files into Makefiles for misc. platforms."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SHARECONFIG}         "Allows sharing of most important settings between Code::Blocks instances or different users."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_LAUNCHER}            "Makes Code::Blocks portable on Windows, including config from APPDATA and alike."

!insertmacro MUI_FUNCTION_DESCRIPTION_END

# ========================================================================================================================
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# ========================================================================================================================
Function confirm_function
  !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE ""
  !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "$(UMUI_TEXT_INSTCONFIRM_TEXTBOX_DESTINATION_LOCATION)"
  !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "      $INSTDIR"
  !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE ""
  
  ;Only if StartMenu Folder is selected
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "$(UMUI_TEXT_INSTCONFIRM_TEXTBOX_START_MENU_FOLDER)"
    !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "      $STARTMENU_FOLDER_INSTALL"
    !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE ""

    ;ShellVarContext
    !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "$(UMUI_TEXT_SHELL_VAR_CONTEXT)"
    !insertmacro UMUI_GETSHELLVARCONTEXT
    Pop $1
    StrCmp $1 "all" 0 current
      !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "      $(UMUI_TEXT_SHELL_VAR_CONTEXT_FOR_ALL_USERS)"
      Goto endsvc
    current:
      !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE "      $(UMUI_TEXT_SHELL_VAR_CONTEXT_ONLY_FOR_CURRENT_USER)"
    endsvc:
    !insertmacro UMUI_CONFIRMPAGE_TEXTBOX_ADDLINE ""

  !insertmacro MUI_STARTMENU_WRITE_END
FunctionEnd
# ========================================================================================================================
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# ========================================================================================================================

Function preuninstall_function

  ; execute this function only in Modify, repair and update function
  !insertmacro UMUI_IF_INSTALLFLAG_IS ${UMUI_MODIFY}|${UMUI_REPAIR}|${UMUI_UPDATE}

    ;ADD YOUR DELETE INSTRUCTION HERE...
  
    Delete "$INSTDIR\Uninstall.exe"
  
    DeleteRegKey ${UMUI_PARAMS_REGISTRY_ROOT} "${UMUI_PARAMS_REGISTRY_KEY}"

  !insertmacro UMUI_ENDIF_INSTALLFLAG

FunctionEnd

# ========================================================================================================================
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# ========================================================================================================================

Function CompilerInstallerDownloadRun
    pop $R0 ; Temp Downloaded filename
    pop $R1 ; URL of file to download
    StrCpy $2 "$TEMP\$R0"
    Delete "$2"

    DetailPrint           "wget.exe $R1 -O $2"
    NSExec::exec "$INSTDIR\wget.exe $R1 -O $2"
    Pop $0 ;Get the return value in $0
    StrCmp $0 0 0 downloadfailed
    DetailPrint "Successfully downloaded : $2"
    DetailPrint "Run : $2"
    ExecWait "$2"
    Delete "$2"
    DetailPrint "Delete : $2"
    goto finishedInstall

downloadfailed:
    SetDetailsView show
    DetailPrint "Download failed : $0"
    MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Could not download the $R1 file.$\r$\nAs such you will need to manually install MinGW by yourself." \
        /SD IDOK

finishedInstall:
    # =============================================================================================
FunctionEnd

Function CompilerInstallerDownloadRun_MinGW
    push "https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe"
    push "mingw-w64-install.exe"
    Call CompilerInstallerDownloadRun
FunctionEnd

Function CompilerInstallerDownloadRun_TDM
    push "https://github.com/jmeubank/tdm-gcc/releases/download/v1.2105.1/tdm-gcc-webdl.exe"
    push "tdm-gcc-webdl.exe"
    Call CompilerInstallerDownloadRun
    
FunctionEnd

Function CompilerInstallerDownloadRun_MSYS2
    push "https://sourceforge.net/projects/msys2/files/Base/msys2-x86_64-latest.exe"
    push "msys2-x86_64-latest.exe"
    Call CompilerInstallerDownloadRun
FunctionEnd

Function CompilerInstallerDownloadRun_Cygwin
    push "https://www.cygwin.com/setup-x86_64.exe"
    push "cygwin_setup-x86_64.exe"
    Call CompilerInstallerDownloadRun
FunctionEnd

Var HWND_COMPILERDLPAGE

Function CompilerDownloadPage_Show

    ; Does not show page if setup cancelled, required only if UMUI_PAGE_ABORT inserted
    !insertmacro UMUI_ABORT_IF_INSTALLFLAG_IS ${UMUI_CANCELLED}

    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "NSIS_CompilerDownload.ini"
    !insertmacro MUI_HEADER_TEXT "Compiler Installer Download and Run" ""

    !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "NSIS_CompilerDownload.ini"
    Pop $HWND_COMPILERDLPAGE ;HWND of dialog

    ; Set control backgrounds to be the same as the background
    GetDlgItem $0 $HWND_COMPILERDLPAGE 1200
    !insertmacro UMUI_IOPAGECTLTRANSPARENT_INIT $0
    GetDlgItem $0 $HWND_COMPILERDLPAGE 1201
    !insertmacro UMUI_IOPAGECTLTRANSPARENT_INIT $0
    GetDlgItem $0 $HWND_COMPILERDLPAGE 1202
    !insertmacro UMUI_IOPAGECTLTRANSPARENT_INIT $0
    GetDlgItem $0 $HWND_COMPILERDLPAGE 1203
    !insertmacro UMUI_IOPAGECTLTRANSPARENT_INIT $0
    GetDlgItem $0 $HWND_COMPILERDLPAGE 1204
    !insertmacro UMUI_IOPAGECTLTRANSPARENT_INIT $0

    ; set page background color to be the same as the background
    !insertmacro UMUI_IOPAGEBGTRANSPARENT_INIT $HWND_COMPILERDLPAGE

    !insertmacro MUI_INSTALLOPTIONS_SHOW
FunctionEnd

Function CompilerDownloadPage_Leave
    ;${LogText} "Leaving compiler setup page"

    !insertmacro INSTALLOPTIONS_READ $R1 "NSIS_CompilerDownload.ini" "Field 1" "State"
    !insertmacro INSTALLOPTIONS_READ $R2 "NSIS_CompilerDownload.ini" "Field 2" "State"
    !insertmacro INSTALLOPTIONS_READ $R3 "NSIS_CompilerDownload.ini" "Field 3" "State"
    !insertmacro INSTALLOPTIONS_READ $R4 "NSIS_CompilerDownload.ini" "Field 4" "State"
    !insertmacro INSTALLOPTIONS_READ $R5 "NSIS_CompilerDownload.ini" "Field 5" "State"
    ; "Field 5IS A 
    ; MessageBox MB_OK|MB_ICONINFORMATION  "Field 1: $R1 , 2: $R2 , 3: $R3 , 4: $R4" , 5: $R5  /SD IDOK

    ${If} $R1 == "1"
        Call CompilerInstallerDownloadRun_MinGW
    ${EndIf}
        ${If} $R2 == "1"
            ExecShell "open" "https://github.com/ssbssa/gdb/releases" SW_SHOWNORMAL
        ${EndIf}

    ${If} $R3 == "1"
        Call CompilerInstallerDownloadRun_TDM
    ${EndIf}
    ${If} $R4 == "1"
        Call CompilerInstallerDownloadRun_MSYS2
    ${EndIf}
    ${If} $R5 == "1"
        Call CompilerInstallerDownloadRun_Cygwin
    ${EndIf}

FunctionEnd


Function onMultiUserModeChanged
    ${If} $MultiUser.InstallMode == "CurrentUser"
        StrCpy $InstDir "$LocalAppdata\Programs\${MULTIUSER_INSTALLMODE_INSTDIR}"
    ${Else}
        !if ${BUILD_TYPE} == 32
            StrCpy $InstDir "$PROGRAMFILES\${MULTIUSER_INSTALLMODE_INSTDIR}"
        !else
            StrCpy $InstDir "$PROGRAMFILES64\${MULTIUSER_INSTALLMODE_INSTDIR}"
        !endif
    ${EndIf}
FunctionEnd