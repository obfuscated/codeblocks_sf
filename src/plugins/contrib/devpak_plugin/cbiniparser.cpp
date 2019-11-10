/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "cbiniparser.h"
#include <wx/utils.h>
#include <wx/file.h>

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(IniKeyValuePairArray);
WX_DEFINE_OBJARRAY(IniGroupArray);

IniParser::IniParser()
{
}

IniParser::~IniParser()
{
}

bool IniParser::ParseFile(const wxString& filename)
{
    if (!wxFileExists(filename))
        return false;

    // open file
    wxString buffer;
    wxFile file(filename);
    if (!file.IsOpened())
        return false;
    int len = file.Length();

    if(len==0)
        buffer.Clear();
    else
    {
        char* buff = new char[len+1];
        file.Read(buff, len);
        buff[len]='\0';
        buffer = wxString(buff,wxConvUTF8);
        delete[] buff;
    }
    file.Close();

    return ParseBuffer(buffer);
}

bool IniParser::ParseBuffer(wxString& buffer)
{
    m_Array.Clear();

    while (!buffer.IsEmpty())
    {
        wxString line = ReadLineFromBuffer(buffer);
        line.Trim(false);
        line.Trim(true);
        if (line.IsEmpty())
            continue;

        if (line.GetChar(0) == _T('['))
        {
            // new group
            IniGroup newgroup;
            newgroup.name = line.Mid(1, line.Length() - 2);
            newgroup.name.Trim(false);
            newgroup.name.Trim(true);
            if (newgroup.name.IsEmpty())
                continue;
            m_Array.Add(newgroup);
        }
        else
        {
            int pos = line.Find(_T('='));
            if (pos == -1)
                pos = line.Length();
            IniKeyValuePair newpair;
            newpair.key = line.Left(pos);
            newpair.value = line.Right(line.Length() - pos - 1);
            newpair.key.Trim(false);
            newpair.key.Trim(true);
            newpair.value.Trim(false);
            newpair.value.Trim(true);
            if (newpair.key.IsEmpty() || newpair.key.GetChar(0) < _T('A') || newpair.key.GetChar(0) > _T('z'))
                continue;
            if (m_Array.GetCount() == 0)
            {
                // add empty group
                IniGroup emptygroup;
                m_Array.Add(emptygroup);
            }
            m_Array[m_Array.GetCount() - 1].pairs.Add(newpair);
        }
    }

    return true;
}

wxString IniParser::ReadLineFromBuffer(wxString& buffer)
{
    int len = buffer.Length();
    int i = 0;
    while (i < len && buffer.GetChar(i) != _T('\n'))
        ++i;
    wxString str = buffer.Left(i);
    while (i < len && (buffer.GetChar(i) == _T('\n') || buffer.GetChar(i) == _T('\r')))
        ++i;
    buffer.Remove(0, i);
    buffer.Trim();
    return str;
}

int IniParser::GetGroupsCount() const
{
    return m_Array.GetCount();
}

const wxString & IniParser::GetGroupName(int idx) const
{
    return m_Array[idx].name;
}

int IniParser::FindGroupByName(const wxString& name, bool caseSensitive) const
{
    for (int i = 0; i < GetGroupsCount(); ++i)
    {
        bool found = caseSensitive
                    ? name.Cmp(m_Array[i].name) == 0
                    : name.CmpNoCase(m_Array[i].name) == 0;
        if (found)
            return i;
    }
    return -1;
}

int IniParser::GetKeysCount(int group) const
{
    return m_Array[group].pairs.GetCount();
}

const wxString & IniParser::GetKeyName(int group, int idx) const
{
    return m_Array[group].pairs[idx].key;
}

const wxString & IniParser::GetKeyValue(int group, int idx) const
{
    return m_Array[group].pairs[idx].value;
}

const wxString& IniParser::GetKeyValue(int group, const wxString& key) const
{
    static wxString empty;
    int keyIdx = FindKeyByName(group, key);
    if (keyIdx == -1)
        return empty;
    return GetKeyValue(group, keyIdx);
}

int IniParser::FindKeyByName(int groupIdx, const wxString& name, bool caseSensitive) const
{
    if (groupIdx == -1)
        return -1;
    for (int i = 0; i < GetKeysCount(groupIdx); ++i)
    {
        bool found = caseSensitive
                    ? name.Cmp(m_Array[groupIdx].pairs[i].key) == 0
                    : name.CmpNoCase(m_Array[groupIdx].pairs[i].key) == 0;
        if (found)
            return i;
    }
    return -1;
}

const wxString & IniParser::GetValue(const wxString& group, const wxString& key, bool caseSensitive) const
{
    static wxString ret;
    ret.Clear();

    int g = FindGroupByName(group, caseSensitive);
    int k = FindKeyByName(g, key, caseSensitive);
    if (g != -1 && k != -1)
        return m_Array[g].pairs[k].value;
    return ret;
}

int my_sorter(IniGroup** first, IniGroup** second)
{
    return (*first)->name.Cmp((*second)->name);
}

int my_sorter_nocase(IniGroup** first, IniGroup** second)
{
    return (*first)->name.CmpNoCase((*second)->name);
}

void IniParser::Sort(bool caseSensitive)
{
    m_Array.Sort(caseSensitive ? my_sorter : my_sorter_nocase);
}
