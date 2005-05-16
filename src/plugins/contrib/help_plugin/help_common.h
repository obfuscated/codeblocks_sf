#ifndef HELP_COMMON_H
#define HELP_COMMON_H

#include <wx/hashmap.h>

/* A hashmap is defined in this file.
 * This is equivalent to STL's std::map<wxString, wxString>
 *
 * The key is the help file's description and the value is the actual help file
 */
WX_DECLARE_STRING_HASH_MAP(wxString, HelpFilesMap);
// the map index of the default file (F1 and Shift-F1 shortcuts are assigned to it automatically)
extern int g_DefaultHelpIndex;

// auxiliary functions to load/save the hashmap
void LoadHelpFilesMap(HelpFilesMap& map);
void SaveHelpFilesMap(HelpFilesMap& map);

#endif // HELP_COMMON_H
