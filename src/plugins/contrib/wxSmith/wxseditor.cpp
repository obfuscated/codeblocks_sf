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

bool wxsEditor::Close()
{
    Unbind();
    Destroy();
    return true;
}

void wxsEditor::Unbind()
{
    if ( Resource )
    {
        MyUnbind();
        wxsResource* ResStore = Resource;
        Resource = NULL;
        ResStore->EditorSaysHeIsClosing();
    }
}

void wxsEditor::OnClose(wxCloseEvent& event)
{
    if ( !QueryClose() || ! Close() ) event.Veto();
}

BEGIN_EVENT_TABLE(wxsEditor,EditorBase)
    EVT_CLOSE(wxsEditor::OnClose)
END_EVENT_TABLE()
