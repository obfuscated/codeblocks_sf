/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#include "compileroptions.h"

CompilerOptions::CompilerOptions()
{
}

CompilerOptions::CompilerOptions(const CompilerOptions& other)
{
    // copy ctor
    *this = other;
}

CompilerOptions& CompilerOptions::operator=(const CompilerOptions& other)
{
    ClearOptions();
    for (unsigned int i = 0; i < other.m_Options.GetCount(); ++i)
    {
        CompOption* coption = new CompOption(*(other.m_Options[i]));
        AddOption(coption);
    }
    return *this;
}

CompilerOptions::~CompilerOptions()
{
    ClearOptions();
}

void CompilerOptions::ClearOptions()
{
    for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
    {
        CompOption* coption = m_Options.Item(i);
        delete coption;
    }
    m_Options.Clear();
}

void CompilerOptions::AddOption(CompOption* coption, int index)
{
    if (index == -1)
        m_Options.Add(coption);
    else
        m_Options.Insert(coption, index);
}

void CompilerOptions::AddOption(const wxString& name,
                                const wxString& option,
                                const wxString& category,
                                const wxString& additionalLibs,
                                const wxString& checkAgainst,
                                const wxString& checkMessage,
                                const wxString& supersedes,
                                bool exclusive,
                                int index)
{
    if (name.IsEmpty() || (option.IsEmpty() && additionalLibs.IsEmpty()))
        return;
    CompOption* coption = new CompOption;

    wxString listboxname = name + _T("  [");
    if (option.IsEmpty())
        listboxname += additionalLibs;
    else
        listboxname += option;
    listboxname += _T("]");

    coption->name = listboxname;
    coption->option = option;
    coption->additionalLibs = additionalLibs;
    coption->enabled = false;
    coption->category = category;
    coption->checkAgainst = checkAgainst;
    coption->checkMessage = checkMessage;
    coption->supersedes = supersedes;
    coption->exclusive = exclusive;
    AddOption(coption, index);
}

void CompilerOptions::RemoveOption(int index)
{
    CompOption* coption = m_Options.Item(index);
    delete coption;
    m_Options.RemoveAt(index);
}

CompOption* CompilerOptions::GetOptionByName(const wxString& name)
{
    for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
    {
        CompOption* coption = m_Options.Item(i);
        if (coption->name == name)
            return coption;
    }
    return nullptr;
}

CompOption* CompilerOptions::GetOptionByOption(const wxString& option)
{
    for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
    {
        CompOption* coption = m_Options.Item(i);
        if (coption->option == option)
            return coption;
    }
    return nullptr;
}

CompOption* CompilerOptions::GetOptionByAdditionalLibs(const wxString& libs)
{
    for (unsigned int i = 0; i < m_Options.GetCount(); ++i)
    {
        CompOption* coption = m_Options.Item(i);
        if (coption->additionalLibs == libs)
            return coption;
    }
    return nullptr;
}
