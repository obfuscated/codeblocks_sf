/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxslistbook.h"
#include "../../wxsadvqppchild.h"
#include "../wxsitemresdata.h"
#include <wx/listbook.h>

//(*Headers(wxsListbookParentQP)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
//*)

//(*InternalHeaders(wxsListbookParentQP)
#include <wx/intl.h>
#include <wx/string.h>
//*)

// TODO: Add images
namespace
{
    wxsRegisterItem<wxsListbook> Reg(_T("Listbook"),wxsTContainer,_T("Standard"),250);

    /** \brief Extra parameters for notebook's children */
    class wxsListbookExtra: public wxsPropertyContainer
    {
        public:

            wxsListbookExtra():
                m_Label(_("Page name")),
                m_Selected(false)
            {}

            wxString m_Label;
            bool m_Selected;

        protected:

            virtual void OnEnumProperties(cb_unused long Flags)
            {
                WXS_SHORT_STRING(wxsListbookExtra,m_Label,_("Page name"),_T("label"),_T(""),false);
                WXS_BOOL(wxsListbookExtra,m_Selected,_("Page selected"),_T("selected"),false);
            }
    };

    /** \brief Inernal Quick properties panel */
    class wxsListbookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsListbookParentQP(wxsAdvQPP* parent,wxsListbookExtra* Extra,wxWindowID id = -1):
                wxsAdvQPPChild(parent,_("Listbook")),
                m_Extra(Extra)
            {
                //(*Initialize(wxsListbookParentQP)
                wxStaticBoxSizer* StaticBoxSizer2;
                wxStaticBoxSizer* StaticBoxSizer1;
                wxFlexGridSizer* FlexGridSizer1;

                Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
                FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
                StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Label"));
                Label = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
                StaticBoxSizer1->Add(Label, 0, wxEXPAND, 5);
                FlexGridSizer1->Add(StaticBoxSizer1, 1, wxEXPAND, 5);
                StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Selection"));
                Selected = new wxCheckBox(this, ID_CHECKBOX1, _("Selected"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
                Selected->SetValue(false);
                StaticBoxSizer2->Add(Selected, 1, wxEXPAND, 5);
                FlexGridSizer1->Add(StaticBoxSizer2, 1, wxEXPAND, 5);
                SetSizer(FlexGridSizer1);
                FlexGridSizer1->Fit(this);
                FlexGridSizer1->SetSizeHints(this);

                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(wxsListbookParentQP::OnLabelText));
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(wxsListbookParentQP::OnSelectionChange));
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsListbookParentQP::OnLabelKillFocus,0,this);
            }

            virtual ~wxsListbookParentQP()
            {
                //(*Destroy(wxsListbookParentQP)
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

            //(*Identifiers(wxsListbookParentQP)
            static const long ID_TEXTCTRL1;
            static const long ID_CHECKBOX1;
            //*)

            //(*Handlers(wxsListbookParentQP)
            void OnLabelText(wxCommandEvent& event);
            void OnLabelKillFocus(wxFocusEvent& event);
            void OnSelectionChange(wxCommandEvent& event);
            //*)

            //(*Declarations(wxsListbookParentQP)
            wxCheckBox* Selected;
            wxTextCtrl* Label;
            //*)

            wxsListbookExtra* m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsListbookParentQP)
    const long wxsListbookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsListbookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsListbookParentQP,wxPanel)
        //(*EventTable(wxsListbookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsListbookParentQP::OnLabelText(cb_unused wxCommandEvent& event)       { SaveData(); }
    void wxsListbookParentQP::OnLabelKillFocus(wxFocusEvent& event)              { SaveData(); event.Skip(); }
    void wxsListbookParentQP::OnSelectionChange(cb_unused wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsListbookStyles,_T(""))
        WXS_ST_CATEGORY("wxListbook")
        WXS_ST(wxLB_DEFAULT)
        WXS_ST(wxLB_LEFT)
        WXS_ST(wxLB_RIGHT)
        WXS_ST(wxLB_TOP)
        WXS_ST(wxLB_BOTTOM)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsListbookEvents)
        WXS_EVI(EVT_LISTBOOK_PAGE_CHANGED,wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,wxListbookEvent,PageChanged)
        WXS_EVI(EVT_LISTBOOK_PAGE_CHANGING,wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING,wxListbookEvent,PageChanging)
    WXS_EV_END()

}


wxsListbook::wxsListbook(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsListbookEvents,
        wxsListbookStyles),
    m_CurrentSelection(0)
{
}

void wxsListbook::OnEnumContainerProperties(cb_unused long Flags)
{
}

bool wxsListbook::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Can not add sizer into Listbook.\nAdd panels first"));
        }
        return false;
    }

    return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

wxsPropertyContainer* wxsListbook::OnBuildExtra()
{
    return new wxsListbookExtra();
}

wxString wxsListbook::OnXmlGetExtraObjectClass()
{
    return _T("listbookpage");
}

void wxsListbook::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsListbookExtra* LBExtra = (wxsListbookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( LBExtra )
    {
        QPP->Register(new wxsListbookParentQP(QPP,LBExtra),_("Listbook"));
    }
}

wxObject* wxsListbook::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();
    wxListbook* Listbook = new wxListbook(Parent,-1,Pos(Parent),Size(Parent),Style());

    if ( !GetChildCount() && !(PreviewFlags&pfExact) )
    {
        // Adding additional empty notebook to prevent from having zero-sized notebook
        Listbook->AddPage(
            new wxPanel(Listbook,-1,wxDefaultPosition,wxSize(50,50)),
            _("No pages"));
    }

    AddChildrenPreview(Listbook,PreviewFlags);

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsListbookExtra* LBExtra = (wxsListbookExtra*)GetChildExtra(i);

        wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
        if ( !ChildPreview ) continue;

        bool Selected = (Child == m_CurrentSelection);
        if ( PreviewFlags & pfExact ) Selected = LBExtra->m_Selected;

        Listbook->AddPage(ChildPreview,LBExtra->m_Label,Selected);
    }

    return Listbook;
}

void wxsListbook::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/listbook.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/notebook.h>"),_T("wxNotebookEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsListbookExtra* LBExtra = (wxsListbookExtra*)GetChildExtra(i);
                Codef(_T("%AAddPage(%o, %t, %b);\n"),i,LBExtra->m_Label.wx_str(),LBExtra->m_Selected);
            }

            break;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsListbook::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

bool wxsListbook::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{
    UpdateCurrentSelection();
    wxListbook* Listbook = (wxListbook*)Preview;
    // for some reason, HitTest() is public in wxBookCtrlBase but they chose to make it protected in wxListbook...
    int Hit = ((wxBookCtrlBase*)Listbook)->HitTest(wxPoint(PosX,PosY));
    if ( Hit != wxNOT_FOUND )
    {
        wxsItem* OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(Hit);
        GetResourceData()->SelectItem(m_CurrentSelection,true);
        return OldSel != m_CurrentSelection;
    }
    return false;
}

bool wxsListbook::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsListbook::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsListbook::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsListbookExtra* LBExtra = (wxsListbookExtra*)GetChildExtra(i);
        if ( (i==0) || LBExtra->m_Selected )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}
