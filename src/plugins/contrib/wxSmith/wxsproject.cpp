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

#include "wxsproject.h"
#include "wxsmith.h"
#include "wxsresource.h"
#include "wxsresourcefactory.h"
#include "wxsguifactory.h"
#include "wxsgui.h"
#include "wxsversionconverter.h"

#include <wx/string.h>
#include <logmanager.h>

namespace
{
    const int CurrentVersion = 1;
    const char* CurrentVersionStr = "1";
}

wxsProject::wxsProject(cbProject* Project):
    m_Project(Project),
    m_GUI(0),
    m_UnknownConfig("unknown_config"),
    m_UnknownResources("unknown_resource"),
    m_WasModifiedDuringLoad(false)
{
    assert(Project);

    // Creating resource tree entery for this project
    m_TreeItem = wxsTree()->NewProjectItem(GetCBProject()->GetTitle(),this);

    // Building paths
    wxFileName PathBuilder( (Project->GetFilename()) );
    m_ProjectPath = PathBuilder.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
}

wxsProject::~wxsProject()
{
    delete m_GUI;
    m_GUI = 0;

    for ( size_t i=m_Resources.Count(); i-->0; )
    {
        delete m_Resources[i];
        m_Resources[i] = 0;
    }
    m_Resources.Clear();

    wxsTree()->Delete(m_TreeItem);
    wxsTree()->Refresh();
}

void wxsProject::ReadConfiguration(TiXmlElement* element)
{
    TiXmlElement* SmithNode = element->FirstChildElement("wxsmith");
    if ( !SmithNode ) return;

    TiXmlDocument TempDoc;

    // Checking version
    if ( wxsVersionConverter::Get().DetectOldConfig(SmithNode,this) )
    {
        TiXmlElement* ConvertedSmithNode = wxsVersionConverter::Get().ConvertFromOldConfig(SmithNode,&TempDoc,this);
        if ( !ConvertedSmithNode )
        {
            for ( TiXmlNode* Node = SmithNode->FirstChild(); Node; Node=Node->NextSibling() )
            {
                m_UnknownConfig.InsertEndChild(*Node);
            }
            return;
        }
        else
        {
            SmithNode = ConvertedSmithNode;
            m_WasModifiedDuringLoad = true;
        }
    }

    const char* VersionStr = SmithNode->Attribute("version");
    int Version = VersionStr ? atoi(VersionStr) : 1;

    if ( Version > CurrentVersion )
    {
        // TODO: Show some dialog box that resources were created by newer version,
        //       store all configuration for later save and return
        return;
    }

    if ( Version < CurrentVersion )
    {
        SmithNode = wxsVersionConverter::Get().Convert(SmithNode,&TempDoc,this);
        if ( !SmithNode )
        {
            // TODO: Show some dialog box that resources were created by newer version,
            //       store all configuration for later save and return
            return;
        }
        else
        {
            m_WasModifiedDuringLoad = true;
        }
    }

    // Iterating through elements
    for ( TiXmlElement* Node = SmithNode->FirstChildElement(); Node; Node=Node->NextSiblingElement() )
    {
        wxString NodeValue = cbC2U(Node->Value());
        if ( NodeValue == _T("gui") )
        {
            wxString GUIName = cbC2U(Node->Attribute("name"));
            wxsGUI* NewGUI = wxsGUIFactory::Build(GUIName,this);
            if ( !NewGUI )
            {
                m_UnknownConfig.InsertEndChild(*Node);
            }
            else
            {
                delete m_GUI;
                m_GUI = NewGUI;
                if ( NewGUI )
                {
                    NewGUI->ReadConfig(Node);
                }
            }
        }
        else if ( NodeValue == _T("resources") )
        {
            for ( TiXmlElement* ResNode = Node->FirstChildElement(); ResNode; ResNode = ResNode->NextSiblingElement() )
            {
                wxString Type = cbC2U(ResNode->Value());
                wxsResource* Res = wxsResourceFactory::Build(Type,this);

                if ( Res )
                {
                    // Storing unknown Xml Element
                    if ( !Res->ReadConfig(ResNode) )
                    {
                        m_UnknownResources.InsertEndChild(*ResNode);
                        delete Res;
                    }
                    else
                    {
                        m_Resources.Add(Res);
                        Res->BuildTreeEntry(GetResourceTypeTreeId(Type));
                    }
                }
                else
                {
                    m_UnknownResources.InsertEndChild(*ResNode);
                }
            }
        }
        else
        {
            m_UnknownConfig.InsertEndChild(*Node);
        }
    }
}

void wxsProject::WriteConfiguration(TiXmlElement* element)
{
    TiXmlElement* SmithElement = element->FirstChildElement("wxsmith");

    if ( !m_GUI && m_Resources.empty() && m_UnknownConfig.NoChildren() && m_UnknownResources.NoChildren() )
    {
        // Nothing to write
        if ( SmithElement )
        {
            element->RemoveChild(SmithElement);
        }
        return;
    }

    if ( !SmithElement )
    {
        SmithElement = element->InsertEndChild(TiXmlElement("wxsmith"))->ToElement();
    }
    SmithElement->Clear();
    SmithElement->SetAttribute("version",CurrentVersionStr);

    // saving GUI item
    if ( m_GUI )
    {
        TiXmlElement* GUIElement = SmithElement->InsertEndChild(TiXmlElement("gui"))->ToElement();
        GUIElement->SetAttribute("name",cbU2C(m_GUI->GetName()));
        m_GUI->WriteConfig(GUIElement);
    }

    // saving resources
    if ( !m_Resources.empty() || !m_UnknownResources.NoChildren() )
    {
        TiXmlElement* ResElement = SmithElement->InsertEndChild(TiXmlElement("resources"))->ToElement();
        size_t Count = m_Resources.Count();
        for ( size_t i=0; i<Count; i++ )
        {
            const wxString& Name = m_Resources[i]->GetResourceName();
            const wxString& Type = m_Resources[i]->GetResourceType();
            TiXmlElement* Element = ResElement->InsertEndChild(TiXmlElement(cbU2C(Type)))->ToElement();
            // TODO: Check value returned from WriteConfig
            m_Resources[i]->WriteConfig(Element);
            Element->SetAttribute("name",cbU2C(Name));
        }

        // Saving all unknown resources
        for ( TiXmlNode* Node = m_UnknownResources.FirstChild(); Node; Node=Node->NextSibling() )
        {
            SmithElement->InsertEndChild(*Node);
        }
    }

    // Saving all unknown configuration nodes
    for ( TiXmlNode* Node = m_UnknownConfig.FirstChild(); Node; Node=Node->NextSibling() )
    {
        SmithElement->InsertEndChild(*Node);
    }

}

bool wxsProject::AddResource(wxsResource* NewResource)
{
    if ( NewResource == 0 )
    {
        return false;
    }

    const wxString& Type = NewResource->GetResourceType();
    const wxString& Name = NewResource->GetResourceName();

    if ( FindResource(Name) != 0 )
    {
        return false;
    }

    m_Resources.Add(NewResource);
    wxsResourceItemId Id = GetResourceTypeTreeId(Type);
    NewResource->BuildTreeEntry(Id);
    m_Project->SetModified(true);
    return true;
}

bool wxsProject::DelResource(wxsResource* Resource)
{
    int Index = m_Resources.Index(Resource);
    if ( Index == wxNOT_FOUND ) return false;

    delete Resource;
    m_Resources.RemoveAt(Index);
    m_Project->SetModified(true);
    return true;
}

wxsResource* wxsProject::FindResource(const wxString& Name)
{
    for ( size_t i = m_Resources.Count(); i-->0; )
    {
        if ( m_Resources[i]->GetResourceName() == Name )
        {
            return m_Resources[i];
        }
    }

    return 0;
}

void wxsProject::Configure()
{
    if ( !m_GUI )
    {
        m_GUI = wxsGUIFactory::SelectNew(_("wxSmith does not manage any GUI for this project.\nPlease select GUI you want to be managed in wxSmith."),this);
        if ( m_GUI )
        {
            NotifyChange();
        }
    }

    if ( m_GUI )
    {
        if ( !m_GUI->CheckIfApplicationManaged() )
        {
            // TODO: Prepare better communicate, consider chancing to cbAnnoyingDiaog
            if ( wxMessageBox(_("wxSmith does not manage this application's source.\n"
                                "Should I create proper bindings?"),_("wxSmith"),wxYES_NO) == wxNO ) return;
            if ( !m_GUI->CreateApplicationBinding() ) return;
        }
        cbConfigurationDialog Dlg(0,-1,_("Configuring wxSmith"));
        Dlg.AttachConfigurationPanel(m_GUI->BuildConfigurationPanel(&Dlg));
        Dlg.ShowModal();
    }
}

cbConfigurationPanel* wxsProject::GetProjectConfigurationPanel(wxWindow* parent)
{
    if ( m_GUI )
    {
        if ( m_GUI->CheckIfApplicationManaged() )
        {
            return m_GUI->BuildConfigurationPanel(parent);
        }
    }
    return 0;
}

wxString wxsProject::GetProjectPath()
{
    return m_ProjectPath;
}

bool wxsProject::CanOpenEditor(const wxString& FileName)
{
    for ( size_t i=m_Resources.Count(); i-->0; )
    {
        if ( m_Resources[i]->OnCanHandleFile(FileName) )
        {
            return true;
        }
    }
    return false;
}

bool wxsProject::TryOpenEditor(const wxString& FileName)
{
    for ( size_t i=m_Resources.Count(); i-->0; )
    {
        if ( m_Resources[i]->OnCanHandleFile(FileName) )
        {
            m_Resources[i]->EditOpen();
            return true;
        }
    }
    return false;
}

void wxsProject::SetGUI(wxsGUI* NewGUI)
{
    if ( m_GUI != NewGUI )
    {
        delete m_GUI;
        m_GUI = NewGUI;
    }
}

void wxsProject::NotifyChange()
{
    return m_Project->SetModified(true);
}

wxsResourceItemId wxsProject::GetResourceTypeTreeId(const wxString& Name)
{
    if ( m_ResBrowserIds.find(Name) != m_ResBrowserIds.end() )
    {
        return m_ResBrowserIds[Name];
    }
    return m_ResBrowserIds[Name] = wxsTree()->AppendItem(m_TreeItem,Name,wxsResourceFactory::ResourceTreeIcon(Name));
}

void wxsProject::UpdateName()
{
    wxsResourceTree::Get()->SetItemText(m_TreeItem,GetCBProject()->GetTitle());
}

bool wxsProject::RecoverWxsFile( const wxString& ResourceDescription )
{
    TiXmlDocument doc;
    doc.Parse( ( _T("<") + ResourceDescription + _T(" />") ).mb_str( wxConvUTF8 ) );
    if ( doc.Error() )
    {
        wxMessageBox( cbC2U( doc.ErrorDesc() ) + wxString::Format(_T(" in %d x %d"), doc.ErrorRow(), doc.ErrorCol() ) );
        return false;
    }
    TiXmlElement* elem = doc.RootElement();
    if ( !elem )
    {
        return false;
    }

    // Try to build resource of given type
    wxString Type = cbC2U(elem->Value());
    wxsResource* Res = wxsResourceFactory::Build(Type,this);
    if ( !Res ) return false;

    // Read settings
    if ( !Res->ReadConfig( elem ) )
    {
        delete Res;
        return false;
    }

    // Prevent duplicating resource names
    if ( FindResource( Res->GetResourceName() ) )
    {
        delete Res;
        return false;
    }

    // Finally add the resource
    m_Resources.Add(Res);
    Res->BuildTreeEntry(GetResourceTypeTreeId(Type));

    return true;
}
