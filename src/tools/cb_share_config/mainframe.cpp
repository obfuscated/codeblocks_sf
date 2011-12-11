/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "mainframe.h"

#include <wx/file.h> // wxFile, wxTempFile
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/tokenzr.h>

#ifndef __WXMSW__
  #include <wx/filename.h> // wxFileName
#endif

#ifdef __WXMSW__
  #include <shlobj.h>
#endif

#include "tinyxml/tinywxuni.h"

//***********************************************************************

//(*InternalHeaders(MainFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(MainFrame)
const long MainFrame::ID_LBL_STEPS = wxNewId();
const long MainFrame::ID_LBL_FILE_SRC = wxNewId();
const long MainFrame::ID_LBL_FILE_DST = wxNewId();
const long MainFrame::ID_TXT_FILE_SRC = wxNewId();
const long MainFrame::ID_BTN_FILE_SRC = wxNewId();
const long MainFrame::ID_TXT_FILE_DST = wxNewId();
const long MainFrame::ID_BTN_FILE_DST = wxNewId();
const long MainFrame::ID_CFG_SRC = wxNewId();
const long MainFrame::ID_LST_CFG = wxNewId();
const long MainFrame::ID_BTN_TRANSFER = wxNewId();
const long MainFrame::ID_BTN_UNCHECK = wxNewId();
const long MainFrame::ID_BTN_EXPORT = wxNewId();
const long MainFrame::ID_BTN_SAVE = wxNewId();
const long MainFrame::ID_BTN_CLOSE = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()

//***********************************************************************

MainFrame::MainFrame(wxWindow* parent,wxWindowID id) :
  mFileSrc(wxT("")), mCfgSrc(0), mCfgSrcValid(false), mNodesSrc(),
  mFileDst(wxT("")), mCfgDst(0), mCfgDstValid(false), mNodesDst()
{
	//(*Initialize(MainFrame)
	Create(parent, id, _("Welcome to Code::Blocks Share Config"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxSYSTEM_MENU|wxRESIZE_BORDER|wxCLOSE_BOX|wxMINIMIZE_BOX, _T("id"));
	SetMinSize(wxSize(640,480));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	bszMain = new wxBoxSizer(wxVERTICAL);
	bszSteps = new wxBoxSizer(wxHORIZONTAL);
	sbsSteps = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Steps to do:"));
	lblSteps = new wxStaticText(this, ID_LBL_STEPS, _("- make sure C::B is *not* running\n- select the C::B source configuration file on the left\n- select the C::B destination configuration file on the right\n- select the sections you would like to transfer\n- verify again and do the transfer\n- save the modified (right) configuration"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_STEPS"));
	sbsSteps->Add(lblSteps, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszSteps->Add(sbsSteps, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszMain->Add(bszSteps, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	grsFileLabel = new wxGridSizer(1, 2, 0, 0);
	lblFileSrc = new wxStaticText(this, ID_LBL_FILE_SRC, _("Source configuration file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_FILE_SRC"));
	grsFileLabel->Add(lblFileSrc, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	lblFileDst = new wxStaticText(this, ID_LBL_FILE_DST, _("Destination configuration file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_FILE_DST"));
	grsFileLabel->Add(lblFileDst, 0, wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	bszMain->Add(grsFileLabel, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	grsFile = new wxGridSizer(1, 2, 0, 0);
	flsFileSrc = new wxFlexGridSizer(1, 2, 0, 0);
	flsFileSrc->AddGrowableCol(0);
	txtFileSrc = new wxTextCtrl(this, ID_TXT_FILE_SRC, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_FILE_SRC"));
	flsFileSrc->Add(txtFileSrc, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnFileSrc = new wxButton(this, ID_BTN_FILE_SRC, _("..."), wxDefaultPosition, wxSize(32,-1), 0, wxDefaultValidator, _T("ID_BTN_FILE_SRC"));
	btnFileSrc->SetToolTip(_("Select the source C::B configuration file."));
	flsFileSrc->Add(btnFileSrc, 0, wxLEFT|wxALIGN_RIGHT|wxALIGN_TOP, 5);
	grsFile->Add(flsFileSrc, 0, wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	flsFileDst = new wxFlexGridSizer(1, 2, 0, 0);
	flsFileDst->AddGrowableCol(0);
	txtFileDst = new wxTextCtrl(this, ID_TXT_FILE_DST, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_FILE_DST"));
	flsFileDst->Add(txtFileDst, 0, wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnFileDst = new wxButton(this, ID_BTN_FILE_DST, _("..."), wxDefaultPosition, wxSize(32,-1), 0, wxDefaultValidator, _T("ID_BTN_FILE_DST"));
	btnFileDst->SetToolTip(_("Select the destination C::B configuration file."));
	flsFileDst->Add(btnFileDst, 0, wxLEFT|wxALIGN_RIGHT|wxALIGN_TOP, 5);
	grsFile->Add(flsFileDst, 0, wxLEFT|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	bszMain->Add(grsFile, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	grsCfg = new wxGridSizer(1, 2, 0, 0);
	clbCfgSrc = new wxCheckListBox(this, ID_CFG_SRC, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CFG_SRC"));
	grsCfg->Add(clbCfgSrc, 0, wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	lstCfgDst = new wxListBox(this, ID_LST_CFG, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LST_CFG"));
	grsCfg->Add(lstCfgDst, 0, wxLEFT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	bszMain->Add(grsCfg, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	grsAction = new wxGridSizer(1, 8, 0, 0);
	btnTransfer = new wxButton(this, ID_BTN_TRANSFER, _("Transfer >>"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_TRANSFER"));
	btnTransfer->SetToolTip(_("Transfer the selection on the left to right."));
	grsAction->Add(btnTransfer, 0, wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnUncheck = new wxButton(this, ID_BTN_UNCHECK, _("Uncheck all"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_UNCHECK"));
	grsAction->Add(btnUncheck, 0, wxALIGN_LEFT|wxALIGN_TOP, 0);
	grsAction->Add(-1,-1,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnExport = new wxButton(this, ID_BTN_EXPORT, _("Export"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_EXPORT"));
	btnExport->SetToolTip(_("Export the selection on the left to a C::B config backup file."));
	grsAction->Add(btnExport, 0, wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnSave = new wxButton(this, ID_BTN_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_SAVE"));
	btnSave->SetToolTip(_("Save the selection on the right into the C::B destination config file."));
	grsAction->Add(btnSave, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_TOP, 5);
	grsAction->Add(-1,-1,1, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	grsAction->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnClose = new wxButton(this, ID_BTN_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_CLOSE"));
	btnClose->SetToolTip(_("Close the application."));
	grsAction->Add(btnClose, 0, wxALIGN_RIGHT|wxALIGN_TOP, 0);
	bszMain->Add(grsAction, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
	SetSizer(bszMain);
	bszMain->Fit(this);
	bszMain->SetSizeHints(this);
	Center();

	Connect(ID_BTN_FILE_SRC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnFileSrcClick);
	Connect(ID_BTN_FILE_DST,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnFileDstClick);
	Connect(ID_BTN_TRANSFER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnTransferClick);
	Connect(ID_BTN_UNCHECK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnUncheckClick);
	Connect(ID_BTN_EXPORT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnExportClick);
	Connect(ID_BTN_SAVE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnSaveClick);
	Connect(ID_BTN_CLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnBtnCloseClick);
	//*)
}// MainFrame

//***********************************************************************

MainFrame::~MainFrame()
{
  if (mCfgSrc) delete mCfgSrc;
  if (mCfgDst) delete mCfgDst;
}// ~MainFrame

//***********************************************************************

void MainFrame::OnBtnFileSrcClick(wxCommandEvent& /*event*/)
{
  wxString filename = FileSelector();
  if (filename.IsEmpty() || SameConfig(filename, txtFileDst))
    return;

  if (!LoadConfig(filename, &mCfgSrc) || !mCfgSrc)
  {
    mCfgSrcValid = false;
    return;
  }

  mFileSrc = filename;
  txtFileSrc->SetValue(filename);
  mCfgSrcValid = true;

  // put configuration to wxCheckListBox
  OfferConfig(mCfgSrc, (wxListBox*)clbCfgSrc, &mNodesSrc);
}// OnBtnFileSrcClick

//***********************************************************************

void MainFrame::OnBtnFileDstClick(wxCommandEvent& /*event*/)
{
  wxString filename = FileSelector();
  if (filename.IsEmpty() || SameConfig(filename, txtFileSrc))
    return;

  if (!LoadConfig(filename, &mCfgDst) || !mCfgDst)
  {
    wxMessageBox(wxT("Hint: To backup (export) your configuration use the \"Export\" button,\n"
                     "to transfer to an existing (valid Code::Blocks) configuration file,\n"
                     "use the \"Transfer\" button."),
                 wxT("Information"), wxICON_INFORMATION | wxOK);
    mCfgDstValid = false;
    return;
  }

  mFileDst = filename;
  txtFileDst->SetValue(filename);
  mCfgDstValid = true;

  // put configuration to wxListBox
  OfferConfig(mCfgDst, lstCfgDst, &mNodesDst);
}// OnBtnFileDstClick

//***********************************************************************

void MainFrame::OnBtnTransferClick(wxCommandEvent& /*event*/)
{
  if (mCfgSrcValid && mCfgDstValid)
  {
    if (wxMessageBox(wxT("Are you sure to transfer the selected nodes to the destination?"),
                     wxT("Question"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT ) == wxYES)
    {
      // Set all (checked) variables of lstEnvVars
      int items_selected = 0;
      for (int i = 0; i < static_cast<int>(clbCfgSrc->GetCount()); ++i)
      {
        if (clbCfgSrc->IsChecked(i) && (static_cast<int>(mNodesSrc.size()) > i))
        {
          items_selected++;

          TiXmlNode* node = mNodesSrc.at(i);
          wxString   path = clbCfgSrc->GetString(i);

          if (!TransferNode(&node, PathToArray(path)))
          {
            wxMessageBox(wxT("The node \"") + path + wxT("\" could not be transferred.\n"
                             "Corrupted / unexpected configuration structure?"),
                         wxT("Error"), wxICON_EXCLAMATION | wxOK);
            return;
          }
        }
      }

      if (items_selected)
      {
        // Update GUI after transfer(s)
        OfferConfig(mCfgDst, lstCfgDst, &mNodesDst);

        wxMessageBox(wxT("Selected items have been transferred successfully.\n"
                         "Save the destination file to update the configuration permanently."),
                     wxT("Info"), wxICON_INFORMATION | wxOK);
      }
      else
      {
        wxMessageBox(wxT("There were no items selected to transfer."),
                     wxT("Warning"), wxICON_EXCLAMATION | wxOK);
      }
    }
  }
  else
  {
    wxMessageBox(wxT("Cannot begin transfer.\n"
                     "At least one configuration is empty or invalid!"),
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
  }
}// OnBtnTransferClick

//***********************************************************************

void MainFrame::OnBtnUncheckClick(wxCommandEvent& /*event*/)
{
  for (int i=0; i < static_cast<int>(clbCfgSrc->GetCount()); ++i)
    clbCfgSrc->Check(i, false);
}// OnBtnUncheckClick

//***********************************************************************

void MainFrame::OnBtnExportClick(wxCommandEvent& /*event*/)
{
  TiXmlDocument* doc = new TiXmlDocument();

  if (doc)
  {
    if (mCfgSrcValid)
    {
      wxMessageBox(wxT("You are about to export the selected node(s) to a backup C::B configuration file.\n"
                       "Please note that this is *not* complete because it includes the selected node(s) only.\n"
                       "It's purpose is to backup misc. nodes for transferring them using cb_share_config."),
                   wxT("Information"), wxICON_INFORMATION);

      TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
      TiXmlElement*     root = new TiXmlElement("CodeBlocksConfig");
      // NOTE (Morten#1#): This has to be in sync with C::B SDK (confimanager)!
      root->SetAttribute("version", 1);

      doc->LinkEndChild(decl);
      doc->LinkEndChild(root);

      // Save all (checked) variables of lstEnvVars
      int items_selected = 0;
      for (int i=0; i < static_cast<int>(clbCfgSrc->GetCount()); ++i)
      {
        if (clbCfgSrc->IsChecked(i) && (static_cast<int>(mNodesSrc.size()) > i))
        {
          items_selected++;

          TiXmlNode*    node     = mNodesSrc.at(i);
          wxString      path     = clbCfgSrc->GetString(i);
          wxArrayString path_arr = PathToArray(path);

          TiXmlElement* element  = root;
          for (size_t i=0; i<path_arr.Count(); ++i)
          {
            wxString section_path = path_arr.Item(i);
            if (element->NoChildren())
            {
              // element has no children -> create new child named after section
              element = (TiXmlElement*) element->InsertEndChild(
                TiXmlElement(
#if wxUSE_UNICODE
                  section_path.mb_str(wxConvUTF8)
#else
                  (wxChar*)section_path.mb_str()
#endif
                ));
            }// if
            else
            {
              // element has children -> find child named after section
              TiXmlElement* new_element = element->FirstChildElement(
#if wxUSE_UNICODE
                section_path.mb_str(wxConvUTF8)
#else
                (wxChar*)section_path.mb_str()
#endif
              );

              if (!new_element)
              {
                // child could not be found -> create child named after section
                element = (TiXmlElement*) element->InsertEndChild(TiXmlElement(
#if wxUSE_UNICODE
                  section_path.mb_str(wxConvUTF8)
#else
                  (wxChar*)section_path.mb_str()
#endif
                  ));
              }
              else
              {
                // child has been found -> switch to this child
                element = new_element;
              }
            }

            if (!element)
              return;
            // ...continue with next section.
          }

          TiXmlNode* parent_node = element->Parent();
          parent_node->ReplaceChild(element, *node);
        }
      }

      if (items_selected)
      {
        wxString filename = wxFileSelector
        (
          wxT("Choose a Code::Blocks backup configuration file"), // title
          wxT(""),                                                // default path
          wxT("backup.conf"),                                     // default file
          wxT("*.conf"),                                          // default extension
          wxT("Code::Blocks configuration files (*.conf)|*.conf|"
              "All files (*.*)|*.*"),                             // wildcards
          wxFD_SAVE                                               // flags
        );
        if (!filename.IsEmpty())
        {
          if (TiXmlSaveDocument(filename, doc))
          {
            wxMessageBox(wxT("Backup configuration file has been saved."),
                         wxT("Information"), wxICON_INFORMATION | wxOK);
          }
          else
          {
            wxMessageBox(wxT("Could not save backup configuration file."),
                         wxT("Warning"), wxICON_EXCLAMATION | wxOK);
          }
        }
      }
      else
      {
        wxMessageBox(wxT("There were no items selected to backup."),
                     wxT("Warning"), wxICON_EXCLAMATION | wxOK);
      }
    }

    delete doc;
  }
  else
  {
    wxMessageBox(wxT("Cannot create empty XML document...?!"),
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
  }
}// OnBtnExportClick

//***********************************************************************

void MainFrame::OnBtnSaveClick(wxCommandEvent& /*event*/)
{
  if (wxMessageBox(wxT("Are you sure to save destination configuration file?"),
                   wxT("Question"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT ) == wxYES)
  {
    if (TiXmlSaveDocument(mFileDst, mCfgDst))
    {
      wxMessageBox(wxT("Destination file has been saved (updated)."),
                   wxT("Information"), wxICON_INFORMATION | wxOK);
    }
    else
    {
      wxMessageBox(wxT("Could not save destination configuration file."),
                   wxT("Warning"), wxICON_EXCLAMATION | wxOK);
    }
  }
}// OnBtnSaveClick

//***********************************************************************

void MainFrame::OnBtnCloseClick(wxCommandEvent& /*event*/)
{
	Close();
}// OnBtnCloseClick

//***********************************************************************

wxString MainFrame::FileSelector()
{
#ifdef __WXMSW__
  TCHAR szPath[MAX_PATH];
  SHGetFolderPath(NULL, CSIDL_APPDATA, 0, 0, szPath);
  wxString config_folder = wxString(szPath) + wxT("\\codeblocks");
#else
  wxFileName f;
  f.AssignHomeDir();
  wxString home_folder   = f.GetFullPath();
  wxString config_folder = home_folder + wxT("/.codeblocks");
#endif

  wxString filename = wxFileSelector
  (
    wxT("Choose a Code::Blocks configuration file"), // title
    config_folder,                                   // default path
    wxT("default.conf"),                             // default file
    wxT("*.conf"),                                   // default extension
    wxT("Code::Blocks configuration files (*.conf)|*.conf|"
        "All files (*.*)|*.*"),                      // wildcards
    wxFD_OPEN | wxFD_FILE_MUST_EXIST                 // flags
  );

  return filename;
}// FileSelector

//***********************************************************************

bool MainFrame::LoadConfig(const wxString& filename, TiXmlDocument** doc)
{
  if (*doc) delete *doc;
  *doc = new TiXmlDocument();

  if (!TiXmlLoadDocument(filename, *doc))
  {
    wxMessageBox(wxT("Error accessing configuration file!"),
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
    return false;
  }

  if (!TiXmlSuccess(*doc))
    return false;

  TiXmlElement* docroot = (*doc)->FirstChildElement("CodeBlocksConfig");

  if (!TiXmlSuccess(*doc))
    return false;

  const char *vers = docroot->Attribute("version");
  if (!vers || atoi(vers) != 1)
  {
    wxMessageBox(wxT("Unknown config file version encountered!"),
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
    return false;
  }

  (*doc)->ClearError();

  return true;
}// LoadConfig

//***********************************************************************

bool MainFrame::SameConfig(const wxString& filename, wxTextCtrl* txt)
{
  if (txt && filename.Matches(txt->GetValue()))
  {
    wxMessageBox(wxT("Cannot transfer configurations between the same file.\n"
                     "Please select two different configuration files!"),
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
    return true;
  }

  return false;
}// SameConfig

//***********************************************************************

void MainFrame::OfferConfig(TiXmlDocument* config, wxListBox* listbox,
                            std::vector<TiXmlNode*> *nodes)
{
  // put configuration to wxCheckListBox or wxListBox
  nodes->clear();
  listbox->Clear();
  TiXmlElement* cfgroot = config->FirstChildElement("CodeBlocksConfig");

  if (!TiXmlSuccess(config))
    return;

  TiXmlNode* child = NULL;
  for (child = cfgroot->FirstChild(); child; child = child->NextSibling())
  {
    if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
    {
      OfferNode(&child, listbox, nodes);
    }
  }
}// OfferConfig

//***********************************************************************

void MainFrame::OfferNode(TiXmlNode** node,               wxListBox* listbox,
                          std::vector<TiXmlNode*> *nodes, const wxString& prefix)
{
  wxString section((*node)->Value(), wxConvLocal);

  if      (section.MakeLower().Matches(wxT("auto_complete"))) // auto complete (abbreviations)
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("code_completion"))) // code completion plugin token replacements
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<code_completion>")); // recursive call
    }
  }
  else if (section.MakeLower().Matches(wxT("compiler")))    // compiler sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler>")); // recursive call
    }
  }
  else if (section.MakeLower().Matches(wxT("editor")))      // editor colour sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<editor>")); // recursive call
    }
  }
  else if (section.MakeLower().Matches(wxT("envvars")))     // envvar plugin variables
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("gcv")))         // global variables
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("help_plugin"))) // help plugin files
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("mime_types")))  // mime types
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("plugins")))     // plugins
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("project_manager"))) // file groups
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<project_manager>")); // recursive call
    }
  }
  else if (section.MakeLower().Matches(wxT("tools")))       // tools setup by the user
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // ----------------------------------------------------------
  // 1st recursion level: code_completion -> token_replacements
  // ----------------------------------------------------------
  else if (   prefix.Matches(wxT("<code_completion>"))
           && section.MakeLower().Matches(wxT("token_replacements")))// token replacements
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // -----------------------------------------------
  // 1st recursion level: compiler -> sets/user sets
  // -----------------------------------------------
  else if (   prefix.Matches(wxT("<compiler>"))
           && section.MakeLower().Matches(wxT("sets")))     // compiler sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler><sets>")); // recursive call
    }
  }
  else if (   prefix.Matches(wxT("<compiler>"))
           && section.MakeLower().Matches(wxT("user_sets")))// compiler user sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler><user_sets>")); // recursive call
    }
  }

  // --------------------------------------------------------
  // 2nd recursion level: compiler -> sets -> individual sets
  // --------------------------------------------------------
  else if (prefix.Matches(wxT("<compiler><sets>")))         // individual compiler sets
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // -------------------------------------------------------------
  // 2nd recursion level: compiler -> user sets -> individual sets
  // -------------------------------------------------------------
  else if (prefix.Matches(wxT("<compiler><user_sets>")))    // individual compiler user sets
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // ------------------------------------------
  // 1st recursion level: editor -> colour sets
  // ------------------------------------------
  else if (   prefix.Matches(wxT("<editor>"))
           && section.MakeLower().Matches(wxT("colour_sets")))// colour sets
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // -------------------------------------------
  // 1st recursion level: editor -> default code
  // -------------------------------------------
  else if (   prefix.Matches(wxT("<editor>"))
           && section.MakeLower().Matches(wxT("default_code")))// default code
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // ---------------------------------------------------
  // 1st recursion level: project_manager -> file_groups
  // ---------------------------------------------------
  else if (   prefix.Matches(wxT("<project_manager>"))
           && section.MakeLower().Matches(wxT("file_groups")))// file groups
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
}// OfferNode

//***********************************************************************

bool MainFrame::TransferNode(TiXmlNode** node, const wxArrayString& path)
{
  if (!path.IsEmpty())
  {
    TiXmlElement* element   = mCfgDst->FirstChildElement("CodeBlocksConfig");
    TiXmlNode*    node_copy = (*node)->Clone();

    if (!TiXmlSuccess(mCfgDst))
      return false;

    for (size_t i=0; i<path.Count(); ++i)
    {
      wxString section_path = path.Item(i);

      if (element->NoChildren())
      {
        // element has no children -> create new child named after section
        element = (TiXmlElement*) element->InsertEndChild(
          TiXmlElement(
#if wxUSE_UNICODE
            section_path.mb_str(wxConvUTF8)
#else
            (wxChar*)section_path.mb_str()
#endif
          ));
      }// if
      else
      {
        // element has children -> find child named after section
        TiXmlElement* new_element = element->FirstChildElement(
#if wxUSE_UNICODE
          section_path.mb_str(wxConvUTF8)
#else
          (wxChar*)section_path.mb_str()
#endif
        );

        if (!new_element)
        {
          // child could not be found -> create child named after section
          element = (TiXmlElement*) element->InsertEndChild(TiXmlElement(
#if wxUSE_UNICODE
            section_path.mb_str(wxConvUTF8)
#else
            (wxChar*)section_path.mb_str()
#endif
            ));
        }
        else
        {
          // child has been found -> switch to this child
          element = new_element;
        }
      }// else

      if (!element)
        return false;

      // ...continue with next section.
    }

    TiXmlNode* parent_node = element->Parent();
    parent_node->ReplaceChild(element, *node_copy);

    return true;
  }

  return false;
}// TransferNode

//***********************************************************************

wxArrayString MainFrame::PathToArray(const wxString& path)
{
  wxString      path_modifications = path;
  wxArrayString as;

  if (path_modifications.Freq('<')==path_modifications.Freq('>'))
  {
    wxStringTokenizer tkz(path_modifications, wxT("<"));
    while (tkz.HasMoreTokens())
    {
      wxString token = tkz.GetNextToken();
      if (!token.IsEmpty())
        as.Add(token.RemoveLast());
    }
  }
  else
  {
    wxMessageBox(wxT("Cannot convert XML path into array of strings!"),
                 wxT("Assertion failure."), wxICON_EXCLAMATION | wxOK);
  }

  return as;
}// PathToArray

//***********************************************************************

bool MainFrame::TiXmlLoadDocument(const wxString& filename, TiXmlDocument*doc)
{
  if (!doc || !wxFile::Access(filename, wxFile::read))
    return false;

  wxFile file(filename);
  size_t len = file.Length();

  char *input = new char[len+1];
  input[len] = '\0';
  file.Read(input, len);

  doc->Parse(input);
  delete[] input;
  return true;
}// TiXmlLoadDocument

//***********************************************************************

bool MainFrame::TiXmlSaveDocument(const wxString& filename, TiXmlDocument* doc)
{
  if (!doc)
    return false;

  TiXmlPrinter printer;
  printer.SetIndent("\t");
  doc->Accept(&printer);

  wxTempFile file(filename);
  if (file.IsOpened())
    if (file.Write(printer.CStr(), printer.Size()) && file.Commit())
      return true;

  return false;
}// TiXmlSaveDocument

//***********************************************************************

bool MainFrame::TiXmlSuccess(TiXmlDocument* doc)
{
  if (doc->ErrorId())
  {
    wxMessageBox(wxT("TinyXML error: ") +
#if wxUSE_UNICODE
                 wxString(doc->ErrorDesc(), wxConvUTF8),
#else
                 wxString(doc->ErrorDesc()),
#endif
                 wxT("Error"), wxICON_EXCLAMATION | wxOK);
    return false;
  }

  return true;
}// TiXmlSuccess
