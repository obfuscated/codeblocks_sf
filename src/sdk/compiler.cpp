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



CompilerSwitches::CompilerSwitches()
{	// default based upon gnu
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
    m_NeedValidityCheck(true)
{
	//ctor
    MakeValidID();

    m_Switches.supportsPCH = false;
    m_Switches.forceFwdSlashes = false;

    Manager::Get()->GetMessageManager()->DebugLog(_T("Added compiler \"%s\""), m_Name.c_str());
}

Compiler::Compiler(const Compiler& other)
    : CompileOptionsBase(other),
    m_ParentID(other.m_ParentID.IsEmpty() ? other.m_ID : other.m_ParentID),
    m_pGenerator(0)
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
    wxString tmp = m_MasterPath + _T("/bin/") + m_Programs.C;
    Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
    m_Valid = wxFileExists(tmp);
    if (!m_Valid)
    {	// and try witout appending the 'bin'
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

void Compiler::SaveSettings(const wxString& baseKey)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("compiler"));

    wxString tmp;

    // delete old-style keys (using integer IDs)
    tmp.Printf(_T("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    cfg->DeleteSubPath(tmp);

    tmp.Printf(_T("%s/%s"), baseKey.c_str(), m_ID.c_str());

	cfg->Write(tmp + _T("/name"), m_Name);
	cfg->Write(tmp + _T("/parent"), m_ParentID, true);

	wxString key = GetStringFromArray(m_CompilerOptions);
	cfg->Write(tmp + _T("/compiler_options"), key, false);
	key = GetStringFromArray(m_LinkerOptions);
	cfg->Write(tmp + _T("/linker_options"), key, false);
	key = GetStringFromArray(m_IncludeDirs);
	cfg->Write(tmp + _T("/include_dirs"), key, false);
	key = GetStringFromArray(m_ResIncludeDirs);
	cfg->Write(tmp + _T("/res_include_dirs"), key, false);
	key = GetStringFromArray(m_LibDirs);
	cfg->Write(tmp + _T("/library_dirs"), key, false);
	key = GetStringFromArray(m_LinkLibs);
	cfg->Write(tmp + _T("/libraries"), key, false);
	key = GetStringFromArray(m_CmdsBefore);
	cfg->Write(tmp + _T("/commands_before"), key, true);
	key = GetStringFromArray(m_CmdsAfter);
	cfg->Write(tmp + _T("/commands_after"), key, true);

    cfg->Write(tmp + _T("/master_path"), m_MasterPath, true);
    cfg->Write(tmp + _T("/extra_paths"), GetStringFromArray(m_ExtraPaths, _T(";")), true);
    cfg->Write(tmp + _T("/c_compiler"), m_Programs.C, true);
    cfg->Write(tmp + _T("/cpp_compiler"), m_Programs.CPP, true);
    cfg->Write(tmp + _T("/linker"), m_Programs.LD, true);
    cfg->Write(tmp + _T("/lib_linker"), m_Programs.LIB, true);
    cfg->Write(tmp + _T("/res_compiler"), m_Programs.WINDRES, true);
    cfg->Write(tmp + _T("/make"), m_Programs.MAKE, true);
    cfg->Write(tmp + _T("/debugger"), m_Programs.DBG, true);

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        cfg->Write(tmp + _T("/macros/") + CommandTypeDescriptions[i], m_Commands[i], true);
    }

    // switches
    cfg->Write(tmp + _T("/switches/includes"), m_Switches.includeDirs, true);
    cfg->Write(tmp + _T("/switches/libs"), m_Switches.libDirs, true);
    cfg->Write(tmp + _T("/switches/link"), m_Switches.linkLibs, true);
    cfg->Write(tmp + _T("/switches/define"), m_Switches.defines, true);
    cfg->Write(tmp + _T("/switches/generic"), m_Switches.genericSwitch, true);
    cfg->Write(tmp + _T("/switches/objectext"), m_Switches.objectExtension, true);
    cfg->Write(tmp + _T("/switches/deps"), m_Switches.needDependencies);
    cfg->Write(tmp + _T("/switches/forceCompilerQuotes"), m_Switches.forceCompilerUseQuotes);
    cfg->Write(tmp + _T("/switches/forceLinkerQuotes"), m_Switches.forceLinkerUseQuotes);
    cfg->Write(tmp + _T("/switches/logging"), m_Switches.logging);
    cfg->Write(tmp + _T("/switches/buildMethod"), m_Switches.buildMethod);
    cfg->Write(tmp + _T("/switches/libPrefix"), m_Switches.libPrefix, true);
    cfg->Write(tmp + _T("/switches/libExtension"), m_Switches.libExtension, true);
    cfg->Write(tmp + _T("/switches/linkerNeedsLibPrefix"), m_Switches.linkerNeedsLibPrefix);
    cfg->Write(tmp + _T("/switches/linkerNeedsLibExtension"), m_Switches.linkerNeedsLibExtension);
    cfg->Write(tmp + _T("/switches/forceFwdSlashes"), m_Switches.forceFwdSlashes);
    cfg->Write(tmp + _T("/switches/supportsPCH"), m_Switches.supportsPCH);
    cfg->Write(tmp + _T("/switches/pchExtension"), m_Switches.PCHExtension);
    cfg->Write(tmp + _T("/switches/UseFlatObjects"), m_Switches.UseFlatObjects);
    cfg->Write(tmp + _T("/switches/UseFullSourcePaths"), m_Switches.UseFullSourcePaths);

    // regexes
    cfg->DeleteSubPath(tmp + _T("/regex"));
    wxString group;
    for (size_t i = 0; i < m_RegExes.Count(); ++i)
    {
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
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("compiler"));

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
    wxString group;
    int index = 1;
    bool cleared = false;
    while (true)
    {
        group.Printf(_T("%s/regex/re%3.3d"), tmp.c_str(), index++);
        if (!cfg->Exists(group+_T("/description")))
            break;
        else if (!cleared)
        {
            cleared = true;
            m_RegExes.Clear();
        }
        RegExStruct rs;
        rs.desc = cfg->Read(group + _T("/description"));
        rs.lt = (CompilerLineType)cfg->ReadInt(group + _T("/type"), 0);
        rs.regex = cfg->Read(group + _T("/regex"));
        rs.msg[0] = cfg->ReadInt(group + _T("/msg1"), 0);
        rs.msg[1] = cfg->ReadInt(group + _T("/msg2"), 0);
        rs.msg[2] = cfg->ReadInt(group + _T("/msg3"), 0);
        rs.filename = cfg->ReadInt(group + _T("/filename"), 0);
        rs.line = cfg->ReadInt(group + _T("/line"), 0);
        m_RegExes.Add(rs);
    }

    // custom vars
    wxString configpath = tmp + _T("/custom_variables/");
	UnsetAllVars();
	wxArrayString list = cfg->EnumerateKeys(configpath);
	for (unsigned int i = 0; i < list.GetCount(); ++i)
		SetVar(list[i], cfg->Read(configpath + _T('/') + list[i]), false);
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
