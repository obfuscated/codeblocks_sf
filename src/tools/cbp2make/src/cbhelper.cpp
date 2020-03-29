/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#include "cbhelper.h"
//------------------------------------------------------------------------------

void ShowStringList(const CString& Title, const CString& LinePrefix,
                    const CStringList& StringList)
{
    std::cout<<Title.GetCString()<<": "<<StringList.GetCount()<<std::endl;
    for (int i = 0, n = StringList.GetCount(); i < n; i++) {
        std::cout<<LinePrefix.GetCString()<<" #"<<(i+1)<<": "<<StringList[i].GetString()<<std::endl;
    }
}

CString MakefileFriendly(const CString& AString)
{
    CString result = AString;
    CCharset c("01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
    for (int i = 0; i < result.GetLength(); i++) {
        if (!c.Isset(result[i])) result[i] = '_';
    }
    return result;//UpperCase(result);
}

CString XMLFriendly(const CString& AString)
{
    return LowerCase(MakefileFriendly(AString));
}

CString FlatFileName(const CString& FileName)
{
    return FindReplaceChar(FindReplaceChar(FileName,'/','_'),'\\','_');
}

//------------------------------------------------------------------------------
