#include "wxsheaders.h"
#include "wxswindoweditor.h"

#include "widget.h"
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include <wx/clipbrd.h>
#include <manager.h>
#include <configmanager.h>
#include "wxspropertiesman.h"
#include "wxsmith.h"
#include "wxsresource.h"
#include "wxsdragwindow.h"
#include "resources/wxswindowres.h"
#include "resources/wxswindowresdataobject.h"
#include "wxswinundobuffer.h"
#include "wxswidgetfactory.h"
#include "wxsevent.h"

DECLARE_EVENT_TYPE(wxsEVT_RELAYOUT, -1)
DEFINE_EVENT_TYPE(wxsEVT_RELAYOUT)



namespace {
struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStricmp(s1, s2) < 0; } };
};

static const long wxsInsIntoId    = wxNewId();
static const long wxsInsBeforeId  = wxNewId();
static const long wxsInsAfterId   = wxNewId();
static const long wxsDelId        = wxNewId();
static const long wxsPreviewId    = wxNewId();
static const long wxsQuickPropsId = wxNewId();
static const long wxsScrollId     = wxNewId();


wxsWindowEditor::wxsWindowEditor(wxWindow* parent,wxsWindowRes* Resource):
    wxsEditor(parent,Resource->GetWxsFile(),Resource),
    QuickPropsOpen(false),
    UndoBuff(new wxsWinUndoBuffer(Resource)),
    InsideMultipleChange(false)
{
    InitializeImages();

    VertSizer = new wxBoxSizer(wxVERTICAL);
    WidgetsSet = new wxNotebook(this,-1);
    BuildPalette(WidgetsSet);
    HorizSizer = new wxBoxSizer(wxHORIZONTAL);
    VertSizer->Add(HorizSizer,1,wxEXPAND);
    VertSizer->Add(WidgetsSet,0,wxEXPAND);

//    Scroll = new wxScrolledWindow(this,wxsScrollId);
//    Scroll->SetScrollRate(4,4);
//    Scroll->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
//    Scroll->Connect(-1,wxEVT_SIZE,(wxObjectEventFunction)&wxsWindowEditor::OnContentSize,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_TOP,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_BOTTOM,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_LINEUP,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_LINEDOWN,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_PAGEUP,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_PAGEDOWN,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_THUMBRELEASE,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
//    Scroll->Connect(-1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&wxsWindowEditor::OnContentScroll,NULL,this);
    DragWnd = new wxsDragWindow(this,NULL);
    HorizSizer->Add(DragWnd,1,wxEXPAND);

    QPArea = new wxScrolledWindow(this,-1,wxDefaultPosition,wxDefaultSize,wxVSCROLL|wxSUNKEN_BORDER|wxALWAYS_SHOW_SB);
    QPArea->SetScrollbars(0,5,0,0);
    HorizSizer->Add(QPArea,0,wxEXPAND);
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);

    OpsSizer = new wxBoxSizer(wxVERTICAL);
    OpsBackground = new wxScrolledWindow(this,-1);
    OpsBackground->SetScrollbars(0,5,0,0);
    HorizSizer->Add(OpsBackground,0,wxEXPAND);
    OpsBackground->SetSizer(OpsSizer);

    OpsSizer->Add(InsIntoBtn   = new wxBitmapButton(OpsBackground,wxsInsIntoId,InsIntoImg));
    OpsSizer->Add(InsBeforeBtn = new wxBitmapButton(OpsBackground,wxsInsBeforeId,InsBeforeImg));
    OpsSizer->Add(InsAfterBtn  = new wxBitmapButton(OpsBackground,wxsInsAfterId,InsAfterImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(DelBtn       = new wxBitmapButton(OpsBackground,wxsDelId,DelImg));
    OpsSizer->Add(PreviewBtn   = new wxBitmapButton(OpsBackground,wxsPreviewId,PreviewImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(QuickPanelBtn = new wxBitmapButton(OpsBackground,wxsQuickPropsId,QuickPropsImgOpen));
    InsIntoBtn   ->SetToolTip(_("Insert new widgets into current selection"));
    InsBeforeBtn ->SetToolTip(_("Insert new widgets before current selection"));
    InsAfterBtn  ->SetToolTip(_("Insert new widgets after current selection"));
    DelBtn       ->SetToolTip(_("Delete current selection"));
    PreviewBtn   ->SetToolTip(_("Show preview"));
    QuickPanelBtn->SetToolTip(_("Open / Close Quick Properties panel"));

    OpsSizer->SetVirtualSizeHints(OpsBackground);

    SetSizer(VertSizer);

    SetInsertionTypeMask(0);

//    DragWnd = new wxsDragWindow(Scroll,NULL,Scroll->GetSize());
//    DragWnd->Hide();
    wxFileName Name(Resource->GetWxsFile());
    SetTitle(Name.GetFullName());

    // Storing current resource data as a base for undo buffer
    UndoBuff->StoreChange();
    UndoBuff->Saved();

    ToggleQuickPropsPanel(false);

    AllEditors.insert(this);
    BuildPreview();

    wxsSelectWidget(GetWinRes()->GetRootWidget());
}

wxsWindowEditor::~wxsWindowEditor()
{
    // Need to associate Drag window with empty widget
    // because sometimes it generated events when destroying window
    // what caused seg faults
    DragWnd->SetWidget(NULL);

    // Destroying also Quick Props panel which usually triggers it's
    // Save() method when being destroyed
    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();

    // First we need to discard all changes,
    // this operation will recreate unmodified code
    // in source files
    if ( GetModified() )
    {
        wxsWidget* NewRoot = UndoBuff->DiscardChanges();
        if ( NewRoot )
        {
            if ( !GetWinRes()->ChangeRootWidget(NewRoot) )
            {
                wxsKILL(NewRoot);
            }
        }
    }

    // Now doing the rest
	wxsUnselectRes(GetResource());
	KillPreview();
	delete UndoBuff;
	AllEditors.erase(this);
	GetResource()->HidePreview();
}

void wxsWindowEditor::ReloadImages()
{
    ImagesLoaded = false;
    InitializeImages();
    for ( std::set<wxsWindowEditor*>::iterator i = AllEditors.begin(); i != AllEditors.end(); ++i )
    {
        (*i)->RebuildIcons();
    }
}

static void WidgetRefreshReq(wxWindow* Wnd)
{
    if ( !Wnd ) return;
    Wnd->Refresh(true);

    wxWindowList& List = Wnd->GetChildren();
    for ( wxWindowListNode* Node = List.GetFirst(); Node; Node = Node->GetNext() )
    {
        wxWindow* Win = Node->GetData();
        WidgetRefreshReq(Win);
    }
}

void wxsWindowEditor::BuildPreview()
{
    DragWnd->SetSizer(NULL);
    Freeze();

    KillPreview();

    // Creating new sizer

    wxsWidget* TopWidget = GetWinRes()->GetRootWidget();
    wxWindow* TopPreviewWindow = TopWidget ? TopWidget->CreatePreview(DragWnd,this) : NULL;

    if ( TopPreviewWindow )
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        DragWnd->Show();
        NewSizer->Add(TopPreviewWindow,0,/*wxALIGN_CENTRE_VERTICAL|wxALIGN_CENTRE_HORIZONTAL|*/wxALL,10);
        DragWnd->SetVirtualSizeHints(1,1);
        DragWnd->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(DragWnd);
        HorizSizer->Layout();
        VertSizer->Layout();
        wxSize Virtual = DragWnd->GetVirtualSize();
        wxSize Real = DragWnd->GetSize();
        wxSize Drag(Virtual.GetWidth() > Real.GetWidth() ? Virtual.GetWidth() : Real.GetWidth(),
                    Virtual.GetHeight() > Real.GetHeight() ? Virtual.GetHeight() : Real.GetHeight());

        // Waiting to reposition and resize all widgets
        Thaw();
        Layout();
        Manager::Yield();
        DragWnd->SetWidget(TopWidget);
        DragWnd->ContentChanged();
    }
    else
    {
        Thaw();
    }
    Refresh();
}

void wxsWindowEditor::KillPreview()
{
    DragWnd->SetSizer(NULL);
    GetWinRes()->GetRootWidget()->KillPreview();
    DragWnd->Hide();
//    DragWnd->SetUpdateMode(true);
}

void wxsWindowEditor::OnMouseClick(wxMouseEvent& event)
{
    wxsPropertiesMan::Get()->SetActiveWidget(GetWinRes()->GetRootWidget());
}

void wxsWindowEditor::OnSelectWidget(wxsEvent& event)
{
	if ( DragWnd )
	{
		DragWnd->ProcessEvent(event);
	}

    int itMask = 0;

    if (  event.GetWidget()->GetParent() &&
        ( !event.GetWidget()->GetInfo().Sizer ||
           event.GetWidget()->GetParent()->GetInfo().Sizer ) )
    {
        itMask |= itBefore | itAfter;
    }

    if ( event.GetWidget()->IsContainer() )
    {
        itMask |= itInto;
    }

    SetInsertionTypeMask(itMask);
    RebuildQuickProps(GetSelection());
}

void wxsWindowEditor::OnUnselectWidget(wxsEvent& event)
{
	if ( event.GetWidget() == GetSelection() )
	{
	    RebuildQuickProps(NULL);
	}

	if ( DragWnd )
	{
		DragWnd->ProcessEvent(event);
	}

}

bool wxsWindowEditor::Close()
{
	return wxsEditor::Close();
}

bool wxsWindowEditor::Save()
{
    GetWinRes()->Save();
	return true;
}

bool wxsWindowEditor::GetModified() const
{
	return GetWinResConst()->GetModified();
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

bool wxsWindowEditor::CanUndo() const
{
	return UndoBuff->CanUndo();
}

bool wxsWindowEditor::CanRedo() const
{
	return UndoBuff->CanRedo();
}

void wxsWindowEditor::Undo()
{
    if ( !CanUndo() ) return;
	wxsWidget* NewRoot = UndoBuff->Undo();
	if ( !NewRoot ) return;
	if ( !GetWinRes()->ChangeRootWidget(NewRoot) )
	{
		DBGLOG(_("wxSmith ERROR: Something wrong with undo buffer !!!"));
		wxsKILL(NewRoot);
	}
	SetModified(UndoBuff->IsModified());
}

void wxsWindowEditor::Redo()
{
    if ( !CanRedo() ) return;
	wxsWidget* NewRoot = UndoBuff->Redo();
	if ( !NewRoot ) return;
	if ( !GetWinRes()->ChangeRootWidget(NewRoot) )
	{
		DBGLOG(_("wxSmith ERROR: Something wrong with undo buffer !!!"));
        wxsKILL(NewRoot);
	}
	SetModified(UndoBuff->IsModified());
}

bool wxsWindowEditor::HasSelection() const
{
    return DragWnd && DragWnd->GetMultipleSelCount();
}

bool wxsWindowEditor::CanPaste() const
{
    if ( !wxTheClipboard->Open() ) return false;
    bool Res = wxTheClipboard->IsSupported(wxsDF_WIDGET);
// FIXME (SpOoN#1#): Add support for text (XRC) data
    wxTheClipboard->Close();
    return Res;
}

void wxsWindowEditor::Cut()
{
    if ( !HasSelection() ) return;

	// Almost all selected widgets will be added into clipboard
	// but with one exception - widget won't be added if parent of this
	// widget at any level is also selected

	std::vector<wxsWidget*> Widgets;
	GetSelectionNoChildren(Widgets);

	if ( !DragWnd ) return;
    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject* Data = new wxsWindowResDataObject;
    int Cnt = (int)Widgets.size();
    for ( int i=0; i<Cnt; i++ )
    {
    	Data->AddWidget(Widgets[i]);
    }
    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();

    // Removing widgets copied into clipboard
    KillPreview();
    for ( int i=0; i<Cnt; i++ )
    {
    	// Can not delete top-most widget
    	if ( Widgets[i]->GetParent() )
    	{
            wxsKILL(Widgets[i]);
    	}
    }
    BuildPreview();
}

void wxsWindowEditor::Copy()
{
    if ( !HasSelection() ) return;

	// Almost all selected widgets will be added into clipboard
	// but with one exception - widget won't be added if parent of this
	// widget at any level is also selected

	std::vector<wxsWidget*> Widgets;
	GetSelectionNoChildren(Widgets);

	if ( !DragWnd ) return;
    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject* Data = new wxsWindowResDataObject;
    int Cnt = (int)Widgets.size();
    for ( int i=0; i<Cnt; i++ )
    {
    	Data->AddWidget(Widgets[i]);
    }
    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();
}

void wxsWindowEditor::Paste()
{
    if ( !CanPaste() ) return;

    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject Data;
    if ( wxTheClipboard->GetData(Data) )
    {
        wxsWidget* RelativeTo = DragWnd->GetSelection();
        int InsertionType = InsType;
        if ( !RelativeTo )
        {
            InsertionType = itInto;
            RelativeTo = GetWinRes()->GetRootWidget();
            if ( RelativeTo->GetChildCount() == 1 &&
                 RelativeTo->GetChild(0)->GetInfo().Sizer )
            {
                RelativeTo = RelativeTo->GetChild(0);
            }
        }

        int Cnt = Data.GetWidgetCount();
        if ( Cnt )
        {
            StartMultipleChange();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Insert = Data.BuildWidget(GetWinRes(),i);
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
            EndMultipleChange();
// FIXME (SpOoN#1#): Updating base properties probably won't work properly
            GetWinRes()->CheckBaseProperties(true,NULL);
            GetWinRes()->NotifyChange();
        }
    }
    wxTheClipboard->Close();
}

bool wxsWindowEditor::StartMultipleChange()
{
	if ( InsideMultipleChange ) return false;
	InsideMultipleChange = true;
    GetWinRes()->GetRootWidget()->StoreCollapsed();
	KillPreview();
	return true;
}

bool wxsWindowEditor::EndMultipleChange()
{
	if ( !InsideMultipleChange ) return false;
	InsideMultipleChange = false;
	BuildPreview();
	GetWinRes()->RebuildTree(wxsTREE());
    GetWinRes()->GetRootWidget()->RestoreCollapsed();
	wxsTREE()->Refresh();
	return true;
}

bool wxsWindowEditor::InsertBefore(wxsWidget* New,wxsWidget* Ref)
{
	if ( !Ref )
	{
        Ref = DragWnd->GetSelection();
	}

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

	if ( !InsideMultipleChange )
	{
		GetWinRes()->GetRootWidget()->StoreCollapsed();
		KillPreview();
	}
    wxsWidget* Parent = Ref->GetParent();

    int Index;
    bool Ret;

    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index) < 0 )
    {
        wxsKILL(New);
        Parent->GetResource()->RebuildTree(wxsTREE());
        Ret = false;
    }
    else
    {
        // Adding this new item into resource tree
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
    	if (Ret && Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadBool(_T("/autoselectwidgets"), true))
            wxsPropertiesMan::Get()->SetActiveWidget(New);
		GetWinRes()->GetRootWidget()->RestoreCollapsed();
    }

    return Ret;
}

bool wxsWindowEditor::InsertAfter(wxsWidget* New,wxsWidget* Ref)
{
	if ( !Ref )
	{
        Ref = DragWnd->GetSelection();
	}

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

	if ( !InsideMultipleChange )
	{
		GetWinRes()->GetRootWidget()->StoreCollapsed();
		KillPreview();
	}
    wxsWidget* Parent = Ref->GetParent();

    int Index;
    bool Ret;

    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index+1) < 0 )
    {
        wxsKILL(New);
        Ret = false;
    }
    else
    {
        // Adding this new item into resource tree
        Parent->GetResource()->RebuildTree(wxsTREE());
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
    	if (Ret && Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadBool(_T("/autoselectwidgets"), true))
            wxsPropertiesMan::Get()->SetActiveWidget(New);
		GetWinRes()->GetRootWidget()->RestoreCollapsed();
    }

    return Ret;
}

bool wxsWindowEditor::InsertInto(wxsWidget* New,wxsWidget* Ref)
{
	if ( !Ref )
	{
        Ref = DragWnd->GetSelection();
	}

	if ( !Ref )
	{
		wxsKILL(New);
		return false;
	}

	if ( !InsideMultipleChange )
	{
		GetWinRes()->GetRootWidget()->StoreCollapsed();
		KillPreview();
	}

	bool Ret;
    if ( Ref->AddChild(New) < 0 )
    {
        wxsKILL(New);
        Ret = false;
    }
    else
    {
        New->GetResource()->RebuildTree(wxsTREE());
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
    	if (Ret && Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadBool(_T("/autoselectwidgets"), true))
    	{
            wxsPropertiesMan::Get()->SetActiveWidget(New);
    	}
		GetWinRes()->GetRootWidget()->RestoreCollapsed();
    }
    return Ret;
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

void wxsWindowEditor::BuildPalette(wxNotebook* Palette)
{
    Palette->DeleteAllPages();

    // First we need to split all widgets into groups
    // it will be done using multimap

    typedef std::multimap<const wxChar*,const wxsWidgetInfo*,ltstr> MapT;
    typedef MapT::iterator MapI;

    MapT Map;

    for ( const wxsWidgetInfo* Info = wxsFACTORY()->GetFirstInfo(); Info; Info = wxsFACTORY()->GetNextInfo() )
        Map.insert(std::pair<const wxChar*,const wxsWidgetInfo*>(Info->Category,Info));

    const wxChar* PreviousGroup = _T("");

    wxScrolledWindow* CurrentPanel = NULL;
    wxSizer* RowSizer = NULL;

    for ( MapI i = Map.begin(); i != Map.end(); ++i )
    {
        if ( !(*i).first || wxStricmp(PreviousGroup,(*i).first) )
        {
            if ( CurrentPanel )
            {
                CurrentPanel->SetSizer(RowSizer);
                RowSizer->SetVirtualSizeHints(CurrentPanel);
            }
            if ( (*i).first && (*i).first[0] )
            {
                // Need to create new tab
                PreviousGroup = (*i).first;
                CurrentPanel = new wxScrolledWindow(Palette,-1,wxDefaultPosition,wxDefaultSize,0/*wxALWAYS_SHOW_SB|wxHSCROLL*/);
                CurrentPanel->SetScrollRate(1,0);
                Palette->AddPage(CurrentPanel,PreviousGroup);
                RowSizer = new wxBoxSizer(wxHORIZONTAL);
            }
            else
            {
                CurrentPanel = NULL;
                RowSizer = NULL;
            }
        }

        if ( CurrentPanel )
        {
            wxBitmap* Icon;

            if ( wxsDWPalIconSize == 16L )
            {
                Icon = (*i).second->Icon16;
            }
            else
            {
                Icon = (*i).second->Icon;
            }

            if ( Icon )
            {
                wxBitmapButton* Btn =
                    new wxBitmapButton(CurrentPanel,-1,*Icon,
                        wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,
                        wxDefaultValidator, (*i).second->Name);
                RowSizer->Add(Btn,0,wxALIGN_CENTER);
                Btn->SetToolTip((*i).second->Name);
            }
            else
            {
                wxButton* Btn = new wxButton(CurrentPanel,-1,(*i).second->Name,
                    wxDefaultPosition,wxDefaultSize,0,
                    wxDefaultValidator,(*i).second->Name);
                RowSizer->Add(Btn,0,wxGROW);
                Btn->SetToolTip((*i).second->Name);
            }
        }
    }

    if ( CurrentPanel )
    {
        CurrentPanel->SetSizer(RowSizer);
        RowSizer->SetVirtualSizeHints(CurrentPanel);
    }
}

void wxsWindowEditor::InsertRequest(const wxString& Name)
{
    wxsWidget* Current = GetSelection();
    if ( Current == NULL )
    {
        DBGLOG(_("wxSmith: No widget selected - couldn't create new widget"));
        return;
    }

    if ( !InsType )
    {
        return;
    }

    wxsWidget* NewWidget = wxsGEN(Name,GetWinRes());
    if ( NewWidget == NULL )
    {
        DBGLOG(_("wxSmith: Culdn't generate widget inside factory"));
        return;
    }

    switch ( InsType )
    {
        case itBefore:
            InsertBefore(NewWidget,Current);
            break;

        case itAfter:
            InsertAfter(NewWidget,Current);
            break;

        case itInto:
            InsertInto(NewWidget,Current);
            break;

        default:
            wxsKILL(NewWidget);
            DBGLOG(_T("wxSmith: Internal error"));
            break;
    }

    BuildPreview();

    if ( GetResource() )
    {
		GetResource()->NotifyChange();
    }
}

void wxsWindowEditor::OnButton(wxCommandEvent& event)
{
    wxWindow* Btn = (wxWindow*)event.GetEventObject();
    if ( Btn )
    {
        InsertRequest(Btn->GetName());
    }
}

inline void wxsWindowEditor::GetSelectionNoChildren(std::vector<wxsWidget*>& Vector)
{
    DragWnd->GetSelectionNoChildren(Vector);
}

inline wxsWidget* wxsWindowEditor::GetSelection()
{
    return DragWnd->GetSelection();
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
    BuildInsTypeIcon(InsIntoBtn,InsIntoImg,(InsType&itInto)!=0,(InsTypeMask&itInto)!=0);
    BuildInsTypeIcon(InsBeforeBtn,InsBeforeImg,(InsType&itBefore)!=0,(InsTypeMask&itBefore)!=0);
    BuildInsTypeIcon(InsAfterBtn,InsAfterImg,(InsType&itAfter)!=0,(InsTypeMask&itAfter)!=0);
}

void wxsWindowEditor::BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,bool Selected,bool Enabled)
{
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
    wxsWidget* Current = GetSelection();
    if ( Current == NULL )
    {
        DBGLOG(_("wxSmith: No widget selecteed - couldn't delete"));
        return;
    }

    wxsWidget* Parent = Current->GetParent();

    if ( !Parent )
    {
        wxMessageBox(_("Can not delete main widget (for now ;)"));
        return;
    }

    wxsBlockSelectEvents(true);
    KillPreview();
    wxsKILL(Current);
    BuildPreview();
    GetResource()->NotifyChange();
    wxsTREE()->Refresh();
    wxsBlockSelectEvents(false);
}

void wxsWindowEditor::OnPreview(wxCommandEvent& event)
{
    if ( GetResource()->IsPreview() )
    {
        GetResource()->HidePreview();
    }
    else
    {
        GetResource()->ShowPreview();
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

void wxsWindowEditor::RebuildQuickProps(wxsWidget* Selection)
{
    Freeze();

    int QPx, QPy;

    QPArea->SetSizer(NULL);
    QPArea->DestroyChildren();
    QPSizer = new wxBoxSizer(wxVERTICAL);
    QPArea->SetSizer(QPSizer);
    QPArea->GetViewStart(&QPx,&QPy);

    if ( Selection )
    {
        wxWindow* QPPanel = Selection->BuildQuickPanel(QPArea);
        if ( QPPanel )
        {
            QPSizer->Add(QPPanel,0,wxEXPAND);
        }
        wxsWidget* Parent = Selection->GetParent();
        if ( Parent )
        {
            wxWindow* QPParentPanel = Parent->BuildChildQuickPanel(QPArea,Parent->FindChild(Selection));
            if ( QPParentPanel )
            {
                QPSizer->Add(QPParentPanel,0,wxEXPAND);
            }
        }
    }
    QPSizer->Layout();
    QPSizer->Fit(QPArea);
    Layout();
    QPArea->Scroll(QPx,QPy);
    Thaw();
}

void wxsWindowEditor::SpreadEvent(wxEvent& event)
{
    for ( std::set<wxsWindowEditor*>::iterator i = AllEditors.begin(); i != AllEditors.end(); ++i )
    {
        (*i)->ProcessEvent(event);
    }
}

void wxsWindowEditor::OnSize(wxSizeEvent& event)
{
    event.Skip();

    wxCommandEvent event2(wxsEVT_RELAYOUT,GetId());
    event2.SetEventObject(this);
    AddPendingEvent(event2);
}

void wxsWindowEditor::OnRelayout(wxCommandEvent& event)
{
    Layout();
    Refresh();
}

//void wxsWindowEditor::OnContentScroll(wxScrollEvent& event)
//{
//    if ( DragWnd )
//    {
//        DragWnd->RebuildBackground();
//        DragWnd->Refresh();
//    }
//    event.Skip();
//}
//
//void wxsWindowEditor::OnContentSize(wxSizeEvent& event)
//{
//    if ( DragWnd )
//    {
//        DragWnd->RebuildBackground();
////        DragWnd->Refresh();
//    }
//    event.Skip();
//}
//
wxImage wxsWindowEditor::InsIntoImg;
wxImage wxsWindowEditor::InsBeforeImg;
wxImage wxsWindowEditor::InsAfterImg;
wxImage wxsWindowEditor::DelImg;
wxImage wxsWindowEditor::PreviewImg;
wxImage wxsWindowEditor::QuickPropsImgOpen;
wxImage wxsWindowEditor::QuickPropsImgClose;
wxImage wxsWindowEditor::SelectedImg;
std::set<wxsWindowEditor*> wxsWindowEditor::AllEditors;
bool wxsWindowEditor::ImagesLoaded = false;

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    //EVT_LEFT_DOWN(wxsWindowEditor::OnMouseClick)
    EVT_SELECT_WIDGET(wxsWindowEditor::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsWindowEditor::OnUnselectWidget)
    EVT_BUTTON(wxsInsIntoId,wxsWindowEditor::OnInsInto)
    EVT_BUTTON(wxsInsBeforeId,wxsWindowEditor::OnInsBefore)
    EVT_BUTTON(wxsInsAfterId,wxsWindowEditor::OnInsAfter)
    EVT_BUTTON(wxsDelId,wxsWindowEditor::OnDelete)
    EVT_BUTTON(wxsPreviewId,wxsWindowEditor::OnPreview)
    EVT_BUTTON(wxsQuickPropsId,wxsWindowEditor::OnQuickProps)
    EVT_BUTTON(-1,wxsWindowEditor::OnButton)
    EVT_SIZE(wxsWindowEditor::OnSize)
    EVT_COMMAND(-1,wxsEVT_RELAYOUT,wxsWindowEditor::OnRelayout)
//    EVT_COMMAND_SCROLL(wxsScrollId,wxsWindowEditor::OnContentScroll)
END_EVENT_TABLE()
