; Debugging:
;#define BUILD_TYPE = "64"
;#define NIGHTLY_BUILD_SVN = "12492_PLUS"

;#########################################################
;# Room for adjustments of most important settings BEGIN #
;#########################################################
; This can be done via batch files using command line parameters
; as defined on https://jrsoftware.org/ispphelp/index.php?topic=isppcc

; The following line defined if the build is for 32 or 64 bits
#ifdef BUILD_TYPE
  #if BUILD_TYPE == "32"
    #define BUILD_TYPE = 32
  #else
    #define BUILD_TYPE = 64
  #endif
#else
  #define BUILD_TYPE = 64
#endif

; The following line defined if the build is a nightly build and it"s the SVN number
; if not defined the version will default to YY.MM (year:month)
#ifndef NIGHTLY_BUILD_SVN
  #define NIGHTLY_BUILD_SVN ="12492"
#else
  #if NIGHTLY_BUILD_SVN == "False"
    #undef NIGHTLY_BUILD_SVN
  #endif
#endif

; Possibly required to adjust manually:
; Note: a) These files are only required for the installer.
;       b) These are in the ${CB_INSTALL_GRAPHICS_DIR} directory
#ifdef NIGHTLY_BUILD_SVN
    #define CB_SPLASH_FILENAME  ="setup_splash_nightly.bmp"
#else
    #define CB_SPLASH_FILENAME  ="setup_splash_2003.bmp"
#endif
#define CB_LOGO_FILENAME        ="setup_logo_2003.bmp"

;#############################################################

#define CB_SrcDir           = SourcePath+"\.."
#if BUILD_TYPE == 32
  #define CB_BuildOutputDir = SourcePath+"\..\src\output31"
#else
  #define CB_BuildOutputDir = SourcePath+"\..\src\output31_64"
#endif
#define CB_PROGRAMDIRNAME   = "CodeBlocks"
#define CURRENT_DATE        = GetDateTimeString("ddmmmyyyy", "", "")
#define CURRENT_DATE_YEAR   = GetDateTimeString("yyyy", "", "")
#define CURRENT_DATE_MONTH  = GetDateTimeString("mm", "", "")
#define CURRENT_DATE_DAY    = GetDateTimeString("dd", "", "")

#define DICTIONARIES_DIR    = SourcePath+"\Dictionaries"
#define DOCUMENTATION_DIR   = SourcePath+"Documentation"
#define GRAPHICS_DIR        = SourcePath+"Graphics"
#define LICENSES_DIR        = SourcePath+"Licenses"

#ifdef NIGHTLY_BUILD_SVN
    #define CB_VERSION    = "SVN-" + Str(NIGHTLY_BUILD_SVN)
#else
    #define CB_VERSION    = CURRENT_DATE_YEAR+"."+CURRENT_DATE_MONTH
#endif


[Setup]
SetupLogging=yes
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{53BB99B2-5263-43B9-A46A-C92E63AAA96F}
AppName=Code::Blocks
AppVerName=Code::Blocks {#CB_VERSION}
AppPublisher=Code::Blocks
AppPublisherURL=www.codeblocks.org
AppVersion={#CB_VERSION}-{#CURRENT_DATE_YEAR}.{#CURRENT_DATE_MONTH}.{#CURRENT_DATE_DAY}.0
AppSupportURL=www.codeblocks.org
AppUpdatesURL=www.codeblocks.org
#if BUILD_TYPE == 64
  ; "ArchitecturesAllowed=x64" specifies that Setup cannot run on anything but x64.
  ArchitecturesAllowed=x64
  ; "ArchitecturesInstallIn64BitMode=x64" requests that the install be done in "64-bit mode" on x64, meaning it should use the native 64-bit Program Files directory and the 64-bit view of the registry.
  ArchitecturesInstallIn64BitMode=x64
#endif
Compression=lzma/ultra
DefaultDirName={autopf}\{#CB_PROGRAMDIRNAME}
DefaultGroupName={#CB_PROGRAMDIRNAME}
DisableWelcomePage=False
InternalCompressLevel=ultra
InfoBeforeFile=
LicenseFile={#LICENSES_DIR}\gpl-3.0.txt
#if BUILD_TYPE == 32
OutputBaseFilename=CodeBlocks-{#CB_VERSION}-32bit-setup-{#CURRENT_DATE}-ISS-FULLONLY
#else
OutputBaseFilename=CodeBlocks-{#CB_VERSION}-64bit-setup-{#CURRENT_DATE}-ISS-FULLONLY
#endif
OutputDir={#SourcePath}
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
SetupIconFile={#GRAPHICS_DIR}\setup_icon.ico
ShowLanguageDialog=no
SolidCompression=true
;VersionInfoVersion={#CB_VERSION}
SourceDir={#CB_BuildOutputDir}
UninstallDisplayIcon={app}\codeblocks.exe
UninstallDisplayName=Code::Blocks
VersionInfoCompany=Code::Blocks
VersionInfoCopyright=(C) Code::Blocks
VersionInfoDescription=Code::Blocks IDE
VersionInfoProductName=Code::Blocks IDE
VersionInfoProductVersion={#CURRENT_DATE_YEAR}.{#CURRENT_DATE_MONTH}.{#CURRENT_DATE_DAY}.0
VersionInfoProductTextVersion={#CB_VERSION}-{#CURRENT_DATE_YEAR}.{#CURRENT_DATE_MONTH}.{#CURRENT_DATE_DAY}.0
WizardImageFile={#GRAPHICS_DIR}\setup_1.bmp
WizardImageStretch=yes
WizardSmallImageFile={#GRAPHICS_DIR}\{#CB_LOGO_FILENAME}
WizardStyle=modern
ShowTasksTreeLines=True
AlwaysShowGroupOnReadyPage=True
AlwaysShowDirOnReadyPage=True
UsePreviousPrivileges=False
UsePreviousSetupType=False
UsePreviousTasks=False
UsePreviousLanguage=False

[Tasks]
Name: startmenu;          Description: "Create a &startmenu entry";   GroupDescription: "Additional icons:";
Name: desktopicon;        Description: "Create a &desktop icon";      GroupDescription: "Additional icons:";
Name: desktopicon\common; Description: "For all users";               GroupDescription: "Additional icons:"; Check: IsAdminInstallMode
Name: desktopicon\user;   Description: "For the current user only";   GroupDescription: "Additional icons:";
Name: quicklaunchicon;          Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:";
Name: quicklaunchicon\common;   Description: "For all users";               GroupDescription: "Additional icons:"; Check: IsAdminInstallMode
Name: quicklaunchicon\user;     Description: "For the current user only";   GroupDescription: "Additional icons:";

[Files]
Source: "*"; Excludes: "*.a,cctest.exe,wxmsw315ud*gcc_cb.dll";  DestDir: "{app}";                       Flags: ignoreversion createallsubdirs recursesubdirs;
Source: "{#DICTIONARIES_DIR}\*";                                DestDir: "{app}\share\CodeBlocks\SpellChecker"; Flags: ignoreversion createallsubdirs recursesubdirs;
Source: "{#DOCUMENTATION_DIR}\manual_codeblocks_en.chm";        DestDir: "{app}\share\CodeBlocks\docs"; Flags: ignoreversion;
Source: "{#DOCUMENTATION_DIR}\manual_codeblocks_en.pdf";        DestDir: "{app}\share\CodeBlocks\docs"; Flags: ignoreversion;
Source: "{#DOCUMENTATION_DIR}\manual_codeblocks_fr.chm";        DestDir: "{app}\share\CodeBlocks\docs"; Flags: ignoreversion;
Source: "{#DOCUMENTATION_DIR}\manual_codeblocks_fr.pdf";        DestDir: "{app}\share\CodeBlocks\docs"; Flags: ignoreversion;
Source: "{#DOCUMENTATION_DIR}\Manual_wxPBGuide.pdf";            DestDir: "{app}\share\CodeBlocks\docs"; Flags: ignoreversion;
Source: "{#LICENSES_DIR}\gpl-3.0.txt";                          DestDir: "{app}";                       Flags: ignoreversion;
Source: "{#LICENSES_DIR}\lgpl-3.0.txt";                         DestDir: "{app}";                       Flags: ignoreversion;

[Icons]
Name: "{group}\{#CB_PROGRAMDIRNAME}";               Filename: "{app}\codeblocks.exe"; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE;  Tasks: startmenu;
Name: "{group}\{cm:UninstallProgram, CodeBlocks}";  Filename: "{uninstallexe}"; Tasks: startmenu;
Name: "{commondesktop}\{#CB_PROGRAMDIRNAME}"; Filename: "{app}\codeblocks.exe";   IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: desktopicon\common;  Check: IsAdminInstallMode
Name: "{userdesktop}\{#CB_PROGRAMDIRNAME}";   Filename: "{app}\codeblocks.exe";   IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: desktopicon\user
Name: "{autoappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}";   Filename: "{app}\codeblocks.exe"; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: quicklaunchicon\common or quicklaunchicon\user;  Check: IsAdminInstallMode
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}";   Filename: "{app}\codeblocks.exe"; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: quicklaunchicon\user

Name: "{group}\CodeBlocks Share Config.lnk";         Filename: "{app}\cb_share_config.exe"; IconIndex: 0; WorkingDir: {app}; Tasks: startmenu;
Name: "{group}\CodeBlocks Address to Line GUI.lnk";  Filename: "{app}\Addr2LineUI.exe";     IconIndex: 0; WorkingDir: {app}; Tasks: startmenu;
Name: "{group}\CodeBlocks CBP2Make.lnk";             Filename: "{app}\cbp2make.exe";        IconIndex: 0; WorkingDir: {app}; Tasks: startmenu;
Name: "{group}\CodeBlocks Share Config.lnk";         Filename: "{app}\cb_share_config.exe"; IconIndex: 0; WorkingDir: {app}; Tasks: startmenu;
Name: "{group}\CodeBlocks Launcher.lnk";             Filename: "{app}\CbLauncher.exe";      IconIndex: 0; WorkingDir: {app}; Tasks: startmenu;

Name: "{group}\Documentation\CodeBlocks License";               Filename: "{app}\gpl-3.0.txt";                                   Comment: "Code::Blocks license"; Tasks: startmenu;
Name: "{group}\Documentation\CodeBlocks SDK License";           Filename: "{app}\lgpl-3.0.txt";                                  Comment: "Code::Blocks SDK license"; Tasks: startmenu;
Name: "{group}\Documentation\CodeBlocks PDF Manual English";    Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_en.pdf";Comment: "The Code::Blocks PDF User Manual in English"; Tasks: startmenu;
Name: "{group}\Documentation\CodeBlocks CHM Manual English";    Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_en.chm";Comment: "The Code::Blocks CHM User Manual in English"; Tasks: startmenu;
Name: "{group}\Documentation\CodeBlocks PDF Manual French";     Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_fr.pdf";Comment: "The Code::Blocks PDF User Manual in French"; Tasks: startmenu;
Name: "{group}\Documentation\CodeBlocks CHM Manual French";     Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_fr.chm";Comment: "The Code::Blocks CHM User Manual in French"; Tasks: startmenu;
Name: "{group}\Documentation\PBs wxWidgets Guide";              Filename: "{app}\share\CodeBlocks\docs\Manual_wxPBGuide.pdf";    Comment: "PBs GuiDe to Starting with wxWidgets with MinGW and Code::Blocks"; Tasks: startmenu;
Name: "{group}\Documentation\Codeblocks beginner instructions"; Filename: "http://www.sci.brooklyn.cuny.edu/~goetz/codeblocks/codeblocks-instructions.pdf";  Comment: "Code::Blocks beginner install and user guide"; Tasks: startmenu;

Name: "{group}\CodeBlocks Web Site";                      Filename: "http://www.codeblocks.org";                      Comment: "Open Code::Blocks IDE website"; Tasks: startmenu;
Name: "{group}\WebSite\CodeBlocks Forums";                Filename: "http://forums.codeblocks.org";                   Comment: "Open Code::Blocks IDE discussion forums"; Tasks: startmenu;
Name: "{group}\WebSite\CodeBlocks WiKi";                  Filename: "http://wiki.codeblocks.org";                     Comment: "Open Code::Blocks IDE WiKi site"; Tasks: startmenu;
Name: "{group}\WebSite\CodeBlocks Tickets";               Filename: "https://sourceforge.net/p/codeblocks/tickets/";  Comment: "Report bugs/enhancements for Code::Blocks"; Tasks: startmenu;

[Run]
Filename: "{app}\codeblocks.exe";                                 Description: "Launch Code::Blocks";                         Flags: nowait postinstall skipifsilent runasoriginaluser;
Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_en.pdf"; Description: "The Code::Blocks PDF User Manual in English"; Flags: nowait postinstall skipifsilent  shellexec runasoriginaluser unchecked; 
Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_en.chm"; Description: "The Code::Blocks CHM User Manual in English"; Flags: nowait postinstall skipifsilent  shellexec runasoriginaluser unchecked;
Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_fr.pdf"; Description: "The Code::Blocks PDF User Manual in French";  Flags: nowait postinstall skipifsilent  shellexec runasoriginaluser unchecked;
Filename: "{app}\share\CodeBlocks\docs\manual_codeblocks_fr.chm"; Description: "The Code::Blocks CHM User Manual in French";  Flags: nowait postinstall skipifsilent  shellexec runasoriginaluser unchecked;
Filename: "http://www.sci.brooklyn.cuny.edu/~goetz/codeblocks/codeblocks-instructions.pdf"; Description: "Code::Blocks beginner install and user guide"; Flags: nowait postinstall skipifsilent  shellexec runasoriginaluser unchecked;

[UninstallDelete]
Type: filesandordirs; Name: "{app}";
Type: files;          Name: "{commondesktop}\{#CB_PROGRAMDIRNAME}"; Check: IsAdminInstallMode
Type: files;          Name: "{userdesktop}\{#CB_PROGRAMDIRNAME}";
Type: files;          Name: "{autoappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}"; Check: IsAdminInstallMode
Type: files;          Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}";


[Code]
// ================================================================================================================================================================

{ ///////////////////////////////////////////////////////////////////// }
// Uninstall
procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = wpSelectTasks then
  begin
    if IsAdminInstallMode then
      begin
        WizardForm.TasksList.Checked[2] := False;   // desktopicon
        WizardForm.TasksList.Checked[3] := True;    // desktopicon\common
        WizardForm.TasksList.Checked[4] := False;   // desktopicon\user

        WizardForm.TasksList.Checked[5] := False;   // quicklaunchicon
        WizardForm.TasksList.Checked[6] := True;    // quicklaunchicon\common
        WizardForm.TasksList.Checked[7] := False;   // quicklaunchicon\user
      end
    else
      begin
        WizardForm.TasksList.Checked[2] := True;    // desktopicon
        // NOT SHOWN WizardForm.TasksList.Checked[3] := False;   // desktopicon\common
        WizardForm.TasksList.Checked[3] := True;    // desktopicon\user

        WizardForm.TasksList.Checked[4] := True;    // quicklaunchicon
        // NOT SHOWN WizardForm.TasksList.Checked[6] := False;   // quicklaunchicon\common
        WizardForm.TasksList.Checked[5] := True;    // quicklaunchicon\user
      end
  end;
end;
{ ///////////////////////////////////////////////////////////////////// }
// Uninstall
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  Subkeys: TArrayOfString;
  Subkey: string;
  I: Integer;
  SubkeyExtension: string;
  SubkeyExtensionLength: Integer;
  SubkeyExtensionDefault: string;
begin
  case CurUninstallStep of
    usUninstall:
      begin
        //RegDeleteKeyIncludingSubkeys(HKCR, '*\shell\Open With CodeBlocks');  // Taken from https://github.com/eranif/codelite/blob/master/InnoSetup/codelite64_mingw.iss

#if BUILD_TYPE == 64
        if RegGetSubkeyNames(HKCU64, 'SOFTWARE\Classes', Subkeys) then 
        begin
          for I := 0 to GetArrayLength(Subkeys) - 1 do
          begin
            Subkey := Subkeys[I];
            if WildcardMatch(Subkey, 'CodeBlocks.*') then
            begin
              SubkeyExtensionLength := Length(Subkey) - 10;
              SubkeyExtension := Copy(Subkey, 11, SubkeyExtensionLength);
              if RegKeyExists(HKCU64, Format('SOFTWARE\Classes\%s',[SubkeyExtension])) then
              begin
                if RegQueryStringValue(HKCU64, Format('SOFTWARE\Classes\%s',[SubkeyExtension]), '', SubkeyExtensionDefault) then
                begin
                  if WildcardMatch(SubkeyExtensionDefault, Subkey) then
                  begin
                      Log(Format('RegDeleteValue(HKCU64, "SOFTWARE\Classes\%s"',[SubkeyExtension]));
                      Log(Format('RegDeleteKeyIfEmpty(HKCU64, "SOFTWARE\Classes\%s"',[SubkeyExtension]));
                      RegDeleteValue(HKCU64, Format('SOFTWARE\Classes\%s', [SubkeyExtension]),'');
                      RegDeleteKeyIfEmpty(HKCU64, Format('SOFTWARE\Classes\%s', [SubkeyExtension]));
                  end;
                end;
              end;
              Log(Format('RegDeleteKeyIncludingSubkeys(HKCU64, "SOFTWARE\Classes\%s"',[Subkey]));
              RegDeleteKeyIncludingSubkeys(HKCU64, Format('SOFTWARE\Classes\%s',[Subkey]))
            end;
          end;
        end;
#endif

        if RegGetSubkeyNames(HKCU32, 'SOFTWARE\Classes', Subkeys) then 
        begin
          for I := 0 to GetArrayLength(Subkeys) - 1 do
          begin
            Subkey := Subkeys[I];
            if WildcardMatch(Subkey, 'CodeBlocks.*') then
            begin
              SubkeyExtensionLength := Length(Subkey) - 10;
              SubkeyExtension := Copy(Subkey, 11, SubkeyExtensionLength);
              if RegKeyExists(HKCU32, Format('SOFTWARE\Classes\%s',[SubkeyExtension])) then
              begin
                if RegQueryStringValue(HKCU32, Format('SOFTWARE\Classes\%s',[SubkeyExtension]), '', SubkeyExtensionDefault) then
                begin
                  if WildcardMatch(SubkeyExtensionDefault, Subkey) then
                  begin
                      Log(Format('RegDeleteValue(HKCU32, "SOFTWARE\Classes\%s"',[SubkeyExtension]));
                      Log(Format('RegDeleteKeyIfEmpty(HKCU32, "SOFTWARE\Classes\%s"',[SubkeyExtension]));
                      RegDeleteValue(HKCU32, Format('SOFTWARE\Classes\%s', [SubkeyExtension]),'');
                      RegDeleteKeyIfEmpty(HKCU32, Format('SOFTWARE\Classes\%s', [SubkeyExtension]));
                  end;
                end;
              end;
              Log(Format('RegDeleteKeyIncludingSubkeys(HKCU32, "SOFTWARE\Classes\%s"',[Subkey]));
              RegDeleteKeyIncludingSubkeys(HKCU32, Format('SOFTWARE\Classes\%s',[Subkey]))
            end;
          end;
        end;

        RegDeleteKeyIncludingSubkeys(HKCR, 'Software\Classes\CodeBlocks');


        // Computer\HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\ApplicationAssociationToasts
        
        // Check if user codeblocks app data directory exists
        if DirExists(ExpandConstant('{userappdata}\CodeBlocks'))
        then begin
          // Prompt the user to delete all his settings, default to "No"
          if MsgBox(ExpandConstant('Do you want to delete the existing configuration files and directory? ({userappdata}\codeblocks)?'), mbConfirmation, MB_YESNO or MB_DEFBUTTON1) = IDYES 
          then begin
              DelTree(ExpandConstant('{userappdata}\CodeBlocks'), True, True, True);
          end;
        end;
      end;
    usPostUninstall:
      begin
      end;
  end;
end;

// ================================================================================================================================================================
var
  DownloadPage: TDownloadWizardPage;
  CompilerSelectionPage: TInputOptionWizardPage;

function OnDownloadProgress(const Url, FileName: String; const Progress, ProgressMax: Int64): Boolean;
begin
  if Progress = ProgressMax then
    Log(Format('Successfully downloaded file to {tmp}: %s', [FileName]));
  Result := True;
end;

function CompilerInstallerDownloadRun(const Url, FileName: String): Boolean;
var
  ResultCode: Integer;
  TmpFileName: String;
begin
    DownloadPage.Clear;
    DownloadPage.Add(Url, FileName, '');
    DownloadPage.Show;
    try
      try
        DownloadPage.Download; // This downloads the files to {tmp}
        TmpFileName := Format('%s\%s', [ExpandConstant('{tmp}'), FileName]);
        Result := Exec(TmpFileName, '', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
        if not Result 
          then begin
            MsgBox(Format('Cannot execute %s', [FileName]), mbError, MB_OK);
          end
        else
          begin
            Result := (ResultCode = 0);
            if not Result then
            begin
              MsgBox(Format('Cannot execute %s', [FileName]), mbError, MB_OK);
            end
        end;
      except
        if DownloadPage.AbortedByUser then
          Log('Aborted by user.')
        else
          SuppressibleMsgBox(AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
        Result := False;
      end;
    finally
      DownloadPage.Hide;
    end;
end;

function CompilerInstallerDownloadRun_MinGW(): Boolean;
begin
  Result := CompilerInstallerDownloadRun('https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe', 'mingw-w64-install.exe');
end;

function CompilerInstallerDownloadRun_TDM(): Boolean;
begin
  Result := CompilerInstallerDownloadRun('https://github.com/jmeubank/tdm-gcc/releases/download/v1.2105.1/tdm-gcc-webdl.exe', 'tdm-gcc-webdl.exe');
end;

function CompilerInstallerDownloadRun_MSYS2(): Boolean;
begin
  Result := CompilerInstallerDownloadRun('https://sourceforge.net/projects/msys2/files/Base/msys2-x86_64-latest.exe/', 'msys2-x86_64-latest.exe');
end;

function CompilerInstallerDownloadRun_Cygwin(): Boolean;
begin
  Result := CompilerInstallerDownloadRun('https://www.cygwin.com/setup-x86_64.exe', 'cygwin_setup-x86_64.exe');
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  ResultTmp : Boolean;
  ErrorCode: Integer;
begin
  Result := True
  ResultTmp := True

  if CurPageID = CompilerSelectionPage.ID then
  begin
    if CompilerSelectionPage.Values[0] = True then
      ResultTmp  := CompilerInstallerDownloadRun_MinGW();
    if ResultTmp = False then
      Result := False;

    if CompilerSelectionPage.Values[1] = True then
      ShellExecAsOriginalUser('', 'https://github.com/ssbssa/gdb/releases', '', '', SW_SHOW, ewNoWait, ErrorCode);

    if CompilerSelectionPage.Values[2] = True then
      ResultTmp  := CompilerInstallerDownloadRun_TDM();
    if ResultTmp = False then
      Result := False;

    if CompilerSelectionPage.Values[3] = True then
      ResultTmp  := CompilerInstallerDownloadRun_MSYS2();
    if ResultTmp = False then
      Result := False;

    if CompilerSelectionPage.Values[4] = True then
      ResultTmp  := CompilerInstallerDownloadRun_Cygwin();
    if ResultTmp = False then
      Result := False;

  end else
    Result := True;
end;

// ================================================================================================================================================================

procedure InitializeWizard();
begin
    DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), @OnDownloadProgress);

    CompilerSelectionPage := CreateInputOptionPage(wpInstalling ,                // AfterID use wpWelcome when testing.
                                'Compiler Installer Download and Run',          // ACaption
                                '',                                             // ADescription
                                'Please select compiler(s) installer to download and run. Once completed click Next.',  // ASubCaption
                                False,                                          // Exclusive
                                False);                                         // ListBox
    CompilerSelectionPage.Add('MinGW-W64 - supports 32 or 64 bit');
    CompilerSelectionPage.AddEx('Open GDB web download page so you can upgrade GDB to resolve MinGW GDB issues.',1, False);
    CompilerSelectionPage.Add('TDM GCC - supports 32 or 32 & 64 bit');
    CompilerSelectionPage.Add('MSYS2 - supports 32 and 64 bit');
    CompilerSelectionPage.Add('Cygwin - 64 bit only');
    
    // Set initial values (optional)
    CompilerSelectionPage.Values[0] := False;
    CompilerSelectionPage.Values[1] := False;
    CompilerSelectionPage.Values[2] := False;
    CompilerSelectionPage.Values[3] := False;
    CompilerSelectionPage.Values[4] := False;

end;

// ================================================================================================================================================================
