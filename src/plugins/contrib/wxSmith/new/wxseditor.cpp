#include "wxseditor.h"
#include "wxsresource.h"

#include <wx/wx.h>

wxsEditor::wxsEditor(wxWindow* parent, const wxString& title,wxsResource* Resource):
    EditorBase(parent,title),
    m_Resource(Resource)
{
}

wxsEditor::~wxsEditor()
{
    if ( m_Resource )
    {
        m_Resource->EditorClosed();
        m_Resource = NULL;
    }
}
