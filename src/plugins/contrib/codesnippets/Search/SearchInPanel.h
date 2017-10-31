/***************************************************************
 * Name:      SearchInPanel
 *
 * Purpose:   This class is a panel that allows the user to
 *            define the search scope : open files, project,
 *            worspace or directory.
 *            It is used in the ThreadSearchView and the
 *            ThreadSearchConfPanel.
 *            It does nothing but forwarding events to the
 *            parent window.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef SEARCH_IN_PANEL_H
#define SEARCH_IN_PANEL_H

// begin wxGlade: ::dependencies
// end wxGlade

#include <wx/panel.h>

class wxWindow;
class wxCheckBox;
class wxCommandEvent;


class SearchInPanel: public wxPanel {
public:
    // begin wxGlade: SearchInPanel::ids
    // end wxGlade

	/** Constructor. */
    SearchInPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

	// Getters
    bool GetSearchInOpenFiles()      const;
    bool GetSearchInSnippetFiles()   const;
    bool GetSearchInWorkspaceFiles() const;
    bool GetSearchInDirectory()      const;

	// Setters
    void SetSearchInOpenFiles     (bool bSearchInOpenFiles);
    void SetSearchInSnippetFiles  (bool bSearchInSnippetFiles);
    void SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles);
    void SetSearchInDirectory     (bool bSearchInDirectory);

private:
    // begin wxGlade: SearchInPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: SearchInPanel::attributes
    wxCheckBox* m_pChkSearchOpenFiles;
    wxCheckBox* m_pChkSearchSnippetFiles;
    //-wxCheckBox* m_pChkSearchWorkspaceFiles;
    wxCheckBox* m_pChkSearchDir;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
	/** Gets all checkboxes click events to forward them to
	  * the parent window..
	  */
    void OnChkClickEvent(wxCommandEvent &event); // wxGlade: <event_handler>

    // The 2 following methods work together. It offers a kind of
    // radio boxes group. They make impossible to select both workspace and
    // project checkboxes (it is stupid to search both in project and
    // workspace because project is already included in workpsace).

	/** If project checkbox is checked, we uncheck if necessary the workspace checkbox.
	  */
    void OnChkSearchSnippetFilesClick(wxCommandEvent &event); // wxGlade: <event_handler>

	/** If workspace checkbox is checked, we uncheck if necessary the project checkbox.
	  */
    void OnChkSearchWorkspaceFilesClick(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // SEARCH_IN_PANEL_H
