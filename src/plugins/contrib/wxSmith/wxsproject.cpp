#include "wxsheaders.h"
#include "wxsproject.h"

#include "widget.h"
#include "defwidgets/wxsstdmanager.h"
#include "defwidgets/wxsdialog.h"
#include "resources/wxswindowres.h"
#include "wxswidgetfactory.h"
#include "wxsmith.h"
#include "wxsprojectconfigurationdlg.h"
#include <wx/string.h>
#include <annoyingdialog.h>

#define XML_DIALOG_STR   "dialog"
#define XML_FRAME_STR    "frame"
#define XML_PANEL_STR    "panel"
#define XML_FNAME_STR    "wxs_file"
#define XML_CNAME_STR    "class"
#define XML_SFILE_STR    "src_file"
#define XML_HFILE_STR    "header_file"
#define XML_XRCFILE_STR  "xrc_file"
#define XML_EDITMODE_STR "edit_mode"
#define XML_CONFIG_STR   "configuration"
#define XML_APPFILE_STR  "app_src_file"
#define XML_MAINRES_STR  "main_resource"
#define XML_INITALL_STR  "init_all_handlers"
#define XML_AUTOLOAD_STR "load_resource"

wxsProject::wxsProject():  Integration(NotBinded), Project(NULL), CallInitAll(true), CallInitAllNecessary(true), DuringClear(false)
{}

wxsProject::~wxsProject()
{
    Clear();
}

wxsProject::IntegrationState wxsProject::BindProject(cbProject* Proj,TiXmlElement* Config)
{
    Clear();

    /* creating new node in resource tree */

    wxTreeCtrl* ResTree = wxsTREE();
    ResTree->Expand(ResTree->GetRootItem());
    TreeItem = ResTree->AppendItem(ResTree->GetRootItem(),Proj->GetTitle(),
        -1,-1,
        new wxsResourceTreeData(this));

    /* Binding project object */
    Project = Proj;
    if ( !Proj )
    {
        return Integration = NotBinded;
    }

    /* Checkign association of C::B project with wxS project */

    ProjectPath.Assign(Proj->GetFilename());
    WorkingPath = ProjectPath;
    WorkingPath.AppendDir(wxSmithSubDirectory);
    WorkingPath.SetName(wxSmithMainConfigFile);
    WorkingPath.SetExt(_T(""));
    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path

    // Trying to find configuration in given node first.
    // If not found, searching for it in previously used external config file

    TiXmlElement* SmithConf = Config ? Config->FirstChildElement("wxsmith") : NULL;

    if ( SmithConf!=NULL )
    {
        if ( !LoadFromXml(SmithConf) )
        {
            return Integration = NotWxsProject;
        }
    }
    else
    {
        if ( ! WorkingPath.FileExists() )
        {
            return Integration = NotWxsProject;
        }

        /* Trying to read configuration data */
        TiXmlDocument Doc(cbU2C(WorkingPath.GetFullPath()));

        if ( !Doc.LoadFile() )
        {
            return Integration = NotWxsProject;
        }

        TiXmlNode* MainNode = Doc.FirstChild("wxsmith");
        if ( MainNode == NULL || ! LoadFromXml(MainNode) )
        {
            return Integration = NotWxsProject;
        }

        AnnoyingDialog dlg(
            _("Old wxSmith configuration detected"),
            _("Previous version of wxSmith used external file to\n"
              "keep it's configuration. In new version, it is stored\n"
              "inside .cbp file. Should I automatically delete\n"
              "previous configuration file ?"),
            wxART_QUESTION);
        if ( dlg.ShowModal() == wxID_YES )
        {
            if ( !::wxRemoveFile(WorkingPath.GetFullPath()) )
            {
                ::wxMessageBox(_("Unable to delete old wxSmith configuration file."),_("Couldn't delete file"));
            }
        }

        SetModified(true);
    }

    BuildTree(ResTree,TreeItem);

    return Integration = Integrated;
}

inline void wxsProject::Clear()
{
    DuringClear = true;

    Integration = NotBinded;
    WorkingPath.Clear();
    ProjectPath.Clear();

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( *i )
        {
            delete *i;
            *i = NULL;
        }
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        if ( *i )
        {
            delete *i;
            *i = NULL;
        }
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        if ( *i )
        {
            delete *i;
            *i = NULL;
        }
    }

    Dialogs.clear();
    Frames.clear();
    Panels.clear();
    if ( Project ) wxsTREE()->Delete(TreeItem);
    Project = NULL;
    wxsTREE()->Refresh();
    DuringClear = false;
}

void wxsProject::BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd)
{
    DialogId = Tree->AppendItem(WhereToAdd,_("Dialog resources"));
    FrameId  = Tree->AppendItem(WhereToAdd,_("Frame resources"));
    PanelId  = Tree->AppendItem(WhereToAdd,_("Panel resources"));

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        (*i)->BuildTree(Tree,DialogId,true);
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        (*i)->BuildTree(Tree,FrameId,true);
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        (*i)->BuildTree(Tree,PanelId,true);
    }

    Tree->Refresh();
}

bool wxsProject::LoadFromXml(TiXmlNode* MainNode)
{
    TiXmlElement* Elem;

    // Loading dialog resources

    for ( Elem = MainNode->FirstChildElement(XML_DIALOG_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_DIALOG_STR) )
    {
    	wxString Mode = cbC2U(Elem->Attribute(XML_EDITMODE_STR));
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : cbC2U( Elem->Attribute(XML_XRCFILE_STR));
        AddDialogResource(
            cbC2U( Elem->Attribute(XML_FNAME_STR) ),
            cbC2U( Elem->Attribute(XML_CNAME_STR) ),
            cbC2U( Elem->Attribute(XML_SFILE_STR) ),
            cbC2U( Elem->Attribute(XML_HFILE_STR) ),
            Xrc );
    }

    // Loading frame resources

    for ( Elem = MainNode->FirstChildElement(XML_FRAME_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_FRAME_STR) )
    {
    	wxString Mode = cbC2U(Elem->Attribute(XML_EDITMODE_STR));
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : cbC2U( Elem->Attribute(XML_XRCFILE_STR) );
        AddFrameResource(
            cbC2U( Elem->Attribute(XML_FNAME_STR) ),
            cbC2U( Elem->Attribute(XML_CNAME_STR) ),
            cbC2U( Elem->Attribute(XML_SFILE_STR) ),
            cbC2U( Elem->Attribute(XML_HFILE_STR) ),
            Xrc );
    }

    // Loading panel resources

    for ( Elem = MainNode->FirstChildElement(XML_PANEL_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_PANEL_STR) )
    {
    	wxString Mode = cbC2U(Elem->Attribute(XML_EDITMODE_STR));
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : cbC2U( Elem->Attribute(XML_XRCFILE_STR) );
        AddPanelResource(
            cbC2U( Elem->Attribute(XML_FNAME_STR) ),
            cbC2U( Elem->Attribute(XML_CNAME_STR) ),
            cbC2U( Elem->Attribute(XML_SFILE_STR) ),
            cbC2U( Elem->Attribute(XML_HFILE_STR) ),
            Xrc );
    }

    // Loading configuration stuff

    Elem = MainNode->FirstChildElement(XML_CONFIG_STR);
    if ( Elem )
    {
        AppFile = cbC2U( Elem->Attribute(XML_APPFILE_STR) );
        MainResource = cbC2U( Elem->Attribute(XML_MAINRES_STR) );
        wxString InitAllMode = cbC2U( Elem->Attribute(XML_INITALL_STR));
        if ( InitAllMode == _T("never") )
        {
            CallInitAll = false;
            CallInitAllNecessary = false;
        }
        else if ( InitAllMode == _T("always") )
        {
            CallInitAll = true;
            CallInitAllNecessary = false;
        }
        else
        {
            CallInitAll = true;
            CallInitAllNecessary = true;
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
                LoadedResources.Add(FileName);
            }
        }
    }

    SetModified(false);

    return true;
}

void wxsProject::AddDialogResource( const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName, const wxString& XrcName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,XrcName,_T("Dialog"));
}

void wxsProject::AddFrameResource( const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName, const wxString& XrcName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,XrcName,_T("Frame"));
}

void wxsProject::AddPanelResource(const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName, const wxString& XrcName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,XrcName,_T("Panel"));
}

void wxsProject::AddWindowResource(
    const wxString& FileName,
    const wxString& ClassName,
    const wxString& SourceName,
    const wxString& HeaderName,
    const wxString& XrcName,
    const wxString& Type)
{
    if ( !FileName   || !ClassName  || !SourceName || !HeaderName )
        return;

    if ( !CheckProjFileExists(SourceName) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't find source file '%s'"),SourceName.c_str());
        Manager::Get()->GetMessageManager()->Log(_("Not all resources will be loaded"));
        return;
    }

    if ( !CheckProjFileExists(HeaderName) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't find header file '%s'"),HeaderName.c_str());
        Manager::Get()->GetMessageManager()->Log(_("Not all resources will be loaded"));
        return;
    }

    /* Creating resource */

    wxString RealFileName = GetInternalFileName(FileName);
    wxsWindowRes* Res = NULL;
    int EditMode = !XrcName ? wxsREMSource : wxsREMMixed;

    if ( Type == _T("Dialog") )
    {
        Res = new wxsDialogRes(this,EditMode,ClassName,RealFileName,SourceName,HeaderName,XrcName);
    }
    else if ( Type == _T("Panel") )
    {
        Res = new wxsPanelRes(this,EditMode,ClassName,RealFileName,SourceName,HeaderName,XrcName);
    }
    else if ( Type == _T("Frame") )
    {
        Res = new wxsFrameRes(this,EditMode,ClassName,RealFileName,SourceName,HeaderName,XrcName);
    }

    if ( !Res )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't create new resource"));
        return;
    }

    Res->Load();

    // Validating and correcting resource

    Res->UpdateWidgetsVarNameId();
    if ( !Res->CheckBaseProperties(true) )
    {
    	wxMessageBox(wxString::Format(_("Corrected some invalid properties for resource '%s'.\n"),Res->GetResourceName().c_str()));
    	Res->NotifyChange();
    }

    if ( Type == _T("Dialog") )
    {
        Dialogs.push_back((wxsDialogRes*)Res);
    }
    else if ( Type == _T("Panel") )
    {
        Panels.push_back((wxsPanelRes*)Res);
    }
    else if ( Type == _T("Frame") )
    {
        Frames.push_back((wxsFrameRes*)Res);
    }

    SetModified(true);
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
    XmlStore(Doc);
    return Doc;
}

void wxsProject::XmlStore(TiXmlNode* Node)
{
    if ( Integration != Integrated ) return;

    TiXmlNode* Elem = Node->InsertEndChild(TiXmlElement("wxsmith"));

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        TiXmlElement Dlg(XML_DIALOG_STR);
        wxsDialogRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Dlg.SetAttribute(XML_FNAME_STR,cbU2C(WxsFile.GetFullPath()));
        Dlg.SetAttribute(XML_CNAME_STR,cbU2C(Sett->GetClassName()));
        Dlg.SetAttribute(XML_SFILE_STR,cbU2C(Sett->GetSourceFile()));
        Dlg.SetAttribute(XML_HFILE_STR,cbU2C(Sett->GetHeaderFile()));
        Dlg.SetAttribute(XML_XRCFILE_STR,cbU2C(Sett->GetXrcFile()));
        Dlg.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsREMSource?"Source":"Xrc");
        Elem->InsertEndChild(Dlg);
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        TiXmlElement Frm(XML_FRAME_STR);
        wxsFrameRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Frm.SetAttribute(XML_FNAME_STR,cbU2C(WxsFile.GetFullPath()));
        Frm.SetAttribute(XML_CNAME_STR,cbU2C(Sett->GetClassName()));
        Frm.SetAttribute(XML_SFILE_STR,cbU2C(Sett->GetSourceFile()));
        Frm.SetAttribute(XML_HFILE_STR,cbU2C(Sett->GetHeaderFile()));
        Frm.SetAttribute(XML_XRCFILE_STR,cbU2C(Sett->GetWxsFile()));
        Frm.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsREMSource?"Source":"Xrc");
        Elem->InsertEndChild(Frm);
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        TiXmlElement Pan(XML_PANEL_STR);
        wxsPanelRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Pan.SetAttribute(XML_FNAME_STR,cbU2C(WxsFile.GetFullPath()));
        Pan.SetAttribute(XML_CNAME_STR,cbU2C(Sett->GetClassName()));
        Pan.SetAttribute(XML_SFILE_STR,cbU2C(Sett->GetSourceFile()));
        Pan.SetAttribute(XML_HFILE_STR,cbU2C(Sett->GetHeaderFile()));
        Pan.SetAttribute(XML_XRCFILE_STR,cbU2C(Sett->GetWxsFile()));
        Pan.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsREMSource?"Source":"Xrc");
        Elem->InsertEndChild(Pan);
    }

    TiXmlElement* Config = Elem->InsertEndChild(TiXmlElement(XML_CONFIG_STR))->ToElement();
    if ( Config )
    {
        Config->SetAttribute(XML_APPFILE_STR,cbU2C(AppFile));
        Config->SetAttribute(XML_MAINRES_STR,cbU2C(MainResource));

        if ( CallInitAll && CallInitAllNecessary )
        {
            Config->SetAttribute(XML_INITALL_STR,"necessary");
        }
        else if ( CallInitAll )
        {
            Config->SetAttribute(XML_INITALL_STR,"always");
        }
        else
        {
            Config->SetAttribute(XML_INITALL_STR,"never");
        }
    }

    for ( size_t i=0; i< LoadedResources.GetCount(); ++i )
    {
        TiXmlNode* Node = Elem->InsertEndChild(TiXmlElement(XML_AUTOLOAD_STR));
        if ( Node ) Node->InsertEndChild(TiXmlText(cbU2C(LoadedResources[i])));
    }

}

//void wxsProject::SaveProject()
//{
//
//    if ( Integration != Integrated ) return;
//    if ( !GetModified() ) return;
//
//    WorkingPath.SetName(wxSmithMainConfigFile);
//    WorkingPath.SetExt(_T(""));
//    WorkingPath.Assign(WorkingPath.GetFullPath());  // Reparsing path
//
//    TiXmlDocument* Doc = GenerateXml();
//
//    if ( Doc )
//    {
//        Doc->SaveFile(cbU2C(WorkingPath.GetFullPath()));
//        delete Doc;
//    }
//
//    SetModified(false);
//}
//
void wxsProject::DeleteDialog(wxsDialogRes* Resource)
{
    if ( DuringClear ) return;

    DialogListI i;
    for ( i=Dialogs.begin(); i!=Dialogs.end(); ++i ) if ( *i == Resource ) break;
    if ( i == Dialogs.end() ) return;
    Dialogs.erase(i);
}

void wxsProject::DeleteFrame(wxsFrameRes* Resource)
{
    if ( DuringClear ) return;

    FrameListI i;
    for ( i=Frames.begin(); i!=Frames.end(); ++i ) if ( *i == Resource ) break;
    if ( i == Frames.end() ) return;
    Frames.erase(i);
}

void wxsProject::DeletePanel(wxsPanelRes* Resource)
{
    if ( DuringClear ) return;

    PanelListI i;
    for ( i=Panels.begin(); i!=Panels.end(); ++i ) if ( *i == Resource ) break;
    if ( i == Panels.end() ) return;
    Panels.erase(i);
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
        wxMessageBox(_("Couldn't create wxsmith directory in main projet's path"),_("Error"),wxOK|wxICON_ERROR);
        return false;
    }

    Integration = Integrated;

    SetModified(true);

    AnnoyingDialog dlg(_("wxSmith extensions added"),
        _("wxSmith extensions were added to this project.\n"
          "\n"
          "Because main application code is not managed by\n"
          "wxSmith, remember about proper wxWidgets initialization:\n"
          " * call wxXmlResource::Get()->InitAllHandlers() before using any XRC files\n"
          " * call wxInitAllImageHandlers() before using graphic files other than .bmp\n"),
        wxART_INFORMATION,
        AnnoyingDialog::OK,
        wxID_OK);
    dlg.ShowModal();

    //SaveProject();

    BuildTree(wxsTREE(),TreeItem);

    return true;
}

void wxsProject::AddDialog(wxsDialogRes* Dialog)
{
    if ( !Dialog ) return;
    Dialogs.push_back(Dialog);
    Dialog->BuildTree(wxsTREE(), DialogId, true);
    SetModified(true);
}

void wxsProject::AddFrame(wxsFrameRes* Frame)
{
    if ( !Frame ) return;
    Frames.push_back(Frame);
    Frame->BuildTree(wxsTREE(), FrameId, true);
    SetModified(true);
}

void wxsProject::AddPanel(wxsPanelRes* Panel)
{
    if ( !Panel ) return;
    Panels.push_back(Panel);
    Panel->BuildTree(wxsTREE(), PanelId, true);
    SetModified(true);
}

wxsResource* wxsProject::FindResource(const wxString& Name)
{
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( (*i)->GetResourceName() == Name ) return *i;
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        if ( (*i)->GetResourceName() == Name ) return *i;
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        if ( (*i)->GetResourceName() == Name ) return *i;
    }

    return NULL;

}

void wxsProject::SendEventToEditors(wxEvent& event)
{
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( (*i)->GetEditor() )
        {
        	(*i)->GetEditor()->ProcessEvent(event);
        }
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        if ( (*i)->GetEditor() )
        {
        	(*i)->GetEditor()->ProcessEvent(event);
        }
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        if ( (*i)->GetEditor() )
        {
        	(*i)->GetEditor()->ProcessEvent(event);
        }
    }
}

void wxsProject::Configure()
{
    wxsProjectConfigurationDlg Dlg(0L,this);
    Dlg.ShowModal();
}

bool wxsProject::SetMainResource(const wxString& NewMainResource)
{
    bool Found = false;

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        if ( (*i)->GetResourceName() == NewMainResource )
        {
            Found = true;
            break;
        }
    }

    if ( !Found ) for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        if ( (*i)->GetResourceName() == NewMainResource )
        {
            Found = true;
            break;
        }
    }

    if ( Found )
    {
        MainResource = NewMainResource;
    }
    else
    {
        MainResource = _T("");
    }
    RebuildAppCode();
    SetModified(true);
    return true;
}

void wxsProject::SetCallInitAll(bool NewInitAll,bool NewNecessary)
{
    CallInitAll = NewInitAll;
    CallInitAllNecessary = NewInitAll && NewNecessary;
    RebuildAppCode();
    SetModified(true);
}

void wxsProject::SetAutoLoadedResources(const wxArrayString& Array)
{
    LoadedResources = Array;
    RebuildAppCode();
    SetModified(true);
}

void wxsProject::EnumerateResources(wxArrayString& Array,bool MainOnly)
{
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        Array.Add((*i)->GetResourceName());
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        Array.Add((*i)->GetResourceName());
    }

    if ( !MainOnly ) for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        Array.Add((*i)->GetResourceName());
    }
}

void wxsProject::RebuildAppCode()
{
    if ( AppFile.empty() ) return;

    bool InitAllHandlers = GetCallInitAll();
    bool MainResourceValid = false;
    bool CheckInitAll = GetCallInitAllNecessary() && InitAllHandlers;
    bool CheckMainRes = MainResource.Length();
    bool IsAnyXRC = false;
    wxsWindowRes* MainResPtr = NULL;

    // Finding out if we are using Xrc
    if ( CheckInitAll || CheckMainRes )
    {
        for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
        {
            if ( CheckMainRes && (MainResource == (*i)->GetResourceName()) )
            {
                MainResourceValid = true;
                MainResPtr = *i;
            }

            if ( CheckInitAll && ( (*i)->GetEditMode() == wxsREMMixed ) )
            {
                IsAnyXRC = true;
            }
        }

        for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
        {
            if ( CheckMainRes && (MainResource == (*i)->GetResourceName()) )
            {
                MainResourceValid = true;
                MainResPtr = *i;
            }

            if ( CheckInitAll && ( (*i)->GetEditMode() == wxsREMMixed ) )
            {
                IsAnyXRC = true;
            }
        }

        for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
        {
            if ( CheckInitAll && ( (*i)->GetEditMode() == wxsREMMixed ) )
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
    wxString NewCode = CodeHeader;

    NewCode.Append(_T("\nbool wxsOK = true;\n"));
    NewCode.Append(_T("::wxInitAllImageHandlers();\n"));

    if ( InitAllHandlers )
    {
        NewCode.Append(_T("wxXmlResource::Get()->InitAllHandlers();\n"));
    }

    for ( size_t i = 0; i<LoadedResources.Count(); ++i )
    {
        NewCode.Append(_T("wxsOK = wxsOK && wxXmlResource::Get()->Load(_T(\""));
        NewCode.Append(LoadedResources[i]);
        NewCode.Append(_T("\"));\n"));
    }

    if ( MainResourceValid )
    {
        NewCode.Append(
            wxString::Format(
                _T("if ( wxsOK )\n")
                _T("{\n")
                _T("\t%s* MainResource = new %s(0L);\n")
                _T("\tif ( MainResource ) MainResource->Show();\n")
                _T("}\n"),
                MainResource.c_str(),
                MainResource.c_str()));
    }

    wxsCoder::Get()->AddCode(GetProjectFileName(AppFile),CodeHeader,NewCode);

    CodeHeader = wxsBBegin() _T("AppHeaders");
    NewCode = CodeHeader;
    NewCode.Append(_T("\n"));

    if ( MainResourceValid && MainResPtr )
    {
        NewCode.Append(
            wxString::Format(
                _T("#include \"%s\"\n"),
                MainResPtr->GetHeaderFile().c_str()));
    }

    if ( IsAnyXRC || LoadedResources.Count() )
    {
        NewCode.Append(_T("#include <wx/xrc/xmlres.h>\n"));
    }

    NewCode.Append(_T("#include <wx/image.h>\n"));

    wxsCoder::Get()->AddCode(GetProjectFileName(AppFile),CodeHeader,NewCode);
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
    return IsAppSourceManager(GetAppSourceFile());
}

bool wxsProject::IsAppSourceManager(const wxString& FileName)
{
    if ( FileName.empty() ) return false;

    if ( wxsCoder::Get()->GetCode(
        GetProjectFileName(FileName),
        wxsBBegin() _T("AppInitialize") ).empty() ) return false;

    if ( wxsCoder::Get()->GetCode(
        GetProjectFileName(FileName),
        wxsBBegin() _T("AppHeaders") ).empty() ) return false;

    return true;
}

bool wxsProject::SetAppSourceFile(const wxString& NewAppFile)
{
    AppFile = NewAppFile;
    return true;
}

void wxsProject::SetModified(bool Modified)
{
    if ( Modified )
        Project->SetModified(true);
}
