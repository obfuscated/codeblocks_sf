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

#include "wxschoicebook.h"
#include "../../wxsadvqppchild.h"
#include <wx/choicebk.h>

//(*Headers(wxsChoicebookParentQP)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//*)

//(*InternalHeaders(wxsChoicebookParentQP)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

namespace
{
    wxsRegisterItem<wxsChoicebook> Reg(_T("Choicebook"),wxsTContainer,_T("Standard"),61);

    /** \brief Extra parameters for notebook's children */
    class wxsChoicebookExtra: public wxsPropertyContainer
    {
        public:

            wxsChoicebookExtra():
                m_Label(_("Page name")),
                m_Selected(false)
            {}

            wxString m_Label;
            bool m_Selected;

        protected:

            virtual void OnEnumProperties(long Flags)
            {
                WXS_SHORT_STRING(wxsChoicebookExtra,m_Label,_("Page name"),_T("label"),_T(""),false);
                WXS_BOOL(wxsChoicebookExtra,m_Selected,_("Page selected"),_T("selected"),false);
            }
    };

    /** \brief Inernal Quick properties panel */
    class wxsChoicebookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsChoicebookParentQP(wxsAdvQPP* parent,wxsChoicebookExtra* Extra,wxWindowID id = -1):
                wxsAdvQPPChild(parent,_("Choicebook")),
                m_Extra(Extra)
            {
                //(*Initialize(wxsChoicebookParentQP)
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
                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsChoicebookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsChoicebookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsChoicebookParentQP::OnLabelKillFocus,0,this);
            }

            virtual ~wxsChoicebookParentQP()
            {
                //(*Destroy(wxsChoicebookParentQP)
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

            //(*Identifiers(wxsChoicebookParentQP)
            static const long ID_TEXTCTRL1;
            static const long ID_CHECKBOX1;
            //*)

            //(*Handlers(wxsChoicebookParentQP)
            void OnLabelText(wxCommandEvent& event);
            void OnLabelKillFocus(wxFocusEvent& event);
            void OnSelectionChange(wxCommandEvent& event);
            //*)

            //(*Declarations(wxsChoicebookParentQP)
            wxFlexGridSizer* FlexGridSizer1;
            wxStaticBoxSizer* StaticBoxSizer1;
            wxTextCtrl* Label;
            wxStaticBoxSizer* StaticBoxSizer2;
            wxCheckBox* Selected;
            //*)

            wxsChoicebookExtra* m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsChoicebookParentQP)
    const long wxsChoicebookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsChoicebookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsChoicebookParentQP,wxPanel)
        //(*EventTable(wxsChoicebookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsChoicebookParentQP::OnLabelText(wxCommandEvent& event)       { SaveData(); }
    void wxsChoicebookParentQP::OnLabelKillFocus(wxFocusEvent& event)    { SaveData(); event.Skip(); }
    void wxsChoicebookParentQP::OnSelectionChange(wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsChoicebookStyles,_T(""))
        WXS_ST_CATEGORY("wxChoicebook")
        // TODO: XRC in wx 2.8 uses wxBK_ prefix, docs stay at wxCHB_
        WXS_ST(wxCHB_DEFAULT)
        WXS_ST(wxCHB_LEFT)
        WXS_ST(wxCHB_RIGHT)
        WXS_ST(wxCHB_TOP)
        WXS_ST(wxCHB_BOTTOM)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsChoicebookEvents)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxNotebookEvent,PageChanged)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,wxNotebookEvent,PageChanging)
    WXS_EV_END()

}


wxsChoicebook::wxsChoicebook(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsChoicebookEvents,
        wxsChoicebookStyles),
    m_CurrentSelection(0)
{
}

void wxsChoicebook::OnEnumContainerProperties(long Flags)
{
}

bool wxsChoicebook::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Can not add sizer into Choicebook.\nAdd panels first"));
        }
        return false;
    }

	return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

wxsPropertyContainer* wxsChoicebook::OnBuildExtra()
{
    return new wxsChoicebookExtra();
}

wxString wxsChoicebook::OnXmlGetExtraObjectClass()
{
    return _T("choicebookpage");
}

void wxsChoicebook::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsChoicebookExtra* Extra = (wxsChoicebookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( Extra )
    {
        QPP->Register(new wxsChoicebookParentQP(QPP,Extra),_("Choicebook"));
    }
}

wxObject* wxsChoicebook::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();
	wxChoicebook* Choicebook = new wxChoicebook(Parent,-1,Pos(Parent),Size(Parent),Style());

	if ( !GetChildCount() && !(PreviewFlags&pfExact) )
	{
	    // Adding additional empty notebook to prevent from having zero-sized notebook
	    Choicebook->AddPage(
            new wxPanel(Choicebook,-1,wxDefaultPosition,wxSize(50,50)),
            _("No pages"));
	}

	AddChildrenPreview(Choicebook,PreviewFlags);

	for ( int i=0; i<GetChildCount(); i++ )
	{
	    wxsItem* Child = GetChild(i);
	    wxsChoicebookExtra* Extra = (wxsChoicebookExtra*)GetChildExtra(i);

	    wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
	    if ( !ChildPreview ) continue;

	    bool Selected = (Child == m_CurrentSelection);
	    if ( PreviewFlags & pfExact ) Selected = Extra->m_Selected;

	    Choicebook->AddPage(ChildPreview,Extra->m_Label,Selected);
	}

	return Choicebook;
}

void wxsChoicebook::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            SetupWindowCode(Code,WindowParent,wxsCPP);
            AddChildrenCode(Code,wxsCPP);

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsChoicebookExtra* Extra = (wxsChoicebookExtra*)GetChildExtra(i);
                Code << Codef(Language,_T("%AAddPage(%v, %t, %b);\n"),
                    GetChild(i)->GetVarName().c_str(),Extra->m_Label.c_str(),Extra->m_Selected);
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsChoicebook::OnBuildCreatingCode"),Language);
        }
    }
}

void wxsChoicebook::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/choicebk.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsChoicebook::OnEnumDeclFiles"),Language);
    }
}

bool wxsChoicebook::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{
    UpdateCurrentSelection();
    if ( GetChildCount()<2 ) return false;
    int NewIndex = GetChildIndex(m_CurrentSelection)+1;
    if ( NewIndex >= GetChildCount() ) NewIndex = 0;
    m_CurrentSelection = GetChild(NewIndex);
    return true;
}

bool wxsChoicebook::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsChoicebook::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsChoicebook::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsChoicebookExtra* Extra = (wxsChoicebookExtra*)GetChildExtra(i);
        if ( (i==0) || Extra->m_Selected )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}
