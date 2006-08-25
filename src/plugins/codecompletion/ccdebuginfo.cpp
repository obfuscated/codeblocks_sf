#include <sdk.h>
#include "ccdebuginfo.h"
#include "parser/parser.h"
#include <wx/utils.h>
#include <wx/choicdlg.h> // wxGetSingleChoiceIndex

BEGIN_EVENT_TABLE(CCDebugInfo,wxDialog)
	//(*EventTable(CCDebugInfo)
	EVT_INIT_DIALOG(CCDebugInfo::OnInit)
	EVT_BUTTON(ID_BUTTON1,CCDebugInfo::OnFindClick)
	EVT_BUTTON(ID_BUTTON4,CCDebugInfo::OnGoParentClick)
	EVT_BUTTON(ID_BUTTON5,CCDebugInfo::OnGoChildrenClick)
	EVT_BUTTON(ID_BUTTON3,CCDebugInfo::OnGoAscClick)
	EVT_BUTTON(ID_BUTTON2,CCDebugInfo::OnGoDescClick)
	//*)
END_EVENT_TABLE()

CCDebugInfo::CCDebugInfo(wxWindow* parent, Parser* parser, Token* token)
    : m_pParser(parser),
    m_pToken(token)
{
    int id = -1;
	//(*Initialize(CCDebugInfo)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;

	Create(parent,id,_("Code-completion debug tool"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Notebook1 = new wxNotebook(this,ID_NOTEBOOK1,wxDefaultPosition,wxDefaultSize,0,_("ID_NOTEBOOK1"));
	Panel1 = new wxPanel(Notebook1,ID_PANEL1,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL1"));
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText29 = new wxStaticText(Panel1,ID_STATICTEXT29,_("Find:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT29"));
	txtFilter = new wxTextCtrl(Panel1,ID_TEXTCTRL1,_("*"),wxDefaultPosition,wxSize(401,21),0,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) txtFilter->SetMaxLength(0);
	btnFind = new wxButton(Panel1,ID_BUTTON1,_("Find"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON1"));
	if (true) btnFind->SetDefault();
	BoxSizer4->Add(StaticText29,0,wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,6);
	BoxSizer4->Add(txtFilter,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer4->Add(btnFind,0,wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticLine1 = new wxStaticLine(Panel1,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),0,_("ID_STATICLINE1"));
	FlexGridSizer1 = new wxFlexGridSizer(0,2,5,5);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText17 = new wxStaticText(Panel1,ID_STATICTEXT17,_("ID:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT17"));
	txtID = new wxStaticText(Panel1,ID_STATICTEXT18,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT18"));
	StaticText1 = new wxStaticText(Panel1,ID_STATICTEXT1,_("Name:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	txtName = new wxStaticText(Panel1,ID_STATICTEXT2,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT2"));
	StaticText9 = new wxStaticText(Panel1,ID_STATICTEXT9,_("Kind:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT9"));
	txtKind = new wxStaticText(Panel1,ID_STATICTEXT10,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT10"));
	StaticText11 = new wxStaticText(Panel1,ID_STATICTEXT11,_("Scope:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT11"));
	txtScope = new wxStaticText(Panel1,ID_STATICTEXT12,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT12"));
	StaticText3 = new wxStaticText(Panel1,ID_STATICTEXT3,_("Type:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT3"));
	txtType = new wxStaticText(Panel1,ID_STATICTEXT4,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(Panel1,ID_STATICTEXT5,_("Actual type:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT5"));
	txtActualType = new wxStaticText(Panel1,ID_STATICTEXT6,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT6"));
	StaticText7 = new wxStaticText(Panel1,ID_STATICTEXT7,_("Arguments:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT7"));
	txtArgs = new wxStaticText(Panel1,ID_STATICTEXT8,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT8"));
	StaticText13 = new wxStaticText(Panel1,ID_STATICTEXT13,_("Is operator\?"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT13"));
	txtIsOp = new wxStaticText(Panel1,ID_STATICTEXT14,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT14"));
	StaticText15 = new wxStaticText(Panel1,ID_STATICTEXT15,_("Is local\?"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT15"));
	txtIsLocal = new wxStaticText(Panel1,ID_STATICTEXT16,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT16"));
	StaticText32 = new wxStaticText(Panel1,ID_STATICTEXT32,_("Is temp\?"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT32"));
	txtIsTemp = new wxStaticText(Panel1,ID_STATICTEXT33,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT33"));
	StaticText19 = new wxStaticText(Panel1,ID_STATICTEXT19,_("Namespace:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT19"));
	txtNamespace = new wxStaticText(Panel1,ID_STATICTEXT20,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT20"));
	StaticText22 = new wxStaticText(Panel1,ID_STATICTEXT22,_("Parent:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT22"));
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	txtParent = new wxStaticText(Panel1,ID_STATICTEXT24,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT24"));
	btnGoParent = new wxButton(Panel1,ID_BUTTON4,_("Go"),wxDefaultPosition,wxSize(36,23),0,wxDefaultValidator,_("ID_BUTTON4"));
	if (false) btnGoParent->SetDefault();
	BoxSizer7->Add(txtParent,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer7->Add(btnGoParent,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	StaticText30 = new wxStaticText(Panel1,ID_STATICTEXT30,_("Children:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT30"));
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	cmbChildren = new wxComboBox(Panel1,ID_COMBOBOX3,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX3"));
	btnGoChildren = new wxButton(Panel1,ID_BUTTON5,_("Go"),wxDefaultPosition,wxSize(36,23),0,wxDefaultValidator,_("ID_BUTTON5"));
	if (false) btnGoChildren->SetDefault();
	BoxSizer8->Add(cmbChildren,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer8->Add(btnGoChildren,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	StaticText21 = new wxStaticText(Panel1,ID_STATICTEXT21,_("Ancestors:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT21"));
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	cmbAncestors = new wxComboBox(Panel1,ID_COMBOBOX2,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX2"));
	btnGoAsc = new wxButton(Panel1,ID_BUTTON3,_("Go"),wxDefaultPosition,wxSize(36,23),0,wxDefaultValidator,_("ID_BUTTON3"));
	if (false) btnGoAsc->SetDefault();
	BoxSizer6->Add(cmbAncestors,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer6->Add(btnGoAsc,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	StaticText23 = new wxStaticText(Panel1,ID_STATICTEXT23,_("Descendants:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT23"));
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	cmbDescendants = new wxComboBox(Panel1,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX1"));
	btnGoDesc = new wxButton(Panel1,ID_BUTTON2,_("Go"),wxDefaultPosition,wxSize(36,23),0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) btnGoDesc->SetDefault();
	BoxSizer5->Add(cmbDescendants,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	BoxSizer5->Add(btnGoDesc,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	StaticText25 = new wxStaticText(Panel1,ID_STATICTEXT25,_("Decl. filename:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT25"));
	txtDeclFile = new wxStaticText(Panel1,ID_STATICTEXT26,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT26"));
	StaticText27 = new wxStaticText(Panel1,ID_STATICTEXT27,_("Impl. filename:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT27"));
	txtImplFile = new wxStaticText(Panel1,ID_STATICTEXT28,_T(""),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT28"));
	FlexGridSizer1->Add(StaticText17,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtID,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtName,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText9,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtKind,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText11,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtScope,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText3,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtType,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText5,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtActualType,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText7,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtArgs,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText13,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtIsOp,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText15,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtIsLocal,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText32,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtIsTemp,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText19,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtNamespace,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText22,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer1->Add(BoxSizer7,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	FlexGridSizer1->Add(StaticText30,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer1->Add(BoxSizer8,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	FlexGridSizer1->Add(StaticText21,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer1->Add(BoxSizer6,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	FlexGridSizer1->Add(StaticText23,0,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,0);
	FlexGridSizer1->Add(BoxSizer5,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	FlexGridSizer1->Add(StaticText25,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtDeclFile,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(StaticText27,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	FlexGridSizer1->Add(txtImplFile,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	BoxSizer2->Add(BoxSizer4,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	BoxSizer2->Add(StaticLine1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	BoxSizer2->Add(FlexGridSizer1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	Panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(Panel1);
	BoxSizer2->SetSizeHints(Panel1);
	Panel2 = new wxPanel(Notebook1,ID_PANEL2,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL2"));
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	lstFiles = new wxListBox(Panel2,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_("ID_LISTBOX1"));
	lstFiles->SetSelection(-1);
	BoxSizer3->Add(lstFiles,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	Panel2->SetSizer(BoxSizer3);
	BoxSizer3->Fit(Panel2);
	BoxSizer3->SetSizeHints(Panel2);
	Notebook1->AddPage(Panel1,_("Tokens"),false);
	Notebook1->AddPage(Panel2,_("Files list"),false);
	lblInfo = new wxStaticText(this,ID_STATICTEXT31,_("The parser contains 0 tokens found in 0 files"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT31"));
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxSize(10,-1),0,_("ID_STATICLINE2"));
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,_("Close")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(Notebook1,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	BoxSizer1->Add(lblInfo,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	BoxSizer1->Add(StaticLine2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(StdDialogButtonSizer1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)
}

CCDebugInfo::~CCDebugInfo()
{
}


void CCDebugInfo::FillFiles()
{
    lstFiles->Freeze();
    lstFiles->Clear();

    TokensTree* tokens = m_pParser->GetTokens();
    for (size_t i = 0; i < tokens->m_FilenamesMap.size(); ++i)
    {
        wxString file = tokens->m_FilenamesMap.GetString(i);
        if (!file.IsEmpty())
            lstFiles->Append(file);
    }

    lstFiles->Thaw();
}

void CCDebugInfo::DisplayTokenInfo()
{
    if (!m_pToken)
    {
        txtID->SetLabel(wxEmptyString);
        txtName->SetLabel(wxEmptyString);
        txtKind->SetLabel(wxEmptyString);
        txtScope->SetLabel(wxEmptyString);
        txtType->SetLabel(wxEmptyString);
        txtActualType->SetLabel(wxEmptyString);
        txtArgs->SetLabel(wxEmptyString);
        txtIsOp->SetLabel(wxEmptyString);
        txtIsLocal->SetLabel(wxEmptyString);
        txtNamespace->SetLabel(wxEmptyString);
        txtParent->SetLabel(wxEmptyString);
        cmbChildren->Clear();
        cmbAncestors->Clear();
        cmbDescendants->Clear();
        txtDeclFile->SetLabel(wxEmptyString);
        txtImplFile->SetLabel(wxEmptyString);
        return;
    }

    TokensTree* tokens = m_pParser->GetTokens();
    Token* parent = tokens->at(m_pToken->m_ParentIndex);

    wxString args = m_pToken->m_Args;
    wxString ttype = m_pToken->m_Type;

    // so they can be displayed in wxStaticText
    args.Replace(_T("&"), _T("&&"), true);
    ttype.Replace(_T("&"), _T("&&"), true);

    txtID->SetLabel(wxString::Format(_T("%d"), m_pToken->GetSelf()));
    txtName->SetLabel(m_pToken->m_Name);
    txtKind->SetLabel(m_pToken->GetTokenKindString());
    txtScope->SetLabel(m_pToken->GetTokenScopeString());
    txtType->SetLabel(ttype);
    txtActualType->SetLabel(m_pToken->m_ActualType);
    txtArgs->SetLabel(args);
    txtIsOp->SetLabel(m_pToken->m_IsOperator ? _T("Yes") : _T("No"));
    txtIsLocal->SetLabel(m_pToken->m_IsLocal ? _T("Yes") : _T("No"));
    txtIsTemp->SetLabel(m_pToken->m_IsTemp ? _T("Yes") : _T("No"));
    txtNamespace->SetLabel(m_pToken->GetNamespace());
    txtParent->SetLabel(wxString::Format(_T("%s (%d)"), parent ? parent->m_Name.c_str() : _T("<Global namespace>"), m_pToken->m_ParentIndex));
    FillChildren();
    FillAncestors();
    FillDescendants();
    if (!m_pToken->GetFilename().IsEmpty())
        txtDeclFile->SetLabel(wxString::Format(_T("%s : %d"), m_pToken->GetFilename().c_str(), m_pToken->m_Line));
    else
        txtDeclFile->SetLabel(wxEmptyString);
    if (!m_pToken->GetImplFilename().IsEmpty())
        txtImplFile->SetLabel(wxString::Format(_T("%s : %d"), m_pToken->GetImplFilename().c_str(), m_pToken->m_ImplLine));
    else
        txtImplFile->SetLabel(wxEmptyString);
}

void CCDebugInfo::FillChildren()
{
    TokensTree* tokens = m_pParser->GetTokens();
    cmbChildren->Clear();
    for (TokenIdxSet::iterator it = m_pToken->m_Children.begin(); it != m_pToken->m_Children.end(); ++it)
    {
        Token* child = tokens->at(*it);
        cmbChildren->Append(wxString::Format(_T("%s (%d)"), child ? child->m_Name.c_str() : _T("<invalid token>"), *it));
    }
    cmbChildren->SetSelection(0);
}

void CCDebugInfo::FillAncestors()
{
    TokensTree* tokens = m_pParser->GetTokens();
    cmbAncestors->Clear();
    for (TokenIdxSet::iterator it = m_pToken->m_Ancestors.begin(); it != m_pToken->m_Ancestors.end(); ++it)
    {
        Token* ancestor = tokens->at(*it);
        cmbAncestors->Append(wxString::Format(_T("%s (%d)"), ancestor ? ancestor->m_Name.c_str() : _T("<invalid token>"), *it));
    }
    cmbAncestors->SetSelection(0);
}

void CCDebugInfo::FillDescendants()
{
    TokensTree* tokens = m_pParser->GetTokens();
    cmbDescendants->Clear();
    for (TokenIdxSet::iterator it = m_pToken->m_Descendants.begin(); it != m_pToken->m_Descendants.end(); ++it)
    {
        Token* descendant = tokens->at(*it);
        cmbDescendants->Append(wxString::Format(_T("%s (%d)"), descendant ? descendant->m_Name.c_str() : _T("<invalid token>"), *it));
    }
    cmbDescendants->SetSelection(0);
}

void CCDebugInfo::OnInit(wxInitDialogEvent& event)
{
    if (!m_pParser)
        return;

    wxBusyCursor busy;

    lblInfo->SetLabel(wxString::Format(_T("The parser contains %d tokens, found in %d files"), m_pParser->GetTokens()->size(), m_pParser->GetFilesCount()));
    DisplayTokenInfo();
    FillFiles();

    txtFilter->SetFocus();
}

void CCDebugInfo::OnFindClick(wxCommandEvent& event)
{
    TokensTree* tokens = m_pParser->GetTokens();
    wxString search = txtFilter->GetValue();

    m_pToken = 0;

    // first determine if the user entered an ID or a search mask
    long unsigned id;
    if (search.ToULong(&id, 10))
    {
        // easy; ID
        m_pToken = tokens->at(id);
    }
    else
    {
        // find all matching tokens
        TokenIdxSet result;
        for (size_t i = 0; i < tokens->size(); ++i)
        {
            Token* token = tokens->at(i);
            if (token && token->m_Name.Matches(search))
                result.insert(i);
        }

        // a single result?
        if (result.size() == 1)
        {
            m_pToken = tokens->at(*(result.begin()));
        }
        else
        {
            // fill a list and ask the user which token to display
            wxArrayString arr;
            wxArrayInt intarr;
            for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
            {
                Token* token = tokens->at(*it);
                arr.Add(token->DisplayName());
                intarr.Add(*it);
            }
            int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), arr);
            if (sel == -1)
                return;
            m_pToken = tokens->at(intarr[sel]);
        }
    }

    DisplayTokenInfo();
}

void CCDebugInfo::OnGoAscClick(wxCommandEvent& event)
{
    int idx = cmbAncestors->GetSelection();
    if (!m_pToken || idx == -1)
        return;

    TokensTree* tokens = m_pParser->GetTokens();
    int count = 0;
    for (TokenIdxSet::iterator it = m_pToken->m_Ancestors.begin(); it != m_pToken->m_Ancestors.end(); ++it)
    {
        if (count == idx)
        {
            m_pToken = tokens->at(*it);
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}

void CCDebugInfo::OnGoDescClick(wxCommandEvent& event)
{
    int idx = cmbDescendants->GetSelection();
    if (!m_pToken || idx == -1)
        return;

    TokensTree* tokens = m_pParser->GetTokens();
    int count = 0;
    for (TokenIdxSet::iterator it = m_pToken->m_Descendants.begin(); it != m_pToken->m_Descendants.end(); ++it)
    {
        if (count == idx)
        {
            m_pToken = tokens->at(*it);
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}

void CCDebugInfo::OnGoParentClick(wxCommandEvent& event)
{
    if (!m_pToken || m_pToken->m_ParentIndex == -1)
        return;
    m_pToken = m_pParser->GetTokens()->at(m_pToken->m_ParentIndex);
    DisplayTokenInfo();
}

void CCDebugInfo::OnGoChildrenClick(wxCommandEvent& event)
{
    int idx = cmbChildren->GetSelection();
    if (!m_pToken || idx == -1)
        return;

    TokensTree* tokens = m_pParser->GetTokens();
    int count = 0;
    for (TokenIdxSet::iterator it = m_pToken->m_Children.begin(); it != m_pToken->m_Children.end(); ++it)
    {
        if (count == idx)
        {
            m_pToken = tokens->at(*it);
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}
