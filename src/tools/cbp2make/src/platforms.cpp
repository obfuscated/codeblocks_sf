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
#include <iostream>
//------------------------------------------------------------------------------
#include "platforms.h"
#include "stlconvert.h"
#include "stlfutils.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CPlatform::CPlatform(void)
{
    Clear();
}

CPlatform::CPlatform(const CPlatform& Platform)
{
    Assign(Platform);
}

CPlatform::~CPlatform(void)
{
    Clear();
}

void CPlatform::Clear(void)
{
    m_OS_Type = OS_Unix;
    m_Active = false;
    m_Cmd_Null.Clear();
    m_Cmd_Copy.Clear();
    m_Cmd_Move.Clear();
    m_Cmd_Make.Clear();
    m_Tool_Make.Clear();
//m_Cmd_TestFile.Clear();
    m_Cmd_RemoveFile.Clear();
    m_Cmd_ForceRemoveFile.Clear();
//m_Cmd_TestDir.Clear();
    m_Cmd_MakeDir.Clear();
    m_Cmd_TestMakeDir.Clear();
    m_Cmd_ForceMakeDir.Clear();
    m_Cmd_RemoveDir.Clear();
    m_Cmd_PrintWorkDir.Clear();
    m_Cmd_EvalWorkDir.Clear();
    m_Cmd_ChangeDir.Clear();
    m_PathDelimiter = '/';
}

void CPlatform::Assign(const CPlatform& Platform)
{
    m_OS_Type  = Platform.m_OS_Type;
    m_Active   = Platform.m_Active;
    m_Cmd_Null = Platform.m_Cmd_Null;
    m_Cmd_Copy = Platform.m_Cmd_Copy;
    m_Cmd_Move = Platform.m_Cmd_Move;
    m_Cmd_Make = Platform.m_Cmd_Make;
    m_Tool_Make = Platform.m_Tool_Make;
//m_Cmd_TestFile        = Platform.m_Cmd_TestFile;
    m_Cmd_RemoveFile      = Platform.m_Cmd_RemoveFile;
    m_Cmd_ForceRemoveFile = Platform.m_Cmd_ForceRemoveFile;
//m_Cmd_TestDir    = Platform.m_Cmd_TestDir;
    m_Cmd_MakeDir      = Platform.m_Cmd_MakeDir;
    m_Cmd_TestMakeDir  = Platform.m_Cmd_TestMakeDir;
    m_Cmd_ForceMakeDir = Platform.m_Cmd_ForceMakeDir;
    m_Cmd_RemoveDir    = Platform.m_Cmd_RemoveDir;
    m_Cmd_PrintWorkDir = Platform.m_Cmd_PrintWorkDir;
    m_Cmd_EvalWorkDir  = Platform.m_Cmd_EvalWorkDir;
    m_Cmd_ChangeDir    = Platform.m_Cmd_ChangeDir;
    m_PathDelimiter    = Platform.m_PathDelimiter;
}

CString CPlatform::Name(const OS_Type PlatformOS)
{
    switch (PlatformOS) {
    default:
    case CPlatform::OS_Other:
    case CPlatform::OS_Count:
    { }
    case CPlatform::OS_Unix: {
        return STR_UNIX;
    }
    case CPlatform::OS_Windows: {
        return STR_WINDOWS;
    }
    case CPlatform::OS_Mac: {
        return STR_MAC;
    }
    case CPlatform::OS_MSys: {
        return STR_MSYS;
    }
    }
    return "Other";
}

CPlatform::OS_Type CPlatform::OS(CString& PlatformName)
{
    if (PlatformName==STR_UNIX) return CPlatform::OS_Unix;
    if (PlatformName==STR_WINDOWS) return CPlatform::OS_Windows;
    if (PlatformName==STR_MAC) return CPlatform::OS_Mac;
    if (PlatformName==STR_MSYS) return CPlatform::OS_MSys;
    return CPlatform::OS_Other;
}

CString CPlatform::Name(void)
{
    return Name(m_OS_Type);
}

CString CPlatform::Pd(const CString& Path) const
{
    if (m_OS_Type==OS_Windows) {
        return MakeWindowsPath(Path);
    } else {
        return MakeUnixPath(Path);
    }
}

CString CPlatform::SpecialChars(void) const
{
    if (m_OS_Type==OS_Windows) {
        return " ";
    } else {
        return ALPHABET_SHELL_CHARS;
    }
}

CString CPlatform::ProtectPath(const CString& Path, const int QuoteMode)
{
   (void)QuoteMode;
    if (Path.GetLength() > 0) {
        if (Path.GetFirstChar() == '$') {
            /* Do not change paths like '$(SOME_VAR)' */
            return Path;
        }
    }
    if (m_OS_Type==OS_Windows) {
        //return QuoteSpaces(Path,QuoteMode);
        return ShieldChars(Path,this->SpecialChars());
    } else {
        return ShieldChars(Path,this->SpecialChars());
    }
}

CString CPlatform::Copy(const CString& Source, const CString& Destination) const
{
    return FindReplaceStr(FindReplaceStr(m_Cmd_Copy,"$src",Source),"$dst",Destination);
}

CString CPlatform::Move(const CString& Source, const CString& Destination) const
{
    return FindReplaceStr(FindReplaceStr(m_Cmd_Move,"$src",Source),"$dst",Destination);
}

CString CPlatform::Make(const CString& Options, const CString& Path) const
{
    return FindReplaceStr(FindReplaceStr(m_Cmd_Make,"$opts",Options),"$file",Path);
}

CString CPlatform::RemoveFile(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_RemoveFile,"$file",Path);
}

CString CPlatform::ForceRemoveFile(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_ForceRemoveFile,"$file",Path);
}

CString CPlatform::MakeDir(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_MakeDir,"$dir",Path);
}

CString CPlatform::TestMakeDir(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_TestMakeDir,"$dir",Path);
}

CString CPlatform::ForceMakeDir(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_ForceMakeDir,"$dir",Path);
}

CString CPlatform::RemoveDir(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_RemoveDir,"$dir",Path);
}

CString CPlatform::PrintWorkDir(void) const
{
    return m_Cmd_PrintWorkDir;
}

CString CPlatform::EvalWorkDir(void) const
{
    return m_Cmd_EvalWorkDir;
}

CString CPlatform::ChangeDir(const CString& Path) const
{
    return FindReplaceStr(m_Cmd_ChangeDir,"$dir",Path);
}

CString CPlatform::MakefileCmd(const CString& Command) const
{
    if (OS_Windows == m_OS_Type) return "cmd /c "+Command;
    return Command;
}

bool CPlatform::IsStaticLibraryExtension(const CString& Ext) const
{
    return (m_StaticLibraryExtensions.FindString(Ext) >= 0);
}

bool CPlatform::IsDynamicLibraryExtension(const CString& Ext) const
{
    return (m_DynamicLibraryExtensions.FindString(Ext) >= 0);
}

void CPlatform::Reset(const CPlatform::OS_Type OS)
{
    m_OS_Type = OS;
    switch (m_OS_Type) {
    default:
    case CPlatform::OS_Unix: {
        m_Cmd_Null = "/dev/null";
        m_Cmd_Copy = "cp -p $src $dst";
        m_Cmd_Move = "mv $src $dst";
        m_Cmd_Make = "make $opts -f $file";
        m_Tool_Make = "make";
        //m_Cmd_TestFile = "test -f $file";
        m_Cmd_RemoveFile = "rm $file";
        m_Cmd_ForceRemoveFile = "rm -f $file";
        //m_Cmd_TestDir = "test -d $dir";
        m_Cmd_MakeDir = "mkdir $dir";
        m_Cmd_TestMakeDir = "test -d $dir || mkdir -p $dir";
        m_Cmd_ForceMakeDir = "mkdir -p $dir";
        m_Cmd_RemoveDir = "rm -rf $dir";
        m_Cmd_PrintWorkDir = "pwd";
        m_Cmd_EvalWorkDir = "`pwd`";
        m_Cmd_ChangeDir = "cd $dir";
        m_PathDelimiter = '/';
        //
        m_StaticLibraryExtensions.Clear()<<"a"<<"lib";
        m_DynamicLibraryExtensions.Clear()<<"so";
        break;
    }
    case CPlatform::OS_MSys: {
        m_Cmd_Null = "/dev/null";
        m_Cmd_Copy = "cp -p $src $dst";
        m_Cmd_Move = "mv $src $dst";
        m_Cmd_Make = "make $opts -f $file";
        m_Tool_Make = "make";
        //m_Cmd_TestFile = "test -f $file";
        m_Cmd_RemoveFile = "rm $file";
        m_Cmd_ForceRemoveFile = "rm -f $file";
        //m_Cmd_TestDir = "test -d $dir";
        m_Cmd_MakeDir = "mkdir $dir";
        m_Cmd_TestMakeDir = "test -d $dir || mkdir -p $dir";
        m_Cmd_ForceMakeDir = "mkdir -p $dir";
        m_Cmd_RemoveDir = "rm -rf $dir";
        m_Cmd_PrintWorkDir = "pwd";
        m_Cmd_EvalWorkDir = "`pwd`";
        m_Cmd_ChangeDir = "cd $dir";
        m_PathDelimiter = '/';
        //
        m_StaticLibraryExtensions.Clear() << "a" << "lib";
        m_DynamicLibraryExtensions.Clear() << "dll";
        break;
    }
    case CPlatform::OS_Windows: {
        m_Cmd_Null = "NUL";
        m_Cmd_Copy = "copy $src $dst";
        m_Cmd_Move = "move $src $dst";
        m_Cmd_Make = "make $opts -f $file";
        m_Tool_Make = "make";
        //m_Cmd_TestFile = "if exist $file";
        m_Cmd_RemoveFile = "del $file";
        m_Cmd_ForceRemoveFile = "del /f $file";
        //m_Cmd_TestDir = "if exist $dir";
        m_Cmd_MakeDir = "md $dir";
        m_Cmd_TestMakeDir = "if not exist $dir md $dir";
        m_Cmd_ForceMakeDir = "md $dir";
        m_Cmd_RemoveDir = "rd $dir";
        m_Cmd_PrintWorkDir = "echo %cd%";
        m_Cmd_EvalWorkDir = "%cd%";
        m_Cmd_ChangeDir = "cd $dir";
        m_PathDelimiter = '\\';
        //
        m_StaticLibraryExtensions.Clear()<<"lib"<<"a";
        m_DynamicLibraryExtensions.Clear()<<"dll";
        break;
    }
    case CPlatform::OS_Mac: {
        m_Cmd_Null = "/dev/null";
        m_Cmd_Copy = "cp -p $src $dst";
        m_Cmd_Move = "mv $src $dst";
        m_Cmd_Make = "make $opts -f $file";
        m_Tool_Make = "make";
        //m_Cmd_TestFile = "test -f $file";
        m_Cmd_RemoveFile = "rm $file";
        m_Cmd_ForceRemoveFile = "rm -f $file";
        //m_Cmd_TestDir = "test -d $dir";
        m_Cmd_MakeDir = "mkdir $dir";
        m_Cmd_TestMakeDir = "test -d $dir || mkdir -p $dir";
        m_Cmd_ForceMakeDir = "mkdir -p $dir";
        m_Cmd_RemoveDir = "rm -rf $dir";
        m_Cmd_PrintWorkDir = "pwd";
        m_Cmd_EvalWorkDir = "`pwd`";
        m_Cmd_ChangeDir = "cd $dir";
        m_PathDelimiter = '/';
        //
        m_StaticLibraryExtensions.Clear()<<"a";
        m_DynamicLibraryExtensions.Clear()<<"dylib";
        break;
    }
    }
}

void CPlatform::Read(const TiXmlElement *Root, const CString& Name, CString& Value)
{
    TiXmlNode *_command = (TiXmlNode *)Root->FirstChild("command");
    while (0!=_command) {
        TiXmlElement* command = _command->ToElement();
        //if (strcmp(command->Value(),"command")!=0) break;
        if (0!=command) {
            char *value = 0;
            if ((value = (char *)command->Attribute(Name.GetCString()))) {
                Value = value;
            }
        }
        _command = (TiXmlNode *)Root->IterateChildren(_command);
    } // command
}

void CPlatform::Read(const TiXmlElement *PlatformRoot)
{
    char *value = 0;
    if ((value = (char *)PlatformRoot->Attribute("name"))) {
        CString name = value;
        m_OS_Type = OS(name);
    }
    Reset(m_OS_Type);
    if ((value = (char *)PlatformRoot->Attribute("path_delimiter"))) {
        m_PathDelimiter = value[0];
    }
    /*
    if ((value = (char *)PlatformRoot->Attribute("")))
    {
     m_ = value;
    }
    */
    Read(PlatformRoot, "make_file",             m_Cmd_Make);
    Read(PlatformRoot, "make_tool",             m_Tool_Make);
    Read(PlatformRoot, "copy_file",             m_Cmd_Copy);
    Read(PlatformRoot, "move_file",             m_Cmd_Move);
    Read(PlatformRoot, "remove_file",           m_Cmd_RemoveFile);
    Read(PlatformRoot, "force_remove_file",     m_Cmd_ForceRemoveFile);
    Read(PlatformRoot, "make_dir",              m_Cmd_MakeDir);
    Read(PlatformRoot, "test_make_dir",         m_Cmd_TestMakeDir);
    Read(PlatformRoot, "force_make_dir",        m_Cmd_ForceMakeDir);
    Read(PlatformRoot, "remove_dir",            m_Cmd_RemoveDir);
    Read(PlatformRoot, "print_work_dir",        m_Cmd_PrintWorkDir);
    Read(PlatformRoot, "eval_work_dir",         m_Cmd_EvalWorkDir);
    Read(PlatformRoot, "change_dir",            m_Cmd_ChangeDir);
    {
        CString s;
        CStringList l;
        Read(PlatformRoot, "static_lib_ext", s);
        if (!s.IsEmpty()) {
            ParseStr(s, ' ', l);
            l.RemoveDuplicates();
            l.RemoveEmpty();
            if (!l.IsEmpty()) {
                m_StaticLibraryExtensions = l;
            }
        }
    }
    {
        CString s;
        CStringList l;
        Read(PlatformRoot, "dynamic_lib_ext", s);
        if (!s.IsEmpty()) {
            ParseStr(s, ' ', l);
            l.RemoveDuplicates();
            l.RemoveEmpty();
            if (!l.IsEmpty()) {
                m_DynamicLibraryExtensions = l;
            }
        }
    }
    //Read(PlatformRoot,"",m_);
}

void CPlatform::Write(TiXmlElement *Root, const CString& Name, const CString& Value)
{
    TiXmlElement *command = new TiXmlElement("command");
    command->SetAttribute(Name.GetCString(), Value.GetCString());
    Root->LinkEndChild(command);
}

void CPlatform::Write(TiXmlElement *PlatformRoot)
{
    PlatformRoot->SetAttribute("name",Name().GetCString());
    PlatformRoot->SetAttribute("path_delimiter",CString(m_PathDelimiter).GetCString());
    //PlatformRoot->SetAttribute("",m_.GetCString());
    Write(PlatformRoot,"make_file",m_Cmd_Make);
    Write(PlatformRoot,"make_tool",m_Tool_Make);
    Write(PlatformRoot,"copy_file",m_Cmd_Copy);
    Write(PlatformRoot,"move_file",m_Cmd_Move);
    Write(PlatformRoot,"remove_file",m_Cmd_RemoveFile);
    Write(PlatformRoot,"force_remove_file",m_Cmd_ForceRemoveFile);
    Write(PlatformRoot,"make_dir",m_Cmd_MakeDir);
    Write(PlatformRoot,"test_make_dir",m_Cmd_TestMakeDir);
    Write(PlatformRoot,"force_make_dir",m_Cmd_ForceMakeDir);
    Write(PlatformRoot,"remove_dir",m_Cmd_RemoveDir);
    Write(PlatformRoot,"print_work_dir",m_Cmd_PrintWorkDir);
    Write(PlatformRoot,"eval_work_dir",m_Cmd_EvalWorkDir);
    Write(PlatformRoot,"change_dir",m_Cmd_ChangeDir);
    Write(PlatformRoot,"static_lib_ext",m_StaticLibraryExtensions.Join(" "));
    Write(PlatformRoot,"dynamic_lib_ext",m_DynamicLibraryExtensions.Join(" "));
    //Write(PlatformRoot,"",m_);
}

void CPlatform::Show(void)
{
    std::cout<<"Platform name: "<<Name().GetString()<<std::endl;
    std::cout<<"Make tool: "<<m_Cmd_Make.GetString()<<std::endl;
//std::cout<<"Test file: "<<m_Cmd_TestFile.GetString()<<std::endl;
    std::cout<<"Remove file: "<<m_Cmd_RemoveFile.GetString()<<std::endl;
    std::cout<<"Remove file (forced): "<<m_Cmd_ForceRemoveFile.GetString()<<std::endl;
//std::cout<<"Test directory: "<<m_Cmd_TestDir.GetString()<<std::endl;
    std::cout<<"Make directory: "<<m_Cmd_MakeDir.GetString()<<std::endl;
    std::cout<<"Test and make directory: "<<m_Cmd_TestMakeDir.GetString()<<std::endl;
    std::cout<<"Make directory (forced): "<<m_Cmd_ForceMakeDir.GetString()<<std::endl;
    std::cout<<"Remove directory: "<<m_Cmd_RemoveDir.GetString()<<std::endl;
    std::cout<<"Print working directory: "<<m_Cmd_PrintWorkDir.GetString()<<std::endl;
    std::cout<<"Get working directory: "<<m_Cmd_EvalWorkDir.GetString()<<std::endl;
    std::cout<<"Change directory: "<<m_Cmd_ChangeDir.GetString()<<std::endl;
    std::cout<<"Path delimiter: '"<<m_PathDelimiter<<"'"<<std::endl;
//std::cout<<": "<<m_.GetString()<<std::endl;
}

CPlatform& CPlatform::operator =(const CPlatform& Platform)
{
    Assign(Platform);
    return *this;
}

//------------------------------------------------------------------------------

CPlatformSet::CPlatformSet(void)
{
    Unlock();
    Clear();
}

CPlatformSet::~CPlatformSet(void)
{
    Unlock();
    Clear();
}

void CPlatformSet::Lock(void)
{
    m_Locked = true;
}

void CPlatformSet::Unlock(void)
{
    m_Locked = false;
}

void CPlatformSet::Clear(void)
{
    if (m_Locked) return;
    for (size_t i = 0; i < m_Platforms.size(); i++) delete m_Platforms[i];
    m_Platforms.clear();
}

size_t CPlatformSet::GetCount(void) const
{
    return m_Platforms.size();
}

CPlatform *CPlatformSet::Platform(const size_t Index) const
{
    if (Index<m_Platforms.size()) {
        return m_Platforms[Index];
    } else {
        return 0;
    }
}

CPlatform *CPlatformSet::Find(const CPlatform::OS_Type OS)
{
    for (int i = 0, n = m_Platforms.size(); i < n; i++) {
        CPlatform *p = m_Platforms[i];
        if (p->OS() == OS) return p;
    }
    return 0;
}

void CPlatformSet::AddDefault(void)
{
    if (m_Locked) return;
    CPlatform *p = Find(CPlatform::OS_Unix);
    if (0 == p) {
        p = new CPlatform();
        p->Reset(CPlatform::OS_Unix);
        m_Platforms.push_back(p);
    }
    p = Find(CPlatform::OS_Windows);
    if (0 == p) {
        p = new CPlatform();
        p->Reset(CPlatform::OS_Windows);
        m_Platforms.push_back(p);
    }
    p = Find(CPlatform::OS_Mac);
    if (0 == p) {
        p = new CPlatform();
        p->Reset(CPlatform::OS_Mac);
        m_Platforms.push_back(p);
    }
    p = Find(CPlatform::OS_MSys);
    if (0 == p) {
        p = new CPlatform();
        p->Reset(CPlatform::OS_MSys);
        m_Platforms.push_back(p);
    }
}

void CPlatformSet::Read(const TiXmlElement *ConfigRoot)
{
    TiXmlNode *_platform = (TiXmlNode *)ConfigRoot->FirstChild("platform");
    while (0 != _platform) {

        TiXmlElement* platform = _platform->ToElement();

        if (strcmp(platform->Value(), "platform") !=0 )
            break;
        if (0!=platform) {
            CPlatform *p = new CPlatform();
            p->Read(platform);
            m_Platforms.push_back(p);
        }
        _platform = (TiXmlNode *)ConfigRoot->IterateChildren(_platform);
    } // platform
}

void CPlatformSet::Write(TiXmlElement *ConfigRoot)
{
    for (int i = 0, n = m_Platforms.size(); i < n; i++) {
        CPlatform *p = m_Platforms[i];
        TiXmlElement *p_root = new TiXmlElement("platform");
        p->Write(p_root);
        ConfigRoot->LinkEndChild(p_root);
    }
}

void CPlatformSet::Show(void)
{
    if (m_Platforms.size()) {
        std::cout<<"Configued "<<m_Platforms.size()<<" platform(s):"<<std::endl;
        for (int i = 0, n = m_Platforms.size(); i < n; i++) {
            std::cout<<"Platform #"<<(i+1)<<": "<<std::endl;
            CPlatform *p = m_Platforms[i];
            p->Show();
        }
    } else {
        std::cout<<"No platforms configured"<<std::endl;
    }
}

//------------------------------------------------------------------------------
