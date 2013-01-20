/***************************************************************
 * Name:      DirectoryParamsPanel
 *
 * Purpose:   This class is a panel that allows the user to
 *            set the directory search parameters (dir,
 *            extensions...).
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

#ifndef DIRECTORY_PARAMS_PANEL_H
#define DIRECTORY_PARAMS_PANEL_H

// begin wxGlade: ::dependencies
// end wxGlade

#include <wx/string.h>
#include <wx/panel.h>

class wxWindow;
class wxButton;
class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class ThreadSearchFindData;

class DirectoryParamsPanel: public wxPanel {
public:
    // begin wxGlade: DirectoryParamsPanel::ids
    // end wxGlade

    /** Constructor. */
    DirectoryParamsPanel(ThreadSearchFindData *findData, wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition,
                         const wxSize& size=wxDefaultSize, long style=0);

    // Getters
    wxString GetSearchDirPath()        const;
    bool     GetSearchDirRecursively() const;
    bool     GetSearchDirHidden()      const;
    wxString GetSearchMask()           const;

    // Setters
    void     SetSearchDirPath(const wxString& sDirPath);
    void     SetSearchDirRecursively(bool bRecurse);
    void     SetSearchDirHidden(bool bSearchHidden);
    void     SetSearchMask(const wxString& sMask);

    void SetSearchHistory(const wxArrayString& searchDirs, const wxArrayString& searchMasks);
    wxArrayString GetSearchDirsHistory() const;
    wxArrayString GetSearchMasksHistory() const;

    void AddExpressionToCombos(const wxString& path, const wxString& mask);

private:
    // begin wxGlade: DirectoryParamsPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: DirectoryParamsPanel::attributes
    wxComboBox* m_pSearchDirPath;
    wxButton* m_pBtnSelectDir;
    wxCheckBox* m_pChkSearchDirRecursively;
    wxCheckBox* m_pChkSearchDirHiddenFiles;
    wxComboBox* m_pMask;
    // end wxGlade
    ThreadSearchFindData *m_pFindData;

    DECLARE_EVENT_TABLE();

private:
    void OnSearchDirTextEvent(wxCommandEvent &event);
    void OnSearchMaskTextEvent(wxCommandEvent &event);

    /** Runs a dialog to set directory path.
      */
    void OnBtnDirSelectClick(wxCommandEvent &event); // wxGlade: <event_handler>

    void OnChkSearchDirRecurse(wxCommandEvent &event);
    void OnChkSearchDirHidden(wxCommandEvent &event);
}; // wxGlade: end class


#endif // DIRECTORY_PARAMS_PANEL_H
