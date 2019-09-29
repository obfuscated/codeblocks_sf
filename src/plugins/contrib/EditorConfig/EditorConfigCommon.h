#ifndef EDITORCONFIGCOMMON_H
#define EDITORCONFIGCOMMON_H

#include <wx/event.h>
#include <wx/object.h>

#include <cbproject.h>

struct EditorSettings
{
    EditorSettings() : active(false) {}
    bool active;
    bool use_tabs;
    bool tab_indents;
    int  tab_width;
    int  indent;
    int  eol_mode;
};

#endif // EDITORCONFIGCOMMON_H
