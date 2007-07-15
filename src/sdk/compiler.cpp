/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "cbexception.h"
    #include "compiler.h"
    #include "manager.h"
    #include "messagemanager.h"
    #include "configmanager.h"
    #include "macrosmanager.h"
    #include "globals.h"
    #include "compilerfactory.h"

    #include <wx/intl.h>
    #include <wx/regex.h>
#endif

#include "compilercommandgenerator.h"
#include <wx/filefn.h>
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(RegExArray);

// static
wxArrayString Compiler::m_CompilerIDs; // map to guarantee unique IDs

// common regex that can be used by the different compiler for matching compiler output
// it can be used in the patterns for warnings, errors, ...
// NOTE : it is an approximation (for example the ':' can appear anywhere and several times)
const wxString Compiler::FilePathWithSpaces = _T("[][{}() \t#%$~A-Za-z0-9_:+/\\.-]+");

// version of compiler settings
// when this is different from what is saved in the config, a message appears
// to the user saying that default settings have changed and asks him if he wants to
// use his own settings or the new defaults
const wxString CompilerSettingsVersion = _T("0.0.2");

CompilerSwitches::CompilerSwitches()
{   // default based upon gnu
    includeDirs = _T("-I");
    libDirs = _T("-L");
    linkLibs = _T("-l");
    defines = _T("-D");
    genericSwitch = _T("-");
    objectExtension = _T("o");
    needDependencies = true;
    forceFwdSlashes = false;
    forceCompilerUseQuotes = false;
    forceLinkerUseQuotes = false;
    logging = clogSimple;
    libPrefix = _T("lib");
    libExtension = _T("a");
    linkerNeedsLibPrefix = false;
    linkerNeedsLibExtension = false;
    buildMethod = cbmDirect;
    supportsPCH = true;
    PCHExtension = _T("h.gch");
    UseFlatObjects = false;
    UseFullSourcePaths = false;
} // end of constructor


wxString Compiler::CommandTypeDescriptions[COMPILER_COMMAND_TYPES_COUNT] =
{
    // These are the strings that describe each CommandType enumerator...
    // No need to say that it must have the same order as the enumerators!
    _("Compile single file to object file"),
    _("Generate dependencies for file"),
    _("Compile Win32 resource file"),
    _("Link object files to executable"),
    _("Link object files to console executable"),
    _("Link object files to dynamic library"),
    _("Link object files to static library"),
    _("Link object files to native executable")
};

Compiler::Compiler(const wxString& name, const wxString& ID, const wxString& parentID)
    : m_Name(name),
    m_ID(ID.Lower()),
    m_ParentID(parentID.Lower()),
    m_pGenerator(0),
    m_Valid(false),
    m_NeedValidityCheck(true),
    m_Mirrored(false)
{
    //ctor
    MakeValidID();

    m_Switches.supportsPCH = false;
    m_Switches.forceFwdSlashes = false;
    m_VersionString = wxEmptyString;

    Manager::Get()->GetMessageManager()->DebugLog(_T("Added compiler \"%s\""), m_Name.c_str());
}

Compiler::Compiler(const Compiler& other)
    : CompileOptionsBase(other),
    m_ParentID(other.m_ParentID.IsEmpty() ? other.m_ID : other.m_ParentID),
    m_pGenerator(0),
    m_Mirror(other.m_Mirror),
    m_Mirrored(other.m_Mirrored)
{
    m_Name = _("Copy of ") + other.m_Name;
    // generate unique ID
    // note that this copy constructor is protected and can only be called
    // by our friend CompilerFactory. It knows what it's doing ;)
    wxDateTime now = wxDateTime::UNow();
    m_ID = now.Format(_T("%c"), wxDateTime::CET);
    MakeValidID();

    m_MasterPath = other.m_MasterPath;
    m_Programs = other.m_Programs;
    m_Switches = other.m_Switches;
    m_Options = other.m_Options;
    m_IncludeDirs = other.m_IncludeDirs;
    m_LibDirs = other.m_LibDirs;
    m_CompilerOptions = other.m_CompilerOptions;
    m_LinkerOptions = other.m_LinkerOptions;
    m_LinkLibs = other.m_LinkLibs;
    m_CmdsBefore = other.m_CmdsBefore;
    m_CmdsAfter = other.m_CmdsAfter;
    m_RegExes = other.m_RegExes;
    m_VersionString = other.m_VersionString;
    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i] = other.m_Commands[i];
    }

    m_Valid = other.m_Valid;
    m_NeedValidityCheck = other.m_NeedValidityCheck;
}

Compiler::~Compiler()
{
    //dtor
    delete m_pGenerator;
}

bool Compiler::IsValid()
{
    if (!m_NeedValidityCheck)
        return m_Valid;

    if (m_MasterPath.IsEmpty())
        return true; // still initializing, don't try to test now

    m_NeedValidityCheck = false;

    if (!SupportsCurrentPlatform())
    {
        m_Valid = false;
        return false;
    }

    wxString tmp = m_MasterPath + _T("/bin/") + m_Programs.C;
    Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
    m_Valid = wxFileExists(tmp);
    if (!m_Valid)
    {   // and try witout appending the 'bin'
        tmp = m_MasterPath + _T("/") + m_Programs.C;
        Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
        m_Valid = wxFileExists(tmp);
    }
    if (!m_Valid)
    {
        // look in extra paths too
        for (size_t i = 0; i < m_ExtraPaths.GetCount(); ++i)
        {
            tmp = m_ExtraPaths[i] + _T("/") + m_Programs.C;
            Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
            m_Valid = wxFileExists(tmp);
            if (m_Valid)
                break;
        }
    }
    return m_Valid;
}

void Compiler::MakeValidID()
{
    // basically, make it XML-element compatible
    // only allow a-z, 0-9 and _
    // (it is already lowercase)
    // any non-conformant character will be removed

    wxString newID;
    if (m_ID.IsEmpty())
        m_ID = m_Name;

    size_t pos = 0;
    while (pos < m_ID.Length())
    {
        wxChar ch = m_ID[pos];
        if (wxIsalnum(ch) || ch == _T('_'))
        {
            // valid character
            newID.Append(ch);
        }
        else if (wxIsspace(ch))
        {
            // convert spaces to underscores
            newID.Append(_T('_'));
        }
        ++pos;
    }

    // make sure it's not starting with a number.
    // if it is, prepend "cb"
    if (wxIsdigit(newID.GetChar(0)))
        newID.Prepend(_T("cb"));

    if (newID.IsEmpty())
    {
        // empty? wtf?
        cbThrow(_T("Can't create a valid compiler ID for ") + m_Name);
    }
    m_ID = newID.Lower();

    // check for unique ID
    if (!IsUniqueID(m_ID))
        cbThrow(_T("Compiler ID already exists for ") + m_Name);
    m_CompilerIDs.Add(m_ID);
}

CompilerCommandGenerator* Compiler::GetCommandGenerator()
{
    return new CompilerCommandGenerator;
}

void Compiler::Init(cbProject* project)
{
    if (!m_pGenerator)
        m_pGenerator = GetCommandGenerator();
    m_pGenerator->Init(project);
}

void Compiler::GenerateCommandLine(wxString& macro,
                                    ProjectBuildTarget* target,
                                    ProjectFile* pf,
                                    const wxString& file,
                                    const wxString& object,
                                    const wxString& FlatObject,
                                    const wxString& deps)
{
    if (!m_pGenerator)
        cbThrow(_T("Compiler::Init() not called or generator invalid!"));
    m_pGenerator->GenerateCommandLine(macro, target, pf, file, object, FlatObject, deps);
}

const wxArrayString& Compiler::GetCompilerSearchDirs(ProjectBuildTarget* target)
{
    static wxArrayString retIfError;
    retIfError.Clear();
    if (!m_pGenerator)
        return retIfError;

    return m_pGenerator->GetCompilerSearchDirs(target);
}

const wxArrayString& Compiler::GetLinkerSearchDirs(ProjectBuildTarget* target)
{
    static wxArrayString retIfError;
    retIfError.Clear();
    if (!m_pGenerator)
        return retIfError;

    return m_pGenerator->GetLinkerSearchDirs(target);
}

void Compiler::MirrorCurrentSettings()
{
    // run just once
    if (m_Mirrored)
        return;

    // keep the current settings safe
    // so we can compare them when saving: this way we can only save what's
    // different from the defaults

    m_Mirror.Name = m_Name;
    m_Mirror.MasterPath = m_MasterPath;
    m_Mirror.ExtraPaths = m_ExtraPaths;
    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
        m_Mirror.Commands[i] = m_Commands[i];
    m_Mirror.Programs = m_Programs;
    m_Mirror.Switches = m_Switches;
    m_Mirror.Options = m_Options;
    m_Mirror.RegExes = m_RegExes;

    m_Mirror.CompilerOptions_ = m_CompilerOptions;
    m_Mirror.LinkerOptions = m_LinkerOptions;
    m_Mirror.IncludeDirs = m_IncludeDirs;
    m_Mirror.ResIncludeDirs = m_ResIncludeDirs;
    m_Mirror.LibDirs = m_LibDirs;
    m_Mirror.LinkLibs = m_LinkLibs;
    m_Mirror.CmdsBefore = m_CmdsBefore;
    m_Mirror.CmdsAfter = m_CmdsAfter;

    m_Mirrored = true;
}

void Compiler::SaveSettings(const wxString& baseKey)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("compiler"));

    // save settings version
    cfg->Write(_T("settings_version"), CompilerSettingsVersion);

    wxString tmp;

    // delete old-style keys (using integer IDs)
    tmp.Printf(_T("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    cfg->DeleteSubPath(tmp);

    tmp.Printf(_T("%s/%s"), baseKey.c_str(), m_ID.c_str());

    cfg->Write(tmp + _T("/name"), m_Name);
    cfg->Write(tmp + _T("/parent"), m_ParentID, true);

    if (m_Mirror.CompilerOptions_ != m_CompilerOptions)
    {
        wxString key = GetStringFromArray(m_CompilerOptions);
        cfg->Write(tmp + _T("/compiler_options"), key, false);
    }
    if (m_Mirror.LinkerOptions != m_LinkerOptions)
    {
        wxString key = GetStringFromArray(m_LinkerOptions);
        cfg->Write(tmp + _T("/linker_options"), key, false);
    }
    if (m_Mirror.IncludeDirs != m_IncludeDirs)
    {
        wxString key = GetStringFromArray(m_IncludeDirs);
        cfg->Write(tmp + _T("/include_dirs"), key, false);
    }
    if (m_Mirror.ResIncludeDirs != m_ResIncludeDirs)
    {
        wxString key = GetStringFromArray(m_ResIncludeDirs);
        cfg->Write(tmp + _T("/res_include_dirs"), key, false);
    }
    if (m_Mirror.LibDirs != m_LibDirs)
    {
        wxString key = GetStringFromArray(m_LibDirs);
        cfg->Write(tmp + _T("/library_dirs"), key, false);
    }
    if (m_Mirror.LinkLibs != m_LinkLibs)
    {
        wxString key = GetStringFromArray(m_LinkLibs);
        cfg->Write(tmp + _T("/libraries"), key, false);
    }
    if (m_Mirror.CmdsBefore != m_CmdsBefore)
    {
        wxString key = GetStringFromArray(m_CmdsBefore);
        cfg->Write(tmp + _T("/commands_before"), key, true);
    }
    if (m_Mirror.CmdsAfter != m_CmdsAfter)
    {
        wxString key = GetStringFromArray(m_CmdsAfter);
        cfg->Write(tmp + _T("/commands_after"), key, true);
    }

    if (m_Mirror.MasterPath != m_MasterPath)
        cfg->Write(tmp + _T("/master_path"), m_MasterPath, true);
    if (m_Mirror.ExtraPaths != m_ExtraPaths)
        cfg->Write(tmp + _T("/extra_paths"), GetStringFromArray(m_ExtraPaths, _T(";")), true);
    if (m_Mirror.Programs.C != m_Programs.C)
        cfg->Write(tmp + _T("/c_compiler"), m_Programs.C, true);
    if (m_Mirror.Programs.CPP != m_Programs.CPP)
        cfg->Write(tmp + _T("/cpp_compiler"), m_Programs.CPP, true);
    if (m_Mirror.Programs.LD != m_Programs.LD)
        cfg->Write(tmp + _T("/linker"), m_Programs.LD, true);
    if (m_Mirror.Programs.LIB != m_Programs.LIB)
        cfg->Write(tmp + _T("/lib_linker"), m_Programs.LIB, true);
    if (m_Mirror.Programs.WINDRES != m_Programs.WINDRES)
        cfg->Write(tmp + _T("/res_compiler"), m_Programs.WINDRES, true);
    if (m_Mirror.Programs.MAKE != m_Programs.MAKE)
        cfg->Write(tmp + _T("/make"), m_Programs.MAKE, true);
    if (m_Mirror.Programs.DBG != m_Programs.DBG)
        cfg->Write(tmp + _T("/debugger"), m_Programs.DBG, true);

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        if (m_Mirror.Commands[i] != m_Commands[i])
            cfg->Write(tmp + _T("/macros/") + CommandTypeDescriptions[i], m_Commands[i], true);
    }

    // switches
    if (m_Mirror.Switches.includeDirs != m_Switches.includeDirs)
        cfg->Write(tmp + _T("/switches/includes"), m_Switches.includeDirs, true);
    if (m_Mirror.Switches.libDirs != m_Switches.libDirs)
        cfg->Write(tmp + _T("/switches/libs"), m_Switches.libDirs, true);
    if (m_Mirror.Switches.linkLibs != m_Switches.linkLibs)
        cfg->Write(tmp + _T("/switches/link"), m_Switches.linkLibs, true);
    if (m_Mirror.Switches.defines != m_Switches.defines)
        cfg->Write(tmp + _T("/switches/define"), m_Switches.defines, true);
    if (m_Mirror.Switches.genericSwitch != m_Switches.genericSwitch)
        cfg->Write(tmp + _T("/switches/generic"), m_Switches.genericSwitch, true);
    if (m_Mirror.Switches.objectExtension != m_Switches.objectExtension)
        cfg->Write(tmp + _T("/switches/objectext"), m_Switches.objectExtension, true);
    if (m_Mirror.Switches.needDependencies != m_Switches.needDependencies)
        cfg->Write(tmp + _T("/switches/deps"), m_Switches.needDependencies);
    if (m_Mirror.Switches.forceCompilerUseQuotes != m_Switches.forceCompilerUseQuotes)
        cfg->Write(tmp + _T("/switches/forceCompilerQuotes"), m_Switches.forceCompilerUseQuotes);
    if (m_Mirror.Switches.forceLinkerUseQuotes != m_Switches.forceLinkerUseQuotes)
        cfg->Write(tmp + _T("/switches/forceLinkerQuotes"), m_Switches.forceLinkerUseQuotes);
    if (m_Mirror.Switches.logging != m_Switches.logging)
        cfg->Write(tmp + _T("/switches/logging"), m_Switches.logging);
    if (m_Mirror.Switches.buildMethod != m_Switches.buildMethod)
        cfg->Write(tmp + _T("/switches/buildMethod"), m_Switches.buildMethod);
    if (m_Mirror.Switches.libPrefix != m_Switches.libPrefix)
        cfg->Write(tmp + _T("/switches/libPrefix"), m_Switches.libPrefix, true);
    if (m_Mirror.Switches.libExtension != m_Switches.libExtension)
        cfg->Write(tmp + _T("/switches/libExtension"), m_Switches.libExtension, true);
    if (m_Mirror.Switches.linkerNeedsLibPrefix != m_Switches.linkerNeedsLibPrefix)
        cfg->Write(tmp + _T("/switches/linkerNeedsLibPrefix"), m_Switches.linkerNeedsLibPrefix);
    if (m_Mirror.Switches.linkerNeedsLibExtension != m_Switches.linkerNeedsLibExtension)
        cfg->Write(tmp + _T("/switches/linkerNeedsLibExtension"), m_Switches.linkerNeedsLibExtension);
    if (m_Mirror.Switches.forceFwdSlashes != m_Switches.forceFwdSlashes)
        cfg->Write(tmp + _T("/switches/forceFwdSlashes"), m_Switches.forceFwdSlashes);
    if (m_Mirror.Switches.supportsPCH != m_Switches.supportsPCH)
        cfg->Write(tmp + _T("/switches/supportsPCH"), m_Switches.supportsPCH);
    if (m_Mirror.Switches.PCHExtension != m_Switches.PCHExtension)
        cfg->Write(tmp + _T("/switches/pchExtension"), m_Switches.PCHExtension);
    if (m_Mirror.Switches.UseFlatObjects != m_Switches.UseFlatObjects)
        cfg->Write(tmp + _T("/switches/UseFlatObjects"), m_Switches.UseFlatObjects);
    if (m_Mirror.Switches.UseFullSourcePaths != m_Switches.UseFullSourcePaths)
        cfg->Write(tmp + _T("/switches/UseFullSourcePaths"), m_Switches.UseFullSourcePaths);

    // regexes
    cfg->DeleteSubPath(tmp + _T("/regex"));
    wxString group;
    for (size_t i = 0; i < m_RegExes.Count(); ++i)
    {
        if (i < m_Mirror.RegExes.GetCount() && m_Mirror.RegExes[i] == m_RegExes[i])
            continue;

        group.Printf(_T("%s/regex/re%3.3d"), tmp.c_str(), i + 1);
        RegExStruct& rs = m_RegExes[i];
        cfg->Write(group + _T("/description"), rs.desc, true);
        if (rs.lt != 0)
            cfg->Write(group + _T("/type"), rs.lt);
            cfg->Write(group + _T("/regex"), rs.regex, true);
        if (rs.msg[0] != 0)
            cfg->Write(group + _T("/msg1"), rs.msg[0]);
        if (rs.msg[1] != 0)
            cfg->Write(group + _T("/msg2"), rs.msg[1]);
        if (rs.msg[2] != 0)
            cfg->Write(group + _T("/msg3"), rs.msg[2]);
        if (rs.filename != 0)
            cfg->Write(group + _T("/filename"), rs.filename);
        if (rs.line != 0)
            cfg->Write(group + _T("/line"), rs.line);
    }

    // custom vars
    wxString configpath = tmp + _T("/custom_variables/");
    cfg->DeleteSubPath(configpath);
    const StringHash& v = GetAllVars();
    for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
    {
        cfg->Write(configpath + it->first, it->second);
    }
}

void Compiler::LoadSettings(const wxString& baseKey)
{
    // before loading any compiler settings, keep the current settings safe
    // so we can compare them when saving: this way we can only save what's
    // different from the defaults
    MirrorCurrentSettings();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("compiler"));

    // read settings version
    wxString version = cfg->Read(_T("settings_version"));
    bool versionMismatch = version != CompilerSettingsVersion;

    wxString tmp;

    // if using old-style keys (using integer IDs), notify user about the changes
    static bool saidAboutCompilerIDs = false;
    tmp.Printf(_T("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    if (cfg->Exists(tmp + _T("/name")))
    {
        if (!saidAboutCompilerIDs)
        {
            saidAboutCompilerIDs = true;
            cbMessageBox(_("Compilers now use unique names instead of integer IDs.\n"
                            "Projects will be updated accordingly on load, mostly automatic."),
                            _("Information"),
                            wxICON_INFORMATION);
        }
        // at this point, we 'll be using the old style configuration to load settings
    }
    else // it's OK to use new style
        tmp.Printf(_T("%s/%s"), baseKey.c_str(), m_ID.c_str());

    if (!cfg->Exists(tmp + _T("/name")))
        return;

    wxString sep = wxFileName::GetPathSeparator();

//    if (m_ID > 255) // name changes are allowed only for user compilers
    m_Name = cfg->Read(tmp + _T("/name"), m_Name);

    m_MasterPath = cfg->Read(tmp + _T("/master_path"), m_MasterPath);
    m_ExtraPaths = GetArrayFromString(cfg->Read(tmp + _T("/extra_paths"), _T("")), _T(";"));
    m_Programs.C = cfg->Read(tmp + _T("/c_compiler"), m_Programs.C);
    m_Programs.CPP = cfg->Read(tmp + _T("/cpp_compiler"), m_Programs.CPP);
    m_Programs.LD = cfg->Read(tmp + _T("/linker"), m_Programs.LD);
    m_Programs.LIB = cfg->Read(tmp + _T("/lib_linker"), m_Programs.LIB);
    m_Programs.WINDRES = cfg->Read(tmp + _T("/res_compiler"), m_Programs.WINDRES);
    m_Programs.MAKE = cfg->Read(tmp + _T("/make"), m_Programs.MAKE);
    m_Programs.DBG = cfg->Read(tmp + _T("/debugger"), m_Programs.DBG);

    SetCompilerOptions(GetArrayFromString(cfg->Read(tmp + _T("/compiler_options"), wxEmptyString)));
    SetLinkerOptions(GetArrayFromString(cfg->Read(tmp + _T("/linker_options"), wxEmptyString)));
    SetIncludeDirs(GetArrayFromString(cfg->Read(tmp + _T("/include_dirs"), m_MasterPath + sep + _T("include"))));
    SetResourceIncludeDirs(GetArrayFromString(cfg->Read(tmp + _T("/res_include_dirs"), m_MasterPath + sep + _T("include"))));
    SetLibDirs(GetArrayFromString(cfg->Read(tmp + _T("/library_dirs"), m_MasterPath + sep + _T("lib"))));
    SetLinkLibs(GetArrayFromString(cfg->Read(tmp + _T("/libraries"), _T(""))));
    SetCommandsBeforeBuild(GetArrayFromString(cfg->Read(tmp + _T("/commands_before"), wxEmptyString)));
    SetCommandsAfterBuild(GetArrayFromString(cfg->Read(tmp + _T("/commands_after"), wxEmptyString)));

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i] = cfg->Read(tmp + _T("/macros/") + CommandTypeDescriptions[i], m_Commands[i]);
    }

    // switches
    m_Switches.includeDirs = cfg->Read(tmp + _T("/switches/includes"), m_Switches.includeDirs);
    m_Switches.libDirs = cfg->Read(tmp + _T("/switches/libs"), m_Switches.libDirs);
    m_Switches.linkLibs = cfg->Read(tmp + _T("/switches/link"), m_Switches.linkLibs);
    m_Switches.defines = cfg->Read(tmp + _T("/switches/define"), m_Switches.defines);
    m_Switches.genericSwitch = cfg->Read(tmp + _T("/switches/generic"), m_Switches.genericSwitch);
    m_Switches.objectExtension = cfg->Read(tmp + _T("/switches/objectext"), m_Switches.objectExtension);
    m_Switches.needDependencies = cfg->ReadBool(tmp + _T("/switches/deps"), m_Switches.needDependencies);
    m_Switches.forceCompilerUseQuotes = cfg->ReadBool(tmp + _T("/switches/forceCompilerQuotes"), m_Switches.forceCompilerUseQuotes);
    m_Switches.forceLinkerUseQuotes = cfg->ReadBool(tmp + _T("/switches/forceLinkerQuotes"), m_Switches.forceLinkerUseQuotes);
    m_Switches.logging = (CompilerLoggingType)cfg->ReadInt(tmp + _T("/switches/logging"), m_Switches.logging);
    m_Switches.buildMethod = (CompilerBuildMethod)cfg->ReadInt(tmp + _T("/switches/buildMethod"), m_Switches.buildMethod);
    m_Switches.libPrefix = cfg->Read(tmp + _T("/switches/libPrefix"), m_Switches.libPrefix);
    m_Switches.libExtension = cfg->Read(tmp + _T("/switches/libExtension"), m_Switches.libExtension);
    m_Switches.linkerNeedsLibPrefix = cfg->ReadBool(tmp + _T("/switches/linkerNeedsLibPrefix"), m_Switches.linkerNeedsLibPrefix);
    m_Switches.linkerNeedsLibExtension = cfg->ReadBool(tmp + _T("/switches/linkerNeedsLibExtension"), m_Switches.linkerNeedsLibExtension);
    m_Switches.forceFwdSlashes = cfg->ReadBool(tmp + _T("/switches/forceFwdSlashes"), m_Switches.forceFwdSlashes);
    m_Switches.supportsPCH = cfg->ReadBool(tmp + _T("/switches/supportsPCH"), m_Switches.supportsPCH);
    m_Switches.PCHExtension = cfg->Read(tmp + _T("/switches/pchExtension"), m_Switches.PCHExtension);
    m_Switches.UseFlatObjects = cfg->ReadBool(tmp + _T("/switches/UseFlatObjects"), m_Switches.UseFlatObjects);
    m_Switches.UseFullSourcePaths = cfg->ReadBool(tmp + _T("/switches/UseFullSourcePaths"), m_Switches.UseFullSourcePaths);

    // regexes

    // because we 're only saving changed regexes, we can't just iterate like before.
    // instead, we must iterate all child-keys and deduce the regex index number from
    // the key name
    wxArrayString keys = cfg->EnumerateSubPaths(tmp + _T("/regex/"));
    wxString group;
    long index;
    for (size_t i = 0; i < keys.GetCount(); ++i)
    {
        wxString key = keys[i];

        // reNNN
        if (!key.StartsWith(_T("re")))
            continue;
        key.Remove(0, 2);
        if (!key.ToLong(&index, 10))
            continue;

        // 'index' now holds the regex index.
        // read everything and either assign it to an existing regex
        // if the index exists, or add a new regex

        group.Printf(_T("%s/regex/re%3.3d"), tmp.c_str(), index);
        if (!cfg->Exists(group+_T("/description")))
            continue;

        RegExStruct rs;
        rs.desc = cfg->Read(group + _T("/description"));
        rs.lt = (CompilerLineType)cfg->ReadInt(group + _T("/type"), 0);
        rs.regex = cfg->Read(group + _T("/regex"));
        rs.msg[0] = cfg->ReadInt(group + _T("/msg1"), 0);
        rs.msg[1] = cfg->ReadInt(group + _T("/msg2"), 0);
        rs.msg[2] = cfg->ReadInt(group + _T("/msg3"), 0);
        rs.filename = cfg->ReadInt(group + _T("/filename"), 0);
        rs.line = cfg->ReadInt(group + _T("/line"), 0);

        if (index <= (long)m_RegExes.GetCount())
            m_RegExes[index - 1] = rs;
        else
            m_RegExes.Add(rs);
    }

    // custom vars
    wxString configpath = tmp + _T("/custom_variables/");
    UnsetAllVars();
    wxArrayString list = cfg->EnumerateKeys(configpath);
    for (unsigned int i = 0; i < list.GetCount(); ++i)
        SetVar(list[i], cfg->Read(configpath + _T('/') + list[i]), false);

    if (versionMismatch)
    {
        wxString msg;
        msg << _("Some compiler settings defaults have changed in this version.\n"
                "It is recommended that you allow updating of your settings to "
                "the new defaults.\n"
                "Only disallow this if you don't want to lose any customizations "
                "you have done to this compiler's settings.\n\n"
                "Note that the only settings that are affected are those found in "
                "\"Advanced compiler options\"...\n\n"
                "Do you want to update your current settings to the new defaults?");
        // don't ask if the compiler is not valid (i.e. not installed), just update
        if (!IsValid() || cbMessageBox(msg, m_Name, wxICON_QUESTION | wxYES_NO) == wxID_YES)
        {
            for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
                m_Commands[i] = m_Mirror.Commands[i];
            m_Switches = m_Mirror.Switches;
            m_Options = m_Mirror.Options;
            m_RegExes = m_Mirror.RegExes;
        }
    }
}

CompilerLineType Compiler::CheckForWarningsAndErrors(const wxString& line)
{
    m_ErrorFilename.Clear();
    m_ErrorLine.Clear();
    m_Error.Clear();

    for (size_t i = 0; i < m_RegExes.Count(); ++i)
    {
        RegExStruct& rs = m_RegExes[i];
        if (rs.regex.IsEmpty())
            continue;
        wxRegEx regex(rs.regex);
        if (regex.Matches(line))
        {
            if (rs.filename > 0)
                 m_ErrorFilename = UnixFilename(regex.GetMatch(line, rs.filename));
            if (rs.line > 0)
                m_ErrorLine = regex.GetMatch(line, rs.line);
            for (int x = 0; x < 3; ++x)
            {
                if (rs.msg[x] > 0)
                {
                    if (!m_Error.IsEmpty())
                        m_Error << _T(" ");
                    m_Error << regex.GetMatch(line, rs.msg[x]);
                }
            }
            return rs.lt;
        }
    }
    return cltNormal; // default return value
}
