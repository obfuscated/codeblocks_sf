#include "wxswindoweditor.h"

#include "widget.h"
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include "wxspropertiesman.h"
#include "wxspalette.h"

wxsWindowEditor::wxsWindowEditor(wxMDIParentFrame* parent, const wxString& title,wxsResource* Resource):
    wxsEditor(parent,title,Resource),
    CurrentWidget(NULL)
{
    DrawArea = new wxScrolledWindow(this);
    wxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
    Sizer->Add(DrawArea,0,wxGROW);
    SetSizer(Sizer);
    DrawArea->SetScrollRate(4,4);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    DrawArea->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    wxsPalette::Get()->SelectResource(GetResource());
}

wxsWindowEditor::~wxsWindowEditor()
{
	KillCurrentPreview();
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

void wxsWindowEditor::BuildPreview(wxsWidget* TopWidget)
{
    SetSizer(NULL);
    
    Freeze();
    
    KillCurrentPreview();

    // Creating new sizer

    wxWindow* TopPreviewWindow = TopWidget ? TopWidget->CreatePreview(DrawArea,this) : NULL;
    CurrentWidget = TopWidget;
    
    if ( TopPreviewWindow )
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(TopPreviewWindow,0,wxALIGN_CENTRE_VERTICAL|wxALIGN_CENTRE_HORIZONTAL|wxALL,10);
        DrawArea->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(DrawArea);
        TopPreviewWindow->Refresh();
    }
    
    Thaw();
    WidgetRefreshReq(this);
}

void wxsWindowEditor::KillCurrentPreview()
{
    if ( CurrentWidget ) CurrentWidget->KillPreview();
    CurrentWidget = NULL;
   
}

void wxsWindowEditor::OnMouseClick(wxMouseEvent& event)
{
    if ( CurrentWidget )
    {
        wxsPropertiesMan::Get()->SetActiveWidget(CurrentWidget);
    }
}

void wxsWindowEditor::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() )
    {
        wxsPalette::Get()->SelectResource(GetResource());
    }
}

void wxsWindowEditor::PreviewReshaped()
{
    SetSizer(NULL);
    
    if ( CurrentWidget && CurrentWidget->GetPreview() )
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(CurrentWidget->GetPreview(),0,wxALIGN_CENTRE_VERTICAL|wxALIGN_CENTRE_HORIZONTAL);
        SetSizer(NewSizer);
        Layout();
        CurrentWidget->GetPreview()->Refresh();
    }
}

void wxsWindowEditor::MyUnbind()
{
    KillCurrentPreview();
}

void wxsWindowEditor::OnClose(wxCloseEvent& event)
{
    wxsPalette::Get()->ResourceClosed(GetResource());
    event.Skip();
}


BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_LEFT_DOWN(wxsWindowEditor::OnMouseClick)
    EVT_ACTIVATE(wxsWindowEditor::OnActivate)
    EVT_CLOSE(wxsWindowEditor::OnClose)
END_EVENT_TABLE()
