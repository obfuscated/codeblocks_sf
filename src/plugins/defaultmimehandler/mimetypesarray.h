/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef MIMETYPESARRAY_H
#define MIMETYPESARRAY_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dynarray.h>

struct cbMimeType
{
    // e.g. "*.xrc" - case insensitive
    wxString wildcard;

    // e.g. "C:\Tools\XRCed.exe $(FILE)" - program to open this file
    // $(FILE) is substituted by the file in question
    // used only if useEditor == false
    wxString program;

    // if true, open it in the Code::Blocks editor, else use "program" (or associated app)
    bool useEditor;
    
    // if true, open it with the associated app
    bool useAssoc;

    // should the IDE be disabled while working on this file?
    // valid only for external programs...
    bool programIsModal;
};
WX_DEFINE_ARRAY(cbMimeType*, MimeTypesArray);

#endif // MIMETYPESARRAY_H
