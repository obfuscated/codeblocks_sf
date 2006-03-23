#include "wxsresource.h"
#include "wxsproject.h"
#include "wxseditor.h"

#include <wx/msgdlg.h>

wxsResource::wxsResource(wxsProject* _Project): Editor(NULL), Project(_Project)
{
}

wxsResource::~wxsResource()
{
    EditClose();
}

void wxsResource::EditOpen()
{
    if ( !Editor )
    {
        Editor = CreateEditor();
    }

    if ( Editor )
    {
        assert ( Editor->GetResource() == this );
        Editor->Activate();
    }
}

void wxsResource::EditClose()
{
    if ( !Editor ) return;
    Editor->Close();
}

void wxsResource::EditorSaysHeIsClosing()
{
    assert ( Editor != NULL );
    Editor = NULL;
    EditorClosed();
}
