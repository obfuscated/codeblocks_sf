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
#include "wxwidgetsresfactory.h"
#include "wxsdialogres.h"
#include "wxsframeres.h"
#include "wxspanelres.h"
#include "wxsnewwindowdlg.h"

#include <wx/choicdlg.h>

namespace
{
    wxWidgetsResFactory Factory;

    enum Resources
    {
        wxDialogId = 0,
        wxFrameId,
        wxPanelId,
        /*=========*/
        ResourcesCount
    };

    const wxChar* NamesPtr[ResourcesCount] =
    {
        _T("wxDialog"),
        _T("wxFrame"),
        _T("wxPanel")
    };

    wxArrayString Names(ResourcesCount,NamesPtr);

    WX_DEFINE_ARRAY(TiXmlElement*,wxArrayElement);
}


wxWidgetsResFactory::wxWidgetsResFactory()
{
}

int wxWidgetsResFactory::OnGetCount()
{
    return ResourcesCount;
}

void wxWidgetsResFactory::OnGetInfo(int Number,wxString& Name,wxString& GUI)
{
    GUI = _T("wxWidgets");
    Name = NamesPtr[Number];
}

wxsResource* wxWidgetsResFactory::OnCreate(int Number,wxsProject* Project)
{
    switch ( Number )
    {
        case wxDialogId: return new wxsDialogRes(Project);
        case wxFrameId:  return new wxsFrameRes(Project);
        case wxPanelId:  return new wxsPanelRes(Project);
    }
    return NULL;
}

bool wxWidgetsResFactory::OnCanHandleExternal(const wxString& FileName)
{
    return wxFileName(FileName).GetExt().Upper() == _T("XRC");
}

wxsResource* wxWidgetsResFactory::OnBuildExternal(const wxString& FileName)
{
    TiXmlDocument Doc;
    if ( !Doc.LoadFile(cbU2C(FileName)) ) return NULL;

    wxArrayString ResourcesFound;
    wxArrayElement XmlElements;
    TiXmlElement* Res = Doc.FirstChildElement("resource");
    if ( !Res )
    {
        // TODO: Some message box about invalid XRC resource structure
        return NULL;
    }

    for ( TiXmlElement* Object = Res->FirstChildElement("object"); Object; Object=Object->NextSiblingElement("object") )
    {
        wxString Class = cbC2U(Object->Attribute("class"));
        wxString Name = cbC2U(Object->Attribute("name"));
        if ( !Name.empty() && Names.Index(Class) != wxNOT_FOUND )
        {
            ResourcesFound.Add(Name + _T(" (") + Class + _T(")"));
            XmlElements.Add(Object);
        }
    }

    if ( ResourcesFound.empty() )
    {
        // TODO: Message box that there are no resoures which could be edited here
        return NULL;
    }

    int Choice = 0;
    if ( ResourcesFound.size() > 1 )
    {
        Choice = ::wxGetSingleChoiceIndex(
            _("There's more than one resource in this file.\n"
              "Please select which one should be edited."),
            _("Choose resource to edit"),
            ResourcesFound);
        if ( Choice<0 )
        {
            return NULL;
        }
    }

    TiXmlElement* Object = XmlElements[Choice];
    if ( !Object ) return NULL;

    wxString Class = cbC2U(Object->Attribute("class"));
    switch ( Names.Index(Class) )
    {
        case wxDialogId: return new wxsDialogRes(FileName,Object);
        case wxFrameId:  return new wxsFrameRes(FileName,Object);
        case wxPanelId:  return new wxsPanelRes(FileName,Object);
    }
    return NULL;
}

bool wxWidgetsResFactory::OnNewWizard(int Number,wxsProject* Project)
{
    wxsNewWindowDlg Dlg(NULL,NamesPtr[Number],Project);
    return Dlg.ShowModal() == wxID_OK;
}
