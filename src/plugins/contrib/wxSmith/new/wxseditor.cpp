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
    if ( Resource )
    {
        wxsResource* ResStore = Resource;
        Resource = NULL;
        ResStore->EditorSaysHeIsClosing();
    }
}
