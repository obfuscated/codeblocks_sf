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
class wxBitmapButton;
class wxImageList;

class SearchInPanel: public wxPanel {
public:
    // begin wxGlade: SearchInPanel::ids
    // end wxGlade

    /** Constructor. */
    SearchInPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

    // Getters
    bool GetSearchInOpenFiles()      const;
    bool GetSearchInTargetFiles()    const;
    bool GetSearchInProjectFiles()   const;
    bool GetSearchInWorkspaceFiles() const;
    bool GetSearchInDirectory()      const;

    // Setters
    void SetSearchInOpenFiles     (bool bSearchInOpenFiles);
    void SetSearchInTargetFiles   (bool bSearchInTargetFiles);
    void SetSearchInProjectFiles  (bool bSearchInProjectFiles);
    void SetSearchInWorkspaceFiles(bool bSearchInWorkspaceFiles);
    void SetSearchInDirectory     (bool bSearchInDirectory);

private:
    // begin wxGlade: SearchInPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: SearchInPanel::attributes
    wxBitmapButton* m_pBtnSearchOpenFiles;
    wxBitmapButton* m_pBtnSearchTargetFiles;
    wxBitmapButton* m_pBtnSearchProjectFiles;
    wxBitmapButton* m_pBtnSearchWorkspaceFiles;
    wxBitmapButton* m_pBtnSearchDir;
    // end wxGlade

    /** @brief Buttonstates
     *
     * Since wxBitmapButton cannot toggle like wxCheckBox,
     * the "state" must be saved in extra variables
     * @{
     */
    bool m_StateSearchOpenFiles;
    bool m_StateSearchTargetFiles;
    bool m_StateSearchProjectFiles;
    bool m_StateSearchWorkspaceFiles;
    bool m_StateSearchDirectoryFiles;
    /** @} */

    /** @brief Imagelist for the search-in buttons
     *
     * The imagelist makes it possible to use images without
     * calculating their name when they change
     */
    wxImageList* m_pButtonImages;

    DECLARE_EVENT_TABLE();

public:
    /** Gets all buttons click events to forward them to the parent window..
      */
    void OnBtnClickEvent(wxCommandEvent &event); // wxGlade: <event_handler>

    // The 3 following methods work together. It offers a kind of
    // radio boxes group. They make impossible to select both workspace,
    // project and target buttons (it is stupid to search both in project
    // and workspace because project is already included in workpsace).

    /** @brief Event for scope button is clicked (target scope)
     *
     * Scope for project and workspace are switched off.
     */
    void OnBtnSearchTargetFilesClick(wxCommandEvent &event); // wxGlade: <event_handler>

    /** @brief Event for scope button is clicked (project scope)
     *
     * Scope for target and workspace are switched off.
     */
    void OnBtnSearchProjectFilesClick(wxCommandEvent &event); // wxGlade: <event_handler>

    /** @brief Event for scope button is clicked (workspace scope)
     *
     * Scope for target and project are switched off.
     */
    void OnBtnSearchWorkspaceFilesClick(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // SEARCH_IN_PANEL_H
