#include "wxsproject.h"

#include "widget.h"
#include "defwidgets/wxsstdmanager.h"
#include "defwidgets/wxsdialog.h"
#include "resources/wxswindowres.h"
#include "wxswidgetfactory.h"
#include "wxsmith.h"
#include <wx/string.h>

#define XML_DIALOG_STR   "dialog"
#define XML_FRAME_STR    "frame"
#define XML_PANEL_STR    "panel"
#define XML_FNAME_STR    "wxs_file"
#define XML_CNAME_STR    "class"
#define XML_SFILE_STR    "src_file"
#define XML_HFILE_STR    "header_file"
#define XML_XRCFILE_STR  "xrc_file"
#define XML_EDITMODE_STR "edit_mode"


wxsProject::wxsProject():  Integration(NotBinded), Project(NULL), DuringClear(false)
{}

wxsProject::~wxsProject()
{
    Clear();
}

wxsProject::IntegrationState wxsProject::BindProject(cbProject* Proj)
{

    Clear();

    /* creating new node in resource tree */
    
    wxTreeCtrl* ResTree = wxSmith::Get()->GetResourceTree();
    ResTree->Expand(ResTree->GetRootItem());
    TreeItem = ResTree->AppendItem(ResTree->GetRootItem(),Proj->GetTitle());
    
    /* Binding project object */
    if ( Proj && !Proj->IsLoaded() )
    {
        Proj = NULL;
    }
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

    if ( ! WorkingPath.FileExists() )
    {
        return Integration = NotWxsProject;
    }

    /* Trying to read configuration data */

    TiXmlDocument Doc(WorkingPath.GetFullPath().mb_str());

    if ( !Doc.LoadFile() )
    {
        return Integration = NotWxsProject;
    }

    TiXmlNode* MainNode = Doc.FirstChild("wxsmith");

    if ( MainNode == NULL || ! LoadFromXml(MainNode) )
    {
        return Integration = NotWxsProject;
    }

    BuildTree(ResTree,TreeItem);
    
    return Integration = Integrated;
}

inline void wxsProject::Clear()
{
    DuringClear = true;
    
    if ( Project ) wxSmith::Get()->GetResourceTree()->Delete(TreeItem);
    
    Integration = NotBinded;
    Project = NULL;
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
    wxSmith::Get()->GetResourceTree()->Refresh();
    DuringClear = false;
}

void wxsProject::BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd)
{
    DialogId = Tree->AppendItem(WhereToAdd,_("Dialog resources"));
    FrameId  = Tree->AppendItem(WhereToAdd,_("Frame resources"));
    PanelId  = Tree->AppendItem(WhereToAdd,_("Panel resources"));

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        (*i)->GetDialog().BuildTree(
            Tree,
            Tree->AppendItem(
                DialogId,
                (*i)->GetClassName(),
                -1, -1,
                new wxsResourceTreeData(*i) ) );
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        (*i)->GetFrame().BuildTree(
            Tree,
            Tree->AppendItem(
                FrameId,
                (*i)->GetClassName(),
                -1, -1,
                new wxsResourceTreeData(*i) ) );
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        (*i)->GetPanel().BuildTree(
            Tree,
            Tree->AppendItem(
                PanelId,
                (*i)->GetClassName(),
                -1, -1,
                new wxsResourceTreeData(*i) ) );
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
    	wxString Mode = Elem->Attribute(XML_EDITMODE_STR);
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : wxString ( Elem->Attribute(XML_XRCFILE_STR), wxConvUTF8 );
        AddDialogResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ),
            Xrc );
    }

    // Loading frame resources

    for ( Elem = MainNode->FirstChildElement(XML_FRAME_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_FRAME_STR) )
    {
    	wxString Mode = Elem->Attribute(XML_EDITMODE_STR);
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : wxString ( Elem->Attribute(XML_XRCFILE_STR), wxConvUTF8 );
        AddFrameResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ),
            Xrc );
    }

    // Loading panel resources

    for ( Elem = MainNode->FirstChildElement(XML_PANEL_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_PANEL_STR) )
    {
    	wxString Mode = Elem->Attribute(XML_EDITMODE_STR);
    	wxString Xrc = ( Mode == _T("Source") ) ? _T("") : wxString ( Elem->Attribute(XML_XRCFILE_STR), wxConvUTF8 );
        AddPanelResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ),
            Xrc );
    }
    
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

    /* Opening wxs data */

    wxString RealFileName = GetInternalFileName(FileName);
    
    TiXmlDocument Doc(RealFileName.mb_str());
    TiXmlElement* Resource;
    
    if ( !  Doc.LoadFile() ||
         ! (Resource = Doc.FirstChildElement("resource")) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load resource data"));
        return;
    }
    
    /* Finding dialog object */
    
    TiXmlElement* XmlWindow = Resource->FirstChildElement("object");
    while ( XmlWindow )
    {
    	wxString TypeName = _T("wx") + Type;
        if ( !strcmp(XmlWindow->Attribute("class"),TypeName.mb_str()) &&
             !strcmp(XmlWindow->Attribute("name"),ClassName.mb_str()) )
        {
            break;
        }
        
        XmlWindow = XmlWindow->NextSiblingElement("object");
    }
    
    if ( !XmlWindow ) return;
    
    /* Creating dialog */

    wxsWindowRes* Res = NULL;
    int EditMode = !XrcName ? wxsResSource : wxsResSource | wxsResFile;
    
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
    
    if ( ! (Res->GetRootWidget()->XmlLoad(XmlWindow))  )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load xrc data"));
        delete Res;
        return;
    }
    
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
    TiXmlNode* Elem = Doc->InsertEndChild(TiXmlElement("wxsmith"));
    
    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        TiXmlElement Dlg(XML_DIALOG_STR);
        wxsDialogRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Dlg.SetAttribute(XML_FNAME_STR,WxsFile.GetFullPath().mb_str());
        Dlg.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Dlg.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Dlg.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
        Dlg.SetAttribute(XML_XRCFILE_STR,Sett->GetXrcFile().mb_str());
        Dlg.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsResSource?"Source":"Xrc");
        Elem->InsertEndChild(Dlg);
    }
    
    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        TiXmlElement Frm(XML_FRAME_STR);
        wxsFrameRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Frm.SetAttribute(XML_FNAME_STR,WxsFile.GetFullPath().mb_str());
        Frm.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Frm.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Frm.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
        Frm.SetAttribute(XML_XRCFILE_STR,Sett->GetWxsFile().mb_str());
        Frm.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsResSource?"Source":"Xrc");
        Elem->InsertEndChild(Frm);
    }
    
    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        TiXmlElement Pan(XML_PANEL_STR);
        wxsPanelRes* Sett = *i;
        wxFileName WxsFile(Sett->GetWxsFile());
        WxsFile.MakeRelativeTo(WorkingPath.GetPath());
        Pan.SetAttribute(XML_FNAME_STR,WxsFile.GetFullPath().mb_str());
        Pan.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Pan.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Pan.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
        Pan.SetAttribute(XML_XRCFILE_STR,Sett->GetWxsFile().mb_str());
        Pan.SetAttribute(XML_EDITMODE_STR,Sett->GetEditMode()==wxsResSource?"Source":"Xrc");
        Elem->InsertEndChild(Pan);
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
        Doc->SaveFile(WorkingPath.GetFullPath().mb_str());
        delete Doc;
    }

    for ( DialogListI i = Dialogs.begin(); i!=Dialogs.end(); ++i )
    {
        (*i)->Save();
    }

    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        (*i)->Save();
    }

    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        (*i)->Save();
    }
}

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
    
    SaveProject();
    
    BuildTree(wxSmith::Get()->GetResourceTree(),TreeItem);
    
    return true;
}

void wxsProject::AddDialog(wxsDialogRes* Dialog)
{
    if ( !Dialog ) return;
    Dialogs.push_back(Dialog);
    wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
    Dialog->GetDialog().BuildTree(Tree, Tree->AppendItem( DialogId, Dialog->GetClassName(), -1, -1, new wxsResourceTreeData(Dialog) ) );
}

void wxsProject::AddFrame(wxsFrameRes* Frame)
{
    if ( !Frame ) return;
    Frames.push_back(Frame);
    wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
    Frame->GetFrame().BuildTree(Tree, Tree->AppendItem( FrameId, Frame->GetClassName(), -1, -1, new wxsResourceTreeData(Frame) ) );
}

void wxsProject::AddPanel(wxsPanelRes* Panel)
{
    if ( !Panel ) return;
    Panels.push_back(Panel);
    wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
    Panel->GetPanel().BuildTree(Tree, Tree->AppendItem( PanelId, Panel->GetClassName(), -1, -1, new wxsResourceTreeData(Panel) ) );
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
