#include "wxsresource.h"

wxsResource::wxsResource(wxSmith* _Plugin):  Editor(NULL), Plugin(_Plugin)
{
}

wxsResource::~wxsResource()
{
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
    if ( Editor->QueryClose() ) Editor->Close();
}

void wxsResource::EditorSaysHeIsClosing()
{
    assert ( Editor != NULL );
    assert ( Editor->GetResource() == NULL || Editor->GetResource() == this );
    Editor = NULL;
}
