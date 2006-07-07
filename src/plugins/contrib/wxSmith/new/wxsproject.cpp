#include "wxsproject.h"
#include "wxsmith.h"
#include "wxsresource.h"
#include "wxsresourcefactory.h"
#include "wxscoder.h"
#include "wxsprojectconfigurationdlg.h"

#include <wx/string.h>
#include <messagemanager.h>

#define XML_CONFIG_STR   "configuration"
#define XML_APPFILE_STR  "app_src_file"
#define XML_MAINRES_STR  "main_resource"
#define XML_INITALL_STR  "init_all_handlers"
#define XML_AUTOLOAD_STR "load_resource"

wxsProject::wxsProject(cbProject* _Project):
    Integration(NotBinded),
    Project(_Project)
{
    Config.CallInitAll = true;
    Config.CallInitAllNecessary = true;

    // Creating resource tree enteries
    TreeItem = wxsTREE()->NewProjectItem(this);

    // Can not bind project which haven't been loaded yet
    if ( Project && !Project->IsLoaded() )
    {
        Project = NULL;
    }

    if ( !Project )
    {
        return;
    }

    // Checkign association of C::B project with wxs project
    ProjectPath.Assign(Project->GetFilename());
    WorkingPath = ProjectPath;
    WorkingPath.AppendDir(wxSmithSubDirectory);
    WorkingPath.SetName(wxSmithMainConfigFile);
    WorkingPath.SetExt(_T(""));
    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path

    if ( ! WorkingPath.FileExists() )
    {
        // Did not found wxs configuration file
        Integration = NotWxsProject;
        return;
    }

    // Trying to read configuration data
    TiXmlDocument Doc(cbU2C(WorkingPath.GetFullPath()));
    if ( !Doc.LoadFile() )
    {
        // Invalid configuration file
        LOG(_("Invalid wxSmith configuration file in project: %s"),Project->GetTitle().c_str());
        Integration = NotWxsProject;
        return;
    }

    // Reading configuration
    TiXmlNode* MainNode = Doc.FirstChild("wxsmith");
    if ( MainNode == NULL || ! LoadFromXml(MainNode) )
    {
        LOG(_("Invalid wxSmith configuration file in project: %s"),Project->GetTitle().c_str());
        Integration = NotWxsProject;
        return;
    }

    // Filling up resource tree
    BuildTree(wxsTREE(),TreeItem);
    Integration = Integrated;
}

wxsProject::~wxsProject()
{
    // Saving pending changes
    if ( Modified ) SaveProject();

    // Removing all resources
    size_t Count = Resources.Count();
    for ( size_t i=0; i<Count; i++ )
    {
        Resources[i]->EditClose();
        delete Resources[i];
    }
    Resources.Clear();

    // Removing item in resource browser
    if ( Project )
    {
        wxsTREE()->Delete(TreeItem);
    }
    wxsTREE()->Refresh();
}

void wxsProject::BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd)
{
    int Cnt = wxsRESFACTORY()->GetResTypesCnt();
    for ( int i=0; i<Cnt; i++ )
    {
        ResBrowserIds[wxsRESFACTORY()->GetResType(i)] =
            Tree->AppendItem(WhereToAdd,wxsRESFACTORY()->GetResBrowserName(i));
    }

    size_t Count = Resources.Count();
    for ( size_t i=0; i<Count; i++ )
    {
        wxString Type = Resources[i]->GetType();

        ResBrowserIdsI it = ResBrowserIds.find(Type);
        if ( it != ResBrowserIds.end() )
        {
            Resources[i]->BuildTree(Tree,it->second);
        }
        else
        {
            Resources[i]->BuildTree(Tree,WhereToAdd);
        }
    }
    for ( ResBrowserIdsI i = ResBrowserIds.begin(); i!=ResBrowserIds.end(); ++i )
    {
        Tree->Expand(i->second);
    }
    Tree->Expand(WhereToAdd);
    Tree->Refresh();
}

bool wxsProject::LoadFromXml(TiXmlNode* MainNode)
{
    TiXmlElement* Elem;

    // Loading resources
    for ( Elem = MainNode->FirstChildElement(); Elem; Elem = Elem->NextSiblingElement() )
    {
        wxString Type = cbC2U(Elem->Value());
        wxsResource* Res = wxsRESFACTORY()->Build(Type,this);
        if ( !Res )
        {
            DBGLOG(_T("wxSmith: Couldn't create resource of type %s"),Type.c_str());
            continue;
        }

        if ( !Res->LoadConfiguration(Elem) )
        {
            delete Res;
        }
        else
        {
            Resources.Add(Res);
        }
    }

    // Loading configuration stuff
    Elem = MainNode->FirstChildElement(XML_CONFIG_STR);
    if ( Elem )
    {
        Config.AppFile = cbC2U( Elem->Attribute(XML_APPFILE_STR) );
        Config.MainResource = cbC2U( Elem->Attribute(XML_MAINRES_STR) );
        wxString InitAllMode = cbC2U( Elem->Attribute(XML_INITALL_STR));
        if ( InitAllMode == _T("never") )
        {
            Config.CallInitAll = false;
            Config.CallInitAllNecessary = false;
        }
        else if ( InitAllMode == _T("always") )
        {
            Config.CallInitAll = true;
            Config.CallInitAllNecessary = false;
        }
        else
        {
            Config.CallInitAll = true;
            Config.CallInitAllNecessary = true;
        }
    }

    // Loading list of automatically loaded resource files
    for ( Elem = MainNode->FirstChildElement(XML_AUTOLOAD_STR);
          Elem;
          Elem = Elem->NextSiblingElement(XML_AUTOLOAD_STR) )
    {
        TiXmlText* Text = Elem->FirstChild()->ToText();
        if ( Text && Text->Value() )
        {
            wxString FileName = cbC2U(Text->Value());
            if ( FileName.Length() )
            {
                Config.LoadedResources.Add(FileName);
            }
        }
    }

    Modified = false;
    return true;
}

bool wxsProject::AddResource(wxsResource* Resource)
{
    if ( Resource == NULL )
    {
        return false;
    }

    wxString Type = Resource->GetType();
    ResBrowserIdsI it = ResBrowserIds.find(Type);
    if ( it != ResBrowserIds.end() )
    {
        Resource->BuildTree(wxsTREE(),it->second);
    }
    else
    {
        Resource->BuildTree(wxsTREE(),TreeItem);
    }
    Resources.Add(Resource);
    Modified = true;

    return true;
}

bool wxsProject::CheckProjFileExists(const wxString& FileName)
{
    if ( !Project ) return false;
    return Project->GetFileByFilename(FileName) != NULL;
}

TiXmlDocument* wxsProject::GenerateXml()
{
    if ( !Project ) return NULL;
    if ( Integration != Integrated ) return NULL;

    TiXmlDocument* Doc = new TiXmlDocument();
    // TODO: Add standard XML header
    TiXmlNode* Elem = Doc->InsertEndChild(TiXmlElement("wxsmith"));

    size_t Count = Resources.Count();
    for ( size_t i=0; i<Count; i++ )
    {
        TiXmlElement* Child = Elem->InsertEndChild(
            TiXmlElement(cbU2C(Resources[i]->GetType())))->ToElement();
        Resources[i]->SaveConfiguration(Child);
    }

    TiXmlElement* ConfigElem = Elem->InsertEndChild(TiXmlElement(XML_CONFIG_STR))->ToElement();
    if ( ConfigElem )
    {
        ConfigElem->SetAttribute(XML_APPFILE_STR,cbU2C(Config.AppFile));
        ConfigElem->SetAttribute(XML_MAINRES_STR,cbU2C(Config.MainResource));

        if ( Config.CallInitAll && Config.CallInitAllNecessary )
        {
            ConfigElem->SetAttribute(XML_INITALL_STR,"necessary");
        }
        else if ( Config.CallInitAll )
        {
            ConfigElem->SetAttribute(XML_INITALL_STR,"always");
        }
        else
        {
            ConfigElem->SetAttribute(XML_INITALL_STR,"never");
        }
    }

    for ( size_t i=0; i<Config.LoadedResources.GetCount(); ++i )
    {
        TiXmlNode* Node = Elem->InsertEndChild(TiXmlElement(XML_AUTOLOAD_STR));
        if ( Node ) Node->InsertEndChild(TiXmlText(cbU2C(Config.LoadedResources[i])));
    }

    return Doc;
}

void wxsProject::SaveProject()
{
    if ( Integration != Integrated ) return;

    WorkingPath.SetName(wxSmithMainConfigFile);
    WorkingPath.SetExt(_T(""));
    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path

    TiXmlDocument* Doc = GenerateXml();
    if ( Doc )
    {
        Doc->SaveFile(cbU2C(WorkingPath.GetFullPath()));
        delete Doc;
    }
    Modified = false;
}

wxString wxsProject::GetInternalFileName(const wxString& FileName)
{
	wxFileName Path = WorkingPath;
    Path.SetName(FileName);
    Path.SetExt(_T(""));
    Path.Assign(Path.GetFullPath());  // Reparsing path
    return Path.GetFullPath();
}

wxString wxsProject::GetProjectFileName(const wxString& FileName)
{
	wxFileName Path = ProjectPath;
    Path.SetName(FileName);
    Path.SetExt(_T(""));
    Path.Assign(Path.GetFullPath());
    return Path.GetFullPath();
}

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

wxsResource* wxsProject::FindResource(const wxString& Name)
{
    for ( size_t i = Resources.Count(); i-->0; )
    {
        if ( Resources[i]->GetResourceName() == Name ) return Resources[i];
    }

    return NULL;
}

void wxsProject::Configure()
{
    while ( !IsAppManaged() )
    {
        int Ret = ::wxMessageBox(
            _("WARNING:\n"
              "\n"
              "In this project, wxSmith does not manage source code for\n"
              "wxApp-derived class, there are no project options.\n"
              "\n"
              "If you want wxSmith to manage Your application class,\n"
              "it must be declared in following form:\n"
              "\n"
              "\t//(*AppHeaders\n"
              "\t  /*... wxSmith will put necesarry header includes here ...*/\n"
              "\t//*)\n"
              "\n"
              "\tclass MyApp : public wxApp\n"
              "\t{\n"
              "\t\tpublic:\n"
              "\t\t\tvirtual bool OnInit();\n"
              "\t};\n"
              "\tIMPLEMENT_APP(MyApp);\n"
              "\n"
              "\tbool MyApp::OnInit()\n"
              "\t{\n"
              "\t\t//(*AppInitialize\n"
              "\t\t  /*... wxSmith will put application's initialization code here ...*/\n"
              "\t\t//*)\n"
              "\t\t  /*... wxsOK will be true if initialized correctly ... */\n"
              "\t\treturn wxsOK;\n"
              "\t}\n"
              "\n"
              "If you have already changed Your application to this form,\n"
              "click Yes and choose source file with application class.\n"
              "\n"
              "If you don't want wxSmith to manage Your application\n"
              "(f.ex. you have your own initialization system), click No.\n"),
            _("Application not manager in wxSmith"),
            wxYES_NO | wxICON_INFORMATION );

        if ( Ret != wxYES )
        {
            return;
        }

        wxString NewFileName;
        for(;;)
        {
            NewFileName = ::wxFileSelector(
                _("Choose source file with your application"),
                _T(""), _T("main.cpp"), _T("cpp"),
                _("C++ source files (*.cpp)|*.cpp|"
                  "All files (*)|*"),
                wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY);
            if ( NewFileName.empty() )
            {
                return;
            }
            wxFileName FN(NewFileName);
            FN.MakeRelativeTo(GetProjectPath());
            NewFileName = FN.GetFullPath();
            if ( CheckProjFileExists(NewFileName) ) break;
            Ret = ::wxMessageBox(
                _("Selected file is not included in this project.\n"
                  "Continue ?"),
                _("Selected external file"),
                wxYES_NO | wxICON_QUESTION );
            if ( Ret == wxYES ) break;
        }

        Config.AppFile = NewFileName;
    }

    wxsProjectConfigurationDlg Dlg(0L,this);
    Dlg.ShowModal();
}

void wxsProject::EnumerateResources(wxArrayString& Array,bool MainOnly)
{
    size_t Count = Resources.Count();
    for ( size_t i=0; i<Count; i++ )
    {
        if ( !MainOnly || Resources[i]->CanBeMain() )
        {
            Array.Add(Resources[i]->GetResourceName());
        }
    }
}

void wxsProject::RebuildAppCode()
{
    if ( Config.AppFile.empty() ) return;

    bool InitAllHandlers = Config.CallInitAll;
    bool CheckInitAll = Config.CallInitAllNecessary && InitAllHandlers;
    bool CheckMainRes = !Config.MainResource.empty();
    bool IsAnyXRC = false;

    wxsResource* MainResPtr = NULL;

    // Finding out if we are using Xrc
    if ( CheckInitAll || CheckMainRes )
    {
        size_t Count = Resources.Count();
        for ( size_t i=0; i<Count; i++ )
        {
            // TODO: Support for other languages
            if ( CheckMainRes &&
                 (Config.MainResource==Resources[i]->GetResourceName()) &&
                 Resources[i]->GetLanguage()==wxsCPP )
            {
                MainResPtr = Resources[i];
            }

            if ( CheckInitAll && (Resources[i]->UsingXRC()) )
            {
                IsAnyXRC = true;
            }
        }

        if ( CheckInitAll && !IsAnyXRC )
        {
            InitAllHandlers = false;
        }
    }

    wxString CodeHeader = wxsBBegin() _T("AppInitialize");
    wxString NewCode;

    NewCode.Append(_T("\nbool wxsOK = true;\n"));

    if ( InitAllHandlers )
    {
        NewCode.Append(_T("wxXmlResource::Get()->InitAllHandlers();\n"));
    }

    for ( size_t i = 0; i<Config.LoadedResources.Count(); ++i )
    {
        NewCode.Append(_T("wxsOK = wxsOK && wxXmlResource::Get()->Load(_T(\""));
        NewCode.Append(Config.LoadedResources[i]);
        NewCode.Append(_T("\"));\n"));
    }

    if ( MainResPtr )
    {
        NewCode << _T("if ( wxsOK )\n{\n");
        MainResPtr->BuildShowingCode(NewCode,wxsCPP);
        NewCode << _T("}\n");
    }

    wxsADDCODE(
        GetProjectFileName(Config.AppFile),
        CodeHeader,
        wxsBEnd(),
        NewCode);

    CodeHeader = wxsBBegin() _T("AppHeaders");
    NewCode = _T("\n");

    if ( MainResPtr )
    {
        wxString IncludeFile = MainResPtr->GetDeclarationFile();
        wxFileName IncludeFileName(GetProjectFileName(IncludeFile));
        if ( IncludeFileName.MakeRelativeTo(GetProjectFileName(GetConfig().AppFile)) )
        {
            // We will use unix path format since it's relative path
            // Using this format will make sources more cross-platform
            IncludeFile = IncludeFileName.GetFullPath(wxPATH_UNIX);
        }

        NewCode << _T("#include \"") << IncludeFile << _T("\"\n");
    }
    if ( IsAnyXRC || Config.LoadedResources.Count() )
    {
        NewCode.Append(_T("#include <wx/xrc/xmlres.h>\n"));
    }

    wxsADDCODE(
        GetProjectFileName(Config.AppFile),
        CodeHeader,
        wxsBEnd(),
        NewCode);
}

wxString wxsProject::GetProjectPath()
{
    return ProjectPath.GetPath();
}

wxString wxsProject::GetInternalPath()
{
    return WorkingPath.GetPath();
}

bool wxsProject::IsAppManaged()
{
    return IsAppSourceManaged(Config.AppFile);
}

bool wxsProject::IsAppSourceManaged(const wxString& FileName)
{
    if ( FileName.empty() ) return false;

    if ( wxsGETCODE(
            GetProjectFileName(FileName),
            wxsBBegin() _T("AppInitialize"),
            wxsBEnd()
            ).empty() )
    {
        return false;
    }

    if ( wxsGETCODE(
            GetProjectFileName(FileName),
            wxsBBegin() _T("AppHeaders"),
            wxsBEnd()
            ).empty() )
    {
        return false;
    }
    return true;
}

bool wxsProject::TryOpenEditor(const wxString& FileName)
{
    for ( size_t i=Resources.Count(); i-->0; )
    {
        if ( Resources[i]->UsingFile(FileName) )
        {
            Resources[i]->EditOpen();
            return true;
        }
    }
    return false;
}
