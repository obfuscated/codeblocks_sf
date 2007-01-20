/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxspropertystream.h"

bool wxsPropertyStream::GetChar(const wxString &Name, wxChar& Value, wxChar Default)
{
    wxString Data;
    if ( !GetString(Name,Data,wxEmptyString) || Data.empty() )
    {
        Value = Default;
        return false;
    }
    Value = Data[0];
    return true;
}

bool wxsPropertyStream::PutChar(const wxString &Name, wxChar& Value, wxChar Default)
{
    wxString Data = wxString::Format(_T("%c"),Value);
    if ( !PutString(Name,Data,wxString::Format(_T("%c"),Default)) ) return false;
    Value = Data.empty() ? Default : Data[0];
    return true;
}

bool wxsPropertyStream::GetLong(const wxString &Name, long& Value, long Default)
{
    wxString Data;
    if ( !GetString(Name,Data,wxEmptyString) || Data.empty() )
    {
        Value = Default;
        return false;
    }
    Data.ToLong(&Value);
    return true;
}

bool wxsPropertyStream::PutLong(const wxString &Name, long& Value, long Default)
{
    wxString Data = wxString::Format(_T("%d"),Value);
    if ( !PutString(Name,Data,wxString::Format(_T("%d"),Default)) ) return false;
    Data.ToLong(&Value);
    return true;
}

bool wxsPropertyStream::GetDouble(const wxString &Name, double& Value, double Default)
{
    wxString Data;
    if ( !GetString(Name,Data,wxEmptyString) || Data.empty() )
    {
        Value = Default;
        return false;
    }
    Data.ToDouble(&Value);
    return true;
}

bool wxsPropertyStream::PutDouble(const wxString &Name, double& Value, double Default)
{
    wxString Data = wxString::Format(_T("%f"),Value);
    if ( !PutString(Name,Data,wxString::Format(_T("%f"),Default)) ) return false;
    Data.ToDouble(&Value);
    return true;
}

bool wxsPropertyStream::GetULong(const wxString &Name, unsigned long& Value, unsigned long Default)
{
    wxString Data;
    if ( !GetString(Name,Data,wxEmptyString) || Data.empty() )
    {
        Value = Default;
        return false;
    }
    Data.ToULong(&Value);
    return true;
}

bool wxsPropertyStream::PutULong(const wxString &Name, unsigned long& Value, unsigned long Default)
{
    wxString Data = wxString::Format(_T("%u"),Value);
    if ( !PutString(Name,Data,wxString::Format(_T("%u"),Default)) ) return false;
    Data.ToULong(&Value);
    return true;
}

bool wxsPropertyStream::GetBool(const wxString &Name, bool& Value, bool Default)
{
    wxString Data;
    if ( !GetString(Name,Data,wxEmptyString) || Data.empty() )
    {
        Value = Default;
        return false;
    }
    long ValueL;
    Data.ToLong(&ValueL);
    Value = ValueL!=0;
    return true;
}

bool wxsPropertyStream::PutBool(const wxString &Name, bool& Value, bool Default)
{
    wxString Data = Value ? _T("1") : _T("0");
    if ( !PutString(Name,Data,Default?_T("1"):_T("0")) ) return false;
    long ValueL;
    Data.ToLong(&ValueL);
    Value = ValueL!=0;
    return true;
}
