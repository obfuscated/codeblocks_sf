/***************************************************************
 * Name:      CodeSnippetsEvent
 *
 * Purpose:   Assorted utility routines
 *
 * Created:   2008/4/01
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/
#include "csutils.h"


#include <wx/window.h>
#include <wx/string.h>
#include "version.h"

// ----------------------------------------------------------------------------
Utils::Utils()
// ----------------------------------------------------------------------------
{
    //ctor
}
// ----------------------------------------------------------------------------
Utils::~Utils()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
wxWindow* Utils::FindWindowRecursively(const wxWindow* parent, const wxString& partialLabel)
// ----------------------------------------------------------------------------
{
    if ( parent )
    {
        #if defined(LOGGING)
            //LOGIT( _T("Parent[%p]Label[%s]Name[%s]"), parent, parent->GetLabel().c_str(), parent->GetName().c_str() );
        #endif
        // see if this is the one we're looking for
        if ( parent->GetLabel().Matches(partialLabel) )
            return (wxWindow *)parent;
        if ( parent->GetName().Matches(partialLabel) )
            return (wxWindow *)parent;

        // It wasn't, so check all its children
        for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
              node; node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = FindWindowRecursively(win, partialLabel);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}
