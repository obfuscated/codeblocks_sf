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
    m_TopPreview(NULL),
    m_InsType(itBefore),
    m_InsTypeMask(itBefore),
    m_QuickPropsOpen(false),
    m_DontStoreUndo(false)
{
    wxASSERT(Resource!=NULL);

    // Loading images if needed
    InitializeImages();

    // Creating content of editor window
    m_VertSizer = new wxBoxSizer(wxVERTICAL);
    m_WidgetsSet = new wxNotebook(this,-1);
    BuildPalette(m_WidgetsSet);
    m_HorizSizer = new wxBoxSizer(wxHORIZONTAL);
    m_VertSizer->Add(m_HorizSizer,1,wxEXPAND);
    m_VertSizer->Add(m_WidgetsSet,0,wxEXPAND);

    m_Content = new wxsWindowEditorContent(this);
    m_HorizSizer->Add(m_Content,1,wxEXPAND);

    m_QPArea = new wxScrolledWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxVSCROLL|wxSUNKEN_BORDER|wxALWAYS_SHOW_SB);
    m_QPArea->SetScrollbars(0,5,0,0);
    m_HorizSizer->Add(m_QPArea,0,wxEXPAND);
    m_QPSizer = new wxBoxSizer(wxVERTICAL);
    m_QPArea->SetSizer(m_QPSizer);

    m_OpsSizer = new wxBoxSizer(wxVERTICAL);
    m_HorizSizer->Add(m_OpsSizer,0,wxEXPAND);

    m_OpsSizer->Add(m_InsIntoBtn   = new wxBitmapButton(this,wxsInsIntoId,m_InsIntoImg));
    m_OpsSizer->Add(m_InsBeforeBtn = new wxBitmapButton(this,wxsInsBeforeId,m_InsBeforeImg));
    m_OpsSizer->Add(m_InsAfterBtn  = new wxBitmapButton(this,wxsInsAfterId,m_InsAfterImg));
    m_OpsSizer->Add(1,5);
    m_OpsSizer->Add(m_DelBtn       = new wxBitmapButton(this,wxsDelId,m_DelImg));
    m_OpsSizer->Add(m_PreviewBtn   = new wxBitmapButton(this,wxsPreviewId,m_PreviewImg));
    m_OpsSizer->Add(1,5);
    m_OpsSizer->Add(m_QuickPanelBtn = new wxBitmapButton(this,wxsQuickPropsId,m_QuickPropsImgOpen));
    m_InsIntoBtn   ->SetToolTip(_("Insert new widgets into current selection"));
    m_InsBeforeBtn ->SetToolTip(_("Insert new widgets before current selection"));
    m_InsAfterBtn  ->SetToolTip(_("Insert new widgets after current selection"));
    m_DelBtn       ->SetToolTip(_("Delete current selection"));
    m_PreviewBtn   ->SetToolTip(_("Show preview"));
    m_QuickPanelBtn->SetToolTip(_("Open / Close Quick Properties panel"));

    SetSizer(m_VertSizer);

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

    m_UndoBuff = new wxsWinUndoBuffer(GetWinRes());
    m_Corrector = new wxsCorrector(GetWinRes());
    ToggleQuickPropsPanel(false);       // TODO: Shouldn't store initial state of panel somewhere?
    m_AllEditors.insert(this);

    bool DataWasCorrected = m_Corrector->GlobalCheck();

    SelectOneItem(RootItem());
    RebuildPreview();

    if ( DataWasCorrected )
    {
        // TODO: Show some nice notification
        m_UndoBuff->StoreChange();
    }

    // Source code is being rebuilt here. If something changed in
    // source code generation, new code will probably be better than old
    // one. If nothing changed, wxsCoder class will detect that changes do
    // not affect files and won't change them.
    GetWinRes()->RebuildCode();
}

wxsWindowEditor::~wxsWindowEditor()
{
    // Destroying also Quick Props panel which usually triggers it's
    // Save() method when being destroyed
    m_QPArea->SetSizer(NULL);
    m_QPArea->DestroyChildren();

    // First we need to discard all changes,
    // this operation will recreate unmodified code
    // in source files
    if ( GetModified() )
    {
        GetWinRes()->LoadResource();
        GetWinRes()->RebuildCode();
    }

    // Now doing the rest
	delete m_UndoBuff;
	delete m_Corrector;
	GetWinRes()->HidePreview();

	m_AllEditors.erase(this);
}

void wxsWindowEditor::ReloadImages()
{
    m_ImagesLoaded = false;
    InitializeImages();
    for ( WindowSet::iterator i=m_AllEditors.begin(); i!=m_AllEditors.end(); ++i )
    {
        (*i)->RebuildIcons();
    }
}

void wxsWindowEditor::RebuildPreview()
{
    wxASSERT_MSG(RootItem()!=NULL,_T("wxsWindowEditor::RebuildPreview() called without valid item"));

    Freeze();

    // If there's previous preview, deleting it
    if ( m_TopPreview )
    {
        m_Content->SetSizer(NULL);
        delete m_TopPreview;
        m_TopPreview = NULL;
    }

    // Generating preview
    wxObject* TopPreviewObject = RootItem()->BuildPreview(m_Content,false);
    m_TopPreview = wxDynamicCast(TopPreviewObject,wxWindow);
    if ( !m_TopPreview )
    {
        DBGLOG(_T("One of root items returned class not derived from wxWindow"));
        delete TopPreviewObject;
    }
    else
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(m_TopPreview,0,wxALL,10);
        m_Content->SetVirtualSizeHints(1,1);
        m_Content->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(m_Content);
        NewSizer->FitInside(m_Content);
        m_HorizSizer->Layout();
        m_VertSizer->Layout();
    }

    Layout();
    Thaw();

    // Updating all informations in Content
    m_Content->NewPreview();

    // Preview bindings are no longer needed
    RootItem()->InvalidatePreview();
}

void wxsWindowEditor::RebuildTree()
{
    GetWinRes()->RebuildResourceTree();
}

void wxsWindowEditor::UpdateSelection()
{
    if ( GetCurrentSelection() )
    {
        GetCurrentSelection()->ShowInPropertyGrid();
        // TODO: Check what's needed to rebuild quick properties panel
    }
}

void wxsWindowEditor::StoreTreeState()
{
    GetWinRes()->StoreTreeState();
}

bool wxsWindowEditor::Save()
{
    // TODO: Add error checking
    GetWinRes()->SaveResource();
    m_UndoBuff->Saved();
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
	return m_UndoBuff->CanUndo();
}

bool wxsWindowEditor::CanRedo()
{
	return m_UndoBuff->CanRedo();
}

void wxsWindowEditor::Undo()
{
    BeginChange();
    m_UndoBuff->Undo();
    m_Corrector->ClearCache();
    // TODO: Restore selection
	SelectOneItem(RootItem());
	SetModified(m_UndoBuff->IsModified());

    // Setting m_DontStoreUndo to false to avoid
    // creating new undo entry from current one
    m_DontStoreUndo = true;
    EndChange();
    m_DontStoreUndo = false;
}

void wxsWindowEditor::Redo()
{
    BeginChange();
    m_UndoBuff->Redo();
    m_Corrector->ClearCache();
    // TODO: Restore selection
	SelectOneItem(RootItem());
	SetModified(m_UndoBuff->IsModified());

    // Setting m_DontStoreUndo to false to avoid
    // creating new undo entry from current one
    m_DontStoreUndo = true;
    EndChange();
    m_DontStoreUndo = false;
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
    KillSelection();
}

void wxsWindowEditor::KillSelection()
{
    BeginChange();

    wxsItem* NewSelection = GetCurrentSelection();

    for (;;)
    {
        while ( NewSelection->GetIsSelected() )
        {
            // This item will probably be deleted it can not be new
            // selection if it's not root resource item
            if ( !NewSelection->GetParent() ) break;
            NewSelection = NewSelection->GetParent();
        }

        // Creating new walker item which will search for any selected
        // item in sequence of parents
        wxsItem* Walker = NewSelection->GetParent();
        while ( Walker && !Walker->GetIsSelected() )
        {
            Walker = Walker->GetParent();
        }

        // when reached NULL, NewSelection is correct because it don't
        // have any selected (grand)parent and it's not selected
        if ( !Walker ) break;

        NewSelection = Walker;
    }


    KillSelection(RootItem());
    m_Corrector->ClearCache();
	SelectOneItem(NewSelection);
    EndChange();
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
        int InsertionType = m_InsType;
        wxsItem* RelativeTo = GetReferenceItem(InsertionType);

        int Cnt = Data.GetItemCount();
        if ( Cnt )
        {
            BeginChange();
            RootItem()->ClearSelection();
            wxsItem* NewRootSelection = NULL;
            for ( int i=0; i<Cnt; i++ )
            {
                wxsItem* Insert = Data.BuildItem(GetWinRes(),i);
                if ( Insert )
                {
                    switch ( InsertionType )
                    {
                        case itAfter:
                            if ( InsertAfter(Insert,RelativeTo) )
                            {
                                RelativeTo = Insert;
                                if ( !NewRootSelection )
                                {
                                    NewRootSelection = Insert;
                                }
                            }
                            break;

                        case itBefore:
                            if ( InsertBefore(Insert,RelativeTo) )
                            {
                                if ( !NewRootSelection )
                                {
                                    NewRootSelection = Insert;
                                }
                            }
                            break;

                        case itInto:
                            if ( InsertInto(Insert,RelativeTo) )
                            {
                                if ( !NewRootSelection )
                                {
                                    NewRootSelection = Insert;
                                }
                            }
                            break;

                        default:
                            wxsKILL(Insert);
                    }
                }
            }

            GetWinRes()->SetRootSelection(NewRootSelection);
            EndChange();
        }
    }
    wxTheClipboard->Close();
}

bool wxsWindowEditor::InsertBefore(wxsItem* New,wxsItem* Ref)
{
    wxASSERT(Ref!=NULL);

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    m_Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index) )
    {
        wxsKILL(New);
        return false;
    }
    m_Corrector->AfterPaste(New);
    New->SetIsSelected(true);
    return true;
}

bool wxsWindowEditor::InsertAfter(wxsItem* New,wxsItem* Ref)
{
    wxASSERT(Ref!=NULL);

    wxsParent* Parent = Ref->GetParent();
    if ( !Parent )
    {
        wxsKILL(New);
        return false;
    }

    m_Corrector->BeforePaste(New);
    int Index = Parent->GetChildIndex(Ref);
    if ( Index<0 || !Parent->AddChild(New,Index+1))
    {
        wxsKILL(New);
        return false;
    }
    m_Corrector->AfterPaste(New);
	New->SetIsSelected(true);
    return true;
}

bool wxsWindowEditor::InsertInto(wxsItem* New,wxsItem* Ref)
{
    m_Corrector->BeforePaste(New);
    wxsParent* P = Ref->ToParent();
    if ( !P->AddChild(New) )
    {
        wxsKILL(New);
        return false;
    }
    m_Corrector->AfterPaste(New);
    New->SetIsSelected(true);
    return true;
}

void wxsWindowEditor::InitializeImages()
{
    if ( m_ImagesLoaded ) return;
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

    m_InsIntoImg.LoadFile(basePath + Array[0]);
    m_InsAfterImg.LoadFile(basePath + Array[1]);
    m_InsBeforeImg.LoadFile(basePath + Array[2]);
    m_DelImg.LoadFile(basePath + Array[3]);
    m_PreviewImg.LoadFile(basePath + Array[4]);
    m_QuickPropsImgOpen.LoadFile(basePath + Array[5]);
    m_QuickPropsImgClose.LoadFile(basePath + Array[6]);
    m_SelectedImg.LoadFile(basePath + Array[7]);

    m_ImagesLoaded = true;
}

void wxsWindowEditor::InsertRequest(const wxString& Name)
{
    int InsertionType = m_InsType;
    wxsItem* ReferenceItem = GetReferenceItem(InsertionType);
    if ( !ReferenceItem )
    {
        DBGLOG(_T("wxSmith: No item selected - couldn't create new item"));
        return;
    }

    if ( InsertionType!=itBefore && InsertionType!=itAfter && InsertionType!=itInto )
    {
        return;
    }

    wxsItem* New = wxsGEN(Name,GetWinRes());
    if ( !New )
    {
        DBGLOG(_T("wxSmith: Culdn't generate item inside factory"));
        return;
    }

    BeginChange();
    switch ( InsertionType )
    {
        case itBefore:
            if ( InsertBefore(New,ReferenceItem) )
            {
                RootItem()->ClearSelection();
                GetWinRes()->SetRootSelection(New);
            }
            break;

        case itAfter:
            if ( InsertAfter(New,ReferenceItem) )
            {
                RootItem()->ClearSelection();
                GetWinRes()->SetRootSelection(New);
            }
            break;

        case itInto:
            if ( InsertInto(New,ReferenceItem) )
            {
                RootItem()->ClearSelection();
                GetWinRes()->SetRootSelection(New);
            }
            break;
    }
    EndChange();
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
    m_InsTypeMask = Mask;
    SetInsertionType(m_InsType);
}

void wxsWindowEditor::SetInsertionType(int Type)
{
    Type &= m_InsTypeMask;

    if ( !Type ) Type = m_InsTypeMask;

    if ( Type & itInto )
    {
        m_InsType = itInto;
    }
    else if ( Type & itAfter )
    {
        m_InsType = itAfter;
    }
    else if ( Type & itBefore )
    {
        m_InsType = itBefore;
    }
    else
    {
        m_InsType = 0;
    }

    RebuildInsTypeIcons();
}

void wxsWindowEditor::RebuildInsTypeIcons()
{
    BuildInsTypeIcon(m_InsIntoBtn,m_InsIntoImg,itInto);
    BuildInsTypeIcon(m_InsBeforeBtn,m_InsBeforeImg,itBefore);
    BuildInsTypeIcon(m_InsAfterBtn,m_InsAfterImg,itAfter);
}

void wxsWindowEditor::BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,int ButtonType)
{
    bool Selected = (m_InsType & ButtonType) != 0;
    bool Enabled = (m_InsTypeMask & ButtonType) != 0;

    if ( !Enabled || !Selected )
    {
        Btn->SetLabel(Original);
    }
    else
    {
        wxBitmap Copy = Original;
        wxMemoryDC DC;
        DC.SelectObject(Copy);
        DC.DrawBitmap(m_SelectedImg,0,0);
        Btn->SetLabel(Copy);
    }

    Btn->Enable(Enabled);
    Btn->Refresh();
}

void wxsWindowEditor::RebuildQuickPropsIcon()
{
    m_QuickPanelBtn->SetLabel( m_QuickPropsOpen ? m_QuickPropsImgClose : m_QuickPropsImgOpen );
}

void wxsWindowEditor::RebuildIcons()
{
    RebuildInsTypeIcons();
    RebuildQuickPropsIcon();
    m_DelBtn->SetLabel(m_DelImg);
    m_PreviewBtn->SetLabel(m_PreviewImg);
    BuildPalette(m_WidgetsSet);
    Layout();
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
    KillSelection();
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
    m_QuickPropsOpen = !m_QuickPropsOpen;
    RebuildQuickPropsIcon();
    ToggleQuickPropsPanel(m_QuickPropsOpen);
}

void wxsWindowEditor::ToggleQuickPropsPanel(bool Open)
{
    m_HorizSizer->Show(m_QPArea,Open,true);
    Layout();
}

void wxsWindowEditor::RebuildQuickProps(wxsItem* Selection)
{
    Freeze();

    int QPx, QPy;
    // TODO: Check if content of previous QPPanel shouldn't be stored into item
    m_QPArea->GetViewStart(&QPx,&QPy);
    m_QPArea->SetSizer(NULL);
    m_QPArea->DestroyChildren();
    m_QPSizer = new wxBoxSizer(wxVERTICAL);
    m_QPArea->SetSizer(m_QPSizer);

    if ( Selection )
    {
        wxWindow* QPPanel = Selection->BuildQuickPropertiesPanel(m_QPArea);
        if ( QPPanel )
        {
            m_QPSizer->Add(QPPanel,0,wxEXPAND);
        }
    }
    m_QPSizer->Layout();
    m_QPSizer->Fit(m_QPArea);
    Layout();
    m_QPArea->Scroll(QPx,QPy);
    Thaw();
}

void wxsWindowEditor::OnChangeInit()
{
    StoreTreeState();
}

void wxsWindowEditor::OnChangeFinish()
{
    if ( !m_DontStoreUndo )
    {
        m_UndoBuff->StoreChange();
    }
    RebuildPreview();
    RebuildTree();
    UpdateSelection();
    SetModified(true);
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
    m_Content->RefreshSelection();

    // TODO: Refresh set of available items inside palette
}

void wxsWindowEditor::NotifyChange(wxsItem* Changed)
{
    BeginChange();
    m_Corrector->AfterChange(Changed);
    EndChange();
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

void wxsWindowEditor::SelectOneItem(wxsItem* ItemToSelect)
{
    RootItem()->ClearSelection();
    if ( ItemToSelect )
    {
        ItemToSelect->SetIsSelected(true);
        GetWinRes()->SetRootSelection(ItemToSelect);
    }
    else
    {
        GetWinRes()->SetRootSelection(NULL);
    }
}

wxsItem* wxsWindowEditor::GetReferenceItem(int& InsertionType)
{
    wxsItem* Reference = GetCurrentSelection();
    if ( !Reference )
    {
        // Fixing up reference item when there's nothing selected
        InsertionType = itInto;
        Reference = GetWinRes()->GetRootItem();
        wxsParent* Parent = Reference->ToParent();
        if ( Parent &&
             Parent->GetChildCount() == 1 &&
             Parent->GetChild(0)->GetType() == wxsTSizer )
        {
            Reference = Parent->GetChild(0);
        }
    }
    return Reference;
}

wxImage wxsWindowEditor::m_InsIntoImg;
wxImage wxsWindowEditor::m_InsBeforeImg;
wxImage wxsWindowEditor::m_InsAfterImg;
wxImage wxsWindowEditor::m_DelImg;
wxImage wxsWindowEditor::m_PreviewImg;
wxImage wxsWindowEditor::m_QuickPropsImgOpen;
wxImage wxsWindowEditor::m_QuickPropsImgClose;
wxImage wxsWindowEditor::m_SelectedImg;
wxsWindowEditor::WindowSet wxsWindowEditor::m_AllEditors;
bool wxsWindowEditor::m_ImagesLoaded = false;

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_BUTTON(wxsInsIntoId,wxsWindowEditor::OnInsInto)
    EVT_BUTTON(wxsInsBeforeId,wxsWindowEditor::OnInsBefore)
    EVT_BUTTON(wxsInsAfterId,wxsWindowEditor::OnInsAfter)
    EVT_BUTTON(wxsDelId,wxsWindowEditor::OnDelete)
    EVT_BUTTON(wxsPreviewId,wxsWindowEditor::OnPreview)
    EVT_BUTTON(wxsQuickPropsId,wxsWindowEditor::OnQuickProps)
    EVT_BUTTON(-1,wxsWindowEditor::OnButton)
END_EVENT_TABLE()
