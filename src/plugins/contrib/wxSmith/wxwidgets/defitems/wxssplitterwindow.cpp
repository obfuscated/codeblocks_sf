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

#include "wxssplitterwindow.h"

#include <wx/splitter.h>

namespace
{
    const long    OrientValues[] = { wxHORIZONTAL, wxVERTICAL, 0 };
    const wxChar* OrientNames[]  = { _T("horizontal"), _T("vertical"), NULL };

    class OrientProp: public wxsEnumProperty
    {
        public:
            OrientProp(int Offset):
                wxsEnumProperty(
                    _("Orientation"),
                    _T("orientation"),
                    Offset,
                    OrientValues,
                    OrientNames,
                    false,
                    wxHORIZONTAL,
                    true)
            {}

            virtual const wxString GetTypeName()
            {
                return _T("Orientation");
            }
    };

    wxsRegisterItem<wxsSplitterWindow> Reg(_T("SplitterWindow"),wxsTContainer,_T("Layout"),30);

    WXS_ST_BEGIN(wxsSplitterWindowStyles,_T("wxSP_3D"))
        WXS_ST_CATEGORY("wxSplitterWindow")
        WXS_ST(wxSP_3D)
        WXS_ST(wxSP_3DSASH)
        WXS_ST(wxSP_3DBORDER)
        WXS_ST(wxSP_FULLSASH)
        WXS_ST(wxSP_BORDER)
        WXS_ST(wxSP_NOBORDER)
        WXS_ST(wxSP_PERMIT_UNSPLIT)
        WXS_ST(wxSP_LIVE_UPDATE)
        WXS_ST_MASK(wxSP_NO_XP_THEME,wxsSFWin,0,true)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSplitterWindowEvents)
        WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGING,wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING,wxSplitterEvent,SashPosChanging)
        WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGED,wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,wxSplitterEvent,SashPosChanged)
        WXS_EVI(EVT_SPLITTER_UNSPLIT,wxEVT_COMMAND_SPLITTER_UNSPLIT,wxSplitterEvent,Unsplit)
        WXS_EVI(EVT_SPLITTER_DCLICK,wxEVT_COMMAND_SPLITTER_DCLICK,wxSplitterEvent,DClick)
    WXS_EV_END()
}

wxsSplitterWindow::wxsSplitterWindow(wxsItemResData* Data):
    wxsContainer(Data,&Reg.Info,wxsSplitterWindowEvents,wxsSplitterWindowStyles),
    SashPos(0),
    MinSize(10),
    Orientation(wxHORIZONTAL)
{
}

wxObject* wxsSplitterWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSplitterWindow* Splitter = new wxSplitterWindow(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    SetupWindow(Splitter,Flags);
    if ( MinSize != -1 )
    {
        Splitter->SetMinimumPaneSize(MinSize);
    }
    AddChildrenPreview(Splitter,Flags);
    if ( GetChildCount() == 0 )
    {
    }
    else if ( GetChildCount() == 1 )
    {
        Splitter->Initialize(wxDynamicCast(GetChild(0)->GetLastPreview(),wxWindow));
    }
    else
    {
        if ( Orientation == wxHORIZONTAL )
        {
            Splitter->SplitHorizontally(
                wxDynamicCast(GetChild(0)->GetLastPreview(),wxWindow),
                wxDynamicCast(GetChild(1)->GetLastPreview(),wxWindow),
                SashPos);
        }
        else
        {
            Splitter->SplitVertically(
                wxDynamicCast(GetChild(0)->GetLastPreview(),wxWindow),
                wxDynamicCast(GetChild(1)->GetLastPreview(),wxWindow),
                SashPos);
        }

        // Some trick to faster relayout splitter window
        Splitter->OnInternalIdle();
    }

    return Splitter;
}

void wxsSplitterWindow::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%P,%S,%T,%N);\n"));
            SetupWindowCode(Code,wxsCPP);
            if ( MinSize != -1 ) Code << Codef(Language,_T("%ASetMinimumPaneSize(%d);\n"),MinSize);
            AddChildrenCode(Code,wxsCPP);
            if ( GetChildCount() == 0 )
            {
            }
            else if ( GetChildCount() == 1 )
            {
                Code << Codef(Language,_T("%AInitialize(%v);\n"),GetChild(0)->GetVarName().c_str());
            }
            else
            {
                Code << Codef(Language,_T("%ASplit%s(%v,%v);\n"),
                            (Orientation==wxHORIZONTAL) ? _T("Horizontally") : _T("Vertically"),
                            GetChild(0)->GetVarName().c_str(),
                            GetChild(1)->GetVarName().c_str());
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSplitterWindow::OnBuildCreatingCode"),Language);
        }
    }
}

void wxsSplitterWindow::OnEnumContainerProperties(long Flags)
{
    WXS_LONG(wxsSplitterWindow,SashPos,0,_("Sash position"),_T("sashpos"),0);
    WXS_LONG(wxsSplitterWindow,MinSize,0,_("Min. pane size"),_T("minsize"),-1);
    static OrientProp Prop(wxsOFFSET(wxsSplitterWindow,Orientation));
    Property(Prop,0);
}

void wxsSplitterWindow::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/splitter.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsSplitterWindow::OnEnumDeclFiles"),Language);
    }
}

bool wxsSplitterWindow::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    // TODO: Allow more tools
	if ( GetChildCount() == 2 )
	{
	    if ( ShowMessage )
	    {
            wxMessageBox(_("Splitter can have at most 2 children"));
	    }
		return false;
	}

	if ( Item->GetType() == wxsTSizer )
	{
	    if ( ShowMessage )
	    {
            wxMessageBox(_("Can not add sizer into Splitter.\nAdd panels first"));
	    }
		return false;
	}

	if ( Item->GetType() == wxsTSpacer )
	{
	    if ( ShowMessage )
	    {
            wxMessageBox(_("Spacer can be added to sizers only"));
	    }
		return false;
	}

    return true;
}
