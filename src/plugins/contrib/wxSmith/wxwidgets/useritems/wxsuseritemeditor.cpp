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

#include "wxsuseritemeditor.h"
#include "wxsuseritemmanager.h"

//(*InternalHeaders(wxsUserItemEditor)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(wxsUserItemEditor)
const long wxsUserItemEditor::ID_LISTBOX1 = wxNewId();
const long wxsUserItemEditor::ID_BUTTON1 = wxNewId();
const long wxsUserItemEditor::ID_BUTTON5 = wxNewId();
const long wxsUserItemEditor::ID_BUTTON2 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT1 = wxNewId();
const long wxsUserItemEditor::ID_TEXTCTRL1 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT2 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT3 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT7 = wxNewId();
const long wxsUserItemEditor::ID_BITMAPBUTTON1 = wxNewId();
const long wxsUserItemEditor::ID_BITMAPBUTTON2 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT4 = wxNewId();
const long wxsUserItemEditor::ID_BITMAPBUTTON3 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT5 = wxNewId();
const long wxsUserItemEditor::ID_BUTTON3 = wxNewId();
const long wxsUserItemEditor::ID_STATICTEXT6 = wxNewId();
const long wxsUserItemEditor::ID_BUTTON4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsUserItemEditor,wxDialog)
	//(*EventTable(wxsUserItemEditor)
	//*)
	EVT_BUTTON(wxID_OK,wxsUserItemEditor::OnOk)
	EVT_BUTTON(wxID_CANCEL,wxsUserItemEditor::OnCancel)
END_EVENT_TABLE()

wxsUserItemEditor::wxsUserItemEditor(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxsUserItemEditor)
	Create(parent,id,_("User item editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,_T("wxDialog"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Item list"));
	m_ItemList = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_ItemList,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	m_Add = new wxButton(this,ID_BUTTON1,_("Add"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON1"));
	BoxSizer3->Add(m_Add,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	m_Duplicate = new wxButton(this,ID_BUTTON5,_("Duplicate"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON5"));
	BoxSizer3->Add(m_Duplicate,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer1->Add(BoxSizer3,0,wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	m_Delete = new wxButton(this,ID_BUTTON2,_("Delete"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON2"));
	StaticBoxSizer1->Add(m_Delete,0,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer2->Add(StaticBoxSizer1,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Description"));
	FlexGridSizer1 = new wxFlexGridSizer(0,2,0,0);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Name:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	m_Name = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(m_Name,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Icons:"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE,_T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2 = new wxFlexGridSizer(0,3,0,0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableCol(2);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("16x16"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(10,10,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticText7 = new wxStaticText(this,ID_STATICTEXT7,_("32x32"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT7"));
	FlexGridSizer2->Add(StaticText7,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	m_Icon16 = new wxBitmapButton(this,ID_BITMAPBUTTON1,wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_("wxART_TIP")),_T("wxART_BUTTON_C")),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,wxDefaultValidator,_T("ID_BITMAPBUTTON1"));
	m_Icon16->SetDefault();
	FlexGridSizer2->Add(m_Icon16,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(10,10,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	m_Icon32 = new wxBitmapButton(this,ID_BITMAPBUTTON2,wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_("wxART_TIP")),_T("wxART_BUTTON_C")),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,wxDefaultValidator,_T("ID_BITMAPBUTTON2"));
	m_Icon32->SetDefault();
	FlexGridSizer2->Add(m_Icon32,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(FlexGridSizer2,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Preview image:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	m_PreviewBitmap = new wxBitmapButton(this,ID_BITMAPBUTTON3,wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_("wxART_TIP")),_T("wxART_BUTTON_C")),wxDefaultPosition,wxSize(68,64),wxBU_AUTODRAW,wxDefaultValidator,_T("ID_BITMAPBUTTON3"));
	m_PreviewBitmap->SetDefault();
	FlexGridSizer1->Add(m_PreviewBitmap,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Properties:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	m_PropertiesEdit = new wxButton(this,ID_BUTTON3,_("Edit"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON3"));
	FlexGridSizer1->Add(m_PropertiesEdit,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	StaticText6 = new wxStaticText(this,ID_STATICTEXT6,_("Source code:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText6,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	m_SourceCodeEdit = new wxButton(this,ID_BUTTON4,_("Edit"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BUTTON4"));
	FlexGridSizer1->Add(m_SourceCodeEdit,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StaticBoxSizer2->Add(FlexGridSizer1,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer2->Add(StaticBoxSizer2,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer1->Add(BoxSizer2,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_OK,wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&wxsUserItemEditor::OnItemListSelect);
	//*)

    FillItemsList();
    if ( m_ItemList->GetCount() == 0 )
    {
        m_ItemList->SetSelection(wxNOT_FOUND);
        SelectItem(NULL);
    }
    else
    {
        m_ItemList->SetSelection(0);
        SelectItem((ItemDataCopy*)m_ItemList->GetClientObject(0));
    }
}

wxsUserItemEditor::~wxsUserItemEditor()
{
	//(*Destroy(wxsUserItemEditor)
	//*)
}

void wxsUserItemEditor::FillItemsList()
{
    wxsUserItemManager& Manager = wxsUserItemManager::Get();

    for ( int i=0; i<Manager.GetItemsCount(); i++ )
    {
        wxsUserItemDescription* Desc = Manager.GetDescription(i);
        ItemDataCopy* Data = new ItemDataCopy;
        Data->m_OriginalIndex = i;
        Data->m_Name = Desc->GetName();
        Data->m_Icon16 = Desc->GetInfo()->Icon16;
        Data->m_Icon32 = Desc->GetInfo()->Icon32;
        Data->m_PreviewBitmap = Desc->GetPreviewBitmap();
        m_ItemList->Append(Data->m_Name,Desc);
    }
}

void wxsUserItemEditor::SelectItem(ItemDataCopy* NewSelection)
{
    StoreCurrentItem();
    m_Selected = NewSelection;

    if ( m_Selected )
    {
        m_Duplicate->Enable();
        m_Delete->Enable();
        m_Name->Enable();
        m_Icon16->Enable();
        m_Icon32->Enable();
        m_PreviewBitmap->Enable();
        m_PropertiesEdit->Enable();
        m_SourceCodeEdit->Enable();

        m_Name->SetValue(m_Selected->m_Name);
        m_Icon16->SetBitmapLabel(m_Selected->m_Icon16);
        m_Icon32->SetBitmapLabel(m_Selected->m_Icon32);
        m_PreviewBitmap->SetBitmapLabel(m_Selected->m_PreviewBitmap);
    }
    else
    {
        m_Duplicate->Disable();
        m_Delete->Disable();
        m_Name->Disable();
        m_Icon16->Disable();
        m_Icon32->Disable();
        m_PreviewBitmap->Disable();
        m_PropertiesEdit->Disable();
        m_SourceCodeEdit->Disable();

        m_Name->SetValue(_T(""));
    }
}

void wxsUserItemEditor::StoreCurrentItem()
{
    if ( !m_Selected ) return;

    m_Selected->m_Name = m_Name->GetValue();
    m_Selected->m_Icon16 = m_Icon16->GetBitmapLabel();
    m_Selected->m_Icon32 = m_Icon32->GetBitmapLabel();
    m_Selected->m_PreviewBitmap = m_PreviewBitmap->GetBitmapLabel();
}

void wxsUserItemEditor::OnItemListSelect(wxCommandEvent& event)
{
    int Selection = m_ItemList->GetSelection();
    if ( Selection == wxNOT_FOUND )
    {
        SelectItem(NULL);
        return;
    }

    SelectItem((ItemDataCopy*)m_ItemList->GetClientObject(Selection));
}

void wxsUserItemEditor::OnOk(wxCommandEvent& event)
{
    StoreCurrentItem();
    wxsUserItemManager& Manager = wxsUserItemManager::Get();

    // Looking for some possible name conflicts
    wxArrayString AllNewNames;
    Manager.UnregisterAll();
    for ( int i=0; i<m_ItemList->GetCount(); i++ )
    {
        ItemDataCopy* Data = ((ItemDataCopy*)m_ItemList->GetClientObject(i));
        wxString Name = Data->m_Name;
        if ( Name.IsEmpty() )
        {
            cbMessageBox(_("Item must have non-empty name"),_("Invalid item name"));
            m_ItemList->SetSelection(i);
            SelectItem(Data);
            return;
        }

        if ( !wxsCodeMarks::ValidateIdentifier(wxsCPP,Name) )
        {
            cbMessageBox(_("Invalid name for item, please use valid type name"),_("Invalid item name"));
            m_ItemList->SetSelection(i);
            SelectItem(Data);
            return;
        }

        if ( AllNewNames.Index(Name) != wxNOT_FOUND )
        {
            cbMessageBox(_("Two items have same name"),_("Invalid item name"));
            m_ItemList->SetSelection(i);
            SelectItem(Data);
            return;
        }

        if ( wxsItemFactory::GetInfo(Name) )
        {
            cbMessageBox(_("Used name of already defined item"),_("Invalid item name"));
            m_ItemList->SetSelection(i);
            SelectItem(Data);
            return;
        }
    }

    // Updating items which/ have been managed before
    bool* UserMask = new bool[Manager.GetItemsCount()];
    for ( int i=0; i<Manager.GetItemsCount(); i++ ) UserMask[i] = false;
    for ( int i=0; i<m_ItemList->GetCount(); i++ )
    {
        ItemDataCopy* Data = (ItemDataCopy*)m_ItemList->GetClientObject(i);
        if ( Data->m_OriginalIndex < 0 ) continue;

        wxsUserItemDescription* Desc = Manager.GetDescription(Data->m_OriginalIndex);
        if ( !Desc )
        {
            Data->m_OriginalIndex = -1;
            continue;
        }

        UserMask[Data->m_OriginalIndex] = true;

        Desc->SetName(Data->m_Name);
        Desc->GetInfo()->Icon16 = Data->m_Icon16;
        Desc->GetInfo()->Icon32 = Data->m_Icon32;
        Desc->GetPreviewBitmap() = Data->m_PreviewBitmap;
        Desc->UpdateTreeIcon();
    }

    // Deleting items which were deleted
    for ( int i=Manager.GetItemsCount(); i-->0; )
    {
        if ( !UserMask[i] )
        {
            Manager.DeleteDescription(i);
        }
    }
    delete[] UserMask;

    // Adding new items
    for ( int i=0; i<m_ItemList->GetCount(); i++ )
    {
        ItemDataCopy* Data = (ItemDataCopy*)m_ItemList->GetClientObject(i);
        if ( Data->m_OriginalIndex >= 0 ) continue;

        // Adding new item
        wxsUserItemDescription* NewDesc = new wxsUserItemDescription(Data->m_Name);
        NewDesc->GetInfo()->ClassName = Data->m_Name;
        NewDesc->GetInfo()->Type = wxsTWidget;
        NewDesc->GetInfo()->License = _T("");
        NewDesc->GetInfo()->Author = _T("");
        NewDesc->GetInfo()->Email = _T("");
        NewDesc->GetInfo()->Site = _T("");
        NewDesc->GetInfo()->Category = _T("User-defined");
        NewDesc->GetInfo()->Priority = 50;
        NewDesc->GetInfo()->DefaultVarName = Data->m_Name;
        NewDesc->GetInfo()->Languages = wxsCPP;
        NewDesc->GetInfo()->VerHi = 0;
        NewDesc->GetInfo()->VerLo = 0;
        NewDesc->GetInfo()->Icon16 = Data->m_Icon16;
        NewDesc->GetInfo()->Icon32 = Data->m_Icon32;
        NewDesc->GetInfo()->AllowInXRC = false;

        NewDesc->UpdateTreeIcon();

        Manager.AddDescription(NewDesc);
    }

    Manager.ReregisterAll();
    EndModal(wxID_OK);
}

void wxsUserItemEditor::OnCancel(wxCommandEvent& event)
{
    wxsUserItemManager::Get().ReregisterAll();
    EndModal(wxID_CANCEL);
}
