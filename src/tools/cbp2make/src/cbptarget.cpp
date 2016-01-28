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
#include "cbptarget.h"
#include "cbhelper.h"
#include "stlconvert.h"
#include "stlfutils.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CBuildTarget::CBuildTarget(void)
{
    Clear();
}

CBuildTarget::~CBuildTarget(void)
{
    Clear();
}

CString CBuildTarget::TargetTypeName(const TargetType Type)
{
    switch (Type) {
    case CBuildTarget::ttExecutable:
        return "executable";
    case CBuildTarget::ttConsoleExecutable:
        return "console executable";
    case CBuildTarget::ttStaticLibrary:
        return "static library";
    case CBuildTarget::ttDynamicLibrary:
        return "dynamic library";
    case CBuildTarget::ttNative:
        return "native";
    case CBuildTarget::ttCommands:
        return "commands";
        //case CBuildTarget::tt: return "";
    }
    return "";
}

CString CBuildTarget::TargetTypeName(void)
{
    return TargetTypeName(m_Type);
}

CString CBuildTarget::OptionsRelationName(const OptionsRelation Relation)
{
    switch (Relation) {
    case CBuildTarget::orProject:
        return "project";
    case CBuildTarget::orTarget:
        return "target";
    case CBuildTarget::orTargetProject:
        return "target project";
    case CBuildTarget::orProjectTarget:
        return "project target";
        //case CBuildTarget::or: return "";
    }
    return "";
}

CString CBuildTarget::AutoFilePrefix(const CPlatform::OS_Type OS)
{
    switch (OS) {
    default:
    case CPlatform::OS_Unix: {
        switch (m_Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "";
        }
        case CBuildTarget::ttExecutable:        {
            return "";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "lib";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "";
        }
        case CBuildTarget::ttNative:            {
            return "";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    case CPlatform::OS_Windows: {
        switch (m_Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "";
        }
        case CBuildTarget::ttExecutable:        {
            return "";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "lib";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "";
        }
        case CBuildTarget::ttNative:            {
            return "";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    case CPlatform::OS_Mac: {
        switch (m_Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "";
        }
        case CBuildTarget::ttExecutable:        {
            return "";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "lib";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "";
        }
        case CBuildTarget::ttNative:            {
            return "";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    }
    return "";
}

CString CBuildTarget::AutoFileExtension(const CPlatform::OS_Type OS, const CBuildTarget::TargetType Type)
{
    switch (OS) {
    default:
    case CPlatform::OS_Unix: {
        switch (Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "";
        }
        case CBuildTarget::ttExecutable:        {
            return "";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "a";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "so";
        }
        case CBuildTarget::ttNative:            {
            return "";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    case CPlatform::OS_MSys:
    case CPlatform::OS_Windows: {
        switch (Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "exe";
        }
        case CBuildTarget::ttExecutable:        {
            return "exe";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "a";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "dll";
        }
        case CBuildTarget::ttNative:            {
            return "sys";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    case CPlatform::OS_Mac: {
        switch (Type) {
        case CBuildTarget::ttConsoleExecutable: {
            return "";
        }
        case CBuildTarget::ttExecutable:        {
            return "";
        }
        case CBuildTarget::ttStaticLibrary:     {
            return "a";
        }
        case CBuildTarget::ttDynamicLibrary:    {
            return "dylib";
        }
        case CBuildTarget::ttNative:            {
            return "";
        }
        case CBuildTarget::ttCommands:          {
            return "";
        }
            //case CBuildTarget::tt: { break; }
        }
        break;
    }
    }
    return "";
}

CString CBuildTarget::AutoFileExtension(CString& Platform)
{
    return AutoFileExtension(CPlatform::OS(Platform),m_Type);
}

CString CBuildTarget::AutoFileExtension(const int Platform)
{
    return AutoFileExtension(m_Platforms.GetString(Platform));
}

CString CBuildTarget::MakeOptions(const OptionsRelation Relation,
                                  const CString& ProjectOptions, const CString& TargetOptions)
{
    switch (Relation) {
    case CBuildTarget::orProject:
        return ProjectOptions;
    case CBuildTarget::orTarget:
        return TargetOptions;
    case CBuildTarget::orTargetProject:
        return JoinStr(TargetOptions,ProjectOptions,' ');
    default:
    case CBuildTarget::orProjectTarget:
        return JoinStr(ProjectOptions,TargetOptions,' ');
    }
}

CString CBuildTarget::CFlags(void)
{
    CString result;
    for (int i = 0; i < m_CompilerOptions.GetCount(); i++) {
        result += " "+m_CompilerOptions[i];
    }
    return result;
}

CString CBuildTarget::CFlags(const CString& ProjectCFlags)
{
    return MakeOptions(m_CompilerOptionsRelation,ProjectCFlags,CFlags());
}

CString CBuildTarget::RCFlags(void)
{
    CString result;
    for (int i = 0; i < m_ResourceCompilerOptions.GetCount(); i++) {
        result += " "+m_ResourceCompilerOptions[i];
    }
    return result;
}

CString CBuildTarget::RCFlags(const CString& ProjectRCFlags)
{
    return MakeOptions(m_ResourceCompilerOptionsRelation,ProjectRCFlags,RCFlags());
}

CString CBuildTarget::LdFlags(void)
{
    CString result;
    for (int i = 0; i < m_LinkerOptions.GetCount(); i++) {
        result += " "+m_LinkerOptions[i];
    }
    return result;
}

CString CBuildTarget::LdFlags(const CString& ProjectLdFlags)
{
    return MakeOptions(m_LinkerOptionsRelation,ProjectLdFlags,LdFlags());
}

CString CBuildTarget::IncDirs(const CString& IncDirSwitch)
{
    CString result;
    for (int i = 0; i < m_CompilerDirectories.GetCount(); i++) {
        result = JoinStr(result,IncDirSwitch+m_CompilerDirectories[i],' ');
    }
    return result;
}

CString CBuildTarget::IncDirs(const CString& ProjectIncDirs, const CString& IncDirSwitch)
{
    return MakeOptions(m_IncludeDirectoriesRelation,ProjectIncDirs,IncDirs(IncDirSwitch));
}

CString CBuildTarget::ResDirs(const CString& IncDirSwitch)
{
    CString result;
    for (int i = 0; i < m_ResourceCompilerDirectories.GetCount(); i++) {
        result = JoinStr(result,IncDirSwitch+m_ResourceCompilerDirectories[i],' ');
    }
    return result;
}

CString CBuildTarget::ResDirs(const CString& ProjectResDirs, const CString& IncDirSwitch)
{
    return MakeOptions(m_ResourceIncludeDirectoriesRelation,ProjectResDirs,ResDirs(IncDirSwitch));
}

CString CBuildTarget::LibDirs(const CString& LibDirSwitch)
{
    CString result;
    for (int i = 0; i < m_LinkerDirectories.GetCount(); i++) {
        result = JoinStr(result,LibDirSwitch+m_LinkerDirectories[i],' ');
    }
    return result;
}

CString CBuildTarget::LibDirs(const CString& ProjectLibDirs, const CString& LibDirSwitch)
{
    return MakeOptions(m_LibraryDirectoriesRelation,ProjectLibDirs,LibDirs(LibDirSwitch));
}

CString CBuildTarget::Libs(const CPlatform& Platform, const CString& LinkLibSwitch)
{
    CString result;
    for (int i = 0; i < m_LinkerLibraries.GetCount(); i++) {
        CString lib_name = m_LinkerLibraries[i];
        CString lib_ext = ExtractFileExt(lib_name);
        //if (lib_ext.IsEmpty()) // wrong for files with dots but without .a extension
        //if (lib_ext==AutoFileExtension(OS,CBuildTarget::ttStaticLibrary)) // a plaform may have several valid extensions
        if (Platform.IsStaticLibraryExtension(lib_ext)) {
            result = JoinStr(result,lib_name,' ');
        } else {
            result = JoinStr(result,LinkLibSwitch+lib_name,' ');
        }
    }
    return result;
}

CString CBuildTarget::ExtDeps(void)
{
    CString result;
    for (int i = 0; i < m_ExternalDependencies.GetCount(); i++) {
        result += " "+m_ExternalDependencies[i];
    }
    return result;
}

void CBuildTarget::Clear(void)
{
    m_Title = "default"; //m_Title.Clear();
    m_Platforms.Clear();
    m_Output = "a.out"; //m_Output.Clear();
    m_ObjectOutput.Clear();
    m_ExternalDependencies.Clear();
    m_Type = ttExecutable;
    m_AutoPrefix = false;
    m_AutoExtension = false;
    m_Compiler.Clear();
    m_CompilerOptions.Clear();
    m_CompilerDirectories.Clear();
    m_ResourceCompilerDirectories.Clear();
    m_LinkerOptions.Clear();
    m_LinkerLibraries.Clear();
    m_LinkerDirectories.Clear();
    m_BeforeBuildCommands.Clear();
    m_AfterBuildCommands.Clear();
    m_ForceBeforeBuildCommands = false;
    m_ForceAfterBuildCommands = false;
    m_CompilerOptionsRelation = orProjectTarget;
    m_LinkerOptionsRelation = orProjectTarget;
    m_IncludeDirectoriesRelation = orProjectTarget;
    m_ResourceIncludeDirectoriesRelation = orProjectTarget;
    m_LibraryDirectoriesRelation = orProjectTarget;
    m_UCName.Clear();
    m_LCName.Clear();
}

void CBuildTarget::Read(const TiXmlElement *TargetRoot)
{
    char *value = 0;
    if ((value = (char *)TargetRoot->Attribute("title"))) m_Title = value;
    TiXmlNode *_option = (TiXmlNode *)TargetRoot->FirstChild("Option");
    while (0!=_option) {
        TiXmlElement* option = _option->ToElement();
        if (0!=option) {
            char *value = 0;
            if ((value = (char *)option->Attribute("platforms"))) {
                ParseStr(value,';',m_Platforms);
                m_Platforms.RemoveEmpty();

                if (m_Platforms.FindString("Windows") != INVALID_INDEX
                    && m_Platforms.FindString("MSys") == INVALID_INDEX) {
                    m_Platforms.Insert("MSys");
                }

            }
            if ((value = (char *)option->Attribute("output"))) {
                m_Output = value;
            }
            if ((value = (char *)option->Attribute("prefix_auto"))) {
                m_AutoPrefix = StringToBoolean(value);
            }
            if ((value = (char *)option->Attribute("extension_auto"))) {
                m_AutoExtension = StringToBoolean(value);
            }
            if ((value = (char *)option->Attribute("working_dir"))) {
                m_WorkingDirectory = value;
            }
            if ((value = (char *)option->Attribute("object_output"))) {
                m_ObjectOutput = value;
            }
            if ((value = (char *)option->Attribute("external_deps"))) {
                ParseStr(value,';',m_ExternalDependencies);
                m_ExternalDependencies.RemoveEmpty();
            }
            if ((value = (char *)option->Attribute("type"))) {
                m_Type = (CBuildTarget::TargetType)StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("compiler"))) {
                m_Compiler = value;
            }
            if ((value = (char *)option->Attribute("projectCompilerOptionsRelation"))) {
                m_CompilerOptionsRelation = (CBuildTarget::OptionsRelation)StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("projectLinkerOptionsRelation"))) {
                m_LinkerOptionsRelation = (CBuildTarget::OptionsRelation)StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("projectIncludeDirsRelation"))) {
                m_IncludeDirectoriesRelation = (CBuildTarget::OptionsRelation)StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("projectResourceIncludeDirsRelation"))) {
                m_ResourceIncludeDirectoriesRelation = (CBuildTarget::OptionsRelation)StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("projectLibDirsRelation"))) {
                m_LibraryDirectoriesRelation = (CBuildTarget::OptionsRelation)StringToInteger(value);
            }
            /*
            if ((value = (char *)option->Attribute("")))
            {
              = value;
            }
            */
        }
        _option = (TiXmlNode *)TargetRoot->IterateChildren(_option);
    } // option
    TiXmlNode *_compiler = (TiXmlNode *)TargetRoot->FirstChild("Compiler");
    if (0!=_compiler) {
        TiXmlNode *_option = (TiXmlNode *)_compiler->FirstChild("Add");
        while (0!=_option) {
            TiXmlElement* option = _option->ToElement();
            if (0!=option) {
                char *value = 0;
                if ((value = (char *)option->Attribute("option"))) {
                    m_CompilerOptions.Insert(value);
                }
                if ((value = (char *)option->Attribute("directory"))) {
                    m_CompilerDirectories.Insert(value);
                }
            }
            _option = (TiXmlNode *)_compiler->IterateChildren(_option);
        } // option
    } // compiler
    TiXmlNode *_res_compiler = (TiXmlNode *)TargetRoot->FirstChild("ResourceCompiler");
    if (0!=_res_compiler) {
        TiXmlNode *_option = (TiXmlNode *)_res_compiler->FirstChild("Add");
        while (0!=_option) {
            TiXmlElement* option = _option->ToElement();
            if (0!=option) {
                char *value = 0;
                if ((value = (char *)option->Attribute("directory"))) {
                    m_ResourceCompilerDirectories.Insert(value);
                }
            }
            _option = (TiXmlNode *)_res_compiler->IterateChildren(_option);
        } // option
    } // resource compiler
    TiXmlNode *_linker = (TiXmlNode *)TargetRoot->FirstChild("Linker");
    if (0!=_linker) {
        TiXmlNode *_option = (TiXmlNode *)_linker->FirstChild("Add");
        while (0!=_option) {
            TiXmlElement* option = _option->ToElement();
            if (0!=option) {
                if (strcmp(option->Value(),"Add")) break;
                char *value = 0;
                if ((value = (char *)option->Attribute("option"))) {
                    m_LinkerOptions.Insert(value);
                }
                if ((value = (char *)option->Attribute("directory"))) {
                    m_LinkerDirectories.Insert(value);
                }
                if ((value = (char *)option->Attribute("library"))) {
                    m_LinkerLibraries.Insert(value);
                }
            }
            _option = (TiXmlNode *)_linker->IterateChildren(_option);
        } // option
    } // linker
    TiXmlNode *_extra_cmd = (TiXmlNode *)TargetRoot->FirstChild("ExtraCommands");
    if (0!=_extra_cmd) {
        TiXmlNode *_option = (TiXmlNode *)_extra_cmd->FirstChild("Add");
        while (0!=_option) {
            TiXmlElement* option = _option->ToElement();
            if (0!=option) {
                if (strcmp(option->Value(),"Add")) break;
                char *value = 0;
                if ((value = (char *)option->Attribute("before"))) {
                    m_BeforeBuildCommands.Insert(value);
                }
                if ((value = (char *)option->Attribute("after"))) {
                    m_AfterBuildCommands.Insert(value);
                }
            }
            _option = (TiXmlNode *)_extra_cmd->IterateChildren(_option);
        } // option
        _option = (TiXmlNode *)_extra_cmd->FirstChild("Mode");
        while (0!=_option) {
            TiXmlElement* option = _option->ToElement();
            if (0!=option) {
                if (strcmp(option->Value(),"Mode")) break;
                char *value = 0;
                if ((value = (char *)option->Attribute("before"))) {
                    m_ForceBeforeBuildCommands = (strcmp(value,"always")==0);
                }
                if ((value = (char *)option->Attribute("after"))) {
                    m_ForceAfterBuildCommands = (strcmp(value,"always")==0);
                }
            }
            _option = (TiXmlNode *)_extra_cmd->IterateChildren(_option);
        } // option
    } // extra commands
// decorate target name
    {
        m_MFName = MakefileFriendly(m_Title);
        m_UCName = UpperCase(m_MFName);
        m_LCName = LowerCase(m_MFName);
        m_ObjectOutput = RemoveTrailingPathDelimiter(m_ObjectOutput);
    }
}

CString CBuildTarget::Name(const CString& Prefix, const int Case) const
{
    switch (Case) {
    default:
    case 0:
        return Prefix+m_MFName;
    case 1:
        return LowerCase(Prefix)+m_LCName;
    case 2:
        return UpperCase(Prefix)+m_UCName;
    }
}

CString CBuildTarget::Name(const CString& Prefix) const
{
    switch (m_NameCase) {
    default:
    case 0:
        return Prefix+m_MFName;
    case 1:
        return LowerCase(Prefix)+m_LCName;
    case 2:
        return UpperCase(Prefix)+m_UCName;
    }
}

void CBuildTarget::Show(void)
{
    std::cout<<"Target title: "<<m_Title.GetString()<<std::endl;
    if (m_Platforms.GetCount()) {
        ShowStringList("Platforms","Platform",m_Platforms);
    } else {
        std::cout<<"Platforms: all"<<std::endl;
    }
    std::cout<<"Binary output: "<<m_Output.GetCString()<<std::endl;
    std::cout<<"Automatic prefix: "<<BooleanToYesNoString(m_AutoPrefix).GetCString()<<std::endl;
    std::cout<<"Automatic extension: "<<BooleanToYesNoString(m_AutoExtension).GetCString()<<std::endl;
    std::cout<<"Working directory: "<<m_WorkingDirectory.GetCString()<<std::endl;
    std::cout<<"Object output: "<<m_ObjectOutput.GetCString()<<std::endl;
    ShowStringList("External dependencies","Dependency",m_ExternalDependencies);
    std::cout<<"Type: "<<TargetTypeName().GetCString()<<std::endl;
    std::cout<<"Compiler: "<<m_Compiler.GetCString()<<std::endl;
    std::cout<<"Compiler options relation: "
             <<OptionsRelationName(m_CompilerOptionsRelation).GetCString()<<std::endl;
    std::cout<<"Linker options relation: "
             <<OptionsRelationName(m_LinkerOptionsRelation).GetCString()<<std::endl;
    std::cout<<"Include directories relation: "
             <<OptionsRelationName(m_IncludeDirectoriesRelation).GetCString()<<std::endl;
    std::cout<<"Resource include directories relation: "
             <<OptionsRelationName(m_ResourceIncludeDirectoriesRelation).GetCString()<<std::endl;
    std::cout<<"Library directories relation: "
             <<OptionsRelationName(m_LibraryDirectoriesRelation).GetCString()<<std::endl;
    ShowStringList("Compiler options","Option",m_CompilerOptions);
    ShowStringList("Compiler directories","Directory",m_CompilerDirectories);
    ShowStringList("Resource compiler directories","Directory",m_ResourceCompilerDirectories);
    ShowStringList("Linker options","Option",m_LinkerOptions);
    ShowStringList("Linker directories","Directory",m_LinkerDirectories);
    ShowStringList("Linked libraries","Library",m_LinkerLibraries);
    ShowStringList("Before-build commands","Command",m_BeforeBuildCommands);
    std::cout<<"Force before-build commands: "
             <<BooleanToYesNoString(m_ForceBeforeBuildCommands).GetCString()<<std::endl;
    ShowStringList("After-build commands","Command",m_AfterBuildCommands);
    std::cout<<"Force after-build commands: "
             <<BooleanToYesNoString(m_ForceAfterBuildCommands).GetCString()<<std::endl;
}

//------------------------------------------------------------------------------

CVirtualTarget::CVirtualTarget(void)
{
    Clear();
}

CVirtualTarget::~CVirtualTarget(void)
{
    Clear();
}

void CVirtualTarget::Clear(void)
{
    m_Alias.Clear();
    m_Targets.Clear();
    m_UCName.Clear();
    m_LCName.Clear();
}

void CVirtualTarget::Read(const TiXmlElement *TargetRoot)
{
    char *value = 0;
    if ((value = (char *)TargetRoot->Attribute("alias"))) {
        m_Alias = value;
    }
    if ((value = (char *)TargetRoot->Attribute("targets"))) {
        ParseStr(value,';',m_Targets);
        m_Targets.RemoveEmpty();
    }
// decorate target name
    {
        m_MFName = MakefileFriendly(m_Alias);
        m_UCName = UpperCase(m_MFName);
        m_LCName = LowerCase(m_MFName);
    }
}

CString CVirtualTarget::Name(const CString& Prefix) const
{
    switch (m_NameCase) {
    default:
    case 0:
        return Prefix+m_MFName;
    case 1:
        return LowerCase(Prefix)+m_LCName;
    case 2:
        return UpperCase(Prefix)+m_UCName;
    }
}

CString CVirtualTarget::Name(const CString& Prefix, const int TargetIndex) const
{
    CString name = MakefileFriendly(m_Targets.GetString(TargetIndex));
    switch (m_NameCase) {
    default:
    case 0:
        return name;
    case 1:
        return LowerCase(name);
    case 2:
        return UpperCase(name);
    }
}

void CVirtualTarget::Show(void)
{
    std::cout<<"Target alias: "<<m_Alias.GetString()<<std::endl;
    if (m_Targets.GetCount()) {
        ShowStringList("Targets","Target",m_Targets);
    }
}

//------------------------------------------------------------------------------
