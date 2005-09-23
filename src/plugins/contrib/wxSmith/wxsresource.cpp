#include "wxsresource.h"

#include "wxsproject.h"

#include <wx/msgdlg.h>

wxsResource::wxsResource(wxsProject* _Project,int _EditMode):  
    Editor(NULL), 
    Project(_Project),
    EditMode(_EditMode)
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
    assert ( Editor->GetResource() == NULL || Editor->GetResource() == this );
    Editor = NULL;
    EditorClosed();
}
