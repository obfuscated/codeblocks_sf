/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "ProjectOptionsManipulatorDlg.h"

//(*InternalHeaders(ProjectOptionsManipulatorDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(ProjectOptionsManipulatorDlg)
const long ProjectOptionsManipulatorDlg::ID_CHO_SCAN = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHO_SCAN_PROJECTS = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_RBO_OPERATION = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_TXT_OPTIONS = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_COMPILER = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_LINKER = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_COMPILER_PATH = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_LINKER_PATH = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_RES_COMP_PATH = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_LINKER_LIBS = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHK_OPTIONS_CUSTOM_VAR = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_TXT_CUSTOM_VAR = wxNewId();
const long ProjectOptionsManipulatorDlg::ID_CHO_OPTIONS_LEVEL = wxNewId();
//*)

#include <cbproject.h>
#include <globals.h> // cbMessageBox
#include <manager.h>
#include <projectmanager.h>

BEGIN_EVENT_TABLE(ProjectOptionsManipulatorDlg,wxDialog)
	//(*EventTable(ProjectOptionsManipulatorDlg)
	//*)
	EVT_BUTTON(wxID_OK,ProjectOptionsManipulatorDlg::OnOk)
END_EVENT_TABLE()

ProjectOptionsManipulatorDlg::ProjectOptionsManipulatorDlg(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(ProjectOptionsManipulatorDlg)
	wxStaticText* lblOptions;
	wxBoxSizer* bszScan;
	wxFlexGridSizer* flsOptions;
	wxStdDialogButtonSizer* sbzOKCancel;
	wxBoxSizer* bszMainH;
	wxStaticText* lblScanColon;
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* bszOptions;
	wxStaticText* lblScanWithin;

	Create(parent, id, _("Project Options Plugin"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	bszMainH = new wxBoxSizer(wxVERTICAL);
	bszScan = new wxBoxSizer(wxHORIZONTAL);
	lblScanWithin = new wxStaticText(this, wxID_ANY, _("Scan within:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	bszScan->Add(lblScanWithin, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ChoScan = new wxChoice(this, ID_CHO_SCAN, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_SCAN"));
	m_ChoScan->SetSelection( m_ChoScan->Append(_("Workspace")) );
	m_ChoScan->Append(_("Project"));
	bszScan->Add(m_ChoScan, 0, wxTOP|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	lblScanColon = new wxStaticText(this, wxID_ANY, _(":"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	bszScan->Add(lblScanColon, 0, wxTOP|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ChoScanProjects = new wxChoice(this, ID_CHO_SCAN_PROJECTS, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_SCAN_PROJECTS"));
	m_ChoScanProjects->Disable();
	bszScan->Add(m_ChoScanProjects, 1, wxTOP|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszMainH->Add(bszScan, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	flsOptions = new wxFlexGridSizer(0, 2, 0, 0);
	flsOptions->AddGrowableCol(1);
	wxString __wxRadioBoxChoices_1[4] =
	{
		_("Search for option present"),
		_("Search for option NOT present"),
		_("Remove option"),
		_("Add option")
	};
	m_RboOperation = new wxRadioBox(this, ID_RBO_OPERATION, _("Operation:"), wxDefaultPosition, wxDefaultSize, 4, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RBO_OPERATION"));
	flsOptions->Add(m_RboOperation, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszOptions = new wxBoxSizer(wxVERTICAL);
	lblOptions = new wxStaticText(this, wxID_ANY, _("Option/Var (i.e. -Wl,--no-undefined):"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	bszOptions->Add(lblOptions, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_TxtOptions = new wxTextCtrl(this, ID_TXT_OPTIONS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_OPTIONS"));
	m_TxtOptions->SetToolTip(_("This is the compiler/linker option or path, linker lib or custom var to search for..."));
	bszOptions->Add(m_TxtOptions, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsCompiler = new wxCheckBox(this, ID_CHK_OPTIONS_COMPILER, _("Compiler options"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_COMPILER"));
	m_ChkOptionsCompiler->SetValue(true);
	bszOptions->Add(m_ChkOptionsCompiler, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsLinker = new wxCheckBox(this, ID_CHK_OPTIONS_LINKER, _("Linker options"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_LINKER"));
	m_ChkOptionsLinker->SetValue(false);
	bszOptions->Add(m_ChkOptionsLinker, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsCompilerPath = new wxCheckBox(this, ID_CHK_OPTIONS_COMPILER_PATH, _("Compiler path\'s"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_COMPILER_PATH"));
	m_ChkOptionsCompilerPath->SetValue(false);
	bszOptions->Add(m_ChkOptionsCompilerPath, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsLinkerPath = new wxCheckBox(this, ID_CHK_OPTIONS_LINKER_PATH, _("Linker path\'s"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_LINKER_PATH"));
	m_ChkOptionsLinkerPath->SetValue(false);
	bszOptions->Add(m_ChkOptionsLinkerPath, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsResCompPath = new wxCheckBox(this, ID_CHK_OPTIONS_RES_COMP_PATH, _("Resource compiler path\'s"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_RES_COMP_PATH"));
	m_ChkOptionsResCompPath->SetValue(false);
	bszOptions->Add(m_ChkOptionsResCompPath, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChkOptionsLinkerLibs = new wxCheckBox(this, ID_CHK_OPTIONS_LINKER_LIBS, _("Linker libs"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_LINKER_LIBS"));
	m_ChkOptionsLinkerLibs->SetValue(false);
	bszOptions->Add(m_ChkOptionsLinkerLibs, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	m_ChkOptionsCustomVar = new wxCheckBox(this, ID_CHK_OPTIONS_CUSTOM_VAR, _("Custom var ="), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_OPTIONS_CUSTOM_VAR"));
	m_ChkOptionsCustomVar->SetValue(false);
	BoxSizer1->Add(m_ChkOptionsCustomVar, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_TxtCustomVar = new wxTextCtrl(this, ID_TXT_CUSTOM_VAR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CUSTOM_VAR"));
	m_TxtCustomVar->SetToolTip(_("This is the value to set set for the custom var, if \"add option\" is chosen"));
	BoxSizer1->Add(m_TxtCustomVar, 1, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	bszOptions->Add(BoxSizer1, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_ChoOptionsLevel = new wxChoice(this, ID_CHO_OPTIONS_LEVEL, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_OPTIONS_LEVEL"));
	m_ChoOptionsLevel->Append(_("At project level"));
	m_ChoOptionsLevel->Append(_("At target level"));
	m_ChoOptionsLevel->SetSelection( m_ChoOptionsLevel->Append(_("Both (project and target level)")) );
	bszOptions->Add(m_ChoOptionsLevel, 0, wxTOP|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	flsOptions->Add(bszOptions, 1, wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszMainH->Add(flsOptions, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	sbzOKCancel = new wxStdDialogButtonSizer();
	sbzOKCancel->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	sbzOKCancel->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	sbzOKCancel->Realize();
	bszMainH->Add(sbzOKCancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(bszMainH);
	bszMainH->Fit(this);
	bszMainH->SetSizeHints(this);

	Connect(ID_CHO_SCAN,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&ProjectOptionsManipulatorDlg::OnScanSelect);
	//*)
}

ProjectOptionsManipulatorDlg::~ProjectOptionsManipulatorDlg()
{
	//(*Destroy(ProjectOptionsManipulatorDlg)
	//*)
}

bool ProjectOptionsManipulatorDlg::GetScanForWorkspace()
{
  return ( m_ChoScan->GetSelection()==0 );
}

bool ProjectOptionsManipulatorDlg::GetScanForProject()
{
  return ( m_ChoScan->GetSelection()==1 );
}

size_t ProjectOptionsManipulatorDlg::GetProjectIdx()
{
  return ( (m_ChoScan->GetSelection()==1) ? m_ChoScanProjects->GetSelection() : -1 );
}

ProjectOptionsManipulatorDlg::EProjectScanOption ProjectOptionsManipulatorDlg::GetScanOption()
{
  switch ( m_RboOperation->GetSelection() )
  {
    case 0: { return eSearch;    } break;
    case 1: { return eSearchNot; } break;
    case 2: { return eRemove;    } break;
    case 3: { return eAdd;       } break;
    default:                       break;
  }
  return eSearch; // should never happen, but is safe to do
}

wxString ProjectOptionsManipulatorDlg::GetOption()
{
  return ( m_TxtOptions->GetValue().Trim(true).Trim(false) );
}

wxString ProjectOptionsManipulatorDlg::GetValue()
{
  return ( m_TxtCustomVar->GetValue().Trim(true).Trim(false) );
}

bool ProjectOptionsManipulatorDlg::GetOptionActive(EProjectOption opt)
{
  if ( m_ChkOptionsCompiler->GetValue()     && (opt==eCompiler)      ) return true;
  if ( m_ChkOptionsLinker->GetValue()       && (opt==eLinker)        ) return true;
  if ( m_ChkOptionsCompilerPath->GetValue() && (opt==eCompilerPaths) ) return true;
  if ( m_ChkOptionsLinkerPath->GetValue()   && (opt==eLinkerPaths)   ) return true;
  if ( m_ChkOptionsLinkerPath->GetValue()   && (opt==eResCompPaths)  ) return true;
  if ( m_ChkOptionsLinkerLibs->GetValue()   && (opt==eLinkerLibs)    ) return true;
  if ( m_ChkOptionsCustomVar->GetValue()    && (opt==eCustomVars)    ) return true;
  return false;
}

bool ProjectOptionsManipulatorDlg::GetOptionActive(EProjectLevelOption opt)
{
  if ( m_ChoOptionsLevel->GetSelection()==0 && (opt==eProject) ) return true;
  if ( m_ChoOptionsLevel->GetSelection()==1 && (opt==eTarget)  ) return true;
  if ( m_ChoOptionsLevel->GetSelection()==2 )                    return true;
  return false;
}

void ProjectOptionsManipulatorDlg::OnScanSelect(wxCommandEvent& event)
{
  m_ChoScanProjects->Clear();
  if ( event.GetInt()==1 ) // project
  {
    ProjectsArray* pa = Manager::Get()->GetProjectManager()->GetProjects();
    if (pa)
    {
      for (size_t i=0; i<pa->GetCount(); ++i)
      {
        cbProject* prj = pa->Item(i);
        if (prj)
          m_ChoScanProjects->Append( prj->GetTitle() );
      }

      if ( pa->GetCount() )
        m_ChoScanProjects->SetSelection(0);
    }
    m_ChoScanProjects->Enable();
  }
  else
    m_ChoScanProjects->Disable();
}

void ProjectOptionsManipulatorDlg::OnOk(wxCommandEvent& WXUNUSED(event))
{
  if ( m_TxtOptions->GetValue().Trim().IsEmpty() )
  {
    cbMessageBox(_("You need to provide an option to search for!"), _("Error"), wxICON_ERROR, this);
    return;
  }

  if (   !m_ChkOptionsCompiler->GetValue()
      && !m_ChkOptionsLinker->GetValue()
      && !m_ChkOptionsCompilerPath->GetValue()
      && !m_ChkOptionsLinkerPath->GetValue()
      && !m_ChkOptionsLinkerLibs->GetValue()
      && !m_ChkOptionsCustomVar->GetValue() )
  {
    cbMessageBox(_("You need to select at least one option to search in!"), _("Error"), wxICON_ERROR, this);
    return;
  }

  EndModal(wxID_OK);
}
