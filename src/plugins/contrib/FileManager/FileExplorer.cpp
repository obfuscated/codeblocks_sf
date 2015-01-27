#include "FileExplorer.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/aui/aui.h>


#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/dnd.h>
    #include <wx/imaglist.h>

    #include <cbproject.h>
    #include <configmanager.h>
    #include <projectmanager.h>
#endif

#include <list>
#include <vector>
#include <iostream>


#include "se_globals.h"



#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(VCSstatearray);

int ID_UPDATETIMER=wxNewId();
int ID_FILETREE=wxNewId();
int ID_FILELOC=wxNewId();
int ID_FILEWILD=wxNewId();
int ID_SETLOC=wxNewId();

int ID_OPENINED=wxNewId();
int ID_FILENEWFILE=wxNewId();
int ID_FILENEWFOLDER=wxNewId();
int ID_FILEMAKEFAV=wxNewId();
int ID_FILECOPY=wxNewId();
int ID_FILEDUP=wxNewId();
int ID_FILEMOVE=wxNewId();
int ID_FILEDELETE=wxNewId();
int ID_FILERENAME=wxNewId();
int ID_FILEEXPANDALL=wxNewId();
int ID_FILECOLLAPSEALL=wxNewId();
int ID_FILESETTINGS=wxNewId();
int ID_FILESHOWHIDDEN=wxNewId();
int ID_FILEPARSECVS=wxNewId();
int ID_FILEPARSESVN=wxNewId();
int ID_FILEPARSEHG=wxNewId();
int ID_FILEPARSEBZR=wxNewId();
int ID_FILEPARSEGIT=wxNewId();
int ID_FILE_UPBUTTON=wxNewId();
int ID_FILEREFRESH=wxNewId();
int ID_FILEADDTOPROJECT=wxNewId();

class UpdateQueue
{
public:
    void Add(const wxTreeItemId &ti)
    {
        for(std::list<wxTreeItemId>::iterator it=qdata.begin();it!=qdata.end();it++)
        {
            if(*it==ti)
            {
                qdata.erase(it);
                break;
            }
        }
        qdata.push_front(ti);
    }
    bool Pop(wxTreeItemId &ti)
    {
        if(qdata.empty())
            return false;
        ti=qdata.front();
        qdata.pop_front();
        return true;
    }
    void Clear()
    {
        qdata.clear();
    }
private:
    std::list<wxTreeItemId> qdata;
};


class DirTraverseFind : public wxDirTraverser     {
public:
    DirTraverseFind(const wxString& wildcard) : m_files(), m_wildcard(wildcard) { }
    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
        if(WildCardListMatch(m_wildcard,filename,true))
            m_files.Add(filename);
        return wxDIR_CONTINUE;
    }
    virtual wxDirTraverseResult OnDir(const wxString& dirname)
    {
        if(WildCardListMatch(m_wildcard,dirname,true))
            m_files.Add(dirname);
        return wxDIR_CONTINUE;
    }
    wxArrayString& GetMatches() {return m_files;}
private:
    wxArrayString m_files;
    wxString m_wildcard;
};


class FEDataObject:public wxDataObjectComposite
{
public:
   FEDataObject():wxDataObjectComposite()
   {
       m_file=new wxFileDataObject;
       Add(m_file,true);
   }
   wxFileDataObject *m_file;

};


class wxFEDropTarget: public wxDropTarget
{
public:
    wxFEDropTarget(FileExplorer *fe):wxDropTarget()
    {
        m_fe=fe;
        m_data_object=new FEDataObject();
        SetDataObject(m_data_object);
    }
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def)
    {
        GetData();
        if(m_data_object->GetReceivedFormat().GetType()==wxDF_FILENAME )
        {
            wxArrayString as=m_data_object->m_file->GetFilenames();
            wxTreeCtrl *tree=m_fe->m_Tree;
            int flags;
            wxTreeItemId id=tree->HitTest(wxPoint(x,y),flags);
            if(!id.IsOk())
                return wxDragCancel;
            if(tree->GetItemImage(id)!=fvsFolder)
                return wxDragCancel;
            if(!(flags&(wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMLABEL)))
                return wxDragCancel;
            if(def==wxDragCopy)
            {
                m_fe->CopyFiles(m_fe->GetFullPath(id),as);
                return def;
            }
            if(def==wxDragMove)
            {
                m_fe->MoveFiles(m_fe->GetFullPath(id),as);
                return def;
            }
            return wxDragCancel;
        }
//            if(sizeof(wxFileDataObject)!=m_data_object->GetDataSize(wxDF_FILENAME))
//            {
//                wxMessageBox(wxString::Format(_("Drop files %i,%i"),sizeof(wxFileDataObject),m_data_object->GetDataSize(wxDF_FILENAME)));
//                return wxDragCancel;
//            }
        return wxDragCancel;
    }
    virtual bool OnDrop(wxCoord /*x*/, wxCoord /*y*/, int /*tab*/, wxWindow */*wnd*/)
    {
        return true;
    }
    virtual wxDragResult OnDragOver(wxCoord /*x*/, wxCoord /*y*/, wxDragResult def)
    {
        return def;
    }
private:
    FEDataObject *m_data_object;
    FileExplorer *m_fe;
};



BEGIN_EVENT_TABLE(FileTreeCtrl, wxTreeCtrl)
//    EVT_TREE_ITEM_ACTIVATED(ID_FILETREE, FileTreeCtrl::OnActivate)  //double click -
    EVT_KEY_DOWN(FileTreeCtrl::OnKeyDown)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(FileTreeCtrl, wxTreeCtrl)

FileTreeCtrl::FileTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style,
    const wxValidator& validator,
    const wxString& name)
    : wxTreeCtrl(parent,id,pos,size,style,validator,name) {}

FileTreeCtrl::FileTreeCtrl() { }

FileTreeCtrl::FileTreeCtrl(wxWindow *parent): wxTreeCtrl(parent) {}

FileTreeCtrl::~FileTreeCtrl()
{
}

void FileTreeCtrl::OnKeyDown(wxKeyEvent &event)
{
    if(event.GetKeyCode()==WXK_DELETE)
        ::wxPostEvent(GetParent(),event);
    else
        event.Skip(true);
}

int FileTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    if((GetItemImage(item1)==fvsFolder)>(GetItemImage(item2)==fvsFolder))
        return -1;
    if((GetItemImage(item1)==fvsFolder)<(GetItemImage(item2)==fvsFolder))
        return 1;
    return (GetItemText(item1).CmpNoCase(GetItemText(item2)));
}

BEGIN_EVENT_TABLE(FileExplorer, wxPanel)
    EVT_TIMER(ID_UPDATETIMER, FileExplorer::OnTimerCheckUpdates)
    EVT_MONITOR_NOTIFY(wxID_ANY, FileExplorer::OnDirMonitor)
    EVT_COMMAND(0, wxEVT_NOTIFY_UPDATE_TREE, FileExplorer::OnUpdateTreeItems)
    EVT_COMMAND(0, wxEVT_NOTIFY_EXEC_REQUEST, FileExplorer::OnExecRequest)
    EVT_TREE_BEGIN_DRAG(ID_FILETREE, FileExplorer::OnBeginDragTreeItem)
    EVT_TREE_END_DRAG(ID_FILETREE, FileExplorer::OnEndDragTreeItem)
    EVT_BUTTON(ID_FILE_UPBUTTON, FileExplorer::OnUpButton)
    EVT_MENU(ID_SETLOC, FileExplorer::OnSetLoc)
    EVT_MENU(ID_OPENINED, FileExplorer::OnOpenInEditor)
    EVT_MENU(ID_FILENEWFILE, FileExplorer::OnNewFile)
    EVT_MENU(ID_FILENEWFOLDER,FileExplorer::OnNewFolder)
    EVT_MENU(ID_FILEMAKEFAV,FileExplorer::OnAddFavorite)
    EVT_MENU(ID_FILECOPY,FileExplorer::OnCopy)
    EVT_MENU(ID_FILEDUP,FileExplorer::OnDuplicate)
    EVT_MENU(ID_FILEMOVE,FileExplorer::OnMove)
    EVT_MENU(ID_FILEDELETE,FileExplorer::OnDelete)
    EVT_MENU(ID_FILERENAME,FileExplorer::OnRename)
    EVT_MENU(ID_FILEEXPANDALL,FileExplorer::OnExpandAll)
    EVT_MENU(ID_FILECOLLAPSEALL,FileExplorer::OnCollapseAll)
    EVT_MENU(ID_FILESETTINGS,FileExplorer::OnSettings)
    EVT_MENU(ID_FILESHOWHIDDEN,FileExplorer::OnShowHidden)
    EVT_MENU(ID_FILEPARSECVS,FileExplorer::OnParseCVS)
    EVT_MENU(ID_FILEPARSESVN,FileExplorer::OnParseSVN)
    EVT_MENU(ID_FILEPARSEHG,FileExplorer::OnParseHG)
    EVT_MENU(ID_FILEPARSEBZR,FileExplorer::OnParseBZR)
    EVT_MENU(ID_FILEPARSEGIT,FileExplorer::OnParseGIT)
    EVT_MENU(ID_FILEREFRESH,FileExplorer::OnRefresh)
    EVT_MENU(ID_FILEADDTOPROJECT,FileExplorer::OnAddToProject)
    EVT_KEY_DOWN(FileExplorer::OnKeyDown)
    EVT_TREE_ITEM_EXPANDING(ID_FILETREE, FileExplorer::OnExpand)
    //EVT_TREE_ITEM_COLLAPSED(id, func) //delete the children
    EVT_TREE_ITEM_ACTIVATED(ID_FILETREE, FileExplorer::OnActivate)  //double click - open file / expand folder (the latter is a default just need event.skip)
    EVT_TREE_ITEM_MENU(ID_FILETREE, FileExplorer::OnRightClick) //right click open context menu -- interpreter actions, rename, delete, copy, properties, set as root etc
    EVT_COMBOBOX(ID_FILELOC, FileExplorer::OnChooseLoc) //location selected from history of combo box - set as root
    EVT_COMBOBOX(ID_FILEWILD, FileExplorer::OnChooseWild) //location selected from history of combo box - set as root
    //EVT_TEXT(ID_FILELOC, FileExplorer::OnLocChanging) //provide autotext hint for dir name in combo box
    EVT_TEXT_ENTER(ID_FILELOC, FileExplorer::OnEnterLoc) //location entered in combo box - set as root
    EVT_TEXT_ENTER(ID_FILEWILD, FileExplorer::OnEnterWild) //location entered in combo box - set as root  ** BUG RIDDEN
END_EVENT_TABLE()

FileExplorer::FileExplorer(wxWindow *parent,wxWindowID id,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name):
    wxPanel(parent,id,pos,size,style, name)
{
    m_kill=false;
    m_update_queue=new UpdateQueue;
    m_updater=NULL;
    m_updatetimer=new wxTimer(this,ID_UPDATETIMER);
    m_update_active=false;
    m_updater_cancel=false;
    m_update_expand=false;
    m_dir_monitor=new wxDirectoryMonitor(this,wxArrayString());
    m_dir_monitor->Start();
    m_droptarget=new wxFEDropTarget(this);

    m_show_hidden=false;
    m_parse_cvs=false;
    m_parse_hg=false;
    m_parse_bzr=false;
    m_parse_git=false;
    m_parse_svn=false;
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* bsh = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bshloc = new wxBoxSizer(wxHORIZONTAL);
    m_Tree = new FileTreeCtrl(this, ID_FILETREE);
    m_Tree->SetIndent(m_Tree->GetIndent()/2);
    m_Tree->SetDropTarget(m_droptarget);
    m_Loc = new wxComboBox(this,ID_FILELOC,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxTE_PROCESS_ENTER|wxCB_DROPDOWN);
    m_WildCards = new wxComboBox(this,ID_FILEWILD,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxTE_PROCESS_ENTER|wxCB_DROPDOWN);
    m_UpButton = new wxButton(this,ID_FILE_UPBUTTON,_("^"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
    bshloc->Add(m_Loc, 1, wxEXPAND);
    bshloc->Add(m_UpButton, 0, wxEXPAND);
    bs->Add(bshloc, 0, wxEXPAND);
    bsh->Add(new wxStaticText(this,wxID_ANY,_("Mask: ")),0,wxALIGN_CENTRE);
    bsh->Add(m_WildCards,1);
    bs->Add(bsh, 0, wxEXPAND);
    bs->Add(m_Tree, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);

    SetImages();
    ReadConfig();
    if(m_Loc->GetCount()>m_favdirs.GetCount())
    {
        m_Loc->Select(m_favdirs.GetCount());
        m_root=m_Loc->GetString(m_favdirs.GetCount());
    } else
    {
        m_root=wxFileName::GetPathSeparator();
        m_Loc->Append(m_root);
        m_Loc->Select(0);
    }
    if(m_WildCards->GetCount()>0)
        m_WildCards->Select(0);
    SetRootFolder(m_root);

    SetSizer(bs);
}

FileExplorer::~FileExplorer()
{
    m_kill=true;
    m_updatetimer->Stop();
    delete m_dir_monitor;
    WriteConfig();
    UpdateAbort();
    delete m_update_queue;
    delete m_updatetimer;
}


bool FileExplorer::SetRootFolder(wxString root)
{
    UpdateAbort();
    if(root[root.Len()-1]!=wxFileName::GetPathSeparator())
        root=root+wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    wxFileName fnroot=wxFileName(root);
    if(fnroot.GetVolume().IsEmpty())
    {
        fnroot.SetVolume(wxFileName(::wxGetCwd()).GetVolume());
        root=fnroot.GetFullPath();//(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR)+fnroot.GetFullName();
    }
#endif
    wxDir dir(root);
    if (!dir.IsOpened())
    {
        // deal with the error here - wxDir would already log an error message
        // explaining the exact reason of the failure
        m_Loc->SetValue(m_root);
        return false;
    }
    m_root=root;
    m_Loc->SetValue(m_root);
    m_Tree->DeleteAllItems();
    m_Tree->AddRoot(m_root,fvsFolder);
    m_Tree->SetItemHasChildren(m_Tree->GetRootItem());
    m_Tree->Expand(m_Tree->GetRootItem());

    return true;
//    return AddTreeItems(m_Tree->GetRootItem());

}

// find a file in the filesystem below a selected root
void FileExplorer::FindFile(const wxString &findfilename, const wxTreeItemId &ti)
{
    wxString path=GetFullPath(ti);

    wxDir dir(path);

    if (!dir.IsOpened())
    {
        // deal with the error here - wxDir would already log an error message
        // explaining the exact reason for the failure
        return;
    }
    wxString filename;
    int flags=wxDIR_FILES|wxDIR_DIRS;
    if(m_show_hidden)
        flags|=wxDIR_HIDDEN;

    DirTraverseFind dtf(findfilename);
    m_findmatchcount=dir.Traverse(dtf,wxEmptyString,flags);
    m_findmatch=dtf.GetMatches();
}

// focus the item in the tree.
void FileExplorer::FocusFile(const wxTreeItemId &ti)
{
    m_Tree->SetFocus();
    m_Tree->UnselectAll();
    m_Tree->SelectItem(ti);
    m_Tree->EnsureVisible(ti);
}

wxTreeItemId FileExplorer::GetNextExpandedNode(wxTreeItemId ti)
{
    wxTreeItemId next_ti;
    if(!ti.IsOk())
    {
        return m_Tree->GetRootItem();
    }
    if(m_Tree->IsExpanded(ti))
    {
        wxTreeItemIdValue cookie;
        next_ti=m_Tree->GetFirstChild(ti,cookie);
        while(next_ti.IsOk())
        {
            if(m_Tree->IsExpanded(next_ti))
                return next_ti;
            next_ti=m_Tree->GetNextChild(ti,cookie);
        }
    }
    next_ti=m_Tree->GetNextSibling(ti);
    while(next_ti.IsOk())
    {
        if(m_Tree->IsExpanded(next_ti))
            return next_ti;
        next_ti=m_Tree->GetNextSibling(next_ti);
    }
    return m_Tree->GetRootItem();
}

bool FileExplorer::GetItemFromPath(const wxString &path, wxTreeItemId &ti)
{
    ti=m_Tree->GetRootItem();
    do
    {
        if(path==GetFullPath(ti))
            return true;
        ti=GetNextExpandedNode(ti);
    } while(ti!=m_Tree->GetRootItem());
    return false;
}


void FileExplorer::GetExpandedNodes(wxTreeItemId ti, Expansion *exp)
{
    exp->name=m_Tree->GetItemText(ti);
    wxTreeItemIdValue cookie;
    wxTreeItemId ch=m_Tree->GetFirstChild(ti,cookie);
    while(ch.IsOk())
    {
        if(m_Tree->IsExpanded(ch))
        {
            Expansion *e=new Expansion();
            GetExpandedNodes(ch,e);
            exp->children.push_back(e);
        }
        ch=m_Tree->GetNextChild(ti,cookie);
    }
}

void FileExplorer::GetExpandedPaths(wxTreeItemId ti,wxArrayString &paths)
{
    if(!ti.IsOk())
    {
        wxMessageBox(_("node error"));
        return;
    }
    if(m_Tree->IsExpanded(ti))
        paths.Add(GetFullPath(ti));
    wxTreeItemIdValue cookie;
    wxTreeItemId ch=m_Tree->GetFirstChild(ti,cookie);
    while(ch.IsOk())
    {
        if(m_Tree->IsExpanded(ch))
            GetExpandedPaths(ch,paths);
        ch=m_Tree->GetNextChild(ti,cookie);
    }
}

void FileExplorer::RefreshExpanded(wxTreeItemId ti)
{
    if(m_Tree->IsExpanded(ti))
        m_update_queue->Add(ti);
    wxTreeItemIdValue cookie;
    wxTreeItemId ch=m_Tree->GetFirstChild(ti,cookie);
    while(ch.IsOk())
    {
        if(m_Tree->IsExpanded(ch))
            RefreshExpanded(ch);
        ch=m_Tree->GetNextChild(ti,cookie);
    }
    m_updatetimer->Start(10,true);

}

void FileExplorer::Refresh(wxTreeItemId ti)
{
    //    Expansion e;
    //    GetExpandedNodes(ti,&e);
    //    RecursiveRebuild(ti,&e);
    //m_updating_node=ti;//m_Tree->GetRootItem();
    m_update_queue->Add(ti);
    m_updatetimer->Start(10,true);
}

void FileExplorer::UpdateAbort()
{
    if(!m_update_active)
        return;
    delete m_updater;
    m_update_active=false;
    m_updatetimer->Stop();
}

void FileExplorer::ResetDirMonitor()
{
    wxArrayString paths;
    GetExpandedPaths(m_Tree->GetRootItem(),paths);
    m_dir_monitor->ChangePaths(paths);
}

void FileExplorer::OnDirMonitor(wxDirectoryMonitorEvent &e)
{
    if(m_kill)
        return;
//  TODO: Apparently creating log messages during Code::Blocks shutdown can create segfaults
//    LogMessage(wxString::Format(_T("Dir Event: %s,%i,%s"),e.m_mon_dir.c_str(),e.m_event_type,e.m_info_uri.c_str()));
    if(e.m_event_type==MONITOR_TOO_MANY_CHANGES)
    {
//        LogMessage(_("directory change read error"));
    }
    wxTreeItemId ti;
    if(GetItemFromPath(e.m_mon_dir,ti))
    {
        m_update_queue->Add(ti);
        m_updatetimer->Start(100,true);
    }
}

void FileExplorer::OnTimerCheckUpdates(wxTimerEvent &/*e*/)
{
    if(m_kill)
        return;
    if(m_update_active)
        return;
    wxTreeItemId ti;
    while(m_update_queue->Pop(ti))
    {
        if(!ti.IsOk())
            continue;
        m_updater_cancel=false;
        m_updater=new FileExplorerUpdater(this);
        m_updated_node=ti;
        m_update_active=true;
        m_updater->Update(m_updated_node);
        break;
    }
}

bool FileExplorer::ValidateRoot()
{
    wxTreeItemId ti=m_Tree->GetRootItem();
    while(true)
    {
    if(!ti.IsOk())
        break;
    if(m_Tree->GetItemImage(ti)!=fvsFolder)
        break;
    if(!wxFileName::DirExists(GetFullPath(ti)))
        break;
    return true;
    }
    return false;
}

void FileExplorer::OnUpdateTreeItems(wxCommandEvent &/*e*/)
{
    if(m_kill)
        return;
    m_updater->Wait();
    wxTreeItemId ti=m_updated_node;
//    cbMessageBox(_T("Update Returned"));
    if(m_updater_cancel || !ti.IsOk())
    { //NODE WAS DELETED - REFRESH NOW!
        //TODO: Should only need to clean up and restart the timer (no need to change queue)
        delete m_updater;
        m_updater=NULL;
        m_update_active=false;
        ResetDirMonitor();
        if(ValidateRoot())
        {
            m_update_queue->Add(m_Tree->GetRootItem());
            m_updatetimer->Start(10,true);
        }
        return;
    }
//    cbMessageBox(_T("Node OK"));
//    m_Tree->DeleteChildren(ti);
    FileDataVec &removers=m_updater->m_removers;
    FileDataVec &adders=m_updater->m_adders;
    if(removers.size()>0||adders.size()>0)
    {
        m_Tree->Freeze();
        //LOOP THROUGH THE REMOVERS LIST AND REMOVE THOSE ITEMS FROM THE TREE
    //    cbMessageBox(_T("Removers"));
        for(FileDataVec::iterator it=removers.begin();it!=removers.end();it++)
        {
    //        cbMessageBox(it->name);
            wxTreeItemIdValue cookie;
            wxTreeItemId ch=m_Tree->GetFirstChild(ti,cookie);
            while(ch.IsOk())
            {
                if(it->name==m_Tree->GetItemText(ch))
                {
                    m_Tree->Delete(ch);
                    break;
                }
                ch=m_Tree->GetNextChild(ti,cookie);
            }
        }
        //LOOP THROUGH THE ADDERS LIST AND ADD THOSE ITEMS TO THE TREE
    //    cbMessageBox(_T("Adders"));
        for(FileDataVec::iterator it=adders.begin();it!=adders.end();it++)
        {
    //        cbMessageBox(it->name);
            wxTreeItemId newitem=m_Tree->AppendItem(ti,it->name,it->state);
            m_Tree->SetItemHasChildren(newitem,it->state==fvsFolder);
        }
        m_Tree->SortChildren(ti);
        m_Tree->Thaw();
    }
    if(!m_Tree->IsExpanded(ti))
    {
        m_update_expand=true;
        m_Tree->Expand(ti);
    }
    //RESTART THE TIMER
//    else
//    {
//        //TODO: Should not need to do anything here (see OnDirMonitor)
//        m_updating_node=GetNextExpandedNode(m_updating_node);
//        if(m_updating_node!=m_Tree->GetRootItem())
//            m_updatetimer->Start(10,true);
//        else
//            ResetDirMonitor();
////        LogMessage(_("Done restarting updater or monitor"));
//
////        else //TODO: Replace this with a directory monitor
////            m_updatetimer->Start(3000,true);
//    }
    delete m_updater;
    m_updater=NULL;
    m_update_active=false;
    m_updatetimer->Start(10,true);
    // Restart the monitor (TODO: move this elsewhere??)
    ResetDirMonitor();
}

void FileExplorer::SetImages()
{
    wxImageList *m_pImages = cbProjectTreeImages::MakeImageList();
    m_Tree->SetImageList(m_pImages);

//    // make sure tree is not "frozen"
//    UnfreezeTree(true);
}

wxString FileExplorer::GetFullPath(const wxTreeItemId &ti)
{
    if(!ti.IsOk())
        return wxEmptyString;
    wxFileName path(m_root);
    if(ti!=m_Tree->GetRootItem())
    {
        std::vector<wxTreeItemId> vti;
        vti.push_back(ti);
        wxTreeItemId pti=m_Tree->GetItemParent(vti[0]);
        if(!pti.IsOk())
            return wxEmptyString;
        while(pti!=m_Tree->GetRootItem())
        {
            vti.insert(vti.begin(),pti);
            pti=m_Tree->GetItemParent(pti);
        }
        for(size_t i=0;i<vti.size();i++)
            path.Assign(path.GetFullPath(),m_Tree->GetItemText(vti[i]));
    }
    return path.GetFullPath();
}

void FileExplorer::OnExpand(wxTreeEvent &event)
{
    if(m_updated_node==event.GetItem() && m_update_expand)
    {
        m_update_expand=false;
        return;
    }
    m_update_queue->Add(event.GetItem());
    m_updatetimer->Start(10,true);
    event.Veto();
    //AddTreeItems(event.GetItem());
}

void FileExplorer::ReadConfig()
{
    //IMPORT SETTINGS FROM LEGACY SHELLEXTENSIONS PLUGIN - TODO: REMOVE IN NEXT VERSION
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("ShellExtensions"));
    if(!cfg->Exists(_("FileExplorer/ShowHidenFiles")))
        cfg = Manager::Get()->GetConfigManager(_T("FileManager"));
    int len=0;
    cfg->Read(_T("FileExplorer/FavRootList/Len"), &len);
    for(int i=0;i<len;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/FavRootList/I%i"),i);
        wxString loc;
        FavoriteDir fav;
        cfg->Read(ref+_T("/alias"), &fav.alias);
        cfg->Read(ref+_T("/path"), &fav.path);
        m_Loc->Append(fav.alias);
        m_favdirs.Add(fav);
    }
    len=0;
    cfg->Read(_T("FileExplorer/RootList/Len"), &len);
    for(int i=0;i<len;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/RootList/I%i"),i);
        wxString loc;
        cfg->Read(ref, &loc);
        m_Loc->Append(loc);
    }
    len=0;
    cfg->Read(_T("FileExplorer/WildMask/Len"), &len);
    for(int i=0;i<len;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/WildMask/I%i"),i);
        wxString wild;
        cfg->Read(ref, &wild);
        m_WildCards->Append(wild);
    }
    cfg->Read(_T("FileExplorer/ParseCVS"), &m_parse_cvs);
    cfg->Read(_T("FileExplorer/ParseSVN"), &m_parse_svn);
    cfg->Read(_T("FileExplorer/ParseHG"), &m_parse_hg);
    cfg->Read(_T("FileExplorer/ParseBZR"), &m_parse_bzr);
    cfg->Read(_T("FileExplorer/ParseGIT"), &m_parse_git);
    cfg->Read(_T("FileExplorer/ShowHiddenFiles"), &m_show_hidden);
}

void FileExplorer::WriteConfig()
{
    //DISCARD SETTINGS FROM LEGACY SHELLEXTENSIONS PLUGIN - TODO: REMOVE IN NEXT VERSION
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("ShellExtensions"));
    if(cfg->Exists(_("FileExplorer/ShowHidenFiles")))
        cfg->DeleteSubPath(_("FileExplorer"));
    cfg = Manager::Get()->GetConfigManager(_T("FileManager"));
    //cfg->Clear();
    int count=static_cast<int>(m_favdirs.GetCount());
    cfg->Write(_T("FileExplorer/FavRootList/Len"), count);
    for(int i=0;i<count;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/FavRootList/I%i"),i);
        cfg->Write(ref+_T("/alias"), m_favdirs[i].alias);
        cfg->Write(ref+_T("/path"), m_favdirs[i].path);
    }
    count=static_cast<int>(m_Loc->GetCount())-static_cast<int>(m_favdirs.GetCount());
    cfg->Write(_T("FileExplorer/RootList/Len"), count);
    for(int i=0;i<count;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/RootList/I%i"),i);
        cfg->Write(ref, m_Loc->GetString(m_favdirs.GetCount()+i));
    }
    count=static_cast<int>(m_Loc->GetCount());
    cfg->Write(_T("FileExplorer/WildMask/Len"), count);
    for(int i=0;i<count;i++)
    {
        wxString ref=wxString::Format(_T("FileExplorer/WildMask/I%i"),i);
        cfg->Write(ref, m_WildCards->GetString(i));
    }
    cfg->Write(_T("FileExplorer/ParseCVS"), m_parse_cvs);
    cfg->Write(_T("FileExplorer/ParseSVN"), m_parse_svn);
    cfg->Write(_T("FileExplorer/ParseHG"), m_parse_hg);
    cfg->Write(_T("FileExplorer/ParseBZR"), m_parse_bzr);
    cfg->Write(_T("FileExplorer/ParseGIT"), m_parse_git);
    cfg->Write(_T("FileExplorer/ShowHiddenFiles"), m_show_hidden);
}

void FileExplorer::OnEnterWild(wxCommandEvent &/*event*/)
{
    wxString wild=m_WildCards->GetValue();
    for(size_t i=0;i<m_WildCards->GetCount();i++)
    {
        wxString cmp;
        cmp=m_WildCards->GetString(i);
        if(cmp==wild)
        {
            m_WildCards->Delete(i);
            m_WildCards->Insert(wild,0);
            m_WildCards->SetSelection(0);
            RefreshExpanded(m_Tree->GetRootItem());
            return;
        }
    }
    m_WildCards->Insert(wild,0);
    if(m_WildCards->GetCount()>10)
        m_WildCards->Delete(10);
    m_WildCards->SetSelection(0);
    RefreshExpanded(m_Tree->GetRootItem());
}

void FileExplorer::OnChooseWild(wxCommandEvent &/*event*/)
{
    // Beware on win32 that if user opens drop down, then types a wildcard the combo box
    // event will contain a -1 selection and an empty string item. Harmless in current code.
    wxString wild=m_WildCards->GetValue();
    m_WildCards->Delete(m_WildCards->GetSelection());
    m_WildCards->Insert(wild,0);
//    event.Skip(true);
//    cbMessageBox(wild);
    m_WildCards->SetSelection(0);
    RefreshExpanded(m_Tree->GetRootItem());
}

void FileExplorer::OnEnterLoc(wxCommandEvent &/*event*/)
{
    wxString loc=m_Loc->GetValue();
    if(!SetRootFolder(loc))
        return;
    for(size_t i=0;i<m_Loc->GetCount();i++)
    {
        wxString cmp;
        if(i<m_favdirs.GetCount())
            cmp=m_favdirs[i].path;
        else
            cmp=m_Loc->GetString(i);
        if(cmp==m_root)
        {
            if(i>=m_favdirs.GetCount())
            {
                m_Loc->Delete(i);
                m_Loc->Insert(m_root,m_favdirs.GetCount());
            }
            m_Loc->SetSelection(m_favdirs.GetCount());
            return;
        }
    }
    m_Loc->Insert(m_root,m_favdirs.GetCount());
    if(m_Loc->GetCount()>10+m_favdirs.GetCount())
        m_Loc->Delete(10+m_favdirs.GetCount());
    m_Loc->SetSelection(m_favdirs.GetCount());
}

void FileExplorer::OnChooseLoc(wxCommandEvent &event)
{
    wxString loc;
    // on WIN32 if the user opens the drop down, but then types a path instead, this event
    // fires with an empty string, so we have no choice but to return null. This event
    // doesn't happen on Linux (the drop down closes when the user starts typing)
    if(event.GetInt()<0)
        return;
    if(event.GetInt()>=static_cast<int>(m_favdirs.GetCount()))
        loc=m_Loc->GetValue();
    else
        loc=m_favdirs[event.GetInt()].path;
    if(!SetRootFolder(loc))
        return;
    if(event.GetInt()>=static_cast<int>(m_favdirs.GetCount()))
    {
        m_Loc->Delete(event.GetInt());
        m_Loc->Insert(m_root,m_favdirs.GetCount());
        m_Loc->SetSelection(m_favdirs.GetCount());
    }
    else
    {
        for(size_t i=m_favdirs.GetCount();i<m_Loc->GetCount();i++)
        {
            wxString cmp;
            cmp=m_Loc->GetString(i);
            if(cmp==m_root)
            {
                m_Loc->Delete(i);
                m_Loc->Insert(m_root,m_favdirs.GetCount());
                m_Loc->SetSelection(event.GetInt());
                return;
            }
        }
        m_Loc->Insert(m_root,m_favdirs.GetCount());
        if(m_Loc->GetCount()>10+m_favdirs.GetCount())
            m_Loc->Delete(10+m_favdirs.GetCount());
        m_Loc->SetSelection(event.GetInt());
    }
}

void FileExplorer::OnSetLoc(wxCommandEvent &/*event*/)
{
    wxString loc=GetFullPath(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
    if(!SetRootFolder(loc))
        return;
    m_Loc->Insert(m_root,m_favdirs.GetCount());
    if(m_Loc->GetCount()>10+m_favdirs.GetCount())
        m_Loc->Delete(10+m_favdirs.GetCount());
}

void FileExplorer::OnOpenInEditor(wxCommandEvent &/*event*/)
{
    for(int i=0;i<m_ticount;i++)
    {
        wxFileName path(GetFullPath(m_selectti[i]));  //SINGLE: m_Tree->GetSelection()
        wxString filename=path.GetFullPath();
        if(!path.FileExists())
            continue;
        EditorManager* em = Manager::Get()->GetEditorManager();
        EditorBase* eb = em->IsOpen(filename);
        if (eb)
        {
            // open files just get activated
            eb->Activate();
            return;
        } else
        em->Open(filename);
    }
}

void FileExplorer::OnActivate(wxTreeEvent &event)
{
    wxString filename=GetFullPath(event.GetItem());
    if(m_Tree->GetItemImage(event.GetItem())==fvsFolder)
    {
        event.Skip(true);
        return;
    }
    EditorManager* em = Manager::Get()->GetEditorManager();
    EditorBase* eb = em->IsOpen(filename);
    if (eb)
    {
        // open files just get activated
        eb->Activate();
        return;
    }

    // Use Mime handler to open file
    cbMimePlugin* plugin = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(filename);
    if (!plugin)
    {
        wxString msg;
        msg.Printf(_("Could not open file '%s'.\nNo handler registered for this type of file."), filename.c_str());
        LogErrorMessage(msg);
//        em->Open(filename); //should never need to open the file from here
    }
    else if (plugin->OpenFile(filename) != 0)
    {
        const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plugin);
        wxString msg;
        msg.Printf(_("Could not open file '%s'.\nThe registered handler (%s) could not open it."), filename.c_str(), info ? info->title.c_str() : wxString(_("<Unknown plugin>")).c_str());
        LogErrorMessage(msg);
    }

//    if(!em->IsOpen(file))
//        em->Open(file);

}


void FileExplorer::OnKeyDown(wxKeyEvent &event)
{
    if(event.GetKeyCode() == WXK_DELETE)
    {
        wxCommandEvent event2;
        OnDelete(event2);
    }
}


void FileExplorer::OnRightClick(wxTreeEvent &event)
{
    wxMenu* Popup = new wxMenu();
    m_ticount=m_Tree->GetSelections(m_selectti);
    if(!IsInSelection(event.GetItem())) //replace the selection with right clicked item if right clicked item isn't in the selection
    {
        for(int i=0;i<m_ticount;i++)
            m_Tree->SelectItem(m_selectti[i],false);
        m_Tree->SelectItem(event.GetItem());
        m_ticount=m_Tree->GetSelections(m_selectti);
        m_Tree->Update();
    }
    FileTreeData* ftd = new FileTreeData(0, FileTreeData::ftdkUndefined);
    ftd->SetKind(FileTreeData::ftdkFile);
    if(m_ticount>0)
    {
        if(m_ticount==1)
        {
            int img = m_Tree->GetItemImage(m_selectti[0]);
            if(img==fvsFolder)
            {
                ftd->SetKind(FileTreeData::ftdkFolder);
                Popup->Append(ID_SETLOC,_("Make roo&t"));
                Popup->Append(ID_FILEEXPANDALL,_("Expand All Children")); //TODO: check availability in wx2.8 for win32 (not avail wx2.6)
                Popup->Append(ID_FILECOLLAPSEALL,_("Collapse All Children")); //TODO: check availability in wx2.8 for win32 (not avail wx2.6)
                Popup->Append(ID_FILEMAKEFAV,_("Add to Favorites"));
                Popup->Append(ID_FILENEWFILE,_("New File..."));
                Popup->Append(ID_FILENEWFOLDER,_("New Directory..."));
                Popup->Append(ID_FILERENAME,_("&Rename..."));
            } else
            {
                Popup->Append(ID_FILERENAME,_("&Rename..."));
            }
        }
        if(IsFilesOnly(m_selectti))
        {
            Popup->Append(ID_OPENINED,_("&Open in CB Editor"));
            if(Manager::Get()->GetProjectManager()->GetActiveProject())
                Popup->Append(ID_FILEADDTOPROJECT,_("&Add to Active Project..."));
        }
        Popup->Append(ID_FILEDUP,_("&Duplicate"));
        Popup->Append(ID_FILECOPY,_("&Copy To..."));
        Popup->Append(ID_FILEMOVE,_("&Move To..."));
        Popup->Append(ID_FILEDELETE,_("D&elete"));
    }
    wxMenu *viewpop=new wxMenu();
    viewpop->Append(ID_FILESETTINGS,_("Favorite Directories..."));
    viewpop->AppendCheckItem(ID_FILESHOWHIDDEN,_("Show &Hidden Files"))->Check(m_show_hidden);
    viewpop->AppendCheckItem(ID_FILEPARSECVS,_("CVS Decorators"))->Check(m_parse_cvs);
    viewpop->AppendCheckItem(ID_FILEPARSESVN,_("SVN Decorators"))->Check(m_parse_svn);
    viewpop->AppendCheckItem(ID_FILEPARSEHG,_("Hg Decorators"))->Check(m_parse_hg);
    viewpop->AppendCheckItem(ID_FILEPARSEBZR,_("Bzr Decorators"))->Check(m_parse_bzr);
    viewpop->AppendCheckItem(ID_FILEPARSEGIT,_("Git Decorators"))->Check(m_parse_git);
    Popup->AppendSubMenu(viewpop,_("&View"));
    Popup->Append(ID_FILEREFRESH,_("Re&fresh"));
    if(m_ticount>1)
    {
        ftd->SetKind(FileTreeData::ftdkVirtualGroup);
        wxString pathlist = GetFullPath(m_selectti[0]);
        for(int i=1;i<m_ticount;i++)
            pathlist += _T("*") + GetFullPath(m_selectti[i]); //passing a '*' separated list of files/directories to any plugin takers
        ftd->SetFolder(pathlist);
    }
    else if ( m_ticount > 0)
    {
        wxString filepath = GetFullPath(m_selectti[0]);
        ftd->SetFolder(filepath);
    }
    if(m_ticount>0)
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtUnknown, Popup, ftd);
    delete ftd;
    wxWindow::PopupMenu(Popup);
    delete Popup;
}

void FileExplorer::OnNewFile(wxCommandEvent &/*event*/)
{
    wxString workingdir=GetFullPath(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
    wxTextEntryDialog te(this,_("Name Your New File: "));
    if(te.ShowModal()!=wxID_OK)
        return;
    wxString name=te.GetValue();
    wxFileName file(workingdir);
    file.Assign(file.GetFullPath(),name);
    wxString newfile=file.GetFullPath();
    if(!wxFileName::FileExists(newfile) &&!wxFileName::DirExists(newfile))
    {
        wxFile fileobj;
        if(fileobj.Create(newfile))
        {
            fileobj.Close();
            Refresh(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
        }
        else
            cbMessageBox(_("File Creation Failed"),_("Error"));
    }
    else
        cbMessageBox(_("File/Directory Already Exists with Name ")+name, _("Error"));
}

void FileExplorer::OnAddFavorite(wxCommandEvent &/*event*/)
{
    FavoriteDir fav;
    fav.path=GetFullPath(m_selectti[0]);
    if(fav.path[fav.path.Len()-1]!=wxFileName::GetPathSeparator())
        fav.path=fav.path+wxFileName::GetPathSeparator();
    wxTextEntryDialog ted(NULL,_("Enter an alias for this directory:"),_("Add Favorite Directory"),fav.path);
    if(ted.ShowModal()!=wxID_OK)
        return;
    wxString name=ted.GetValue();
    fav.alias=name;
    m_favdirs.Insert(fav,0);
    m_Loc->Insert(name,0);
}

void FileExplorer::OnNewFolder(wxCommandEvent &/*event*/)
{
    wxString workingdir=GetFullPath(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
    wxTextEntryDialog te(this,_("New Directory Name: "));
    if(te.ShowModal()!=wxID_OK)
        return;
    wxString name=te.GetValue();
    wxFileName dir(workingdir);
    dir.Assign(dir.GetFullPath(),name);
    wxString mkd=dir.GetFullPath();
    if(!wxFileName::DirExists(mkd) &&!wxFileName::DirExists(mkd))
    {
        dir.Mkdir(mkd);
        Refresh(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
    }
    else
        cbMessageBox(_("File/Directory Already Exists with Name ")+name);
}

void FileExplorer::OnDuplicate(wxCommandEvent &/*event*/)
{
    m_ticount=m_Tree->GetSelections(m_selectti);
    for(int i=0;i<m_ticount;i++)
    {
        wxFileName path(GetFullPath(m_selectti[i]));  //SINGLE: m_Tree->GetSelection()
        if(wxFileName::FileExists(path.GetFullPath())||wxFileName::DirExists(path.GetFullPath()))
        {
            if(!PromptSaveOpenFile(_("File is modified, press Yes to save before duplication, No to copy unsaved file or Cancel to skip file"),wxFileName(path)))
                continue;
            int j=1;
            wxString destpath(path.GetPathWithSep()+path.GetName()+wxString::Format(_T("(%i)"),j));
            if(path.GetExt()!=wxEmptyString)
                destpath+=_T(".")+path.GetExt();
            while(j<100 && (wxFileName::FileExists(destpath) || wxFileName::DirExists(destpath)))
            {
                j++;
                destpath=path.GetPathWithSep()+path.GetName()+wxString::Format(_T("(%i)"),j);
                if(path.GetExt()!=wxEmptyString)
                    destpath+=_T(".")+path.GetExt();
            }
            if(j==100)
            {
                cbMessageBox(_("Too many copies of file or directory"));
                continue;
            }

#ifdef __WXMSW__
            wxArrayString output;
            wxString cmdline;
            if(wxFileName::FileExists(path.GetFullPath()))
                cmdline=_T("cmd /c copy /Y \"")+path.GetFullPath()+_T("\" \"")+destpath+_T("\"");
            else
                cmdline=_T("cmd /c xcopy /S/E/Y/H/I \"")+path.GetFullPath()+_T("\" \"")+destpath+_T("\"");
            int hresult=::wxExecute(cmdline,output,wxEXEC_SYNC);
#else
            wxString cmdline=_T("/bin/cp -r -b \"")+path.GetFullPath()+_T("\" \"")+destpath+_T("\"");
            int hresult=::wxExecute(cmdline,wxEXEC_SYNC);
#endif
            if(hresult)
                MessageBox(m_Tree,_("Command '")+cmdline+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
        }
    }
    Refresh(m_Tree->GetRootItem()); //TODO: Can probably be more efficient than this
    //TODO: Reselect item in new location?? (what it outside root scope?)
}


void FileExplorer::CopyFiles(const wxString &destination, const wxArrayString &selectedfiles)
{
    for(unsigned int i=0;i<selectedfiles.Count();i++)
    {
        wxString path=selectedfiles[i];
        wxFileName destpath;
        destpath.Assign(destination,wxFileName(path).GetFullName());
        if(destpath.SameAs(path))
            continue;
        if(wxFileName::FileExists(path)||wxFileName::DirExists(path))
        {
            if(!PromptSaveOpenFile(_("File is modified, press Yes to save before duplication, No to copy unsaved file or Cancel to skip file"),wxFileName(path)))
                continue;
#ifdef __WXMSW__
            wxArrayString output;
            wxString cmdline;
            if(wxFileName::FileExists(path))
                cmdline=_T("cmd /c copy /Y \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\"");
            else
                cmdline=_T("cmd /c xcopy /S/E/Y/H/I \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\"");
            int hresult=::wxExecute(cmdline,output,wxEXEC_SYNC);
#else
            int hresult=::wxExecute(_T("/bin/cp -r -b \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),wxEXEC_SYNC);
#endif
            if(hresult)
                MessageBox(m_Tree,_("Copying '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
        }
    }
}

void FileExplorer::OnCopy(wxCommandEvent &/*event*/)
{
    wxDirDialog dd(this,_("Copy to"));
    dd.SetPath(GetFullPath(m_Tree->GetRootItem()));
    wxArrayString selectedfiles;
    m_ticount=m_Tree->GetSelections(m_selectti);
    for(int i=0;i<m_ticount;i++) // really important not to rely on TreeItemId ater modal dialogs because file updates can change the file tree in the background.
    {
        selectedfiles.Add(GetFullPath(m_selectti[i]));  //SINGLE: m_Tree->GetSelection()
    }
    if(dd.ShowModal()==wxID_CANCEL)
        return;
    CopyFiles(dd.GetPath(),selectedfiles);
//    Refresh(m_Tree->GetRootItem()); //TODO: Use this if monitoring not available
    //TODO: Reselect item in new location?? (what if outside root scope?)
}

void FileExplorer::MoveFiles(const wxString &destination, const wxArrayString &selectedfiles)
{
    for(unsigned int i=0;i<selectedfiles.Count();i++)
    {
        wxString path=selectedfiles[i];
        wxFileName destpath;
        destpath.Assign(destination,wxFileName(path).GetFullName());
        if(destpath.SameAs(path)) //TODO: Log message that can't copy over self.
            continue;
        if(wxFileName::FileExists(path)||wxFileName::DirExists(path))
        {
#ifdef __WXMSW__
            wxArrayString output;
            int hresult=::wxExecute(_T("cmd /c move /Y \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),output,wxEXEC_SYNC);
#else
            int hresult=::wxExecute(_T("/bin/mv -b \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),wxEXEC_SYNC);
#endif
            if(hresult)
                MessageBox(m_Tree,_("Moving '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
        }
    }
}

void FileExplorer::OnMove(wxCommandEvent &/*event*/)
{
    wxDirDialog dd(this,_("Move to"));
    wxArrayString selectedfiles;
    m_ticount=m_Tree->GetSelections(m_selectti);
    for(int i=0;i<m_ticount;i++)
        selectedfiles.Add(GetFullPath(m_selectti[i]));  //SINGLE: m_Tree->GetSelection()
    dd.SetPath(GetFullPath(m_Tree->GetRootItem()));
    if(dd.ShowModal()==wxID_CANCEL)
        return;
    MoveFiles(dd.GetPath(),selectedfiles);
//    Refresh(m_Tree->GetRootItem()); //TODO: Can probably be more efficient than this
    //TODO: Reselect item in new location?? (what if outside root scope?)
}

wxArrayString FileExplorer::GetSelectedPaths()
{
    wxArrayString paths;
    for(int i=0;i<m_ticount;i++)
    {
        wxString path(GetFullPath(m_selectti[i]));  //SINGLE: m_Tree->GetSelection()
        paths.Add(path);
    }
    return paths;
}

void FileExplorer::OnDelete(wxCommandEvent &/*event*/)
{
    m_ticount=m_Tree->GetSelections(m_selectti);
    wxArrayString as=GetSelectedPaths();
    wxString prompt=_("Your are about to delete\n\n");
    for(unsigned int i=0;i<as.Count();i++)
        prompt+=as[i]+_("\n");
    prompt+=_("\nAre you sure?");
    if(MessageBox(m_Tree,prompt,_("Delete"),wxYES_NO)!=wxID_YES)
        return;
    for(unsigned int i=0;i<as.Count();i++)
    {
        wxString path=as[i];  //SINGLE: m_Tree->GetSelection()
        if(wxFileName::FileExists(path))
        {
            //        EditorManager* em = Manager::Get()->GetEditorManager();
            //        if(em->IsOpen(path))
            //        {
            //            cbMessageBox(_("Close file ")+path.GetFullPath()+_(" first"));
            //            return;
            //        }
            if(!::wxRemoveFile(path))
                MessageBox(m_Tree,_("Delete file '")+path+_("' failed"));
        } else
        if(wxFileName::DirExists(path))
        {
#ifdef __WXMSW__
            wxArrayString output;
            int hresult=::wxExecute(_T("cmd /c rmdir /S/Q \"")+path+_T("\""),output,wxEXEC_SYNC);
#else
            int hresult=::wxExecute(_T("/bin/rm -r -f \"")+path+_T("\""),wxEXEC_SYNC);
#endif
            if(hresult)
                MessageBox(m_Tree,_("Delete directory '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
        }
    }
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnRename(wxCommandEvent &/*event*/)
{
    wxString path(GetFullPath(m_selectti[0]));  //SINGLE: m_Tree->GetSelection()
    if(wxFileName::FileExists(path))
    {
        EditorManager* em = Manager::Get()->GetEditorManager();
        if(em->IsOpen(path))
        {
            cbMessageBox(_("Close file first"));
            return;
        }
        wxTextEntryDialog te(this,_("New name:"),_("Rename File"),wxFileName(path).GetFullName());
        if(te.ShowModal()==wxID_CANCEL)
            return;
        wxFileName destpath(path);
        destpath.SetFullName(te.GetValue());
        if(!::wxRenameFile(path,destpath.GetFullPath()))
            cbMessageBox(_("Rename failed"));
    }
    if(wxFileName::DirExists(path))
    {
        wxTextEntryDialog te(this,_("New name:"),_("Rename File"),wxFileName(path).GetFullName());
        if(te.ShowModal()==wxID_CANCEL)
            return;
        wxFileName destpath(path);
        destpath.SetFullName(te.GetValue());
#ifdef __WXMSW__
        wxArrayString output;
        int hresult=::wxExecute(_T("cmd /c move /Y \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),output,wxEXEC_SYNC);
#else
        int hresult=::wxExecute(_T("/bin/mv \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),wxEXEC_SYNC);
#endif
        if(hresult)
            MessageBox(m_Tree,_("Rename directory '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
    }
    Refresh(m_Tree->GetItemParent(m_selectti[0])); //SINGLE: m_Tree->GetSelection()
}

void FileExplorer::OnExpandAll(wxCommandEvent &/*event*/)
{
    m_Tree->ExpandAllChildren(m_Tree->GetSelection());
}

void FileExplorer::OnCollapseAll(wxCommandEvent &/*event*/)
{
    m_Tree->CollapseAllChildren(m_Tree->GetSelection());
}

void FileExplorer::OnSettings(wxCommandEvent &/*event*/)
{
    FileBrowserSettings fbs(m_favdirs,NULL);
    if(fbs.ShowModal()==wxID_OK)
    {
        size_t count=m_favdirs.GetCount();
        for(size_t i=0;i<count;i++)
            m_Loc->Delete(0);
        m_favdirs=fbs.m_favdirs;
        count=m_favdirs.GetCount();
        for(size_t i=0;i<count;i++)
            m_Loc->Insert(m_favdirs[i].alias,i);
    }

}

void FileExplorer::OnShowHidden(wxCommandEvent &/*event*/)
{
    m_show_hidden=!m_show_hidden;
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnParseCVS(wxCommandEvent &/*event*/)
{
    m_parse_cvs=!m_parse_cvs;
    //cfg->Clear();
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnParseSVN(wxCommandEvent &/*event*/)
{
    m_parse_svn=!m_parse_svn;
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnParseGIT(wxCommandEvent &/*event*/)
{
    m_parse_git=!m_parse_git;
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnParseHG(wxCommandEvent &/*event*/)
{
    m_parse_hg=!m_parse_hg;
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnParseBZR(wxCommandEvent &/*event*/)
{
    m_parse_bzr=!m_parse_bzr;
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnUpButton(wxCommandEvent &/*event*/)
{
    wxFileName loc(m_root);
    loc.RemoveLastDir();
    SetRootFolder(loc.GetFullPath()); //TODO: Check if this is always the root folder
}

void FileExplorer::OnRefresh(wxCommandEvent &/*event*/)
{
    if(m_Tree->GetItemImage(m_selectti[0])==fvsFolder) //SINGLE: m_Tree->GetSelection()
        Refresh(m_selectti[0]); //SINGLE: m_Tree->GetSelection()
    else
        Refresh(m_Tree->GetRootItem());
}

//TODO: Set copy cursor state if necessary
void FileExplorer::OnBeginDragTreeItem(wxTreeEvent &event)
{
//    SetCursor(wxCROSS_CURSOR);
//    if(IsInSelection(event.GetItem()))
//        return; // don't start a drag for an unselected item
    if(m_Tree->GetItemImage(event.GetItem())==fvsNormal||m_Tree->GetItemImage(event.GetItem())==fvsFolder)
        event.Allow();
//    m_dragtest=GetFullPath(event.GetItem());
    m_ticount=m_Tree->GetSelections(m_selectti);
}

bool FileExplorer::IsInSelection(const wxTreeItemId &ti)
{
    for(int i=0;i<m_ticount;i++)
        if(ti==m_selectti[i])
            return true;
    return false;
}

//TODO: End copy cursor state if necessary
void FileExplorer::OnEndDragTreeItem(wxTreeEvent &event)
{
//    SetCursor(wxCursor(wxCROSS_CURSOR));
    if(m_Tree->GetItemImage(event.GetItem())!=fvsFolder) //can only copy to folders
        return;
    for(int i=0;i<m_ticount;i++)
    {
        wxString path(GetFullPath(m_selectti[i]));
        wxFileName destpath;
        if(!event.GetItem().IsOk())
            return;
        destpath.Assign(GetFullPath(event.GetItem()),wxFileName(path).GetFullName());
        if(destpath.SameAs(path))
            continue;
        if(wxFileName::DirExists(path)||wxFileName::FileExists(path))
        {
            if(!::wxGetKeyState(WXK_CONTROL))
            {
                if(wxFileName::FileExists(path))
                    if(!PromptSaveOpenFile(_("File is modified, press Yes to save before move, No to move unsaved file or Cancel to skip file"),wxFileName(path)))
                        continue;
#ifdef __WXMSW__
                wxArrayString output;
                int hresult=::wxExecute(_T("cmd /c move /Y \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),output,wxEXEC_SYNC);
#else
                int hresult=::wxExecute(_T("/bin/mv -b \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),wxEXEC_SYNC);
#endif
                if(hresult)
                    MessageBox(m_Tree,_("Move directory '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
            } else
            {
                if(wxFileName::FileExists(path))
                    if(!PromptSaveOpenFile(_("File is modified, press Yes to save before copy, No to copy unsaved file or Cancel to skip file"),wxFileName(path)))
                        continue;
#ifdef __WXMSW__
                wxArrayString output;
                wxString cmdline;
                if(wxFileName::FileExists(path))
                    cmdline=_T("cmd /c copy /Y \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\"");
                else
                    cmdline=_T("cmd /c xcopy /S/E/Y/H/I \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\"");
                int hresult=::wxExecute(cmdline,output,wxEXEC_SYNC);
#else
                int hresult=::wxExecute(_T("/bin/cp -r -b \"")+path+_T("\" \"")+destpath.GetFullPath()+_T("\""),wxEXEC_SYNC);
#endif
                if(hresult)
                    MessageBox(m_Tree,_("Copy directory '")+path+_("' failed with error ")+wxString::Format(_T("%i"),hresult));
            }
        }
//        if(!PromptSaveOpenFile(_T("File is modified, press \"Yes\" to save before move/copy, \"No\" to move/copy unsaved file or \"Cancel\" to abort the operation"),path)) //TODO: specify move or copy depending on whether CTRL held down
//            return;
    }
    Refresh(m_Tree->GetRootItem());
}

void FileExplorer::OnAddToProject(wxCommandEvent &/*event*/)
{
    wxArrayString files;
    wxString file;
    for(int i=0;i<m_ticount;i++)
    {
        file=GetFullPath(m_selectti[i]);
        if(wxFileName::FileExists(file))
            files.Add(file);
    }
    wxArrayInt prompt;
    Manager::Get()->GetProjectManager()->AddMultipleFilesToProject(files, NULL, prompt);
    Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
}

bool FileExplorer::IsFilesOnly(wxArrayTreeItemIds tis)
{
    for(size_t i=0;i<tis.GetCount();i++)
        if(m_Tree->GetItemImage(tis[i])==fvsFolder)
            return false;
    return true;
}

void FileExplorer::OnExecRequest(wxCommandEvent &/*event*/)
{
    m_updater->ExecMain();
}
