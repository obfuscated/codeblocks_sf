/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxscodinglang.h"
#include <logmanager.h>

namespace wxsCodeMarks
{
    wxString Name(wxsCodingLang Lang)
    {
        switch ( Lang )
        {
            case wxsCPP: return _T("CPP");
            default:;
        }
        return wxEmptyString;
    }

    wxsCodingLang Id(const wxString& Name)
    {
        if ( Name == _T("CPP") ) return wxsCPP;
        return wxsUnknownLanguage;
    }

    wxsCodingLang IdFromExt(const wxString& Extension)
    {
        wxString ExtLower = Extension.Lower();
        if ( (ExtLower==_T("c")) ||
             (ExtLower==_T("h")) ||
             (ExtLower==_T("cpp")) ||
             (ExtLower==_T("hpp")) ) return wxsCPP;
        return wxsUnknownLanguage;
    }

    wxString Beg(wxsCodingLang Lang,const wxString& BlockName)
    {
        switch ( Lang )
        {
            case wxsCPP: return _T("//(*") + BlockName;
            default:     return wxEmptyString;
        }
    }

    wxString Beg(wxsCodingLang Lang,const wxString& BlockName,const wxString& Param)
    {
        switch ( Lang )
        {
            case wxsCPP: return wxString::Format(_T("//(*%s(%s)"),BlockName.c_str(),Param.c_str());
            default:     return wxEmptyString;
        }
    }

    wxString End(wxsCodingLang Lang)
    {
        switch ( Lang )
        {
            case wxsCPP: return _T("//*)");
            default:     return wxEmptyString;
        }
    }

    void Unknown(const wxString& Function,wxsCodingLang Lang)
    {
        Manager::Get()->GetLogManager()->DebugLog(F(
            _T("Unknown coding language %s (%d) in function %s"),
            Name(Lang).c_str(),
            (int)Lang,
            Function.c_str()));
    }

    wxString String(wxsCodingLang Lang,const wxString& Source)
    {
        switch ( Lang )
        {
            case wxsCPP:
            {
                wxString Result = _T("\"");

                int Len = Source.Length();

                for ( int i=0; i<Len; i++ )
                {
                    wxChar ch = Source.GetChar(i);

                    if ( (unsigned)ch < _T(' ') )
                    {
                        switch ( ch )
                        {
                            case _T('\n') : Result.Append(_T("\\n")); break;
                            case _T('\t') : Result.Append(_T("\\t")); break;
                            case _T('\v') : Result.Append(_T("\\v")); break;
                            case _T('\b') : Result.Append(_T("\\b")); break;
                            case _T('\r') : Result.Append(_T("\\r")); break;
                            case _T('\f') : Result.Append(_T("\\f")); break;
                            case _T('\a') : Result.Append(_T("\\a")); break;
                            default   :
                                {
                                    wxString Formater = wxString::Format(_T("\\%d%d%d"),
                                        ( ch >> 6 ) & 7,
                                        ( ch >> 3 ) & 7,
                                        ( ch >> 0 ) & 7 );
                                    Result.Append(Formater.c_str());
                                }
                        }
                    }
                    else
                    {
                        switch ( ch )
                        {
                            case _T('\\'): Result.Append(_T("\\\\")); break;
                            case _T('\?'): Result.Append(_T("\\\?")); break;
                            case _T('\''): Result.Append(_T("\\\'")); break;
                            case _T('\"'): Result.Append(_T("\\\"")); break;
                            default  : Result.Append(ch);
                        }
                    }
                }

                Result.Append(_T('\"'));
                return Result;
            }

            default:
            {
                Unknown(_T("wxsCodeMarks::String"),Lang);
            }
        }
        return wxEmptyString;
    }

    wxString WxString(wxsCodingLang Lang, const wxString& Source, bool WithTranslation)
    {
        switch ( Lang )
        {
            case wxsCPP:
            {
                if ( Source.empty() )
                {
                    // Always empty string, no matter if we have translation
                    return _T("wxEmptyString");
                }

                if ( WithTranslation )
                {
                    return _T("_(") + String(Lang,Source) + _T(")");
                }
                else
                {
                    return _T("_T(") + String(Lang,Source) + _T(")");
                }
            }

            default:
            {
                Unknown(_T("wxsCodeMarks::WxString"),Lang);
            }
        }
        return wxEmptyString;
    }

    namespace
    {
        /** \brief Set of names which can not be used as widget names in C++
         *
         * This names must be placed in alphabetical order
         */
        static const wxChar* DeadNamesCPP[] =
        {
            _T("asm"), _T("auto"), _T("bool"), _T("break"), _T("case"), _T("catch"),
            _T("char"), _T("class"), _T("const"), _T("const_cast"), _T("continue"),
            _T("default"), _T("delete"), _T("do"), _T("double"), _T("dynamic_cast"),
            _T("else"), _T("enum"), _T("explicit"), _T("export"), _T("extern"),
            _T("false"), _T("float"), _T("for"), _T("friend"), _T("goto"), _T("if"),
            _T("inline"), _T("int"), _T("long"), _T("mutable"), _T("namespace"),
            _T("new"), _T("operator"), _T("private"), _T("protected"), _T("public"),
            _T("register"), _T("reinterpret_cast"), _T("return"), _T("short"),
            _T("signed"), _T("sizeof"), _T("sizeritem"), _T("static"),
            _T("static_cast"), _T("struct"), _T("switch"), _T("template"), _T("this"),
            _T("throw"), _T("true"), _T("try"), _T("typedef"), _T("typeid"),
            _T("typename"), _T("union"), _T("unsigned"), _T("using"), _T("virtual"),
            _T("void"), _T("volatile"), _T("wchar_t"), _T("while")
        };

        /** \brief Number of enteries in array of dead names */
        static const int DeadNamesCPPLen = sizeof(DeadNamesCPP) / sizeof(DeadNamesCPP[0]);
    }

    bool ValidateIdentifier(wxsCodingLang Lang, const wxString& NameStr)
    {
        switch ( Lang )
        {
            case wxsCPP:
            {
                const wxChar* Name = NameStr.c_str();
                if ( !Name ) return false;

                if (( *Name < _T('a') || *Name > _T('z') ) &&
                    ( *Name < _T('A') || *Name > _T('Z') ) &&
                    ( *Name != _T('_') ))
                {
                    return false;
                }

                while ( *++Name )
                {
                    if (( *Name < _T('a') || *Name > _T('z') ) &&
                        ( *Name < _T('A') || *Name > _T('Z') ) &&
                        ( *Name < _T('0') || *Name > _T('9') ) &&
                        ( *Name != _T('_') ))
                    {
                        return false;
                    }
                }

                int Begin = 0;
                int End = DeadNamesCPPLen-1;

                while ( Begin <= End )
                {
                    int Middle = ( Begin + End ) >> 1;

                    int Res = wxStrcmp(DeadNamesCPP[Middle],NameStr);

                    if ( Res < 0 )
                    {
                        Begin = Middle+1;
                    }
                    else if ( Res > 0 )
                    {
                        End = Middle-1;
                    }
                    else
                    {
                        return false;
                    }

                }

                return true;
            }

            default:
            {
                Unknown(_T("wxscodeMarks::ValidateIdentifier"),Lang);
            }
        }

        return false;
    }

}
