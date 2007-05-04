/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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

#include "wxslistbook.h"
#include "../../wxsadvqppchild.h"
#include <wx/listbook.h>

//(*Headers(wxsListbookParentQP)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//*)

//(*InternalHeaders(wxsListbookParentQP)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

// TODO: Add images
namespace
{
    wxsRegisterItem<wxsListbook> Reg(_T("Listbook"),wxsTContainer,_T("Standard"),61);

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

            virtual void OnEnumProperties(long Flags)
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
                Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("id"));
                FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
                StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Label"));
                Label = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
                StaticBoxSizer1->Add(Label,0,wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
                FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
                StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Selection"));
                Selected = new wxCheckBox(this,ID_CHECKBOX1,_("Selected"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
                StaticBoxSizer2->Add(Selected,1,wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
                FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
                SetSizer(FlexGridSizer1);
                FlexGridSizer1->Fit(this);
                FlexGridSizer1->SetSizeHints(this);
                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsListbookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsListbookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsListbookParentQP::OnLabelKillFocus,NULL,this);
            }

            virtual ~wxsListbookParentQP() {}

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
            wxFlexGridSizer* FlexGridSizer1;
            wxStaticBoxSizer* StaticBoxSizer1;
            wxTextCtrl* Label;
            wxStaticBoxSizer* StaticBoxSizer2;
            wxCheckBox* Selected;
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

    void wxsListbookParentQP::OnLabelText(wxCommandEvent& event)       { SaveData(); }
    void wxsListbookParentQP::OnLabelKillFocus(wxFocusEvent& event)    { SaveData(); event.Skip(); }
    void wxsListbookParentQP::OnSelectionChange(wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsListbookStyles,_T(""))
        WXS_ST_CATEGORY("wxListbook")
        WXS_ST(wxLB_DEFAULT)
        WXS_ST(wxLB_LEFT)
        WXS_ST(wxLB_RIGHT)
        WXS_ST(wxLB_TOP)
        WXS_ST(wxLB_BOTTOM)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsListbookEvents)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxNotebookEvent,PageChanged)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,wxNotebookEvent,PageChanging)
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

void wxsListbook::OnEnumContainerProperties(long Flags)
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
    wxsListbookExtra* Extra = (wxsListbookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( Extra )
    {
        QPP->Register(new wxsListbookParentQP(QPP,Extra),_("Listbook"));
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
	    wxsListbookExtra* Extra = (wxsListbookExtra*)GetChildExtra(i);

	    wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
	    if ( !ChildPreview ) continue;

	    bool Selected = (Child == m_CurrentSelection);
	    if ( PreviewFlags & pfExact ) Selected = Extra->m_Selected;

	    Listbook->AddPage(ChildPreview,Extra->m_Label,Selected);
	}

	return Listbook;
}

void wxsListbook::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%P,%S,%T,%N);\n"));
            SetupWindowCode(Code,WindowParent,wxsCPP);
            AddChildrenCode(Code,wxsCPP);

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsListbookExtra* Extra = (wxsListbookExtra*)GetChildExtra(i);
                Code << Codef(Language,_T("%AAddPage(%v,%t,%b);\n"),
                    GetChild(i)->GetVarName().c_str(),Extra->m_Label.c_str(),Extra->m_Selected);
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsListbook::OnBuildCreatingCode"),Language);
        }
    }
}

void wxsListbook::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/listbook.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsListbook::OnEnumDeclFiles"),Language);
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
    wxsItem* NewCurrentSelection = NULL;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsListbookExtra* Extra = (wxsListbookExtra*)GetChildExtra(i);
        if ( (i==0) || Extra->m_Selected )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}
