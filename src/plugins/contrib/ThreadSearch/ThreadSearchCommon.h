#ifndef THREAD_SEARCH_COMMON_H
#define THREAD_SEARCH_COMMON_H

#include <wx/string.h>

class wxComboBox;

/// @return A string with the path to the icon images, it takes into account the size of the images.
wxString GetImagePrefix(bool toolbar, wxWindow *window = nullptr);

/// Setups the min/max size of the window in terms of characters.
void SetWindowMinMaxSize(wxWindow &window, int numChars, int minSize);

void AddItemToCombo(wxComboBox *combo, const wxString &str);

#endif // THREAD_SEARCH_COMMON_H
