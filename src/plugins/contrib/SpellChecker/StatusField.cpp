#include "StatusField.h"

#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/filefn.h>

#include <algorithm>
#include <vector>

#include "SpellCheckerConfig.h"
#include "SpellCheckerPlugin.h"
#define LANGS 10

namespace
{
    const int idCommand[LANGS]  = {static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),
                                   static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId()),static_cast<int>(wxNewId())};
    const int idEnableSpellCheck = wxNewId();
    const int idEditPersonalDictionary = wxNewId();
};

SpellCheckerStatusField::SpellCheckerStatusField(wxWindow* parent, SpellCheckerPlugin *plugin, SpellCheckerConfig *sccfg)
    :wxPanel(parent, wxID_ANY),
    m_bitmap(NULL),
    m_text(NULL),
    m_sccfg(sccfg),
    m_plugin(plugin)
{
    //ctor
    m_text = new wxStaticText(this, wxID_ANY, m_sccfg->GetDictionaryName());

    wxString imgPath = m_sccfg->GetBitmapPath() + wxFILE_SEP_PATH + m_sccfg->GetDictionaryName() + _T(".png");
    if ( wxFileExists(imgPath) )
    {
        wxBitmap bm(wxImage(imgPath, wxBITMAP_TYPE_PNG));
        if ( bm.IsOk() )
            m_bitmap = new wxStaticBitmap(this, wxID_ANY, bm);
    }

    Update();

    Connect(wxEVT_SIZE, wxSizeEventHandler(SpellCheckerStatusField::OnSize), NULL, this);
    Connect(idCommand[0],idCommand[LANGS-1], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnSelect), NULL, this);
    Connect(idEnableSpellCheck, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnSelect), NULL, this);
    Connect(idEditPersonalDictionary, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnEditPersonalDictionary), NULL, this);

    m_text->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
    if (m_bitmap)
        m_bitmap->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
    m_text->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
    if (m_bitmap)
        m_bitmap->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
    Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp), NULL, this);
}

SpellCheckerStatusField::~SpellCheckerStatusField()
{
    //dtor
    Disconnect(wxEVT_SIZE, wxSizeEventHandler(SpellCheckerStatusField::OnSize), NULL, this);
    Disconnect(idCommand[0],idCommand[LANGS-1], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnSelect), NULL, this);
    Disconnect(idEnableSpellCheck, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnSelect), NULL, this);
    Disconnect(idEditPersonalDictionary, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerStatusField::OnEditPersonalDictionary), NULL, this);

    m_text->Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp));
    if (m_bitmap)
        m_bitmap->Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp));
    Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(SpellCheckerStatusField::OnRightUp));
}
//void SpellCheckerStatusField::SetLanguage(const wxString &language)
void SpellCheckerStatusField::Update()
{
    wxString imgPath = m_sccfg->GetBitmapPath() + wxFILE_SEP_PATH;
    if (m_sccfg->GetEnableOnlineChecker())
    {
        wxString name = m_sccfg->GetDictionaryName();
        m_text->SetLabel(name);
        if (!wxFileExists(imgPath + name + _T(".png")))
            name.Replace(wxT("-"), wxT("_")); // some dictionaries are distributed with hyphens
        imgPath += name + _T(".png");
    }
    else
    {
        m_text->SetLabel(_("off"));
        imgPath += _T("disabled.png");
    }

    bool imgOK = false;
    if ( m_bitmap && wxFileExists(imgPath) )
    {
        wxBitmap bm(wxImage(imgPath, wxBITMAP_TYPE_PNG));
        if ( bm.IsOk() )
        {
            m_text->Hide();
            m_bitmap->Hide();
            m_bitmap->SetBitmap(bm);
            m_bitmap->Show();
            imgOK = true;
        }
    }

    if (!imgOK)
    {
        if (m_bitmap)
            m_bitmap->Hide();
        m_text->Show();
    }

    DoSize();
}

void SpellCheckerStatusField::OnSize(wxSizeEvent &event)
{
    DoSize();
}
void SpellCheckerStatusField::DoSize()
{
    wxSize msize = this->GetSize();

    m_text->SetSize(msize);

    if (m_bitmap)
    {
        wxSize bsize = m_bitmap->GetSize();
        m_bitmap->Move(msize.x/2 - bsize.x/2, msize.y/2 - bsize.y/2);
    }
}

void SpellCheckerStatusField::OnRightUp(wxMouseEvent &event)
{
    m_sccfg->ScanForDictionaries();
    wxMenu *popup = new wxMenu();
    std::vector<wxString> dicts = m_sccfg->GetPossibleDictionaries();
    for ( unsigned int i = 0 ; i < dicts.size() && i < LANGS ; i++ )
        popup->Append( idCommand[i], m_sccfg->GetLanguageName(dicts[i]), _T(""), wxITEM_CHECK)->Check(dicts[i] == m_sccfg->GetDictionaryName() );
    if (!dicts.empty())
        popup->AppendSeparator();
    popup->Append(idEnableSpellCheck, _("Enable spell check"), wxEmptyString, wxITEM_CHECK)->Check(m_sccfg->GetEnableOnlineChecker());
    wxMenuItem *mnuItm = popup->Append( idEditPersonalDictionary, _("Edit personal dictionary"), _T(""));
    mnuItm->Enable( wxFile::Exists(m_sccfg->GetPersonalDictionaryFilename()) );

    PopupMenu(popup);
    delete popup;
}
void SpellCheckerStatusField::OnSelect(wxCommandEvent &event)
{
    unsigned int idx;
    for ( idx = 0 ; idx < LANGS ; idx++)
        if ( event.GetId() == idCommand[idx])
            break;

    std::vector<wxString> dicts = m_sccfg->GetPossibleDictionaries();

    if ( idx < dicts.size() )
    {
        m_sccfg->SetDictionaryName(dicts[idx]);
        m_sccfg->SetEnableOnlineChecker(true);
        m_sccfg->Save(); // save it
    }
    else if (!dicts.empty() && event.GetId() == idEnableSpellCheck)
    {
        m_sccfg->SetEnableOnlineChecker(!m_sccfg->GetEnableOnlineChecker()); // toggle
        if (   m_sccfg->GetEnableOnlineChecker()
            && std::find(dicts.begin(), dicts.end(), m_sccfg->GetDictionaryName()) == dicts.end() )
        {
            // insure there always is a valid dictionary selected when enabled
            m_sccfg->SetDictionaryName(dicts[0]);
        }
        m_sccfg->Save();
    }

}
void SpellCheckerStatusField::OnEditPersonalDictionary(wxCommandEvent &event)
{
    m_plugin->EditPersonalDictionary();
}
