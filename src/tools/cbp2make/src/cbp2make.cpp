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
#include "macros.h"
#include "revision.h"
#include "stlfutils.h"
#include "stlgpm.h"
#include "cbbuildmgr.h"
#include "cbhelper.h"
//------------------------------------------------------------------------------

class CProcessingMachine: public CGenericProcessingMachine
{
protected:
    CCodeBlocksBuildManager m_BuildManager;
protected:
    void ConfigureToolchain(CToolChain *ToolChain);
    void ConfigureBuildTool(CBuildTool *BuildTool);
    void ConfigurePlatform(CPlatform *Platform);
public:
    virtual void DisplayHelpMessage(void);
    virtual void CreateConfiguration(void);
    virtual CString ConfigurationName(void);
    virtual bool Configure(const CString& FileName);
    virtual CString TargetName(const int FileIndex, const CString& SourceFileName);
    virtual bool ProcessFile(const CString& SourceFileName, CString& TargetFileName);
public:
    CProcessingMachine(void);
    ~CProcessingMachine(void);
};

CProcessingMachine::CProcessingMachine(void)
{
    m_BuildManager.Clear();
}

CProcessingMachine::~CProcessingMachine(void)
{
//
}

void CProcessingMachine::DisplayHelpMessage(void)
{
    std::cout<<
             "Usage syntax:\n\n"
             "\tGenerate makefile:\n"
             "\t\tcbp2make -in <project_file> [-cfg <configuration>] [-out <makefile>]\n"
             "\t\t\t [-msys] [-unix] [-windows] [-mac] [--all-os] [-targets \"<target1>[,<target2>[, ...]]\"]\n"
             "\t\t\t [--flat-objects] [--flat-objpath] [--wrap-objects] [--wrap-options]\n"
             "\t\t\t [--with-deps] [--keep-objdir] [--keep-outdir] [--target-case keep|lower|upper]\n"
             "\t\t\t [--macros-case keep|lower|upper] [--quote-path auto|never|always]\n"
             "\t\tcbp2make -list -in <project_file_list> [-cfg <configuration>]\n"
             "\t\t\t [-msys] [-unix] [-windows] [-mac] [--all-os] [-targets \"<target1>[,<target2>[, ...]]\"]\n"
             "\t\t\t [--flat-objects] [--flat-objpath] [--wrap-objects] [--wrap-options]\n"
             "\t\t\t [--with-deps] [--keep-objdir] [--keep-outdir] [--target-case keep|lower|upper]\n\n"
             "\t\t\t [--macros-case keep|lower|upper] [--quote-path auto|never|always]\n"
             "\tManage toolchains:\n"
             "\t\tcbp2make --config toolchain --add [-msys|-unix|-windows|-mac] -chain <toolchain>\n"
             "\t\tcbp2make --config toolchain --remove [-msys|-unix|-windows|-mac] -chain <toolchain>\n\n"
             "\tManage build tools:\n"
             "\t\tcbp2make --config tool --add [-msys|-unix|-windows|-mac] -chain <toolchain>\n"
             "\t\t\t -tool <tool> -type <type> <tool options>\n"
             "\t\tcbp2make --config tool --remove [-msys|-unix|-windows|-mac] -chain <toolchain>\n"
             "\t\t\t -tool <tool>\n\n"
             "\tTool types:\t pp=preprocessor as=assembler cc=compiler rc=resource compiler\n"
             "\t\t\t sl=static linker dl=dynamic linker el=executable linker\n"
             "\t\t\t nl=native linker\n"
             "\tTool options (common):\n"
             "\t\t\t -desc <description> -program <executable> -command <command_template>\n"
             "\t\t\t -mkv <make_variable> -srcext <source_extensions> -outext <output_extension>\n"
             "\t\t\t -quotepath <yes|no> -fullpath <yes|no> -unixpath <yes|no>\n"
             "\tTool options (compiler):\n"
             "\t\t\t -incsw <include_switch> -defsw <define_switch> -deps <yes|no>\n"
             "\tTool options (linker):\n"
             "\t\t\t -ldsw <library_dir_switch> -llsw <link_library_switch> -lpfx <library_prefix>\n"
             "\t\t\t -lext <library_extension> -objext <object_extension> -lflat <yes|no>\n\n"
             "\tManage platforms:\n"
             "\t\tcbp2make --config platform [-msys|-unix|-windows|-mac] [-pwd <print_dir_command>]\n"
             "\t\t\t [-cd <change_dir_command>] [-rm <remove_file_command>]\n"
             "\t\t\t [-rmf <remove_file_forced>] [-rmd <remove_dir_command>]\n"
             "\t\t\t [-cp <copy_file_command>] [-mv <move_file_command>]\n"
//"\t\t\t [-tf <test_file_command>] [-td <test_dir_command>]\n"
             "\t\t\t [-md <make_dir_command>] [-mdf <make_dir_forced>]\n"
             "\t\t\t [-make <default_make_tool>]\n\n"
             "\tManage global compiler variables:\n"
             "\t\tcbp2make --config variable --add [-set <set_name>] -name <var_name>\n"
             "\t\t\t [-desc <description>] [-field <field_name>] -value <var_value>\n"
             "\t\tcbp2make --config variable --remove [-set <set_name>] [-name <var_name>]\n"
             "\t\t\t [-field <field_name>]\n\n"
             "\tManage options:\n"
             "\t\tcbp2make --config options --default-options \"<options>\"\n"
             "\t\tcbp2make --config show\n\n"
             "\t\tcbp2make --config inherit\t// copy configuration into local\n"
             "\tCommon options:\n"
             "\t\tcbp2make --local\t// use configuration from current directory\n"
             "\t\tcbp2make --global\t// use configuration from home directory\n"
             "\t\tcbp2make --verbose\t// show project information\n"
             "\t\tcbp2make --quiet\t// hide all messages\n"
             "\t\tcbp2make --help\t\t// display this message\n"
             "\t\tcbp2make --version\t// display version information\n"
             <<std::endl;
}

void CProcessingMachine::CreateConfiguration(void)
{
    CGenericProcessingMachine::CreateConfiguration();
    CPlatform pl;
    pl.Reset(CPlatform::OS_Unix);
    CToolChain tc("gcc");
//
    PSC().InsertBooleanVariable("--version");
    PSC().InsertStringVariable("--config");
    PSC().InsertBooleanVariable("--local");
    PSC().InsertBooleanVariable("--global");
    PSC().InsertBooleanVariable("--add");
    PSC().InsertBooleanVariable("--remove");
    PSC().InsertStringVariable("-set","default");
    PSC().InsertStringVariable("-name","");
    PSC().InsertStringVariable("-desc","");
    PSC().InsertStringVariable("-field","base");
    PSC().InsertStringVariable("-value","");
//
    PSC().InsertStringVariable("-alias","gcc");
    PSC().InsertStringVariable("-chain","gcc");
    PSC().InsertStringVariable("-make","make");
    PSC().InsertStringVariable("-tool","gnu_c_compiler");
    PSC().InsertStringVariable("-type","cc");
    PSC().InsertStringVariable("-desc","GNU C Compiler");
    PSC().InsertStringVariable("-program","gcc");
    PSC().InsertStringVariable("-command","$compiler $options $includes -c $file -o $object");
    PSC().InsertStringVariable("-mkv","CC");
    PSC().InsertStringVariable("-srcext","c");
    PSC().InsertStringVariable("-outext","o");
    PSC().InsertStringVariable("-quotepath","yes");
    PSC().InsertStringVariable("-fullpath","no");
    PSC().InsertStringVariable("-unixpath","no");
    PSC().InsertStringVariable("-incsw","-I");
    PSC().InsertStringVariable("-defsw","-D");
    PSC().InsertStringVariable("-deps","yes");
    PSC().InsertStringVariable("-ldsw","-L");
    PSC().InsertStringVariable("-llsw","-l");
    PSC().InsertStringVariable("-lext","a");
    PSC().InsertStringVariable("-objext","o");
    PSC().InsertStringVariable("-lflat","no");

    PSC().InsertBooleanVariable("-msys");
    PSC().InsertBooleanVariable("-unix");
    PSC().InsertBooleanVariable("-windows");
    PSC().InsertBooleanVariable("-mac");
    PSC().InsertBooleanVariable("--all-os");
    PSC().InsertBooleanVariable("--flat-objects");
    PSC().InsertBooleanVariable("--flat-objpath");
    PSC().InsertBooleanVariable("--wrap-objects");
    PSC().InsertBooleanVariable("--wrap-options");
    PSC().InsertBooleanVariable("--with-deps");
    PSC().InsertBooleanVariable("--keep-objdir");
    PSC().InsertBooleanVariable("--keep-outdir");
    PSC().InsertStringVariable("--target-case","lower");
    PSC().InsertStringVariable("--macros-case","upper");
    PSC().InsertStringVariable("--quote-path","auto");
    PSC().InsertStringVariable("--default-options");
    PSC().InsertStringVariable("-targets","");

    PSC().InsertStringVariable("-pwd",pl.Cmd_PrintWorkDir());
    PSC().InsertStringVariable("-wd",pl.Cmd_EvalWorkDir());
    PSC().InsertStringVariable("-cd",pl.Cmd_ChangeDir());
    PSC().InsertStringVariable("-rm",pl.Cmd_RemoveFile());
    PSC().InsertStringVariable("-rmf",pl.Cmd_ForceRemoveFile());
    PSC().InsertStringVariable("-rmd",pl.Cmd_RemoveDir());
//PSC().InsertStringVariable("-tf",pl.Cmd_TestFile());
//PSC().InsertStringVariable("-td",pl.Cmd_TestDir());
    PSC().InsertStringVariable("-cp",pl.Cmd_Copy());
    PSC().InsertStringVariable("-mv",pl.Cmd_Move());
    PSC().InsertStringVariable("-md",pl.Cmd_MakeDir());
    PSC().InsertStringVariable("-mdf",pl.Cmd_ForceMakeDir());
//PSC().InsertBooleanVariable("--no-header");
//
//PSC().InsertIntegerVariable("-");
//PSC().InsertFloatVariable("-");
//PSC().InsertBooleanVariable("-");
//PSC().InsertStringVariable("-");
//PSC().InsertCharVariable("-");
}

CString CProcessingMachine::ConfigurationName(void)
{
//return CString();
    return CGenericProcessingMachine::ConfigurationName();
}

void CProcessingMachine::ConfigureToolchain(CToolChain *ToolChain)
{
 (void)ToolChain;
  /*
  CToolChain *tc = ToolChain;
  if (PSC().VarDefined("-"))
  {
   tc-> = PSC().VarNamed("-");
  }
  */
}

void CProcessingMachine::ConfigureBuildTool(CBuildTool *BuildTool)
{
    CBuildTool *bt = BuildTool;
    if (PSC().VarDefined("-desc")) {
        bt->Description() = PSC().VarNamed("-desc").GetString();
    }
    if (PSC().VarDefined("-program")) {
        bt->Program() = PSC().VarNamed("-program").GetString();
    }
    if (PSC().VarDefined("-command")) {
        bt->CommandTemplate() = PSC().VarNamed("-command").GetString();
    }
    if (PSC().VarDefined("-mkv")) {
        bt->MakeVariable() = PSC().VarNamed("-mkv").GetString();
    }
    if (PSC().VarDefined("-srcext")) {
        ParseStr(PSC().VarNamed("-srcext").GetString(),' ',bt->SourceExtensions());
        bt->SourceExtensions().RemoveDuplicates();
        bt->SourceExtensions().RemoveEmpty();
    }
    if (PSC().VarDefined("-outext")) {
        bt->TargetExtension() = PSC().VarNamed("-outext").GetString();
    }
    if (PSC().VarDefined("-quotepath")) {
        bt->NeedQuotedPath() = PSC().VarNamed("-quotepath").GetBoolean();
    }
    if (PSC().VarDefined("-fullpath")) {
        bt->NeedFullPath() = PSC().VarNamed("-fullpath").GetBoolean();
    }
    if (PSC().VarDefined("-unixpath")) {
        bt->NeedUnixPath() = PSC().VarNamed("-unixpath").GetBoolean();
    }
    CCompiler *cc = dynamic_cast<CCompiler *>(BuildTool);
    if (0!=cc) {
        if (PSC().VarDefined("-incsw")) {
            cc->IncludeDirSwitch() = PSC().VarNamed("-incsw").GetString();
        }
        if (PSC().VarDefined("-defsw")) {
            cc->DefineSwitch() = PSC().VarNamed("-defsw").GetString();
        }
        if (PSC().VarDefined("-deps")) {
            cc->NeedDependencies() = PSC().VarNamed("-deps").GetBoolean();
        }
    }
    CLinker *ln = dynamic_cast<CLinker *>(BuildTool);
    if (0!=ln) {
        if (PSC().VarDefined("-ldsw")) {
            ln->LibraryDirSwitch() = PSC().VarNamed("-ldsw").GetString();
        }
        if (PSC().VarDefined("-llsw")) {
            ln->LinkLibrarySwitch() = PSC().VarNamed("-llsw").GetString();
        }
        if (PSC().VarDefined("-lpfx")) {
            ln->LibraryPrefix() = PSC().VarNamed("-lpfx").GetString();
            ln->NeedLibraryPrefix() = !(ln->LibraryPrefix().IsEmpty());
        }
        if (PSC().VarDefined("-lext")) {
            ln->LibraryExtension() = PSC().VarNamed("-lext").GetString();
        }
        if (PSC().VarDefined("-objext")) {
            ln->ObjectExtension() = PSC().VarNamed("-objext").GetString();
        }
        if (PSC().VarDefined("-lflat")) {
            ln->NeedFlatObjects() = PSC().VarNamed("-lflat").GetBoolean();
        }
    }
}

void CProcessingMachine::ConfigurePlatform(CPlatform *Platform)
{
    CPlatform *p = Platform;
    if (PSC().VarDefined("-make")) {
        p->Cmd_Make() = PSC().VarNamed("-make").GetString();
    }
    if (PSC().VarDefined("-pwd")) {
        p->Cmd_PrintWorkDir() = PSC().VarNamed("-pwd").GetString();
    }
    if (PSC().VarDefined("-wd")) {
        p->Cmd_EvalWorkDir() = PSC().VarNamed("-wd").GetString();
    }
    if (PSC().VarDefined("-cd")) {
        p->Cmd_ChangeDir() = PSC().VarNamed("-cd").GetString();
    }
    if (PSC().VarDefined("-rm")) {
        p->Cmd_RemoveFile() = PSC().VarNamed("-rm").GetString();
    }
    if (PSC().VarDefined("-rmf")) {
        p->Cmd_ForceRemoveFile() = PSC().VarNamed("-rmf").GetString();
    }
    if (PSC().VarDefined("-rmd")) {
        p->Cmd_RemoveDir() = PSC().VarNamed("-rmd").GetString();
    }
    /*
    if (PSC().VarDefined("-tf"))
    {
     p->Cmd_TestFile() = PSC().VarNamed("-tf").GetString();
    }
    if (PSC().VarDefined("-td"))
    {
     p->Cmd_TestDir() = PSC().VarNamed("-td").GetString();
    }
    */
    if (PSC().VarDefined("-cp")) {
        p->Cmd_Copy() = PSC().VarNamed("-cp").GetString();
    }
    if (PSC().VarDefined("-mv")) {
        p->Cmd_Move() = PSC().VarNamed("-mv").GetString();
    }
    if (PSC().VarDefined("-md")) {
        p->Cmd_MakeDir() = PSC().VarNamed("-md").GetString();
    }
    if (PSC().VarDefined("-mdf")) {
        p->Cmd_ForceMakeDir() = PSC().VarNamed("-mdf").GetString();
    }
}

bool CProcessingMachine::Configure(const CString& FileName)
{
   (void)FileName;
    CGenericProcessingMachine::Configure(""/*FileName*/);
    if (DoShowHelp()) {
        DisplayHelpMessage();
        return false;
    }
    if (BeVerbose()) {
        std::cout<<"Command line parameters:"<<std::endl;
        PSC().Print(std::cout);
    }
    if (PSC().VarDefined("--version")) {
        std::cout<<"cbp2make rev."<<REVISION_NUMBER<<std::endl;
        return false;
    }
#ifdef REVISION_NUMBER
    if (!BeQuiet()) std::cout<<"Starting cbp2make rev."<<REVISION_NUMBER<<"..."<<std::endl;
#endif
    m_BuildManager.Config().BeQuiet() = BeQuiet();
    m_BuildManager.Config().BeVerbose() = BeVerbose();
    CString cfg_name = ConfigurationName();
// Lookup configuration file
// 1) if "-cfg" option is specified, use it as-is
// 2) if "-cfg" option is not specified do following:
// 2A) use default configuration in current directory
//     if it exists or "--local" option is specified.
// 2B) find and/or create configuration in user's home directory,
//     if it is not possible, fallback to current directory.
    if (!PSC().VarDefined(GPM_VAR_NAME_CFG) &&
            ((!FileExists(cfg_name) && !PSC().VarDefined("--local")) ||
             PSC().VarDefined("--global"))) {
        CString cfg_path = JoinPaths(HomeDirPath(),".cbp2make");
        if (!DirExists(cfg_path)) {
            MakeDir(cfg_path);
        }
        if (DirExists(cfg_path)) {
            cfg_name = JoinPaths(cfg_path,DefaultConfigurationName());
        }
    }
    if (FileExists(cfg_name)) {
        if (!BeQuiet()) std::cout<<"Using configuration: "<<cfg_name.GetCString()<<std::endl;
        m_BuildManager.Config().Load(cfg_name);
        m_BuildManager.Config().BeQuiet() = BeQuiet();
        m_BuildManager.Config().BeVerbose() = BeVerbose();
    } else {
        if (!BeQuiet()) std::cout<<"Using default configuration."<<std::endl;
        m_BuildManager.Config().Platforms().AddDefault();
        m_BuildManager.Config().ToolChains().AddDefault();
        //do not create configuration file unless explicitly instructed to do so
        //m_BuildManager.Config().Save(cfg_name);
    }
    if (PSC().VarDefined("--default-options")) {
        m_BuildManager.Config().DefaultOptions() = PSC().VarNamed("--default-options").GetString();
    }
    if (!m_BuildManager.Config().DefaultOptions().IsEmpty()) {
        PS().AddParameters(m_BuildManager.Config().DefaultOptions());
        PSC().ProcessParameters(PS());
        CGenericProcessingMachine::Configure("");
        m_BuildManager.Config().BeQuiet() = BeQuiet();
        m_BuildManager.Config().BeVerbose() = BeVerbose();
    }
// cache frequently used variables
    bool os_unix = PSC().VarDefined("-unix");
    bool os_windows = PSC().VarDefined("-windows");
    bool os_mac = PSC().VarDefined("-mac");
    bool os_msys = PSC().VarDefined("-msys");
    CPlatform::OS_Type os_type = CPlatform::OS_Unix;
    bool os_any = true;
    if (os_unix) os_type = CPlatform::OS_Unix;
    else if (os_windows) os_type = CPlatform::OS_Windows;
    else if (os_mac) os_type = CPlatform::OS_Mac;
    else if (os_msys) os_type = CPlatform::OS_MSys;
    else os_any = false;
// configure
    m_BuildManager.Config().Platforms().AddDefault();
    if (PSC().VarDefined("--config")) {
        CString config_item_str = PSC().VarNamed("--config").GetString();
        int config_item = GuessStr(config_item_str,"toolchain tool platform variable options inherit",
                                   config_item_str,true);
        if (0==config_item) {
            CString chain_name = PSC().VarNamed("-chain").GetString();
            if (PSC().VarDefined("--add")) {
                if (PSC().VarDefined("-chain")) {
                    CToolChain *tc = m_BuildManager.ToolChains().Find(os_type,chain_name);
                    if (0==tc) {
                        tc = m_BuildManager.ToolChains().Add(os_type,chain_name);
                    }
                    if (0==tc) return false;
                    ConfigureToolchain(tc);
                }
            } // add-toolchain
            else if (PSC().VarDefined("--remove")&&PSC().VarDefined("-chain")&&os_any) {
                m_BuildManager.ToolChains().Remove(os_type,chain_name);
            }
        }
        if (1==config_item) {
            CString chain_name = PSC().VarNamed("-chain").GetString();
            if (PSC().VarDefined("--add")) {
                if (PSC().VarDefined("-chain")) {
                    CToolChain *tc = m_BuildManager.ToolChains().Find(os_type,chain_name);
                    if (0==tc) {
                        tc = m_BuildManager.ToolChains().Add(os_type,chain_name);
                    }
                    if (0==tc) return false;
                    if (PSC().VarDefined("-tool")&&PSC().VarDefined("-type")) {
                        CString tool_type_str = PSC().VarNamed("-type").GetString();
                        int tool_type_int = GuessStr(tool_type_str,"other pp as cc rc sl dl el nl count",
                                                     tool_type_str,false);
                        CBuildTool::ToolType tool_type = (CBuildTool::ToolType)tool_type_int;
                        CString tool_name = PSC().VarNamed("-tool").GetString();
                        CBuildTool *bt = tc->FindBuildToolByName(tool_name);
                        if (0==bt) {
                            bt = tc->CreateBuildTool(tool_type);
                            bt->Alias() = tool_name;
                        }
                        if (0==bt) return false;
                        ConfigureBuildTool(bt);
                    }
                }
            } // add-tool
            else if (PSC().VarDefined("--remove")&&PSC().VarDefined("-chain")&&PSC().VarDefined("-tool")&&os_any) {
                CToolChain *tc = m_BuildManager.ToolChains().Find(os_type,chain_name);
                if (0!=tc) {
                    return tc->RemoveToolByName(PSC().VarNamed("-tool").GetString());
                }
            }
        }
        if (2==config_item) {
            if (os_any) {
                CPlatform *p = m_BuildManager.Platforms().Find(os_type);
                if (0==p) return false;
                ConfigurePlatform(p);
            }
        }
        if (3==config_item) {
            if (PSC().VarDefined("--add")) {
                CString set_name = PSC().VarNamed("-set").GetString();
                if (PSC().VarDefined("-name") && PSC().VarDefined("-value")) {
                    CString var_name = PSC().VarNamed("-name").GetString();
                    CGlobalVariableSet *vset = m_BuildManager.Config().GlobalVariables().Add(set_name);
                    CGlobalVariable *var = vset->Add(var_name);
                    if (PSC().VarDefined("-desc")) {
                        var->Description() = PSC().VarNamed("-desc").GetString();
                    }
                    if (PSC().VarDefined("-value")) {
                        var->Add(PSC().VarNamed("-field").GetString(),
                                 PSC().VarNamed("-value").GetString());
                    }
                }
            } // add variable
            else if (PSC().VarDefined("--remove")) {
                CString set_name = PSC().VarNamed("-set").GetString();
                if (PSC().VarDefined("-name")) {
                    CString var_name = PSC().VarNamed("-name").GetString();
                    CGlobalVariableSet *vset = m_BuildManager.Config().GlobalVariables().Add(set_name);
                    CGlobalVariable *var = vset->Add(var_name);
                    if (PSC().VarDefined("-field")) {
                        // if both variable and field names are defined, remove the field
                        var->Remove(PSC().VarNamed("-field").GetString());
                    } else {
                        // if variable name is defined, but field name is not, remove the variable
                        vset->Remove(var_name);
                    }
                } else {
                    // if both variable and field names are not defined,
                    // but set name is defined, remove the entire set
                    if (PSC().VarDefined("-set")) {
                        m_BuildManager.Config().GlobalVariables().Remove(set_name);
                    }
                }
            } // remove variable
        } // config variable

        if (5 == config_item) {
            /* Inherit */
            cfg_name = ConfigurationName();
        }

        if (config_item < 0) m_BuildManager.Config().Show();
        std::cout<<"Saving configuration: "<<cfg_name.GetCString()<<std::endl;
        m_BuildManager.Config().Save(cfg_name);
        return false;
    } // config
//
    bool os_all = PSC().VarDefined("--all-os");
    os_all = os_all || (os_unix && os_windows && os_mac);
    os_unix = os_unix || os_all;
    os_windows = os_windows || os_all;
    os_mac = os_mac || os_all;
    bool os_none = !os_all && !os_unix && !os_windows && !os_mac &&!os_msys;
    if (os_none) {
#ifdef OS_UNIX
        os_unix = true;
#endif
#ifdef OS_WIN
        os_windows = true;
#endif
#ifdef OS_MAC
        os_mac = true;
#endif
    }
    if (os_unix) {
        CPlatform *p = m_BuildManager.Config().Platforms().Find(CPlatform::OS_Unix);
        if (p) p->Active() = true;
    }
    if (os_msys) {
        CPlatform *p = m_BuildManager.Config().Platforms().Find(CPlatform::OS_MSys);
        if (p) p->Active() = true;
    }
    if (os_windows) {
        CPlatform *p = m_BuildManager.Config().Platforms().Find(CPlatform::OS_Windows);
        if (p) p->Active() = true;
    }
    if (os_mac) {
        CPlatform *p = m_BuildManager.Config().Platforms().Find(CPlatform::OS_Mac);
        if (p) p->Active() = true;
    }
//
    {
        CString vset_name = PSC().VarNamed("-set").GetString();
        CGlobalVariableSet *vset = m_BuildManager.Config().GlobalVariables().Find(vset_name);
        if (0!=vset) {
            vset->Active() = true;
        }
    }
    m_BuildManager.Config().FlatObjectNames() = PSC().VarDefined("--flat-objects");
    m_BuildManager.Config().FlatObjectPaths() = PSC().VarDefined("--flat-objpath");
    m_BuildManager.Config().MultilineObjects() = PSC().VarDefined("--wrap-objects");
    m_BuildManager.Config().MultilineOptions() = PSC().VarDefined("--wrap-options");
    m_BuildManager.Config().IncludeDependencies() = PSC().VarDefined("--with-deps");
    {
        CString target_case_name = PSC().VarNamed("--target-case").GetString();
        m_BuildManager.Config().TargetNameCase() =
            GuessStr(target_case_name,"keep lower upper",target_case_name,false);
    }
    {
        CString macros_case_name = PSC().VarNamed("--macros-case").GetString();
        m_BuildManager.Config().MacroVariableCase() =
            GuessStr(macros_case_name,"keep lower upper",macros_case_name,false);
    }
    {
        CString quote_path_name = PSC().VarNamed("--quote-path").GetString();
        m_BuildManager.Config().QuotePathMode() =
            GuessStr(quote_path_name,"auto never always",quote_path_name,false);
    }
    m_BuildManager.Config().KeepObjectDirectories() = PSC().VarDefined("--keep-objdir");
    m_BuildManager.Config().KeepOutputDirectories() = PSC().VarDefined("--keep-outdir");
    {
        CStringList targets;
        ParseStr(PSC().VarNamed("-targets").GetString(),',',targets);
        targets.RemoveEmpty();
        targets.RemoveDuplicates();
        m_BuildManager.Config().Targets() = targets;
    }
//if (BeVerbose()) PSC().Print(std::cout);
    return true;
}

CString CProcessingMachine::TargetName(const int FileIndex, const CString& SourceFileName)
{
   (void)FileIndex;
    return SourceFileName+".mak";//ChangeFileExt(SourceFileName,".mak");
}

bool CProcessingMachine::ProcessFile(const CString& SourceFileName, CString& TargetFileName)
{
    bool result = false;
    if (!BeQuiet()) std::cout<<"Loading file '"
                                 <<SourceFileName.GetString()<<"': "<<std::flush;
    result = m_BuildManager.LoadProjectOrWorkspace(SourceFileName);
    if (result) {
        if (!BeQuiet()) std::cout<<"[DONE]"<<std::endl<<std::flush;
    } else {
        if (!BeQuiet()) std::cout<<"[FAILED]"<<std::endl<<std::flush;
    }
//
    if (BeVerbose()) m_BuildManager.Show();
//
    if (!BeQuiet()) std::cout<<"Generating makefile(s): "<<TargetFileName.GetString()<<": "<<std::flush;
    m_BuildManager.GenerateMakefile(TargetFileName);
    if (!BeQuiet()) std::cout<<"[DONE]"<<std::endl<<std::flush;
    return result;
}

CProcessingMachine PM;

int main(int argc, char* argv[])
{
    PM.Initialize(argc,argv);
    PM.Run();
    PM.Finalize();
    return 0;
}
//---------------------------------------------------------------------------
