#include "wxswindoweditor.h"

#include "widget.h"
#include <wx/settings.h>
#include <wx/scrolwin.h>
#include "defwidgets/wxsdialog.h"

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
}

wxsWindowEditor::~wxsWindowEditor()
{
	KillCurrentPreview();
}

void wxsWindowEditor::BuildPreview(wxsWidget* TopWidget)
{
    SetSizer(NULL);
    KillCurrentPreview();

    // Creating new sizer

    wxWindow* TopPreviewWindow = TopWidget ? TopWidget->CreatePreview(DrawArea,this) : NULL;
    CurrentWidget = TopWidget;
    
    if ( TopPreviewWindow )
    {
        wxSizer* NewSizer = new wxGridSizer(1);
        NewSizer->Add(TopPreviewWindow,0,wxALIGN_CENTRE_VERTICAL|wxALIGN_CENTRE_HORIZONTAL|wxALL,10);
        DrawArea->SetSizer(NewSizer);
        DrawArea->Layout();
        NewSizer->SetVirtualSizeHints(DrawArea);
        TopPreviewWindow->Refresh();
    }
}

void wxsWindowEditor::KillCurrentPreview()
{
    if ( CurrentWidget ) CurrentWidget->KillPreview();
    CurrentWidget = NULL;
   
}

void wxsWindowEditor::OnMouseClick(wxMouseEvent& event)
{
    if ( CurrentWidget && CurrentWidget->GetPreview() )
    {
        CurrentWidget->GetPreview()->ProcessEvent(event);
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

BEGIN_EVENT_TABLE(wxsWindowEditor,wxsEditor)
    EVT_LEFT_DOWN(wxsWindowEditor::OnMouseClick)
END_EVENT_TABLE()
