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
#include "wxsitemfactory.h"
#include "wxwidgetsgui.h"
#include "../wxsresourcetree.h"
#include "../wxsmith.h"

#include <wx/choicdlg.h>
#include <tinyxml/tinywxuni.h>
#include <sqplus.h>
#include <sc_base_types.h>

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

    /** \brief Function used inside wxWidgets wizard to
      *        bind wxSmith's extensions
      * \param Project - newly created project
      * \param AppSource - name of source file with application source code, relative to cbp file's path
      * \param MainResSource - name of source file with main resource (frame/dialog), relative to cbp file's path
      * \param MainResHeader - name of header file with main resource (frame/dialog), relative to cbp file's path
      * \param WxsFile - name of wxs file with main resource (frame/dialog), relative to cbp file's path
      */
    void AddWxExtensions(cbProject* Project,const wxString& AppSource,const wxString& MainResSource,const wxString& MainResHeader,const wxString& WxsFile)
    {
        wxsProject* WxsProject = wxSmith::Get()->GetSmithProject(Project);

        wxString ResourceName;
        wxString ResourceType;

        // First thing we fetch resource name and type from wxs file
        TiXmlDocument Doc;
        if ( TinyXML::LoadDocument(WxsProject->GetProjectPath()+WxsFile,&Doc) )
        {
            TiXmlElement* Root = Doc.RootElement();
            if ( Root )
            {
                if ( cbC2U(Root->Value()) == _T("wxsmith") )
                {
                    TiXmlElement* Object = Root->FirstChildElement("object");
                    if ( Object )
                    {
                        ResourceType = cbC2U(Object->Attribute("class"));
                        ResourceName = cbC2U(Object->Attribute("name"));
                    }
                }
            }
        }

        if ( ResourceType.IsEmpty() || ResourceName.IsEmpty() )
        {
            // Can not continue, show some error
            cbMessageBox(_("Coudn't parse newly created Wxs file\nwxSmith support is disabled"));
            return;
        }

        // Checking if this resource is really supported inside wxWidgets gui
        if ( Names.Index(ResourceType) == wxNOT_FOUND )
        {
            cbMessageBox(_("Resource type in newly created Wxs file is not supported\nwxSmith support is disabled"));
            return;
        }

        // Creating new resource matching parameters from WXS file
        wxsResource* MainResourceGeneric = wxsResourceFactory::Build(ResourceType,WxsProject);
        wxsItemRes* MainResource = wxDynamicCast(MainResourceGeneric,wxsItemRes);
        if ( !MainResource )
        {
            delete MainResourceGeneric;
            cbMessageBox(_("Resource type in newly created Wxs file is not supported\nwxSmith support is disabled"));
            return;
        }

        // Creating new resource using existing files
        MainResource->CreateNewResource(
            ResourceName,
            MainResSource,false,
            MainResHeader,false,
            wxEmptyString,false,
            WxsFile);

        // Registering new resource inside project
        WxsProject->AddResource(MainResource);

        // Creating new GUI class
        wxWidgetsGUI* GUI = new wxWidgetsGUI(WxsProject);
        GUI->SetAppSourceFile(AppSource);
        GUI->SetInitParams(true,true);
        GUI->SetMainResourceName(ResourceName);
        GUI->RebuildApplicationCode();

        // Registering GUI class inside project and saving all project's changes
        WxsProject->SetGUI(GUI);
        Project->Save();

        // Final step - opening main resource to recreate source
        //              and show results of wizard :)
        MainResource->EditOpen();
    }
}


wxWidgetsResFactory::wxWidgetsResFactory()
{
}

void wxWidgetsResFactory::OnAttach()
{
    // TODO: Call OnAttach for item factories

    // Registering wizard function in scripting manager
    Manager::Get()->GetScriptingManager();
    if (SquirrelVM::GetVMPtr())
    {
        SqPlus::RegisterGlobal(AddWxExtensions,"WxsAddWxExtensions");
    }
}

void wxWidgetsResFactory::OnRelease()
{
    // TODO: Call OnRelease for item factories

    // Unregistering wizard function
    Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    if ( v )
    {
        sq_pushroottable(v);
        sq_pushstring(v,"WxsAddWxExtensions",-1);
        sq_deleteslot(v,-2,false);
        sq_poptop(v);
    }
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
    return 0;
}

bool wxWidgetsResFactory::OnCanHandleExternal(const wxString& FileName)
{
    return wxFileName(FileName).GetExt().Upper() == _T("XRC");
}

wxsResource* wxWidgetsResFactory::OnBuildExternal(const wxString& FileName)
{
    TiXmlDocument Doc;
    if ( !TinyXML::LoadDocument(FileName,&Doc) ) return 0;

    wxArrayString ResourcesFound;
    wxArrayElement XmlElements;
    TiXmlElement* Res = Doc.FirstChildElement("resource");
    if ( !Res )
    {
        // TODO: Some message box about invalid XRC resource structure
        return 0;
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
        return 0;
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
            return 0;
        }
    }

    TiXmlElement* Object = XmlElements[Choice];
    if ( !Object ) return 0;

    wxString Class = cbC2U(Object->Attribute("class"));
    switch ( Names.Index(Class) )
    {
        case wxDialogId: return new wxsDialogRes(FileName,Object);
        case wxFrameId:  return new wxsFrameRes(FileName,Object);
        case wxPanelId:  return new wxsPanelRes(FileName,Object);
    }
    return 0;
}

bool wxWidgetsResFactory::OnNewWizard(int Number,wxsProject* Project)
{
    wxsNewWindowDlg Dlg(0,NamesPtr[Number],Project);
    return Dlg.ShowModal() == wxID_OK;
}

int wxWidgetsResFactory::OnResourceTreeIcon(int Number)
{
    const wxsItemInfo* Info = wxsItemFactory::GetInfo(Names[Number]);
    if ( Info ) return Info->TreeIconId;
    // If there's no valid info, we return id from wxsResourceFactory
    // to get default value
    return wxsResourceFactory::OnResourceTreeIcon(0);
}
