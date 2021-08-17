/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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
    #include "logmanager.h"
    #include "configmanager.h"
    #include "macrosmanager.h"
    #include "globals.h"
    #include "compilerfactory.h"

    #include <wx/intl.h>
    #include <wx/process.h>
    #include <wx/regex.h>
    #include <wx/txtstrm.h>
#endif

#include "compilercommandgenerator.h"
#include <wx/arrimpl.cpp>
#include <wx/filefn.h>
#include <wx/xml/xml.h>

// static
wxArrayString Compiler::m_CompilerIDs; // map to guarantee unique IDs

// common regex that can be used by the different compiler for matching compiler output
// it can be used in the patterns for warnings, errors, ...
// NOTE : it is an approximation (for example the ':' can appear anywhere and several times)
const wxString Compiler::FilePathWithSpaces = "[][{}() \t#%$~[:alnum:]&_:+/\\.-]+";

// version of compiler settings
// when this is different from what is saved in the config, a message appears
// to the user saying that default settings have changed and asks him if he wants to
// use his own settings or the new defaults
const wxString CompilerSettingsVersion = "0.0.3";

const wxString EmptyString;

CompilerSwitches::CompilerSwitches()
{   // default based upon gnu
    includeDirs             = "-I";
    libDirs                 = "-L";
    linkLibs                = "-l";
    defines                 = "-D";
    genericSwitch           = "-";
    objectExtension         = "o";
    needDependencies        = true;
    forceFwdSlashes         = false;
    forceCompilerUseQuotes  = false;
    forceLinkerUseQuotes    = false;
    logging                 = defaultLogging;
    libPrefix               = "lib";
    libExtension            = "a";
    linkerNeedsLibPrefix    = false;
    linkerNeedsLibExtension = false;
    linkerNeedsPathResolved = false;
    supportsPCH             = true;
    PCHExtension            = "gch";
    UseFlatObjects          = false;
    UseFullSourcePaths      = false;
    Use83Paths              = false;
    includeDirSeparator     = ' ';
    libDirSeparator         = ' ';
    objectSeparator         = ' ';
    statusSuccess           = 0;
}

wxString Compiler::CommandTypeDescriptions[ctCount] =
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

Compiler::Compiler(const wxString& name, const wxString& ID, const wxString& parentID, int weight) :
    m_Name(name),
    m_MultiLineMessages(false),
    m_ID(ID.Lower()),
    m_ParentID(parentID.Lower()),
    m_Valid(false),
    m_NeedValidityCheck(true),
    m_Mirrored(false)
{
    //ctor
    MakeValidID();

    m_Switches.supportsPCH = false;
    m_Switches.forceFwdSlashes = false;
    m_VersionString = wxString();
    m_Weight = weight;
    m_RegExes.reserve(100);

    m_DebuggerInitialConfiguation.validData                  = false;
    m_DebuggerInitialConfiguation.compilerMasterPath         = "";
    m_DebuggerInitialConfiguation.compilerIDName             = m_ID;
    m_DebuggerInitialConfiguation.executablePath             = "gdb.exe";
    m_DebuggerInitialConfiguation.userArguments              = "";
    m_DebuggerInitialConfiguation.type                       = "GDB";
    m_DebuggerInitialConfiguation.initCommands               = "";
    m_DebuggerInitialConfiguation.disableInit                = true;
    m_DebuggerInitialConfiguation.watchArgs                  = true;
    m_DebuggerInitialConfiguation.watchLocals                = true;
    m_DebuggerInitialConfiguation.catchExceptions            = true;
    m_DebuggerInitialConfiguation.evalExpressionAsTooltip    = false;
    m_DebuggerInitialConfiguation.addOtherSearchDirs         = false;
    m_DebuggerInitialConfiguation.doNoRunDebuggee            = false;
    m_DebuggerInitialConfiguation.disassemblyFlavor          = "System default";
    m_DebuggerInitialConfiguation.instructionSet             = "";

    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Added compiler \"%s\""), m_Name));
}

Compiler::Compiler(const Compiler& other) :
    CompileOptionsBase(other),
    m_ParentID(other.m_ParentID.IsEmpty() ? other.m_ID : other.m_ParentID),
    m_Mirror(other.m_Mirror),
    m_Mirrored(other.m_Mirrored)
{
    m_Name = _("Copy of ") + other.m_Name;
    m_MultiLineMessages = other.m_MultiLineMessages;
    // generate unique ID
    // note that this copy constructor is protected and can only be called
    // by our friend CompilerFactory. It knows what it's doing ;)
    wxDateTime now = wxDateTime::UNow();
    m_ID = now.Format(_("%c"), wxDateTime::CET);
    MakeValidID();

    m_MasterPath      = other.m_MasterPath;
    m_ExtraPaths      = other.m_ExtraPaths;
    m_Programs        = other.m_Programs;
    m_Switches        = other.m_Switches;
    m_Options         = other.m_Options;
    m_SortOptions[0]  = other.m_SortOptions[0];
    m_SortOptions[1]  = other.m_SortOptions[1];
    m_IncludeDirs     = MakeUniqueArray(other.m_IncludeDirs,    true);
    m_ResIncludeDirs  = MakeUniqueArray(other.m_ResIncludeDirs, true);
    m_LibDirs         = MakeUniqueArray(other.m_LibDirs,        true);
    m_CompilerOptions = other.m_CompilerOptions;
    m_LinkerOptions   = other.m_LinkerOptions;
    m_LinkLibs        = other.m_LinkLibs;
    m_CmdsBefore      = other.m_CmdsBefore;
    m_CmdsAfter       = other.m_CmdsAfter;
    m_RegExes         = other.m_RegExes;
    m_VersionString   = other.m_VersionString;
    m_Weight          = 100; // place copied compilers at the end

    for (int i = 0; i < ctCount; ++i)
        m_Commands[(CommandType)i] = other.m_Commands[(CommandType)i];

    m_Valid = other.m_Valid;
    m_NeedValidityCheck = other.m_NeedValidityCheck;

    m_DebuggerInitialConfiguation.validData                  = false;\
    m_DebuggerInitialConfiguation.compilerMasterPath         = "";
    m_DebuggerInitialConfiguation.compilerIDName             = m_ID;
    m_DebuggerInitialConfiguation.executablePath             = "gdb.exe";
    m_DebuggerInitialConfiguation.userArguments              = "";
    m_DebuggerInitialConfiguation.type                       = "GDB";
    m_DebuggerInitialConfiguation.initCommands               = "";
    m_DebuggerInitialConfiguation.disableInit                = true;
    m_DebuggerInitialConfiguation.watchArgs                  = true;
    m_DebuggerInitialConfiguation.watchLocals                = true;
    m_DebuggerInitialConfiguation.catchExceptions            = true;
    m_DebuggerInitialConfiguation.evalExpressionAsTooltip    = false;
    m_DebuggerInitialConfiguation.addOtherSearchDirs         = false;
    m_DebuggerInitialConfiguation.doNoRunDebuggee            = false;
    m_DebuggerInitialConfiguation.disassemblyFlavor          = "System default";
    m_DebuggerInitialConfiguation.instructionSet             = "";

    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Added compiler \"%s\""), m_Name));
}

Compiler::~Compiler()
{
    //dtor
}

void Compiler::PostRegisterCompilerSetup()
{
    if (m_DebuggerInitialConfiguation.validData == true)
    {
        if (m_MasterPath.IsEmpty())
        {
            AutoDetectInstallationDir();
        }
        m_DebuggerInitialConfiguation.compilerMasterPath = m_MasterPath;
        Manager::Get()->GetDebuggerManager()->SaveDebuggerConfigOptions(m_DebuggerInitialConfiguation);
    }
}

void Compiler::Reset()
{
    m_Options.ClearOptions();
    for (int i = 0; i < ctCount; ++i)
        m_Commands[i].clear();
    LoadDefaultOptions(GetID());

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
    SetVersionString(); // Does nothing unless reimplemented
}

void Compiler::ReloadOptions()
{
    if (ConfigManager::LocateDataFile("compilers/options_" + GetID() + ".xml", sdDataUser | sdDataGlobal).IsEmpty())
        return; // Do not clear if the options cannot be reloaded
    m_Options.ClearOptions();
    LoadDefaultOptions(GetID());
    LoadDefaultRegExArray();
}

void Compiler::LoadDefaultRegExArray(bool globalPrecedence)
{
    m_RegExes.clear();
    LoadRegExArray(GetID(), globalPrecedence);
}

// Keep in sync with the MakeInvalidCompilerMessages method.
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

    wxString tmp = m_MasterPath + "/bin/" + m_Programs.C;
    MacrosManager *macros = Manager::Get()->GetMacrosManager();
    macros->ReplaceMacros(tmp);
    m_Valid = wxFileExists(tmp);
    if (!m_Valid)
    {
        // and try without appending the 'bin'
        tmp = m_MasterPath + "/" + m_Programs.C;
        macros->ReplaceMacros(tmp);
        m_Valid = wxFileExists(tmp);
    }
    if (!m_Valid)
    {
        // look in extra paths too
        for (size_t i = 0; i < m_ExtraPaths.GetCount(); ++i)
        {
            tmp = m_ExtraPaths[i] + "/" + m_Programs.C;
            macros->ReplaceMacros(tmp);
            m_Valid = wxFileExists(tmp);
            if (m_Valid)
                break;
        }
    }
    return m_Valid;
}

// Keep in sync with the IsValid method.
wxString Compiler::MakeInvalidCompilerMessages() const
{
    if (!SupportsCurrentPlatform())
        return _("Compiler doesn't support this platform!\n");

    MacrosManager *macros = Manager::Get()->GetMacrosManager();

    wxString triedPathsMsgs;
    wxString tmp = m_MasterPath + "/bin/" + m_Programs.C;
    macros->ReplaceMacros(tmp);
    triedPathsMsgs += wxString::Format(_("Tried to run compiler executable '%s', but failed!\n"), tmp);

    // and try without appending the 'bin'
    tmp = m_MasterPath + "/" + m_Programs.C;
    macros->ReplaceMacros(tmp);

    // look in extra paths too
    for (size_t i = 0; i < m_ExtraPaths.GetCount(); ++i)
    {
        triedPathsMsgs += wxString::Format(_("Tried to run compiler executable '%s', but failed!\n"), tmp);

        tmp = m_ExtraPaths[i] + "/" + m_Programs.C;
        macros->ReplaceMacros(tmp);
    }

    return triedPathsMsgs;
}

void Compiler::MakeValidID()
{
    // basically, make it XML-element compatible
    // only allow a-z, 0-9, _, and -
    // (it is already lowercase)
    // any non-conformant character will be removed

    wxString newID;
    if (m_ID.IsEmpty())
        m_ID = m_Name;

    size_t pos = 0;
    while (pos < m_ID.Length())
    {
        wxChar ch = m_ID[pos];
        if (wxIsalnum(ch) || ch == '_' || ch == '-') // valid character
            newID.Append(ch);
        else if (wxIsspace(ch)) // convert spaces to underscores
            newID.Append('_');
        ++pos;
    }

    // make sure it's not starting with a number or a '-'.
    // if it is, prepend "cb"
    if (wxIsdigit(newID.GetChar(0)) || newID.GetChar(0) == '-')
        newID.Prepend("cb");

    if (newID.IsEmpty()) // empty? wtf?
        cbThrow("Can't create a valid compiler ID for " + m_Name);
    m_ID = newID.Lower();

    // check for unique ID
    if (!IsUniqueID(m_ID))
        cbThrow("Compiler ID already exists for " + m_Name);
    m_CompilerIDs.Add(m_ID);
}

CompilerCommandGenerator* Compiler::GetCommandGenerator(cbProject* project)
{
    CompilerCommandGenerator* generator = new CompilerCommandGenerator;
    generator->Init(project);
    return generator;
}

const wxString& Compiler::GetCommand(CommandType ct, const wxString& fileExtension) const
{
    const CompilerToolsVector& vec = m_Commands[ct];

    // no command?
    if (vec.empty())
        return EmptyString;

    size_t catchAll = 0;

    if (!fileExtension.IsEmpty())
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (vec[i].extensions.GetCount() == 0)
            {
                catchAll = i;
                continue;
            }
            for (size_t n = 0; n < vec[i].extensions.GetCount(); ++n)
            {
                if (vec[i].extensions[n] == fileExtension)
                    return vec[i].command;
            }
        }
    }
    return vec[catchAll].command;
}

const CompilerTool* Compiler::GetCompilerTool(CommandType ct, const wxString& fileExtension) const
{
    const CompilerToolsVector& vec = m_Commands[ct];
    if (vec.empty())
        return nullptr;

    size_t catchAll = 0;
    if (!fileExtension.IsEmpty())
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (vec[i].extensions.GetCount() == 0)
            {
                catchAll = i;
                continue;
            }
            for (size_t n = 0; n < vec[i].extensions.GetCount(); ++n)
            {
                if (vec[i].extensions[n] == fileExtension)
                    return &vec[i];
            }
        }
    }
    return &vec[catchAll];
}

void Compiler::MirrorCurrentSettings()
{
    // run just once
    if (m_Mirrored)
        return;

    // keep the current settings safe
    // so we can compare them when saving: this way we can only save what's
    // different from the defaults

    m_Mirror.Name             = m_Name;
    m_Mirror.MasterPath       = m_MasterPath;
    m_Mirror.ExtraPaths       = m_ExtraPaths;
    for (int i = 0; i < ctCount; ++i)
        m_Mirror.Commands[i]  = m_Commands[i];
    m_Mirror.Programs         = m_Programs;
    m_Mirror.Switches         = m_Switches;
    m_Mirror.Options          = m_Options;
    m_Mirror.RegExes          = m_RegExes;

    m_Mirror.CompilerOptions_ = m_CompilerOptions;
    m_Mirror.LinkerOptions    = m_LinkerOptions;
    m_Mirror.IncludeDirs      = MakeUniqueArray(m_IncludeDirs,    true);
    m_Mirror.ResIncludeDirs   = MakeUniqueArray(m_ResIncludeDirs, true);
    m_Mirror.LibDirs          = MakeUniqueArray(m_LibDirs,        true);
    m_Mirror.LinkLibs         = m_LinkLibs;
    m_Mirror.CmdsBefore       = m_CmdsBefore;
    m_Mirror.CmdsAfter        = m_CmdsAfter;

    m_Mirror.SortOptions[0] = m_SortOptions[0];
    m_Mirror.SortOptions[1] = m_SortOptions[1];

    m_Mirrored                = true;
}

void Compiler::SaveSettings(const wxString& baseKey)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager("compiler");

    // save settings version
    cfg->Write("settings_version", CompilerSettingsVersion);

    wxString tmp;

    // delete old-style keys (using integer IDs)
    tmp.Printf(_("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    cfg->DeleteSubPath(tmp);

    tmp.Printf(_("%s/%s"), baseKey.c_str(), m_ID.c_str());

    cfg->Write(tmp + "/name",   m_Name);
    cfg->Write(tmp + "/parent", m_ParentID, true);

    if (m_Mirror.CompilerOptions_ != m_CompilerOptions)
    {
        wxString key = GetStringFromArray(m_CompilerOptions);
        cfg->Write(tmp + "/compiler_options", key, false);
    }
    if (m_Mirror.ResourceCompilerOptions != m_ResourceCompilerOptions)
    {
        wxString key = GetStringFromArray(m_ResourceCompilerOptions);
        cfg->Write(tmp + "/resource_compiler_options", key, false);
    }
    if (m_Mirror.LinkerOptions != m_LinkerOptions)
    {
        wxString key = GetStringFromArray(m_LinkerOptions);
        cfg->Write(tmp + "/linker_options",   key, false);
    }
    if (m_Mirror.IncludeDirs != m_IncludeDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_IncludeDirs, true) );
        cfg->Write(tmp + "/include_dirs",     key, false);
    }
    if (m_Mirror.ResIncludeDirs != m_ResIncludeDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_ResIncludeDirs, true) );
        cfg->Write(tmp + "/res_include_dirs", key, false);
    }
    if (m_Mirror.LibDirs != m_LibDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_LibDirs, true) );
        cfg->Write(tmp + "/library_dirs",     key, false);
    }
    if (m_Mirror.LinkLibs != m_LinkLibs)
    {
        wxString key = GetStringFromArray(m_LinkLibs);
        cfg->Write(tmp + "/libraries",        key, false);
    }
    if (m_Mirror.CmdsBefore != m_CmdsBefore)
    {
        wxString key = GetStringFromArray(m_CmdsBefore);
        cfg->Write(tmp + "/commands_before",  key, true);
    }
    if (m_Mirror.CmdsAfter != m_CmdsAfter)
    {
        wxString key = GetStringFromArray(m_CmdsAfter);
        cfg->Write(tmp + "/commands_after",   key, true);
    }

    if (m_Mirror.MasterPath != m_MasterPath)
        cfg->Write(tmp + "/master_path",     m_MasterPath,         true);
    if (m_Mirror.ExtraPaths != m_ExtraPaths)
        cfg->Write(tmp + "/extra_paths",     GetStringFromArray( MakeUniqueArray(m_ExtraPaths, true), ";" ), true);
    if (m_Mirror.Programs.C != m_Programs.C)
        cfg->Write(tmp + "/c_compiler",      m_Programs.C,         true);
    if (m_Mirror.Programs.CPP != m_Programs.CPP)
        cfg->Write(tmp + "/cpp_compiler",    m_Programs.CPP,       true);
    if (m_Mirror.Programs.LD != m_Programs.LD)
        cfg->Write(tmp + "/linker",          m_Programs.LD,        true);
    if (m_Mirror.Programs.LIB != m_Programs.LIB)
        cfg->Write(tmp + "/lib_linker",      m_Programs.LIB,       true);
    if (m_Mirror.Programs.WINDRES != m_Programs.WINDRES)
        cfg->Write(tmp + "/res_compiler",    m_Programs.WINDRES,   true);
    if (m_Mirror.Programs.MAKE != m_Programs.MAKE)
        cfg->Write(tmp + "/make",            m_Programs.MAKE,      true);
    if (m_Mirror.Programs.DBGconfig != m_Programs.DBGconfig)
        cfg->Write(tmp + "/debugger_config", m_Programs.DBGconfig, true);

    for (int i = 0; i < ctCount; ++i)
    {
        for (size_t n = 0; n < m_Commands[i].size(); ++n)
        {
            if (n >= m_Mirror.Commands[i].size() || m_Mirror.Commands[i][n] != m_Commands[i][n])
            {
                wxString key = wxString::Format(_("%s/macros/%s/tool%lu/"), tmp.c_str(), CommandTypeDescriptions[i].c_str(), static_cast<unsigned long>(n));
                cfg->Write(key + "command", m_Commands[i][n].command);
                cfg->Write(key + "extensions", m_Commands[i][n].extensions);
                cfg->Write(key + "generatedFiles", m_Commands[i][n].generatedFiles);
            }
        }
    }

    // switches
    if (m_Mirror.Switches.includeDirs != m_Switches.includeDirs)
        cfg->Write(tmp + "/switches/includes",                m_Switches.includeDirs,     true);
    if (m_Mirror.Switches.libDirs != m_Switches.libDirs)
        cfg->Write(tmp + "/switches/libs",                    m_Switches.libDirs,         true);
    if (m_Mirror.Switches.linkLibs != m_Switches.linkLibs)
        cfg->Write(tmp + "/switches/link",                    m_Switches.linkLibs,        true);
    if (m_Mirror.Switches.defines != m_Switches.defines)
        cfg->Write(tmp + "/switches/define",                  m_Switches.defines,         true);
    if (m_Mirror.Switches.genericSwitch != m_Switches.genericSwitch)
        cfg->Write(tmp + "/switches/generic",                 m_Switches.genericSwitch,   true);
    if (m_Mirror.Switches.objectExtension != m_Switches.objectExtension)
        cfg->Write(tmp + "/switches/objectext",               m_Switches.objectExtension, true);
    if (m_Mirror.Switches.needDependencies != m_Switches.needDependencies)
        cfg->Write(tmp + "/switches/deps",                    m_Switches.needDependencies);
    if (m_Mirror.Switches.forceCompilerUseQuotes != m_Switches.forceCompilerUseQuotes)
        cfg->Write(tmp + "/switches/forceCompilerQuotes",     m_Switches.forceCompilerUseQuotes);
    if (m_Mirror.Switches.forceLinkerUseQuotes != m_Switches.forceLinkerUseQuotes)
        cfg->Write(tmp + "/switches/forceLinkerQuotes",       m_Switches.forceLinkerUseQuotes);
    if (m_Mirror.Switches.logging != m_Switches.logging)
        cfg->Write(tmp + "/switches/logging",                 m_Switches.logging);
    if (m_Mirror.Switches.libPrefix != m_Switches.libPrefix)
        cfg->Write(tmp + "/switches/libPrefix",               m_Switches.libPrefix,       true);
    if (m_Mirror.Switches.libExtension != m_Switches.libExtension)
        cfg->Write(tmp + "/switches/libExtension",            m_Switches.libExtension,    true);
    if (m_Mirror.Switches.linkerNeedsLibPrefix != m_Switches.linkerNeedsLibPrefix)
        cfg->Write(tmp + "/switches/linkerNeedsLibPrefix",    m_Switches.linkerNeedsLibPrefix);
    if (m_Mirror.Switches.linkerNeedsLibExtension != m_Switches.linkerNeedsLibExtension)
        cfg->Write(tmp + "/switches/linkerNeedsLibExtension", m_Switches.linkerNeedsLibExtension);
    if (m_Mirror.Switches.linkerNeedsPathResolved != m_Switches.linkerNeedsPathResolved)
        cfg->Write(tmp + "/switches/linkerNeedsPathResolved", m_Switches.linkerNeedsPathResolved);
    if (m_Mirror.Switches.forceFwdSlashes != m_Switches.forceFwdSlashes)
        cfg->Write(tmp + "/switches/forceFwdSlashes",         m_Switches.forceFwdSlashes);
    if (m_Mirror.Switches.supportsPCH != m_Switches.supportsPCH)
        cfg->Write(tmp + "/switches/supportsPCH",             m_Switches.supportsPCH);
    if (m_Mirror.Switches.PCHExtension != m_Switches.PCHExtension)
        cfg->Write(tmp + "/switches/pchExtension",            m_Switches.PCHExtension);
    if (m_Mirror.Switches.UseFlatObjects != m_Switches.UseFlatObjects)
        cfg->Write(tmp + "/switches/UseFlatObjects",          m_Switches.UseFlatObjects);
    if (m_Mirror.Switches.UseFullSourcePaths != m_Switches.UseFullSourcePaths)
        cfg->Write(tmp + "/switches/UseFullSourcePaths",      m_Switches.UseFullSourcePaths);
    if (m_Mirror.Switches.includeDirSeparator != m_Switches.includeDirSeparator)
        cfg->Write(tmp + "/switches/includeDirSeparator",     (int)m_Switches.includeDirSeparator);
    if (m_Mirror.Switches.libDirSeparator != m_Switches.libDirSeparator)
        cfg->Write(tmp + "/switches/libDirSeparator",         (int)m_Switches.libDirSeparator);
    if (m_Mirror.Switches.objectSeparator != m_Switches.objectSeparator)
        cfg->Write(tmp + "/switches/objectSeparator",         (int)m_Switches.objectSeparator);
    if (m_Mirror.Switches.statusSuccess != m_Switches.statusSuccess)
        cfg->Write(tmp + "/switches/statusSuccess",           m_Switches.statusSuccess);
    if (m_Mirror.Switches.Use83Paths != m_Switches.Use83Paths)
        cfg->Write(tmp + "/switches/Use83Paths",              m_Switches.Use83Paths);

    // regexes
    cfg->DeleteSubPath(tmp + "/regex");
    wxString group;
    for (size_t i = 0; i < m_RegExes.size(); ++i)
    {
        if (i < m_Mirror.RegExes.size() && m_Mirror.RegExes[i] == m_RegExes[i])
            continue;

        group.Printf(_("%s/regex/re%3.3lu"), tmp.c_str(), static_cast<unsigned long>(i + 1));
        RegExStruct& rs = m_RegExes[i];
        cfg->Write(group + "/description",  rs.desc,  true);
        if (rs.lt != 0)
            cfg->Write(group + "/type",     rs.lt);
        cfg->Write(group + "/regex",        rs.GetRegExString(), true);
        if (rs.msg[0] != 0)
            cfg->Write(group + "/msg1",     rs.msg[0]);
        if (rs.msg[1] != 0)
            cfg->Write(group + "/msg2",     rs.msg[1]);
        if (rs.msg[2] != 0)
            cfg->Write(group + "/msg3",     rs.msg[2]);
        if (rs.filename != 0)
            cfg->Write(group + "/filename", rs.filename);
        if (rs.line != 0)
            cfg->Write(group + "/line",     rs.line);
    }

    // sorted flags
    if (m_Mirror.SortOptions[0] != GetCOnlyFlags())
        cfg->Write(tmp + "/sort/C",   GetCOnlyFlags());
    if (m_Mirror.SortOptions[1] != GetCPPOnlyFlags())
        cfg->Write(tmp + "/sort/CPP", GetCPPOnlyFlags());

    // custom vars
    wxString configpath = tmp + "/custom_variables/";
    cfg->DeleteSubPath(configpath);
    const StringHash& v = GetAllVars();
    for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
        cfg->Write(configpath + it->first, it->second);
}

void Compiler::LoadSettings(const wxString& baseKey)
{
    // before loading any compiler settings, keep the current settings safe
    // so we can compare them when saving: this way we can only save what's
    // different from the defaults
    MirrorCurrentSettings();

    ConfigManager* cfg = Manager::Get()->GetConfigManager("compiler");

    // read settings version
    wxString version = cfg->Read("settings_version");
    bool versionMismatch = version != CompilerSettingsVersion;

    wxString tmp;

    // if using old-style keys (using integer IDs), notify user about the changes
    static bool saidAboutCompilerIDs = false;
    tmp.Printf(_("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    if (cfg->Exists(tmp + "/name"))
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
        tmp.Printf(_("%s/%s"), baseKey.c_str(), m_ID.c_str());

    if (!cfg->Exists(tmp + "/name"))
    {
        tmp.Replace("-", wxString()); // try again using previous id format
        if (!cfg->Exists(tmp + "/name"))
            return;
    }

    wxString sep = wxFileName::GetPathSeparator();

    m_Name = cfg->Read(tmp + "/name", m_Name);

    m_MasterPath         = cfg->Read(tmp + "/master_path",     m_MasterPath);
    m_ExtraPaths         = MakeUniqueArray(GetArrayFromString(cfg->Read(tmp + "/extra_paths", ""), ";"), true);
    m_Programs.C         = cfg->Read(tmp + "/c_compiler",      m_Programs.C);
    m_Programs.CPP       = cfg->Read(tmp + "/cpp_compiler",    m_Programs.CPP);
    m_Programs.LD        = cfg->Read(tmp + "/linker",          m_Programs.LD);
    m_Programs.LIB       = cfg->Read(tmp + "/lib_linker",      m_Programs.LIB);
    m_Programs.WINDRES   = cfg->Read(tmp + "/res_compiler",    m_Programs.WINDRES);
    m_Programs.MAKE      = cfg->Read(tmp + "/make",            m_Programs.MAKE);
    m_Programs.DBGconfig = cfg->Read(tmp + "/debugger_config", m_Programs.DBGconfig);

    // set member variable containing the version string with the configuration toolchain executables, not only
    // with the default ones, otherwise we might have an empty version-string
    // Some MinGW installations do not include "mingw32-gcc" !!
    SetVersionString();

    SetCompilerOptions    (GetArrayFromString(cfg->Read(tmp + "/compiler_options", wxString())));
    SetResourceCompilerOptions(GetArrayFromString(cfg->Read(tmp + "/resource_compiler_options", wxString())));
    SetLinkerOptions      (GetArrayFromString(cfg->Read(tmp + "/linker_options",   wxString())));
    SetIncludeDirs        (GetArrayFromString(cfg->Read(tmp + "/include_dirs",     wxString())));
    SetResourceIncludeDirs(GetArrayFromString(cfg->Read(tmp + "/res_include_dirs", wxString())));
    SetLibDirs            (GetArrayFromString(cfg->Read(tmp + "/library_dirs",     wxString())));
    SetLinkLibs           (GetArrayFromString(cfg->Read(tmp + "/libraries",        wxString())));
    SetCommandsBeforeBuild(GetArrayFromString(cfg->Read(tmp + "/commands_before",  wxString())));
    SetCommandsAfterBuild (GetArrayFromString(cfg->Read(tmp + "/commands_after",   wxString())));

    for (int i = 0; i < ctCount; ++i)
    {
        wxArrayString keys = cfg->EnumerateSubPaths(tmp + "/macros/" + CommandTypeDescriptions[i]);
        for (size_t n = 0; n < keys.size(); ++n)
        {
            unsigned long index = 0;
            if (keys[n].Mid(4).ToULong(&index)) // skip 'tool'
            {
                while (index >= m_Commands[i].size())
                    m_Commands[i].push_back(CompilerTool());
                CompilerTool& tool = m_Commands[i][index];

                wxString key        = wxString::Format(_("%s/macros/%s/tool%lu/"), tmp.c_str(), CommandTypeDescriptions[i].c_str(), index);
                tool.command        = cfg->Read(key + "command");
                tool.extensions     = cfg->ReadArrayString(key + "extensions");
                tool.generatedFiles = cfg->ReadArrayString(key + "generatedFiles");
            }
        }
    }

    // switches
    m_Switches.includeDirs             = cfg->Read(tmp + "/switches/includes",                    m_Switches.includeDirs);
    m_Switches.libDirs                 = cfg->Read(tmp + "/switches/libs",                        m_Switches.libDirs);
    m_Switches.linkLibs                = cfg->Read(tmp + "/switches/link",                        m_Switches.linkLibs);
    m_Switches.defines                 = cfg->Read(tmp + "/switches/define",                      m_Switches.defines);
    m_Switches.genericSwitch           = cfg->Read(tmp + "/switches/generic",                     m_Switches.genericSwitch);
    m_Switches.objectExtension         = cfg->Read(tmp + "/switches/objectext",                   m_Switches.objectExtension);
    m_Switches.needDependencies        = cfg->ReadBool(tmp + "/switches/deps",                    m_Switches.needDependencies);
    m_Switches.forceCompilerUseQuotes  = cfg->ReadBool(tmp + "/switches/forceCompilerQuotes",     m_Switches.forceCompilerUseQuotes);
    m_Switches.forceLinkerUseQuotes    = cfg->ReadBool(tmp + "/switches/forceLinkerQuotes",       m_Switches.forceLinkerUseQuotes);
    m_Switches.logging = (CompilerLoggingType)cfg->ReadInt(tmp + "/switches/logging",             m_Switches.logging);
    m_Switches.libPrefix               = cfg->Read(tmp + "/switches/libPrefix",                   m_Switches.libPrefix);
    m_Switches.libExtension            = cfg->Read(tmp + "/switches/libExtension",                m_Switches.libExtension);
    m_Switches.linkerNeedsLibPrefix    = cfg->ReadBool(tmp + "/switches/linkerNeedsLibPrefix",    m_Switches.linkerNeedsLibPrefix);
    m_Switches.linkerNeedsLibExtension = cfg->ReadBool(tmp + "/switches/linkerNeedsLibExtension", m_Switches.linkerNeedsLibExtension);
    m_Switches.linkerNeedsPathResolved = cfg->ReadBool(tmp + "/switches/linkerNeedsPathResolved", m_Switches.linkerNeedsPathResolved);
    m_Switches.forceFwdSlashes         = cfg->ReadBool(tmp + "/switches/forceFwdSlashes",         m_Switches.forceFwdSlashes);
    m_Switches.supportsPCH             = cfg->ReadBool(tmp + "/switches/supportsPCH",             m_Switches.supportsPCH);
    m_Switches.PCHExtension            = cfg->Read(tmp + "/switches/pchExtension",                m_Switches.PCHExtension);
    m_Switches.UseFlatObjects          = cfg->ReadBool(tmp + "/switches/UseFlatObjects",          m_Switches.UseFlatObjects);
    m_Switches.UseFullSourcePaths      = cfg->ReadBool(tmp + "/switches/UseFullSourcePaths",      m_Switches.UseFullSourcePaths);
    m_Switches.Use83Paths              = cfg->ReadBool(tmp + "/switches/Use83Paths",              m_Switches.Use83Paths);
    m_Switches.includeDirSeparator  = (wxChar)cfg->ReadInt(tmp + "/switches/includeDirSeparator", (int)m_Switches.includeDirSeparator);
    m_Switches.libDirSeparator         = (wxChar)cfg->ReadInt(tmp + "/switches/libDirSeparator",  (int)m_Switches.libDirSeparator);
    m_Switches.objectSeparator         = (wxChar)cfg->ReadInt(tmp + "/switches/objectSeparator",  (int)m_Switches.objectSeparator);
    m_Switches.statusSuccess           = cfg->ReadInt(tmp + "/switches/statusSuccess",            m_Switches.statusSuccess);

    // regexes

    // because we 're only saving changed regexes, we can't just iterate like before.
    // instead, we must iterate all child-keys and deduce the regex index number from
    // the key name
    wxArrayString keys = cfg->EnumerateSubPaths(tmp + "/regex/");
    wxString group;
    long index = 0;
    for (size_t i = 0; i < keys.GetCount(); ++i)
    {
        wxString key = keys[i];

        // reNNN
        if (!key.StartsWith("re"))
            continue;
        key.Remove(0, 2);
        if (!key.ToLong(&index, 10))
            continue;

        // 'index' now holds the regex index.
        // read everything and either assign it to an existing regex
        // if the index exists, or add a new regex

        group.Printf(_("%s/regex/re%3.3ld"), tmp.c_str(), index);
        if (!cfg->Exists(group + "/description"))
            continue;

        RegExStruct rs(cfg->Read(group + "/description"),
                       (CompilerLineType)cfg->ReadInt(group + "/type", 0),
                       cfg->Read(group + "/regex"),
                       cfg->ReadInt(group + "/msg1", 0),
                       cfg->ReadInt(group + "/filename", 0),
                       cfg->ReadInt(group + "/line", 0),
                       cfg->ReadInt(group + "/msg2", 0),
                       cfg->ReadInt(group + "/msg3", 0));

        if (index <= (long)m_RegExes.size())
            m_RegExes[index - 1] = rs;
        else
            m_RegExes.push_back(rs);
    }

    // sorted flags
    m_SortOptions[0] = cfg->Read(tmp + "/sort/C", m_SortOptions[0]);
    m_SortOptions[1] = cfg->Read(tmp + "/sort/CPP", m_SortOptions[1]);

    // custom vars
    wxString configpath = tmp + "/custom_variables/";
    UnsetAllVars();
    wxArrayString list = cfg->EnumerateKeys(configpath);
    for (unsigned int i = 0; i < list.GetCount(); ++i)
        SetVar(list[i], cfg->Read(configpath + '/' + list[i]), false);

    if (versionMismatch)
    {
        wxString msg;
        msg << _("Some compiler settings defaults have changed in this version.\n"
                 "It is recommended that you allow updating of your settings to the new defaults.\n"
                 "Only disallow this if you don't want to lose any customizations you have done to this compiler's settings.\n\n"
                 "Note that the only settings that are affected are those found in \"Advanced compiler options\"...\n\n"
                 "Do you want to update your current settings to the new defaults?");
        // don't ask if the compiler is not valid (i.e. not installed), just update
        if (!IsValid() || cbMessageBox(msg, m_Name, wxICON_QUESTION | wxYES_NO) == wxID_YES)
        {
            for (int i = 0; i < ctCount; ++i)
                m_Commands[i] = m_Mirror.Commands[i];
            m_Switches = m_Mirror.Switches;
            m_Options  = m_Mirror.Options;
            m_RegExes  = m_Mirror.RegExes;
        }
    }
}

CompilerLineType Compiler::CheckForWarningsAndErrors(const wxString& line)
{
    if (!m_MultiLineMessages || (m_MultiLineMessages && !m_Error.IsEmpty()))
    {
        m_ErrorFilename.Clear();
        m_ErrorLine.Clear();
        m_Error.Clear();
    }

    for (size_t i = 0; i < m_RegExes.size(); ++i)
    {
        RegExStruct& rs = m_RegExes[i];
        if (!rs.HasRegEx())
            continue;
        const wxRegEx &regex = rs.GetRegEx();
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
                        m_Error << " ";
                    m_Error << regex.GetMatch(line, rs.msg[x]);
                }
            }
            return rs.lt;
        }
    }
    return cltNormal; // default return value
}

void Compiler::LoadDefaultOptions(const wxString& name, int recursion)
{
    wxXmlDocument options;
    wxString doc = ConfigManager::LocateDataFile("compilers/options_" + name + ".xml", sdDataUser | sdDataGlobal);
    if (doc.IsEmpty())
    {
        wxString msg(_("Error: file 'options_") + name + _(".xml' not found."));
        Manager::Get()->GetLogManager()->Log(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_ERROR);
        return;
    }
    if (recursion > 5)
    {
        wxString msg(_("Warning: '") + doc + _("' not loaded due to excessive recursion."));
        Manager::Get()->GetLogManager()->LogWarning(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_EXCLAMATION);
        return;
    }
    if (!options.Load(doc))
    {
        wxString msg(_("Error: Compiler options file '") + doc + _("' not found for compiler '") + name + "'.");
        Manager::Get()->GetLogManager()->Log(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_ERROR);
        return;
    }
    if (options.GetRoot()->GetName() != "CodeBlocks_compiler_options")
    {
        wxString msg(_("Error: Invalid Code::Blocks compiler options file for compiler '") + name + "'.");
        Manager::Get()->GetLogManager()->Log(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_ERROR);
        return;
    }
    wxString extends = options.GetRoot()->GetAttribute("extends", wxString());
    if (!extends.IsEmpty())
        LoadDefaultOptions(extends, recursion + 1);
    wxXmlNode* node = options.GetRoot()->GetChildren();
    int depth = 0;
    wxString categ;
    bool exclu = false;

    wxString baseKey = GetParentID().IsEmpty() ? "/sets" : "/user_sets";
    ConfigManager* cfg = Manager::Get()->GetConfigManager("compiler");
    wxString cmpKey;
    cmpKey.Printf(_("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    if (!cfg->Exists(cmpKey + "/name"))
        cmpKey.Printf(_("%s/%s"), baseKey.c_str(), m_ID.c_str());
    if (!cfg->Exists(cmpKey + "/name"))
        cmpKey.Replace("-", wxString());

    while (node)
    {
        const wxString value = node->GetAttribute("value", wxString());
        if ((node->GetName() == "if") && node->GetChildren())
        {
            if (EvalXMLCondition(node))
            {
                node = node->GetChildren();
                ++depth;
                continue;
            }
            else if (node->GetNext() && (node->GetNext()->GetName() == "else") &&
                     node->GetNext()->GetChildren())
            {
                node = node->GetNext()->GetChildren();
                ++depth;
                continue;
            }
        }
        else if (node->GetName() == "Program") // configuration is read so execution of renamed programs work, m_Mirror is needed to reset before leaving this function
        {
            wxString prog = node->GetAttribute("name", wxString());
            if (prog == "C")
            {
                m_Programs.C = cfg->Read(cmpKey + "/c_compiler", value);
                m_Mirror.Programs.C = value;
            }
            else if (prog == "CPP")
            {
                m_Programs.CPP = cfg->Read(cmpKey + "/cpp_compiler", value);
                m_Mirror.Programs.CPP = value;
            }
            else if (prog == "LD")
            {
                m_Programs.LD = cfg->Read(cmpKey + "/linker", value);
                m_Mirror.Programs.LD = value;
            }
            else if ((prog == "DBGconfig") && (recursion == 0))
            {
                // Check if the DBGconfig is of the format <plugin name>:<debugger config name>
                if (value.Find(':') == wxNOT_FOUND)
                {
                    if (value.IsEmpty())
                    {
                        m_Programs.DBGconfig = wxString::Format(_("NoPlugin:%s"), m_DebuggerInitialConfiguation.compilerIDName);;
                    }
                    else
                    {
                        m_Programs.DBGconfig = wxString::Format(_("%s:%s"),value, m_DebuggerInitialConfiguation.compilerIDName);;
                    }
                    m_DebuggerInitialConfiguation.debuggerConfigurationName = m_Programs.DBGconfig;
                }
                else
                {
                    m_Programs.DBGconfig = value;
                    m_DebuggerInitialConfiguation.debuggerConfigurationName = value;
                }
            }
            else if (prog == "LIB")
            {
                m_Programs.LIB = cfg->Read(cmpKey + "/lib_linker", value);
                m_Mirror.Programs.LIB = value;
            }
            else if (prog == "WINDRES")
            {
                m_Programs.WINDRES = cfg->Read(cmpKey + "/res_compiler", value);
                m_Mirror.Programs.WINDRES = value;
            }
            else if (prog == "MAKE")
            {
                m_Programs.MAKE = cfg->Read(cmpKey + "/make", value);
                m_Mirror.Programs.MAKE = value;
            }
        }
        else if (node->GetName() == "Switch")
        {
            wxString swi = node->GetAttribute("name", wxString());
            if (swi == "includeDirs")
                m_Switches.includeDirs = value;
            else if (swi == "libDirs")
                m_Switches.libDirs = value;
            else if (swi == "linkLibs")
                m_Switches.linkLibs = value;
            else if (swi == "defines")
                m_Switches.defines = value;
            else if (swi == "genericSwitch")
                m_Switches.genericSwitch = value;
            else if (swi == "objectExtension")
                m_Switches.objectExtension = value;
            else if (swi == "forceFwdSlashes")
                m_Switches.forceFwdSlashes = (value == "true");
            else if (swi == "forceLinkerUseQuotes")
                m_Switches.forceLinkerUseQuotes = (value == "true");
            else if (swi == "forceCompilerUseQuotes")
                m_Switches.forceCompilerUseQuotes = (value == "true");
            else if (swi == "needDependencies")
                m_Switches.needDependencies = (value == "true");
            else if (swi == "logging")
            {
                if (value == "full")
                    m_Switches.logging = clogFull;
                else if (value == "simple")
                    m_Switches.logging = clogSimple;
                else if (value == "none")
                    m_Switches.logging = clogNone;
                else
                    m_Switches.logging = CompilerSwitches::defaultLogging;
            }
            else if (swi == "libPrefix")
                m_Switches.libPrefix = value;
            else if (swi == "libExtension")
                m_Switches.libExtension = value;
            else if (swi == "linkerNeedsLibPrefix")
                m_Switches.linkerNeedsLibPrefix = (value == "true");
            else if (swi == "linkerNeedsLibExtension")
                m_Switches.linkerNeedsLibExtension = (value == "true");
            else if (swi == "linkerNeedsPathResolved")
                m_Switches.linkerNeedsPathResolved = (value == "true");
            else if (swi == "supportsPCH")
                m_Switches.supportsPCH = (value == "true");
            else if (swi == "PCHExtension")
                m_Switches.PCHExtension = value;
            else if (swi == "UseFlatObjects")
                m_Switches.UseFlatObjects = (value == "true");
            else if (swi == "UseFullSourcePaths")
                m_Switches.UseFullSourcePaths = (value == "true");
            else if (swi == "includeDirSeparator" && !value.IsEmpty())
                m_Switches.includeDirSeparator = value[0];
            else if (swi == "libDirSeparator" && !value.IsEmpty())
                m_Switches.libDirSeparator = value[0];
            else if (swi == "objectSeparator" && !value.IsEmpty())
                m_Switches.objectSeparator = value[0];
            else if (swi == "statusSuccess" && !value.IsEmpty())
            {
                long val;
                if (value.ToLong(&val))
                    m_Switches.statusSuccess = val;
            }
            else if (swi == "Use83Paths")
                m_Switches.Use83Paths = (value == "true");
        }
        else if (node->GetName() == "Category" && node->GetChildren())
        {
            categ = node->GetAttribute("name", wxString());
            exclu = (node->GetAttribute("exclusive", wxString()) == "true");
            node = node->GetChildren();
            ++depth;
            continue;
        }
        else if (node->GetName() == "Option")
        {
            wxString category;
            if (!node->GetAttribute("category", &category))
            {
                if (categ.IsEmpty())
                    category = "General";
                else
                    category = categ;
            }
            wxString exclusive;
            if (!node->GetAttribute("exclusive", &exclusive))
                exclusive = (exclu ? "true":"false");
            m_Options.AddOption(wxGetTranslation(node->GetAttribute("name", wxString())),
                                node->GetAttribute("option", wxString()),
                                wxGetTranslation(category),
                                node->GetAttribute("additionalLibs", wxString()),
                                node->GetAttribute("checkAgainst", wxString()),
                                wxGetTranslation(node->GetAttribute("checkMessage", wxString())),
                                node->GetAttribute("supersedes", wxString()),
                                exclusive == "true");
        }
        else if (node->GetName() == "Command")
        {
            wxString cmd = node->GetAttribute("name", wxString());
            wxString unEscape = value;
            unEscape.Replace("\\n", "\n"); // a single tool can support multiple commands
            CompilerTool tool(unEscape, node->GetAttribute("ext", wxString()),
                              node->GetAttribute("gen", wxString()));
            CommandType cmdTp = ctCount;
            if (cmd == "CompileObject")
                cmdTp = ctCompileObjectCmd;
            else if (cmd == "GenDependencies")
                cmdTp = ctGenDependenciesCmd;
            else if (cmd == "CompileResource")
                cmdTp = ctCompileResourceCmd;
            else if (cmd == "LinkExe")
                cmdTp = ctLinkExeCmd;
            else if (cmd == "LinkConsoleExe")
                cmdTp = ctLinkConsoleExeCmd;
            else if (cmd == "LinkDynamic")
                cmdTp = ctLinkDynamicCmd;
            else if (cmd == "LinkStatic")
                cmdTp = ctLinkStaticCmd;
            else if (cmd == "LinkNative")
                cmdTp = ctLinkNativeCmd;
            if  (cmdTp != ctCount)
            {
                bool assigned = false;
                CompilerToolsVector& tools = m_Commands[cmdTp];
                for (size_t i = 0; i < tools.size(); ++i)
                {
                    if (tools[i].extensions == tool.extensions)
                    {
                        tools[i] = tool;
                        assigned = true;
                        break;
                    }
                }
                if (!assigned)
                    tools.push_back(tool);
            }
        }
        else if (node->GetName() == "Sort")
        {
            wxString flags;
            if (node->GetAttribute("CFlags", &flags))
            {
                flags.Replace("\n", " ");
                flags.Replace("\r", " ");
                SetCOnlyFlags( MakeUniqueString(GetCOnlyFlags() + " " + flags, " ") );
            }
            else if (node->GetAttribute("CPPFlags", &flags))
            {
                flags.Replace("\n", " ");
                flags.Replace("\r", " ");
                SetCPPOnlyFlags( MakeUniqueString(GetCPPOnlyFlags() + " " + flags, " ") );
            }
        }
        else if (node->GetName() == "Common")
        {
            LoadDefaultOptions("common_" + node->GetAttribute("name", wxString()), recursion + 1);
        }
        else if (node->GetName() == "Debugger")
        {
            wxString debugName = node->GetAttribute("name", wxString());
            if (debugName == "executable_path")
            {
                m_DebuggerInitialConfiguation.validData = true;
                m_DebuggerInitialConfiguation.executablePath = value;
            }
            else if (debugName == "user_arguments")
                m_DebuggerInitialConfiguation.userArguments = value;
            else if (debugName == "type")
                m_DebuggerInitialConfiguation.type = value;
            else if (debugName == "init_commands")
                m_DebuggerInitialConfiguation.initCommands = value;
            else if (debugName == "disable_init")
                m_DebuggerInitialConfiguation.disableInit = (value == "true");
            else if (debugName == "watch_args")
                m_DebuggerInitialConfiguation.watchArgs = (value == "true");
            else if (debugName == "watch_locals")
                m_DebuggerInitialConfiguation.watchLocals = (value == "true");
            else if (debugName == "catch_exceptions")
                m_DebuggerInitialConfiguation.catchExceptions = (value == "true");
            else if (debugName == "eval_expression_as_tooltip")
                m_DebuggerInitialConfiguation.evalExpressionAsTooltip = (value == "true");
            else if (debugName == "add_other_search_dirs")
                m_DebuggerInitialConfiguation.addOtherSearchDirs = (value == "true");
            else if (debugName == "do_not_run_debuggee")
                m_DebuggerInitialConfiguation.doNoRunDebuggee = (value == "true");
            else if (debugName == "disassembly_flavor")
                m_DebuggerInitialConfiguation.disassemblyFlavor = value;
            else if (debugName == "instruction_set")
                m_DebuggerInitialConfiguation.instructionSet = value;
        }
        while (!node->GetNext() && depth > 0)
        {
            node = node->GetParent();
            if (node->GetName() == "Category")
            {
                categ = wxString();
                exclu = false;
            }
            --depth;
        }
        node = node->GetNext();
    }
    if (recursion == 0) // reset programs to their actual defaults (customized settings are loaded in a different function)
    {
        m_Programs.C       = m_Mirror.Programs.C;
        m_Programs.CPP     = m_Mirror.Programs.CPP;
        m_Programs.LD      = m_Mirror.Programs.LD;
        m_Programs.LIB     = m_Mirror.Programs.LIB;
        m_Programs.WINDRES = m_Mirror.Programs.WINDRES;
        m_Programs.MAKE    = m_Mirror.Programs.MAKE;
    }
}

void Compiler::LoadRegExArray(const wxString& name, bool globalPrecedence, int recursion)
{
    wxXmlDocument options;
    wxString doc;
    const wxString fn = "compilers/options_" + name + ".xml";
    if (globalPrecedence)
    {
        doc = ConfigManager::LocateDataFile(fn, sdDataGlobal);
        if (doc.IsEmpty())
            doc = ConfigManager::LocateDataFile(fn, sdDataUser);
    }
    else
        doc = ConfigManager::LocateDataFile(fn, sdDataUser | sdDataGlobal);
    if (doc.IsEmpty())
    {
        Manager::Get()->GetLogManager()->Log(_("Error: file 'options_") + name + _(".xml' not found"));
        return;
    }
    if (recursion > 5)
    {
        Manager::Get()->GetLogManager()->LogWarning(_("Warning: '") + doc + _("' not loaded due to excessive recursion"));
        return;
    }
    if (!options.Load(doc))
    {
        Manager::Get()->GetLogManager()->Log(_("Error parsing ") + doc);
        return;
    }
    wxString extends = options.GetRoot()->GetAttribute("extends", wxString());
    if (!extends.IsEmpty())
        LoadRegExArray(extends, globalPrecedence, recursion + 1);
    wxXmlNode* node = options.GetRoot()->GetChildren();
    int depth = 0;
    while (node)
    {
        const wxString value = node->GetAttribute("value", wxString());
        if (node->GetName() == "if" && node->GetChildren())
        {
            if (EvalXMLCondition(node))
            {
                node = node->GetChildren();
                ++depth;
                continue;
            }
            else if (node->GetNext() && node->GetNext()->GetName() == "else" &&
                     node->GetNext()->GetChildren())
            {
                node = node->GetNext()->GetChildren();
                ++depth;
                continue;
            }
        }
        else if (node->GetName() == "RegEx")
        {
            wxString tp = node->GetAttribute("type", wxString());
            CompilerLineType clt = cltNormal;
            if      (tp == "warning")
                clt = cltWarning;
            else if (tp == "error")
                clt = cltError;
            else if (tp == "info")
                clt = cltInfo;
            wxArrayString msg = GetArrayFromString(node->GetAttribute("msg", wxString()) + ";0;0");
            m_RegExes.push_back(RegExStruct(wxGetTranslation(node->GetAttribute("name", wxString())), clt,
                                      node->GetNodeContent().Trim().Trim(false), wxAtoi(msg[0]),
                                      wxAtoi(node->GetAttribute("file", "0")),
                                      wxAtoi(node->GetAttribute("line", "0")),
                                      wxAtoi(msg[1]), wxAtoi(msg[2]) ) );
        }
        else if (node->GetName() == "Common")
        {
            LoadRegExArray("common_" + node->GetAttribute("name", wxString()),
                           globalPrecedence, recursion + 1);
        }
        while (!node->GetNext() && depth > 0)
        {
            node = node->GetParent();
            --depth;
        }
        node = node->GetNext();
    }
}

/// Return true if a number was correctly parsed or the string is exhausted
/// The variable pointed by value will contain the parsed value or 0.
/// On failure result and index might contain random values.
static bool GetNextValue(int *result, size_t *index, const wxString &s, size_t length)
{
    int value = 0;
    size_t ii = *index;

    for ( ; ii < length; (ii)++)
    {
        const wxUniChar c = s[ii];
        if (!wxIsdigit(c))
        {
            // This should catch '..'.
            if (ii == *index)
                return false;
            else
                break;
        }

        value = value * 10 + (c - '0');
    }

    // If the string is exhausted return
    if (ii == length)
    {
        *index = ii;
        *result = value;
        return true;
    }

    // Skip the next character; if it was not a dot return error
    if (s[ii++] != '.')
        return false;

    // Check if the dot was the last character, this is a syntax error
    *index = ii;
    *result = value;
    return (ii != length);
}

/// Compares two strings in major[.minor[.patch[.tweak]]] format
/// @param[out] result Set to -1 if first < second, 0 if they are equal and 1 if first > second.
/// @return true on success and false on invalid input in first or second.
static bool CmpVersion(int &result, const wxString& first, const wxString& second)
{
    // Cache the lengths for speed
    const size_t lengthFirst = first.length();
    const size_t lengthSecond = second.length();

    // Sanity checks
    if (!lengthFirst && !lengthSecond)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("CmpVersion: Both compiler test strings are empty"));
        return false;
    }

    if (!lengthFirst)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("CmpVersion: The first compiler test string is empty"));
        return false;
    }

    if (!lengthSecond)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("CmpVersion: The second compiler test string is empty"));
        return false;
    }

    // Extract version numbers from left to right.
    // If we've exhausted one of the strings use 0 in the comparisons.
    size_t indexFirst = 0, indexSecond = 0;
    while ((indexFirst < lengthFirst) || (indexSecond < lengthSecond))
    {
        int valueFirst, valueSecond;

        if (!GetNextValue(&valueFirst, &indexFirst, first, lengthFirst))
        {
            const wxString msg = wxString::Format(_("CmpVersion: Invalid first compiler test string \"%s\""),
                                                  first);
            Manager::Get()->GetLogManager()->DebugLog(msg);
            return false;
        }

        if (!GetNextValue(&valueSecond, &indexSecond, second, lengthSecond))
        {
            const wxString msg = wxString::Format(_("CmpVersion: Invalid second compiler test string \"%s\""),
                                                  second);
            Manager::Get()->GetLogManager()->DebugLog(msg);
            return false;
        }

        if (valueFirst < valueSecond)
        {
            result = -1;
            return true;
        }
        else if (valueFirst > valueSecond)
        {
            result = 1;
            return true;
        }
    }

    result = 0;
    return true;
}

bool Compiler::EvalXMLCondition(const wxXmlNode* node)
{
    bool val = false;
    wxString test;
    if (node->GetAttribute("platform", &test))
    {
        if (test == "windows")
            val = platform::windows;
        else if (test == "macosx")
            val = platform::macosx;
        else if (test == "linux")
            val = platform::Linux;
        else if (test == "freebsd")
            val = platform::freebsd;
        else if (test == "netbsd")
            val = platform::netbsd;
        else if (test == "openbsd")
            val = platform::openbsd;
        else if (test == "darwin")
            val = platform::darwin;
        else if (test == "solaris")
            val = platform::solaris;
        else if (test == "unix")
            val = platform::Unix;
    }
    else if (node->GetAttribute("exec", &test))
    {
        wxArrayString cmd = GetArrayFromString(test, " ");
        if (cmd.IsEmpty())
            return false;
        wxString path;
        wxGetEnv("PATH", &path);
        const wxString origPath = path;
        {
            ConfigManager* cfg = Manager::Get()->GetConfigManager("compiler");
            wxString masterPath;
            wxString loc = (m_ParentID.IsEmpty() ? "/sets/" : "/user_sets/") + m_ID;
            wxArrayString extraPaths;
            if (cfg->Exists(loc + "/name"))
            {
                masterPath = cfg->Read(loc + "/master_path", wxString());
                extraPaths = MakeUniqueArray(GetArrayFromString(cfg->Read(loc + "/extra_paths",
                                                                          wxString())),
                                             true);
            }

            for (size_t i = 0; i < extraPaths.GetCount(); ++i)
                path.Prepend(extraPaths[i] + wxPATH_SEP);

            if (!masterPath.IsEmpty())
                path.Prepend(masterPath + wxPATH_SEP + masterPath + wxFILE_SEP_PATH + "bin" + wxPATH_SEP);
        }
        wxSetEnv("PATH", path);
        cmd[0] = GetExecName(cmd[0]);

        long ret = -1;
        if (!cmd[0].IsEmpty()) // should never be empty
            ret = Execute(GetStringFromArray(cmd, " ", false), cmd);

        wxSetEnv("PATH", origPath); // restore path

        if (ret != 0) // execution failed
            return (node->GetAttribute("default", wxString()) == "true");

        // If multiple tests are specified they will be ANDed; as soon as one fails the loop ends
        val = true;
        for (wxXmlAttribute *attr = node->GetAttributes(); attr && val; attr = attr->GetNext())
        {
            const wxString &name = attr->GetName();

            // Not really tests
            if ((name == "exec") || (name == "default"))
                continue;

            if (name == "regex")
            {
                wxRegEx re;
                if (re.Compile(attr->GetValue()))
                {
                    bool found = false;
                    for (size_t i = 0; i < cmd.GetCount(); ++i)
                    {
                        if (re.Matches(cmd[i]))
                        {
                            found = true;
                            break;
                        }
                    }
                    val = found;
                }
                else
                {
                    val = false;
                    const wxString msg = wxString::Format(_("Can not compile regex \"%s\" in compiler test"),
                                                          attr->GetValue());
                    Manager::Get()->GetLogManager()->DebugLog(msg);
                }

                continue;
            }

            if (!name.empty() && name[0] == 'v')
            {
                if (name == "version_greater")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check > 0);
                    else
                        val = false;
                    continue;
                }

                if (name == "version_greater_equal")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check >= 0);
                    else
                        val = false;
                    continue;
                }

                if (name == "version_equal")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check == 0);
                    else
                        val = false;
                    continue;
                }

                if (name == "version_not_equal")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check != 0);
                    else
                        val = false;
                    continue;
                }

                if (name == "version_less_equal")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check <= 0);
                    else
                        val = false;
                    continue;
                }

                if (name == "version_less")
                {
                    int check;
                    if (CmpVersion(check, cmd[0], attr->GetValue()))
                        val = (check < 0);
                    else
                        val = false;
                    continue;
                }
            }

            // Unknown test
            val = false;
            LogManager *log = Manager::Get()->GetLogManager();
            log ->DebugLog(wxString::Format(_("EvalXMLCondition: Unknown compiler test \"%s\""),
                                            name));
        }
    }
    return val;
}

wxString Compiler::GetExecName(const wxString& name)
{
    wxString ret = name;
    if (name == "C")
        ret = m_Programs.C;
    else if (name == "CPP")
        ret = m_Programs.CPP;
    else if (name == "LD")
        ret = m_Programs.LD;
    else if (name == "LIB")
        ret = m_Programs.LIB;
    else if (name == "WINDRES")
        ret = m_Programs.WINDRES;
    else if (name == "MAKE")
        ret = m_Programs.MAKE;
    return ret;
}

#ifdef __WXMSW__

// In MSW calling wxExecute in synchronous mode while the main window is not visible makes
// the system show a C::B icon in the taskbar. When this is made repeatedly (as in compiler
// loading) the result is a stream of flashing icons.
// However, wxExecute in asynchronous mode does not do this. The caveat is that we must wait
// in a loop for the end of the task and extract the command output in a separate step.

// This auxiliary class is needed for detecting the end of the task and retrieving the ouput.
// OnTerminate() will be called when the task ends with the return code of the task, and then
// the task output can be retrieved (as a stream).

class ExecProcess : public wxProcess
{
    public:
          ExecProcess(cb_unused wxEvtHandler *parent = NULL, cb_unused int id = -1)
          {
              m_status = 0;
          }

          long GetStatus() const {return m_status;}
          wxSemaphore &GetSemaphore() {return m_semaphore;}
          void OnTerminate(cb_unused int pid, int status)
          {
              m_status = status;
              m_semaphore.Post();
          }

    protected:
          long m_status;
          wxSemaphore m_semaphore;
};

// Emulates wxExecute() in synchronous mode using asynchronous mode

long Compiler::Execute(const wxString &cmd, wxArrayString &output)
{
    wxLogNull logNo; // do not warn if execution fails

    output.Clear();

    ExecProcess process;
    process.Redirect(); // capture task input/output streams

    // wxExecute in asynchronous mode returns 0 if execution failed.
    // Return -1 emulating the behaviour of wxExecute in synchronous mode
    if ( !wxExecute(cmd, wxEXEC_ASYNC, &process) )
        return -1;

    // Wait for the end of the task
    for (;;)
    {
        Manager::Yield(); // needed for semaphore update
        if (process.GetSemaphore().WaitTimeout(25) == wxSEMA_NO_ERROR)
            break;
    }

    // Loads the wxArrayString with the task output (returned in a wxInputStream)
    wxInputStream *inputStream = process.GetInputStream();
    wxTextInputStream text(*inputStream);
#if wxCHECK_VERSION(3, 0, 0)
    while (!text.GetInputStream().Eof())
#else
    while (!inputStream->Eof())
#endif
    {
        output.Add(text.ReadLine());
    }

    // Return task exit code emulating the behaviour of wxExecute in synchronous mode
    return process.GetStatus();
}

#else // __WXMSW__

long Compiler::Execute(const wxString &cmd, wxArrayString &output)
{
    wxLogNull logNo; // do not warn if execution fails
    int flags = wxEXEC_SYNC;
    #if wxCHECK_VERSION(3, 0, 0)
        // Stop event-loop while wxExecute runs, to avoid a deadlock on startup,
        // that occurs from time to time on wx3
        flags |= wxEXEC_NOEVENTS;
    #else
        flags |= wxEXEC_NODISABLE;
    #endif
    return wxExecute(cmd, output, flags);
}
#endif // __WXMSW__
