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

#include "wxssplitterwindow.h"

#include <wx/splitter.h>

namespace
{
    wxsRegisterItem<wxsSplitterWindow> Reg(_T("SplitterWindow"),wxsTContainer,_T("Layout"),30);

    WXS_ST_BEGIN(wxsSplitterWindowStyles,_T("wxSP_3D"))
        WXS_ST_CATEGORY("wxSplitterWindow")
        WXS_ST(wxSP_3D)
        WXS_ST(wxSP_3DSASH)
        WXS_ST(wxSP_3DBORDER)
        WXS_ST(wxSP_BORDER)
        WXS_ST(wxSP_NOBORDER)
        WXS_ST(wxSP_PERMIT_UNSPLIT)
        WXS_ST(wxSP_LIVE_UPDATE)
        WXS_ST(wxSP_NO_XP_THEME)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsSplitterWindowEvents)
        WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGING,wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING,wxSplitterEvent,SashPosChanging)
        WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGED,wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,wxSplitterEvent,SashPosChanged)
        WXS_EVI(EVT_SPLITTER_UNSPLIT,wxEVT_COMMAND_SPLITTER_UNSPLIT,wxSplitterEvent,Unsplit)
        WXS_EVI(EVT_SPLITTER_DCLICK,wxEVT_COMMAND_SPLITTER_DOUBLECLICKED,wxSplitterEvent,DClick)
    WXS_EV_END()
}

wxsSplitterWindow::wxsSplitterWindow(wxsItemResData* Data):
    wxsContainer(Data,&Reg.Info,wxsSplitterWindowEvents,wxsSplitterWindowStyles),
    SashPos(0),
    MinSize(10),
    Orientation(wxHORIZONTAL),
    SashGravity(0.5f)
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
        Splitter->SetSashGravity(SashGravity);

        // Some trick to faster relayout splitter window
        Splitter->OnInternalIdle();
    }

    return Splitter;
}

void wxsSplitterWindow::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/splitter.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/splitter.h>"),_T("wxSplitterEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            if ( MinSize != -1 ) Codef(_T("%ASetMinimumPaneSize(%d);\n"),MinSize);
            Codef(_T("%ASetSashGravity(%f);\n"),SashGravity);
            AddChildrenCode();
            if ( GetChildCount() == 0 )
            {
            }
            else if ( GetChildCount() == 1 )
            {
                Codef(_T("%AInitialize(%o);\n"),0);
            }
            else
            {
                Codef(_T("%ASplit%s(%o, %o);\n"),((Orientation==wxHORIZONTAL) ? _T("Horizontally") : _T("Vertically")),0,1);
                if ( SashPos != 0 ) Codef(_T("%ASetSashPosition(%d);\n"),SashPos);
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSplitterWindow::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

void wxsSplitterWindow::OnEnumContainerProperties(long Flags)
{
    static const long    OrientValues[] = { wxHORIZONTAL, wxVERTICAL, 0 };
    static const wxChar* OrientNames[]  = { _T("horizontal"), _T("vertical"), 0 };

    WXS_LONG(wxsSplitterWindow,SashPos,_("Sash position"),_T("sashpos"),0);
    WXS_FLOAT(wxsSplitterWindow,SashGravity,_("Sash gravity"), _T("sashgravity"), 0.5);
    WXS_LONG(wxsSplitterWindow,MinSize,_("Min. pane size"),_T("minsize"),-1);
    WXS_ENUM(wxsSplitterWindow,Orientation,_("Orientation"),_T("orientation"),OrientValues,OrientNames,wxHORIZONTAL);
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
