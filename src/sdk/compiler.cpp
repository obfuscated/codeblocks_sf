#include "compiler.h"
#include "manager.h"
#include "messagemanager.h"
#include "configmanager.h"
#include "globals.h"
#include <wx/intl.h>
#include <wx/regex.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(RegExArray);

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
    _("Link object files to static library")
};
long Compiler::CompilerIDCounter = 0; // built-in compilers can have IDs from 1 to 255
long Compiler::UserCompilerIDCounter = 255; // user compilers have IDs over 255 (256+)

Compiler::Compiler(const wxString& name)
    : m_Name(name),
    m_ID(++CompilerIDCounter),
    m_ParentID(-1)
{
	//ctor
    Manager::Get()->GetMessageManager()->DebugLog("Added compiler \"%s\"", m_Name.c_str());
}

Compiler::Compiler(const Compiler& other)
    : m_ID(++UserCompilerIDCounter),
    m_ParentID(other.m_ID)
{
    m_Name = "Copy of " + other.m_Name;
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
}

Compiler::~Compiler()
{
	//dtor
}

void Compiler::SaveSettings(const wxString& baseKey)
{
    wxString tmp;
    tmp.Printf("%s/%3.3d", baseKey.c_str(), (int)m_ID);
	ConfigManager::Get()->Write(tmp + "/_name", m_Name);
	ConfigManager::Get()->Write(tmp + "/_parent", m_ParentID);

	wxString key = GetStringFromArray(m_CompilerOptions);
	ConfigManager::Get()->Write(tmp + "/compiler_options", key);
	key = GetStringFromArray(m_LinkerOptions);
	ConfigManager::Get()->Write(tmp + "/linker_options", key);
	key = GetStringFromArray(m_IncludeDirs);
	ConfigManager::Get()->Write(tmp + "/include_dirs", key);
	key = GetStringFromArray(m_LibDirs);
	ConfigManager::Get()->Write(tmp + "/library_dirs", key);
	key = GetStringFromArray(m_LinkLibs);
	ConfigManager::Get()->Write(tmp + "/libraries", key);
	key = GetStringFromArray(m_CmdsBefore);
	ConfigManager::Get()->Write(tmp + "/commands_before", key);
	key = GetStringFromArray(m_CmdsAfter);
	ConfigManager::Get()->Write(tmp + "/commands_after", key);

    ConfigManager::Get()->Write(tmp + "/master_path", m_MasterPath);
    ConfigManager::Get()->Write(tmp + "/c_compiler", m_Programs.C);
    ConfigManager::Get()->Write(tmp + "/cpp_compiler", m_Programs.CPP);
    ConfigManager::Get()->Write(tmp + "/linker", m_Programs.LD);
    ConfigManager::Get()->Write(tmp + "/lib_linker", m_Programs.LIB);
    ConfigManager::Get()->Write(tmp + "/res_compiler", m_Programs.WINDRES);
    ConfigManager::Get()->Write(tmp + "/make", m_Programs.MAKE);
    ConfigManager::Get()->Write(tmp + "/debugger", m_Programs.DBG);

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        ConfigManager::Get()->Write(tmp + "/macros/" + CommandTypeDescriptions[i], m_Commands[i]);
    }

    // switches
    ConfigManager::Get()->Write(tmp + "/switches/includes", m_Switches.includeDirs);
    ConfigManager::Get()->Write(tmp + "/switches/libs", m_Switches.libDirs);
    ConfigManager::Get()->Write(tmp + "/switches/link", m_Switches.linkLibs);
    ConfigManager::Get()->Write(tmp + "/switches/define", m_Switches.defines);
    ConfigManager::Get()->Write(tmp + "/switches/generic", m_Switches.genericSwitch);
    ConfigManager::Get()->Write(tmp + "/switches/objectext", m_Switches.objectExtension);
    ConfigManager::Get()->Write(tmp + "/switches/deps", m_Switches.needDependencies);
    ConfigManager::Get()->Write(tmp + "/switches/forceCompilerQuotes", m_Switches.forceCompilerUseQuotes);
    ConfigManager::Get()->Write(tmp + "/switches/forceLinkerQuotes", m_Switches.forceLinkerUseQuotes);
    ConfigManager::Get()->Write(tmp + "/switches/logging", m_Switches.logging);
    ConfigManager::Get()->Write(tmp + "/switches/buildMethod", m_Switches.buildMethod);
    ConfigManager::Get()->Write(tmp + "/switches/libPrefix", m_Switches.libPrefix);
    ConfigManager::Get()->Write(tmp + "/switches/libExtension", m_Switches.libExtension);
    ConfigManager::Get()->Write(tmp + "/switches/linkerNeedsLibPrefix", m_Switches.linkerNeedsLibPrefix);
    ConfigManager::Get()->Write(tmp + "/switches/linkerNeedsLibExtension", m_Switches.linkerNeedsLibExtension);

    // regexes
    ConfigManager::Get()->DeleteGroup(tmp + "/regex");
    wxString group;
    for (size_t i = 0; i < m_RegExes.Count(); ++i)
    {
        group.Printf("%s/regex/%3.3d", tmp.c_str(), i + 1);
        RegExStruct& rs = m_RegExes[i];
        ConfigManager::Get()->Write(group + "/description", rs.desc);
        ConfigManager::Get()->Write(group + "/type", rs.lt);
        ConfigManager::Get()->Write(group + "/regex", rs.regex);
        ConfigManager::Get()->Write(group + "/msg1", rs.msg[0]);
        ConfigManager::Get()->Write(group + "/msg2", rs.msg[1]);
        ConfigManager::Get()->Write(group + "/msg3", rs.msg[2]);
        ConfigManager::Get()->Write(group + "/filename", rs.filename);
        ConfigManager::Get()->Write(group + "/line", rs.line);
    }
}

void Compiler::LoadSettings(const wxString& baseKey)
{
    wxString tmp;
    tmp.Printf("%s/%3.3d", baseKey.c_str(), (int)m_ID);
    if (!ConfigManager::Get()->HasEntry(tmp) && !ConfigManager::Get()->HasGroup(tmp))
        return;

    wxString sep = wxFileName::GetPathSeparator();

	m_Name = ConfigManager::Get()->Read(tmp + "/_name", m_Name);

    m_MasterPath = ConfigManager::Get()->Read(tmp + "/master_path", m_MasterPath);
    m_Programs.C = ConfigManager::Get()->Read(tmp + "/c_compiler", m_Programs.C);
    m_Programs.CPP = ConfigManager::Get()->Read(tmp + "/cpp_compiler", m_Programs.CPP);
    m_Programs.LD = ConfigManager::Get()->Read(tmp + "/linker", m_Programs.LD);
    m_Programs.LIB = ConfigManager::Get()->Read(tmp + "/lib_linker", m_Programs.LIB);
    m_Programs.WINDRES = ConfigManager::Get()->Read(tmp + "/res_compiler", m_Programs.WINDRES);
    m_Programs.MAKE = ConfigManager::Get()->Read(tmp + "/make", m_Programs.MAKE);
    m_Programs.DBG = ConfigManager::Get()->Read(tmp + "/debugger", m_Programs.DBG);

    SetCompilerOptions(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/compiler_options", wxEmptyString)));
    SetLinkerOptions(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/linker_options", wxEmptyString)));
    SetIncludeDirs(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/include_dirs", m_MasterPath + sep + "include")));
    SetLibDirs(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/library_dirs", m_MasterPath + sep + "lib")));
    SetLinkLibs(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/libraries", "")));
    SetCommandsBeforeBuild(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/commands_before", wxEmptyString)));
    SetCommandsAfterBuild(GetArrayFromString(ConfigManager::Get()->Read(tmp + "/commands_after", wxEmptyString)));

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i] = ConfigManager::Get()->Read(tmp + "/macros/" + CommandTypeDescriptions[i], m_Commands[i]);
    }

    // switches
    m_Switches.includeDirs = ConfigManager::Get()->Read(tmp + "/switches/includes", m_Switches.includeDirs);
    m_Switches.libDirs = ConfigManager::Get()->Read(tmp + "/switches/libs", m_Switches.libDirs);
    m_Switches.linkLibs = ConfigManager::Get()->Read(tmp + "/switches/link", m_Switches.linkLibs);
    m_Switches.defines = ConfigManager::Get()->Read(tmp + "/switches/define", m_Switches.defines);
    m_Switches.genericSwitch = ConfigManager::Get()->Read(tmp + "/switches/generic", m_Switches.genericSwitch);
    m_Switches.objectExtension = ConfigManager::Get()->Read(tmp + "/switches/objectext", m_Switches.objectExtension);
    m_Switches.needDependencies = ConfigManager::Get()->Read(tmp + "/switches/deps", m_Switches.needDependencies);
    m_Switches.forceCompilerUseQuotes = ConfigManager::Get()->Read(tmp + "/switches/forceCompilerQuotes", m_Switches.forceCompilerUseQuotes);
    m_Switches.forceLinkerUseQuotes = ConfigManager::Get()->Read(tmp + "/switches/forceLinkerQuotes", m_Switches.forceLinkerUseQuotes);
    m_Switches.logging = (CompilerLoggingType)ConfigManager::Get()->Read(tmp + "/switches/logging", m_Switches.logging);
    m_Switches.buildMethod = (CompilerBuildMethod)ConfigManager::Get()->Read(tmp + "/switches/buildMethod", m_Switches.buildMethod);
    m_Switches.libPrefix = ConfigManager::Get()->Read(tmp + "/switches/libPrefix", m_Switches.libPrefix);
    m_Switches.libExtension = ConfigManager::Get()->Read(tmp + "/switches/libExtension", m_Switches.libExtension);
    m_Switches.linkerNeedsLibPrefix = ConfigManager::Get()->Read(tmp + "/switches/linkerNeedsLibPrefix", m_Switches.linkerNeedsLibPrefix);
    m_Switches.linkerNeedsLibExtension = ConfigManager::Get()->Read(tmp + "/switches/linkerNeedsLibExtension", m_Switches.linkerNeedsLibExtension);

    // regexes
    wxString group;
    int index = 1;
    bool cleared = false;
    while (true)
    {
        group.Printf("%s/regex/%3.3d", tmp.c_str(), index++);
        if (!ConfigManager::Get()->HasGroup(group))
            break;
        else if (!cleared)
        {
            cleared = true;
            m_RegExes.Clear();
        }
        RegExStruct rs;
        rs.desc = ConfigManager::Get()->Read(group + "/description");
        rs.lt = (CompilerLineType)ConfigManager::Get()->Read(group + "/type", 0L);
        rs.regex = ConfigManager::Get()->Read(group + "/regex");
        rs.msg[0] = ConfigManager::Get()->Read(group + "/msg1", 0L);
        rs.msg[1] = ConfigManager::Get()->Read(group + "/msg2", 0L);
        rs.msg[2] = ConfigManager::Get()->Read(group + "/msg3", 0L);
        rs.filename = ConfigManager::Get()->Read(group + "/filename", 0L);
        rs.line = ConfigManager::Get()->Read(group + "/line", 0L);
        m_RegExes.Add(rs);
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
                 m_ErrorFilename = regex.GetMatch(line, rs.filename);
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
