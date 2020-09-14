/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/arrstr.h>
    #include <wx/datetime.h>
#endif // CB_PRECOMP

#include "todo_parser.h"

// arrimpl.cpp says about the usage:
// 1) #include dynarray.h
// 2) WX_DECLARE_OBJARRAY
// ...these two are in todolistview.h
// 3) #include arrimpl.cpp
// 4) WX_DEFINE_OBJARRAY
// ...these come now:
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ToDoItems); // TODO: find out why this causes a shadow warning for 'Item'

static void SkipSpaces(const wxString& buffer, size_t &pos)
{
    wxChar c = buffer.GetChar(pos);
    while ( c == _T(' ') || c == _T('\t') )
        c = buffer.GetChar(++pos);
}

static size_t CountLines(const wxString& buffer, size_t from_pos, const size_t to_pos)
{
    size_t number_of_lines = 0;
    for (; from_pos < to_pos; ++from_pos)
    {
        if      (buffer.GetChar(from_pos) == '\r' && buffer.GetChar(from_pos + 1) == '\n')
            continue;
        else if (buffer.GetChar(from_pos) == '\r' || buffer.GetChar(from_pos)     == '\n')
            ++number_of_lines;
    }
    return number_of_lines;
}

void ParseBufferForTODOs(TodoItemsMap &outItemsMap, ToDoItems &outItems,
                         const wxArrayString &startStrings, const wxArrayString &allowedTypes,
                         const wxString& buffer, const wxString& filename)
{
    for (size_t k = 0; k < startStrings.size(); ++k)
    {
        size_t pos = 0;
        size_t last_start_pos = 0;
        size_t current_line_count = 0;

        while (1)
        {
            pos = buffer.find(startStrings[k], pos);
            if ( pos == wxString::npos )
                break;

            pos += startStrings[k].length();
            SkipSpaces(buffer, pos);

            for (size_t i = 0; i < allowedTypes.size(); ++i)
            {
                const wxString type = buffer.substr(pos, allowedTypes[i].length());

                if (type != allowedTypes[i])
                    continue;

                ToDoItem item;
                item.type = type;
                item.filename = filename;

                pos += type.length();
                SkipSpaces(buffer, pos);

                // ok, we look for two basic kinds of todo entries in the text
                // our version...
                // TODO (mandrav#0#): Implement code to do this and the other...
                // and a generic version...
                // TODO: Implement code to do this and the other...

                // is it ours or generic todo?
                if (pos < buffer.length() && buffer.GetChar(pos) == _T('('))
                {
                    // it's ours, find user and/or priority
                    ++pos;
                    while (pos < buffer.length())
                    {
                        wxChar c1 = buffer.GetChar(pos);
                        if (c1 != _T('#') && c1 != _T(')'))
                        {
                            // a little logic doesn't hurt ;)
                            if (c1 == _T(' ') || c1 == _T('\t'))
                            {
                                // allow one consecutive space
                                if (!item.user.empty() && item.user.Last() != _T(' '))
                                    item.user += _T(' ');
                            }
                            else
                                item.user += c1;
                        }
                        else if (c1 == _T('#'))
                        {
                            // look for priority
                            c1 = buffer.GetChar(++pos);
                            static const wxString allowedChars = _T("0123456789");
                            if (allowedChars.find(c1) != wxString::npos)
                                item.priorityStr += c1;
                            // skip to start of date
                            while (pos < buffer.length() && buffer.GetChar(pos) != _T('\r') && buffer.GetChar(pos) != _T('\n') )
                            {
                                const wxChar c2 = buffer.GetChar(pos);
                                if ( c2 == _T('#'))
                                {
                                    ++pos;
                                    break;
                                }
                                if ( c2 == _T(')') )
                                    break;
                                ++pos;
                            }
                            // look for date
                            while (pos < buffer.length() && buffer.GetChar(pos) != _T('\r') && buffer.GetChar(pos) != _T('\n') )
                            {
                                const wxChar c2 = buffer.GetChar(pos++);
                                if (c2 == _T(')'))
                                    break;
                                item.date += c2;
                            }

                            break;
                        }
                        else if (c1 == _T(')'))
                        {
                            ++pos;
                            break;
                        }
                        else
                            break;
                        ++pos;
                    }
                }
                // ok, we 've reached the actual todo text :)
                // take everything up to the end of line
                if (pos < buffer.length() && buffer.GetChar(pos) == _T(':'))
                    ++pos;
                size_t idx = pos;
                while (idx < buffer.length() && buffer.GetChar(idx) != _T('\r') && buffer.GetChar(idx) != _T('\n'))
                    ++idx;
                item.text = buffer.substr(pos, idx-pos);

                // do some clean-up
                item.text.Trim(true).Trim(false);
                // for a C block style comment like /* TODO: xxx */
                // we should delete the "*/" at the end of the item.text
                if (startStrings[k].StartsWith(_T("/*")) && item.text.EndsWith(_T("*/")))
                {
                    // remove the tailing "*/"
                    item.text.RemoveLast();
                    item.text.RemoveLast();
                }

                item.user.Trim();
                item.user.Trim(false);
                wxDateTime date;
                if ( !date.ParseDate(item.date.wx_str()) )
                {
                    item.date.clear(); // not able to parse date so clear the string
                }

                // adjust line count
                current_line_count += CountLines(buffer, last_start_pos, pos);
                last_start_pos = pos;

                item.line = current_line_count;
                item.lineStr = wxString::Format(_T("%d"), item.line + 1); // 1-based line number for list
                outItemsMap[filename].push_back(item);
                outItems.Add(item);

                pos = idx;
            }
            ++pos;
        }
    }
}
