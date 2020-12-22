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

#include "wxstoolbareditor.h"

#include "../wxsitemresdata.h"
#include "../properties/wxsbitmapiconeditordlg.h"

//(*InternalHeaders(wxsToolBarEditor)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxsToolBarEditor)
const long wxsToolBarEditor::ID_LISTBOX1 = wxNewId();
const long wxsToolBarEditor::ID_STATICTEXT1 = wxNewId();
const long wxsToolBarEditor::ID_CHOICE1 = wxNewId();
const long wxsToolBarEditor::ID_TEXTCTRL4 = wxNewId();
const long wxsToolBarEditor::ID_TEXTCTRL1 = wxNewId();
const long wxsToolBarEditor::ID_BITMAPBUTTON1 = wxNewId();
const long wxsToolBarEditor::ID_BITMAPBUTTON2 = wxNewId();
const long wxsToolBarEditor::ID_TEXTCTRL2 = wxNewId();
const long wxsToolBarEditor::ID_TEXTCTRL3 = wxNewId();
const long wxsToolBarEditor::ID_STATICLINE2 = wxNewId();
const long wxsToolBarEditor::ID_BUTTON3 = wxNewId();
const long wxsToolBarEditor::ID_BUTTON4 = wxNewId();
const long wxsToolBarEditor::ID_BUTTON1 = wxNewId();
const long wxsToolBarEditor::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsToolBarEditor,wxPanel)
    //(*EventTable(wxsToolBarEditor)
    //*)
END_EVENT_TABLE()

wxsToolBarEditor::wxsToolBarEditor(wxWindow* parent,wxsToolBar* ToolBar):
    m_Selected(nullptr),
    m_ToolBar(ToolBar),
    m_BlockTextChange(false),
    m_BlockSelect(false)
{
    //(*Initialize(wxsToolBarEditor)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer3;
    wxButton* Button1;
    wxButton* Button2;
    wxButton* Button3;
    wxButton* Button4;
    wxFlexGridSizer* FlexGridSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxStaticText* StaticText1;
    wxStaticText* StaticText2;
    wxStaticText* StaticText3;
    wxStaticText* StaticText4;
    wxStaticText* StaticText5;
    wxStaticText* StaticText6;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Content"));
    m_Content = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    StaticBoxSizer1->Add(m_Content, 1, wxEXPAND, 4);
    BoxSizer1->Add(StaticBoxSizer1, 1, wxEXPAND, 4);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Options"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer1->AddGrowableCol(1);
    StaticText7 = new wxStaticText(this, ID_STATICTEXT1, _("Tool type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer1->Add(StaticText7, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Type = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    m_Type->SetSelection( m_Type->Append(_("Normal")) );
    m_Type->Append(_("Check"));
    m_Type->Append(_("Radio"));
    m_Type->Append(_("Separator"));
    m_Type->Append(_("Stretchable separator"));
    FlexGridSizer1->Add(m_Type, 1, wxTOP|wxLEFT, 5);
    StaticText6 = new wxStaticText(this, wxID_ANY, _("Id:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText6, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Id = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer1->Add(m_Id, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText1 = new wxStaticText(this, wxID_ANY, _("Label:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText1, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Label = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer1->Add(m_Label, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText4 = new wxStaticText(this, wxID_ANY, _("Bitmap:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText4, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Bitmap = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    m_Bitmap->SetDefault();
    FlexGridSizer1->Add(m_Bitmap, 1, wxTOP|wxLEFT, 5);
    StaticText5 = new wxStaticText(this, wxID_ANY, _("Disabled bitmap:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText5, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_Bitmap2 = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    m_Bitmap2->SetDefault();
    FlexGridSizer1->Add(m_Bitmap2, 1, wxTOP|wxLEFT, 5);
    StaticText2 = new wxStaticText(this, wxID_ANY, _("Tooltip:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText2, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_ToolTip = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer1->Add(m_ToolTip, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticText3 = new wxStaticText(this, wxID_ANY, _("Help text:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(StaticText3, 1, wxTOP|wxALIGN_CENTER_VERTICAL, 5);
    m_HelpText = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer1->Add(m_HelpText, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticBoxSizer2->Add(FlexGridSizer1, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
    StaticBoxSizer2->Add(StaticLine2, 0, wxTOP|wxEXPAND, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Button3 = new wxButton(this, ID_BUTTON3, _("Move up"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer3->Add(Button3, 1, wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(this, ID_BUTTON4, _("Move down"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer3->Add(Button4, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer3, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 1, wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer2->Add(Button2, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer2, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer1->Add(StaticBoxSizer2, 2, wxLEFT|wxRIGHT|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&wxsToolBarEditor::Onm_ContentSelect);
    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&wxsToolBarEditor::OnTypeChanged);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsToolBarEditor::Onm_LabelText);
    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnBitmapClick);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnBitmap2Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnUpClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnDownClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnNewClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsToolBarEditor::OnDelClick);
    //*)

    for ( int i=0; i<m_ToolBar->GetChildCount(); i++ )
    {
        wxsItem* Child = m_ToolBar->GetChild(i);
        if ( Child->GetClassName() == _T("wxToolBarToolBase") )
        {
            wxsToolBarItem* Item = (wxsToolBarItem*)Child;
            ToolBarItem* New = new ToolBarItem;
            New->m_OriginalPos = -1;
            New->m_Id = Item->GetIdName();
            New->m_Variable = Item->GetVarName();
            New->m_Label = Item->m_Label;
            New->m_Bitmap = Item->m_Bitmap;
            New->m_Bitmap2 = Item->m_Bitmap2;
            New->m_ToolTip = Item->m_ToolTip;
            New->m_HelpText = Item->m_HelpText;

            wxsEvents& Events = Item->GetEvents();
            if ( Events.GetCount() > 0 ) New->m_Handler1 = Events.GetHandler(0);
            if ( Events.GetCount() > 1 ) New->m_Handler2 = Events.GetHandler(1);

            switch (Item->m_Type)
            {
                case wxsToolBarItem::Separator:
                case wxsToolBarItem::Stretchable:
                    New->m_Id = wxString(); // fall-through
                case wxsToolBarItem::Radio:
                case wxsToolBarItem::Check:
                    New->m_Type = Item->m_Type;
                    break;
                case wxsToolBarItem::Normal: // fall-though
                default:
                    New->m_Type = wxsToolBarItem::Normal;
            }

            m_Content->Append(GetItemLabel(New),New);
        }
        else
        {
            ToolBarItem* New = new ToolBarItem;
            New->m_OriginalPos = i;
            New->m_Label = Child->GetClassName();
            if ( Child->GetPropertiesFlags() & flVariable )
            {
                New->m_Label << _T(": ") << Child->GetVarName();
            }
            New->m_Type = wxsToolBarItem::Control;
            m_Content->Append(GetItemLabel(New),New);
        }
    }

    if ( m_Content->GetCount() == 0 )
    {
        SelectItem(nullptr);
    }
    else
    {
        m_Content->Select(0);
        SelectItem((ToolBarItem*)m_Content->GetClientObject(0));
    }
}

wxsToolBarEditor::~wxsToolBarEditor()
{
    //(*Destroy(wxsToolBarEditor)
    //*)
}

void wxsToolBarEditor::OnTypeChanged(cb_unused wxCommandEvent& event)
{
    // Saving data
    ToolBarItem* Selected = m_Selected;
    SelectItem(nullptr);        // Storing current content
    SelectItem(Selected);       // Updating screen's content
}

namespace
{
    typedef wxsItem* wxsItemP;
}

void wxsToolBarEditor::ApplyChanges()
{
    SelectItem(m_Selected); // Store changes

    m_ToolBar->GetResourceData()->BeginChange();
    const int NewCount = m_Content->GetCount();

    // If all items have been removed just remove everything from toolbar
    if (NewCount == 0)
    {
        for (int i = m_ToolBar->GetChildCount(); i-- > 0;)
        {
            wxsItem* Child = m_ToolBar->GetChild(i);
            m_ToolBar->UnbindChild(i);
            delete Child;
        }
    }
    else
    {
        wxsItemP* NewChildrenArray = new wxsItemP[NewCount];
        wxArrayBool ParentChildrenUsed;
        ParentChildrenUsed.Add(false,m_ToolBar->GetChildCount());
        for (int i = 0; i < NewCount; ++i)
        {
            ToolBarItem* Item = (ToolBarItem*)m_Content->GetClientObject(i);
            if (Item->m_Type == wxsToolBarItem::Control)
            {
                const int Index = Item->m_OriginalPos;
                wxASSERT(ParentChildrenUsed[Index]==false);
                NewChildrenArray[i] = m_ToolBar->GetChild(Index);
                ParentChildrenUsed[Index] = true;
            }
            else
            {
                wxsToolBarItem* New = new wxsToolBarItem(m_ToolBar->GetResourceData(),
                                                         Item->m_Type);
                switch (Item->m_Type)
                {
                    case wxsToolBarItem::Separator:
                    case wxsToolBarItem::Stretchable:
                    case wxsToolBarItem::Radio:
                    case wxsToolBarItem::Check:
                        New->m_Type = Item->m_Type;
                        break;
                    case wxsToolBarItem::Normal:  // fall-through
                    case wxsToolBarItem::Control: // fall-through
                    default:
                        New->m_Type = wxsToolBarItem::Normal;
                }

                if (Item->m_Type != wxsToolBarItem::Separator
                    && Item->m_Type != wxsToolBarItem::Stretchable)
                {
                    New->SetIdName(Item->m_Id);
                    New->SetVarName(Item->m_Variable);
                    New->m_Label = Item->m_Label;
                    New->m_Bitmap = Item->m_Bitmap;
                    New->m_Bitmap2 = Item->m_Bitmap2;
                    New->m_ToolTip = Item->m_ToolTip;
                    New->m_HelpText = Item->m_HelpText;

                    wxsEvents& Events = New->GetEvents();
                    if (Events.GetCount() > 0)
                        Events.SetHandler(0,Item->m_Handler1);
                    if (Events.GetCount() > 1)
                        Events.SetHandler(1,Item->m_Handler2);
                }

                NewChildrenArray[i] = New;
            }
        }

        // Removing items from toolbar which are not reused
        for (int i = m_ToolBar->GetChildCount(); i-- > 0;)
        {
            wxsItem* Child = m_ToolBar->GetChild(i);
            m_ToolBar->UnbindChild(Child);
            if (!ParentChildrenUsed[i])
                delete Child;
        }

        // Adding new items into toolbar
        for (int i = 0; i < NewCount; ++i)
        {
            if (!m_ToolBar->AddChild(NewChildrenArray[i]))
                delete NewChildrenArray[i];
        }

        delete[] NewChildrenArray;
    }

    m_ToolBar->GetResourceData()->EndChange();
}

wxString wxsToolBarEditor::GetItemLabel(ToolBarItem* Item)
{
    if (Item->m_Type == wxsToolBarItem::Separator)
        return "--------";
    if (Item->m_Type == wxsToolBarItem::Stretchable)
        return "<------>";
    return Item->m_Label;
}


void wxsToolBarEditor::Onm_ContentSelect(cb_unused wxCommandEvent& event)
{
    if ( m_BlockSelect ) return;
    const int Selection = m_Content->GetSelection();
    if ( Selection == wxNOT_FOUND )
    {
        SelectItem(nullptr);
    }
    else
    {
        SelectItem((ToolBarItem*)m_Content->GetClientObject(Selection));
    }
}


void wxsToolBarEditor::Onm_LabelText(cb_unused wxCommandEvent& event)
{
    if ( m_BlockTextChange ) return;
    m_BlockSelect = true;
    if ( m_Selected )
    {
        m_Selected->m_Label = m_Label->GetValue();
        const int Selection = m_Content->GetSelection();
        m_Content->SetString(Selection,GetItemLabel(m_Selected));
        m_Content->SetSelection(Selection);
    }
    m_BlockSelect = false;
}

void wxsToolBarEditor::SelectItem(ToolBarItem* Item)
{
    m_BlockTextChange = true;
    if (m_Selected != nullptr)
    {
        // Storing current content
        // If it's control we do not store anything since
        // can not change anything inside external control
        if (m_Selected->m_Type != wxsToolBarItem::Control)
        {
            const int SelectedType = m_Type->GetSelection();
            if (SelectedType < 0)
                m_Selected->m_Type = wxsToolBarItem::Normal;
            else if (SelectedType > wxsToolBarItem::ToolType::Control)
                m_Selected->m_Type = wxsToolBarItem::Control;
            else
                m_Selected->m_Type = static_cast<wxsToolBarItem::ToolType>(SelectedType);

            m_Selected->m_Id = m_Id->GetValue();
            m_Selected->m_Label = m_Label->GetValue();
            m_Selected->m_ToolTip = m_ToolTip->GetValue();
            m_Selected->m_HelpText = m_HelpText->GetValue();
        }
    }

    if ( m_Selected == Item )
    {
        m_BlockTextChange = false;
        return;
    }
    m_Selected = Item;

    if ( m_Selected )
    {
        if ( m_Selected->m_Type == wxsToolBarItem::Control )
        {
            m_Type->Disable();
            m_Id->Disable();
            m_Id->SetValue(_T(""));
            m_Label->Disable();
            m_Label->SetValue(_T(""));
            m_Bitmap->Disable();
            m_Bitmap->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_("wxART_FILE_OPEN")),wxART_TOOLBAR));
            m_Bitmap2->Disable();
            m_Bitmap2->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_("wxART_FILE_OPEN")),wxART_TOOLBAR));
            m_ToolTip->Disable();
            m_ToolTip->SetValue(_T(""));
            m_HelpText->Disable();
            m_HelpText->SetValue(_T(""));
        }
        else
        {
            const bool IsSeparator = (m_Selected->m_Type == wxsToolBarItem::Separator) || (m_Selected->m_Type == wxsToolBarItem::Stretchable);
            m_Type->Enable();
            m_Type->SetSelection(m_Selected->m_Type);
            m_Id->Enable(!IsSeparator);
            m_Id->SetValue(m_Selected->m_Id);
            m_Label->Enable(!IsSeparator);
            m_Label->SetValue(m_Selected->m_Label);
            m_Bitmap->Enable(!IsSeparator);
            // A bitmap created from code can't be rendered easily here, so show the default for that case, too
            if ( m_Selected->m_Bitmap.IsEmpty() || !m_Selected->m_Bitmap.CodeText.IsEmpty())
            {
                m_Bitmap->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR));
            }
            else
            {
                m_Bitmap->SetBitmapLabel(m_Selected->m_Bitmap.GetPreview(wxDefaultSize,_T("wxART_TOOLBAR")));
            }
            m_Bitmap2->Enable(!IsSeparator);
            if ( m_Selected->m_Bitmap2.IsEmpty() )
            {
                m_Bitmap2->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR));
            }
            else
            {
                m_Bitmap2->SetBitmapLabel(m_Selected->m_Bitmap2.GetPreview(wxDefaultSize,_T("wxART_TOOLBAR")));
            }
            m_ToolTip->Enable(!IsSeparator);
            m_ToolTip->SetValue(m_Selected->m_ToolTip);
            m_HelpText->Enable(!IsSeparator);
            m_HelpText->SetValue(m_Selected->m_HelpText);
        }
    }

    m_BlockTextChange = false;
}

void wxsToolBarEditor::OnUpClick(cb_unused wxCommandEvent& event)
{
    if ( !m_Selected ) return;
    SelectItem(m_Selected);
    const int SelIndex = m_Content->GetSelection();
    if ( SelIndex == wxNOT_FOUND ) return;
    if ( SelIndex == 0 ) return;
    // We will delete previous item and put it after this one
    ToolBarItem* Copy = new ToolBarItem(*((ToolBarItem*)m_Content->GetClientObject(SelIndex-1)));
    m_Content->Delete(SelIndex-1);
    m_Content->Insert(GetItemLabel(Copy),SelIndex,Copy);
}

void wxsToolBarEditor::OnDownClick(cb_unused wxCommandEvent& event)
{
    if ( !m_Selected ) return;
    SelectItem(m_Selected);
    const int SelIndex = m_Content->GetSelection();
    if ( SelIndex == wxNOT_FOUND ) return;
    if ( SelIndex > (int)m_Content->GetCount()-2  ) return;
    // We will delete next item and put it before this one
    ToolBarItem* Copy = new ToolBarItem(*((ToolBarItem*)m_Content->GetClientObject(SelIndex+1)));
    m_Content->Delete(SelIndex+1);
    m_Content->Insert(GetItemLabel(Copy),SelIndex,Copy);
}

void wxsToolBarEditor::OnNewClick(cb_unused wxCommandEvent& event)
{
    SelectItem(m_Selected);
    ToolBarItem* New = new ToolBarItem();
    New->m_Label = _("New item");
    const int SelIndex = m_Content->GetSelection();
    if ( SelIndex == wxNOT_FOUND )
    {
        m_Content->SetSelection(m_Content->Append(GetItemLabel(New),New));
    }
    else
    {
        m_Content->Insert(GetItemLabel(New),SelIndex+1,New);
        m_Content->SetSelection(SelIndex+1);
    }
    SelectItem(New);
}

void wxsToolBarEditor::OnDelClick(cb_unused wxCommandEvent& event)
{
    int Selection = m_Content->GetSelection();
    if (Selection == wxNOT_FOUND)
        return;
    if (cbMessageBox(_("Are you sure to delete this item?"), _("Deleting wxToolBar item"),
                     wxYES_NO) == wxID_YES)
    {
        // Clear m_Selected, because it will be invalid after the call to Delete.
        SelectItem(nullptr);
        m_Content->Delete(Selection);
        const int NewCount = m_Content->GetCount();
        if (NewCount > 0)
        {
            if (Selection == NewCount)
               --Selection;

            m_Content->SetSelection(Selection);
            SelectItem((ToolBarItem*)m_Content->GetClientObject(Selection));
        }
        else
            m_Content->SetSelection(wxNOT_FOUND);
    }
}

void wxsToolBarEditor::OnBitmapClick(cb_unused wxCommandEvent& event)
{
    if ( !m_Selected ) return;
    ToolBarItem* Selected = m_Selected;
    SelectItem(Selected);
    wxsBitmapIconEditorDlg Dlg(this,Selected->m_Bitmap,_T("wxART_TOOLBAR"));
    Dlg.ShowModal();
    SelectItem(nullptr);
    SelectItem(Selected);
}


void wxsToolBarEditor::OnBitmap2Click(cb_unused wxCommandEvent& event)
{
    if ( !m_Selected ) return;
    ToolBarItem* Selected = m_Selected;
    SelectItem(Selected);
    wxsBitmapIconEditorDlg Dlg(this,Selected->m_Bitmap2,_T("wxART_TOOLBAR"));
    Dlg.ShowModal();
    SelectItem(nullptr);
    SelectItem(Selected);
}
