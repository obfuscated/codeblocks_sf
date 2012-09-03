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
        100,
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
#if wxCHECK_VERSION(2,9,0)
                DC.SetBrush(wxBrush(wxColour(0,0,0),wxHATCHSTYLE_CROSSDIAG));
#else
                DC.SetBrush(wxBrush(wxColour(0,0,0),wxCROSSDIAG_HATCH));
#endif
                DC.SetPen(wxPen(wxColour(0,0,0),1));
                DC.DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsSpacerPreview,wxPanel)
        EVT_PAINT(wxsSpacerPreview::OnPaint)
    END_EVENT_TABLE()

}

wxsSpacer::wxsSpacer(wxsItemResData* Data): wxsItem(Data,&Reg.Info,flSize,0,0)
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

void wxsSpacer::OnBuildCreatingCode()
{
    int Index = GetParent()->GetChildIndex(this);
    wxsSizerExtra* Extra = (wxsSizerExtra*) GetParent()->GetChildExtra(Index);

    if ( Extra == 0 ) return;

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            wxsSizeData& Size = GetBaseProps()->m_Size;
            if ( Size.DialogUnits )
            {
                // We use 'SpacerSizes' extra variable to keep count of currently added spacer sizes
                // length of this extra string indicates current spacer size number
                wxString SizeName = GetCoderContext()->GetUniqueName(_T("__SpacerSize"));

                Codef(_T("wxSize %s = %z;\n")
                      _T("%MAdd(%s.GetWidth(),%s.GetHeight(),%s);\n"),
                      SizeName.wx_str(),
                      &Size,
                      SizeName.wx_str(),
                      SizeName.wx_str(),
                      Extra->AllParamsCode(GetCoderContext()).wx_str());
            }
            else
            {
                Codef(_T("%MAdd(%d,%d,%s);\n"),
                    (int)Size.X,
                    (int)Size.Y,
                    Extra->AllParamsCode(GetCoderContext()).wx_str());
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSpacer::OnBuildCreatingCode"),GetLanguage());
        }
    }
}
