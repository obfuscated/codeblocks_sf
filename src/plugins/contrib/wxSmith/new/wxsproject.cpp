#include "wxsproject.h"
#include "wxsmith.h"
#include "wxsresource.h"
#include "wxsresourcefactory.h"
#include "wxsguifactory.h"

#include <wx/string.h>
#include <messagemanager.h>

namespace
{
    const wxString InternalDir(_T("wxsmith"));
}

wxsProject::wxsProject(cbProject* Project):
    m_Project(Project),
    m_GUI(NULL),
    m_UnknownConfig("unknown_config"),
    m_UnknownResources("unknown_resource")
{
    // Creating resource tree entery for this project
    m_TreeItem = wxsTree()->NewProjectItem(GetCBProject()->GetTitle(),this);

    // Building paths
    wxFileName PathBuilder(Project->GetFilename());
    m_ProjectPath = PathBuilder.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
    PathBuilder.AppendDir(InternalDir);
    m_WorkingPath = PathBuilder.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
}

wxsProject::~wxsProject()
{
    delete m_GUI;
    m_GUI = NULL;

    for ( size_t i=m_Resources.Count(); i-->0; )
    {
        delete m_Resources[i];
        m_Resources[i] = NULL;
    }
    m_Resources.Clear();

    wxsTree()->Delete(m_TreeItem);
    wxsTree()->Refresh();
}

void wxsProject::ReadConfiguration(TiXmlElement* element)
{
    TiXmlElement* SmithNode = element->FirstChildElement("wxsmith");
    if ( !SmithNode ) return;
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
                    NewGUI->OnReadConfig(Node);
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
                    if ( !Res->OnReadConfig(ResNode) )
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
    if ( !m_GUI && m_Resources.empty() )
    {
        // Ths project does not use wxSmith at all - we do not store anything
        return;
    }

    TiXmlElement* SmithElement = element->InsertEndChild(TiXmlElement("wxsmith"))->ToElement();

    // saving GUI item
    if ( m_GUI )
    {
        TiXmlElement* GUIElement = SmithElement->InsertEndChild(TiXmlElement("gui"))->ToElement();
        GUIElement->SetAttribute("name",cbU2C(m_GUI->GetName()));
        m_GUI->OnWriteConfig(GUIElement);
    }

    // saving resources
    if ( !m_Resources.empty() )
    {
        TiXmlElement* ResElement = SmithElement->InsertEndChild(TiXmlElement("resources"))->ToElement();
        size_t Count = m_Resources.Count();
        for ( size_t i=0; i<Count; i++ )
        {
            const wxString& Name = m_Resources[i]->GetResourceName();
            const wxString& Type = m_Resources[i]->GetResourceType();
            TiXmlElement* Element = ResElement->InsertEndChild(TiXmlElement(cbU2C(Type)))->ToElement();
            m_Resources[i]->OnWriteConfig(Element);
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
    if ( NewResource == NULL )
    {
        return false;
    }

    const wxString& Type = NewResource->GetResourceType();
    const wxString& Name = NewResource->GetResourceName();

    if ( FindResource(Name) != NULL )
    {
        return false;
    }

    m_Resources.Add(NewResource);
    wxsResourceItemId Id = GetResourceTypeTreeId(Type);
    NewResource->BuildTreeEntry(Id);
    m_Project->SetModified(true);
    return true;
}

wxsResource* wxsProject::FindResource(const wxString& Name)
{
    for ( size_t i = m_Resources.Count(); i-->0; )
    {
        if ( m_Resources[i]->GetResourceName() == Name ) return m_Resources[i];
    }

    return NULL;
}

void wxsProject::Configure()
{
    if ( !m_GUI )
    {
        m_GUI = wxsGUIFactory::SelectNew(_("wxSmith does not manage any GUI for this project.\nPlease select GUI you want to be managed in wxSmith."),this);
    }

    if ( m_GUI )
    {
        cbConfigurationDialog Dlg(NULL,-1,_("Configuring wxSmith"));
        Dlg.AttachConfigurationPanel(m_GUI->OnBuildConfigurationPanel(&Dlg));
        Dlg.ShowModal();
    }
}

wxString wxsProject::GetProjectPath()
{
    return m_ProjectPath;
}

wxString wxsProject::GetInternalPath()
{
    return m_WorkingPath;
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

wxsResourceItemId wxsProject::GetResourceTypeTreeId(const wxString& Name)
{
    if ( m_ResBrowserIds.find(Name) != m_ResBrowserIds.end() )
    {
        return m_ResBrowserIds[Name];
    }
    return m_ResBrowserIds[Name] = wxsTree()->AppendItem(m_TreeItem,Name);
}

/*
bool wxsProject::AddSmithConfig()
{
    if ( GetIntegration() != NotWxsProject ) return false;

    if ( ! wxFileName::Mkdir(WorkingPath.GetPath(wxPATH_GET_VOLUME),0744,wxPATH_MKDIR_FULL) )
    {
        ::wxMessageBox(_("Couldn't create wxsmith directory in main projet's path"),_("Error"),wxOK|wxICON_ERROR);
        return false;
    }

    Integration = Integrated;
    SaveProject();
    BuildTree(wxsTREE(),TreeItem);
    return true;
}
*/


//bool wxsProject::IsAppManaged()
//{
//    return IsAppSourceManaged(m_Config.AppFile);
//}
//
//bool wxsProject::IsAppSourceManaged(const wxString& FileName)
//{
//    // TODO: Add support for other coding languages
//    if ( FileName.empty() ) return false;
//
//    if ( wxsGETCODE(
//            GetProjectFileName(FileName),
//            wxsBBegin() _T("AppInitialize"),
//            wxsBEnd()
//            ).empty() )
//    {
//        return false;
//    }
//
//    if ( wxsGETCODE(
//            GetProjectFileName(FileName),
//            wxsBBegin() _T("AppHeaders"),
//            wxsBEnd()
//            ).empty() )
//    {
//        return false;
//    }
//    return true;
//}


//void wxsProject::EnumerateResources(wxArrayString& Array,bool MainOnly)
//{
//    size_t Count = m_Resources.Count();
//    for ( size_t i=0; i<Count; i++ )
//    {
//        if ( !MainOnly || wxsResourceFactory::CanBeMain(m_Resources[i]->GetResourceType()) )
//        {
//            Array.Add(m_Resources[i]->GetResourceName());
//        }
//    }
//}
//
//void wxsProject::EnumerateResources(wxArrayString& Array,const wxString& ResourceType)
//{
//    size_t Count = m_Resources.Count();
//    for ( size_t i=0; i<Count; i++ )
//    {
//        if ( m_Resources[i]->GetResourceType() == ResourceType )
//        {
//            Array.Add(m_Resources[i]->GetResourceName());
//        }
//    }
//}
//
//wxString wxsProject::GetProjectFileName(const wxString& FileName)
//{
//    return m_ProjectPath + FileName;
//}
//
//wxString wxsProject::GetInternalFileName(const wxString& FileName)
//{
//    return m_WorkingPath + FileName;
//}
//
//void wxsProject::RebuildAppCode()
//{
//    if ( m_Config.AppFile.empty() ) return;
//
//    // TODO: Move this to resources/wxwidgets
//    bool InitAllHandlers = m_Config.CallInitAll;
//    bool CheckInitAll = m_Config.CallInitAllNecessary && InitAllHandlers;
//    bool CheckMainRes = !m_Config.MainResource.empty();
//    bool IsAnyXRC = false;
//
//    wxsResource* MainResPtr = NULL;
//
//    // Finding out if we are using Xrc
//    if ( CheckInitAll || CheckMainRes )
//    {
//        size_t Count = m_Resources.Count();
//        for ( size_t i=0; i<Count; i++ )
//        {
//            // TODO: Support for other languages
//            if ( CheckMainRes &&
//                 (m_Config.MainResource==m_Resources[i]->GetResourceName()) &&
//                 m_Resources[i]->GetLanguage()==wxsCPP )
//            {
//                MainResPtr = m_Resources[i];
//            }
//
//            if ( CheckInitAll && (m_Resources[i]->UsingXRC()) )
//            {
//                IsAnyXRC = true;
//            }
//        }
//
//        if ( CheckInitAll && !IsAnyXRC )
//        {
//            InitAllHandlers = false;
//        }
//    }
//
//    wxString CodeHeader = wxsBBegin() _T("AppInitialize");
//    wxString NewCode;
//
//    NewCode.Append(_T("\nbool wxsOK = true;\n"));
//
//    if ( InitAllHandlers )
//    {
//        NewCode.Append(_T("wxXmlResource::Get()->InitAllHandlers();\n"));
//    }
//
//    for ( size_t i = 0; i<m_Config.LoadedResources.Count(); ++i )
//    {
//        NewCode.Append(_T("wxsOK = wxsOK && wxXmlResource::Get()->Load(_T(\""));
//        NewCode.Append(m_Config.LoadedResources[i]);
//        NewCode.Append(_T("\"));\n"));
//    }
//
//    if ( MainResPtr )
//    {
//        NewCode << _T("if ( wxsOK )\n{\n");
//        NewCode << MainResPtr->OnGetAppBuildingCode();
//        NewCode << _T("}\n");
//    }
//
//    wxsADDCODE(
//        GetProjectFileName(m_Config.AppFile),
//        CodeHeader,
//        wxsBEnd(),
//        NewCode);
//
//    CodeHeader = wxsBBegin() _T("AppHeaders");
//    NewCode = _T("\n");
//
//    if ( MainResPtr )
//    {
//        wxString IncludeFile = MainResPtr->OnGetDeclarationFile();
//        wxFileName IncludeFileName(GetProjectFileName(IncludeFile));
//        if ( IncludeFileName.MakeRelativeTo(GetProjectFileName(GetConfig().AppFile)) )
//        {
//            // We will use unix path format since it's relative path
//            // Using this format will make sources more cross-platform
//            IncludeFile = IncludeFileName.GetFullPath(wxPATH_UNIX);
//        }
//
//        NewCode << _T("#include \"") << IncludeFile << _T("\"\n");
//    }
//    if ( IsAnyXRC || m_Config.LoadedResources.Count() )
//    {
//        NewCode.Append(_T("#include <wx/xrc/xmlres.h>\n"));
//    }
//
//    wxsADDCODE(
//        GetProjectFileName(m_Config.AppFile),
//        CodeHeader,
//        wxsBEnd(),
//        NewCode);
//}
//
//void wxsProject::Configure()
//{
//    // TODO: Move this to separate function and into resources/wxwidgets rather than here
//    while ( !IsAppManaged() )
//    {
//        int Ret = ::wxMessageBox(
//            _("WARNING:\n"
//              "\n"
//              "In this project, wxSmith does not manage source code for\n"
//              "wxApp-derived class, there are no project options.\n"
//              "\n"
//              "If you want wxSmith to manage Your application class,\n"
//              "it must be declared in following form:\n"
//              "\n"
//              "\t//(*AppHeaders\n"
//              "\t  /*... wxSmith will put necesarry header includes here ...*/\n"
//              "\t//*)\n"
//              "\n"
//              "\tclass MyApp : public wxApp\n"
//              "\t{\n"
//              "\t\tpublic:\n"
//              "\t\t\tvirtual bool OnInit();\n"
//              "\t};\n"
//              "\tIMPLEMENT_APP(MyApp);\n"
//              "\n"
//              "\tbool MyApp::OnInit()\n"
//              "\t{\n"
//              "\t\t//(*AppInitialize\n"
//              "\t\t  /*... wxSmith will put application's initialization code here ...*/\n"
//              "\t\t//*)\n"
//              "\t\t  /*... wxsOK will be true if initialized correctly ... */\n"
//              "\t\treturn wxsOK;\n"
//              "\t}\n"
//              "\n"
//              "If you have already changed Your application to this form,\n"
//              "click Yes and choose source file with application class.\n"
//              "\n"
//              "If you don't want wxSmith to manage Your application\n"
//              "(f.ex. you have your own initialization system), click No.\n"),
//            _("Application not manager in wxSmith"),
//            wxYES_NO | wxICON_INFORMATION );
//
//        if ( Ret != wxYES )
//        {
//            return;
//        }
//
//        wxString NewFileName;
//        for(;;)
//        {
//            NewFileName = ::wxFileSelector(
//                _("Choose source file with your application"),
//                _T(""), _T("main.cpp"), _T("cpp"),
//                _("C++ source files (*.cpp)|*.cpp|"
//                  "All files (*)|*"),
//                wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY);
//            if ( NewFileName.empty() )
//            {
//                return;
//            }
//            wxFileName FN(NewFileName);
//            FN.MakeRelativeTo(GetProjectPath());
//            NewFileName = FN.GetFullPath();
//            if ( CheckProjFileExists(NewFileName) ) break;
//            Ret = ::wxMessageBox(
//                _("Selected file is not included in this project.\n"
//                  "Continue ?"),
//                _("Selected external file"),
//                wxYES_NO | wxICON_QUESTION );
//            if ( Ret == wxYES ) break;
//        }
//
//        m_Config.AppFile = NewFileName;
//    }
//
//    wxsProjectConfigurationDlg Dlg(0L,this);
//    Dlg.ShowModal();
//}
