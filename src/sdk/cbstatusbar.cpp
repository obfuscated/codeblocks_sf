/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/frame.h> // wxFrame
    #include <wx/menu.h>
    #include <wx/statusbr.h>
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
END_EVENT_TABLE()


cbStatusBar::cbStatusBar(wxWindow* parent,  wxWindowID id, long style, const wxString& name)
    : wxStatusBar(parent, wxID_ANY, style, name)
{
}

cbStatusBar::~cbStatusBar()
{
}

void cbStatusBar::SetStatusTextByPlugin(const wxString& text, cbPlugin* plugin)
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if (pos != -1)
        wxStatusBar::SetStatusText(text, pos);
}

wxString cbStatusBar::GetStatusTextByPlugin(cbPlugin* plugin) const
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if (pos != -1)
        return wxStatusBar::GetStatusText(pos);
    return wxEmptyString;
}

bool cbStatusBar::GetFieldRectByPlugin(cbPlugin* plugin, wxRect& rect) const
{
    int pos = GetFieldNumberOfPlugin(plugin);
    if (pos != -1)
        return wxStatusBar::GetFieldRect(pos, rect);
    return false;
}

void cbStatusBar::SetStatusWidths(int n, const int* widths)
{
    if (widths)
    {
        m_MainWidths.clear();
        for (int i = 0 ; i < n ; i++)
            m_MainWidths.push_back(widths[i]);
        UpdateWidths();
    }
}

void cbStatusBar::AddField(cbPlugin* plugin, wxWindow* ctrl, int width)
{
    cbStatusBarElement elem;
    elem.control = ctrl;
    elem.width = width;
    elem.plugin =  plugin;

    m_Elements.push_back(elem);

    UpdateWidths();

    return;
}
void cbStatusBar::AddField(cbPlugin* plugin, int width)
{
    return AddField(plugin, (wxControl*)NULL, width);
}

void cbStatusBar::RemoveField(cbPlugin* plugin)
{
    for (ElementVector::iterator it = m_Elements.begin() ; it != m_Elements.end() ; it++)
    {
        if (it->plugin == plugin)
        {
            if (it->control)
                it->control->Destroy();
            m_Elements.erase(it);
            break;
        }
    }
    UpdateWidths();
}

void cbStatusBar::OnSize(wxSizeEvent& event)
{
    AdjustFieldsSize();
    event.Skip();
}

void cbStatusBar::UpdateWidths()
{
    unsigned int n = m_MainWidths.size() + m_Elements.size();

    int *widths = new int[n];
    unsigned int i = 0;
    for (; i < m_MainWidths.size() ; i++)
        widths[i] = m_MainWidths[i];
    for (int k= 0 ; i < n ; i++, k++)
        widths[i] = m_Elements[k].width;

    wxStatusBar::SetFieldsCount(n);
    wxStatusBar::SetStatusWidths(n, widths);
    delete[] widths;
}

void cbStatusBar::AdjustFieldsSize()
{
    for (unsigned int i = 0 ; i < m_Elements.size(); i++)
    {
        wxWindow *ctrl = m_Elements[i].control;
        if (ctrl)
        {
            wxRect rect;
            GetFieldRectByPlugin(m_Elements[i].plugin, rect);
            ctrl->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);
        }
    }
}

int cbStatusBar::GetFieldNumberOfPlugin(cbPlugin* plugin) const
{
    for (unsigned int i = 0 ; i < m_Elements.size() ; i++)
    {
        if (m_Elements[i].plugin == plugin)
            return i+m_MainWidths.size();
    }
    // return -1 to indicate that the id does not exist
    return -1;
}
