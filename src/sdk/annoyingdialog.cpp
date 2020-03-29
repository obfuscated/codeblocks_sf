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
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/intl.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include "cbexception.h"
    #include "configmanager.h"
    #include "manager.h"
    #include "logmanager.h"
    #include "globals.h"
#endif
#include <wx/statbmp.h>
#include "annoyingdialog.h"

BEGIN_EVENT_TABLE(AnnoyingDialog, wxScrollingDialog)
    EVT_BUTTON(-1, AnnoyingDialog::OnButton)
END_EVENT_TABLE()

AnnoyingDialog::AnnoyingDialog(const wxString& caption, const wxString& message, const wxArtID icon,
                               dStyle style, dReturnType defaultReturn,
                               const wxString& b1, const wxString& b2, const wxString& b3)
        : wxScrollingDialog(nullptr, -1, caption, wxDefaultPosition, wxDefaultSize, wxCAPTION),
        m_DefRet(defaultReturn)
{
    Init(caption, caption, message, icon, style, b1, b2, b3);
}

AnnoyingDialog::AnnoyingDialog(const wxString& caption, const wxString &id, const wxString& message,
                               const wxArtID icon, dStyle style, dReturnType defaultReturn,
                               const wxString& b1, const wxString& b2, const wxString& b3)
        : wxScrollingDialog(nullptr, -1, caption, wxDefaultPosition, wxDefaultSize, wxCAPTION),
        m_DefRet(defaultReturn)
{
    Init(caption, id, message, icon, style, b1, b2, b3);
}

void AnnoyingDialog::Init(const wxString &caption, const wxString &id, const wxString& message, const wxArtID icon,
                          dStyle style, const wxString& b1, const wxString& b2, const wxString& b3)
{
    m_CheckBox = nullptr;
    m_DontAnnoy = false;
    m_Id = id;

    static_assert(wxMinimumVersion<2,8,12>::eval, "wxWidgets 2.8.12 is required");

    ConfigManagerContainer::StringSet disabled;
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("an_dlg"));
    if (cfg->Exists(wxT("/disabled_ret")))
    {
        // new config style, includes return code in format:
        // "id:dReturnType"
        // example:
        // "Question XYZ?:4"
        disabled = cfg->ReadSSet(wxT("/disabled_ret"));
    }
    else
    {
        // if the new config key does not exist, read from the old one
        // old keys are in format:
        // "id"
        disabled = cfg->ReadSSet(wxT("/disabled"));
        // and copy it to the new one
        cfg->Write(wxT("/disabled_ret"), disabled);
        // we do not do an in place upgrade of the format to maintain
        // compatibility with previous versions
    }

    ConfigManagerContainer::StringSet::const_iterator it = disabled.lower_bound(m_Id);
    if (it != disabled.end())
    {
        if (*it == m_Id)
        {
            // upgrade old settings
            m_DontAnnoy = true;
            if (m_DefRet == rtSAVE_CHOICE)
                m_DefRet = rtYES; // default value
            disabled.erase(it);
            disabled.insert(m_Id + F(wxT(":%d"), m_DefRet));
            // save updated format
            cfg->Write(wxT("/disabled_ret"), disabled);
            return;
        }
        else if (it->BeforeLast(wxT(':')) == m_Id)
        {
            m_DontAnnoy = true;
            // read the saved choice and store it for ShowModal() to use
            long ret = rtSAVE_CHOICE;
            if (it->AfterLast(wxT(':')).ToLong(&ret) && ret != rtSAVE_CHOICE)
            {
                Manager::Get()->GetLogManager()->Log(*it);
                m_DefRet = (dReturnType)ret;
            }
            else if (m_DefRet == rtSAVE_CHOICE)
                m_DefRet = rtYES; // default value
            return;
        }
    }

    wxBoxSizer *outerSizer = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *mainArea = new wxFlexGridSizer(2, 0, 0);
    wxStaticBitmap *bitmap = new wxStaticBitmap(this, -1, wxArtProvider::GetBitmap(icon,  wxART_MESSAGE_BOX), wxDefaultPosition);
    mainArea->Add(bitmap, 0, wxALL, 5);

    wxStaticText *txt = new wxStaticText(this, -1, message, wxDefaultPosition, wxDefaultSize, 0);
    mainArea->Add( txt, 0, wxALIGN_CENTER|wxALL, 5 );

    mainArea->Add( 1, 1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    int numButtons = 0;
    dReturnType id1 = rtINVALID;
    dReturnType id2 = rtINVALID;
    dReturnType id3 = rtINVALID;
    wxString bTxt1;
    wxString bTxt2;
    wxString bTxt3;

    if(style == OK || style == ONE_BUTTON)
    {
        numButtons = 1;
        // only one choice, so set m_DefRet
        m_DefRet = (style == OK ? rtOK : rtONE);
        id1 = m_DefRet;
        bTxt1 = b1.IsEmpty() ? wxString(_("&OK")) : b1;
    }
    else if(style == YES_NO || style == OK_CANCEL || style == TWO_BUTTONS)
    {
        numButtons = 2;
        id1 = (style == YES_NO ? rtYES : (style == OK_CANCEL ? rtOK     : rtONE));
        id2 = (style == YES_NO ? rtNO  : (style == OK_CANCEL ? rtCANCEL : rtTWO));
        bTxt1 = b1.IsEmpty() ? (style == YES_NO ? wxString(_("&Yes")) : wxString(_("&OK")))     : b1;
        bTxt2 = b2.IsEmpty() ? (style == YES_NO ? wxString(_("&No"))  : wxString(_("&Cancel"))) : b2;
        // this is the default, so apply correct return type (if it was not set)
        if (m_DefRet == rtYES)
            m_DefRet = id1;
    }
    else if(style == YES_NO_CANCEL || style == THREE_BUTTONS)
    {
        numButtons = 3;
        id1 = (style == YES_NO_CANCEL ? rtYES    : rtONE);
        id2 = (style == YES_NO_CANCEL ? rtNO     : rtTWO);
        id3 = (style == YES_NO_CANCEL ? rtCANCEL : rtTHREE);
        bTxt1 = b1.IsEmpty() ? wxString(_("&Yes"))    : b1;
        bTxt2 = b2.IsEmpty() ? wxString(_("&No"))     : b2;
        bTxt3 = b3.IsEmpty() ? wxString(_("&Cancel")) : b3;
    }
    else
        cbThrow(wxString(_T("Fatal error:\nUndefined style in dialog ")) << caption);

    wxSizer* buttonSizer = nullptr;
    if (style < ONE_BUTTON) // standard buttons? use wxStdDialogButtonSizer
    {
        wxStdDialogButtonSizer *buttonArea = new wxStdDialogButtonSizer();

        wxButton* but1 = new wxButton(this, id1 == rtYES ? wxID_YES : wxID_OK, bTxt1, wxDefaultPosition, wxDefaultSize, 0);
        but1->SetDefault();
        buttonArea->AddButton(but1);

        if(numButtons > 1)
        {
            wxButton* but2 = new wxButton(this, id2 == rtNO ? wxID_NO : wxID_CANCEL, bTxt2, wxDefaultPosition, wxDefaultSize, 0);
            if (id2 == m_DefRet)
                but2->SetDefault();
            buttonArea->AddButton(but2);
        }
        if(numButtons > 2)
        {
            wxButton* but3 = new wxButton(this, wxID_CANCEL, bTxt3, wxDefaultPosition, wxDefaultSize, 0);
            if (id3 == m_DefRet)
                but3->SetDefault();
            buttonArea->AddButton(but3);
        }
        buttonArea->Realize();
        buttonSizer = buttonArea;
    }
    else
    {
        // wxStdDialogButtonSizer accepts only standard IDs for its buttons, so we can't use
        // it with custom buttons
        buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        wxButton *but1 = new wxButton(this, id1, bTxt1, wxDefaultPosition, wxDefaultSize, 0);
        but1->SetDefault();
        buttonSizer->Add(but1, 0, wxRIGHT, 5);

        if(numButtons > 1)
        {
            wxButton *but2 = new wxButton(this, id2, bTxt2, wxDefaultPosition, wxDefaultSize, 0);
            if (id2 == m_DefRet)
                but2->SetDefault();
            buttonSizer->Add(but2, 0, wxRIGHT, 5);
        }
        if(numButtons > 2)
        {
            wxButton *but3 = new wxButton(this, id3, bTxt3, wxDefaultPosition, wxDefaultSize, 0);
            if (id3 == m_DefRet)
                but3->SetDefault();
            buttonSizer->Add(but3, 0, wxRIGHT, 5);
        }
    }

    outerSizer->Add( mainArea, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    outerSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);

    m_CheckBox = new wxCheckBox(this, wxID_ANY, _("Don't annoy me again!"), wxDefaultPosition, wxDefaultSize, 0);
    outerSizer->Add(m_CheckBox, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    SetSizer( outerSizer );
    outerSizer->SetSizeHints(this);

    Centre();
}

void AnnoyingDialog::OnButton(wxCommandEvent& event)
{
    if(!m_CheckBox)
        cbThrow(_T("Ow... null pointer."));

    int id = event.GetId();
    // convert IDs from standard buttons to dReturnType
    switch (id)
    {
        case wxID_YES:
            id = rtYES;
            break;
        case wxID_OK:
            id = rtOK;
            break;
        case wxID_NO:
            id = rtNO;
            break;
        case wxID_CANCEL:
            id = rtCANCEL;
            break;
        default:
            break;
    }

    if(m_CheckBox->IsChecked())
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("an_dlg"));
        ConfigManagerContainer::StringSet disabled = cfg->ReadSSet(wxT("/disabled_ret"));
        // if we are supposed to remember the users choice, save the button
        disabled.insert(m_Id + F(wxT(":%d"), m_DefRet == rtSAVE_CHOICE ? id : m_DefRet));
        cfg->Write(wxT("/disabled_ret"), disabled);
    }
    EndModal(id);
}

int AnnoyingDialog::ShowModal()
{
    if(m_DontAnnoy)
        return m_DefRet;
    PlaceWindow(this);
    return wxScrollingDialog::ShowModal();
}
