/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

//(*InternalHeaders(Execution)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/dialog.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/utils.h>
#include <wx/window.h>

#include <cbproject.h>
#include <editormanager.h>
#include <globals.h>
#include <manager.h>
#include <logmanager.h>
#include <projectfile.h>
#include <projectmanager.h>

#include "execution.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

//(*IdInit(Execution)
const long Execution::ID_RB_SCOPE = wxNewId();
const long Execution::ID_RB_OPTIONS = wxNewId();
const long Execution::ID_CHK_DEBUG_LOG = wxNewId();
const long Execution::ID_CHK_OBSOLETE_LOG = wxNewId();
const long Execution::ID_CHK_FWD_DECL = wxNewId();
const long Execution::ID_RDO_FILE_TYPE = wxNewId();
const long Execution::ID_CHK_SIMULATION = wxNewId();
const long Execution::ID_LST_SETS = wxNewId();
const long Execution::ID_BTN_SELECT_ALL = wxNewId();
const long Execution::ID_BTN_SELECT_NONE = wxNewId();
const long Execution::ID_BTN_INVERT = wxNewId();
const long Execution::ID_GAU_PROGRESS = wxNewId();
const long Execution::ID_BTN_RUN = wxNewId();
const long Execution::ID_BTN_EXIT = wxNewId();
//*)

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(Execution,wxDialog)
  //(*EventTable(Execution)
  //*)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Execution::Execution(wxWindow* parent,wxWindowID id)
{
  //(*Initialize(Execution)
  Create(parent, id, _("Header Fixup"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
  flsMain = new wxFlexGridSizer(0, 2, 0, 0);
  sizLeft = new wxBoxSizer(wxVERTICAL);
  wxString __wxRadioBoxChoices_1[2] =
  {
  	_("Scan source files in project"),
  	_("Scan source files in workspace")
  };
  m_Scope = new wxRadioBox(this, ID_RB_SCOPE, _("Scope"), wxDefaultPosition, wxSize(337,70), 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RB_SCOPE"));
  m_Scope->SetSelection(0);
  m_Scope->SetToolTip(_("This will setup on what files to operate: All from active project or whole workspace."));
  sizLeft->Add(m_Scope, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  wxString __wxRadioBoxChoices_2[2] =
  {
  	_("Use \"include.h\" (quotation marks)"),
  	_("Use <include.h> (brackets)")
  };
  m_Options = new wxRadioBox(this, ID_RB_OPTIONS, _("Options"), wxDefaultPosition, wxSize(337,70), 2, __wxRadioBoxChoices_2, 2, 0, wxDefaultValidator, _T("ID_RB_OPTIONS"));
  m_Options->SetSelection(1);
  m_Options->SetToolTip(_("This will setup the way missing header files are included: By quotation mark or brackets."));
  sizLeft->Add(m_Options, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizAdvancedOptions = new wxStaticBoxSizer(wxVERTICAL, this, _("Advanced options"));
  m_DebugLog = new wxCheckBox(this, ID_CHK_DEBUG_LOG, _("Full log of parser operations to debug log"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_DEBUG_LOG"));
  m_DebugLog->SetValue(false);
  m_DebugLog->SetToolTip(_("This will print *all* interesting parser operations to the C::B debug log."));
  sizAdvancedOptions->Add(m_DebugLog, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_ObsoleteLog = new wxCheckBox(this, ID_CHK_OBSOLETE_LOG, _("Show includes not required for known bindings"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OBSOLETE_LOG"));
  m_ObsoleteLog->SetValue(false);
  m_ObsoleteLog->SetToolTip(_("This will show a list of included files found with unknown bindings which could possibly be removed in the debug log."));
  sizAdvancedOptions->Add(m_ObsoleteLog, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_FwdDecl = new wxCheckBox(this, ID_CHK_FWD_DECL, _("Try to use forward declarations in headers"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_FWD_DECL"));
  m_FwdDecl->SetValue(false);
  m_FwdDecl->SetToolTip(_("This will setup if forward declarations shall be used for objects apearing *only* as pointers/references in header files."));
  sizAdvancedOptions->Add(m_FwdDecl, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  wxString __wxRadioBoxChoices_3[3] =
  {
  	_("Process header files only"),
  	_("Process implementation files only"),
  	_("Process both files (2-step-fix up)")
  };
  m_FileType = new wxRadioBox(this, ID_RDO_FILE_TYPE, _("File types to process:"), wxDefaultPosition, wxSize(317,95), 3, __wxRadioBoxChoices_3, 3, 0, wxDefaultValidator, _T("ID_RDO_FILE_TYPE"));
  m_FileType->SetSelection(2);
  m_FileType->SetToolTip(_("This will setup on what file types the plugin shall operate: Header files, implementation files or both (after each other)."));
  sizAdvancedOptions->Add(m_FileType, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_Simulation = new wxCheckBox(this, ID_CHK_SIMULATION, _("Only simulate (do *not* change any files)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_SIMULATION"));
  m_Simulation->SetValue(false);
  m_Simulation->SetToolTip(_("This will setup if all operation shall be simulated only - interesting in combination with full log to analyse a project without modifications."));
  sizAdvancedOptions->Add(m_Simulation, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  sizLeft->Add(sizAdvancedOptions, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  flsMain->Add(sizLeft, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizRight = new wxBoxSizer(wxVERTICAL);
  sizHeaderSets = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Header sets"));
  m_Sets = new wxCheckListBox(this, ID_LST_SETS, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LST_SETS"));
  sizHeaderSets->Add(m_Sets, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizAllNoneInvert = new wxBoxSizer(wxVERTICAL);
  m_SelectAll = new wxButton(this, ID_BTN_SELECT_ALL, _("All"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_SELECT_ALL"));
  sizAllNoneInvert->Add(m_SelectAll, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_SelectNone = new wxButton(this, ID_BTN_SELECT_NONE, _("None"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_SELECT_NONE"));
  sizAllNoneInvert->Add(m_SelectNone, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_Invert = new wxButton(this, ID_BTN_INVERT, _("Invert"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_INVERT"));
  sizAllNoneInvert->Add(m_Invert, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizHeaderSets->Add(sizAllNoneInvert, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
  sizRight->Add(sizHeaderSets, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizExecute = new wxStaticBoxSizer(wxVERTICAL, this, _("Execute"));
  m_Progress = new wxGauge(this, ID_GAU_PROGRESS, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAU_PROGRESS"));
  sizExecute->Add(m_Progress, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizRunExit = new wxBoxSizer(wxHORIZONTAL);
  m_Run = new wxButton(this, ID_BTN_RUN, _("Run"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_RUN"));
  m_Run->SetToolTip(_("Run the fixup plugin and begin parsing..."));
  sizRunExit->Add(m_Run, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_Exit = new wxButton(this, ID_BTN_EXIT, _("Exit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_EXIT"));
  m_Exit->SetToolTip(_("Exit the plugin without (further) modifications."));
  sizRunExit->Add(m_Exit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizExecute->Add(sizRunExit, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizRight->Add(sizExecute, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  flsMain->Add(sizRight, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  SetSizer(flsMain);
  flsMain->Fit(this);
  flsMain->SetSizeHints(this);
  Center();

  Connect(ID_BTN_SELECT_ALL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Execution::OnBtnSelectAllClick);
  Connect(ID_BTN_SELECT_NONE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Execution::OnBtnSelectNoneClick);
  Connect(ID_BTN_INVERT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Execution::OnBtnInvertClick);
  Connect(ID_BTN_RUN,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Execution::OnBtnRunClick);
  Connect(ID_BTN_EXIT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Execution::OnBtnExitClick);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&Execution::OnClose);
  //*)

  wxArrayString Groups = m_Bindings.GetGroups();
  for ( size_t i=0; i<Groups.GetCount(); i++ )
    m_Sets->Check(m_Sets->Append(Groups[i]),true);
  m_Execute = false;
}// Execution

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Execution::~Execution()
{
  //(*Destroy(Execution)
  //*)
}// ~Execution

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnBtnExitClick(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}// OnBtnExitClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnBtnSelectAllClick(wxCommandEvent& event)
{
  for ( size_t i=0; i<m_Sets->GetCount(); i++ )
    m_Sets->Check(i,true);
}// OnBtnSelectAllClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnBtnSelectNoneClick(wxCommandEvent& event)
{
  for ( size_t i=0; i<m_Sets->GetCount(); i++ )
    m_Sets->Check(i,false);
}// OnBtnSelectNoneClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnBtnInvertClick(wxCommandEvent& event)
{
  for ( size_t i=0; i<m_Sets->GetCount(); i++ )
    m_Sets->Check(i,!m_Sets->IsChecked(i));
}// OnBtnInvertClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnClose(wxCloseEvent& event)
{
  m_Execute = false;
  event.Skip();
}// OnClose

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OnBtnRunClick(wxCommandEvent& event)
{
  ToggleControls(true);

  ProjectsArray* Projects = Manager::Get()->GetProjectManager()->GetProjects();
  if ( !Projects->GetCount() )
  {
    cbMessageBox(_("No active project(s) to process."),_T("Header Fixup"));
    ToggleControls(false);
    return;
  }

  // Generating list of files to process
  wxArrayString FilesToProcess;

  if ( m_Scope->GetSelection()==0 ) // project scope
  {
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    AddFilesFromProject(FilesToProcess,Project);
  }
  else                              // workspace scope
  {
    ProjectsArray* Projects = Manager::Get()->GetProjectManager()->GetProjects();
    for ( size_t i=0; i<Projects->GetCount(); i++ )
      AddFilesFromProject(FilesToProcess,(*Projects)[i]);
  }

  if ( FilesToProcess.IsEmpty() )
  {
    cbMessageBox(_("No files to process."),_T("Header Fixup"));
    ToggleControls(false);
    return;
  }

  // Generating list of header groups to use
  wxArrayString Groups;
  for ( size_t i=0; i<m_Sets->GetCount(); i++ )
    if ( m_Sets->IsChecked(i) ) Groups.Add(m_Sets->GetString(i));

  if ( Groups.IsEmpty() )
  {
    cbMessageBox(_("Please select at least one header group."),_T("Header Fixup"));
    ToggleControls(false);
    return;
  }

  // Running the scan
  int HeadersAdded = 0;
  if      ( m_FileType->GetSelection()==0 )
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Processing header files...")));
    m_Processor   = ProcessHeaderFiles;
    HeadersAdded += RunScan(FilesToProcess,Groups);
  }
  else if ( m_FileType->GetSelection()==1 )
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Processing source files...")));
    m_Processor   = ProcessSourceFiles;
    HeadersAdded += RunScan(FilesToProcess,Groups);
  }
  else
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Processing header files...")));
    m_Processor   = ProcessHeaderFiles;
    HeadersAdded += RunScan(FilesToProcess,Groups);
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Processing source files...")));
    m_Processor   = ProcessSourceFiles;
    HeadersAdded += RunScan(FilesToProcess,Groups);
  }

  if ( HeadersAdded )
    cbMessageBox(wxString::Format(_("Added %d extra includes."),HeadersAdded),_T("Header Fixup"));
  else
    cbMessageBox(_("All files were OK."),_T("Header Fixup"));

  EndModal(0);
}// OnBtnRunClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::ToggleControls(bool Disable)
{
  if ( Disable )
  {
    m_Options->Disable();
    m_Scope->Disable();

    m_DebugLog->Disable();
    m_ObsoleteLog->Disable();
    m_FwdDecl->Disable();
    m_FileType->Disable();
    m_Simulation->Disable();

    m_Sets->Disable();
    m_SelectAll->Disable();
    m_SelectNone->Disable();
    m_Invert->Disable();
  }
  else
  {
    m_Options->Enable();
    m_Scope->Enable();

    m_DebugLog->Enable();
    m_ObsoleteLog->Enable();
    m_FwdDecl->Enable();
    m_FileType->Enable();
    m_Simulation->Enable();

    m_Sets->Enable();
    m_SelectAll->Enable();
    m_SelectNone->Enable();
    m_Invert->Enable();
  }
}// ToggleControls

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int Execution::RunScan(const wxArrayString& FilesToProcess,
                       const wxArrayString& Groups)
{
  // Running the scan
  m_Execute = true;
  int HeadersAdded = 0;
  m_Progress->SetRange((int)FilesToProcess.GetCount());
  for ( size_t i=0; i<FilesToProcess.GetCount(); i++ )
  {
    m_Progress->SetValue((int)i);
    if ( !m_Execute )
      return HeadersAdded;
    HeadersAdded += ProcessFile(FilesToProcess[i],Groups);
  }
  m_Progress->SetValue((int)FilesToProcess.GetCount());
  m_Execute = false;

  return HeadersAdded;
}// RunScan

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::AddFilesFromProject(wxArrayString& Files,cbProject* Project)
{
  if (!Project)
    return;

  for ( int i=0; i<Project->GetFilesCount(); i++ )
  {
    wxFileName Name = Project->GetFile(i)->file;
    if ( Name.GetExt().Lower() == _T("c")   ||
         Name.GetExt().Lower() == _T("cc")  ||
         Name.GetExt().Lower() == _T("cpp") ||
         Name.GetExt().Lower() == _T("c++") ||
         Name.GetExt().Lower() == _T("cxx") ||

         Name.GetExt().Lower() == _T("h")   ||
         Name.GetExt().Lower() == _T("hh")  ||
         Name.GetExt().Lower() == _T("hpp") ||
         Name.GetExt().Lower() == _T("h++") ||
         Name.GetExt().Lower() == _T("hxx") )
    {
      Files.Add(Name.GetFullPath());
    }
  }
}// AddFilesFromProject

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int Execution::ProcessFile(wxString& GlobalFileName,const wxArrayString& Groups)
{
  m_FileAnalysis.ReInit(GlobalFileName);

  // special operation modes:
  if ( !m_FileAnalysis.IsHeaderFile() && m_Processor==ProcessHeaderFiles )
    return 0; // header files only
  if (  m_FileAnalysis.IsHeaderFile() && m_Processor==ProcessSourceFiles )
    return 0; // implementation files only

  Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Processing \"")+GlobalFileName+_T("\"...")));

  // first iteration: scanning the content for existing includes
  m_FileAnalysis.LoadFile();
  wxArrayString IncludedHeaders = m_FileAnalysis.ParseForIncludes();
  wxArrayString RequiredHeaders;

  bool InMultilineComment = false;
  bool InsideString       = false;
  wxString PrevToken;
  while ( m_FileAnalysis.HasMoreLines() )
  {
    wxString Line = m_FileAnalysis.GetNextLine();
    Line.Trim(false);
    if ( !Line.IsEmpty() && Line.GetChar(0)!=_T('#') )
    {
      // line with content to analyse
      while ( !Line.IsEmpty() )
      {
        if ( InMultilineComment )
          InMultilineComment = nsHeaderFixUp::IsInsideMultilineComment( Line );
        if ( InsideString )
          InsideString       = nsHeaderFixUp::IsInsideString( Line );

        // Extracting one token
        wxString Token;

        while ( !Line.IsEmpty() && !InsideString )
        {
          // Take the next char to process and remove it from Line
          wxChar Ch = Line.GetChar(0);
          Line.Remove(0,1);
          // Check for comments of style "//" and "/* */"
          if ( Ch == '/' )
          {
            if ( !Line.IsEmpty() )
            {
              if ( Line.GetChar(0)=='/' )
              {
                // Comment of style "//" to the end of line -> skip line
                Line.Clear();
                break; // end while
              }
              if ( Line.GetChar(0)=='*' )
              {
                // BEGIN Multiline "/* */" comment
                Line.Remove(0,1);
                InMultilineComment = true;
                break; // end while
              }
            }
          }
          // Check for strings
          else if ( Ch == '\"' )
          {
            // BEGIN Inside string
            InsideString = true;
            break; // end while
          }

          if ( Token.IsEmpty() )
          {
            // first char -> only alpha chars (cannot be numeric)
            if ( wxString(g_alpha_chars).Find(Ch) != wxNOT_FOUND )
              Token.Append(Ch);
          }
          else
          {
            // n'th char -> alpha and numeric chars
            if ( wxString(g_alpha_numeric_chars).Find(Ch) != wxNOT_FOUND )
              Token.Append(Ch);
            else
            {
              OperateToken(PrevToken,Token,Groups,IncludedHeaders,
                           Ch,Line,RequiredHeaders);
              PrevToken = Token;
              Token.Clear();
            }
          }
        }
      }
    }
  }

  // Now in RequiredHeaders we have all headers used in this file,
  // in IncludedHeaders we have list of includes from this file.
  // IncludedHeaders and/or RequiredHeaders may contain duplicated
  // entries so we sort them so they will be grouped.
  IncludedHeaders.Sort();
  RequiredHeaders.Sort();

  wxString PrevHeader; // avoid duplicates: remember header just been checked

  // Now check if there *might* be entries not required and report them
  if ( m_ObsoleteLog->IsChecked() )
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: List of included files not required for known bindings:")));
    for ( size_t i=0; i<IncludedHeaders.GetCount(); i++ )
    {
      if ( IncludedHeaders[i] != PrevHeader )
      {
        PrevHeader = IncludedHeaders[i];
        if ( RequiredHeaders.Index(PrevHeader) == wxNOT_FOUND )
          Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: - \"")+PrevHeader+_T("\"")));
      }
    }
  }

  // Now check what includes are missing and add them to the code...
  wxString HeadersCode = _T("// *** ADDED BY HEADER FIXUP ***\n");

  int Added = 0;
  PrevHeader.Empty();
  for ( size_t i=0; i<RequiredHeaders.GetCount(); i++ )
  {
    if ( RequiredHeaders[i] != PrevHeader )
    {
      PrevHeader = RequiredHeaders[i];
      if ( IncludedHeaders.Index(PrevHeader) == wxNOT_FOUND )
      {
        Added++;
        if ( PrevHeader.StartsWith(_T("class ")) ) // fwd decl
          HeadersCode << PrevHeader+_T("\n");
        else
        {
          if ( m_Options->GetSelection() == 0 )
            HeadersCode << _T("#include \"")+PrevHeader+_T("\"\n");
          else
            HeadersCode << _T("#include <")+PrevHeader+_T(">\n");
        }
      }
    }
    // else: header already operated
  }
  HeadersCode << _T("// *** END ***\n");

  // Nothing to add so we don't touch original file
  if ( !Added )
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+GlobalFileName+_T("\" requires no update.")));
    return 0;
  }


  // Now we detect end-of-line style to prevent inconsistent EOLs
  HeadersCode.Replace(_T("\n"),m_FileAnalysis.GetEOL(),true);


  if ( m_Simulation->IsChecked() )
  {
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+GlobalFileName+_T("\" *would have been* updated (simulation only).")));
  }
  else
  {
    m_FileAnalysis.SaveFile(HeadersCode);
    Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: ")+GlobalFileName+_T("\" has been updated.")));
  }

  return Added;
}// ProcessFile

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Execution::OperateToken(const wxString&      PrevToken,
                             const wxString&      Token,
                             const wxArrayString& Groups,
                             const wxArrayString& IncludedHeaders,
                             const wxChar&        Ch,
                             const wxString&      Line,
                             wxArrayString&       RequiredHeaders)
{
  // iterate through all groups for bindings
  for ( size_t i=0; i<Groups.GetCount(); i++ )
  {
    wxArrayString RequiredHeadersForToken;
    m_Bindings.GetBindings(Groups[i],Token,RequiredHeadersForToken);

    if ( !RequiredHeadersForToken.IsEmpty() ) // -> found bindings
    {
      // if header file: filter forward declarations
      if ( m_FileAnalysis.IsHeaderFile() && PrevToken.IsSameAs(_T("class")) )
      {
        if ( nsHeaderFixUp::IsNextChar(_T(";"),Ch,Line) )
        {
          if ( m_DebugLog->IsChecked() )
            Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Token \"")+Token+_T("\" skipped (used as forward declaration).")));
          return;
        }
      }

      for ( size_t i=0; i<RequiredHeadersForToken.GetCount(); i++ )
      {
        // check if required header file is already included
        if ( IncludedHeaders.Index(RequiredHeadersForToken[i]) == wxNOT_FOUND )
        {
          // check: required header file not already included due to other binding
          if ( RequiredHeaders.Index(RequiredHeadersForToken[i]) == wxNOT_FOUND )
          {
            // check: forward declaration might be enough, applies if:
            // - we operate a header file (only there forward decls make sense)
            // - option is enabled by the user
            if ( m_FileAnalysis.IsHeaderFile() && m_FwdDecl->IsChecked() )
            {
              if (   nsHeaderFixUp::IsNextChar(_T("*"),Ch,Line)
                  || nsHeaderFixUp::IsNextChar(_T("&"),Ch,Line) )
              {
                // replace header file with forward declaration
                RequiredHeadersForToken[i] = _T("class ")+Token+_T(";");
              }
            }

            RequiredHeaders.Add(RequiredHeadersForToken[i]);
            if ( m_DebugLog->IsChecked() )
            {
              Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Token \"")+Token+_T("\" requires entry \"")+
                     RequiredHeadersForToken[i]+_T("\".")));
            }
          }
        }
        else if ( m_ObsoleteLog->IsChecked() ) // log header file anyway for obsolete record
        {
          RequiredHeaders.Add(RequiredHeadersForToken[i]);
          if ( m_DebugLog->IsChecked() )
          {
            Manager::Get()->GetLogManager()->DebugLog(F(_T("[HeaderFixup]: Token \"")+Token+_T("\" would require entry \"")+
                   RequiredHeadersForToken[i]+_T("\" (already included).")));
          }
        }
      }
    }
  }
}// OperateToken
