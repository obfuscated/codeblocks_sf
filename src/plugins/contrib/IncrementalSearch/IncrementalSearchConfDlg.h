/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2008 Jens Lody
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifndef INCREMENTALSEARCHCONFDLG_H
#define INCREMENTALSEARCHCONFDLG_H

#include <configurationpanel.h>

class IncrementalSearchConfDlg : public cbConfigurationPanel
{
public:
    IncrementalSearchConfDlg(wxWindow* parent);
    ~IncrementalSearchConfDlg();

private:
    wxString GetTitle() const { return _("Incremental search settings"); }
    wxString GetBitmapBaseName() const { return _T("incsearch"); }
    void OnApply() { SaveSettings(); }
    void OnCancel() {}
    void OnChooseColour(wxCommandEvent& event);
    void SaveSettings();

    DECLARE_EVENT_TABLE()
};

#endif // INCREMENTALSEARCHCONFDLG_H
