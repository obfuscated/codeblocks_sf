#####################################################################
# The installer is devided into 4 main sections (section groups):   #
# "Default"           -> includes C::B core and core plugins        #
# "Lexers"            -> includes C::B lexers for different langs   #
# "Contrib plugins"   -> includes C::B contrib plugins              #
# "C::B share config" -> includes the C::B Share Config tool        #
#####################################################################
# What to do to add a new installer section (e.g. a new plugin):    #
# 1.) Add Installer section                                         #
# 2.) Add Uninstaller section (files in reverse order of installer) #
# 3.) Add macro to uninstaller functions                            #
# 4.) Add section description                                       #
#####################################################################

Name CodeBlocks

#########################################################
# Room for adjustments of most important settings BEGIN #
#########################################################

# The following line toggles whether the installer includes the MinGW
# compiler suite (including GDB) or not. Comment out to exclude MinGW.
#!define MINGW_BUNDLE

# Defines
!define REGKEY           "SOFTWARE\$(^Name)"
!define VERSION          8.02
!define COMPANY          "The Code::Blocks Team"
!define URL              http://www.codeblocks.org
# Possibly required to adjust manually:
!define WX_BASE          C:\wxMSW-2.8.7\lib\gcc_dll
# Possibly required to adjust manually:
!define CB_BASE          C:\CodeBlocks\src\output
!define CB_SHARE         \share
!define CB_SHARE_CB      ${CB_SHARE}\CodeBlocks
!define CB_DOCS          ${CB_SHARE_CB}\docs
!define CB_LEXERS        ${CB_SHARE_CB}\lexers
!define CB_PLUGINS       ${CB_SHARE_CB}\plugins
!define CB_SCRIPTS       ${CB_SHARE_CB}\scripts
!define CB_TEMPLATES     ${CB_SHARE_CB}\templates
!define CB_WIZARD        ${CB_TEMPLATES}\wizard
!define CB_IMAGES        ${CB_SHARE_CB}\images
!define CB_IMG_16        ${CB_IMAGES}\16x16
!define CB_IMG_SETTINGS  ${CB_IMAGES}\settings
# Possibly required to adjust manually:
!define CB_ADDONS        ${CB_BASE}\..\installer
!define CB_DOCS_SRC      ${CB_BASE}\..\installer
!ifdef MINGW_BUNDLE
# Possibly required to adjust manually:
!define MINGW_BASE       D:\Devel\CodeBlocks_Devel\src\setup\MinGW
!define CB_MINGW         \MinGW
!endif
# Possibly required to adjust manually:
# Note: This file is only rerquired for the installer.
!define CB_SPLASH        ${CB_ADDONS}\setup_splash.bmp
!define CB_LOGO          ${CB_ADDONS}\setup_logo.bmp
# Possibly required to adjust manually:
# Note: This file is only rerquired for the installer.
!define CB_LICENSE       ${CB_ADDONS}\gpl-3.0.txt
!define CB_SM_GROUP      $(^Name)

# Interface configuration (MUI defines)
!define MUI_ICON                             "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP               "${CB_LOGO}" ; optional
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNICON                           "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

#######################################################
# Room for adjustments of most important settings END #
#######################################################

##################################################################################
# Usually below here no changes are rquired unless adding new installer features #
##################################################################################

# Included files
!include Sections.nsh
!include MUI.nsh

# Reserved Files
ReserveFile "${NSISDIR}\Plugins\AdvSplash.dll"

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE    ${CB_LICENSE}
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes (usually these do not change)
# Note: Not always we can use "Code::Blocks" as the "::" conflicts with the filesystem.
OutFile           setup.exe
InstallDir        $PROGRAMFILES\CodeBlocks
CRCCheck          on
XPStyle           on
ShowInstDetails   show
VIProductVersion  1.0.0.0
VIAddVersionKey   ProductName     "Code::Blocks"
VIAddVersionKey   ProductVersion  "${VERSION}"
VIAddVersionKey   CompanyName     "${COMPANY}"
VIAddVersionKey   CompanyWebsite  "${URL}"
VIAddVersionKey   FileVersion     "${VERSION}"
VIAddVersionKey   FileDescription "Code::Blocks cross-platform IDE"
VIAddVersionKey   LegalCopyright  ""
InstallDirRegKey  HKCU "${REGKEY}" Path
ShowUninstDetails show

######################
# Installer sections #
######################

# These are installer types.
# They basically wrap different selections of components.
# To use a component in section "Full" (=1) and "Edit" (=4) choose
# "SectionIn 1 4" in the section(s) accordingly (see how it's done below).
InstType "Full: All plugins, all tools, just everything"
InstType "Standard: Core plugins, core tools, and core lexers"
InstType "Minimal: Important plugins, important lexers"
InstType "Editor: Code::Blocks as editor only (all lexers)"

# Now the installer sections and section groups start.
# They basically define the tree of components available.
SectionGroup "!Default install" SECGRP_DEFAULT

    # C::B core begin

    Section "!Core Files (required)" SEC_CORE
        SectionIn 1 2 3 4 RO
        # If $INSTDIR is present, ask the user if that is OK
        IfFileExists $INSTDIR 0 doInstall
            # It appears $INSTDIR is present (and not empty).
            # Ask the user to probably abort installation process.
            MessageBox MB_YESNO|MB_ICONQUESTION \
                "The target directory does already exist. OK to continue anyways?$\r$\n(If you are unsure and want to keep the folder, click No.)" \
                IDYES continueInstall IDNO abortInstall
abortInstall:
            # If user selected "no" -> abort the installation
            DetailPrint "Aborting installation."
            Abort
continueInstall:
            # fall through
doInstall:
        SetOutPath $INSTDIR
        # Verify if creating/accessing the target folder succeeded.
        # If not, issue an error message and abort installation
        IfErrors 0 accessOK
            MessageBox MB_OK|MB_ICONEXCLAMATION \
                "Cannot create the target folder.$\r$\nInstallation cannot continue.$\r$\n(Probably missing access rights?)"
            DetailPrint "Aborting installation."
            Abort
accessOK:
        SetOverwrite on
        File ${WX_BASE}\wxmsw28u_gcc_cb.dll
        File ${CB_BASE}\cb_console_runner.exe
        File ${CB_BASE}\codeblocks.dll
        File ${CB_BASE}\codeblocks.exe
        File ${CB_BASE}\exchndl.dll
        File ${CB_BASE}\mingwm10.dll
        File ${CB_BASE}\wxscintilla.dll
        SetOutPath $INSTDIR${CB_SHARE_CB}
        File ${CB_BASE}${CB_SHARE_CB}\start_here.zip
        File ${CB_BASE}${CB_SHARE_CB}\tips.txt
        File ${CB_BASE}${CB_SHARE_CB}\manager_resources.zip
        File ${CB_BASE}${CB_SHARE_CB}\resources.zip
        SetOutPath $INSTDIR${CB_DOCS}
        File ${CB_DOCS_SRC}\codeblocks.chm
        File ${CB_DOCS_SRC}\index.ini
        SetOutPath $INSTDIR${CB_SCRIPTS}
        File ${CB_BASE}${CB_SCRIPTS}\*.script
        SetOutPath $INSTDIR${CB_TEMPLATES}
        File ${CB_BASE}${CB_TEMPLATES}\*.*
        SetOutPath $INSTDIR${CB_IMAGES}
        File ${CB_BASE}${CB_IMAGES}\*.png
        SetOutPath $INSTDIR${CB_IMG_16}
        File ${CB_BASE}${CB_IMG_16}\*.png
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "Core Files (required)" 1
    SectionEnd

    # C::B core end

    # C::B shortcuts begin

    SectionGroup "Shortcuts" SECGRP_SHORTCUTS

        Section "Program Shortcut" SEC_PROGRAMSHORTCUT
            SectionIn 1 2 3 4
            SetOutPath $SMPROGRAMS\${CB_SM_GROUP}
            CreateShortcut "$SMPROGRAMS\${CB_SM_GROUP}\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            WriteRegStr HKCU "${REGKEY}\Components" "Program Shortcut" 1
        SectionEnd

        Section /o "Program Shortcut All Users" SEC_PROGRAMSHORTCUT_ALL
            SectionIn 1 2 3 4
            SetShellVarContext all
            SetOutPath $SMPROGRAMS\${CB_SM_GROUP}
            CreateShortcut "$SMPROGRAMS\${CB_SM_GROUP}\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            # Verify if that succeeded. If not, issue an error message
            IfErrors 0 +2
                MessageBox MB_OK|MB_ICONEXCLAMATION \
                    "Cannot create shortcut for all users.$\r$\n(Probably missing admin rights?)"
            SetShellVarContext current
            WriteRegStr HKCU "${REGKEY}\Components" "Program Shortcut All Users" 1
        SectionEnd

        Section /o "Desktop Shortcut" SEC_DESKTOPSHORTCUT
            SectionIn 1
            CreateShortCut "$DESKTOP\$(^Name).lnk" $INSTDIR\CodeBlocks.exe
            WriteRegStr HKCU "${REGKEY}\Components" "Desktop Shortcut" 1
        SectionEnd

        Section /o "Quick Launch Shortcut" SEC_QUICKLAUNCHSHORTCUT
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

            Section /o "The D Language"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_d.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_d.xml
                WriteRegStr HKCU "${REGKEY}\Components" "The D Language" 1
            SectionEnd

            Section /o "Fortran"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_f77.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_f77.xml
                File ${CB_BASE}${CB_LEXERS}\lexer_fortran.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_fortran.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Fortran" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Script Languages"
            Section "Squirrel"
                SectionIn 1 2 3 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_squirrel.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_squirrel.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Squirrel" 1
            SectionEnd

            Section /o "Angelscript"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_angelscript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_angelscript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Angelscript" 1
            SectionEnd

            Section /o "Lua"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_lua.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_lua.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Lua" 1
            SectionEnd

            Section /o "Python"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_python.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_python.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Python" 1
            SectionEnd

            Section /o "Perl"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_perl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_perl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Perl" 1
            SectionEnd

            Section /o "VB Script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_vbscript.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_vbscript.xml
                WriteRegStr HKCU "${REGKEY}\Components" "VB Script" 1
            SectionEnd

            Section /o "Game Monkey"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_gm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_gm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Game Monkey" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Markup Languages"
            Section "XML"
                SectionIn 1 2 3 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_xml.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_xml.xml
                WriteRegStr HKCU "${REGKEY}\Components" "XML" 1
            SectionEnd

            Section /o "HTML"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_html.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_html.xml
                WriteRegStr HKCU "${REGKEY}\Components" "HTML" 1
            SectionEnd

            Section /o "CSS"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_css.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_css.xml
                WriteRegStr HKCU "${REGKEY}\Components" "CSS" 1
            SectionEnd

            Section /o "LaTeX"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_latex.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_latex.xml
                WriteRegStr HKCU "${REGKEY}\Components" "LaTeX" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Graphics Programming"
            Section /o "GLSL (GLslang)"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_glsl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_glsl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "GLSL (GLslang)" 1
            SectionEnd

            Section /o "nVidia Cg"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_cg.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_cg.xml
                WriteRegStr HKCU "${REGKEY}\Components" "nVidia Cg" 1
            SectionEnd

            Section /o "Ogre"
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
            Section /o "Hitachi Assembler"
                SetOutPath $INSTDIR${CB_LEXERS}
                SectionIn 1 4
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_hitasm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_hitasm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Hitachi Assembler" 1
            SectionEnd

            Section /o "VHDL"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_vhdl.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_vhdl.xml
                WriteRegStr HKCU "${REGKEY}\Components" "VHDL" 1
            SectionEnd

            Section /o "Verilog"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_verilog.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_verilog.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Verilog" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Shell / Binutils"
            Section /o "diff"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_diff.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_diff.xml
                WriteRegStr HKCU "${REGKEY}\Components" "diff" 1
            SectionEnd

            Section /o "Makefile"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_make.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_make.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Makefile" 1
            SectionEnd

            Section /o "bash script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_bash.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_bash.xml
                WriteRegStr HKCU "${REGKEY}\Components" "bash script" 1
            SectionEnd

            Section /o "DOS batch files"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_batch.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_batch.xml
                WriteRegStr HKCU "${REGKEY}\Components" "DOS batch files" 1
            SectionEnd
        SectionGroupEnd


        SectionGroup "Others"
            Section /o "MASM"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_masm.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_masm.xml
                WriteRegStr HKCU "${REGKEY}\Components" "MASM" 1
            SectionEnd

            Section /o "NSIS installer script"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_nsis.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_nsis.xml
                WriteRegStr HKCU "${REGKEY}\Components" "NSIS installer script" 1
            SectionEnd

            Section /o "Matlab"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_matlab.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_matlab.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Matlab" 1
            SectionEnd

            Section /o "XBase"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_prg.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_prg.xml
                WriteRegStr HKCU "${REGKEY}\Components" "XBase" 1
            SectionEnd

            Section /o "Property file"
                SectionIn 1 4
                SetOutPath $INSTDIR${CB_LEXERS}
                SetOverwrite on
                File ${CB_BASE}${CB_LEXERS}\lexer_properties.sample
                File ${CB_BASE}${CB_LEXERS}\lexer_properties.xml
                WriteRegStr HKCU "${REGKEY}\Components" "Property file" 1
            SectionEnd
        SectionGroupEnd
    SectionGroupEnd

    # C::B lexers end

    # C::B core plugins begin

    SectionGroup "Core Plugins" SECGRP_CORE_PLUGINS
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
            SetOutPath $INSTDIR${CB_IMAGES}\codecompletion
            File ${CB_BASE}${CB_IMAGES}\codecompletion\*.png
            WriteRegStr HKCU "${REGKEY}\Components" "Code Completion plugin" 1
        SectionEnd

        Section "Compiler plugin" SEC_COMPILER
            SectionIn 1 2 3
            SetOutPath $INSTDIR${CB_SHARE_CB}
            SetOverwrite on
            File ${CB_BASE}${CB_SHARE_CB}\compiler.zip
            SetOutPath $INSTDIR${CB_PLUGINS}
            File ${CB_BASE}${CB_PLUGINS}\compiler.dll
            SetOutPath $INSTDIR${CB_IMAGES}
            File ${CB_BASE}${CB_IMAGES}\compile.png
            File ${CB_BASE}${CB_IMAGES}\compilerun.png
            File ${CB_BASE}${CB_IMAGES}\rebuild.png
            File ${CB_BASE}${CB_IMAGES}\run.png
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
            File ${CB_BASE}${CB_IMAGES}\dbgnext.png
            File ${CB_BASE}${CB_IMAGES}\dbgrun.png
            File ${CB_BASE}${CB_IMAGES}\dbgrunto.png
            File ${CB_BASE}${CB_IMAGES}\dbgstep.png
            File ${CB_BASE}${CB_IMAGES}\dbgstop.png
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

    Section /o "Auto Versioning plugin" SEC_AUTOVERSIONING
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\AutoVersioning.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\AutoVersioning.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Auto Versioning plugin" 1
    SectionEnd

    Section /o "Browse Tracker plugin" SEC_BROWSETRACKER
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\BrowseTracker.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\BrowseTracker.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Browse Tracker plugin" 1
    SectionEnd

    Section /o "Byo Games plugin" SEC_BYOGAMES
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\byogames.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\byogames.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Byo Games plugin" 1
    SectionEnd

    Section /o "Koders plugin" SEC_KODERS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\cb_koders.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\cb_koders.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Koders plugin" 1
    SectionEnd

    Section /o "Code Snippets plugin" SEC_CODESNIPPETS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\codesnippets.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\codesnippets.dll
        SetOutPath $INSTDIR
        File ${CB_BASE}\codesnippets.exe
        SetOutPath $SMPROGRAMS\${CB_SM_GROUP}
        CreateShortcut "$SMPROGRAMS\${CB_SM_GROUP}\Code Snippets.lnk" $INSTDIR\codesnippets.exe
        WriteRegStr HKCU "${REGKEY}\Components" "Code Snippets plugin" 1
    SectionEnd

    Section /o "Code Stat plugin" SEC_CODESTAT
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\codestat.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\codestat.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\codestats.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\codestats-off.png
        WriteRegStr HKCU "${REGKEY}\Components" "Code Stat plugin" 1
    SectionEnd

    Section /o "Copy Strings plugin" SEC_COPYSTRINGS
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\copystrings.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\copystrings.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Copy Strings plugin" 1
    SectionEnd

    Section /o "DevPak plugin" SEC_DEVPAK
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\devpakupdater.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\devpakupdater.dll
        WriteRegStr HKCU "${REGKEY}\Components" "DevPak plugin" 1
    SectionEnd

    Section /o "Drag Scroll plugin" SEC_DRAGSCROLL
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

    Section /o "EnvVars plugin" SEC_ENVVARS
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

    Section /o "Help plugin" SEC_HELP
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

    Section /o "Key Binder plugin" SEC_KEYBINDER
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

    Section /o "Lib Finder plugin" SEC_LIBFINDER
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

    Section /o "Profiler plugin" SEC_PROFILER
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

    Section /o "RegEx Testbed plugin" SEC_REGEXTESTBED
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\RegExTestbed.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\RegExTestbed.dll
        WriteRegStr HKCU "${REGKEY}\Components" "RegEx Testbed plugin" 1
    SectionEnd

    Section /o "Exporter plugin" SEC_EXPORTER
        SectionIn 1 4
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\Exporter.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\Exporter.dll
        WriteRegStr HKCU "${REGKEY}\Components" "Exporter plugin" 1
    SectionEnd

    Section /o "SymTab plugin" SEC_SYMTAB
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\SymTab.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\SymTab.dll
        WriteRegStr HKCU "${REGKEY}\Components" "SymTab plugin" 1
    SectionEnd

    Section /o "ThreadSearch plugin" SEC_THREADSEARCH
        SectionIn 1
        SetOutPath $INSTDIR${CB_SHARE_CB}
        SetOverwrite on
        File ${CB_BASE}${CB_SHARE_CB}\ThreadSearch.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\ThreadSearch.dll
        SetOutPath $INSTDIR${CB_IMG_SETTINGS}
        File ${CB_BASE}${CB_IMG_SETTINGS}\ThreadSearch.png
        File ${CB_BASE}${CB_IMG_SETTINGS}\ThreadSearch-off.png
        SetOutPath $INSTDIR${CB_IMAGES}\ThreadSearch
        File ${CB_BASE}${CB_IMAGES}\ThreadSearch\*.png
        WriteRegStr HKCU "${REGKEY}\Components" "ThreadSearch plugin" 1
    SectionEnd

    Section "wxSmith plugin" SEC_WXSMITH
        SectionIn 1 2
        SetOutPath $INSTDIR
        SetOverwrite on
        File ${CB_BASE}\wxsmithlib.dll
        SetOutPath $INSTDIR${CB_SHARE_CB}
        File ${CB_BASE}${CB_SHARE_CB}\wxsmith.zip
        File ${CB_BASE}${CB_SHARE_CB}\wxSmithContribItems.zip
        SetOutPath $INSTDIR${CB_PLUGINS}
        File ${CB_BASE}${CB_PLUGINS}\wxsmith.dll
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

Section /o "C::B Share Config" SEC_SHARECONFIG
    SectionIn 1
    SetOutPath $INSTDIR
    SetOverwrite on
    File ${CB_BASE}\cb_share_config.exe
    SetOutPath $SMPROGRAMS\${CB_SM_GROUP}
    CreateShortcut "$SMPROGRAMS\${CB_SM_GROUP}\CB Share Config.lnk" $INSTDIR\cb_share_config.exe
    WriteRegStr HKCU "${REGKEY}\Components" "C::B Share Config" 1
SectionEnd

!ifdef MINGW_BUNDLE
Section "MinGW Compiler Suite" SEC_MINGW
    SectionIn 1 2 3
    SetOutPath $INSTDIR${CB_MINGW}
    SetOverwrite on
    File /r ${MINGW_BASE}\*.*
    WriteRegStr HKCU "${REGKEY}\Components" "MinGW Compiler Suite" 1
SectionEnd
!endif

Section -post SEC_MISC
    WriteRegStr HKCU "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    SetOutPath $SMPROGRAMS\${CB_SM_GROUP}
    CreateShortcut "$SMPROGRAMS\${CB_SM_GROUP}\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1

    MessageBox MB_YESNO|MB_ICONQUESTION \
        "Do you want to run Code::Blocks now?" IDYES yesRunCB IDNO noRunCB
yesRunCB:
    DetailPrint "Running Code::Blocks."
    Exec '"$INSTDIR\codeblocks.exe"'
noRunCB:
    # fall through
SectionEnd

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

!ifdef MINGW_BUNDLE
Section "-un.MinGW Compiler Suite" UNSEC_MINGW
    Delete /REBOOTOK $INSTDIR${CB_MINGW}\*.*
    RMDir  /r /REBOOTOK $INSTDIR${CB_MINGW}
    DeleteRegValue HKCU "${REGKEY}\Components" "MinGW Compiler Suite"
SectionEnd
!endif

Section /o "-un.C::B Share Config" UNSEC_SHARECONFIG
    Delete /REBOOTOK $INSTDIR\cb_share_config.exe
    Delete /REBOOTOK "$SMPROGRAMS\${CB_SM_GROUP}\CB Share Config.lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "C::B Share Config"
SectionEnd

# C::B contrib plugins begin

Section /o "-un.Auto Versioning plugin" UNSEC_AUTOVERSIONING
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\AutoVersioning.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\AutoVersioning.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Auto Versioning plugin"
SectionEnd

Section /o "-un.Browse Tracker plugin" UNSEC_BROWSETRACKER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\BrowseTracker.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\BrowseTracker.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Browse Tracker plugin"
SectionEnd

Section /o "-un.Byo Games plugin" UNSEC_BYOGAMES
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\byogames.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\byogames.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Byo Games plugin"
SectionEnd

Section /o "-un.Koders plugin" UNSEC_KODERS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\cb_koders.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\cb_koders.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Koders plugin"
SectionEnd

Section /o "-un.Code Snippets plugin" UNSEC_CODESNIPPETS
    Delete /REBOOTOK $INSTDIR\codesnippets.exe
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codesnippets.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codesnippets.zip
    Delete /REBOOTOK "$SMPROGRAMS\${CB_SM_GROUP}\Code Snippets.lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Snippets plugin"
SectionEnd

Section /o "-un.Code Stat plugin" UNSEC_CODESTAT
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codestats-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\codestats.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codestat.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codestat.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Stat plugin"
SectionEnd

Section /o "-un.Copy Strings plugin" UNSEC_COPYSTRINGS
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\copystrings.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\copystrings.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Copy Strings plugin"
SectionEnd

Section /o "-un.DevPak plugin" UNSEC_DEVPAK
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\devpakupdater.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\devpakupdater.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "DevPak plugin"
SectionEnd

Section /o "-un.Drag Scroll plugin" UNSEC_DRAGSCROLL
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\dragscroll-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\dragscroll.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\DragScroll.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\DragScroll.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Drag Scroll plugin"
SectionEnd

Section /o "-un.EnvVars plugin" UNSEC_ENVVARS
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\envvars-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\envvars.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\envvars.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\envvars.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "EnvVars plugin"
SectionEnd

Section /o "-un.Help plugin" UNSEC_HELP
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\help-plugin-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\help-plugin.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\help_plugin.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\help_plugin.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Help plugin"
SectionEnd

Section /o "-un.Key Binder plugin" UNSEC_KEYBINDER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\onekeytobindthem-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\onekeytobindthem.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\keybinder.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\keybinder.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Key Binder plugin"
SectionEnd

Section /o "-un.Lib Finder plugin" UNSEC_LIBFINDER
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder\*.xml
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\lib_finder.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\lib_finder.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Lib Finder plugin"
SectionEnd

Section /o "-un.Profiler plugin" UNSEC_PROFILER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\profiler-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\profiler.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Profiler.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Profiler.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Profiler plugin"
SectionEnd

Section /o "-un.RegEx Testbed plugin" UNSEC_REGEXTESTBED
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\RegExTestbed.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\RegExTestbed.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "RegEx Testbed plugin"
SectionEnd

Section /o "-un.Exporter plugin" UNSEC_EXPORTER
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\Exporter.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\Exporter.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Exporter plugin"
SectionEnd

Section /o "-un.SymTab plugin" UNSEC_SYMTAB
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\SymTab.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\SymTab.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "SymTab plugin"
SectionEnd

Section /o "-un.ThreadSearch plugin" UNSEC_THREADSEARCH
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\ThreadSearch\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\ThreadSearch
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
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\wxSmithContribItems.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\wxsmith.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\wxSmithContribItems.zip
    Delete /REBOOTOK $INSTDIR\wxsmithlib.dll
    DeleteRegValue HKCU "${REGKEY}\Components" "wxSmith plugin"
SectionEnd

# C::B contrib plugins end

# C::B core plugins begin

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
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\codecompletion.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\codecompletion.zip
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\codecompletion\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}\codecompletion
    DeleteRegValue HKCU "${REGKEY}\Components" "Code Completion plugin"
SectionEnd

Section "-un.Compiler plugin" UNSEC_COMPILER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\compiler-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\compiler.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\stop.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\run.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\rebuild.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\compilerun.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\compile.png
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\compiler.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\compiler.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Compiler plugin"
SectionEnd

Section "-un.Debugger plugin" UNSEC_DEBUGGER
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\debugger-off.png
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\debugger.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\dbgstop.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\dbgstep.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\dbgrunto.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\dbgrun.png
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\dbgnext.png
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

Section "-un.Scripted Wizard plugin" UNSEC_SCRIPTEDWIZARD
    Delete /REBOOTOK $INSTDIR${CB_WIZARD}\*.*
    RMDir  /r /REBOOTOK $INSTDIR${CB_WIZARD}
    Delete /REBOOTOK $INSTDIR${CB_PLUGINS}\scriptedwizard.dll
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\scriptedwizard.zip
    DeleteRegValue HKCU "${REGKEY}\Components" "Scripted Wizard plugin"
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

Section "-un.C/C++" UNSEC_CPP
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_rc.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_rc.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cpp.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cpp.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "C/C++"
SectionEnd

Section /o "-un.The D Language" UNSEC_D
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_d.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_d.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "The D Language"
SectionEnd

Section /o "-un.Fortran" UNSEC_F
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_f77.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_f77.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_fortran.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_fortran.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Fortran"
SectionEnd

Section "-un.Squirrel" UNSEC_SQ
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_squirrel.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_squirrel.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Squirrel"
SectionEnd

Section /o "-un.Angelscript" UNSEC_AS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_angelscript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_angelscript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Angelscript"
SectionEnd

Section /o "-un.Lua" UNSEC_LUA
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lua.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_lua.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Lua"
SectionEnd

Section /o "-un.Python" UNSEC_PY
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_python.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_python.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Python"
SectionEnd

Section /o "-un.Perl" UNSEC_PERL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_perl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_perl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Perl"
SectionEnd

Section /o "-un.VB Script" UNSEC_VB
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vbscript.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vbscript.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "VB Script"
SectionEnd

Section /o "-un.Game Monkey" UNSEC_GM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_gm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_gm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Game Monkey"
SectionEnd

Section /o "-un.diff" UNSEC_DIFF
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_diff.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_diff.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "diff"
SectionEnd

Section /o "-un.Makefile" UNSEC_MAKE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_make.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_make.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Makefile"
SectionEnd

Section /o "-un.bash script" UNSEC_BASH
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bash.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_bash.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "bash script"
SectionEnd

Section /o "-un.DOS batch files" UNSEC_DOS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_batch.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_batch.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "DOS batch files"
SectionEnd

Section "-un.XML" UNSEC_XML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_xml.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_xml.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "XML"
SectionEnd

Section /o "-un.HTML" UNSEC_HTML
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_html.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_html.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "HTML"
SectionEnd

Section /o "-un.CSS" UNSEC_CSS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_css.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_css.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "CSS"
SectionEnd

Section /o "-un.LaTeX" UNSEC_LATEX
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_latex.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_latex.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "LaTeX"
SectionEnd

Section /o "-un.GLSL (GLslang)" UNSEC_GLSL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_glsl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_glsl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "GLSL (GLslang)"
SectionEnd

Section /o "-un.nVidia Cg" UNSEC_CG
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cg.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_cg.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "nVidia Cg"
SectionEnd

Section /o "-un.Ogre" UNSEC_OGRE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreMaterial.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreMaterial.sample
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreCompositor.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_OgreCompositor.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Ogre"
SectionEnd

Section /o "-un.Hitachi Assembler" UNSEC_HITACHI
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_hitasm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_hitasm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Hitachi Assembler"
SectionEnd

Section /o "-un.VHDL" UNSEC_VHDL
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vhdl.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_vhdl.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "VHDL"
SectionEnd

Section /o "-un.Verilog" UNSEC_VERILOG
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_verilog.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_verilog.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Verilog"
SectionEnd

Section /o "-un.MASM" UNSEC_MASM
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_masm.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_masm.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "MASM"
SectionEnd

Section /o "-un.NSIS installer script" UNSEC_NSIS
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nsis.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_nsis.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "NSIS installer script"
SectionEnd

Section /o "-un.Matlab" UNSEC_MATLAB
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_matlab.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_matlab.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Matlab"
SectionEnd

Section /o "-un.XBase" UNSEC_XBASE
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_prg.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_prg.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "XBase"
SectionEnd

Section /o "-un.Property file" UNSEC_PROP
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_properties.xml
    Delete /REBOOTOK $INSTDIR${CB_LEXERS}\lexer_properties.sample
    DeleteRegValue HKCU "${REGKEY}\Components" "Property file"
SectionEnd

# C::B lexers end

# C::B shortcuts begin

Section /o "-un.Program Shortcut" UNSEC_PROGRAMSHORTCUT
    Delete "$SMPROGRAMS\${CB_SM_GROUP}\$(^Name).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Program Shortcut"
SectionEnd

Section /o "-un.Program Shortcut All Users" UNSEC_PROGRAMSHORTCUT_ALL
    SetShellVarContext all
    Delete "$SMPROGRAMS\${CB_SM_GROUP}\$(^Name).lnk"
    SetShellVarContext current
    DeleteRegValue HKCU "${REGKEY}\Components" "Program Shortcut All Users"
SectionEnd

Section /o "-un.Desktop Shortcut" UNSEC_DESKTOPSHORTCUT
    Delete /REBOOTOK "$DESKTOP\$(^Name).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Desktop Shortcut"
SectionEnd

Section /o "-un.Quick Launch Shortcut" UNSEC_QUICKLAUNCHSHORTCUT
    Delete /REBOOTOK "$QUICKLAUNCH\$(^Name).lnk"
    DeleteRegValue HKCU "${REGKEY}\Components" "Quick Launch Shortcut"
SectionEnd

# C::B shortcuts end

# C::B core begin

Section "-un.Core Files (required)" UNSEC_CORE
    Delete /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMG_SETTINGS}
    Delete /REBOOTOK $INSTDIR${CB_IMG_16}\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMG_16}
    Delete /REBOOTOK $INSTDIR${CB_IMAGES}\*.png
    RMDir  /REBOOTOK $INSTDIR${CB_IMAGES}
    Delete /REBOOTOK $INSTDIR${CB_TEMPLATES}\*.*
    # Just try the following, if it fails that's ok
    # cause the post section will handles this.
    RMDir            $INSTDIR${CB_TEMPLATES}
    Delete /REBOOTOK $INSTDIR${CB_SCRIPTS}\*.script
    # Just try the following, if it fails that's ok
    # cause the post section will handles this.
    RMDir            $INSTDIR${CB_SCRIPTS}
    RMDir            $INSTDIR${CB_LEXERS}
    RMDir            $INSTDIR${CB_PLUGINS}
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\index.ini
    Delete /REBOOTOK $INSTDIR${CB_DOCS}\codeblocks.chm
    RMDir  /REBOOTOK $INSTDIR${CB_DOCS}
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\resources.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\manager_resources.zip
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\tips.txt
    Delete /REBOOTOK $INSTDIR${CB_SHARE_CB}\start_here.zip
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE_CB}
    RMDir  /REBOOTOK $INSTDIR${CB_SHARE}
    Delete /REBOOTOK $INSTDIR\wxscintilla.dll
    Delete /REBOOTOK $INSTDIR\mingwm10.dll
    Delete /REBOOTOK $INSTDIR\exchndl.dll
    Delete /REBOOTOK $INSTDIR\codeblocks.exe
    Delete /REBOOTOK $INSTDIR\codeblocks.dll
    Delete /REBOOTOK $INSTDIR\cb_console_runner.exe
    Delete /REBOOTOK $INSTDIR\wxmsw28u_gcc_custom.dll
    DeleteRegValue HKCU "${REGKEY}\Components" "Core Files (required)"
SectionEnd

# C::B core end

Section -un.post UNSEC_MISC
    Delete "$SMPROGRAMS\${CB_SM_GROUP}\Uninstall $(^Name).lnk"
    RMDir  $SMPROGRAMS\${CB_SM_GROUP}

    DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK $INSTDIR\uninstall.exe

    DeleteRegValue HKCU "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKCU "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKCU "${REGKEY}"

    RMDir /REBOOTOK $INSTDIR

    # If $INSTDIR was already removed, skip these next steps
    IfFileExists $INSTDIR 0 instDirOperated
        # It appears $INSTDIR is not empty, thus it could not be removed.
        # Ask the user to probably force the removal.
        MessageBox MB_YESNO|MB_ICONQUESTION \
            "Remove all files in your Code::Blocks directory?$\r$\n(If you have anything you created that you want to keep, click No.)" \
            IDYES yesRMDir IDNO noRMDir
yesRMDir:
        # Try to delete all remaining files and finally $INSTDIR recursively
        Delete $INSTDIR\*.*
        RMDir /r $INSTDIR
        IfFileExists $INSTDIR 0 instDirOperated
            MessageBox MB_OK|MB_ICONEXCLAMATION \
                "Warning: $INSTDIR could not be removed.$\r$\n(Probably missing access rights?)"
noRMDir:
        # If user selected "no" -> skip the next steps
instDirOperated:
    # fall through
SectionEnd

#######################
# Installer functions #
#######################

Function .onInit
    InitPluginsDir
    Push $R1
    File /oname=$PLUGINSDIR\spltmp.bmp ${CB_SPLASH}
    advsplash::show 1000 600 400 -1 $PLUGINSDIR\spltmp
    Pop $R1
    Pop $R1
FunctionEnd

#########################
# Uninstaller functions #
#########################

Function un.onInit
    ReadRegStr $INSTDIR HKCU "${REGKEY}" Path
    !insertmacro SELECT_UNSECTION "Core Files (required)"      ${UNSEC_CORE}

    !insertmacro SELECT_UNSECTION "Program Shortcut"           ${UNSEC_PROGRAMSHORTCUT}
    !insertmacro SELECT_UNSECTION "Program Shortcut All Users" ${UNSEC_PROGRAMSHORTCUT_ALL}
    !insertmacro SELECT_UNSECTION "Desktop Shortcut"           ${UNSEC_DESKTOPSHORTCUT}
    !insertmacro SELECT_UNSECTION "Quick Launch Shortcut"      ${UNSEC_QUICKLAUNCHSHORTCUT}
                                                             
    !insertmacro SELECT_UNSECTION "C/C++"                      ${UNSEC_CPP}
    !insertmacro SELECT_UNSECTION "The D Language"             ${UNSEC_D}
    !insertmacro SELECT_UNSECTION "Fortran"                    ${UNSEC_F}
    !insertmacro SELECT_UNSECTION "Squirrel"                   ${UNSEC_SQ}
    !insertmacro SELECT_UNSECTION "Angelscript"                ${UNSEC_AS}
    !insertmacro SELECT_UNSECTION "Lua"                        ${UNSEC_LUA}
    !insertmacro SELECT_UNSECTION "Python"                     ${UNSEC_PY}
    !insertmacro SELECT_UNSECTION "Perl"                       ${UNSEC_PERL}
    !insertmacro SELECT_UNSECTION "VB Script"                  ${UNSEC_VB}
    !insertmacro SELECT_UNSECTION "Game Monkey"                ${UNSEC_GM}
    !insertmacro SELECT_UNSECTION "diff"                       ${UNSEC_DIFF}
    !insertmacro SELECT_UNSECTION "makefile"                   ${UNSEC_MAKE}
    !insertmacro SELECT_UNSECTION "bash script"                ${UNSEC_BASH}
    !insertmacro SELECT_UNSECTION "DOS batch files"            ${UNSEC_DOS}
    !insertmacro SELECT_UNSECTION "XML"                        ${UNSEC_XML}
    !insertmacro SELECT_UNSECTION "HTML"                       ${UNSEC_HTML}
    !insertmacro SELECT_UNSECTION "CSS"                        ${UNSEC_CSS}
    !insertmacro SELECT_UNSECTION "LaTeX"                      ${UNSEC_LATEX}
    !insertmacro SELECT_UNSECTION "GLSL (GLSlang)"             ${UNSEC_GLSL}
    !insertmacro SELECT_UNSECTION "nVidia Cg"                  ${UNSEC_CG}
    !insertmacro SELECT_UNSECTION "Ogre"                       ${UNSEC_OGRE}
    !insertmacro SELECT_UNSECTION "Hitachi assembler"          ${UNSEC_HITACHI}
    !insertmacro SELECT_UNSECTION "VHDL"                       ${UNSEC_VHDL}
    !insertmacro SELECT_UNSECTION "Verilog"                    ${UNSEC_VERILOG}
    !insertmacro SELECT_UNSECTION "MASM"                       ${UNSEC_MASM}
    !insertmacro SELECT_UNSECTION "NSIS installer script"      ${UNSEC_NSIS}
    !insertmacro SELECT_UNSECTION "MATLAB"                     ${UNSEC_MATLAB}
    !insertmacro SELECT_UNSECTION "XBASE"                      ${UNSEC_XBASE}
    !insertmacro SELECT_UNSECTION "Property file"              ${UNSEC_PROP}
                                                             
    !insertmacro SELECT_UNSECTION "AStyle plugin"              ${UNSEC_ASTYLE}
    !insertmacro SELECT_UNSECTION "Autosave plugin"            ${UNSEC_AUTOSAVE}
    !insertmacro SELECT_UNSECTION "Class Wizard plugin"        ${UNSEC_CLASSWIZARD}
    !insertmacro SELECT_UNSECTION "Code Completion plugin"     ${UNSEC_CODECOMPLETION}
    !insertmacro SELECT_UNSECTION "Compiler plugin"            ${UNSEC_COMPILER}
    !insertmacro SELECT_UNSECTION "Debugger plugin"            ${UNSEC_DEBUGGER}
    !insertmacro SELECT_UNSECTION "MIME Handler plugin"        ${UNSEC_MIMEHANDLER}
    !insertmacro SELECT_UNSECTION "Open Files List plugin"     ${UNSEC_OPENFILESLIST}
    !insertmacro SELECT_UNSECTION "Projects Importer plugin"   ${UNSEC_PROJECTSIMPORTER}
    !insertmacro SELECT_UNSECTION "Scripted Wizard plugin"     ${UNSEC_SCRIPTEDWIZARD}
    !insertmacro SELECT_UNSECTION "ToDo List plugin"           ${UNSEC_TODOLIST}
    !insertmacro SELECT_UNSECTION "XP Look And Feel plugin"    ${UNSEC_XPLOOKANDFEEL}
                                                             
    !insertmacro SELECT_UNSECTION "Auto Versioning plugin"     ${UNSEC_AUTOVERSIONING}
    !insertmacro SELECT_UNSECTION "Browse Tracker plugin"      ${UNSEC_BROWSETRACKER}
    !insertmacro SELECT_UNSECTION "Byo Games plugin"           ${UNSEC_BYOGAMES}
    !insertmacro SELECT_UNSECTION "Koders plugin"              ${UNSEC_KODERS}
    !insertmacro SELECT_UNSECTION "Code Snippets plugin"       ${UNSEC_CODESNIPPETS}
    !insertmacro SELECT_UNSECTION "Code Stat plugin"           ${UNSEC_CODESTAT}
    !insertmacro SELECT_UNSECTION "Copy Strings plugin"        ${UNSEC_COPYSTRINGS}
    !insertmacro SELECT_UNSECTION "DevPak plugin"              ${UNSEC_DEVPAK}
    !insertmacro SELECT_UNSECTION "Drag Scroll plugin"         ${UNSEC_DRAGSCROLL}
    !insertmacro SELECT_UNSECTION "EnvVars plugin"             ${UNSEC_ENVVARS}
    !insertmacro SELECT_UNSECTION "Help plugin"                ${UNSEC_HELP}
    !insertmacro SELECT_UNSECTION "Key Binder plugin"          ${UNSEC_KEYBINDER}
    !insertmacro SELECT_UNSECTION "Lib Finder plugin"          ${UNSEC_LIBFINDER}
    !insertmacro SELECT_UNSECTION "Profiler plugin"            ${UNSEC_PROFILER}
    !insertmacro SELECT_UNSECTION "RegEx Testbed plugin"       ${UNSEC_REGEXTESTBED}
    !insertmacro SELECT_UNSECTION "Exporter plugin"            ${UNSEC_EXPORTER}
    !insertmacro SELECT_UNSECTION "SymTab plugin"              ${UNSEC_SYMTAB}
    !insertmacro SELECT_UNSECTION "ThreadSearch plugin"        ${UNSEC_THREADSEARCH}
    !insertmacro SELECT_UNSECTION "wxSmith plugin"             ${UNSEC_WXSMITH}
                                                             
    !insertmacro SELECT_UNSECTION "C::B Share Config"          ${UNSEC_SHARECONFIG}
                                                             
!ifdef MINGW_BUNDLE                                          
    !insertmacro SELECT_UNSECTION "MinGW Compiler Suite"       ${UNSEC_MINGW}
!endif
FunctionEnd

########################
# Section Descriptions #
########################

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_DEFAULT}          "The default install which consists of the Code::Blocks core components and the core plugins."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE}                "Code::Blocks core components (without these, Code::Blocks will not work properly)."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_SHORTCUTS}        "Shortcuts to be created."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROGRAMSHORTCUT}     "Creates a shortcut to Code::Blocks in the startmenu."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROGRAMSHORTCUT_ALL} "Creates a shortcut to Code::Blocks in the startmenu for all users (requires admins rights)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DESKTOPSHORTCUT}     "Creates a shortcut to Code::Blocks on the desktop."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_QUICKLAUNCHSHORTCUT} "Creates a shortcut to Code::Blocks in the quick lauch bar."

!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_LEXERS}           "Lexer files provide syntax styling and delimiter matching for different programming languages and others."
                                                            
!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_CORE_PLUGINS}     "Core plugins that are most likely desired. This includes the compiler and debugger plugin."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ASTYLE}              "Source code formatter. Uses AStyle to reformat your sources."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_AUTOSAVE}            "Saves your work in regular intervals."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CLASSWIZARD}         "Provides an easy way to create a new C++ class file pair."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODECOMPLETION}      "Provides a symbols browser for your projects and code-completion inside the editor."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPILER}            "Provides an interface to various compilers, including GNU compiler suite, Microsoft, Borland, etc."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEBUGGER}            "Provides interfaces to the GNU GDB and MS CDB debuggers."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_MIMEHANDLER}         "Provides a (default) files extension handler."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_OPENFILESLIST}       "Shows all currently open files (editors) in a list."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROJECTSIMPORTER}    "Imports foreign projects/workspaces (Dev-C++, MSVC6, MSVS7, MSVS8)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SCRIPTEDWIZARD}      "Provides a generic platform for creating project wizards (already includes a lot of wizards)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_TODOLIST}            "Provides a To-Do list and collects items accoringly from source files of a file/project/workspace."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_XPLOOKANDFEEL}       "Creates a manifest file that makes use of common controls 6.0 under Windows XP."
                                                            
!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP_CONTRIB_PLUGINS}  "Contributed plugins by the Code::Blocks user/developer community. These plugins extend the IDE nicely."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_AUTOVERSIONING}      "Auto increments the version and build number of your application every time a change has been made."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BROWSETRACKER}       "Browse to previous source positions / editors comfortable."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BYOGAMES}            "Provides a collection of games inside C::B for fun."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_KODERS}              "Provides an interface to search for code snippets at the Koders webpage."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODESNIPPETS}        "Allows to create and save small pieces of code (snippets) for later use."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_CODESTAT}            "A plugin for counting code, comments and empty lines of a project."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_COPYSTRINGS}         "Copies all the strings in the current editor into the clipboard."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEVPAK}              "Installs selected DevPaks from the internet."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DRAGSCROLL}          "Mouse drag and scroll using right or middle mouse key."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_ENVVARS}             "Sets up environment variables within the focus of Code::Blocks."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_HELP}                "Add a list of help/MAN files to the help menu so you can have them handy to launch."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_KEYBINDER}           "Provides the user an ability to bind custom key combinations to the menu items."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_LIBFINDER}           "Tool which automatically searches for installed libraries and adds them to global variables and projects."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_PROFILER}            "Provides a simple graphical interface to the GNU GProf profiler."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_REGEXTESTBED}        "Provides a regular expressions testbed."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXPORTER}            "Provides the ability to export syntax highlighted source files to HTML, RTF, ODT or PDF."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SYMTAB}              "Provides a simple graphical interface to the GNU symbol table displayer (nm)."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_THREADSEARCH}        "Multi-threaded 'Search in files' with preview window."
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_WXSMITH}             "RAD tool used to create wxWidgets based GUI applications, forms, dialogs and other."
                                                            
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_SHARECONFIG}         "Allows sharing of most important settings between Code::Blocks instances or different users."
                                                            
!ifdef MINGW_BUNDLE                                         
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_MINGW}               "Additional setup that will install the GNU compiler suite (requires additional downloads)."
!endif
!insertmacro MUI_FUNCTION_DESCRIPTION_END
