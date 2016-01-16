#ifndef BROWSEMARKERDEFS_H_INCLUDED
#define BROWSEMARKERDEFS_H_INCLUDED

//svn5785 #include "wxscintilla/include/wx/wxscintilla.h"
#include <wx/wxscintilla.h>

    const int MaxEntries = 20;

    // from cbeditor.cpp
    #define BOOKMARK_MARKER        4
    #define BOOKMARK_STYLE         wxSCI_MARK_ARROW

    const int BROWSETRACKER_MARKER       = 9 ;
    const int BROWSETRACKER_MARKER_STYLE = wxSCI_MARK_DOTDOTDOT ;
    const int BROWSETRACKER_HIDDEN_STYLE = wxSCI_MARK_EMPTY ;
    const int BROWSETRACKER_MARKER_MASK  = 1<<BROWSETRACKER_MARKER ;

class EditorBase;
class BrowseMarks;
class ProjectData;
class cbProject;
// ----------------------------------------------------------------------------
// active editors
WX_DEFINE_ARRAY_PTR(EditorBase*, ArrayOfEditorBasePtrs);
// a hash containing pointers to cursor positions indexed by EditorBase pointers.
WX_DECLARE_HASH_MAP(EditorBase*, BrowseMarks*, wxPointerHash, wxPointerEqual, EbBrowse_MarksHash);
// ProjectData class pointers associating files to a loaded project
WX_DECLARE_HASH_MAP(cbProject*, ProjectData*, wxPointerHash, wxPointerEqual, ProjectDataHash);
// ----------------------------------------------------------------------------
// hashmap for fast searches: filename, BrowseMark*
WX_DECLARE_STRING_HASH_MAP(BrowseMarks*, FileBrowse_MarksHash);

    enum{
            Left_Mouse = 0,
            Ctrl_Left_Mouse = 1,
            ClearAllOnSingleClick = 0,
            ClearAllOnDoubleClick = 1,

            BrowseMarksStyle  = 0,
            BookMarksStyle   = 1,
            HiddenMarksStyle = 2

        };


#endif // BROWSEMARKERDEFS_H_INCLUDED
