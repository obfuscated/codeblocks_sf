/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/frame.h> // wxFrame
    #include <wx/menu.h>
    #include "cbplugin.h"
    #include "compiler.h" // GetSwitches
    #include "sdk_events.h"
    #include "manager.h"
    #include "projectbuildtarget.h"
    #include "cbproject.h"
    #include "logmanager.h"
#endif

#include "cbstatusbar.h"

BEGIN_EVENT_TABLE(cbStatusBar, wxStatusBar)
    EVT_SIZE(cbStatusBar::OnSize)
//    EVT_TIMER(wxID_ANY, MyStatusBar::OnTimer)
END_EVENT_TABLE()


cbStatusBar::cbStatusBar(wxWindow *parent,  wxWindowID id, long style, const wxString& name)
    : wxStatusBar(parent, wxID_ANY, style, name)
{
    //single = this;
}
//cbStatusBar* cbStatusBar::single = NULL;
//cbStatusBar* cbStatusBar::GetInstance()
//{
//    return single;
//}

cbStatusBar::~cbStatusBar()
{
    //single = NULL;
}


void cbStatusBar::OnSize(wxSizeEvent& event)
{
    AdjustFieldsSize();
    event.Skip();
}

void cbStatusBar::AdjustFieldsSize()
{
    for ( unsigned int i = 0 ; i < m_elements.size(); i++)
    {
        wxWindow *ctrl = m_elements[i].control;
        if ( ctrl )
        {
            wxRect rect;
            GetFieldRect(m_elements[i].plugin, rect);
            ctrl->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);
        }
    }

}


void cbStatusBar::AddField(cbPlugin *plugin, wxWindow *ctrl, int width )
{
    cbStatusBarElement elem;
    elem.control = ctrl;
    elem.width = width;
    elem.plugin =  plugin;

    m_elements.push_back(elem);

    UpdateWidths();

    return;
}
void cbStatusBar::AddField(cbPlugin *plugin, int width)
{
    return AddField(plugin, (wxControl*)NULL, width);
}
void cbStatusBar::RemoveField(cbPlugin *plugin)
{
    for ( ElementVector::iterator it = m_elements.begin() ; it != m_elements.end() ; it++ )
    {
        if ( it->plugin == plugin )
        {
            if ( it->control )
                it->control->Destroy();
            m_elements.erase(it);
            break;
        }
    }
    UpdateWidths();
}
void cbStatusBar::UpdateWidths()
{
    unsigned int n = m_mainswidths.size() + m_elements.size();

    int *widths = new int[n];
    unsigned int i = 0;
    for ( ; i < m_mainswidths.size() ; i++)
        widths[i] = m_mainswidths[i];
    for (int k= 0 ; i < n ; i++, k++ )
        widths[i] = m_elements[k].width;

    wxStatusBar::SetFieldsCount(n);
    wxStatusBar::SetStatusWidths(n, widths);
    delete[] widths;
}
void cbStatusBar::SetStatusWidths(int n, const int* widths)
{
    if (widths)
    {
        m_mainswidths.clear();
        for (int i = 0 ; i < n ; i++)
            m_mainswidths.push_back(widths[i]);
        UpdateWidths();
    }
}
int cbStatusBar::GetFieldNumberOfPlugin(cbPlugin *plugin) const
{
    for ( unsigned int i = 0 ; i < m_elements.size() ; i++ )
    {
        if ( m_elements[i].plugin == plugin )
            return i+m_mainswidths.size();
    }
    // return -1 to indicate that the id does not exist
    return -1;
}

void cbStatusBar::SetStatusText(const wxString& text, cbPlugin *plugin)
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if ( pos != -1 )
        wxStatusBar::SetStatusText(text, pos);
}
wxString cbStatusBar::GetStatusText(cbPlugin *plugin) const
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if ( pos != -1 )
        return wxStatusBar::GetStatusText(pos);
    return wxEmptyString;
}
bool cbStatusBar::GetFieldRect(cbPlugin *plugin, wxRect& rect) const
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if ( pos != -1 )
        return wxStatusBar::GetFieldRect(pos, rect);
    return false;
}
