#include "wxsheaders.h"
#include "wxswindoweditor.h"

#include "widget.h"
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include <wx/clipbrd.h>
#include "wxspropertiesman.h"
#include "wxspalette.h"
#include "wxsmith.h"
#include "wxsresource.h"
#include "wxsdragwindow.h"
#include "resources/wxswindowres.h"
#include "resources/wxswindowresdataobject.h"
#include "wxswinundobuffer.h"
#include "wxswidgetfactory.h"

namespace {
struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStricmp(s1, s2) < 0; } };
};

static const long wxsInsIntoId    = wxNewId();
static const long wxsInsBeforeId  = wxNewId();
static const long wxsInsAfterId   = wxNewId();
static const long wxsDelId        = wxNewId();
static const long wxsPreviewId    = wxNewId();
static const long wxsQuickPropsId = wxNewId();


wxsWindowEditor::wxsWindowEditor(wxWindow* parent,wxsWindowRes* Resource):
    wxsEditor(parent,Resource->GetWxsFile(),Resource),
    UndoBuff(new wxsWinUndoBuffer(Resource)),
    InsideMultipleChange(false)
{
    InitializeImages();

    VertSizer = new wxBoxSizer(wxVERTICAL);
    WidgetsSet = new wxNotebook(this,-1);
    BuildPalette(WidgetsSet);
    VertSizer->Add(WidgetsSet,0,wxEXPAND);
    HorizSizer = new wxBoxSizer(wxHORIZONTAL);
    VertSizer->Add(HorizSizer,1,wxEXPAND);

    Scroll = new wxScrolledWindow(this);
    Scroll = new wxScrolledWindow(this);
    Scroll->SetScrollRate(4,4);
    Scroll->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    HorizSizer->Add(Scroll,1,wxEXPAND);

    QPSizer = new wxBoxSizer(wxVERTICAL);
    HorizSizer->Add(QPSizer,0,wxEXPAND);

    OpsSizer = new wxBoxSizer(wxVERTICAL);
    HorizSizer->Add(OpsSizer,0,wxEXPAND);

    OpsSizer->Add(new wxBitmapButton(this,wxsInsIntoId,InsIntoImg));
    OpsSizer->Add(new wxBitmapButton(this,wxsInsBeforeId,InsBeforeImg));
    OpsSizer->Add(new wxBitmapButton(this,wxsInsAfterId,InsAfterImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(new wxBitmapButton(this,wxsDelId,DelImg));
    OpsSizer->Add(new wxBitmapButton(this,wxsPreviewId,PreviewImg));
    OpsSizer->Add(1,5);
    OpsSizer->Add(new wxBitmapButton(this,wxsQuickPropsId,QuickPropsImgOpen));

    SetSizer(VertSizer);

    DragWnd = new wxsDragWindow(Scroll,NULL,Scroll->GetSize());
    DragWnd->Hide();
    wxFileName Name(Resource->GetWxsFile());
    SetTitle(Name.GetFullName());

    // Storing current resource data as a base for undo buffer
    UndoBuff->StoreChange();
    UndoBuff->Saved();
}

wxsWindowEditor::~wxsWindowEditor()
{
	wxsUnselectRes(GetResource());
	KillPreview();
	delete UndoBuff;
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
    Scroll->SetSizer(NULL);
    Freeze();

    KillPreview();

    // Creating new sizer

    wxsWidget* TopWidget = GetWinRes()->GetRootWidget();
    wxWindow* TopPreviewWindow = TopWidget ? TopWidget->CreatePreview(Scroll,this) : NULL;

    if ( TopPreviewWindow )
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(TopPreviewWindow,0,/*wxALIGN_CENTRE_VERTICAL|wxALIGN_CENTRE_HORIZONTAL|*/wxALL,10);
        //Scroll->SetVirtualSizeHints(1,1);
        Scroll->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(Scroll);
        HorizSizer->Layout();
        VertSizer->Layout();
        wxSize Virtual = Scroll->GetVirtualSize();
        wxSize Real = Scroll->GetSize();
        wxSize Drag(Virtual.GetWidth() > Real.GetWidth() ? Virtual.GetWidth() : Real.GetWidth(),
                    Virtual.GetHeight() > Real.GetHeight() ? Virtual.GetHeight() : Real.GetHeight());
        // Waiting to reposition and resize all widgets
// FIXME (SpOoN#1#): Don't ever use wxYield, just add pending event and do all required stuff in it's handler
        ::wxYield();
        DragWnd->SetSize(Drag);
        DragWnd->NotifySizeChange(Drag);
        DragWnd->SetWidget(TopWidget);
        DragWnd->Show();
    }
    Layout();

    Thaw();
    Refresh();
}

void wxsWindowEditor::KillPreview()
{
    Scroll->SetSizer(NULL);
    GetWinRes()->GetRootWidget()->KillPreview();
    DragWnd->Hide();
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
}

void wxsWindowEditor::OnUnselectWidget(wxsEvent& event)
{
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
	wxsWidget* NewRoot = UndoBuff->Undo();
	if ( !NewRoot ) return;
	if ( !GetWinRes()->ChangeRootWidget(NewRoot) )
	{
		DebLog(_("wxSmith ERROR: Something wrong with undo buffer !!!"));
		wxsKILL(NewRoot);
	}
	SetModified(UndoBuff->IsModified());
}

void wxsWindowEditor::Redo()
{
	wxsWidget* NewRoot = UndoBuff->Redo();
	if ( !NewRoot ) return;
	if ( !GetWinRes()->ChangeRootWidget(NewRoot) )
	{
		DebLog(_("wxSmith ERROR: Something wrong with undo buffer !!!"));
        wxsKILL(NewRoot);
	}
	SetModified(UndoBuff->IsModified());
}

bool wxsWindowEditor::CanCut()
{
    return DragWnd && DragWnd->GetMultipleSelCount();
}

bool wxsWindowEditor::CanCopy()
{
    return DragWnd && DragWnd->GetMultipleSelCount();
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
    if ( !wxTheClipboard->Open() ) return;
    wxsWindowResDataObject Data;
    if ( wxTheClipboard->GetData(Data) )
    {
        wxsWidget* RelativeTo = DragWnd->GetSelection();
        int InsertionType = wxsPalette::Get()->GetInsertionType();
        if ( !RelativeTo )
        {
            InsertionType = wxsPalette::itInto;
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
                        case wxsPalette::itAfter:
                            InsertAfter(Insert,RelativeTo);
                            RelativeTo = Insert;
                            break;

                        case wxsPalette::itBefore:
                            InsertBefore(Insert,RelativeTo);
                            break;

                        case wxsPalette::itInto:
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

void wxsWindowEditor::GetSelectionNoChildren(std::vector<wxsWidget*>& Vector)
{
    DragWnd->GetSelectionNoChildren(Vector);
}

bool wxsWindowEditor::StartMultipleChange()
{
	if ( InsideMultipleChange ) return false;
	InsideMultipleChange = true;
	KillPreview();
	return true;
}

bool wxsWindowEditor::EndMultipleChange()
{
	if ( !InsideMultipleChange ) return false;
	InsideMultipleChange = false;
	BuildPreview();
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
		KillPreview();
	}
    wxsWidget* Parent = Ref->GetParent();

    int Index;
    bool Ret;

    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index) < 0 )
    {
        wxsKILL(New);
        Ret = false;
    }
    else
    {
        // Adding this new item into resource tree
        New->BuildTree(wxsTREE(),Parent->GetTreeId(),Index);
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
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
        New->BuildTree(wxsTREE(),Parent->GetTreeId(),Index+1);
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
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
        New->BuildTree(wxsTREE(),Ref->GetTreeId());
        Ret = true;
    }

    if ( !InsideMultipleChange )
    {
    	wxsTREE()->Refresh();
    	BuildPreview();
    }
    return Ret;
}

void wxsWindowEditor::InitializeImages()
{
    wxString basePath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString) + _T("/images/wxsmith/");

    static const wxString NormalNames[] =
    {
        _T("insertinto.png"),
        _T("insertafter.png"),
        _T("insertbefore.png"),
        _T("deletewidget.png"),
        _T("showpreview.png"),
        _T("quickpropsopen.png"),
        _T("quickpropsclose.png")
    };

    static const wxString SmallNames[] =
    {
        _T("insertinto16.png"),
        _T("insertafter16.png"),
        _T("insertbefore16.png"),
        _T("deletewidget16.png"),
        _T("showpreview16.png"),
        _T("quickpropsopen16.png"),
        _T("quickpropsclose16.png")
    };

    const wxString* Array = /*( wxsDWPalIconSize == 16L ) ? SmallNames : */ NormalNames;

    InsIntoImg.LoadFile(basePath + Array[0]);
    InsBeforeImg.LoadFile(basePath + Array[1]);
    InsAfterImg.LoadFile(basePath + Array[2]);
    DelImg.LoadFile(basePath + Array[3]);
    PreviewImg.LoadFile(basePath + Array[4]);
    QuickPropsImgOpen.LoadFile(basePath + Array[5]);
    QuickPropsImgClose.LoadFile(basePath + Array[6]);
}

void wxsWindowEditor::BuildPalette(wxNotebook* Palette)
{
    // First we need to split all widgets into groups
    // it will be done using multimap

    typedef std::multimap<const wxChar*,const wxsWidgetInfo*,ltstr> MapT;
    typedef MapT::iterator MapI;

    MapT Map;

    for ( const wxsWidgetInfo* Info = wxsFACTORY()->GetFirstInfo(); Info; Info = wxsFACTORY()->GetNextInfo() )
        Map.insert(std::pair<const wxChar*,const wxsWidgetInfo*>(Info->Category,Info));

    const wxChar* PreviousGroup = _T("");

    wxPanel* CurrentPanel = NULL;
    wxSizer* RowSizer = NULL;

    for ( MapI i = Map.begin(); i != Map.end(); ++i )
    {
        if ( !(*i).first || wxStricmp(PreviousGroup,(*i).first) )
        {
            if ( CurrentPanel )
            {
                CurrentPanel->SetSizer(RowSizer);
            }
            if ( (*i).first && (*i).first[0] )
            {
                // Need to create new tab
                PreviousGroup = (*i).first;
                CurrentPanel = new wxPanel(Palette);
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
                RowSizer->Add(Btn,0,wxGROW);
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
    }
}

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_LEFT_DOWN(wxsWindowEditor::OnMouseClick)
    EVT_SELECT_WIDGET(wxsWindowEditor::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsWindowEditor::OnUnselectWidget)
END_EVENT_TABLE()
