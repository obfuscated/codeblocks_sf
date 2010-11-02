/***************************************************************
 * Name:      ThreadSearchConfPanel
 * Purpose:   This class implements the configuration panel used
 *            in modal dialog called on "Options" button click
 *            and by C::B on "Environment" settings window.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_CONF_PANEL_H
#define THREAD_SEARCH_CONF_PANEL_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode

// end wxGlade

#include <wx/string.h>

#include "configurationpanel.h"


class wxWindow;
class wxRadioBox;
class wxCheckBox;
class wxStaticBox;
class wxCommandEvent;

class ThreadSearch;
class SearchInPanel;
class DirectoryParamsPanel;


class ThreadSearchConfPanel: public cbConfigurationPanel {
public:
    // begin wxGlade: ThreadSearchConfPanel::ids
    // end wxGlade

    /** Constructor. */
    ThreadSearchConfPanel(ThreadSearch& threadSearchPlugin, wxWindow* parent = NULL, wxWindowID id = -1);

    /** Returns the title displayed in the left column of the "Settings/Environment" dialog. */
    wxString GetTitle()          const {return _("Thread search");}

    /** Returns string used to build active/inactive images path in the left column
      * of the "Settings/Environment" dialog.
      */
    wxString GetBitmapBaseName() const {return wxT("ThreadSearch");}

    /** Called automatically when user clicks on OK
      */
    void OnApply();

    /** Called automatically when user clicks on Cancel
      */
    void OnCancel() {}

private:
    // begin wxGlade: ThreadSearchConfPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    ThreadSearch& m_ThreadSearchPlugin;  // Reference on the ThreadSearch plugin we configure

protected:
    // begin wxGlade: ThreadSearchConfPanel::attributes
    wxStaticBox* SizerThreadSearchLayout_staticbox;
    wxStaticBox* SizerListControlOptions_staticbox;
    wxStaticBox* SizerThreadSearchLayoutGlobal_staticbox;
    wxStaticBox* SizerThreadSearchOptions_staticbox;
    wxStaticBox* SizerSearchIn_staticbox;
    SearchInPanel* m_pPnlSearchIn;
    DirectoryParamsPanel* m_pPnlDirParams;
    wxCheckBox* m_pChkWholeWord;
    wxCheckBox* m_pChkStartWord;
    wxCheckBox* m_pChkMatchCase;
    wxCheckBox* m_pChkRegExp;
    wxCheckBox* m_pChkThreadSearchEnable;
    wxCheckBox* m_pChkUseDefaultOptionsForThreadSearch;
    wxCheckBox* m_pChkShowMissingFilesError;
    wxCheckBox* m_pChkShowCantOpenFileError;
    wxCheckBox* m_pChkDeletePreviousResults;
    wxCheckBox* m_pChkShowThreadSearchToolBar;
    wxCheckBox* m_pChkShowThreadSearchWidgets;
    wxCheckBox* m_pChkShowCodePreview;
    wxCheckBox* m_pChkDisplayLogHeaders;
    wxCheckBox* m_pChkDrawLogLines;
    wxRadioBox* m_pRadPanelManagement;
    wxRadioBox* m_pRadLoggerType;
    wxRadioBox* m_pRadSplitterWndMode;
    wxRadioBox* m_pRadSortBy;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    /** The m_pChkThreadSearchEnable checkbox is used to enable/disable 'Find occurrences'
      * contextual menu integration.
      * This method disables the m_pChkUseDefaultOptionsForThreadSearch checkbox if
      * 'Find occurrences' is not present in the contextual menu.
      */
    void OnThreadSearchEnable(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnChkShowThreadSearchToolBarClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnChkCodePreview(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnChkShowThreadSearchWidgetsClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnChkShowMissingFilesErrorClick(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnChkShowCantOpenFileErrorClick(wxCommandEvent &event); // wxGlade: <event_handler>

}; // wxGlade: end class


#endif // THREAD_SEARCH_CONF_PANEL_H
