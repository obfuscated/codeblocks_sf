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

wxsWindowEditor::wxsWindowEditor(wxWindow* parent,wxsWindowRes* Resource):
    wxsEditor(parent,Resource->GetWxsFile(),Resource),
    UndoBuff(new wxsWinUndoBuffer(Resource))
{
    wxSizer* Sizer = new wxBoxSizer(wxVERTICAL);

    Scroll = new wxScrolledWindow(this);
    Scroll->SetScrollRate(4,4);

    Sizer->Add(Scroll,1,wxGROW);
    Scroll->SetScrollRate(4,4);

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    Scroll->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

    SetSizer(Sizer);

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
        Scroll->SetVirtualSizeHints(1,1);
        Scroll->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(Scroll);
        Layout();
        wxSize Virtual = Scroll->GetVirtualSize();
        wxSize Real = Scroll->GetSize();
        wxSize Drag(Virtual.GetWidth() > Real.GetWidth() ? Virtual.GetWidth() : Real.GetWidth(),
                    Virtual.GetHeight() > Real.GetHeight() ? Virtual.GetHeight() : Real.GetHeight());
        // Waiting to reposition and resize all widgets
        ::wxYield();
        DragWnd->SetSize(Drag);
        DragWnd->NotifySizeChange(Drag);
        DragWnd->SetWidget(TopWidget);
        DragWnd->Show();
    }

    Thaw();
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
		wxsWidgetFactory::Get()->Kill(NewRoot);
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
        wxsWidgetFactory::Get()->Kill(NewRoot);
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
            wxsWidgetFactory::Get()->Kill(Widgets[i]);
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
}

void wxsWindowEditor::GetSelectionNoChildren(std::vector<wxsWidget*>& Vector)
{
	Vector.clear();
	int Cnt = DragWnd->GetMultipleSelCount();
	for ( int i=0; i<Cnt; i++ )
	{
		Vector.push_back(DragWnd->GetMultipleSelWidget(i));
	}
	
	for ( int i=0; i<Cnt; i++ )
	{
		for ( int j=0; j<Cnt; j++ )
		{
			// Yes, I know it could be O(n) ;)
			wxsWidget* Check = Vector[i];
			while ( Check = Check->GetParent() )
			{
				if ( Check == Vector[j] ) break;
			}
			if ( Check )
			{
                // i-th vector item has selected (grand)parent, it must be removed
                Vector.erase(Vector.begin()+i);
                --i;
                --Cnt;
                break;
            }
		}
	}
}


BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_LEFT_DOWN(wxsWindowEditor::OnMouseClick)
    EVT_SELECT_WIDGET(wxsWindowEditor::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsWindowEditor::OnUnselectWidget)
END_EVENT_TABLE()
