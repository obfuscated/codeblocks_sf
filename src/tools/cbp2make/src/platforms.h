/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)
    Copyright (C) 2014      Sergey "dmpas" Batanov (sergey.batanov (at) dmpas (dot) ru)

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
#ifndef PLATFORMS_H
#define PLATFORMS_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "cbhelper.h"
//------------------------------------------------------------------------------

static const CString STR_UNIX     = "Unix";
static const CString STR_WINDOWS  = "Windows";
static const CString STR_MAC      = "Mac";
static const CString STR_MSYS     = "MSys";

class TiXmlElement;
class CPlatformSet;

class CPlatform
{
public:
    enum OS_Type {
        OS_Other,
        OS_Unix,
        OS_Windows,
        OS_Mac,
        OS_MSys,
        OS_Count
    };
private:
    bool m_Active;
    OS_Type m_OS_Type;
    CString m_Cmd_Null;
    CString m_Cmd_Copy;
    CString m_Cmd_Move;
    CString m_Cmd_Make;
    CString m_Tool_Make;
    //CString m_Cmd_TestFile;
    CString m_Cmd_RemoveFile;
    CString m_Cmd_ForceRemoveFile;
    //CString m_Cmd_TestDir;
    CString m_Cmd_MakeDir;
    CString m_Cmd_TestMakeDir;
    CString m_Cmd_ForceMakeDir;
    CString m_Cmd_RemoveDir;
    CString m_Cmd_PrintWorkDir;
    CString m_Cmd_EvalWorkDir;
    CString m_Cmd_ChangeDir;
    char m_PathDelimiter;
    //
    CStringList m_StaticLibraryExtensions;
    CStringList m_DynamicLibraryExtensions;
    //CString m_;
public:
    static CString Name(const OS_Type PlatformOS);
    static OS_Type OS(CString& PlatformName);
    bool& Active(void)
    {
        return m_Active;
    }
    OS_Type  OS(void) const
    {
        return m_OS_Type;
    }
    CString  Name(void);
    CString& Cmd_Copy(void)
    {
        return m_Cmd_Copy;
    }
    CString& Cmd_Move(void)
    {
        return m_Cmd_Move;
    }
    CString& Cmd_Make(void)
    {
        return m_Cmd_Make;
    }
    CString& Tool_Make(void)
    {
        return m_Tool_Make;
    }
    //CString& Cmd_TestFile(void)     { return m_Cmd_TestFile; }
    CString& Cmd_RemoveFile(void)
    {
        return m_Cmd_RemoveFile;
    }
    CString& Cmd_ForceRemoveFile(void)
    {
        return m_Cmd_ForceRemoveFile;
    }
    //CString& Cmd_TestDir(void)         { return m_Cmd_TestDir; }
    CString& Cmd_MakeDir(void)
    {
        return m_Cmd_MakeDir;
    }
    CString& Cmd_TestMakeDir(void)
    {
        return m_Cmd_TestMakeDir;
    }
    CString& Cmd_ForceMakeDir(void)
    {
        return m_Cmd_ForceMakeDir;
    }
    CString& Cmd_RemoveDir(void)
    {
        return m_Cmd_RemoveDir;
    }
    CString& Cmd_PrintWorkDir(void)
    {
        return m_Cmd_PrintWorkDir;
    }
    CString& Cmd_EvalWorkDir(void)
    {
        return m_Cmd_EvalWorkDir;
    }
    CString& Cmd_ChangeDir(void)
    {
        return m_Cmd_ChangeDir;
    }
    char Pd(void) const
    {
        return m_PathDelimiter;
    }
    CString Pd(const CString& Path) const;
    CString SpecialChars(void) const;
    CString ProtectPath(const CString& Path, const int QuoteMode = QUOTE_AUTO);
    CString Copy(const CString& Source, const CString& Destination) const;
    CString Move(const CString& Source, const CString& Destination) const;
    CString Make(const CString& Options, const CString& Path) const;
    CString RemoveFile(const CString& Path) const;
    CString ForceRemoveFile(const CString& Path) const;
    CString MakeDir(const CString& Path) const;
    CString TestMakeDir(const CString& Path) const;
    CString ForceMakeDir(const CString& Path) const;
    CString RemoveDir(const CString& Path) const;
    CString PrintWorkDir(void) const;
    CString EvalWorkDir(void) const;
    CString ChangeDir(const CString& Path) const;
    CString MakefileCmd(const CString& Command) const;
    //
    bool IsStaticLibraryExtension(const CString& Ext) const;
    bool IsDynamicLibraryExtension(const CString& Ext) const;
    //CString (void) const { return m_; }
private:
    void Read(const TiXmlElement *Root, const CString& Name, CString& Value);
    void Write(TiXmlElement *Root, const CString& Name, const CString& Value);
public:
    void Assign(const CPlatform& Platform);
    void Clear(void);
    void Reset(const CPlatform::OS_Type OS);
    void Read(const TiXmlElement *PlatformRoot);
    void Write(TiXmlElement *PlatformRoot);
    void Show(void);
    CPlatform& operator =(const CPlatform& Platform);
public:
    CPlatform(void);
    CPlatform(const CPlatform& Platform);
    ~CPlatform(void);
};

class CPlatformSet
{
private:
    std::vector<CPlatform *> m_Platforms;
    bool m_Locked;
public:
    void Lock(void);
    void Unlock(void);
    void Clear(void);
    size_t GetCount(void) const;
    CPlatform *Platform(const size_t Index) const;
    CPlatform *Find(const CPlatform::OS_Type OS);
    void AddDefault(void);
    //CPlatform *Add(const CPlatform::OS_Type OS);
    //void Remove(const CPlatform::OS_Type OS);
    void Read(const TiXmlElement *ConfigRoot);
    void Write(TiXmlElement *ConfigRoot);
    //bool Load(const CString& FileName);
    //bool Save(const CString& FileName);
    void Show(void);
public:
    CPlatformSet(void);
    ~CPlatformSet(void);
};

#endif
//------------------------------------------------------------------------------
