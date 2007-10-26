/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2007  olivetti (fabricio.olivetti at gmail dot com)
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsFlatNotebook.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/wxFlatNotebook/renderer.h>
#include <wxsadvqppchild.h>
#include <wxwidgets/wxsitemresdata.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wxwidgets/wxsflags.h>

using namespace wxsFlags;

//(*Headers(wxsNotebookParentQP)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
//*)

//(*InternalHeaders(wxsNotebookParentQP)
#include <wx/intl.h>
#include <wx/string.h>
//*)

namespace
{
    // Loading images from xpm files
    #include "images/flatnote16.xpm"
    #include "images/flatnote32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsFlatNotebook> Reg(
        _T("wxFlatNotebook"),                     // Class name
        wxsTContainer,                            // Item type
        _T("wxWindows"),                       // License
        _T("Eran Ifrah"),                      // Author
        _T("eranif@users.sourceforge.net"),  // Author's email (in real plugin there's no need to do anti-spam tricks ;) )
        _T("http://sourceforge.net/projects/wxflatnotebook"),    // Item's homepage
        _T("Contrib"),                         // Category in palette
        80,                                    // Priority in palette
        _T("FlatNotebook"),                           // Base part of names for new items
        wxsCPP,                                // List of coding languages supported by this item
        1, 0,                                  // Version
        wxBitmap(flatnote32),               // 32x32 bitmap
        wxBitmap(flatnote16),               // 16x16 bitmap
        false);                                // We do not allow this item inside XRC files

    /** \brief Extra parameters for notebook's children */
    class wxsFlatNotebookExtra: public wxsPropertyContainer
    {
        public:

            wxsFlatNotebookExtra():
                m_Label(_("Page name")),
                m_Selected(false)
            {}

            wxString m_Label;
            bool m_Selected;

        protected:

            virtual void OnEnumProperties(long Flags)
            {
                WXS_SHORT_STRING(wxsFlatNotebookExtra,m_Label,_("Page name"),_T("label"),_T(""),false);
                WXS_BOOL(wxsFlatNotebookExtra,m_Selected,_("Page selected"),_T("selected"),false);
            }
    };

        /** \brief Inernal Quick properties panel */

    class wxsFlatNotebookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsFlatNotebookParentQP(wxsAdvQPP* parent,wxsFlatNotebookExtra* Extra,wxWindowID id = -1):
                wxsAdvQPPChild(parent,_("FlatNotebook")),
                m_Extra(Extra)
            {
                //(*Initialize(wxsNotebookParentQP)
                Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
                FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
                StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Label"));
                Label = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
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

                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsFlatNotebookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsFlatNotebookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsFlatNotebookParentQP::OnLabelKillFocus,0,this);
            }

            virtual ~wxsFlatNotebookParentQP()
            {
                //(*Destroy(wxsNotebookParentQP)
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

            //(*Identifiers(wxsNotebookParentQP)
            static const long ID_TEXTCTRL1;
            static const long ID_CHECKBOX1;
            //*)

            //(*Handlers(wxsNotebookParentQP)
            void OnLabelText(wxCommandEvent& event);
            void OnLabelKillFocus(wxFocusEvent& event);
            void OnSelectionChange(wxCommandEvent& event);
            //*)

            //(*Declarations(wxsNotebookParentQP)
            wxStaticBoxSizer* StaticBoxSizer2;
            wxCheckBox* Selected;
            wxTextCtrl* Label;
            wxStaticBoxSizer* StaticBoxSizer1;
            wxFlexGridSizer* FlexGridSizer1;
            //*)

            wxsFlatNotebookExtra* m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsNotebookParentQP)
    const long wxsFlatNotebookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsFlatNotebookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsFlatNotebookParentQP,wxPanel)
        //(*EventTable(wxsNotebookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsFlatNotebookParentQP::OnLabelText(wxCommandEvent& event)       { SaveData(); }
    void wxsFlatNotebookParentQP::OnLabelKillFocus(wxFocusEvent& event)    { SaveData(); event.Skip(); }
    void wxsFlatNotebookParentQP::OnSelectionChange(wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsFlatNotebookStyles,_T("wxFNB_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxFlatNotebook")
        WXS_ST(wxFNB_DEFAULT_STYLE)
        WXS_ST(wxFNB_VC71)
        WXS_ST(wxFNB_FANCY_TABS)
        WXS_ST(wxFNB_TABS_BORDER_SIMPLE)
        WXS_ST(wxFNB_NO_X_BUTTON)
        WXS_ST(wxFNB_NO_NAV_BUTTONS)
        WXS_ST(wxFNB_MOUSE_MIDDLE_CLOSES_TABS)
        WXS_ST(wxFNB_BOTTOM)
        WXS_ST(wxFNB_NODRAG)
        WXS_ST(wxFNB_VC8)
        WXS_ST(wxFNB_X_ON_TAB)
        WXS_ST(wxFNB_BACKGROUND_GRADIENT)
        WXS_ST(wxFNB_COLORFUL_TABS)
        WXS_ST(wxFNB_DCLICK_CLOSES_TABS)
        WXS_ST(wxFNB_SMART_TABS)
        WXS_ST(wxFNB_DROPDOWN_TABS_LIST)
        WXS_ST(wxFNB_ALLOW_FOREIGN_DND)
        WXS_ST(wxFNB_FF2)
        WXS_ST(wxFNB_CUSTOM_DLG)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsFlatNotebookEvents)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxFlatNotebookEvent,PageChanged)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,wxFlatNotebookEvent,PageChanging)
    WXS_EV_END()

    const long popupNewPageId = wxNewId();
    const long popupPrevPageId = wxNewId();
    const long popupNextPageId = wxNewId();
    const long popupFirstId = wxNewId();
    const long popupLastId = wxNewId();


}

wxsFlatNotebook::wxsFlatNotebook(wxsItemResData* Data) : wxsContainer(
        Data,               // Data passed to constructor
        &Reg.Info,          // Info taken from Registering object previously created
        wxsFlatNotebookEvents,
        wxsFlatNotebookStyles),
    m_CurrentSelection(0)
{
    //ctor

}

void wxsFlatNotebook::OnEnumContainerProperties(long Flags)
{
}

bool wxsFlatNotebook::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Can not add sizer into FlatNotebook.\nAdd panels first"));
        }
        return false;
    }

	return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

wxsPropertyContainer* wxsFlatNotebook::OnBuildExtra()
{
    return new wxsFlatNotebookExtra();
}

wxString wxsFlatNotebook::OnXmlGetExtraObjectClass()
{
    return _T("notebookpage");
}

void wxsFlatNotebook::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsFlatNotebookExtra* Extra = (wxsFlatNotebookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( Extra )
    {
        QPP->Register(new wxsFlatNotebookParentQP(QPP,Extra),_("FlatNotebook"));
    }
}

wxObject* wxsFlatNotebook::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();

	wxFlatNotebook* Notebook = new wxFlatNotebook(Parent,GetID(),Pos(Parent),Size(Parent),Style());

	if ( !GetChildCount() && !(PreviewFlags&pfExact) )
	{
	    // Adding additional empty notebook to prevent from having zero-sized notebook
	    Notebook->AddPage(
            new wxPanel(Notebook,GetID(),wxDefaultPosition,wxSize(200,50)),
            _("No pages"));
	}

	AddChildrenPreview(Notebook,PreviewFlags);

	for ( int i=0; i<GetChildCount(); i++ )
	{
	    wxsItem* Child = GetChild(i);
	    wxsFlatNotebookExtra* Extra = (wxsFlatNotebookExtra*)GetChildExtra(i);

	    wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
	    if ( !ChildPreview ) continue;

	    bool Selected = (Child == m_CurrentSelection);
	    if ( PreviewFlags & pfExact ) Selected = Extra->m_Selected;

	    Notebook->AddPage(ChildPreview,Extra->m_Label,Selected);
	}

	return Notebook;
}

void wxsFlatNotebook::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/wxFlatNotebook/wxFlatNotebook.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/wxFlatNotebook/renderer.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W, %I, %P, %S, %T);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsFlatNotebookExtra* Extra = (wxsFlatNotebookExtra*)GetChildExtra(i);
                Codef(_T("%AAddPage(%o, %t, %b);\n"),i,Extra->m_Label.c_str(),Extra->m_Selected);
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsFlatNotebook::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

bool wxsFlatNotebook::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{

    UpdateCurrentSelection();

    wxFlatNotebook* Notebook = (wxFlatNotebook*)Preview;
    wxPageContainer* Tabs = (wxPageContainer*)Notebook->GetTabArea();

    int tabidx;

	// Test whether a left click was made on a tab
	bool bFoundMatch = false;
	wxPageInfoArray PageVec = Tabs->GetPageInfoVector();

	for(int cur=0; cur<Notebook->GetPageCount(); cur++)
	{
		wxPageInfo pgInfo = PageVec[cur];
        // just check the X coordinate, since the Y is also relative
        // to the tabs panel
        // still must fix when the tab is not visible!
		if(Style() & wxFNB_VC8){
            wxRect rect = (PageVec[cur].GetRegion()).GetBox();
            if(PosX > rect.x && PosX < rect.x+rect.width)
            {
                tabidx = cur;
                bFoundMatch = true;
            }
		}else{
            if(PosX > pgInfo.GetPosition().x && PosX < pgInfo.GetPosition().x+pgInfo.GetSize().x)
            {
                tabidx = cur;
                bFoundMatch = true;
            }
		}
	}

    if ( bFoundMatch )
    {
        wxsItem* OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(tabidx);
        GetResourceData()->SelectItem(m_CurrentSelection,true);
        Notebook->SetSelection(tabidx);
        return OldSel != m_CurrentSelection;
    }
    return false;

}

bool wxsFlatNotebook::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsFlatNotebook::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsFlatNotebook::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsFlatNotebookExtra* Extra = (wxsFlatNotebookExtra*)GetChildExtra(i);
        if ( (i==0) || Extra->m_Selected )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}
void wxsFlatNotebook::OnPreparePopup(wxMenu* Menu)
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

bool wxsFlatNotebook::OnPopup(long Id)
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
                    wxsFlatNotebookExtra* Extra = (wxsFlatNotebookExtra*)GetChildExtra(GetChildCount()-1);
                    if ( Extra )
                    {
                        Extra->m_Label = Dlg.GetValue();
                    }
                    m_CurrentSelection = Panel;
                }
                else
                {
                    delete Panel;
                }
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
    {
        return wxsContainer::OnPopup(Id);
    }
    return true;
}
/*
wxsFlatNotebook::~wxsFlatNotebook()
{
    //dtor
    for ( int i=0; i<GetChildCount(); i++ )
    {
        delete (wxsFlatNotebookExtra*)GetChildExtra(i);
    }
   //delete this;
}
*/


