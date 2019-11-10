/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include "configmanager.h"
#include "logmanager.h"
#endif
#include "configurationpanel.h"
#include "loghacker.h"

const int id_text = wxNewId();
const int id_channel = wxNewId();
const int id_logger = wxNewId();

namespace
{
    PluginRegistrant<LogHacker> reg(_T("LogHacker"));
}


LogHacker::LogHacker()
{
}

LogHacker::~LogHacker()
{
}

void LogHacker::OnAttach()
{
}

void LogHacker::OnRelease(bool /*appShutDown*/)
{
}

class CfgPanel: public cbConfigurationPanel
{
    ConfigManagerContainer::StringToStringMap c2l;
    ConfigManagerContainer::StringToStringMap c2f;

    wxListBox*    channel;
    wxListBox*    logger;
    wxTextCtrl*    filename;

    //DECLARE_EVENT_TABLE
public:

    void Create(wxWindow* parent);

    virtual wxString GetTitle() const { return _T("Log Hacker"); };
    virtual wxString GetBitmapBaseName() const { return _T(""); };

    virtual void OnApply();
    virtual void OnCancel(){};

    virtual void Change();
};


//BEGIN_EVENT_TABLE
//   add list control change event ---> Change()
//END_EVENT_TABLE


void CfgPanel::Create(wxWindow* parent)
{
    wxPanel::Create(parent,-1);

    wxArrayString channelStrings;

    int slot = 0;
    while (slot < LogManager::max_logs)
    {
        const wxString t = LogManager::Get()->Slot(slot).title;

        if (!!t)
            channelStrings.Add(t);

        ++slot;
    }

    wxArrayString loggerStrings = LogManager::Get()->ListAvailable();
    loggerStrings.Insert(_T("<application default>"), 0);

    wxFlexGridSizer* flex = new wxFlexGridSizer(2, 2, 0, 0);
    flex->AddGrowableRow(1);
    flex->AddGrowableCol(0);
    flex->AddGrowableCol(1);

    wxStaticText* txt;
    txt = new wxStaticText(this, wxID_STATIC, _("Log Source"));
    flex->Add(txt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);
    txt = new wxStaticText(this, wxID_STATIC, _("Associated Logger"));
    flex->Add(txt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    channel = new wxListBox(this, id_channel, wxDefaultPosition, wxDefaultSize, channelStrings, wxLB_SINGLE );
    logger  = new wxListBox(this, id_logger,  wxDefaultPosition, wxDefaultSize, loggerStrings,  wxLB_SINGLE );
    flex->Add(channel, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    flex->Add(logger,  0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    channel->SetSelection(0);
    logger->SetSelection(0);

    // spacer
    flex->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    filename = new wxTextCtrl(this, id_text);
    flex->Add(filename, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    SetSizer(flex);
}



cbConfigurationPanel* LogHacker::GetConfigurationPanel(wxWindow* parent)
{
    CfgPanel *p = new CfgPanel();
    p->Create(parent);
    return p;
}



void CfgPanel::Change()
{
    wxString c = channel->GetStringSelection();
    wxString l = logger->GetStringSelection();
    filename->Enable(LogManager::Get()->FilenameRequired(l));

    if (l.at(0) == _T('<'))  // "<application default>"
        l.Empty();

//  blah... do something, update the maps according to what is selected
}


void CfgPanel::OnApply()
{
    LogManager *m = LogManager::Get();

    for (ConfigManagerContainer::StringToStringMap::iterator i = c2l.begin(); i != c2l.end(); ++i)
    {
        if (!!i->second.IsEmpty())
        {
            int slot = 0;
            while (slot <= LogManager::max_logs && m->Slot(slot).title != i->second)
                ++slot;

            m->SetLog(m->New(i->second), slot);
        }
    }

    // since we use ConfigManagerContainer::StringToStringMap, we could store whatever settings we have like:
    //   ConfigManager::Get()->Write(_T("/loghacker/c2l"), c2l);
    //   ConfigManager::Get()->Write(_T("/loghacker/c2f"), c2f);
}
