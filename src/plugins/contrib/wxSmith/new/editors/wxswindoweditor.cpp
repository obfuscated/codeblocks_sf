#include "wxswindoweditor.h"
#include "wxsdrawingwindow.h"
#include "wxswindowresdataobject.h"
#include "wxswindoweditorcontent.h"
#include "../wxsitem.h"
#include "../wxsparent.h"
#include "../wxsproject.h"
#include "../wxsresourcetree.h"
#include "../wxsitemfactory.h"
#include "../wxsbaseproperties.h"

#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <manager.h>
#include <configmanager.h>

static const long wxsInsIntoId    = wxNewId();
static const long wxsInsBeforeId  = wxNewId();
static const long wxsInsAfterId   = wxNewId();
static const long wxsDelId        = wxNewId();
static const long wxsPreviewId    = wxNewId();
static const long wxsQuickPropsId = wxNewId();

wxsWindowEditor::wxsWindowEditor(wxWindow* parent,wxsResource* Resource):
    wxsEditor(parent,wxEmptyString,Resource),
    TopPreview(NULL),
    QuickPropsOpen(false),
    ResourceLockCnt(0)
{
    wxASSERT(Resource!=NULL);

    InitializeImages();

    VertSizer = new wxBoxSizer(wxVERTICAL);
    WidgetsSet = new wxNotebook(this,-1);
    BuildPalette(WidgetsSet);
    HorizSizer = new wxBoxSizer(wxHORIZONTAL);
    VertSizer->Add(HorizSizer,1,wxEXPAND);
    VertSizer->Add(WidgetsSet,0,wxEXPAND);

    Content = new wxsWindowEditorContent(this);
    HorizSizer->Add(Content,1,wxEXPAND);

    QPArea = new wxScrolledWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxVSCROLL|wxSUNKEN_BORDER|wxALWAYS_SHOW_SB);
    QPArea->SetScrollbars(0,5,0,0);
    HorizSizer->Add(QPArea,0,wxEXPAND);
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);

    OpsSizer = new wxBoxSizer(wxVERTICAL);
    HorizSizer->Add(OpsSizer,0,wxEXPAND);

    OpsSizer->Add(InsIntoBtn   = new wxBitmapButton(this,wxsInsIntoId,InsIntoImg));
    OpsSizer->Add(InsBeforeBtn = new wxBitmapButton(this,wxsInsBeforeId,InsBeforeImg));
    OpsSizer->Add(InsAfterBtn  = new wxBitmapButton(this,wxsInsAfterId,InsAfterImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(DelBtn       = new wxBitmapButton(this,wxsDelId,DelImg));
    OpsSizer->Add(PreviewBtn   = new wxBitmapButton(this,wxsPreviewId,PreviewImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(QuickPanelBtn = new wxBitmapButton(this,wxsQuickPropsId,QuickPropsImgOpen));
    InsIntoBtn   ->SetToolTip(_("Insert new widgets into current selection"));
    InsBeforeBtn ->SetToolTip(_("Insert new widgets before current selection"));
    InsAfterBtn  ->SetToolTip(_("Insert new widgets after current selection"));
    DelBtn       ->SetToolTip(_("Delete current selection"));
    PreviewBtn   ->SetToolTip(_("Show preview"));
    QuickPanelBtn->SetToolTip(_("Open / Close Quick Properties panel"));

    SetSizer(VertSizer);

    SetInsertionTypeMask(0);

    if ( GetWinRes()->GetBasePropsFilter() == wxsFLFile )
    {
        InitFilename(GetWinRes()->GetXrcFile());
        SetTitle(m_Shortname);
    }
    else
    {
        wxASSERT_MSG( GetProject() != NULL, _T("Only wxsFLFile mode may not have project associated") );
        wxString FileName = GetProject()->GetProjectFileName(GetWinRes()->GetWxsFile());
        InitFilename(FileName);
        SetTitle(m_Shortname);
    }

    UndoBuff = new wxsWinUndoBuffer(GetWinRes());
    Corrector = new wxsCorrector(GetWinRes());
    ToggleQuickPropsPanel(false);
    AllEditors.insert(this);
    BuildPreview();

    // Correcting data after loading it
    Corrector->GlobalCheck();

    // Changing selection to root item
    RootItem()->ClearSelection();
    GetWinRes()->SelectionChanged(NULL);
}

wxsWindowEditor::~wxsWindowEditor()
{
    // Destroying also Quick Props panel which usually triggers it's
    // Save() method when being destroyed
    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();

	KillPreview();

    // First we need to discard all changes,
    // this operation will recreate unmodified code
    // in source files
    if ( GetModified() )
    {
        // Loading resource from disk - this will recreate source code
        GetWinRes()->LoadResource();
        GetWinRes()->RebuildCode();
    }

    // Now doing the rest
	delete UndoBuff;
	delete Corrector;
	GetWinRes()->HidePreview();

	AllEditors.erase(this);
}

void wxsWindowEditor::ReloadImages()
{
    ImagesLoaded = false;
    InitializeImages();
    for ( WindowSet::iterator i=AllEditors.begin(); i!=AllEditors.end(); ++i )
    {
        (*i)->RebuildIcons();
    }
}

void wxsWindowEditor::BuildPreview()
{
    Content->SetSizer(NULL);
    Freeze();

    // Generating preview

    wxsItem* TopItem = GetWinRes()->GetRootItem();
    wxObject* TopPreviewObject = TopItem ? TopItem->BuildPreview(Content,false) : NULL;
    TopPreview = wxDynamicCast(TopPreviewObject,wxWindow);
    if ( !TopPreview )
    {
        delete TopPreviewObject;
    }
    else
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(TopPreview,0,wxALL,10);
        Content->SetVirtualSizeHints(1,1);
        Content->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(Content);
        NewSizer->FitInside(Content);
        HorizSizer->Layout();
        VertSizer->Layout();
    }

    Layout();
    Thaw();
    Content->NewPreview();
    // Preview bindings are no longer needed
    TopItem->InvalidatePreview();
}

void wxsWindowEditor::KillPreview()
{
    if ( TopPreview )
    {
        Content->SetSizer(NULL);
        delete TopPreview;
        TopPreview = NULL;
    }
}

bool wxsWindowEditor::Save()
{
    GetWinRes()->SaveResource();
    UndoBuff->Saved();
	return true;
}

bool wxsWindowEditor::GetModified()
{
	return GetWinRes()->GetModified();
}

void wxsWindowEditor::SetModified(bool modified)
{
    GetWinRes()->SetModified(modified);
    if ( GetWinRes()->GetModified() )
    {
        SetTitle(_T("*") + GetShortName());
    }
    else
    {
        SetTitle(GetShortName());
    }
}

bool wxsWindowEditor::CanUndo()
{
	return UndoBuff->CanUndo();
}

bool wxsWindowEditor::CanRedo()
{
	return UndoBuff->CanRedo();
}

void wxsWindowEditor::Undo()
{
    ResourceLock();
    UndoBuff->Undo();
    Corrector->ClearCache();
    // FIXME (SpOoN#1#): ResourceUnlock will create new undo entry, need to avoid that
    ResourceUnlock();
	SetModified(UndoBuff->IsModified());
	// TODO: Restore selection
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::Redo()
{
    ResourceLock();
    UndoBuff->Redo();
    Corrector->ClearCache();
    // FIXME (SpOoN#1#): ResourceUnlock will create new undo entry, need to avoid that
    ResourceUnlock();
	SetModified(UndoBuff->IsModified());
	// TODO: Restore selection
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

bool wxsWindowEditor::HasSelection()
{
    return HasSelection(RootItem());
}

bool wxsWindowEditor::HasSelection(wxsItem* Item)
{
    if ( Item->GetIsSelected() ) return true;
    wxsParent* Parent = Item->ToParent();
    if ( !Parent ) return false;
    for ( int i=0; i<Parent->GetChildCount(); ++i )
    {
        if ( HasSelection(Parent->GetChild(i)) ) return true;
    }
    return false;
}

bool wxsWindowEditor::CanPaste()
{
    if ( !wxTheClipboard->Open() ) return false;
    bool Res = wxTheClipboard->IsSupported(wxsDF_WIDGET);
    // FIXME (SpOoN#1#): Add support for text (XRC) data
    wxTheClipboard->Close();
    return Res;
}

void wxsWindowEditor::Cut()
{
    Copy();

    // Removing items copied into clipboard
    ResourceLock();
    KillSelection(RootItem());
    Corrector->ClearCache();
    ResourceUnlock();

    // TODO: Select previous item / parent item etc
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::KillSelection(wxsItem* Item)
{
    wxsParent* P = Item->ToParent();
    if ( P )
    {
        for ( int i = P->GetChildCount(); i-->0; )
        {
            wxsItem* Child = P->GetChild(i);
            if ( Child->GetIsSelected() )
            {
                 wxsTREE()->Delete(Child->GetLastTreeItemId());
                 P->UnbindChild(Child);
                 wxsKILL(Child);
            }
            else
            {
                KillSelection(Child);
            }
        }
    }
}

void wxsWindowEditor::Copy()
{
	// Almost all selected widgets will be added into clipboard
	// but with one exception - widget won't be added if parent of this
	// widget at any level is also selected

	ItemArray Items;
	GetSelectionNoChildren(Items,RootItem());

    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject* Data = new wxsWindowResDataObject;
    size_t Cnt = Items.Count();
    for ( size_t i=0; i<Cnt; i++ )
    {
    	Data->AddItem(Items[i]);
    }
    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();

}

void wxsWindowEditor::Paste()
{
    if ( !wxTheClipboard->Open() ) return;

    wxsWindowResDataObject Data;
    if ( wxTheClipboard->GetData(Data) )
    {
        wxsItem* RelativeTo = GetCurrentSelection();
        int InsertionType = InsType;
        if ( !RelativeTo )
        {
            InsertionType = itInto;
            RelativeTo = GetWinRes()->GetRootItem();
            wxsParent* Parent = RelativeTo->ToParent();
            if ( Parent &&
                 Parent->GetChildCount() == 1 &&
                 Parent->GetChild(0)->GetType() == wxsTSizer )
            {
                RelativeTo = Parent->GetChild(0);
            }
        }

        int Cnt = Data.GetItemCount();
        if ( Cnt )
        {
            ResourceLock();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsItem* Insert = Data.BuildItem(GetWinRes(),i);
                if ( Insert )
                {
                    switch ( InsertionType )
                    {
                        case itAfter:
                            InsertAfter(Insert,RelativeTo);
                            RelativeTo = Insert;
                            break;

                        case itBefore:
                            InsertBefore(Insert,RelativeTo);
                            break;

                        case itInto:
                            InsertInto(Insert,RelativeTo);
                            break;
                    }
                }
            }
            ResourceUnlock();
            GetWinRes()->RebuildCode();
        }
    }
    wxTheClipboard->Close();

	// TODO: Select added items
	RootItem()->ClearSelection();
	GetWinRes()->SelectionChanged(NULL);
}

bool wxsWindowEditor::InsertBefore(wxsItem* New,wxsItem* Ref)
{
	if ( !Ref ) Ref = GetCurrentSelection();

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index) )
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),Parent->GetLastTreeItemId(),Index);

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

bool wxsWindowEditor::InsertAfter(wxsItem* New,wxsItem* Ref)
{
	if ( !Ref ) Ref = GetCurrentSelection();

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index+1))
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),Parent->GetLastTreeItemId(),Index+1);

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

bool wxsWindowEditor::InsertInto(wxsItem* New,wxsItem* Ref)
{//# Called from InsertRequest() and Paste()
	if ( !Ref ) Ref = GetCurrentSelection();    //# Is this needed ?
	if ( !Ref || !Ref->ToParent() )
	{
		wxsKILL(New);
		return false;
	}

    Corrector->BeforePaste(New);
    wxsParent* P = Ref->ToParent();
    if ( !P->AddChild(New) )
    {
        wxsKILL(New);
        return false;
    }
    Corrector->AfterPaste(New);

    // Adding this new item into resource tree
    New->BuildItemTree(wxsTREE(),P->GetLastTreeItemId());

	// TODO: Set selection properly
	GetWinRes()->SelectionChanged(NULL);
    return true;
}

void wxsWindowEditor::InitializeImages()
{
    if ( ImagesLoaded ) return;
    wxString basePath = ConfigManager::GetDataFolder() + _T("/images/wxsmith/");

    static const wxString NormalNames[] =
    {
        _T("insertinto.png"),
        _T("insertafter.png"),
        _T("insertbefore.png"),
        _T("deletewidget.png"),
        _T("showpreview.png"),
        _T("quickpropsopen.png"),
        _T("quickpropsclose.png"),
        _T("selected.png")
    };

    static const wxString SmallNames[] =
    {
        _T("insertinto16.png"),
        _T("insertafter16.png"),
        _T("insertbefore16.png"),
        _T("deletewidget16.png"),
        _T("showpreview16.png"),
        _T("quickpropsopen16.png"),
        _T("quickpropsclose16.png"),
        _T("selected16.png")
    };

    const wxString* Array = ( wxsDWToolIconSize == 16L ) ? SmallNames : NormalNames;

    InsIntoImg.LoadFile(basePath + Array[0]);
    InsAfterImg.LoadFile(basePath + Array[1]);
    InsBeforeImg.LoadFile(basePath + Array[2]);
    DelImg.LoadFile(basePath + Array[3]);
    PreviewImg.LoadFile(basePath + Array[4]);
    QuickPropsImgOpen.LoadFile(basePath + Array[5]);
    QuickPropsImgClose.LoadFile(basePath + Array[6]);
    SelectedImg.LoadFile(basePath + Array[7]);

    ImagesLoaded = true;
}

namespace
{
    int PrioritySort(const wxsItemInfo** it1,const wxsItemInfo** it2)
    {
        return (*it1)->Priority - (*it2)->Priority;
    }

    WX_DEFINE_ARRAY(const wxsItemInfo*,ItemsT);
    WX_DECLARE_STRING_HASH_MAP(ItemsT,MapT);
}

void wxsWindowEditor::BuildPalette(wxNotebook* Palette)
{
    Palette->DeleteAllPages();

    // First we need to split all widgets into groups
    // it will be done using multimap (map of arrays)

    MapT Map;

    for ( const wxsItemInfo* Info = wxsFACTORY()->GetFirstInfo(); Info; Info = wxsFACTORY()->GetNextInfo() )
    {
        if ( !Info->Category.empty() )
        {
            Map[Info->Category].Add(Info);
        }
    }

    for ( MapT::iterator i = Map.begin(); i!=Map.end(); ++i )
    {
        wxScrolledWindow* CurrentPanel = new wxScrolledWindow(Palette,-1,wxDefaultPosition,wxDefaultSize,0/*wxALWAYS_SHOW_SB|wxHSCROLL*/);
        CurrentPanel->SetScrollRate(1,0);
        Palette->AddPage(CurrentPanel,i->first);
        wxSizer* RowSizer = new wxBoxSizer(wxHORIZONTAL);

        ItemsT& Items = i->second;
        Items.Sort(PrioritySort);

        for ( size_t j=Items.Count(); j-->0; )
        {
            const wxsItemInfo* Info = Items[j];
            wxBitmap* Icon;
            if ( wxsDWPalIconSize == 16L )
            {
                Icon = Info->Icon16;
            }
            else
            {
                Icon = Info->Icon32;
            }

            if ( Icon )
            {
                wxBitmapButton* Btn =
                    new wxBitmapButton(CurrentPanel,-1,*Icon,
                        wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,
                        wxDefaultValidator, Info->Name);
                RowSizer->Add(Btn,0,wxALIGN_CENTER);
                Btn->SetToolTip(Info->Name);
            }
            else
            {
                wxButton* Btn = new wxButton(CurrentPanel,-1,Info->Name,
                    wxDefaultPosition,wxDefaultSize,0,
                    wxDefaultValidator,Info->Name);
                RowSizer->Add(Btn,0,wxGROW);
                Btn->SetToolTip(Info->Name);
            }
        }
        CurrentPanel->SetSizer(RowSizer);
        RowSizer->SetVirtualSizeHints(CurrentPanel);
    }
}

void wxsWindowEditor::InsertRequest(const wxString& Name)
{
    wxsItem* CurrentSelection = GetCurrentSelection();
    if ( !CurrentSelection )
    {
        DBGLOG(_("wxSmith: No item selected - couldn't create new item"));
        return;
    }

    if ( !InsType ) return;

    wxsItem* New = wxsGEN(Name,GetWinRes());
    if ( !New )
    {
        DBGLOG(_("wxSmith: Culdn't generate item inside factory"));
        return;
    }

    ResourceLock();
    switch ( InsType )
    {
        case itBefore:
            InsertBefore(New,CurrentSelection);
            break;

        case itAfter:
            InsertAfter(New,CurrentSelection);
            break;

        case itInto:
            InsertInto(New,CurrentSelection);
            break;

        default:
            wxsKILL(New);
            DBGLOG(_T("wxSmith: Internal error"));
            break;
    }
    ResourceUnlock();
    GetWinRes()->RebuildCode();
}

void wxsWindowEditor::OnButton(wxCommandEvent& event)
{
    wxWindow* Btn = (wxWindow*)event.GetEventObject();
    if ( Btn )
    {
        InsertRequest(Btn->GetName());
    }
}

void wxsWindowEditor::SetInsertionTypeMask(int Mask)
{
    InsTypeMask = Mask;
    SetInsertionType(InsType);
}

void wxsWindowEditor::SetInsertionType(int Type)
{
    Type &= InsTypeMask;

    if ( !Type ) Type = InsTypeMask;

    if ( Type & itInto )
    {
        InsType = itInto;
    }
    else if ( Type & itAfter )
    {
        InsType = itAfter;
    }
    else if ( Type & itBefore )
    {
        InsType = itBefore;
    }
    else
    {
        InsType = 0;
    }

    RebuildInsTypeIcons();
}

void wxsWindowEditor::RebuildInsTypeIcons()
{
    BuildInsTypeIcon(InsIntoBtn,InsIntoImg,itInto);
    BuildInsTypeIcon(InsBeforeBtn,InsBeforeImg,itBefore);
    BuildInsTypeIcon(InsAfterBtn,InsAfterImg,itAfter);
}

void wxsWindowEditor::BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,int ButtonType)
{
    bool Selected = (InsType & ButtonType) != 0;
    bool Enabled = (InsTypeMask & ButtonType) != 0;

    if ( !Enabled || !Selected )
    {
        Btn->SetLabel(Original);
    }
    else
    {
        wxBitmap Copy = Original;
        wxMemoryDC DC;
        DC.SelectObject(Copy);
        DC.DrawBitmap(SelectedImg,0,0);
        Btn->SetLabel(Copy);
    }

    Btn->Enable(Enabled);
    Btn->Refresh();
}

void wxsWindowEditor::RebuildQuickPropsIcon()
{
    QuickPanelBtn->SetLabel( QuickPropsOpen ? QuickPropsImgClose : QuickPropsImgOpen );
}

void wxsWindowEditor::RebuildIcons()
{
    RebuildInsTypeIcons();
    RebuildQuickPropsIcon();
    DelBtn->SetLabel(DelImg);
    PreviewBtn->SetLabel(PreviewImg);
    BuildPalette(WidgetsSet);
    Layout();
}

void wxsWindowEditor::OnInsInto(wxCommandEvent& event)
{
    SetInsertionType(itInto);
}

void wxsWindowEditor::OnInsAfter(wxCommandEvent& event)
{
    SetInsertionType(itAfter);
}

void wxsWindowEditor::OnInsBefore(wxCommandEvent& event)
{
    SetInsertionType(itBefore);
}

void wxsWindowEditor::OnDelete(wxCommandEvent& event)
{
    ResourceLock();
    KillSelection(RootItem());
    ResourceUnlock();

	// TODO: Select previous item / parent etc.
	GetWinRes()->SelectionChanged(NULL);
}

void wxsWindowEditor::OnPreview(wxCommandEvent& event)
{
    if ( GetWinRes()->IsPreview() )
    {
        GetWinRes()->HidePreview();
    }
    else
    {
        GetWinRes()->ShowPreview();
    }
}

void wxsWindowEditor::OnQuickProps(wxCommandEvent& event)
{
    QuickPropsOpen = !QuickPropsOpen;
    RebuildQuickPropsIcon();
    ToggleQuickPropsPanel(QuickPropsOpen);
}

void wxsWindowEditor::ToggleQuickPropsPanel(bool Open)
{
    HorizSizer->Show(QPArea,Open,true);
    Layout();
}

void wxsWindowEditor::RebuildQuickProps(wxsItem* Selection)
{
    Freeze();

    int QPx, QPy;
    // TODO: Check if content of previous QPPanel shouldn't be stored into item
    QPArea->GetViewStart(&QPx,&QPy);
    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);

    if ( Selection )
    {
        wxWindow* QPPanel = Selection->BuildQuickPropertiesPanel(QPArea);
        if ( QPPanel )
        {
            QPSizer->Add(QPPanel,0,wxEXPAND);
        }
    }
    QPSizer->Layout();
    QPSizer->Fit(QPArea);
    Layout();
    QPArea->Scroll(QPx,QPy);
    Thaw();
}

void wxsWindowEditor::ResourceLock()
{
    ResourceLockCnt++;
}

void wxsWindowEditor::ResourceUnlock()
{
    if ( ! --ResourceLockCnt )
    {
        UndoBuff->StoreChange();
        Freeze();
        KillPreview();
        BuildPreview();
        Thaw();
        SetModified(true);
    }

    wxASSERT_MSG(ResourceLockCnt>=0,
        _T("wxsWindowEditor::ResourceUnlock() called without\n")
        _T("corresponding wxsWindowEditor::ResourceLock()"));
}

void wxsWindowEditor::SelectionChanged()
{
    wxsItem* Item = GetCurrentSelection();
    // Updating insertion type mask

    int itMask = 0;
    if ( Item )
    {
        if ( Item->GetParent() )
        {
            // When sizer is added into non-sizer parent, no other items can be added to
            // this parent
            if ( Item->GetType() != wxsTSizer ||
                 Item->GetParent()->GetType() == wxsTSizer )
            {
                itMask |= itBefore | itAfter;
            }
        }

        if ( Item->ToParent() )
        {
            itMask |= itInto;
        }
    }

    SetInsertionTypeMask(itMask);
    RebuildQuickProps(Item);

    // Refreshing selection items inside content window
    Content->RefreshSelection();

    // TODO: Refresh set of available items inside palette
}

void wxsWindowEditor::NotifyChange(wxsItem* Changed)
{
    ResourceLock();
    Corrector->AfterChange(Changed);
    ResourceUnlock();
}

void wxsWindowEditor::GetSelectionNoChildren(wxsWindowEditor::ItemArray& Array,wxsItem* Item)
{
    if ( Item->GetIsSelected() )
    {
        Array.Add(Item);
    }
    else
    {
        wxsParent* P = Item->ToParent();
        if ( P )
        {
            for ( int i=0; i<P->GetChildCount(); i++ )
            {
                GetSelectionNoChildren(Array,P->GetChild(i));
            }
        }
    }
}

wxImage wxsWindowEditor::InsIntoImg;
wxImage wxsWindowEditor::InsBeforeImg;
wxImage wxsWindowEditor::InsAfterImg;
wxImage wxsWindowEditor::DelImg;
wxImage wxsWindowEditor::PreviewImg;
wxImage wxsWindowEditor::QuickPropsImgOpen;
wxImage wxsWindowEditor::QuickPropsImgClose;
wxImage wxsWindowEditor::SelectedImg;
wxsWindowEditor::WindowSet wxsWindowEditor::AllEditors;
bool wxsWindowEditor::ImagesLoaded = false;

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_BUTTON(wxsInsIntoId,wxsWindowEditor::OnInsInto)
    EVT_BUTTON(wxsInsBeforeId,wxsWindowEditor::OnInsBefore)
    EVT_BUTTON(wxsInsAfterId,wxsWindowEditor::OnInsAfter)
    EVT_BUTTON(wxsDelId,wxsWindowEditor::OnDelete)
    EVT_BUTTON(wxsPreviewId,wxsWindowEditor::OnPreview)
    EVT_BUTTON(wxsQuickPropsId,wxsWindowEditor::OnQuickProps)
    EVT_BUTTON(-1,wxsWindowEditor::OnButton)
END_EVENT_TABLE()
