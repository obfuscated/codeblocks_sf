/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiNotebook.h"

#include "wxSmithAuiNotebook.h"

#include <wxsadvqppchild.h>
#include <wxwidgets/wxsitemresdata.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wxwidgets/wxsflags.h>

using namespace wxsFlags;

//(*Headers(wxsAuiNotebookParentQP)
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
//*)

//(*InternalHeaders(wxsAuiNotebookParentQP)
#include <wx/string.h>
#include <wx/intl.h>
//*)

namespace
{
    #include "../images/wxsAuiNotebook16.xpm"
    #include "../images/wxsAuiNotebook32.xpm"

    wxsRegisterItem<wxsAuiNotebook> Reg(
        _T("wxAuiNotebook"),                // Class name
        wxsTContainer,                      // Item type
        _T("wxWindows"),                    // License
        _T("Benjamin I. Williams"),         // Author
        _T(""),                             // Author's email
        _T(""),                             // Item's homepage
        _T("Aui"),                          // Category in palette
        50,                                 // Priority in palette
        _T("AuiNotebook"),                  // Base part of names for new items
        wxsCPP,                             // List of coding languages supported by this item
        2, 8,                               // Version
        wxBitmap(wxsAuiNotebook32_xpm),     // 32x32 bitmap
        wxBitmap(wxsAuiNotebook16_xpm),     // 16x16 bitmap
        false);                             // Allow in XRC

    /** \brief Extra parameters for aui notebook's children */
    class wxsAuiNotebookExtra: public wxsPropertyContainer
    {
        public:

            wxsAuiNotebookExtra():
                m_Label(_("Page name")),
                m_Selected(false)
            {}

            wxString m_Label;
            bool m_Selected;
            wxsIconData m_Icon;

        protected:

            virtual void OnEnumProperties(long Flags)
            {
                WXS_SHORT_STRING(wxsAuiNotebookExtra,m_Label,_("Page name"),_T("label"),_T(""),false)
                WXS_BOOL(wxsAuiNotebookExtra,m_Selected,_("Page selected"),_T("selected"),false)
                WXS_ICON(wxsAuiNotebookExtra,m_Icon,_("Page icon"),_T("icon"),wxART_MENU)
            }
    };

    /** \brief Internal Quick properties panel */
    class wxsAuiNotebookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsAuiNotebookParentQP(wxsAdvQPP* parent,wxsAuiNotebookExtra* Extra,wxWindowID id = -1):
                wxsAdvQPPChild(parent,_("AuiNotebook")),
                m_Extra(Extra)
            {
                //(*Initialize(wxsAuiNotebookParentQP)
                Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
                FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
                StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Label"));
                Label = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
                StaticBoxSizer1->Add(Label, 0, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
                FlexGridSizer1->Add(StaticBoxSizer1, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
                StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Selection"));
                Selected = new wxCheckBox(this, ID_CHECKBOX1, _("Selected"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
                Selected->SetValue(false);
                StaticBoxSizer2->Add(Selected, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
                FlexGridSizer1->Add(StaticBoxSizer2, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
                SetSizer(FlexGridSizer1);
                FlexGridSizer1->Fit(this);
                FlexGridSizer1->SetSizeHints(this);

                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsAuiNotebookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsAuiNotebookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsAuiNotebookParentQP::OnLabelKillFocus,0,this);
            }

            virtual ~wxsAuiNotebookParentQP()
            {
                //(*Destroy(wxsAuiNotebookParentQP)
                //*)
            }

        private:

            virtual void Update()
            {
                ReadData();
            }

            void ReadData()
            {
                if ( !GetPropertyContainer() || !m_Extra ) return;
                Label->SetValue(m_Extra->m_Label);
                Selected->SetValue(m_Extra->m_Selected);
            }

            void SaveData()
            {
                if ( !GetPropertyContainer() || !m_Extra ) return;
                m_Extra->m_Label = Label->GetValue();
                m_Extra->m_Selected = Selected->GetValue();
                NotifyChange();
            }

            //(*Identifiers(wxsAuiNotebookParentQP)
            static const long ID_TEXTCTRL1;
            static const long ID_CHECKBOX1;
            //*)

            //(*Handlers(wxsAuiNotebookParentQP)
            void OnLabelText(wxCommandEvent& event);
            void OnLabelKillFocus(wxFocusEvent& event);
            void OnSelectionChange(wxCommandEvent& event);
            //*)

            //(*Declarations(wxsAuiNotebookParentQP)
            wxStaticBoxSizer* StaticBoxSizer2;
            wxTextCtrl* Label;
            wxFlexGridSizer* FlexGridSizer1;
            wxCheckBox* Selected;
            wxStaticBoxSizer* StaticBoxSizer1;
            //*)

            wxsAuiNotebookExtra* m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsAuiNotebookParentQP)
    const long wxsAuiNotebookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsAuiNotebookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsAuiNotebookParentQP,wxPanel)
        //(*EventTable(wxsAuiNotebookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsAuiNotebookParentQP::OnLabelText(wxCommandEvent& event)       { SaveData(); }
    void wxsAuiNotebookParentQP::OnLabelKillFocus(wxFocusEvent& event)    { SaveData(); event.Skip(); }
    void wxsAuiNotebookParentQP::OnSelectionChange(wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsAuiNotebookStyles,_T("wxAUI_NB_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxAuiNotebook")
        WXS_ST(wxAUI_NB_TAB_SPLIT)
        WXS_ST(wxAUI_NB_TAB_MOVE)
        WXS_ST(wxAUI_NB_TAB_EXTERNAL_MOVE)
        WXS_ST(wxAUI_NB_TAB_FIXED_WIDTH)
        WXS_ST(wxAUI_NB_SCROLL_BUTTONS)
        WXS_ST(wxAUI_NB_WINDOWLIST_BUTTON)
        WXS_ST(wxAUI_NB_CLOSE_BUTTON)
        WXS_ST(wxAUI_NB_CLOSE_ON_ACTIVE_TAB)
        WXS_ST(wxAUI_NB_CLOSE_ON_ALL_TABS)
        WXS_ST(wxAUI_NB_TOP)
        WXS_ST(wxAUI_NB_BOTTOM)
        WXS_ST(wxAUI_NB_DEFAULT_STYLE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsAuiNotebookEvents)
        WXS_EVI(EVT_AUINOTEBOOK_PAGE_CLOSE,wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,wxAuiNotebookEvent,PageClose)
        WXS_EVI(EVT_AUINOTEBOOK_PAGE_CLOSED,wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED,wxAuiNotebookEvent,PageClosed)
        WXS_EVI(EVT_AUINOTEBOOK_PAGE_CHANGED,wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,wxAuiNotebookEvent,PageChanged)
        WXS_EVI(EVT_AUINOTEBOOK_PAGE_CHANGING,wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,wxAuiNotebookEvent,PageChanging)
        WXS_EVI(EVT_AUINOTEBOOK_BUTTON,wxEVT_COMMAND_AUINOTEBOOK_BUTTON,wxAuiNotebookEvent,Button)
        WXS_EVI(EVT_AUINOTEBOOK_BEGIN_DRAG,wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG,wxAuiNotebookEvent,BeginDrag)
        WXS_EVI(EVT_AUINOTEBOOK_END_DRAG,wxEVT_COMMAND_AUINOTEBOOK_END_DRAG,wxAuiNotebookEvent,EndDrag)
        WXS_EVI(EVT_AUINOTEBOOK_DRAG_MOTION,wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION,wxAuiNotebookEvent,DragMotion)
        WXS_EVI(EVT_AUINOTEBOOK_DRAG_DONE,wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE,wxAuiNotebookEvent,DragDone)
        WXS_EVI(EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN,wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN,wxAuiNotebookEvent,TabMiddleDown)
        WXS_EVI(EVT_AUINOTEBOOK_TAB_MIDDLE_UP,wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP,wxAuiNotebookEvent,TabMiddleUp)
        WXS_EVI(EVT_AUINOTEBOOK_TAB_RIGHT_DOWN,wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN,wxAuiNotebookEvent,TabRightDown)
        WXS_EVI(EVT_AUINOTEBOOK_TAB_RIGHT_UP,wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP,wxAuiNotebookEvent,TabRightUp)
        WXS_EVI(EVT_AUINOTEBOOK_BG_DCLICK,wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK,wxAuiNotebookEvent,BgDClick)
    WXS_EV_END()

    const long popupNewPageId = wxNewId();
    const long popupPrevPageId = wxNewId();
    const long popupNextPageId = wxNewId();
    const long popupFirstId = wxNewId();
    const long popupLastId = wxNewId();
}

wxsAuiNotebook::wxsAuiNotebook(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsAuiNotebookEvents,
        wxsAuiNotebookStyles),
    m_CurrentSelection(0)
{
    //ctor
}

wxsAuiNotebook::~wxsAuiNotebook()
{
    //dtor
}

void wxsAuiNotebook::OnEnumContainerProperties(long Flags)
{
}

bool wxsAuiNotebook::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if (!Item) return false;

    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
            wxMessageBox(_("Can not add sizer into AuiNotebook.\nAdd panels first."));
        return false;
    }

	return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

wxsPropertyContainer* wxsAuiNotebook::OnBuildExtra()
{
    return new wxsAuiNotebookExtra();
}

wxString wxsAuiNotebook::OnXmlGetExtraObjectClass()
{
    return _T("AuiNotebookPage");
}

void wxsAuiNotebook::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsAuiNotebookExtra* Extra = (wxsAuiNotebookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( Extra )
        QPP->Register(new wxsAuiNotebookParentQP(QPP,Extra),_("AuiNotebook"));
}

wxObject* wxsAuiNotebook::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();
	wxAuiNotebook* AuiNotebook = new wxAuiNotebook(Parent,-1,Pos(Parent),Size(Parent),Style());

	if ( !GetChildCount() && !(PreviewFlags&pfExact) )
	{
	    // Adding additional empty AuiNotebook to prevent from having zero-sized AuiNotebook
	    AuiNotebook->AddPage(
            new wxPanel(AuiNotebook,-1,wxDefaultPosition,wxSize(50,50)),
            _("No pages"));
	}

	AddChildrenPreview(AuiNotebook,PreviewFlags);

	for ( int i=0; i<GetChildCount(); i++ )
	{
	    wxsItem* Child = GetChild(i);
	    wxsAuiNotebookExtra* Extra = (wxsAuiNotebookExtra*)GetChildExtra(i);

	    wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
	    if ( !ChildPreview ) continue;

	    bool Selected = (Child == m_CurrentSelection);
	    if ( PreviewFlags & pfExact ) Selected = Extra->m_Selected;

	    AuiNotebook->AddPage(ChildPreview,Extra->m_Label,Selected,Extra->m_Icon.GetPreview(wxDefaultSize,wxART_OTHER));
	}

	return AuiNotebook;
}

void wxsAuiNotebook::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/aui/aui.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/aui/aui.h>"),_T("wxAuiNotebookEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsAuiNotebookExtra* Extra = (wxsAuiNotebookExtra*)GetChildExtra(i);
                if ( !Extra->m_Icon.IsEmpty() )
                {
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("%AAddPage(%o, %t, %b, %i);\n"),i,Extra->m_Label.wx_str(),Extra->m_Selected,&(Extra->m_Icon),_T("wxART_MENU"));
                    #else
                    Codef(_T("%AAddPage(%o, %t, %b, %i);\n"),i,Extra->m_Label.c_str(),Extra->m_Selected,&(Extra->m_Icon),_T("wxART_MENU"));
                    #endif
                }
                else if ( Extra->m_Selected )
                {
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("%AAddPage(%o, %t, %b);\n"),i,Extra->m_Label.wx_str(),Extra->m_Selected);
                    #else
                    Codef(_T("%AAddPage(%o, %t, %b);\n"),i,Extra->m_Label.c_str(),Extra->m_Selected);
                    #endif
                }
                else
                {
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("%AAddPage(%o, %t);\n"),i,Extra->m_Label.wx_str());
                    #else
                    Codef(_T("%AAddPage(%o, %t);\n"),i,Extra->m_Label.c_str());
                    #endif
                }
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsAuiNotebook::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

bool wxsAuiNotebook::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{
    UpdateCurrentSelection();
    wxSmithAuiNotebook* AuiNotebook = (wxSmithAuiNotebook*)Preview;
    int Hit = wxNOT_FOUND;

    if ( AuiNotebook && GetChildCount() ) Hit = AuiNotebook->HitTest(wxPoint(PosX,PosY));
    if ( Hit != wxNOT_FOUND )
    {
        wxsItem* OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(Hit);
        GetResourceData()->SelectItem(m_CurrentSelection,true);
        return OldSel != m_CurrentSelection;
    }
    return false;
}

bool wxsAuiNotebook::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsAuiNotebook::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsAuiNotebook::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsAuiNotebookExtra* Extra = (wxsAuiNotebookExtra*)GetChildExtra(i);
        if ( (i==0) || (Extra && Extra->m_Selected) )
            NewCurrentSelection = GetChild(i);
    }
    m_CurrentSelection = NewCurrentSelection;
}

void wxsAuiNotebook::OnPreparePopup(wxMenu* Menu)
{
    Menu->Append(popupNewPageId,_("Add new page"));
    Menu->AppendSeparator();
    // This require some extra fixing
    //wxMenuItem* Item1 = Menu->Append(popupPrevPageId,_("Go to previous page"));
    //wxMenuItem* Item2 = Menu->Append(popupNextPageId,_("Go to next page"));
    //Menu->AppendSeparator();
    wxMenuItem* Item3 = Menu->Append(popupFirstId,_("Make current page the first one"));
    wxMenuItem* Item4 = Menu->Append(popupLastId,_("Make current page the last one"));
    if ( !m_CurrentSelection || GetChildIndex(m_CurrentSelection)==0 )
    {
        //Item1->Enable(false);
        Item3->Enable(false);
    }
    if ( !m_CurrentSelection || GetChildIndex(m_CurrentSelection)==GetChildCount()-1 )
    {
        //Item2->Enable(false);
        Item4->Enable(false);
    }
}

bool wxsAuiNotebook::OnPopup(long Id)
{
    if ( Id == popupNewPageId )
    {
        wxTextEntryDialog Dlg(0,_("Enter name of new page"),_("Adding page"),_("New page"));
        if ( Dlg.ShowModal() == wxID_OK )
        {
            wxsItem* Panel = wxsItemFactory::Build(_T("wxPanel"),GetResourceData());
            if ( Panel )
            {
                GetResourceData()->BeginChange();
                if ( AddChild(Panel) )
                {
                    wxsAuiNotebookExtra* Extra = (wxsAuiNotebookExtra*)GetChildExtra(GetChildCount()-1);
                    if ( Extra )
                        Extra->m_Label = Dlg.GetValue();
                    m_CurrentSelection = Panel;
                }
                else
                    delete Panel;
                GetResourceData()->EndChange();
            }
        }
    }
    else if ( Id == popupNextPageId )
    {
        GetResourceData()->BeginChange();
        int Index = GetChildIndex(m_CurrentSelection);
        m_CurrentSelection = GetChild(Index-1);
        UpdateCurrentSelection();
        GetResourceData()->EndChange();
    }
    else if ( Id == popupPrevPageId )
    {
        GetResourceData()->BeginChange();
        int Index = GetChildIndex(m_CurrentSelection);
        m_CurrentSelection = GetChild(Index+1);
        UpdateCurrentSelection();
        GetResourceData()->EndChange();
    }
    else if ( Id == popupFirstId )
    {
        GetResourceData()->BeginChange();
        MoveChild(GetChildIndex(m_CurrentSelection),0);
        GetResourceData()->EndChange();
    }
    else if ( Id == popupLastId )
    {
        GetResourceData()->BeginChange();
        MoveChild(GetChildIndex(m_CurrentSelection),GetChildCount()-1);
        GetResourceData()->EndChange();
    }
    else
        return wxsContainer::OnPopup(Id);

    return true;
}
