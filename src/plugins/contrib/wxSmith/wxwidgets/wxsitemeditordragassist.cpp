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

#include "wxsitemeditordragassist.h"

#include <manager.h>

wxsItemEditorDragAssist::wxsItemEditorDragAssist(wxsItemEditorContent* Content):
    m_PreviousTarget(0),
    m_PreviousParent(0),
    m_PreviousAddAfter(false),
    m_TargetBitmap(0),
    m_TargetRect(0,0,0,0),
    m_IsTarget(false),
    m_ParentBitmap(0),
    m_ParentRect(0,0,0,0),
    m_IsParent(false),
    m_Content(Content)
{
}

wxsItemEditorDragAssist::~wxsItemEditorDragAssist()
{
    if ( m_TargetBitmap ) delete m_TargetBitmap;
    if ( m_ParentBitmap ) delete m_ParentBitmap;
}

void wxsItemEditorDragAssist::NewDragging()
{
    m_PreviousTarget = 0;
    m_PreviousParent = 0;
    if ( m_TargetBitmap )
    {
        delete m_TargetBitmap;
        m_TargetBitmap = 0;
    }
    if ( m_ParentBitmap )
    {
        delete m_ParentBitmap;
        m_ParentBitmap = 0;
    }
    m_IsTarget = false;
    m_IsParent = false;
}

void wxsItemEditorDragAssist::DrawExtra(wxsItem* Target,wxsItem* Parent,bool AddAfter,wxDC* DC)
{
    UpdateAssist(Target,Parent,AddAfter);

    if ( m_IsParent )
    {
        switch ( AssistType() )
        {
            case dtOutline:
                DC->SetPen(wxPen(ParentColour(),2,wxSOLID));
                DC->DrawRectangle(m_ParentRect.x,m_ParentRect.y,m_ParentRect.width,m_ParentRect.height);
                break;

            case dtColourMix:
                DC->DrawBitmap(*m_ParentBitmap,m_ParentRect.x,m_ParentRect.y);
                break;

            default:;
        }
    }

    if ( m_IsTarget )
    {
        switch ( AssistType() )
        {
            case dtOutline:
                DC->SetPen(wxPen(TargetColour(),2,wxSOLID));
                DC->DrawRectangle(m_TargetRect.x,m_TargetRect.y,m_TargetRect.width,m_TargetRect.height);
                break;

            case dtColourMix:
                DC->DrawBitmap(*m_TargetBitmap,m_TargetRect.x,m_TargetRect.y);
                break;

            default:;
        }
    }
}

void wxsItemEditorDragAssist::UpdateAssist(wxsItem* NewTarget,wxsItem* NewParent,bool NewAddAfter)
{
    if ( NewParent != m_PreviousParent )
    {
        m_PreviousParent = NewParent;
        RebuildParentAssist();
    }
    if ( NewTarget != m_PreviousTarget || NewAddAfter != m_PreviousAddAfter )
    {
        m_PreviousTarget = NewTarget;
        m_PreviousAddAfter = NewAddAfter;
        RebuildTargetAssist();
    }

}

void wxsItemEditorDragAssist::RebuildParentAssist()
{
    int PosX;
    int PosY;
    int SizeX;
    int SizeY;

    if ( m_PreviousParent && m_Content->FindAbsoluteRect(m_PreviousParent,PosX,PosY,SizeX,SizeY) )
    {
        m_ParentRect = wxRect(PosX,PosY,SizeX,SizeY);

        if ( m_ParentBitmap )
        {
            delete m_ParentBitmap;
            m_ParentBitmap = 0;
        }

        switch ( AssistType() )
        {
            case dtColourMix:
                {
                    const wxBitmap& Bmp = m_Content->GetBitmap();
                    UpdateRect(m_ParentRect,Bmp);
                    wxImage ParentImg = Bmp.GetSubBitmap(m_ParentRect).ConvertToImage();
                    ColourMix(ParentImg,ParentColour());
                    m_ParentBitmap = new wxBitmap(ParentImg);
                }
                break;

            default:;
        }

        m_IsParent = true;
    }
    else
    {
        if ( m_ParentBitmap )
        {
            delete m_ParentBitmap;
            m_ParentBitmap = 0;
        }
        m_IsParent = false;
    }
}

void wxsItemEditorDragAssist::RebuildTargetAssist()
{
    int PosX;
    int PosY;
    int SizeX;
    int SizeY;

    if ( m_PreviousTarget && m_Content->FindAbsoluteRect(m_PreviousTarget,PosX,PosY,SizeX,SizeY) )
    {
        SizeX = SizeX/2;
        if ( m_PreviousAddAfter ) PosX = PosX + SizeX;

        m_TargetRect = wxRect(PosX,PosY,SizeX,SizeY);

        if ( m_TargetBitmap )
        {
            delete m_TargetBitmap;
            m_TargetBitmap = 0;
        }

        switch ( AssistType() )
        {
            case dtColourMix:
                {
                    const wxBitmap& Bmp = m_Content->GetBitmap();
                    UpdateRect(m_TargetRect,Bmp);
                    wxImage TargetImg = Bmp.GetSubBitmap(m_TargetRect).ConvertToImage();
                    ColourMix(TargetImg,TargetColour());
                    m_TargetBitmap = new wxBitmap(TargetImg);
                }
                break;

            default:;
        }

        m_IsTarget = true;
    }
    else
    {
        if ( m_TargetBitmap )
        {
            delete m_TargetBitmap;
            m_TargetBitmap = 0;
        }
        m_IsTarget = false;
    }
}

inline wxsItemEditorDragAssist::DragAssistType wxsItemEditorDragAssist::AssistType()
{
    return (DragAssistType)Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragassisttype"),(long)dtColourMix);
}

inline wxColour wxsItemEditorDragAssist::TargetColour()
{
    int Col = Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragtargetcol"),0x608CDFL);
    return wxColour((Col>>16)&0xFF,(Col>>8)&0xFF,Col&0xFF);
}

inline wxColour wxsItemEditorDragAssist::ParentColour()
{
    int Col = Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/dragparentcol"),0x0D177BL);
    return wxColour((Col>>16)&0xFF,(Col>>8)&0xFF,Col&0xFF);
}

void wxsItemEditorDragAssist::ColourMix(wxImage& Image,const wxColour& Col)
{
    int R = Col.Red();
    int G = Col.Green();
    int B = Col.Blue();
    for ( int y = Image.GetHeight(); y-->0; )
        for ( int x = Image.GetWidth(); x-->0; )
        {
            Image.SetRGB(x,y,
                ((int)Image.GetRed(x,y)   + R ) / 2,
                ((int)Image.GetGreen(x,y) + G ) / 2,
                ((int)Image.GetBlue(x,y)  + B ) / 2);
        }
}

void wxsItemEditorDragAssist::UpdateRect(wxRect& Rect,const wxBitmap& Bmp)
{
    if ( Rect.x < 0 )
    {
        Rect.width += Rect.x;
        Rect.x = 0;
    }
    if ( Rect.y < 0 )
    {
        Rect.height += Rect.y;
        Rect.y = 0;
    }

    if ( Rect.width > Bmp.GetWidth() )
    {
        Rect.width = Bmp.GetWidth();
    }

    if ( Rect.height > Bmp.GetHeight() )
    {
        Rect.height = Bmp.GetHeight();
    }
}
