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
    #include <wx/regex.h>
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
const wxString Compiler::FilePathWithSpaces = _T("[][{}() \t#%$~[:alnum:]&_:+/\\.-]+");

// version of compiler settings
// when this is different from what is saved in the config, a message appears
// to the user saying that default settings have changed and asks him if he wants to
// use his own settings or the new defaults
const wxString CompilerSettingsVersion = _T("0.0.3");

const wxString EmptyString;

CompilerSwitches::CompilerSwitches()
{   // default based upon gnu
    includeDirs             = _T("-I");
    libDirs                 = _T("-L");
    linkLibs                = _T("-l");
    defines                 = _T("-D");
    genericSwitch           = _T("-");
    objectExtension         = _T("o");
    needDependencies        = true;
    forceFwdSlashes         = false;
    forceCompilerUseQuotes  = false;
    forceLinkerUseQuotes    = false;
    logging                 = defaultLogging;
    libPrefix               = _T("lib");
    libExtension            = _T("a");
    linkerNeedsLibPrefix    = false;
    linkerNeedsLibExtension = false;
    linkerNeedsPathResolved = false;
    supportsPCH             = true;
    PCHExtension            = _T("gch");
    UseFlatObjects          = false;
    UseFullSourcePaths      = false;
    Use83Paths              = false;
    includeDirSeparator     = _T(' ');
    libDirSeparator         = _T(' ');
    objectSeparator         = _T(' ');
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
    m_VersionString = wxEmptyString;
    m_Weight = weight;
    m_RegExes.reserve(100);
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Added compiler \"%s\""), m_Name.wx_str()));
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
    m_ID = now.Format(_T("%c"), wxDateTime::CET);
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
}

Compiler::~Compiler()
{
    //dtor
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
    if (ConfigManager::LocateDataFile(wxT("compilers/options_") + GetID() + wxT(".xml"), sdDataUser | sdDataGlobal).IsEmpty())
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
    {   // and try without appending the 'bin'
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
        if (wxIsalnum(ch) || ch == _T('_') || ch == _T('-')) // valid character
            newID.Append(ch);
        else if (wxIsspace(ch)) // convert spaces to underscores
            newID.Append(_T('_'));
        ++pos;
    }

    // make sure it's not starting with a number or a '-'.
    // if it is, prepend "cb"
    if (wxIsdigit(newID.GetChar(0)) || newID.GetChar(0) == _T('-'))
        newID.Prepend(_T("cb"));

    if (newID.IsEmpty()) // empty? wtf?
        cbThrow(_T("Can't create a valid compiler ID for ") + m_Name);
    m_ID = newID.Lower();

    // check for unique ID
    if (!IsUniqueID(m_ID))
        cbThrow(_T("Compiler ID already exists for ") + m_Name);
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
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("compiler"));

    // save settings version
    cfg->Write(_T("settings_version"), CompilerSettingsVersion);

    wxString tmp;

    // delete old-style keys (using integer IDs)
    tmp.Printf(_T("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    cfg->DeleteSubPath(tmp);

    tmp.Printf(_T("%s/%s"), baseKey.c_str(), m_ID.c_str());

    cfg->Write(tmp + _T("/name"),   m_Name);
    cfg->Write(tmp + _T("/parent"), m_ParentID, true);

    if (m_Mirror.CompilerOptions_ != m_CompilerOptions)
    {
        wxString key = GetStringFromArray(m_CompilerOptions);
        cfg->Write(tmp + _T("/compiler_options"), key, false);
    }
    if (m_Mirror.LinkerOptions != m_LinkerOptions)
    {
        wxString key = GetStringFromArray(m_LinkerOptions);
        cfg->Write(tmp + _T("/linker_options"),   key, false);
    }
    if (m_Mirror.IncludeDirs != m_IncludeDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_IncludeDirs, true) );
        cfg->Write(tmp + _T("/include_dirs"),     key, false);
    }
    if (m_Mirror.ResIncludeDirs != m_ResIncludeDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_ResIncludeDirs, true) );
        cfg->Write(tmp + _T("/res_include_dirs"), key, false);
    }
    if (m_Mirror.LibDirs != m_LibDirs)
    {
        wxString key = GetStringFromArray( MakeUniqueArray(m_LibDirs, true) );
        cfg->Write(tmp + _T("/library_dirs"),     key, false);
    }
    if (m_Mirror.LinkLibs != m_LinkLibs)
    {
        wxString key = GetStringFromArray(m_LinkLibs);
        cfg->Write(tmp + _T("/libraries"),        key, false);
    }
    if (m_Mirror.CmdsBefore != m_CmdsBefore)
    {
        wxString key = GetStringFromArray(m_CmdsBefore);
        cfg->Write(tmp + _T("/commands_before"),  key, true);
    }
    if (m_Mirror.CmdsAfter != m_CmdsAfter)
    {
        wxString key = GetStringFromArray(m_CmdsAfter);
        cfg->Write(tmp + _T("/commands_after"),   key, true);
    }

    if (m_Mirror.MasterPath != m_MasterPath)
        cfg->Write(tmp + _T("/master_path"),     m_MasterPath,         true);
    if (m_Mirror.ExtraPaths != m_ExtraPaths)
        cfg->Write(tmp + _T("/extra_paths"),     GetStringFromArray( MakeUniqueArray(m_ExtraPaths, true), _T(";") ), true);
    if (m_Mirror.Programs.C != m_Programs.C)
        cfg->Write(tmp + _T("/c_compiler"),      m_Programs.C,         true);
    if (m_Mirror.Programs.CPP != m_Programs.CPP)
        cfg->Write(tmp + _T("/cpp_compiler"),    m_Programs.CPP,       true);
    if (m_Mirror.Programs.LD != m_Programs.LD)
        cfg->Write(tmp + _T("/linker"),          m_Programs.LD,        true);
    if (m_Mirror.Programs.LIB != m_Programs.LIB)
        cfg->Write(tmp + _T("/lib_linker"),      m_Programs.LIB,       true);
    if (m_Mirror.Programs.WINDRES != m_Programs.WINDRES)
        cfg->Write(tmp + _T("/res_compiler"),    m_Programs.WINDRES,   true);
    if (m_Mirror.Programs.MAKE != m_Programs.MAKE)
        cfg->Write(tmp + _T("/make"),            m_Programs.MAKE,      true);
    if (m_Mirror.Programs.DBGconfig != m_Programs.DBGconfig)
        cfg->Write(tmp + _T("/debugger_config"), m_Programs.DBGconfig, true);

    for (int i = 0; i < ctCount; ++i)
    {
        for (size_t n = 0; n < m_Commands[i].size(); ++n)
        {
            if (n >= m_Mirror.Commands[i].size() || m_Mirror.Commands[i][n] != m_Commands[i][n])
            {
                wxString key = wxString::Format(_T("%s/macros/%s/tool%lu/"), tmp.c_str(), CommandTypeDescriptions[i].c_str(), static_cast<unsigned long>(n));
                cfg->Write(key + _T("command"), m_Commands[i][n].command);
                cfg->Write(key + _T("extensions"), m_Commands[i][n].extensions);
                cfg->Write(key + _T("generatedFiles"), m_Commands[i][n].generatedFiles);
            }
        }
    }

    // switches
    if (m_Mirror.Switches.includeDirs != m_Switches.includeDirs)
        cfg->Write(tmp + _T("/switches/includes"),                m_Switches.includeDirs,     true);
    if (m_Mirror.Switches.libDirs != m_Switches.libDirs)
        cfg->Write(tmp + _T("/switches/libs"),                    m_Switches.libDirs,         true);
    if (m_Mirror.Switches.linkLibs != m_Switches.linkLibs)
        cfg->Write(tmp + _T("/switches/link"),                    m_Switches.linkLibs,        true);
    if (m_Mirror.Switches.defines != m_Switches.defines)
        cfg->Write(tmp + _T("/switches/define"),                  m_Switches.defines,         true);
    if (m_Mirror.Switches.genericSwitch != m_Switches.genericSwitch)
        cfg->Write(tmp + _T("/switches/generic"),                 m_Switches.genericSwitch,   true);
    if (m_Mirror.Switches.objectExtension != m_Switches.objectExtension)
        cfg->Write(tmp + _T("/switches/objectext"),               m_Switches.objectExtension, true);
    if (m_Mirror.Switches.needDependencies != m_Switches.needDependencies)
        cfg->Write(tmp + _T("/switches/deps"),                    m_Switches.needDependencies);
    if (m_Mirror.Switches.forceCompilerUseQuotes != m_Switches.forceCompilerUseQuotes)
        cfg->Write(tmp + _T("/switches/forceCompilerQuotes"),     m_Switches.forceCompilerUseQuotes);
    if (m_Mirror.Switches.forceLinkerUseQuotes != m_Switches.forceLinkerUseQuotes)
        cfg->Write(tmp + _T("/switches/forceLinkerQuotes"),       m_Switches.forceLinkerUseQuotes);
    if (m_Mirror.Switches.logging != m_Switches.logging)
        cfg->Write(tmp + _T("/switches/logging"),                 m_Switches.logging);
    if (m_Mirror.Switches.libPrefix != m_Switches.libPrefix)
        cfg->Write(tmp + _T("/switches/libPrefix"),               m_Switches.libPrefix,       true);
    if (m_Mirror.Switches.libExtension != m_Switches.libExtension)
        cfg->Write(tmp + _T("/switches/libExtension"),            m_Switches.libExtension,    true);
    if (m_Mirror.Switches.linkerNeedsLibPrefix != m_Switches.linkerNeedsLibPrefix)
        cfg->Write(tmp + _T("/switches/linkerNeedsLibPrefix"),    m_Switches.linkerNeedsLibPrefix);
    if (m_Mirror.Switches.linkerNeedsLibExtension != m_Switches.linkerNeedsLibExtension)
        cfg->Write(tmp + _T("/switches/linkerNeedsLibExtension"), m_Switches.linkerNeedsLibExtension);
    if (m_Mirror.Switches.linkerNeedsPathResolved != m_Switches.linkerNeedsPathResolved)
        cfg->Write(tmp + _T("/switches/linkerNeedsPathResolved"), m_Switches.linkerNeedsPathResolved);
    if (m_Mirror.Switches.forceFwdSlashes != m_Switches.forceFwdSlashes)
        cfg->Write(tmp + _T("/switches/forceFwdSlashes"),         m_Switches.forceFwdSlashes);
    if (m_Mirror.Switches.supportsPCH != m_Switches.supportsPCH)
        cfg->Write(tmp + _T("/switches/supportsPCH"),             m_Switches.supportsPCH);
    if (m_Mirror.Switches.PCHExtension != m_Switches.PCHExtension)
        cfg->Write(tmp + _T("/switches/pchExtension"),            m_Switches.PCHExtension);
    if (m_Mirror.Switches.UseFlatObjects != m_Switches.UseFlatObjects)
        cfg->Write(tmp + _T("/switches/UseFlatObjects"),          m_Switches.UseFlatObjects);
    if (m_Mirror.Switches.UseFullSourcePaths != m_Switches.UseFullSourcePaths)
        cfg->Write(tmp + _T("/switches/UseFullSourcePaths"),      m_Switches.UseFullSourcePaths);
    if (m_Mirror.Switches.includeDirSeparator != m_Switches.includeDirSeparator)
        cfg->Write(tmp + _T("/switches/includeDirSeparator"),     (int)m_Switches.includeDirSeparator);
    if (m_Mirror.Switches.libDirSeparator != m_Switches.libDirSeparator)
        cfg->Write(tmp + _T("/switches/libDirSeparator"),         (int)m_Switches.libDirSeparator);
    if (m_Mirror.Switches.objectSeparator != m_Switches.objectSeparator)
        cfg->Write(tmp + _T("/switches/objectSeparator"),         (int)m_Switches.objectSeparator);
    if (m_Mirror.Switches.statusSuccess != m_Switches.statusSuccess)
        cfg->Write(tmp + _T("/switches/statusSuccess"),           m_Switches.statusSuccess);
    if (m_Mirror.Switches.Use83Paths != m_Switches.Use83Paths)
        cfg->Write(tmp + _T("/switches/Use83Paths"),              m_Switches.Use83Paths);

    // regexes
    cfg->DeleteSubPath(tmp + _T("/regex"));
    wxString group;
    for (size_t i = 0; i < m_RegExes.size(); ++i)
    {
        if (i < m_Mirror.RegExes.size() && m_Mirror.RegExes[i] == m_RegExes[i])
            continue;

        group.Printf(_T("%s/regex/re%3.3lu"), tmp.c_str(), static_cast<unsigned long>(i + 1));
        RegExStruct& rs = m_RegExes[i];
        cfg->Write(group + _T("/description"),  rs.desc,  true);
        if (rs.lt != 0)
            cfg->Write(group + _T("/type"),     rs.lt);
        cfg->Write(group + _T("/regex"),        rs.GetRegExString(), true);
        if (rs.msg[0] != 0)
            cfg->Write(group + _T("/msg1"),     rs.msg[0]);
        if (rs.msg[1] != 0)
            cfg->Write(group + _T("/msg2"),     rs.msg[1]);
        if (rs.msg[2] != 0)
            cfg->Write(group + _T("/msg3"),     rs.msg[2]);
        if (rs.filename != 0)
            cfg->Write(group + _T("/filename"), rs.filename);
        if (rs.line != 0)
            cfg->Write(group + _T("/line"),     rs.line);
    }

    // sorted flags
    if (m_Mirror.SortOptions[0] != GetCOnlyFlags())
        cfg->Write(tmp + _T("/sort/C"),   GetCOnlyFlags());
    if (m_Mirror.SortOptions[1] != GetCPPOnlyFlags())
        cfg->Write(tmp + _T("/sort/CPP"), GetCPPOnlyFlags());

    // custom vars
    wxString configpath = tmp + _T("/custom_variables/");
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
    {
        tmp.Replace(wxT("-"), wxEmptyString); // try again using previous id format
        if (!cfg->Exists(tmp + _T("/name")))
            return;
    }

    wxString sep = wxFileName::GetPathSeparator();

    m_Name = cfg->Read(tmp + _T("/name"), m_Name);

    m_MasterPath         = cfg->Read(tmp + _T("/master_path"),     m_MasterPath);
    m_ExtraPaths         = MakeUniqueArray(GetArrayFromString(cfg->Read(tmp + _T("/extra_paths"), _T("")), _T(";")), true);
    m_Programs.C         = cfg->Read(tmp + _T("/c_compiler"),      m_Programs.C);
    m_Programs.CPP       = cfg->Read(tmp + _T("/cpp_compiler"),    m_Programs.CPP);
    m_Programs.LD        = cfg->Read(tmp + _T("/linker"),          m_Programs.LD);
    m_Programs.LIB       = cfg->Read(tmp + _T("/lib_linker"),      m_Programs.LIB);
    m_Programs.WINDRES   = cfg->Read(tmp + _T("/res_compiler"),    m_Programs.WINDRES);
    m_Programs.MAKE      = cfg->Read(tmp + _T("/make"),            m_Programs.MAKE);
    m_Programs.DBGconfig = cfg->Read(tmp + _T("/debugger_config"), m_Programs.DBGconfig);

    // set member variable containing the version string with the configuration toolchain executables, not only
    // with the default ones, otherwise we might have an empty version-string
    // Some MinGW installations do not include "mingw32-gcc" !!
    SetVersionString();

    SetCompilerOptions    (GetArrayFromString(cfg->Read(tmp + _T("/compiler_options"), wxEmptyString)));
    SetLinkerOptions      (GetArrayFromString(cfg->Read(tmp + _T("/linker_options"),   wxEmptyString)));
    SetIncludeDirs        (GetArrayFromString(cfg->Read(tmp + _T("/include_dirs"),     wxEmptyString)));
    SetResourceIncludeDirs(GetArrayFromString(cfg->Read(tmp + _T("/res_include_dirs"), wxEmptyString)));
    SetLibDirs            (GetArrayFromString(cfg->Read(tmp + _T("/library_dirs"),     wxEmptyString)));
    SetLinkLibs           (GetArrayFromString(cfg->Read(tmp + _T("/libraries"),        wxEmptyString)));
    SetCommandsBeforeBuild(GetArrayFromString(cfg->Read(tmp + _T("/commands_before"),  wxEmptyString)));
    SetCommandsAfterBuild (GetArrayFromString(cfg->Read(tmp + _T("/commands_after"),   wxEmptyString)));

    for (int i = 0; i < ctCount; ++i)
    {
        wxArrayString keys = cfg->EnumerateSubPaths(tmp + _T("/macros/") + CommandTypeDescriptions[i]);
        for (size_t n = 0; n < keys.size(); ++n)
        {
            unsigned long index = 0;
            if (keys[n].Mid(4).ToULong(&index)) // skip 'tool'
            {
                while (index >= m_Commands[i].size())
                    m_Commands[i].push_back(CompilerTool());
                CompilerTool& tool = m_Commands[i][index];

                wxString key        = wxString::Format(_T("%s/macros/%s/tool%lu/"), tmp.c_str(), CommandTypeDescriptions[i].c_str(), index);
                tool.command        = cfg->Read(key + _T("command"));
                tool.extensions     = cfg->ReadArrayString(key + _T("extensions"));
                tool.generatedFiles = cfg->ReadArrayString(key + _T("generatedFiles"));
            }
        }
    }

    // switches
    m_Switches.includeDirs             = cfg->Read(tmp + _T("/switches/includes"),                    m_Switches.includeDirs);
    m_Switches.libDirs                 = cfg->Read(tmp + _T("/switches/libs"),                        m_Switches.libDirs);
    m_Switches.linkLibs                = cfg->Read(tmp + _T("/switches/link"),                        m_Switches.linkLibs);
    m_Switches.defines                 = cfg->Read(tmp + _T("/switches/define"),                      m_Switches.defines);
    m_Switches.genericSwitch           = cfg->Read(tmp + _T("/switches/generic"),                     m_Switches.genericSwitch);
    m_Switches.objectExtension         = cfg->Read(tmp + _T("/switches/objectext"),                   m_Switches.objectExtension);
    m_Switches.needDependencies        = cfg->ReadBool(tmp + _T("/switches/deps"),                    m_Switches.needDependencies);
    m_Switches.forceCompilerUseQuotes  = cfg->ReadBool(tmp + _T("/switches/forceCompilerQuotes"),     m_Switches.forceCompilerUseQuotes);
    m_Switches.forceLinkerUseQuotes    = cfg->ReadBool(tmp + _T("/switches/forceLinkerQuotes"),       m_Switches.forceLinkerUseQuotes);
    m_Switches.logging = (CompilerLoggingType)cfg->ReadInt(tmp + _T("/switches/logging"),             m_Switches.logging);
    m_Switches.libPrefix               = cfg->Read(tmp + _T("/switches/libPrefix"),                   m_Switches.libPrefix);
    m_Switches.libExtension            = cfg->Read(tmp + _T("/switches/libExtension"),                m_Switches.libExtension);
    m_Switches.linkerNeedsLibPrefix    = cfg->ReadBool(tmp + _T("/switches/linkerNeedsLibPrefix"),    m_Switches.linkerNeedsLibPrefix);
    m_Switches.linkerNeedsLibExtension = cfg->ReadBool(tmp + _T("/switches/linkerNeedsLibExtension"), m_Switches.linkerNeedsLibExtension);
    m_Switches.linkerNeedsPathResolved = cfg->ReadBool(tmp + _T("/switches/linkerNeedsPathResolved"), m_Switches.linkerNeedsPathResolved);
    m_Switches.forceFwdSlashes         = cfg->ReadBool(tmp + _T("/switches/forceFwdSlashes"),         m_Switches.forceFwdSlashes);
    m_Switches.supportsPCH             = cfg->ReadBool(tmp + _T("/switches/supportsPCH"),             m_Switches.supportsPCH);
    m_Switches.PCHExtension            = cfg->Read(tmp + _T("/switches/pchExtension"),                m_Switches.PCHExtension);
    m_Switches.UseFlatObjects          = cfg->ReadBool(tmp + _T("/switches/UseFlatObjects"),          m_Switches.UseFlatObjects);
    m_Switches.UseFullSourcePaths      = cfg->ReadBool(tmp + _T("/switches/UseFullSourcePaths"),      m_Switches.UseFullSourcePaths);
    m_Switches.Use83Paths              = cfg->ReadBool(tmp + _T("/switches/Use83Paths"),              m_Switches.Use83Paths);
    m_Switches.includeDirSeparator  = (wxChar)cfg->ReadInt(tmp + _T("/switches/includeDirSeparator"), (int)m_Switches.includeDirSeparator);
    m_Switches.libDirSeparator         = (wxChar)cfg->ReadInt(tmp + _T("/switches/libDirSeparator"),  (int)m_Switches.libDirSeparator);
    m_Switches.objectSeparator         = (wxChar)cfg->ReadInt(tmp + _T("/switches/objectSeparator"),  (int)m_Switches.objectSeparator);
    m_Switches.statusSuccess           = cfg->ReadInt(tmp + _T("/switches/statusSuccess"),            m_Switches.statusSuccess);

    // regexes

    // because we 're only saving changed regexes, we can't just iterate like before.
    // instead, we must iterate all child-keys and deduce the regex index number from
    // the key name
    wxArrayString keys = cfg->EnumerateSubPaths(tmp + _T("/regex/"));
    wxString group;
    long index = 0;
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

        group.Printf(_T("%s/regex/re%3.3ld"), tmp.c_str(), index);
        if (!cfg->Exists(group+_T("/description")))
            continue;

        RegExStruct rs(cfg->Read(group + _T("/description")),
                       (CompilerLineType)cfg->ReadInt(group + _T("/type"), 0),
                       cfg->Read(group + _T("/regex")),
                       cfg->ReadInt(group + _T("/msg1"), 0),
                       cfg->ReadInt(group + _T("/filename"), 0),
                       cfg->ReadInt(group + _T("/line"), 0),
                       cfg->ReadInt(group + _T("/msg2"), 0),
                       cfg->ReadInt(group + _T("/msg3"), 0));

        if (index <= (long)m_RegExes.size())
            m_RegExes[index - 1] = rs;
        else
            m_RegExes.push_back(rs);
    }

    // sorted flags
    m_SortOptions[0] = cfg->Read(tmp + _T("/sort/C"), m_SortOptions[0]);
    m_SortOptions[1] = cfg->Read(tmp + _T("/sort/CPP"), m_SortOptions[1]);

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
                        m_Error << _T(" ");
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
    wxString doc = ConfigManager::LocateDataFile(wxT("compilers/options_") + name + wxT(".xml"), sdDataUser | sdDataGlobal);
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
        wxString msg(_("Error: Compiler options file '") + doc + _("' not found for compiler '") + name + wxT("'."));
        Manager::Get()->GetLogManager()->Log(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_ERROR);
        return;
    }
    if (options.GetRoot()->GetName() != wxT("CodeBlocks_compiler_options"))
    {
        wxString msg(_("Error: Invalid Code::Blocks compiler options file for compiler '") + name + wxT("'."));
        Manager::Get()->GetLogManager()->Log(msg);
        cbMessageBox(msg, _("Compiler options"), wxICON_ERROR);
        return;
    }
    wxString extends = options.GetRoot()->GetAttribute(wxT("extends"), wxEmptyString);
    if (!extends.IsEmpty())
        LoadDefaultOptions(extends, recursion + 1);
    wxXmlNode* node = options.GetRoot()->GetChildren();
    int depth = 0;
    wxString categ;
    bool exclu = false;

    wxString baseKey = GetParentID().IsEmpty() ? wxT("/sets") : wxT("/user_sets");
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("compiler"));
    wxString cmpKey;
    cmpKey.Printf(wxT("%s/set%3.3d"), baseKey.c_str(), CompilerFactory::GetCompilerIndex(this) + 1);
    if (!cfg->Exists(cmpKey + wxT("/name")))
        cmpKey.Printf(wxT("%s/%s"), baseKey.c_str(), m_ID.c_str());
    if (!cfg->Exists(cmpKey + wxT("/name")))
        cmpKey.Replace(wxT("-"), wxEmptyString);

    while (node)
    {
        const wxString value = node->GetAttribute(wxT("value"), wxEmptyString);
        if (node->GetName() == wxT("if") && node->GetChildren())
        {
            if (EvalXMLCondition(node))
            {
                node = node->GetChildren();
                ++depth;
                continue;
            }
            else if (node->GetNext() && node->GetNext()->GetName() == wxT("else") &&
                     node->GetNext()->GetChildren())
            {
                node = node->GetNext()->GetChildren();
                ++depth;
                continue;
            }
        }
        else if (node->GetName() == wxT("Program")) // configuration is read so execution of renamed programs work, m_Mirror is needed to reset before leaving this function
        {
            wxString prog = node->GetAttribute(wxT("name"), wxEmptyString);
            if (prog == wxT("C"))
            {
                m_Programs.C = cfg->Read(cmpKey + wxT("/c_compiler"), value);
                m_Mirror.Programs.C = value;
            }
            else if (prog == wxT("CPP"))
            {
                m_Programs.CPP = cfg->Read(cmpKey + wxT("/cpp_compiler"), value);
                m_Mirror.Programs.CPP = value;
            }
            else if (prog == wxT("LD"))
            {
                m_Programs.LD = cfg->Read(cmpKey + wxT("/linker"), value);
                m_Mirror.Programs.LD = value;
            }
            else if (prog == wxT("DBGconfig"))
                m_Programs.DBGconfig = value;
            else if (prog == wxT("LIB"))
            {
                m_Programs.LIB = cfg->Read(cmpKey + wxT("/lib_linker"), value);
                m_Mirror.Programs.LIB = value;
            }
            else if (prog == wxT("WINDRES"))
            {
                m_Programs.WINDRES = cfg->Read(cmpKey + wxT("/res_compiler"), value);
                m_Mirror.Programs.WINDRES = value;
            }
            else if (prog == wxT("MAKE"))
            {
                m_Programs.MAKE = cfg->Read(cmpKey + wxT("/make"), value);
                m_Mirror.Programs.MAKE = value;
            }
        }
        else if (node->GetName() == wxT("Switch"))
        {
            wxString swi = node->GetAttribute(wxT("name"), wxEmptyString);
            if (swi == wxT("includeDirs"))
                m_Switches.includeDirs = value;
            else if (swi == wxT("libDirs"))
                m_Switches.libDirs = value;
            else if (swi == wxT("linkLibs"))
                m_Switches.linkLibs = value;
            else if (swi == wxT("defines"))
                m_Switches.defines = value;
            else if (swi == wxT("genericSwitch"))
                m_Switches.genericSwitch = value;
            else if (swi == wxT("objectExtension"))
                m_Switches.objectExtension = value;
            else if (swi == wxT("forceFwdSlashes"))
                m_Switches.forceFwdSlashes = (value == wxT("true"));
            else if (swi == wxT("forceLinkerUseQuotes"))
                m_Switches.forceLinkerUseQuotes = (value == wxT("true"));
            else if (swi == wxT("forceCompilerUseQuotes"))
                m_Switches.forceCompilerUseQuotes = (value == wxT("true"));
            else if (swi == wxT("needDependencies"))
                m_Switches.needDependencies = (value == wxT("true"));
            else if (swi == wxT("logging"))
            {
                if (value == wxT("full"))
                    m_Switches.logging = clogFull;
                else if (value == wxT("simple"))
                    m_Switches.logging = clogSimple;
                else if (value == wxT("none"))
                    m_Switches.logging = clogNone;
                else
                    m_Switches.logging = CompilerSwitches::defaultLogging;
            }
            else if (swi == wxT("libPrefix"))
                m_Switches.libPrefix = value;
            else if (swi == wxT("libExtension"))
                m_Switches.libExtension = value;
            else if (swi == wxT("linkerNeedsLibPrefix"))
                m_Switches.linkerNeedsLibPrefix = (value == wxT("true"));
            else if (swi == wxT("linkerNeedsLibExtension"))
                m_Switches.linkerNeedsLibExtension = (value == wxT("true"));
            else if (swi == wxT("linkerNeedsPathResolved"))
                m_Switches.linkerNeedsPathResolved = (value == wxT("true"));
            else if (swi == wxT("supportsPCH"))
                m_Switches.supportsPCH = (value == wxT("true"));
            else if (swi == wxT("PCHExtension"))
                m_Switches.PCHExtension = value;
            else if (swi == wxT("UseFlatObjects"))
                m_Switches.UseFlatObjects = (value == wxT("true"));
            else if (swi == wxT("UseFullSourcePaths"))
                m_Switches.UseFullSourcePaths = (value == wxT("true"));
            else if (swi == wxT("includeDirSeparator") && !value.IsEmpty())
                m_Switches.includeDirSeparator = value[0];
            else if (swi == wxT("libDirSeparator") && !value.IsEmpty())
                m_Switches.libDirSeparator = value[0];
            else if (swi == wxT("objectSeparator") && !value.IsEmpty())
                m_Switches.objectSeparator = value[0];
            else if (swi == wxT("statusSuccess") && !value.IsEmpty())
            {
                long val;
                if (value.ToLong(&val))
                    m_Switches.statusSuccess = val;
            }
            else if (swi == wxT("Use83Paths"))
                m_Switches.Use83Paths = (value == wxT("true"));
        }
        else if (node->GetName() == wxT("Category") && node->GetChildren())
        {
            categ = node->GetAttribute(wxT("name"), wxEmptyString);
            exclu = (node->GetAttribute(wxT("exclusive"), wxEmptyString) == wxT("true"));
            node = node->GetChildren();
            ++depth;
            continue;
        }
        else if (node->GetName() == wxT("Option"))
        {
            wxString category;
            if (!node->GetAttribute(wxT("category"), &category))
            {
                if (categ.IsEmpty())
                    category = wxT("General");
                else
                    category = categ;
            }
            wxString exclusive;
            if (!node->GetAttribute(wxT("exclusive"), &exclusive))
                exclusive = (exclu ? wxT("true") : wxT("false"));
            m_Options.AddOption(wxGetTranslation(node->GetAttribute(wxT("name"), wxEmptyString)),
                                node->GetAttribute(wxT("option"), wxEmptyString),
                                wxGetTranslation(category),
                                node->GetAttribute(wxT("additionalLibs"), wxEmptyString),
                                node->GetAttribute(wxT("checkAgainst"), wxEmptyString),
                                wxGetTranslation(node->GetAttribute(wxT("checkMessage"), wxEmptyString)),
                                node->GetAttribute(wxT("supersedes"), wxEmptyString),
                                exclusive == wxT("true"));
        }
        else if (node->GetName() == wxT("Command"))
        {
            wxString cmd = node->GetAttribute(wxT("name"), wxEmptyString);
            wxString unEscape = value;
            unEscape.Replace(wxT("\\n"), wxT("\n")); // a single tool can support multiple commands
            CompilerTool tool(unEscape, node->GetAttribute(wxT("ext"), wxEmptyString),
                              node->GetAttribute(wxT("gen"), wxEmptyString));
            CommandType cmdTp = ctCount;
            if (cmd == wxT("CompileObject"))
                cmdTp = ctCompileObjectCmd;
            else if (cmd == wxT("GenDependencies"))
                cmdTp = ctGenDependenciesCmd;
            else if (cmd == wxT("CompileResource"))
                cmdTp = ctCompileResourceCmd;
            else if (cmd == wxT("LinkExe"))
                cmdTp = ctLinkExeCmd;
            else if (cmd == wxT("LinkConsoleExe"))
                cmdTp = ctLinkConsoleExeCmd;
            else if (cmd == wxT("LinkDynamic"))
                cmdTp = ctLinkDynamicCmd;
            else if (cmd == wxT("LinkStatic"))
                cmdTp = ctLinkStaticCmd;
            else if (cmd == wxT("LinkNative"))
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
        else if (node->GetName() == wxT("Sort"))
        {
            wxString flags;
            if (node->GetAttribute(wxT("CFlags"), &flags))
            {
                flags.Replace(wxT("\n"), wxT(" "));
                flags.Replace(wxT("\r"), wxT(" "));
                SetCOnlyFlags( MakeUniqueString(GetCOnlyFlags() + wxT(" ") + flags,
                                                wxT(" ")) );
            }
            else if (node->GetAttribute(wxT("CPPFlags"), &flags))
            {
                flags.Replace(wxT("\n"), wxT(" "));
                flags.Replace(wxT("\r"), wxT(" "));
                SetCPPOnlyFlags( MakeUniqueString(GetCPPOnlyFlags() + wxT(" ") + flags,
                                                  wxT(" ")) );
            }
        }
        else if (node->GetName() == wxT("Common"))
        {
            LoadDefaultOptions(wxT("common_") + node->GetAttribute(wxT("name"), wxEmptyString), recursion + 1);
        }
        while (!node->GetNext() && depth > 0)
        {
            node = node->GetParent();
            if (node->GetName() == wxT("Category"))
            {
                categ = wxEmptyString;
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
    const wxString fn = wxT("compilers/options_") + name + wxT(".xml");
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
    wxString extends = options.GetRoot()->GetAttribute(wxT("extends"), wxEmptyString);
    if (!extends.IsEmpty())
        LoadRegExArray(extends, globalPrecedence, recursion + 1);
    wxXmlNode* node = options.GetRoot()->GetChildren();
    int depth = 0;
    while (node)
    {
        const wxString value = node->GetAttribute(wxT("value"), wxEmptyString);
        if (node->GetName() == wxT("if") && node->GetChildren())
        {
            if (EvalXMLCondition(node))
            {
                node = node->GetChildren();
                ++depth;
                continue;
            }
            else if (node->GetNext() && node->GetNext()->GetName() == wxT("else") &&
                     node->GetNext()->GetChildren())
            {
                node = node->GetNext()->GetChildren();
                ++depth;
                continue;
            }
        }
        else if (node->GetName() == wxT("RegEx"))
        {
            wxString tp = node->GetAttribute(wxT("type"), wxEmptyString);
            CompilerLineType clt = cltNormal;
            if      (tp == wxT("warning"))
                clt = cltWarning;
            else if (tp == wxT("error"))
                clt = cltError;
            else if (tp == wxT("info"))
                clt = cltInfo;
            wxArrayString msg = GetArrayFromString(node->GetAttribute(wxT("msg"), wxEmptyString) + wxT(";0;0"));
            m_RegExes.push_back(RegExStruct(wxGetTranslation(node->GetAttribute(wxT("name"), wxEmptyString)), clt,
                                      node->GetNodeContent().Trim().Trim(false), wxAtoi(msg[0]),
                                      wxAtoi(node->GetAttribute(wxT("file"), wxT("0"))),
                                      wxAtoi(node->GetAttribute(wxT("line"), wxT("0"))),
                                      wxAtoi(msg[1]), wxAtoi(msg[2]) ) );
        }
        else if (node->GetName() == wxT("Common"))
        {
            LoadRegExArray(wxT("common_") + node->GetAttribute(wxT("name"), wxEmptyString),
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

bool Compiler::EvalXMLCondition(const wxXmlNode* node)
{
    bool val = false;
    wxString test;
    if (node->GetAttribute(wxT("platform"), &test))
    {
        if (test == wxT("windows"))
            val = platform::windows;
        else if (test == wxT("macosx"))
            val = platform::macosx;
        else if (test == wxT("linux"))
            val = platform::Linux;
        else if (test == wxT("freebsd"))
            val = platform::freebsd;
        else if (test == wxT("netbsd"))
            val = platform::netbsd;
        else if (test == wxT("openbsd"))
            val = platform::openbsd;
        else if (test == wxT("darwin"))
            val = platform::darwin;
        else if (test == wxT("solaris"))
            val = platform::solaris;
        else if (test == wxT("unix"))
            val = platform::Unix;
    }
    else if (node->GetAttribute(wxT("exec"), &test))
    {
        wxArrayString cmd = GetArrayFromString(test, wxT(" "));
        if (cmd.IsEmpty())
            return false;
        wxString path;
        wxGetEnv(wxT("PATH"), &path);
        const wxString origPath = path;
        {
            ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("compiler"));
            wxString masterPath;
            wxString loc = (m_ParentID.IsEmpty() ? wxT("/sets/") : wxT("/user_sets/")) + m_ID;
            wxArrayString extraPaths;
            if (cfg->Exists(loc + wxT("/name")))
            {
                masterPath = cfg->Read(loc + wxT("/master_path"), wxEmptyString);
                extraPaths = MakeUniqueArray(GetArrayFromString(cfg->Read(loc + wxT("/extra_paths"), wxEmptyString)), true);
            }
            for (size_t i = 0; i < extraPaths.GetCount(); ++i)
                path.Prepend(extraPaths[i] + wxPATH_SEP);
            if (!masterPath.IsEmpty())
                path.Prepend(masterPath + wxPATH_SEP + masterPath + wxFILE_SEP_PATH + wxT("bin") + wxPATH_SEP);
        }
        wxSetEnv(wxT("PATH"), path);
        cmd[0] = GetExecName(cmd[0]);

        long ret = -1;
        if ( !cmd[0].IsEmpty() ) // should never be empty
        {
            int flags = wxEXEC_SYNC;
            #if wxCHECK_VERSION(3, 0, 0)
                // Stop event-loop while wxExecute runs, to avoid a deadlock on startup,
                // that occurs from time to time on wx3
                flags |= wxEXEC_NOEVENTS;
            #else
                flags |= wxEXEC_NODISABLE;
            #endif
            wxLogNull logNo; // do not warn if execution fails
            ret = wxExecute(GetStringFromArray(cmd, wxT(" "), false), cmd, flags);
        }

        if (ret != 0) // execution failed
            val = (node->GetAttribute(wxT("default"), wxEmptyString) == wxT("true"));
        else if (node->GetAttribute(wxT("regex"), &test))
        {
            wxRegEx re;
            if (re.Compile(test))
            {
                for (size_t i = 0; i < cmd.GetCount(); ++i)
                {
                    if (re.Matches(cmd[i]))
                    {
                        val = true;
                        break;
                    }
                }
            }
        }
        else // execution succeeded (and no regex test given)
            val = true;

        wxSetEnv(wxT("PATH"), origPath); // restore path
    }
    return val;
}

wxString Compiler::GetExecName(const wxString& name)
{
    wxString ret = name;
    if (name == wxT("C"))
        ret = m_Programs.C;
    else if (name == wxT("CPP"))
        ret = m_Programs.CPP;
    else if (name == wxT("LD"))
        ret = m_Programs.LD;
    else if (name == wxT("LIB"))
        ret = m_Programs.LIB;
    else if (name == wxT("WINDRES"))
        ret = m_Programs.WINDRES;
    else if (name == wxT("MAKE"))
        ret = m_Programs.MAKE;
    return ret;
}
