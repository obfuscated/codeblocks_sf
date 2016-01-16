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

BEGIN_EVENT_TABLE(GenericMultiLineNotesDlg, wxScrollingDialog)
    //
END_EVENT_TABLE()

GenericMultiLineNotesDlg::GenericMultiLineNotesDlg(wxWindow* parent, const wxString& caption, const wxString& notes, bool readOnly)
    : m_Notes(notes),
    m_ReadOnly(readOnly)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgGenericMultiLineNotes"),_T("wxScrollingDialog"));
    SetTitle(caption);

    wxTextCtrl *notesCtrl = XRCCTRL(*this, "txtNotes", wxTextCtrl);

    notesCtrl->SetValue(m_Notes);
    if (m_ReadOnly)
    {
        notesCtrl->SetEditable(false);
        if (wxWindow* win = FindWindowById(wxID_CANCEL, this))
        {
            win->Enable(false);
        }
    }
}

GenericMultiLineNotesDlg::~GenericMultiLineNotesDlg()
{
    //dtor
}

void GenericMultiLineNotesDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK && !m_ReadOnly)
    {
        m_Notes = XRCCTRL(*this, "txtNotes", wxTextCtrl)->GetValue();
    }
    wxScrollingDialog::EndModal(retCode);
}
