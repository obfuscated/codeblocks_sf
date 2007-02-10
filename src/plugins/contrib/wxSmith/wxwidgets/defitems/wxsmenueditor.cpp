/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#include "wxsmenueditor.h"

#include "wxsmenu.h"
#include "wxsmenubar.h"
#include "../wxsitemresdata.h"

//(*InternalHeaders(wxsMenuEditor)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(wxsMenuEditor)
const long wxsMenuEditor::ID_TREECTRL1 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON1 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON4 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON2 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON5 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON3 = wxNewId();
const long wxsMenuEditor::ID_STATICLINE1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT6 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL4 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT1 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT2 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL2 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT3 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL3 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT4 = wxNewId();
const long wxsMenuEditor::ID_CHECKBOX1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT5 = wxNewId();
const long wxsMenuEditor::ID_CHECKBOX2 = wxNewId();
const long wxsMenuEditor::ID_STATICLINE2 = wxNewId();
const long wxsMenuEditor::ID_BUTTON1 = wxNewId();
const long wxsMenuEditor::ID_BUTTON2 = wxNewId();
const long wxsMenuEditor::ID_BUTTON3 = wxNewId();
const long wxsMenuEditor::ID_BUTTON4 = wxNewId();
const long wxsMenuEditor::ID_BUTTON5 = wxNewId();
const long wxsMenuEditor::ID_BUTTON6 = wxNewId();
//*)

wxsMenuEditor::wxsMenuEditor(wxWindow* parent,wxsMenuBar* MenuBar):
    m_MenuBar(MenuBar),
    m_Menu(NULL),
    m_First(NULL),
    m_Selected(NULL),
    m_BlockSel(false)
{
    CreateDataCopy();
    CreateContent(parent);
    UpdateMenuContent();
}

wxsMenuEditor::wxsMenuEditor(wxWindow* parent,wxsMenu* Menu):
    m_MenuBar(NULL),
    m_Menu(Menu),
    m_First(NULL),
    m_Selected(NULL),
    m_BlockSel(false)
{
    CreateDataCopy();
    CreateContent(parent);
    UpdateMenuContent();
}

void wxsMenuEditor::CreateContent(wxWindow* parent)
{
    wxWindowID id = wxID_ANY;
	//(*Initialize(wxsMenuEditor)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("id"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Content"));
	m_Content = new wxTreeCtrl(this,ID_TREECTRL1,wxDefaultPosition,wxSize(200,295),wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE,wxDefaultValidator,_T("ID_TREECTRL1"));
	StaticBoxSizer1->Add(m_Content,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	BoxSizer1->Add(StaticBoxSizer1,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Options"));
	GridSizer1 = new wxGridSizer(0,2,0,0);
	m_TypeNormal = new wxRadioButton(this,ID_RADIOBUTTON1,_("Normal"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON1"));
	m_TypeNormal->SetValue(true);
	m_TypeNormal->Disable();
	GridSizer1->Add(m_TypeNormal,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	m_TypeSeparator = new wxRadioButton(this,ID_RADIOBUTTON4,_("Separator"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON4"));
	m_TypeSeparator->Disable();
	GridSizer1->Add(m_TypeSeparator,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	m_TypeCheck = new wxRadioButton(this,ID_RADIOBUTTON2,_("Check"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON2"));
	m_TypeCheck->Disable();
	GridSizer1->Add(m_TypeCheck,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	m_TypeBreak = new wxRadioButton(this,ID_RADIOBUTTON5,_("Break"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON5"));
	m_TypeBreak->Disable();
	GridSizer1->Add(m_TypeBreak,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	m_TypeRadio = new wxRadioButton(this,ID_RADIOBUTTON3,_("Radio"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON3"));
	m_TypeRadio->Disable();
	GridSizer1->Add(m_TypeRadio,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(GridSizer1,0,wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,10);
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),wxLI_HORIZONTAL,_T("ID_STATICLINE1"));
	StaticBoxSizer2->Add(StaticLine1,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer1 = new wxFlexGridSizer(0,2,0,0);
	StaticText6 = new wxStaticText(this,ID_STATICTEXT6,_("Id:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText6,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Id = new wxTextCtrl(this,ID_TEXTCTRL4,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL4"));
	m_Id->Disable();
	FlexGridSizer1->Add(m_Id,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Label:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Label = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
	m_Label->Disable();
	FlexGridSizer1->Add(m_Label,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Accelerator:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Accelerator = new wxTextCtrl(this,ID_TEXTCTRL2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL2"));
	m_Accelerator->Disable();
	FlexGridSizer1->Add(m_Accelerator,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Help:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Help = new wxTextCtrl(this,ID_TEXTCTRL3,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL3"));
	m_Help->Disable();
	FlexGridSizer1->Add(m_Help,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Checked:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Checked = new wxCheckBox(this,ID_CHECKBOX1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
	m_Checked->SetValue(false);
	m_Checked->Disable();
	FlexGridSizer1->Add(m_Checked,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Enabled:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	m_Enabled = new wxCheckBox(this,ID_CHECKBOX2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX2"));
	m_Enabled->SetValue(false);
	m_Enabled->Disable();
	FlexGridSizer1->Add(m_Enabled,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(FlexGridSizer1,0,wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,10);
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxSize(10,-1),wxLI_HORIZONTAL,_T("ID_STATICLINE2"));
	StaticBoxSizer2->Add(StaticLine2,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("<"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON1"));
	BoxSizer2->Add(Button1,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,2);
	Button2 = new wxButton(this,ID_BUTTON2,_(">"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON2"));
	BoxSizer2->Add(Button2,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,2);
	Button3 = new wxButton(this,ID_BUTTON3,_("^"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON3"));
	BoxSizer2->Add(Button3,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,2);
	Button4 = new wxButton(this,ID_BUTTON4,_("v"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON4"));
	BoxSizer2->Add(Button4,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,2);
	StaticBoxSizer2->Add(BoxSizer2,0,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button5 = new wxButton(this,ID_BUTTON5,_("New"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON5"));
	BoxSizer3->Add(Button5,1,wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	Button6 = new wxButton(this,ID_BUTTON6,_("Delete"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,wxDefaultValidator,_T("ID_BUTTON6"));
	BoxSizer3->Add(Button6,1,wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(BoxSizer3,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	BoxSizer1->Add(StaticBoxSizer2,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&wxsMenuEditor::OnContentSelectionChanged);
	Connect(ID_RADIOBUTTON1,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsMenuEditor::OnTypeChanged);
	Connect(ID_RADIOBUTTON4,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsMenuEditor::OnTypeChanged);
	Connect(ID_RADIOBUTTON2,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsMenuEditor::OnTypeChanged);
	Connect(ID_RADIOBUTTON5,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsMenuEditor::OnTypeChanged);
	Connect(ID_RADIOBUTTON3,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&wxsMenuEditor::OnTypeChanged);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsMenuEditor::OnLabelChanged);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonLeftClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonRightClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonUpClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonDownClick);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonNewClick);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsMenuEditor::OnButtonDelClick);
	//*)
}

wxsMenuEditor::~wxsMenuEditor()
{
    DeleteDataCopy();
}

void wxsMenuEditor::CreateDataCopy()
{
    wxsParent* Parent = m_MenuBar ? ((wxsItem*)m_MenuBar)->ConvertToParent() : ((wxsItem*)m_Menu)->ConvertToParent();
    if ( Parent )
    {
        MenuItem* LastChild = NULL;
        for ( int i=0; i<Parent->GetChildCount(); i++ )
        {
            wxsMenu* Child = (wxsMenu*)Parent->GetChild(i);
            MenuItem* NewChild = new MenuItem;
            NewChild->m_Next = NULL;
            NewChild->m_Child = NULL;
            NewChild->m_Parent = NULL;
            if ( LastChild )
            {
                LastChild->m_Next = NewChild;
            }
            else
            {
                m_First = NewChild;
            }
            LastChild = NewChild;
            CreateDataCopyReq(Child,NewChild);
        }
    }
}

void wxsMenuEditor::CreateDataCopyReq(wxsMenu* Menu,MenuItem* Item)
{
    Item->m_Type = Menu->m_Type;
    Item->m_Id = Menu->GetIdName();
    Item->m_Label = Menu->m_Label;
    Item->m_Accelerator = Menu->m_Accelerator;
    Item->m_Help = Menu->m_Help;
    Item->m_Enabled = Menu->m_Enabled;
    Item->m_Checked = Menu->m_Checked;

    MenuItem* LastChild = NULL;
    for ( int i=0; i<Menu->GetChildCount(); i++ )
    {
        wxsMenu* Child = (wxsMenu*)Menu->GetChild(i);
        MenuItem* NewChild = new MenuItem;
        NewChild->m_Next = NULL;
        NewChild->m_Child = NULL;
        NewChild->m_Parent = Item;
        if ( LastChild )
        {
            LastChild->m_Next = NewChild;
        }
        else
        {
            Item->m_Child = NewChild;
        }
        LastChild = NewChild;
        CreateDataCopyReq(Child,NewChild);
    }
}

void wxsMenuEditor::DeleteDataCopy()
{
    DeleteDataCopyReq(m_First);
    m_First = NULL;
}

void wxsMenuEditor::DeleteDataCopyReq(MenuItem* Item)
{
    while ( Item )
    {
        MenuItem* Next = Item->m_Next;
        DeleteDataCopyReq(Item->m_Child);
        delete Item;
        Item = Next;
    }
}

void wxsMenuEditor::UpdateMenuContent()
{
    CheckConsistency();
    m_Content->Freeze();
    m_BlockSel = true;              // wxTreeCtrl changes selection during delete
    m_Content->DeleteAllItems();
    m_BlockSel = false;
    wxTreeItemId RootId = m_Content->AddRoot(_T("Menu"));
    UpdateMenuContentReq(RootId,m_First);
    m_Content->Expand(RootId);
    m_Content->Thaw();
    if ( m_Selected )
    {
        m_Content->SelectItem(m_Selected->m_TreeId);
    }
}

void wxsMenuEditor::UpdateMenuContentReq(wxTreeItemId Id,MenuItem* Item)
{
    for ( ; Item; Item = Item->m_Next )
    {
        wxTreeItemId ItemId = m_Content->AppendItem(Id,GetItemTreeName(Item),-1,-1,new MenuItemHolder(Item));
        Item->m_TreeId = ItemId;
        UpdateMenuContentReq(ItemId,Item->m_Child);
        m_Content->Expand(ItemId);
    }
}

wxString wxsMenuEditor::GetItemTreeName(MenuItem* Item)
{
    switch ( Item->m_Type )
    {
        case wxsMenu::Separator: return _T("--------");
        case wxsMenu::Break:     return _("** BREAK **");
        default:                 return Item->m_Label;
    }
}

void wxsMenuEditor::OnContentSelectionChanged(wxTreeEvent& event)
{
    if ( m_BlockSel ) return;
    wxTreeItemId Id = event.GetItem();
    if ( !Id.IsOk() ) return;
    MenuItem* Selected = ((MenuItemHolder*)m_Content->GetItemData(Id))->m_Item;
    SelectItem(Selected);
}

void wxsMenuEditor::SelectItem(MenuItem* NewSelection)
{
    if ( m_Selected )
    {
        // Storing current data to item
        if ( m_TypeNormal->GetValue()    ) m_Selected->m_Type = wxsMenu::Normal;
        if ( m_TypeCheck->GetValue()     ) m_Selected->m_Type = wxsMenu::Check;
        if ( m_TypeRadio->GetValue()     ) m_Selected->m_Type = wxsMenu::Radio;
        if ( m_TypeBreak->GetValue()     ) m_Selected->m_Type = wxsMenu::Break;
        if ( m_TypeSeparator->GetValue() ) m_Selected->m_Type = wxsMenu::Separator;
        m_Selected->m_Id = m_Id->GetValue();
        m_Selected->m_Label = m_Label->GetValue();
        m_Selected->m_Accelerator = m_Accelerator->GetValue();
        m_Selected->m_Help = m_Help->GetValue();
        m_Selected->m_Enabled = m_Enabled->GetValue();
        m_Selected->m_Checked = m_Checked->GetValue();
    }

    if ( m_Selected == NewSelection ) return;
    m_Selected = NewSelection;

    if ( m_Selected )
    {
        m_TypeNormal->Enable();
        m_TypeCheck->Enable();
        m_TypeRadio->Enable();
        m_TypeBreak->Enable();
        m_TypeSeparator->Enable();

        bool UseId = false;
        bool UseLabel = false;
        bool UseAccelerator = false;
        bool UseHelp = false;
        bool UseEnabled = false;
        bool UseChecked = false;
        switch ( CorrectType(m_Selected,UseId,UseLabel,UseAccelerator,UseHelp,UseEnabled,UseChecked) )
        {
            case wxsMenu::Menu:
            case wxsMenu::Normal:
                m_TypeNormal->SetValue(true);
                // If item has children, can not change type to anything else
                // Same goes for children of wxMenuBar
                if ( m_Selected->m_Child || (!m_Selected->m_Parent && m_MenuBar) )
                {
                    m_TypeCheck->Disable();
                    m_TypeRadio->Disable();
                    m_TypeBreak->Disable();
                    m_TypeSeparator->Disable();
                }
                break;

            case wxsMenu::Radio:
                m_TypeRadio->SetValue(true);
                break;

            case wxsMenu::Check:
                m_TypeCheck->SetValue(true);
                break;

            case wxsMenu::Separator:
                m_TypeSeparator->SetValue(true);
                break;

            case wxsMenu::Break:
                m_TypeBreak->SetValue(true);
                break;
        }

        m_Id->Enable(UseId);
        m_Id->SetValue(m_Selected->m_Id);
        m_Label->Enable(UseLabel);
        m_Label->SetValue(m_Selected->m_Label);
        m_Accelerator->Enable(UseAccelerator);
        m_Accelerator->SetValue(m_Selected->m_Accelerator);
        m_Help->Enable(UseHelp);
        m_Help->SetValue(m_Selected->m_Help);
        m_Enabled->Enable(UseEnabled);
        m_Enabled->SetValue(m_Selected->m_Enabled);
        m_Checked->Enable(UseChecked);
        m_Checked->SetValue(m_Selected->m_Checked);
    }
    else
    {
        m_Id->Clear();
        m_Label->Clear();
        m_Accelerator->Clear();
        m_Help->Clear();
        m_Enabled->SetValue(true);
        m_Checked->SetValue(false);

        m_Id->Disable();
        m_Label->Disable();
        m_Accelerator->Disable();
        m_Help->Disable();
        m_Enabled->Disable();
        m_Checked->Disable();
        m_TypeNormal->Disable();
        m_TypeCheck->Disable();
        m_TypeRadio->Disable();
        m_TypeBreak->Disable();
        m_TypeSeparator->Disable();
    }
}

void wxsMenuEditor::ApplyChanges()
{
    // Re-selecting item to write data from window to ItemData object
    SelectItem(m_Selected);

    // Now storing everything into original structure
    StoreDataCopy();
}

void wxsMenuEditor::StoreDataCopy()
{
    wxsParent* Parent = m_MenuBar ? ((wxsItem*)m_MenuBar)->ConvertToParent() : ((wxsItem*)m_Menu)->ConvertToParent();
    if ( Parent )
    {
        // First notifying that structure is going to change
        Parent->GetResourceData()->BeginChange();

        // Have to delete all children of menu
        for ( int Count = Parent->GetChildCount(); Count-->0; )
        {
            wxsItem* Child = Parent->GetChild(Count);
            m_Menu->UnbindChild(Count);
            delete Child;
        }
        StoreDataCopyReq(Parent,m_First);

        // Notifying about finished change
        Parent->GetResourceData()->EndChange();
    }
}

void wxsMenuEditor::StoreDataCopyReq(wxsParent* Parent,MenuItem* Item)
{
    // First need to copy data from Item to Menu, not all data will
    // be copied, only things proper to item type
    for ( ; Item; Item = Item->m_Next )
    {
        wxsMenu* Menu = new wxsMenu(Parent->GetResourceData());
        if ( !Parent->AddChild(Menu) )
        {
            delete Menu;
            continue;
        }

        bool UseId = false;
        bool UseLabel = false;
        bool UseAccelerator = false;
        bool UseHelp = false;
        bool UseEnabled = false;
        bool UseChecked = false;

        Menu->SetIdName(_T(""));
        Menu->m_Label.Clear();
        Menu->m_Accelerator.Clear();
        Menu->m_Help.Clear();
        Menu->m_Enabled = true;
        Menu->m_Checked = false;
        Menu->m_Type = CorrectType(Item,UseId,UseLabel,UseAccelerator,UseHelp,UseEnabled,UseChecked);
        if ( UseId          ) Menu->SetIdName(Item->m_Id);
        if ( UseLabel       ) Menu->m_Label = Item->m_Label;
        if ( UseAccelerator ) Menu->m_Accelerator = Item->m_Accelerator;
        if ( UseHelp        ) Menu->m_Help = Item->m_Help;
        if ( UseEnabled     ) Menu->m_Enabled = Item->m_Enabled;
        if ( UseChecked     ) Menu->m_Checked = Item->m_Checked;

        StoreDataCopyReq(Menu,Item->m_Child);
    }
}

wxsMenuEditor::Type wxsMenuEditor::CorrectType(MenuItem* Item,bool& UseId,bool& UseLabel,bool& UseAccelerator,bool& UseHelp,bool& UseEnabled,bool& UseChecked)
{
    UseId = false;
    UseLabel = false;
    UseAccelerator = false;
    UseHelp = false;
    UseEnabled = false;
    UseChecked = false;

    if ( !Item->m_Parent && m_MenuBar )
    {
        // Children of wxMenuBar must be menus
        UseId = true;
        UseLabel = true;
        return wxsMenu::Menu;
    }

    if ( Item->m_Child )
    {
        // There's child item, so it must be wxMenu also
        if ( m_MenuBar && !Item->m_Parent )
        {
            // Only id and label (title) is used when child of wxMenuBar
            UseId = true;
            UseLabel = true;
        }
        else
        {
            UseId = true;
            UseLabel = true;
            UseHelp = true;
            UseEnabled = true;
        }
        return wxsMenu::Menu;
    }

    switch ( Item->m_Type )
    {
        case wxsMenu::Separator:
            return wxsMenu::Separator;

        case wxsMenu::Break:
            return wxsMenu::Break;

        case wxsMenu::Check:
            UseChecked = true;
            // Fall through
        case wxsMenu::Radio:
        case wxsMenu::Normal:
            UseId = true;
            UseLabel = true;
            UseAccelerator = true;
            UseHelp = true;
            UseEnabled = true;
            return Item->m_Type;

        default:;
    }

    return wxsMenu::Normal;
}

void wxsMenuEditor::OnTypeChanged(wxCommandEvent& event)
{
    MenuItem* Selected = m_Selected;
    SelectItem(Selected);
    m_Selected = NULL;
    SelectItem(Selected);
    m_Content->SetItemText(m_Selected->m_TreeId,GetItemTreeName(m_Selected));
}

void wxsMenuEditor::OnButtonUpClick(wxCommandEvent& event)
{
    if ( !m_Selected ) return;

    MenuItem* Parent = m_Selected->m_Parent;
    MenuItem* Previous = GetPrevious(m_Selected);

    if ( Previous )
    {
        Previous->m_Next = m_Selected->m_Next;
        m_Selected->m_Next = Previous;

        MenuItem* Previous2 = GetPrevious(Previous);
        if ( Previous2 )
        {
            Previous2->m_Next = m_Selected;
        }
        else
        {
            // Moving to first position in parent
            if ( Parent )
            {
                Parent->m_Child = m_Selected;
            }
            else
            {
                m_First = m_Selected;
            }
        }
    }
    else
    {
        // Have to put outside current parent
        if ( !Parent ) return;
        Parent->m_Child = m_Selected->m_Next;
        m_Selected->m_Next = Parent;
        Parent = m_Selected->m_Parent = Parent->m_Parent;
        MenuItem* Previous2 = GetPrevious(Parent);
        if ( Previous2 )
        {
            Previous2->m_Next = m_Selected;
        }
        else if ( Parent )
        {
            Parent->m_Child = m_Selected;
        }
        else
        {
            m_First = m_Selected;
        }
    }

    // Rebuilding tree
    UpdateMenuContent();
}

void wxsMenuEditor::OnButtonDownClick(wxCommandEvent& event)
{
    if ( !m_Selected ) return;

    MenuItem* Previous = GetPrevious(m_Selected);
    MenuItem* Next = m_Selected->m_Next;
    MenuItem* Parent = m_Selected->m_Parent;

    if ( Next )
    {
        if ( Previous )
        {
            Previous->m_Next = Next;
        }
        else if ( Parent )
        {
            Parent->m_Child = Next;
        }
        else
        {
            m_First = Next;
        }

        m_Selected->m_Next = Next->m_Next;
        Next->m_Next = m_Selected;
    }
    else
    {
        // Last item of parent, need to jump out of menu
        if ( !Parent ) return;
        if ( Previous )
        {
            Previous->m_Next = NULL;
        }
        else
        {
            Parent->m_Child = NULL;
        }
        m_Selected->m_Next = Parent->m_Next;
        m_Selected->m_Parent = Parent->m_Parent;
        Parent->m_Next = m_Selected;
    }

    // Rebuilding tree
    UpdateMenuContent();
}

wxsMenuEditor::MenuItem* wxsMenuEditor::GetPrevious(MenuItem* Item)
{
    MenuItem* Parent = Item->m_Parent;
    if ( !Parent && Item == m_First         ) return NULL;
    if (  Parent && Item == Parent->m_Child ) return NULL;

    for ( MenuItem* Prev = Parent ? Parent->m_Child : m_First ; Prev; Prev = Prev->m_Next )
    {
        if ( Prev->m_Next == Item ) return Prev;
    }
    return NULL;
}

void wxsMenuEditor::OnButtonNewClick(wxCommandEvent& event)
{
    MenuItem* NewItem = new MenuItem;
    NewItem->m_Type = wxsMenu::Normal;
    NewItem->m_Label = _("New Menu");
    NewItem->m_Child = NULL;

    if ( !m_Selected )
    {
        // Just adding new item into m_Data
        NewItem->m_Parent = NULL;
        NewItem->m_Next = m_First;
        m_First = NewItem;
    }
    else
    {
        NewItem->m_Parent = m_Selected->m_Parent;
        NewItem->m_Next = m_Selected->m_Next;
        m_Selected->m_Next = NewItem;
    }

    SelectItem(NewItem);
    UpdateMenuContent();
}

void wxsMenuEditor::OnButtonDelClick(wxCommandEvent& event)
{
    if ( !m_Selected ) return;

    if ( cbMessageBox(
        _("Are you sure to delete this menu item ?\n"
          "(It will delete all sub menus too)"),
        _("Delete menu"),
        wxYES_NO) != wxID_YES ) return;

    MenuItem* Previous = GetPrevious(m_Selected);
    MenuItem* Parent = m_Selected->m_Parent;

    if ( Previous )
    {
        Previous->m_Next = m_Selected->m_Next;
        m_Selected->m_Next = NULL;
        DeleteDataCopyReq(m_Selected);
        m_Selected = NULL;
        if ( Previous->m_Next )
        {
            SelectItem(Previous->m_Next);
        }
        else
        {
            SelectItem(Previous);
        }
    }
    else
    {
        if ( Parent )
        {
            Parent->m_Child = m_Selected->m_Next;
        }
        else
        {
            m_First = m_Selected->m_Next;
        }

        MenuItem* NewSelected = m_Selected;

        if ( m_Selected->m_Next )
        {
            NewSelected = m_Selected->m_Next;
        }
        else
        {
            NewSelected = m_Selected->m_Parent;
        }

        m_Selected->m_Next = NULL;
        DeleteDataCopyReq(m_Selected);
        m_Selected = NULL;
        SelectItem(NewSelected);
    }

    UpdateMenuContent();
}

void wxsMenuEditor::OnButtonLeftClick(wxCommandEvent& event)
{
    if ( !m_Selected ) return;
    if ( !m_Selected->m_Parent ) return;

    MenuItem* Previous = GetPrevious(m_Selected);
    MenuItem* Parent = m_Selected->m_Parent;

    if ( Previous )
    {
        Previous->m_Next = m_Selected->m_Next;
    }
    else
    {
        Parent->m_Child = m_Selected->m_Next;
    }

    m_Selected->m_Parent = Parent->m_Parent;
    m_Selected->m_Next = Parent->m_Next;
    Parent->m_Next = m_Selected;

    UpdateMenuContent();
}


void wxsMenuEditor::OnButtonRightClick(wxCommandEvent& event)
{
    if ( !m_Selected ) return;

    MenuItem* Previous = GetPrevious(m_Selected);
    if ( !Previous ) return;
    if ( Previous->m_Type == wxsMenu::Separator ) return;
    if ( Previous->m_Type == wxsMenu::Break ) return;

    Previous->m_Type = wxsMenu::Normal;
    Previous->m_Next = m_Selected->m_Next;
    m_Selected->m_Parent = Previous;
    m_Selected->m_Next = NULL;

    if ( !Previous->m_Child )
    {
        Previous->m_Child = m_Selected;
    }
    else
    {
        Previous = Previous->m_Child;
        while ( Previous->m_Next )
        {
            Previous = Previous->m_Next;
        }

        Previous->m_Next = m_Selected;
    }

    UpdateMenuContent();
}

void wxsMenuEditor::OnLabelChanged(wxCommandEvent& event)
{
    SelectItem(m_Selected);
    if ( m_Selected && m_Selected->m_TreeId.IsOk() )
    {
        m_Content->SetItemText(m_Selected->m_TreeId,GetItemTreeName(m_Selected));
    }
}

void wxsMenuEditor::CheckConsistency()
{
    //CheckConsistencyReq(m_First);
}

void wxsMenuEditor::CheckConsistencyReq(MenuItem* Item)
{
    if ( !Item ) return;
    MenuItem* Parent = Item->m_Parent;

    for ( MenuItem* Scan = Item; Scan; Scan = Scan->m_Next )
    {
        for ( MenuItem* LoopScan = Scan->m_Next; LoopScan; LoopScan = LoopScan->m_Next )
        {
            if ( LoopScan == Scan )
            {
                // We've got looping sequence
                wxMessageBox(_T("wxSmith: Loop"));
                return;
            }
        }

        if ( Scan->m_Parent != Parent )
        {
            wxMessageBox(_T("wxSmith: Parent"));
            return;
        }

        CheckConsistencyReq(Scan->m_Child);
    }
}
