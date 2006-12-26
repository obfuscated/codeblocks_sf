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

#ifdef TIXML_USE_STL
  #include <string>
#endif
#include "tinyxml/tinywxuni.h"

//***********************************************************************

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	EVT_BUTTON(ID_BTN_FILE_SRC,MainFrame::OnBtnFileSrcClick)
	EVT_BUTTON(ID_BTN_FILE_DST,MainFrame::OnBtnFileDstClick)
	EVT_BUTTON(ID_BTN_TRANSFER,MainFrame::OnBtnTransferClick)
	EVT_BUTTON(ID_BTN_SAVE,MainFrame::OnBtnSaveClick)
	EVT_BUTTON(ID_BTN_CLOSE,MainFrame::OnBtnCloseClick)
	//*)
END_EVENT_TABLE()

//***********************************************************************

MainFrame::MainFrame(wxWindow* parent,wxWindowID id) :
  mFileSrc(wxT("")), mCfgSrc(0), mCfgSrcValid(false), mNodesSrc(),
  mFileDst(wxT("")), mCfgDst(0), mCfgDstValid(false), mNodesDst()
{
	//(*Initialize(MainFrame)
	Create(parent,id,_("Welcome to Code::Blocks Share Config"),wxDefaultPosition,wxDefaultSize,wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxTHICK_FRAME|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX,_T(""));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	bszMain = new wxBoxSizer(wxVERTICAL);
	bszSteps = new wxBoxSizer(wxHORIZONTAL);
	txtSteps = new wxStaticText(this,ID_TXT_STEPS,_("Steps to do:\n- select the C::B source configuration file on the left\n- select the C::B destination configuration file on the right\n- select the sections you would like to transfer\n- verify again and do the transfer\n- save the modified (right) configuration"),wxDefaultPosition,wxDefaultSize,0,_("ID_TXT_STEPS"));
	bszSteps->Add(txtSteps,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	grsFileLabel = new wxGridSizer(1,2,0,0);
	lblFileSrc = new wxStaticText(this,ID_LBL_FILE_SRC,_("Source configuration file:"),wxDefaultPosition,wxDefaultSize,0,_("ID_LBL_FILE_SRC"));
	lblFileDst = new wxStaticText(this,ID_LBL_FILE_DST,_("Destination configuration file:"),wxDefaultPosition,wxDefaultSize,0,_("ID_LBL_FILE_DST"));
	grsFileLabel->Add(lblFileSrc,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	grsFileLabel->Add(lblFileDst,0,wxLEFT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	grsFile = new wxGridSizer(1,2,0,0);
	flsFileSrc = new wxFlexGridSizer(1,2,0,0);
	flsFileSrc->AddGrowableCol(0);
	txtFileSrc = new wxTextCtrl(this,ID_TXT_FILE_SRC,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_READONLY,wxDefaultValidator,_("ID_TXT_FILE_SRC"));
	if ( 0 ) txtFileSrc->SetMaxLength(0);
	btnFileSrc = new wxButton(this,ID_BTN_FILE_SRC,_("..."),wxDefaultPosition,wxSize(32,-1),0,wxDefaultValidator,_("ID_BTN_FILE_SRC"));
	if (false) btnFileSrc->SetDefault();
	flsFileSrc->Add(txtFileSrc,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	flsFileSrc->Add(btnFileSrc,0,wxLEFT|wxALIGN_RIGHT|wxALIGN_TOP,5);
	flsFileDst = new wxFlexGridSizer(1,2,0,0);
	flsFileDst->AddGrowableCol(0);
	txtFileDst = new wxTextCtrl(this,ID_TXT_FILE_DST,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_READONLY,wxDefaultValidator,_("ID_TXT_FILE_DST"));
	if ( 0 ) txtFileDst->SetMaxLength(0);
	btnFileDst = new wxButton(this,ID_BTN_FILE_DST,_("..."),wxDefaultPosition,wxSize(32,-1),0,wxDefaultValidator,_("ID_BTN_FILE_DST"));
	if (false) btnFileDst->SetDefault();
	flsFileDst->Add(txtFileDst,0,wxLEFT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	flsFileDst->Add(btnFileDst,0,wxLEFT|wxALIGN_RIGHT|wxALIGN_TOP,5);
	grsFile->Add(flsFileSrc,0,wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
	grsFile->Add(flsFileDst,0,wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
	grsCfg = new wxGridSizer(1,2,0,0);
	clbCfgSrc = new wxCheckListBox(this,ID_CFG_SRC,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CFG_SRC"));
	lstCfgDst = new wxListBox(this,ID_LST_CFG,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_("ID_LST_CFG"));
	lstCfgDst->SetSelection(-1);
	grsCfg->Add(clbCfgSrc,0,wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	grsCfg->Add(lstCfgDst,0,wxLEFT|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	grsAction = new wxGridSizer(1,4,0,0);
	btnTransfer = new wxButton(this,ID_BTN_TRANSFER,_("Transfer >>"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BTN_TRANSFER"));
	if (false) btnTransfer->SetDefault();
	btnSave = new wxButton(this,ID_BTN_SAVE,_("Save"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BTN_SAVE"));
	if (false) btnSave->SetDefault();
	btnClose = new wxButton(this,ID_BTN_CLOSE,_("Close"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BTN_CLOSE"));
	if (false) btnClose->SetDefault();
	grsAction->Add(btnTransfer,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	grsAction->Add(-1,-1,0);
	grsAction->Add(btnSave,0,wxLEFT|wxALIGN_LEFT|wxALIGN_TOP,5);
	grsAction->Add(btnClose,0,wxALL|wxALIGN_RIGHT|wxALIGN_TOP,0);
	bszMain->Add(bszSteps,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszMain->Add(grsFileLabel,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszMain->Add(grsFile,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszMain->Add(grsCfg,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszMain->Add(grsAction,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	this->SetSizer(bszMain);
	bszMain->Fit(this);
	bszMain->SetSizeHints(this);
	Center();
	//*)
}// MainFrame

//***********************************************************************

MainFrame::~MainFrame()
{
  if (mCfgSrc) delete mCfgSrc;
  if (mCfgDst) delete mCfgDst;
}// ~MainFrame

//***********************************************************************

void MainFrame::OnBtnFileSrcClick(wxCommandEvent& event)
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

void MainFrame::OnBtnFileDstClick(wxCommandEvent& event)
{
  wxString filename = FileSelector();
  if (filename.IsEmpty() || SameConfig(filename, txtFileSrc))
    return;

  if (!LoadConfig(filename, &mCfgDst) || !mCfgDst)
  {
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

void MainFrame::OnBtnCloseClick(wxCommandEvent& event)
{
	Close();
}// OnBtnCloseClick

//***********************************************************************

void MainFrame::OnBtnTransferClick(wxCommandEvent& event)
{
  if (mCfgSrcValid && mCfgDstValid)
  {
    if (wxMessageBox(wxT("Are you sure to transfer the selected nodes to the destination?"),
                     wxT("Question"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT ) == wxYES)
    {
      int items_selected = 0;

      // Set all (checked) variables of lstEnvVars
      for (int i=0; i<clbCfgSrc->GetCount(); ++i)
      {
        if (clbCfgSrc->IsChecked(i) && (mNodesSrc.size()>i))
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

void MainFrame::OnBtnSaveClick(wxCommandEvent& event)
{
  if (wxMessageBox(wxT("Are you sure to save destination configuration file?"),
                   wxT("Question"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT ) == wxYES)
  {
    if (!TiXmlSaveDocument(mFileDst, mCfgDst))
    {
      wxMessageBox(wxT("Could not save destination configuration file."),
                   wxT("Warning"), wxICON_EXCLAMATION | wxOK);
    }
  }
}// OnBtnSaveClick

//***********************************************************************

wxString MainFrame::FileSelector()
{
#ifdef __WXMSW__
  TCHAR szPath[MAX_PATH];
  SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
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
    wxOPEN | wxFILE_MUST_EXIST                       // flags
#if (WXWIN_COMPATIBILITY_2_4)
    | wxHIDE_READONLY
#endif
  );

  return filename;
}// FileSelector

//***********************************************************************

bool MainFrame::LoadConfig(const wxString& filename, TiXmlDocument** doc)
{
  if (*doc) delete *doc;
  *doc = new TiXmlDocument();

  if(!TiXmlLoadDocument(filename, *doc))
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
  if(!vers || atoi(vers) != 1)
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
    if (child->Type()==TiXmlNode::ELEMENT)
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

  if      (section.MakeLower().Matches(wxT("envvars")))     // envvar plugin variables
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
  else if (section.MakeLower().Matches(wxT("tools")))       // tools setup by the user
  {
    listbox->Append(wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }
  else if (section.MakeLower().Matches(wxT("compiler")))    // compiler sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler>")); // recursive call
    }
  }

  // ------------------------------
  // compiler -> sets and user sets
  // ------------------------------
  else if (   prefix.Matches(wxT("<compiler>"))
           && section.MakeLower().Matches(wxT("sets")))     // compiler sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler><sets>")); // recursive call
    }
  }
  else if (   prefix.Matches(wxT("<compiler>"))
           && section.MakeLower().Matches(wxT("user_sets")))// compiler user sets
  {
    TiXmlNode* child = NULL;
    for (child = (*node)->FirstChild(); child; child = child->NextSibling())
    {
      if (child->Type()==TiXmlNode::ELEMENT)
        OfferNode(&child, listbox, nodes, wxT("<compiler><user_sets>")); // recursive call
    }
  }

  // -----------------------------------
  // compiler -> sets -> individual sets
  // -----------------------------------
  else if (prefix.Matches(wxT("<compiler><sets>")))         // individual compiler sets
  {
    listbox->Append(prefix + wxT("<") + section + wxT(">"));
    nodes->push_back(*node);
  }

  // ----------------------------------------
  // compiler -> user sets -> individual sets
  // ----------------------------------------
  else if (prefix.Matches(wxT("<compiler><user_sets>")))    // individual compiler user sets
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
    int           level     = 0;
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
  if(!doc || !wxFile::Access(filename, wxFile::read))
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
  if(file.IsOpened())
    if(file.Write(printer.CStr(), printer.Size()) && file.Commit())
      return true;

  return false;
}// TiXmlSaveDocument

//***********************************************************************

bool MainFrame::TiXmlSuccess(TiXmlDocument* doc)
{
  if(doc->ErrorId())
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
