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

#include "wxsspacer.h"

#include "../wxssizer.h"

namespace
{
    wxsRegisterItem<wxsSpacer> Reg(
        _T("Spacer"),
        wxsTSpacer,
        _("wxWidgets license"),
        _("wxWidgets team"),
        _T(""),
        _T("www.wxwidgets.org"),
        _T("Layout"),
        30,
        _T(""),
        wxsCPP,
        2,6,
        _T("images/wxsmith/Spacer32.png"),
        _T("images/wxsmith/Spacer16.png"));

    class wxsSpacerPreview: public wxPanel
    {
        public:
            wxsSpacerPreview(wxWindow* Parent,const wxSize& Size):
                wxPanel(Parent,-1,wxDefaultPosition,Size)
            {}

        private:

            void OnPaint(wxPaintEvent& event)
            {
                wxPaintDC DC(this);
                DC.SetBrush(wxBrush(wxColour(0,0,0),wxCROSSDIAG_HATCH));
                DC.SetPen(wxPen(wxColour(0,0,0),1));
                DC.DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsSpacerPreview,wxPanel)
        EVT_PAINT(wxsSpacerPreview::OnPaint)
    END_EVENT_TABLE()

}

wxsSpacer::wxsSpacer(wxsItemResData* Data): wxsItem(Data,&Reg.Info,flSize,0)
{}

void wxsSpacer::OnEnumItemProperties(long Flags)
{}

wxObject* wxsSpacer::OnBuildPreview(wxWindow* Parent,long Flags)
{
    if ( Flags & pfExact )
    {
        wxSize Sz = GetBaseProps()->m_Size.GetSize(Parent);
        return new wxSizerItem(Sz.GetWidth(),Sz.GetHeight(),0,0,0,0);
    }
    return new wxsSpacerPreview(Parent,GetBaseProps()->m_Size.GetSize(Parent));
}

void wxsSpacer::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    int Index = GetParent()->GetChildIndex(this);
    wxsSizerExtra* Extra = (wxsSizerExtra*) GetParent()->GetChildExtra(Index);
    wxString ParentName = GetParent()->GetVarName();

    if ( Extra == 0 ) return;

    switch ( Language )
    {
        case wxsCPP:
        {
            wxsSizeData& Size = GetBaseProps()->m_Size;
            if ( Size.DialogUnits )
            {
                wxString SizeName = ParentName + wxString::Format(_T("SpacerSize%d"),Index);
                Code << _T("wxSize ") << SizeName << _T(" = ") << Size.GetSizeCode(WindowParent,wxsCPP) << _T(";\n")
                     << ParentName << _T("->Add(")
                     << SizeName << _T(".GetWidth(),")
                     << SizeName << _T(".GetHeight(),")
                     << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
            }
            else
            {
                Code << ParentName << wxString::Format(_T("->Add(%d,%d,"),Size.X,Size.Y)
                     << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSpacer::OnBuildCreatingCode"),Language);
        }
    }
}
