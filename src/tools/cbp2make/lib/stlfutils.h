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
#ifndef STL_FUTILS_H
#define STL_FUTILS_H
//------------------------------------------------------------------------------
#include "macros.h"
#ifdef OS_WIN
#include <io.h>
#endif
//------------------------------------------------------------------------------
#include "stlstrings.h"
//------------------------------------------------------------------------------
char PathDelimiter(void);
CString NativePathDelimiter(void);
CString ChangeFileExt(const CString& FileName, const CString& FileExt);
CString ExtractFilePath(const CString& FilePathName);
CString ExtractFileName(const CString& FilePathName);
CString ExtractFileExt(const CString& FilePathName);
void SplitFilePathName(const CString& FilePathName, CString& FilePath, CString& FileName, CString& FileExt);
CString MakeUnixPath(const CString& FilePathName);
CString MakeWindowsPath(const CString& FilePathName);
CString MakeNativePath(const CString& FilePathName);
CString RemoveLeadingPathDelimiter(const CString& FilePathName);
CString IncludeLeadingPathDelimiter(const CString& FilePathName);
CString RemoveTrailingPathDelimiter(const CString& FilePathName);
CString IncludeTrailingPathDelimiter(const CString& FilePathName);
CString QuoteSpaces(const CString& FilePathName, const int QuoteMode = 0);
CString JoinPaths(const CString& HeadPath, const CString& TailPath, const char Separator = PathDelimiter());

bool FileExists(const CString& FileName);
bool DirExists(const CString& FileName);
bool MakeDir(const CString& DirPath, int DirMode = 0755);
CString RootDirPath(void);
CString HomeDirPath(void);
CString TempDirPath(void);

CString GetCurrentDir(void);
bool ChangeDir(const CString& DirPath);
//------------------------------------------------------------------------------
#endif
