#include "wxsheaders.h"
#include "wxseditor.h"

#include "wxsresource.h"

#include <wx/wx.h>

wxsEditor::wxsEditor(wxWindow* parent, const wxString& title,wxsResource* _Resource):
    EditorBase(parent,title),
    Resource(_Resource)
{
}

wxsEditor::~wxsEditor()
{
    Unbind();
}

void wxsEditor::Unbind()
{
    if ( Resource )
    {
        //MyUnbind();       // Can not call this since we're inside destructor
        wxsResource* ResStore = Resource;
        Resource = NULL;
        ResStore->EditorSaysHeIsClosing();
    }
}

void wxsEditor::OnSmithEvent(wxsEvent& event)
{
}

BEGIN_EVENT_TABLE(wxsEditor,EditorBase)
    EVT_SELECT_RES(wxsEditor::OnSmithEvent)
    EVT_UNSELECT_RES(wxsEditor::OnSmithEvent)
    EVT_SELECT_WIDGET(wxsEditor::OnSmithEvent)
    EVT_UNSELECT_WIDGET(wxsEditor::OnSmithEvent)
END_EVENT_TABLE()
