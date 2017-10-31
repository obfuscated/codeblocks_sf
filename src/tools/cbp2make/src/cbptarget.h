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
#ifndef CBP_TARGET_H
#define CBP_TARGET_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "platforms.h"
//------------------------------------------------------------------------------

class TiXmlNode;
class TiXmlElement;
class CCodeBlocksProject;

class CBuildTarget
{
public:
    enum TargetType {
        ttExecutable,
        ttConsoleExecutable,
        ttStaticLibrary,
        ttDynamicLibrary,
        ttCommands,
        ttNative // windows-only ".sys" files
    };
    enum OptionsRelation {
        orProject,
        orTarget,
        orTargetProject,
        orProjectTarget
    };
private:
    CString m_Title;
    CStringList m_Platforms;
    CString m_Output;
    CString m_WorkingDirectory;
    CString m_ObjectOutput;
    CStringList m_ExternalDependencies;
    TargetType m_Type;
    bool m_AutoPrefix;
    bool m_AutoExtension;
    CString m_Compiler;
    CStringList m_CompilerOptions;
    CStringList m_CompilerDirectories;
    CStringList m_LinkerOptions;
    CStringList m_LinkerLibraries;
    CStringList m_LinkerDirectories;
    CStringList m_BeforeBuildCommands;
    CStringList m_AfterBuildCommands;
    bool m_ForceBeforeBuildCommands;
    bool m_ForceAfterBuildCommands;
    CStringList m_ResourceCompilerDirectories;
    CStringList m_ResourceCompilerOptions;
    OptionsRelation m_CompilerOptionsRelation;
    OptionsRelation m_LinkerOptionsRelation;
    OptionsRelation m_IncludeDirectoriesRelation;
    OptionsRelation m_ResourceIncludeDirectoriesRelation;
    OptionsRelation m_LibraryDirectoriesRelation;
    OptionsRelation m_ResourceCompilerOptionsRelation;
    //
    int m_NameCase;
    CString m_UCName;
    CString m_LCName;
    CString m_MFName;
public:
    CString Title(void) const
    {
        return m_Title;
    }
    CStringList Platforms(void) const
    {
        return m_Platforms;
    }
    CString Output(void) const
    {
        return m_Output;
    }
    CString ObjectOutput(void) const
    {
        return m_ObjectOutput;
    }
    TargetType Type(void) const
    {
        return m_Type;
    }
    bool AutoPrefix(void) const
    {
        return m_AutoPrefix;
    }
    bool AutoExtension(void) const
    {
        return m_AutoExtension;
    }
    CString Compiler(void) const
    {
        return m_Compiler;
    }
    CStringList CompilerDirectories(void) const
    {
        return m_CompilerDirectories;
    }
    CStringList BeforeBuildCommands(void) const
    {
        return m_BeforeBuildCommands;
    }
    CStringList AfterBuildCommands(void) const
    {
        return m_AfterBuildCommands;
    }
    bool ForceBeforeBuildCommands(void) const
    {
        return m_ForceBeforeBuildCommands;
    }
    bool ForceAfterBuildCommands(void) const
    {
        return m_ForceAfterBuildCommands;
    }
    int& NameCase(void)
    {
        return m_NameCase;
    }
    CString UCName(void) const
    {
        return m_UCName;
    }
    CString LCName(void) const
    {
        return m_LCName;
    }
    CString MFName(void) const
    {
        return m_MFName;
    }
    CString Name(const CString& Prefix, const int Case) const;
    CString Name(const CString& Prefix) const;
    //
    CString TargetTypeName(const TargetType Type);
    CString TargetTypeName(void);
    CString OptionsRelationName(const OptionsRelation Relation);
    CString AutoFilePrefix(const CPlatform::OS_Type OS);
    CString AutoFileExtension(const CPlatform::OS_Type OS, const CBuildTarget::TargetType Type);
    CString AutoFileExtension(CString& Platform);
    CString AutoFileExtension(const int Platform);
    CString MakeOptions(const OptionsRelation Relation,
                        const CString& ProjectOptions,
                        const CString& TargetOptions);
    CString CFlags(void);
    CString CFlags(const CString& ProjectCFlags);
    CString RCFlags(void);
    CString RCFlags(const CString& ProjectRCFlags);
    CString LdFlags(void);
    CString LdFlags(const CString& ProjectLdFlags);
    CString IncDirs(const CString& IncDirSwitch);
    CString IncDirs(const CString& ProjectIncDirs, const CString& IncDirSwitch);
    CString ResDirs(const CString& IncDirSwitch);
    CString ResDirs(const CString& ProjectResDirs, const CString& IncDirSwitch);
    CString LibDirs(const CString& LibDirSwitch);
    CString LibDirs(const CString& ProjectLibDirs, const CString& LibDirSwitch);
    CString Libs(const CPlatform& Platform, const CString& LinkLibSwitch);
    CString ExtDeps(void);
public:
    void Clear(void);
    void Read(const TiXmlElement *TargetRoot);
    void Show(void);
public:
    CBuildTarget(void);
    ~CBuildTarget(void);
};

class CVirtualTarget
{
//friend class CCodeBlocksProject;
private:
    CString m_Alias;
    CStringList m_Targets;
    //
    int m_NameCase;
    CString m_UCName;
    CString m_LCName;
    CString m_MFName;
public:
    CString Alias(void) const
    {
        return m_Alias;
    }
    CStringList& Targets(void)
    {
        return m_Targets;
    }
    int& NameCase(void)
    {
        return m_NameCase;
    }
    CString UCName(void) const
    {
        return m_UCName;
    }
    CString LCName(void) const
    {
        return m_LCName;
    }
    CString MFName(void) const
    {
        return m_MFName;
    }
    CString Name(const CString& Prefix) const;
    CString Name(const CString& Prefix, const int TargetIndex) const;
    //
    void Clear(void);
    void Read(const TiXmlElement *TargetRoot);
    void Show(void);
public:
    CVirtualTarget(void);
    ~CVirtualTarget(void);
};

#endif
//------------------------------------------------------------------------------
