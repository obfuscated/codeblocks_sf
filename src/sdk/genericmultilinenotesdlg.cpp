/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
#endif

#include "genericmultilinenotesdlg.h"
#include <wx/textctrl.h>

BEGIN_EVENT_TABLE(GenericMultiLineNotesDlg, wxDialog)
    //
END_EVENT_TABLE()

GenericMultiLineNotesDlg::GenericMultiLineNotesDlg(wxWindow* parent, const wxString& caption, const wxString& notes, bool readOnly)
    : m_Notes(notes),
    m_ReadOnly(readOnly)
{
    //ctor
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgGenericMultiLineNotes"));
    SetTitle(caption);

    XRCCTRL(*this, "txtNotes", wxTextCtrl)->SetValue(m_Notes);
    XRCCTRL(*this, "txtNotes", wxTextCtrl)->SetEditable(!m_ReadOnly);

    if (m_ReadOnly)
    {
        wxWindow* win = FindWindowById(wxID_OK, this);
        if (win)
            win->Enable(false);
    }
}

GenericMultiLineNotesDlg::~GenericMultiLineNotesDlg()
{
    //dtor
}

void GenericMultiLineNotesDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
        m_Notes = XRCCTRL(*this, "txtNotes", wxTextCtrl)->GetValue();
    wxDialog::EndModal(retCode);
}
