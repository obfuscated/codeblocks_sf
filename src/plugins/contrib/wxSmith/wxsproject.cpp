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
#define XML_FNAME_STR    "xrc_file"
#define XML_CNAME_STR    "class"
#define XML_SFILE_STR    "src_file"
#define XML_HFILE_STR    "header_file"


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
        AddDialogResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ) );
    }

    // Loading frame resources

    for ( Elem = MainNode->FirstChildElement(XML_FRAME_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_FRAME_STR) )
    {
        AddFrameResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ) );
    }

    // Loading panel resources

    for ( Elem = MainNode->FirstChildElement(XML_PANEL_STR);
            Elem;
            Elem = Elem->NextSiblingElement(XML_PANEL_STR) )
    {
        AddPanelResource(
            wxString ( Elem->Attribute(XML_FNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_CNAME_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_SFILE_STR), wxConvUTF8 ),
            wxString ( Elem->Attribute(XML_HFILE_STR), wxConvUTF8 ) );
    }
    
    return true;
}

void wxsProject::AddDialogResource( const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,wxsWindowRes::Dialog);
}

void wxsProject::AddFrameResource( const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,wxsWindowRes::Frame);
}

void wxsProject::AddPanelResource(const wxString& FileName, const wxString& ClassName, const wxString& SourceName, const wxString& HeaderName)
{
	AddWindowResource(FileName,ClassName,SourceName,HeaderName,wxsWindowRes::Panel);
}

void wxsProject::AddWindowResource(
    const wxString& FileName,
    const wxString& ClassName,
    const wxString& SourceName,
    const wxString& HeaderName,
    int Type)
{
    if ( !FileName   || !*FileName   || !ClassName  || !*ClassName ||
         !SourceName || !*SourceName || !HeaderName || !*HeaderName )
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

    /* Opening xrc data */

    wxFileName Name;
    Name.Assign(WorkingPath.GetPath(),wxString(FileName));
    
    TiXmlDocument Doc(Name.GetFullPath().mb_str());
    TiXmlElement* Resource;
    
    if ( !  Doc.LoadFile() ||
         ! (Resource = Doc.FirstChildElement("resource")) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load xrc data"));
        return;
    }
    
    /* Finding dialog object */
    
    TiXmlElement* XmlDialog = Resource->FirstChildElement("object");
    while ( XmlDialog )
    {
    	const char* TypeName = "";
    	switch ( Type )
    	{
    		case wxsWindowRes::Dialog: TypeName = "wxDialog"; break;
    		case wxsWindowRes::Panel:  TypeName = "wxPanel" ; break;
    		case wxsWindowRes::Frame:  TypeName = "wxFrame" ; break;
    	}
        if ( !strcmp(XmlDialog->Attribute("class"),TypeName) &&
             !strcmp(XmlDialog->Attribute("name"),ClassName.mb_str()) )
        {
            break;
        }
        
        XmlDialog = Resource->NextSiblingElement("object");
    }
    
    if ( !XmlDialog ) return;
    
    /* Creating dialog */

    wxsWindowRes* Res = NULL;
    
    switch ( Type )
    {
        case wxsWindowRes::Dialog: Res = new wxsDialogRes(this,ClassName,FileName,SourceName,HeaderName); break;
        case wxsWindowRes::Panel:  Res = new wxsPanelRes(this,ClassName,FileName,SourceName,HeaderName); break;
        case wxsWindowRes::Frame:  Res = new wxsFrameRes(this,ClassName,FileName,SourceName,HeaderName); break;
    }
    
    if ( !Res )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't create new resource"));
        return;
    }
    
    if ( ! (Res->GetRootWidget()->XmlLoad(XmlDialog))  )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load xrc data"));
        delete Res;
        return;
    }
    
    // Validating and correcting resource
    
    Res->UpdateWidgetsVarNameId();
    if ( !Res->CheckBaseProperties(true) )
    {
    	wxMessageBox(wxString::Format(_("Some properties for resource '%s' had invalid values and were corrected.\n"),Res->GetResourceName().c_str()));
    	Res->NotifyChange();
    }
    
    switch ( Type )
    {
        case wxsWindowRes::Dialog: Dialogs.push_back((wxsDialogRes*)Res); break;
        case wxsWindowRes::Panel:  Panels.push_back((wxsPanelRes*)Res); break;
        case wxsWindowRes::Frame:  Frames.push_back((wxsFrameRes*)Res); break;
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
        Dlg.SetAttribute(XML_FNAME_STR,Sett->GetXrcFile().mb_str());
        Dlg.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Dlg.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Dlg.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
        Elem->InsertEndChild(Dlg);
    }
    
    for ( FrameListI i = Frames.begin(); i!=Frames.end(); ++i )
    {
        TiXmlElement Frm(XML_FRAME_STR);
        wxsFrameRes* Sett = *i;
        Frm.SetAttribute(XML_FNAME_STR,Sett->GetXrcFile().mb_str());
        Frm.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Frm.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Frm.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
        Elem->InsertEndChild(Frm);
    }
    
    for ( PanelListI i = Panels.begin(); i!=Panels.end(); ++i )
    {
        TiXmlElement Pan(XML_PANEL_STR);
        wxsPanelRes* Sett = *i;
        Pan.SetAttribute(XML_FNAME_STR,Sett->GetXrcFile().mb_str());
        Pan.SetAttribute(XML_CNAME_STR,Sett->GetClassName().mb_str());
        Pan.SetAttribute(XML_SFILE_STR,Sett->GetSourceFile().mb_str());
        Pan.SetAttribute(XML_HFILE_STR,Sett->GetHeaderFile().mb_str());
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
