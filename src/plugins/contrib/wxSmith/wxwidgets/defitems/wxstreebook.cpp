/** \file wxstreebook.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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
*/

#include "wxstreebook.h"
#include "../../wxsadvqppchild.h"
#include "../wxsitemresdata.h"
#include <wx/treebook.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include "../wxsflags.h"
#include "logmanager.h"

using namespace wxsFlags;

//(*Headers(wxsTreebookParentQP)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
//*)

//(*InternalHeaders(wxsTreebookParentQP)
#include <wx/intl.h>
#include <wx/string.h>
//*)

namespace
{
    wxsRegisterItem<wxsTreebook> Reg(_T("Treebook"), wxsTContainer, _T("Standard"), 20);

    /** \brief Extra parameters for Treebook's children
    *
    * I store the data required to manage the tree hierarchy here.
    * Child windows are stored as a flat list here but reconstructed in tree order when the code is built.
    * The parent node is the node in the tree to which a given node is attached. If the parent node value is -1,
    * the default, the node is attached to the trunk of the tree. Since the number assigned by the control to a node
    * changes with each insertion and deletion, the collection is re-parsed after each such operation and the node
    * numbers adjusted.
    */
    class wxsTreebookExtra: public wxsPropertyContainer
    {
        public:

            wxsTreebookExtra():
                m_Label(_("Page name")),
                m_Selected(false),
                m_iParentNode(0),
                m_iTreePos(0)
            {}

            wxString m_Label;
            bool m_Selected;
            int m_iIndex;
            long m_iParentNode;
            int m_iTreePos;

        protected:

            virtual void OnEnumProperties(cb_unused long Flags)
            {
                WXS_SHORT_STRING(wxsTreebookExtra, m_Label, _("Page name"), _T("label"), _T(""), false);
                WXS_BOOL(wxsTreebookExtra, m_Selected, _("Page selected"), _T("selected"), false);
            }
    };

    /** \brief Internal Quick properties panel */
    class wxsTreebookParentQP: public wxsAdvQPPChild
    {
        public:

            wxsTreebookParentQP(wxsAdvQPP *parent, wxsTreebookExtra *Extra, wxWindowID id = -1):
                wxsAdvQPPChild(parent, _("Treebook")),
                m_Extra(Extra)
            {
                //(*Initialize(wxsTreebookParentQP)
                Create(parent, id, wxDefaultPosition, wxSize(120,133), wxTAB_TRAVERSAL, _T("id"));
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
                FlexGridSizer1->SetSizeHints(this);

                Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxsTreebookParentQP::OnLabelText);
                Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsTreebookParentQP::OnSelectionChange);
                //*)
                ReadData();

                Label->Connect(-1, wxEVT_KILL_FOCUS, (wxObjectEventFunction)&wxsTreebookParentQP::OnLabelKillFocus, 0, this);
            }

            virtual ~wxsTreebookParentQP()
            {
                //(*Destroy(wxsTreebookParentQP)
                //*)
            }

        private:

            virtual void Update()
            {
                ReadData();
            }

            void ReadData()
            {
                if (!GetPropertyContainer() || !m_Extra) return;
                Label->SetValue(m_Extra->m_Label);
                Selected->SetValue(m_Extra->m_Selected);
            }

            void SaveData()
            {
                if (!GetPropertyContainer() || !m_Extra) return;
                m_Extra->m_Label = Label->GetValue();
                m_Extra->m_Selected = Selected->GetValue();
                NotifyChange();
            }

            //(*Identifiers(wxsTreebookParentQP)
            static const long ID_TEXTCTRL1;
            static const long ID_CHECKBOX1;
            //*)

            //(*Handlers(wxsTreebookParentQP)
            void OnLabelText(wxCommandEvent &event);
            void OnLabelKillFocus(wxFocusEvent &event);
            void OnSelectionChange(wxCommandEvent &event);
            //*)

            //(*Declarations(wxsTreebookParentQP)
            wxStaticBoxSizer* StaticBoxSizer2;
            wxCheckBox* Selected;
            wxTextCtrl* Label;
            wxStaticBoxSizer* StaticBoxSizer1;
            wxFlexGridSizer* FlexGridSizer1;
            //*)

            wxsTreebookExtra *m_Extra;

            DECLARE_EVENT_TABLE()
    };

    //(*IdInit(wxsTreebookParentQP)
    const long wxsTreebookParentQP::ID_TEXTCTRL1 = wxNewId();
    const long wxsTreebookParentQP::ID_CHECKBOX1 = wxNewId();
    //*)

    BEGIN_EVENT_TABLE(wxsTreebookParentQP, wxPanel)
        //(*EventTable(wxsTreebookParentQP)
        //*)
    END_EVENT_TABLE()

    void wxsTreebookParentQP::OnLabelText(cb_unused wxCommandEvent &event)       {
        SaveData();
    }
    void wxsTreebookParentQP::OnLabelKillFocus(wxFocusEvent &event)    {
        SaveData();
        event.Skip();
    }
    void wxsTreebookParentQP::OnSelectionChange(cb_unused wxCommandEvent &event) {
        SaveData();
    }

    WXS_ST_BEGIN(wxsTreebookStyles, wxT("wxBK_DEFAULT"))
    WXS_ST_CATEGORY("wxTreebook")
    WXS_ST(wxBK_DEFAULT)
    WXS_ST(wxBK_LEFT)
    WXS_ST(wxBK_RIGHT)
    WXS_ST(wxBK_TOP)
    WXS_ST(wxBK_BOTTOM)
    WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsTreebookEvents)
    WXS_EVI(EVT_TREEBOOK_PAGE_CHANGED, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, wxNotebookEvent, PageChanged)
    WXS_EVI(EVT_TREEBOOK_PAGE_CHANGING, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, wxNotebookEvent, PageChanging)
    WXS_EVI(EVT_TREEBOOK_NODE_COLLAPSED, wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED, wxNotebookEvent, NodeCollapsed)
    WXS_EVI(EVT_TREEBOOK_NODE_EXPANDED, wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED, wxNotebookEvent, NodeExpanded)
    WXS_EV_END()

    const long popupNewPageId = wxNewId();
    const long popupPrevPageId = wxNewId();
    const long popupNextPageId = wxNewId();
    const long popupFirstId = wxNewId();
    const long popupLastId = wxNewId();
}

wxsTreebook::wxsTreebook(wxsItemResData *Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsTreebookEvents,
        wxsTreebookStyles)
{
}

void wxsTreebook::OnEnumContainerProperties(cb_unused long Flags)
{
}

bool wxsTreebook::OnCanAddChild(wxsItem *Item, bool ShowMessage)
{
    if (Item->GetType() == wxsTSizer)
    {
        if (ShowMessage)
        {
            wxMessageBox(_("Can not add sizer into treebook.\nAdd panels first"));
        }
        return false;
    }

    return wxsContainer::OnCanAddChild(Item, ShowMessage);
}

wxsPropertyContainer *wxsTreebook::OnBuildExtra()
{
    return new wxsTreebookExtra();
}

wxString wxsTreebook::OnXmlGetExtraObjectClass()
{
    return _T("treebookpage");
}

void wxsTreebook::OnAddChildQPP(wxsItem *Child, wxsAdvQPP *QPP)
{
    wxsTreebookExtra *TBExtra = (wxsTreebookExtra*)GetChildExtra(GetChildIndex(Child));
    if (TBExtra)
    {
        QPP->Register(new wxsTreebookParentQP(QPP, TBExtra), _("Treebook"));
    }
}

wxObject *wxsTreebook::OnBuildPreview(wxWindow *Parent, long PreviewFlags)
{
    UpdateCurrentSelection();
    wxTreebook *Treebook = new wxTreebook(Parent, -1, Pos(Parent), Size(Parent), Style());

    if (!GetChildCount() && !(PreviewFlags & pfExact))
    {
        // Adding additional empty Treebook to prevent from having zero-sized Treebook
        Treebook->AddPage(new wxPanel(Treebook, -1, wxDefaultPosition, wxSize(50, 50)), _("No pages"));
    }

    AddChildrenPreview(Treebook, PreviewFlags);

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsTreebookExtra* TBExtra = (wxsTreebookExtra*)GetChildExtra(i);

        wxWindow* ChildPreview = wxDynamicCast(GetChild(i)->GetLastPreview(),wxWindow);
        if ( !ChildPreview ) continue;

        bool Selected = (Child == m_CurrentSelection);
        if ( PreviewFlags & pfExact ) Selected = TBExtra->m_Selected;

        Treebook->AddPage(ChildPreview,TBExtra->m_Label,Selected);
    }

    return Treebook;
}

void wxsTreebook::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_T("<wx/treebook.h>"), GetInfo().ClassName, 0);
                AddHeader(_T("<wx/treebook.h>"), _T("wxTreebookEvent"), 0);
                Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
                BuildSetupWindowCode();
                AddChildrenCode();

                for (int i = 0; i < GetChildCount(); i++)
                {
                    wxsTreebookExtra *TBExtra = (wxsTreebookExtra*)GetChildExtra(i);
                    if (TBExtra)
                    {
                        Codef(_T("%AAddPage(%o, %t, %b);\n"), i, TBExtra->m_Label.wx_str(), TBExtra->m_Selected);
                    }
                }

                break;
            }

        case wxsUnknownLanguage:
        default:
            {
                wxsCodeMarks::Unknown(_T("wxsTreebook::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

bool wxsTreebook::OnMouseClick(wxWindow *Preview, int PosX, int PosY)
{
    UpdateCurrentSelection();
    wxTreebook *Treebook = (wxTreebook*)Preview;
    int Hit = Treebook->HitTest(wxPoint(PosX, PosY));
    if (Hit != wxNOT_FOUND)
    {
        wxsItem *OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(Hit);
        GetResourceData()->SelectItem(m_CurrentSelection, true);
        return OldSel != m_CurrentSelection;
    }
    return false;
}

bool wxsTreebook::OnIsChildPreviewVisible(wxsItem *Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsTreebook::OnEnsureChildPreviewVisible(wxsItem *Child)
{
    if (IsChildPreviewVisible(Child)) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsTreebook::UpdateCurrentSelection()
{
    wxsItem *NewCurrentSelection = 0;
    for (int i = 0; i < GetChildCount(); i++)
    {
        if (m_CurrentSelection == GetChild(i)) return;
        wxsTreebookExtra *TBExtra = (wxsTreebookExtra*)GetChildExtra(i);
        if ((i == 0) || (TBExtra && TBExtra->m_Selected))
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}

void wxsTreebook::OnPreparePopup(wxMenu *Menu)
{
    Menu->Append(popupNewPageId, _("Add new page"));
    Menu->AppendSeparator();
    // This require some extra fixing
    //wxMenuItem* Item1 = Menu->Append(popupPrevPageId,_("Go to previous page"));
    //wxMenuItem* Item2 = Menu->Append(popupNextPageId,_("Go to next page"));
    //Menu->AppendSeparator();
    wxMenuItem *Item3 = Menu->Append(popupFirstId, _("Make current page the first one"));
    wxMenuItem *Item4 = Menu->Append(popupLastId, _("Make current page the last one"));
    if (!m_CurrentSelection || GetChildIndex(m_CurrentSelection) == 0)
    {
        //Item1->Enable(false);
        Item3->Enable(false);
    }
    if (!m_CurrentSelection || GetChildIndex(m_CurrentSelection) == GetChildCount() - 1)
    {
        //Item2->Enable(false);
        Item4->Enable(false);
    }
}

bool wxsTreebook::OnPopup(long Id)
{
    if (Id == popupNewPageId)
    {
        wxTextEntryDialog Dlg(0, _("Enter name of new page"), _("Adding page"), _("New page"));
        PlaceWindow(&Dlg);
        if (Dlg.ShowModal() == wxID_OK)
        {
            wxsItem *Panel = wxsItemFactory::Build(_T("wxPanel"), GetResourceData());
            if (Panel)
            {
                GetResourceData()->BeginChange();
                if (AddChild(Panel))
                {
                    wxsTreebookExtra *TBExtra = (wxsTreebookExtra*)GetChildExtra(GetChildCount() - 1);
                    if (TBExtra)
                    {
                        TBExtra->m_Label = Dlg.GetValue();
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
    else if (Id == popupNextPageId)
    {
        GetResourceData()->BeginChange();
        int Index = GetChildIndex(m_CurrentSelection);
        m_CurrentSelection = GetChild(Index - 1);
        UpdateCurrentSelection();
        GetResourceData()->EndChange();
    }
    else if (Id == popupPrevPageId)
    {
        GetResourceData()->BeginChange();
        int Index = GetChildIndex(m_CurrentSelection);
        m_CurrentSelection = GetChild(Index + 1);
        UpdateCurrentSelection();
        GetResourceData()->EndChange();
    }
    else if (Id == popupFirstId)
    {
        GetResourceData()->BeginChange();
        MoveChild(GetChildIndex(m_CurrentSelection), 0);
        GetResourceData()->EndChange();
    }
    else if (Id == popupLastId)
    {
        GetResourceData()->BeginChange();
        MoveChild(GetChildIndex(m_CurrentSelection), GetChildCount() - 1);
        GetResourceData()->EndChange();
    }
    else
    {
        return wxsContainer::OnPopup(Id);
    }
    return true;
}

