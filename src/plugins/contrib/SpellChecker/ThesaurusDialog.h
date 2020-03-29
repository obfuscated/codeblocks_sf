/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef THESAURUSDIALOG_H
#define THESAURUSDIALOG_H

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/sizer.h>

#include <map>
#include <vector>

typedef std::map<wxString, std::vector< wxString > > synonyms;
class ThesaurusDialog: public wxDialog
{
public:

    ThesaurusDialog(wxWindow* parent, wxString Word, synonyms &syn);
    virtual ~ThesaurusDialog();

    wxString GetSelection();
private:
    wxStaticText* m_StaticText;
    wxListBox* m_ListBoxMeanings;
    wxListBox* m_ListBoxSynonyme;
    wxChoice* m_ChoiceLookedUp;
    wxTextCtrl* m_TextCtrlReplaceSynonym;

    wxStdDialogButtonSizer* m_sdbSizer1;
    wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;



protected:


private:
    synonyms &m_syn;
    void UpdateSynonyme();
    void UpdateSelectedSynonym();

    void OnMeaningsSelected(wxCommandEvent& event);
    void OnSynonymeSelected(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
