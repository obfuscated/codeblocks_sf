/***************************************************************
 * Name:      BrowseTrackerConfPanel
 * Purpose:   This class implements the configuration panel used
 *            in modal dialog called on "Settings" menu click
 *            and by C::B on "Environment" settings window.
 * Author:    Pecan
 * Created:   2008/03/13
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/

#ifndef BROWSETRACKER_CONF_PANEL_H
#define BROWSETRACKER_CONF_PANEL_H

#include "configurationpanel.h"
#include "ConfigPanel.h"

class BrowseTracker;

// ----------------------------------------------------------------------------
class BrowseTrackerConfPanel: public cbConfigurationPanel
// ----------------------------------------------------------------------------
{
  public:

	/** Constructor. */
    BrowseTrackerConfPanel(BrowseTracker& browseTrackerPlugin, wxWindow* parent = NULL, wxWindowID id = -1);

	/** Returns the title displayed in the left column of the "Settings/Environment" dialog. */
	wxString GetTitle()          const {return wxT("BrowseTracker");}

	/** Returns string used to build active/inactive images path in the left column
	  * of the "Settings/Environment" dialog.
	  */
	wxString GetBitmapBaseName() const {return wxT("BrowseTracker");}

	/** Called automatically when user clicks on OK
	  */
	void OnApply();

	/** Called automatically when user clicks on Cancel
	  */
	void OnCancel() {}

  private:

    BrowseTracker& m_BrowseTrackerPlugin;  // Reference on the BrowseTracker plugin we configure

 protected:

    DECLARE_EVENT_TABLE();

    // -----------------------------------------------------
    // Shell class for wxFormBuilder generated CfgPanel.cpp/h
    // -----------------------------------------------------
  public:

        void GetUserOptions( wxString cfgFullPath);

  protected:
    ConfigPanel* m_pConfigPanel;

  private:

    // Virtual event handlers, overriden fron ConfigPanel.h
    virtual void OnEnableBrowseMarks( wxCommandEvent& event );
    virtual void OnToggleBrowseMarkKey( wxCommandEvent& event );
    virtual void OnClearAllBrowseMarksKey( wxCommandEvent& event );

};
#endif // THREAD_SEARCH_CONF_PANEL_H
