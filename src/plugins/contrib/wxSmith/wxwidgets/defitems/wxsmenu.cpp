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

#include "wxsmenu.h"
#include "wxsmenueditor.h"
#include "../wxsitemresdata.h"
#include <globals.h>

namespace
{
    wxsRegisterItem<wxsMenu> Reg(_T("Menu"),wxsTTool,_T("Tools"),90);

    class MenuEditorDialog: public wxDialog
    {
        public:

            wxsMenuEditor* Editor;

            MenuEditorDialog(wxsMenu* Menu):
                wxDialog(NULL,-1,_("Menu editor"),wxDefaultPosition,wxDefaultSize)
            {
                wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
                Sizer->Add(Editor = new wxsMenuEditor(this,Menu),1,wxEXPAND,0);
                Sizer->Add(CreateButtonSizer(wxOK|wxCANCEL),0,wxEXPAND,5);
                SetSizer(Sizer);
                Sizer->SetSizeHints(this);
                PlaceWindow(this,pdlCentre,true);
            }

            void OnOK(wxCommandEvent& event)
            {
                Editor->ApplyChanges();
                EndModal(wxID_OK);
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(MenuEditorDialog,wxDialog)
        EVT_BUTTON(wxID_OK,MenuEditorDialog::OnOK)
    END_EVENT_TABLE()
}

wxsMenu::wxsMenu(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info),
    m_Type(Menu),
    m_Enabled(true),
    m_Checked(false)
{
}

wxSize wxsMenu::CalculateSize(wxArrayInt* Cols)
{
    if ( m_Type != Menu )
    {
        return wxSize(0,0);
    }

    wxBitmap EmptyBitmap(10,10);
    wxMemoryDC DC;
    DC.SelectObject(EmptyBitmap);

    wxSize TotalSize(0,0);
    wxSize ColumnSize(0,0);
    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsMenu* Child = (wxsMenu*)GetChild(i);
        switch ( Child->m_Type )
        {
            case Break:
            {
                if ( Cols )
                {
                    Cols->Add(ColumnSize.GetWidth());
                }

                TotalSize.SetWidth(TotalSize.GetWidth()+ColumnSize.GetWidth()+8);
                if ( TotalSize.GetHeight() < ColumnSize.GetHeight() )
                {
                    TotalSize.SetHeight(ColumnSize.GetHeight());
                }
                ColumnSize.Set(0,0);
                break;
            }

            case Separator:
            {
                ColumnSize.SetHeight(ColumnSize.GetHeight() + 8);
                break;
            }

            case Menu:
            case Normal:
            case Radio:
            case Check:
            {
                wxCoord W, H;
                wxString Text = m_Label;
                if ( !m_Accelerator.IsEmpty() ) Text << _T(' ') << m_Accelerator;
                DC.GetTextExtent(Text,&W,&H);
                W += 36;
                ColumnSize.SetHeight(ColumnSize.GetHeight() + H);
                if ( ColumnSize.GetWidth() < W )
                {
                    ColumnSize.SetWidth(W);
                }
                break;
            }
        }
    }

    if ( Cols )
    {
        Cols->Add(ColumnSize.GetWidth());
    }

    return wxSize(TotalSize.GetWidth() + ColumnSize.GetWidth() + 4,
                  TotalSize.GetHeight() + ColumnSize.GetHeight() + 4);
}

void wxsMenu::Draw(wxDC* DC,int BegX,int BegY)
{
    if ( m_Type != Menu )
    {
        return;
    }
    wxColour Back(0x80,0x80,0x80);
    wxColour Light(0xC0,0xC0,0xC0);
    wxColour Dark(0x40,0x40,0x40);
    wxArrayInt Cols;
    wxSize Size = CalculateSize(&Cols);
    DC->SetBrush(Back);
    DC->SetPen(Light);
    DC->DrawRectangle(BegX,BegY,Size.GetWidth(),Size.GetHeight());
    DC->SetPen(Dark);
    DC->DrawLine(BegX+Size.GetWidth(),BegY, BegX+Size.GetWidth(),BegY+Size.GetHeight());
    DC->DrawLine(BegX,BegY+Size.GetHeight(),BegX+Size.GetWidth(),BegY+Size.GetHeight());
    int PosX = BegX + 2;
    int PosY = BegY + 2;
    int Column = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsMenu* Child = (wxsMenu*)GetChild(i);
        switch ( Child->m_Type )
        {
            case Break:
            {
                PosX += Cols[Column++];
                PosY = BegY + 2;
                DC->SetPen(Dark);
                DC->DrawLine(PosX+3,PosY,PosX+3,PosY+Size.GetHeight()-4);
                DC->SetPen(Light);
                DC->DrawLine(PosX+4,PosY,PosX+4,PosY+Size.GetHeight()-4);
                PosX += 8;
                break;
            }

            case Separator:
            {
                DC->SetPen(Dark);
                DC->DrawLine(PosX,PosY+3,PosX+Cols[Column],PosY+3);
                DC->SetPen(Light);
                DC->DrawLine(PosX,PosY+4,PosX+Cols[Column],PosY+4);
                PosY += 8;
                break;
            }

            case Menu:
            case Normal:
            case Radio:
            case Check:
            {
                DC->SetPen(*wxBLACK);
                DC->DrawText(m_Label,PosX,PosY);
                wxCoord W, H;
                DC->GetTextExtent(m_Label,&W,&H);
                PosY += H;
                break;
            }
        }
    }
}

void wxsMenu::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
}

void wxsMenu::OnEnumToolProperties(long Flags)
{
}

void wxsMenu::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
}

bool wxsMenu::OnMouseDClick(wxWindow* Preview,int PosX,int PosY)
{
    MenuEditorDialog Dlg(this);
    return Dlg.ShowModal() == wxID_OK;
}
