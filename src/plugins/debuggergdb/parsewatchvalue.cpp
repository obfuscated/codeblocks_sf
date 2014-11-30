/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
#   include <wx/regex.h>
#endif

#include "parsewatchvalue.h"

struct Token
{
    enum Type
    {
        Undefined,
        OpenBrace,
        CloseBrace,
        Equal,
        String,
        Comma
    };

    Token() :
        start(0),
        end(0),
        type(Undefined),
        hasRepeatedChar(false)
    {
    }
    Token(int start_, int end_, Type type_) :
        start(start_),
        end(end_),
        type(type_),
        hasRepeatedChar(false)
    {
    }

    bool operator == (Token const &t) const
    {
        return start == t.start && end == t.end && type == t.type;
    }
    wxString ExtractString(wxString const &s) const
    {
        assert(end <= static_cast<int>(s.length()));
        return s.substr(start, end - start);
    }

    void Trim(wxString const &s)
    {
        while (start < static_cast<int>(s.length())
               && (s[start] == wxT(' ') || s[start] == wxT('\t') || s[start] == wxT('\n')))
            start++;
        while (end > 0
               && (s[end - 1] == wxT(' ') || s[end - 1] == wxT('\t') || s[end - 1] == wxT('\n')))
            end--;
    }

    int start, end;
    Type type;
    bool hasRepeatedChar;
};

wxRegEx regexRepeatedChars(wxT("^((\\\\'.{1,6}\\\\')|('.{1,6}'))[ \\t](<repeats[ \\t][0-9]+[ \\t]times>)"),
#ifndef __WXMAC__
                           wxRE_ADVANCED);
#else
                           wxRE_EXTENDED);
#endif

inline int DetectRepeatingSymbols(wxString const &str, int pos)
{
    int newPos = -1, currPos = pos;
    while (1)
    {
        if (currPos + 4 >= static_cast<int>(str.length()))
            break;
        if (str[currPos + 1] != wxT(','))
            break;
        if (str[currPos + 3] == wxT('\''))
        {
            const wxString &s = str.substr(currPos + 3, str.length() - (currPos + 3));
            if (regexRepeatedChars.Matches(s))
            {
                size_t start, length;
                regexRepeatedChars.GetMatch(&start, &length, 0);
                newPos = currPos + 3 + length;
                if ((newPos + 4 < static_cast<int>(str.length()))
                    && str[newPos] == wxT(',') && str[newPos + 2] == wxT('"'))
                {
                    newPos += 3;
                    while (newPos < static_cast<int>(str.length()) && str[newPos] != wxT('"'))
                        ++newPos;
                    if (newPos + 1 < static_cast<int>(str.length()) && str[newPos] == wxT('"'))
                        ++newPos;
                }
                currPos = newPos;
            }
            else
                break;
        }
        else
            break;

        // move the current position to point at the '"' character
        currPos--;
    }
    return newPos;
}

inline bool GetNextToken(wxString const &str, int pos, Token &token)
{
    token.hasRepeatedChar = false;
    while (pos < static_cast<int>(str.length())
           && (str[pos] == _T(' ') || str[pos] == _T('\t') || str[pos] == _T('\n')))
        ++pos;

    if (pos >= static_cast<int>(str.length()))
        return false;

    token.start = -1;
    bool in_quote = false, in_char = false;
    int open_braces = 0;
    struct BraceType { enum Enum { None, Angle, Square, Normal }; };
    BraceType::Enum brace_type = BraceType::None;

    switch (static_cast<wxChar>(str[pos]))
    {
    case _T('='):
        token = Token(pos, pos + 1, Token::Equal);
        return true;
    case _T(','):
        token = Token(pos, pos + 1, Token::Comma);
        return true;
    case _T('{'):
        token = Token(pos, pos + 1, Token::OpenBrace);
        return true;
    case _T('}'):
        token = Token(pos, pos + 1, Token::CloseBrace);
        return true;

    case _T('"'):
        in_quote = true;
        token.type = Token::String;
        token.start = pos;
        break;
    case _T('\''):
        in_char = true;
        token.type = Token::String;
        token.start = pos;
        break;
    case _T('<'):
        token.type = Token::String;
        token.start = pos;
        open_braces = 1;
        brace_type = BraceType::Angle;
        break;
    case wxT('['):
        token.type = Token::String;
        token.start = pos;
        open_braces = 1;
        brace_type = BraceType::Square;
        break;
    case wxT('('):
        token.type = Token::String;
        open_braces = 1;
        brace_type = BraceType::Normal;
        token.start = pos;
        break;
    default:
        token.type = Token::String;
        token.start = pos;
    }
    ++pos;

    bool escape_next = false;
    while (pos < static_cast<int>(str.length()))
    {
        if (open_braces == 0)
        {
            if (str[pos] == _T(',') && !in_quote)
            {
                token.end = pos;
                return true;
            }
            else if ((str[pos] == _T('=') || str[pos] == _T('{') || str[pos] == _T('}')) && !in_quote && !in_char)
            {
                token.end = pos;
                return true;
            }
            else if (str[pos] == _T('"'))
            {
                if (in_quote)
                {
                    if (!escape_next)
                    {
                        int newPos = DetectRepeatingSymbols(str, pos);
                        if (newPos != -1)
                        {
                            token.hasRepeatedChar = true;
                            token.end =  newPos;
                            return true;
                        }
                        else
                        {
                            token.end = pos + 1;
                            return true;
                        }
                    }
                    else
                        escape_next = false;
                }
                else
                {
                    if (escape_next)
                        return false;
                    in_quote = true;
                }
            }
            else if (str[pos] == _T('\''))
            {
                if (!escape_next)
                    in_char = !in_char;
                escape_next = false;
            }
            else if (str[pos] == _T('\\'))
                escape_next = true;
            else
                escape_next = false;

            switch (brace_type)
            {
                case BraceType::Angle:
                    if (str[pos] == wxT('<'))
                        open_braces++;
                    break;
                case BraceType::Square:
                    if (str[pos] == wxT('['))
                        open_braces++;
                    break;
                case BraceType::None:
                default:
                    break;
            }
        }
        else
        {
            switch (brace_type)
            {
                case BraceType::Angle:
                    if (str[pos] == wxT('<'))
                        open_braces++;
                    else if (str[pos] == wxT('>'))
                        --open_braces;
                    break;
                case BraceType::Square:
                    if (str[pos] == wxT('['))
                        open_braces++;
                    else if (str[pos] == wxT(']'))
                        --open_braces;
                    break;
                case BraceType::Normal:
                    if (str[pos] == wxT('('))
                        open_braces++;
                    else if (str[pos] == wxT(')'))
                        --open_braces;
                    break;
                case BraceType::None:
                default:
                    break;
            }
        }
        ++pos;
    }

    if (in_quote)
    {
        token.end = -1;
        return false;
    }
    else
    {
        token.end = pos;
        return true;
    }
}

inline cb::shared_ptr<GDBWatch> AddChild(cb::shared_ptr<GDBWatch> parent, wxString const &full_value, Token &name)
{
    wxString const &str_name = name.ExtractString(full_value);
    cb::shared_ptr<cbWatch> old_child = parent->FindChild(str_name);
    cb::shared_ptr<GDBWatch> child;
    if (old_child)
        child = cb::static_pointer_cast<GDBWatch>(old_child);
    else
    {
        child = cb::shared_ptr<GDBWatch>(new GDBWatch(str_name));
        cbWatch::AddChild(parent, child);
    }
    child->MarkAsRemoved(false);
    return child;
}

inline cb::shared_ptr<GDBWatch> AddChild(cb::shared_ptr<GDBWatch> parent, wxString const &str_name)
{
    int index = parent->FindChildIndex(str_name);
    cb::shared_ptr<GDBWatch> child;
    if (index != -1)
        child = cb::static_pointer_cast<GDBWatch>(parent->GetChild(index));
    else
    {
        child = cb::shared_ptr<GDBWatch>(new GDBWatch(str_name));
        cbWatch::AddChild(parent, child);
    }
    child->MarkAsRemoved(false);
    return child;
}

wxRegEx regexRepeatedChar(wxT(".+[ \\t](<repeats[ \\t][0-9]+[ \\t]times>)$"));

inline bool ParseGDBWatchValue(cb::shared_ptr<GDBWatch> watch, wxString const &value, int &start, int length)
{
    watch->SetDebugValue(value);
    watch->MarkChildsAsRemoved();

    int position = start;
    Token token, token_name, token_value;
    wxString pythonToStringValue;
    bool skip_comma = false;
    bool last_was_closing_brace = false;
    int added_children = 0;
    int token_real_end = 0;
    while (GetNextToken(value, position, token))
    {
        token_real_end = token.end;
        token.Trim(value);
        const wxString &str = token.ExtractString(value);
        if (str.StartsWith(wxT("members of ")))
        {
            wxString::size_type pos = str.find(wxT('\n'));
            if (pos == wxString::npos)
            {
                // If the token has no '\n' character, then we have to search the whole value
                // for the token and then we skip this token completely.
                wxString::size_type pos_val = value.find(wxT('\n'), token_real_end);
                if (pos_val == wxString::npos)
                    return false;
                position = pos_val+1;
                if (length > 0 && position >= start + length)
                    break;
                continue;
            }
            else
            {
                // If we have the '\n' in the token, then we have the next valid token, too,
                // so we correct the current token to be the correct one.
                if (str.find_last_of(wxT(':'), pos) == wxString::npos)
                    return false;
                token.start += pos + 2;
                token.Trim(value);
            }
        }

        if (!token.hasRepeatedChar && regexRepeatedChar.Matches(str))
        {
            Token expanded_token = token;
            while (1)
            {
                if (value[expanded_token.end] == wxT(','))
                {
                    position = token.end + 1;
                    token_real_end = position;
                    int comma_end = expanded_token.end;
                    if (GetNextToken(value, position, expanded_token))
                    {
                        const wxString &expanded_str = expanded_token.ExtractString(value);
                        if (!expanded_str.empty() && (expanded_str[0] != wxT('"') && expanded_str[0] != wxT('\'')))
                        {
                            token.end = comma_end;
                            position = comma_end;
                            token_real_end = comma_end;
                            break;
                        }
                        token.end = expanded_token.end;
                        if (regexRepeatedChar.Matches(expanded_str))
                            continue;
                        token_real_end = expanded_token.end;
                    }
                }
                else if (expanded_token.end == static_cast<int>(value.length()))
                {
                    token.end = expanded_token.end;
                    token_real_end = expanded_token.end;
                }
                break;
            }
        }

        switch (token.type)
        {
        case Token::String:
            if (token_name.type == Token::Undefined)
                token_name = token;
            else if (token_value.type == Token::Undefined)
            {
                if (   wxIsdigit(str[0])
                    || str[0]==wxT('\'')
                    || str[0]==wxT('"')
                    || str[0]==wxT('<')
                    || str[0]==wxT('-')
                    || str.StartsWith(wxT("L\""))
                    || str.StartsWith(wxT("L'")) )
                {
                    token_value = token;
                }
                else
                {
                    // Detect strings generated by python pretty printing to_string() method.
                    Token expanded_token = token;
                    int firstCloseBrace = -1;
                    for (; expanded_token.end < static_cast<int>(value.length()); ++expanded_token.end)
                    {
                        if (value[expanded_token.end] == wxT('='))
                        {
                            bool foundBrace = false;
                            for (int ii = expanded_token.end + 1; ii < static_cast<int>(value.length()); ++ii)
                            {
                                if (value[ii] == wxT('{'))
                                {
                                    foundBrace = true;
                                    break;
                                }
                                else if (value[ii] != wxT(' ') && value[ii] != wxT('\t')
                                         && value[ii] != wxT('\n') && value[ii] != wxT(' '))
                                {
                                    break;
                                }

                            }
                            if (foundBrace)
                            {
                                token.end = token_real_end = expanded_token.end;
                                token_value = token;
                                token_value.end--;
                                pythonToStringValue = token_value.ExtractString(value);
                            }
                            else
                            {
                                while (expanded_token.end >= 0)
                                {
                                    if (value[expanded_token.end] == wxT(','))
                                    {
                                        token.end = token_real_end = expanded_token.end;
                                        token_value = token;
                                        pythonToStringValue = token_value.ExtractString(value);
                                        break;
                                    }
                                    expanded_token.end--;
                                }
                            }
                            break;
                        }
                        else if (firstCloseBrace == -1 && value[expanded_token.end] == wxT('}'))
                        {
                            firstCloseBrace=expanded_token.end;
                            break;
                        }
                    }

                    if (pythonToStringValue.empty())
                    {
                        if (firstCloseBrace == -1)
                            return false;
                        token.end = token_real_end = firstCloseBrace;
                        token_value = token;
                        pythonToStringValue = token_value.ExtractString(value);
                        if (pythonToStringValue.empty())
                            return false;
                    }
                }
            }
            else
                return false;
            last_was_closing_brace = false;
            break;
        case Token::Equal:
            last_was_closing_brace = false;
            break;
        case Token::Comma:
            pythonToStringValue = wxEmptyString;
            last_was_closing_brace = false;
            if (skip_comma)
                skip_comma = false;
            else
            {
                if (token_name.type != Token::Undefined)
                {
                    if (token_value.type != Token::Undefined)
                    {
                        cb::shared_ptr<GDBWatch> child = AddChild(watch, value, token_name);
                        child->SetValue(token_value.ExtractString(value));
                    }
                    else
                    {
                        int start_arr = watch->IsArray() ? watch->GetArrayStart() : 0;
                        cb::shared_ptr<GDBWatch> child = AddChild(watch, wxString::Format(wxT("[%d]"), start_arr + added_children));
                        child->SetValue(token_name.ExtractString(value));
                    }
                    token_name.type = token_value.type = Token::Undefined;
                    added_children++;
                }
                else
                    return false;
            }
            break;
        case Token::OpenBrace:
            {
                cb::shared_ptr<GDBWatch> child;
                if(token_name.type == Token::Undefined)
                {
                    int start_arr = watch->IsArray() ? watch->GetArrayStart() : 0;
                    child = AddChild(watch, wxString::Format(wxT("[%d]"), start_arr + added_children));
                }
                else
                    child = AddChild(watch, value, token_name);
                if (!pythonToStringValue.empty())
                    child->SetValue(pythonToStringValue);
                position = token_real_end;
                added_children++;

                if(!ParseGDBWatchValue(child, value, position, 0))
                    return false;
                token_real_end = position;
                token_name.type = token_value.type = Token::Undefined;
                skip_comma = true;
                last_was_closing_brace = true;
            }
            break;
        case Token::CloseBrace:
            if (!last_was_closing_brace)
            {
                if (token_name.type != Token::Undefined)
                {
                    if (token_value.type != Token::Undefined)
                    {
                        cb::shared_ptr<GDBWatch> child = AddChild(watch, value, token_name);
                        child->SetValue(token_value.ExtractString(value));
                    }
                    else
                    {
                        int start_arr = watch->IsArray() ? watch->GetArrayStart() : 0;
                        cb::shared_ptr<GDBWatch> child = AddChild(watch, wxString::Format(wxT("[%d]"),
                                                                                          start_arr + added_children));
                        child->SetValue(token_name.ExtractString(value));
                    }
                    token_name.type = token_value.type = Token::Undefined;
                    added_children++;
                }
                else
                    watch->SetValue(wxT(""));
            }

            start = token_real_end;
            return true;
        case Token::Undefined:
        default:
            return false;
        }

        position = token_real_end;
        if (length > 0 && position >= start + length)
            break;
    }

    start = position + 1;
    if (token_name.type != Token::Undefined)
    {
        if (token_value.type != Token::Undefined)
        {
            cb::shared_ptr<GDBWatch> child = AddChild(watch, value, token_name);
            child->SetValue(token_value.ExtractString(value));
        }
        else
        {
            int start_arr = watch->IsArray() ? watch->GetArrayStart() : 0;
            cb::shared_ptr<GDBWatch> child = AddChild(watch, wxString::Format(wxT("[%d]"), start_arr + added_children));
            child->SetValue(token_name.ExtractString(value));
        }
    }

    return true;
}

inline wxString RemoveWarnings(wxString const &input)
{
    wxString::size_type pos = input.find(wxT('\n'));

    if (pos == wxString::npos)
        return input;

    wxString::size_type lastPos = 0;
    wxString result;

    while (pos != wxString::npos)
    {
        wxString const &line = input.substr(lastPos, pos - lastPos);

        if (!line.StartsWith(wxT("warning:")))
        {
            result += line;
            result += wxT('\n');
        }

        lastPos = pos + 1;
        pos = input.find(wxT('\n'), lastPos);
    }

    if (lastPos < input.length())
        result += input.substr(lastPos, input.length() - lastPos);

    return result;
}

inline void RemoveBefore(wxString &str, const wxString &s)
{
    wxString::size_type pos = str.find(s);
    if (pos != wxString::npos)
    {
        str.Remove(0, pos+s.length());
        str.Trim(false);
    }
}

bool ParseGDBWatchValue(cb::shared_ptr<GDBWatch> watch, wxString const &inputValue)
{
    if(inputValue.empty())
    {
        watch->SetValue(inputValue);
        return true;
    }

    wxString value = RemoveWarnings(inputValue);

    // Try to find the first brace.
    // If the watch is for a reference the brace is not at position = 0
    wxString::size_type start = value.find(wxT('{'));

    if (start != wxString::npos && value[value.length() - 1] == wxT('}'))
    {
        // make sure the value is correct, even when the type has changed
        watch->SetValue(wxEmptyString);

        int t_start = start + 1;
        bool result = ParseGDBWatchValue(watch, value, t_start, value.length() - 2);
        if (result)
        {
            if (start > 0)
            {
                wxString referenceValue = value.substr(0, start);
                referenceValue.Trim(true);
                referenceValue.Trim(false);
                if (referenceValue.EndsWith(wxT("=")))
                {
                    referenceValue.RemoveLast(1);
                    referenceValue.Trim(true);
                }
                watch->SetValue(referenceValue);
            }
            watch->RemoveMarkedChildren();
        }
        return result;
    }
    else
    {
        watch->SetValue(value);
        watch->RemoveChildren();
        return true;
    }
    return false;
}

//
//    struct HWND__ * 0x7ffd8000
//
//    struct tagWNDCLASSEXA
//       +0x000 cbSize           : 0x7c8021b5
//       +0x004 style            : 0x7c802011
//       +0x008 lpfnWndProc      : 0x7c80b529     kernel32!GetModuleHandleA+0
//       +0x00c cbClsExtra       : 0
//       +0x010 cbWndExtra       : 2147319808
//       +0x014 hInstance        : 0x00400000
//       +0x018 hIcon            : 0x0012fe88
//       +0x01c hCursor          : 0x0040a104
//       +0x020 hbrBackground    : 0x689fa962
//       +0x024 lpszMenuName     : 0x004028ae  "???"
//       +0x028 lpszClassName    : 0x0040aa30  "CodeBlocksWindowsApp"
//       +0x02c hIconSm          : (null)
//
//    char * 0x0040aa30
//     "CodeBlocksWindowsApp"
//
//    char [16] 0x0012fef8
//    116 't'
//
//    int [10] 0x0012fee8
//    0

bool ParseCDBWatchValue(cb::shared_ptr<GDBWatch> watch, wxString const &value)
{
    wxArrayString lines = GetArrayFromString(value, wxT('\n'));
    watch->SetDebugValue(value);
    watch->MarkChildsAsRemoved();

    if (lines.GetCount() == 0)
        return false;

    static wxRegEx unexpected_error(wxT("^Unexpected token '.+'$"));
    static wxRegEx resolve_error(wxT("^Couldn't resolve error at '.+'$"));

    // search for errors
    for (unsigned ii = 0; ii < lines.GetCount(); ++ii)
    {
        if (unexpected_error.Matches(lines[ii])
            || resolve_error.Matches(lines[ii])
            || lines[ii] == wxT("No pointer for operator* '<EOL>'"))
        {
            watch->SetValue(lines[ii]);
            return true;
        }
    }

    if (lines.GetCount() == 1)
    {
        wxArrayString tokens = GetArrayFromString(lines[0], wxT(' '));
        if (tokens.GetCount() < 2)
            return false;

        int type_token = 0;
        if (tokens[0] == wxT("class") || tokens[0] == wxT("struct"))
            type_token = 1;

        if (static_cast<int>(tokens.GetCount()) < type_token + 2)
            return false;

        int value_start = type_token + 1;
        if (tokens[type_token + 1] == wxT('*'))
        {
            watch->SetType(tokens[type_token] + tokens[type_token + 1]);
            value_start++;
        }
        else
            watch->SetType(tokens[type_token]);

        if(value_start >= static_cast<int>(tokens.GetCount()))
            return false;

        watch->SetValue(tokens[value_start]);
        watch->RemoveMarkedChildren();
        return true;
    }
    else
    {
        wxArrayString tokens = GetArrayFromString(lines[0], wxT(' '));

        if (tokens.GetCount() < 2)
            return false;

        bool set_type = true;
        if (tokens.GetCount() > 2)
        {
            if (tokens[0] == wxT("struct") || tokens[0] == wxT("class"))
            {
                if (tokens[2] == wxT('*') || tokens[2].StartsWith(wxT("[")))
                {
                    watch->SetType(tokens[1] + tokens[2]);
                    set_type = false;
                }
            }
            else
            {
                if (tokens[1] == wxT('*') || tokens[1].StartsWith(wxT("[")))
                {

                    watch->SetType(tokens[0] + tokens[1]);
                    watch->SetValue(lines[1]);
                    return true;
                }
            }
        }

        if (set_type)
            watch->SetType(tokens[1]);

        static wxRegEx class_line(wxT("[ \\t]*\\+(0x[0-9a-f]+)[ \\t]([a-zA-Z0-9_]+)[ \\t]+:[ \\t]+(.+)"));
        if (!class_line.IsValid())
        {
            int *p = NULL;
            *p = 0;
        }
        else
        {
            if (!class_line.Matches(wxT("   +0x000 a                : 10")))
            {
                int *p = NULL;
                *p = 0;
            }
        }

        for (unsigned ii = 1; ii < lines.GetCount(); ++ii)
        {
            if (class_line.Matches(lines[ii]))
            {
                cb::shared_ptr<GDBWatch> w = AddChild(watch, class_line.GetMatch(lines[ii], 2));
                w->SetValue(class_line.GetMatch(lines[ii], 3));
                w->SetDebugValue(lines[ii]);
            }
        }
        watch->RemoveMarkedChildren();
        return true;
    }

    return false;
}

GDBLocalVariable::GDBLocalVariable(wxString const &nameValue, size_t start, size_t length)
{
    for (size_t ii = 0; ii < length; ++ii)
    {
        if (nameValue[start + ii] == wxT('='))
        {
            name = nameValue.substr(start, ii);
            name.Trim();
            value = nameValue.substr(start + ii + 1, length - ii - 1);
            value.Trim(false);
            error = false;
            return;
        }
    }
    error = true;
}

void TokenizeGDBLocals(std::vector<GDBLocalVariable> &results, wxString const &value)
{
    size_t count = value.length();
    size_t start = 0;
    int curlyBraces = 0;
    bool inString = false, inChar = false;
    bool escaped = false;

    for (size_t ii = 0; ii < count; ++ii)
    {
        wxChar ch = value[ii];
        switch (ch)
        {
        case wxT('\n'):
            if (!inString && !inChar && curlyBraces == 0)
            {
                results.push_back(GDBLocalVariable(value, start, ii - start));
                start = ii + 1;
            }
            break;
        case wxT('{'):
            if (!inString && !inChar)
                curlyBraces++;
            break;
        case wxT('}'):
            if (!inString && !inChar)
                curlyBraces--;
            break;
        case wxT('"'):
            if (!inChar && !escaped)
                inString=!inString;
            break;
        case wxT('\''):
            if (!inString && !escaped)
                inChar=!inChar;
            break;
        default:
            break;
        }

        escaped = (ch == wxT('\\') && !escaped);
    }
    results.push_back(GDBLocalVariable(value, start, value.length() - start));
}
