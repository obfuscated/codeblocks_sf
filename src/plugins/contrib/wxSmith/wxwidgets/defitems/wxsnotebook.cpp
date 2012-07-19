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

#include "wxsnotebook.h"
#include "../../wxsadvqppchild.h"
#include "../wxsitemresdata.h"
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include "../wxsflags.h"

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

// TODO: Add notebook images
namespace
{
    wxsRegisterItem<wxsNotebook> Reg(_T("Notebook"),wxsTContainer,_T("Standard"),200);

    /** \brief Extra parameters for notebook's children */
    class wxsNotebookExtra: public wxsPropertyContainer
    {
        public:

            wxsNotebookExtra():
                m_Label(_("Page name")),
                m_Selected(false)
            {}

            wxString m_Label;
            bool m_Selected;

        protected:

            virtual void OnEnumProperties(long Flags)
            {
                WXS_SHORT_STRING(wxsNotebookExtra,m_Label,_("Page name"),_T("label"),_T(""),false);
                WXS_BOOL(wxsNotebookExtra,m_Selected,_("Page selected"),_T("selected"),false);
            }
    };

    /** \brief Inernal Quick properties panel */
    class wxsNotebookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsNotebookParentQP(wxsAdvQPP* parent,wxsNotebookExtra* Extra,wxWindowID id = -1):
                wxsAdvQPPChild(parent,_("Notebook")),
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

                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsNotebookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNotebookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1,wxEVT_KILL_FOCUS,(wxObjectEventFunction)&wxsNotebookParentQP::OnLabelKillFocus,0,this);
            }

            virtual ~wxsNotebookParentQP()
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

            wxsNotebookExtra* m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsNotebookParentQP)
    const long wxsNotebookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsNotebookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsNotebookParentQP,wxPanel)
        //(*EventTable(wxsNotebookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsNotebookParentQP::OnLabelText(wxCommandEvent& event)       { SaveData(); }
    void wxsNotebookParentQP::OnLabelKillFocus(wxFocusEvent& event)    { SaveData(); event.Skip(); }
    void wxsNotebookParentQP::OnSelectionChange(wxCommandEvent& event) { SaveData(); }

    WXS_ST_BEGIN(wxsNotebookStyles,_T(""))
        WXS_ST_CATEGORY("wxNotebook")
        WXS_ST(wxNB_DEFAULT)
        WXS_ST(wxNB_LEFT)
        WXS_ST(wxNB_RIGHT)
        WXS_ST(wxNB_TOP)
        WXS_ST(wxNB_BOTTOM)
        WXS_ST(wxNB_FIXEDWIDTH)
        WXS_ST(wxNB_MULTILINE)
        WXS_ST(wxNB_NOPAGETHEME)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsNotebookEvents)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxNotebookEvent,PageChanged)
        WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,wxNotebookEvent,PageChanging)
    WXS_EV_END()

    const long popupNewPageId = wxNewId();
    const long popupPrevPageId = wxNewId();
    const long popupNextPageId = wxNewId();
    const long popupFirstId = wxNewId();
    const long popupLastId = wxNewId();
}


wxsNotebook::wxsNotebook(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsNotebookEvents,
        wxsNotebookStyles),
    m_CurrentSelection(0)
{
}

void wxsNotebook::OnEnumContainerProperties(long Flags)
{
}

bool wxsNotebook::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Can not add sizer into Notebook.\nAdd panels first"));
        }
        return false;
    }

    return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

wxsPropertyContainer* wxsNotebook::OnBuildExtra()
{
    return new wxsNotebookExtra();
}

wxString wxsNotebook::OnXmlGetExtraObjectClass()
{
    return _T("notebookpage");
}

void wxsNotebook::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsNotebookExtra* Extra = (wxsNotebookExtra*)GetChildExtra(GetChildIndex(Child));
    if ( Extra )
    {
        QPP->Register(new wxsNotebookParentQP(QPP,Extra),_("Notebook"));
    }
}

wxObject* wxsNotebook::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();
    wxNotebook* Notebook = new wxNotebook(Parent,-1,Pos(Parent),Size(Parent),Style());

    if ( !GetChildCount() && !(PreviewFlags&pfExact) )
    {
        // Adding additional empty notebook to prevent from having zero-sized notebook
        Notebook->AddPage(
            new wxPanel(Notebook,-1,wxDefaultPosition,wxSize(50,50)),
            _("No pages"));
    }

    AddChildrenPreview(Notebook,PreviewFlags);

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsNotebookExtra* Extra = (wxsNotebookExtra*)GetChildExtra(i);

        wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
        if ( !ChildPreview ) continue;

        bool Selected = (Child == m_CurrentSelection);
        if ( PreviewFlags & pfExact ) Selected = Extra->m_Selected;

        Notebook->AddPage(ChildPreview,Extra->m_Label,Selected);
    }

    return Notebook;
}

void wxsNotebook::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/notebook.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/notebook.h>"),_T("wxNotebookEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsNotebookExtra* Extra = (wxsNotebookExtra*)GetChildExtra(i);
                #if wxCHECK_VERSION(2, 9, 0)
                Codef(_T("%AAddPage(%o, %t, %b);\n"),i,Extra->m_Label.wx_str(),Extra->m_Selected);
                #else
                Codef(_T("%AAddPage(%o, %t, %b);\n"),i,Extra->m_Label.c_str(),Extra->m_Selected);
                #endif
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsNotebook::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

bool wxsNotebook::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{
    UpdateCurrentSelection();
    wxNotebook* Notebook = (wxNotebook*)Preview;
    int Hit = Notebook->HitTest(wxPoint(PosX,PosY));
    if ( Hit != wxNOT_FOUND )
    {
        wxsItem* OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(Hit);
        GetResourceData()->SelectItem(m_CurrentSelection,true);
        return OldSel != m_CurrentSelection;
    }
    return false;
}

bool wxsNotebook::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsNotebook::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsNotebook::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        wxsNotebookExtra* Extra = (wxsNotebookExtra*)GetChildExtra(i);
        if ( (i==0) || Extra->m_Selected )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}

void wxsNotebook::OnPreparePopup(wxMenu* Menu)
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

bool wxsNotebook::OnPopup(long Id)
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
                    wxsNotebookExtra* Extra = (wxsNotebookExtra*)GetChildExtra(GetChildCount()-1);
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
