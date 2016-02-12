/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <settings.h>
    #include <manager.h>
    #include <logmanager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <projectmanager.h>
    #include <macrosmanager.h>
    #include <compilerfactory.h>
    #include <cbproject.h>
    #include <cbeditor.h>
    #include <globals.h>
#endif
#include "cbstyledtextctrl.h"

#include "scripting/bindings/scriptbindings.h"
#include <cbexception.h>
#include "scripting/bindings/sc_base_types.h"
#include "scripting/bindings/sc_cb_vm.h"
#include "scripting/bindings/sq_wx/sq_wx.h"

/** \defgroup Squirrel Squirrel Binding
 *  \brief The Squirrel scripting Module of Code Blocks
 */

namespace ScriptBindings
{
    // FIXME (bluehazzard#1#): UGLY UGLY UGLY
    extern void Register_Constants(HSQUIRRELVM vm);
    extern void Register_Globals(HSQUIRRELVM vm);
    extern void Register_wxTypes(HSQUIRRELVM vm);
    extern void Register_Dialog(HSQUIRRELVM vm);
    extern void Register_ProgressDialog(HSQUIRRELVM vm);
    extern void Register_UtilDialogs(HSQUIRRELVM vm);
    extern void Register_IO(HSQUIRRELVM vm);
    extern void Register_ScriptPlugin(HSQUIRRELVM vm);

    /** \defgroup sq_config_manager Squirrel Config-Manager binding
     *  \ingroup Squirrel
     *  \brief The binding for the Config Manager to Squirrel. It an be used to safe and load configuration Data from the global/local Configuration-File that C::B uses
     */

    /** \ingroup sq_config_manager
     *### Read(key,default_value)
     *  Reads an Value from the Config file
     *
     *  - __key__            A Key which represents the Value
     *  - __default_value__  A default value if the key was not found in the config file
     *
     * The first parameter is a Key which represents the Value in the Config file. The second parameter defines a default value which will be returned if the key was not found
     *  Supported types are int, bool, float and wxString.
     */
    SQInteger ConfigManager_Read(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        int paramCount = sa.GetParamCount();
        try
        {
        if (paramCount == 3)
        {
            wxString key = *sa.GetInstance<wxString>(2);
            if(key.IsEmpty())
                return sa.ThrowError(_("ConfigManager.read: have no key"));

            SQObjectType config_type = sq_gettype(vm,3);
            if (config_type == OT_INTEGER)
            {
                Sqrat::Var<SQInteger> val(vm,3);
                int value = Manager::Get()->GetConfigManager(_T("scripts"))->ReadInt(key, val.value);
                sa.PushValue<SQInteger>(value);
            }
            else if (config_type == OT_BOOL)
            {
                Sqrat::Var<bool> val(vm,3);
                bool value = Manager::Get()->GetConfigManager(_T("scripts"))->ReadBool(key, val.value);
                 sa.PushValue<SQBool>(value);
            }
            else if (config_type == OT_FLOAT)
            {
                Sqrat::Var<float> val(vm,3);
                bool value = Manager::Get()->GetConfigManager(_T("scripts"))->ReadDouble(key, val.value);
                sa.PushValue<SQFloat>(value);
            }
            else
            {
                wxString val = *sa.GetInstance<wxString>(3);
                wxString ret = Manager::Get()->GetConfigManager(_T("scripts"))->Read(key, val);
                sa.PushInstanceCopy<wxString>(ret);
            }
            // TODO (bluehazzard#1#): Not sure if this works...
            if(sa.HasError())
                return SC_RETURN_FAILED;
            else
                return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"ConfigManager::Read\""));
    }

    /** \ingroup sq_config_manager
     *### Write(key,value)
     *  Writes an value to the configuration file
     *
     *  - __key__   A Key which represents the Value
     *  - __value__ The value written to the file
     *
     * The first parameter is a Key which represents the Value in the Config file. The second parameter defines a default value which will be returned if the key was not found
     *  Supported types are int, bool, float and wxString.
     */
    SQInteger ConfigManager_Write(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        int paramCount = sa.GetParamCount();
        try
        {
        if (paramCount == 3)
        {
            wxString *key = sa.GetInstance<wxString>(2);

            if(key == NULL)
                return sa.ThrowError(_T("have no key"));

            SQObjectType config_type = sq_gettype(vm,3);
            if (config_type == OT_INTEGER)
            {
                int value = sa.GetValue<int>(3);
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(*key, value);
                return SC_RETURN_OK;
            }
            else if (config_type == OT_BOOL)
            {
                bool value = sa.GetValue<bool>(3);
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(*key, value);
                return SC_RETURN_OK;
            }
            else if (config_type == OT_FLOAT)
            {
                float value = sa.GetValue<float>(3);
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(*key, value);
                return SC_RETURN_OK;
            }
            else
            {
                wxString val = sa.GetValue<wxString>(3);
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(*key, val);
                return SC_RETURN_OK;
            }

        }
        else if (paramCount == 4)
        {
            wxString *key = sa.GetInstance<wxString>(2);
            wxString *val = sa.GetInstance<wxString>(3);
            if (sa.GetType(4) == OT_BOOL)
            {
                bool value = sa.GetInstance<bool>(4);
                Manager::Get()->GetConfigManager(_T("scripts"))->Write(*key, *val, value);
                return SC_RETURN_OK;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_T("Invalid arguments to \"ConfigManager::Write\""));
    }

    /** \defgroup sq_editor_manager SquirrelEditor-Manager binding
     *  \ingroup Squirrel
     *  \brief The binding for the Editor Manager to Squirrel.
     */

    /** \ingroup sq_editor_manager
     *### GetBuiltinEditor(id)
     *  Writes an value to the configuration file
     *
     *  - __id__     The identifier of the Editor you want
     *
     *  Supported types are int and wxString.
     */
    SQInteger EditorManager_GetBuiltinEditor(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbEditor* ed = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
            {
                ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(sa.GetValue<SQInteger>(2));
            }
            else
            {
                wxString *val = sa.GetInstance<wxString>(2);
                ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(*val);
            }
            sa.PushValue<cbEditor*>(ed);
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"EditorManager::GetBuiltinEditor\""));
    }

    /** \ingroup sq_editor_manager
     *### Open(FileName)
     *  Opens a file in the editor
     *
     *  - __FileName__     The Name of the file to be opened
     *
     *  Supported types are wxString.
     */
    SQInteger EditorManager_Open(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            wxString *val = sa.GetInstance<wxString>(2);
            cbEditor* ed = Manager::Get()->GetEditorManager()->Open(*val);
            sa.PushValue<cbEditor*>(ed);
            return SC_RETURN_VALUE;
        }
        return sa.ThrowError(_("Invalid arguments to \"EditorManager::Open\""));
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }


    /** \ingroup sq_editor_manager
     *### Close(id)
     *  Closes an editor
     *
     *  - __id__     The identifier of the Editor you want close
     *
     *  Supported types are int and wxString.
     */
    SQInteger EditorManager_Close(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            if (sa.GetType(2) == OT_INTEGER)
            {
                sa.PushValue<bool>(Manager::Get()->GetEditorManager()->Close(sa.GetValue<SQInteger>(2)));
                return SC_RETURN_VALUE;
            }
            else
            {
                wxString *val = sa.GetInstance<wxString>(2);
                sa.PushValue<bool>(Manager::Get()->GetEditorManager()->Close(*val));
                return SC_RETURN_VALUE;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"EditorManager::Close\""));
    }

    /** \ingroup sq_editor_manager
     *### Save(id)
     *  Save the content from the Editor with the _id_
     *
     *  - __id__     The identifier of the Editor you want to save
     *
     *  Supported types are int and wxString.
     */
    SQInteger EditorManager_Save(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            if (sa.GetType(2) == OT_INTEGER)
            {
                sa.PushValue<bool>(Manager::Get()->GetEditorManager()->Save(sa.GetValue<SQInteger>(2)));
                return SC_RETURN_VALUE;
            }
            else
            {
                wxString *val = sa.GetInstance<wxString>(2);
                sa.PushValue<bool>(Manager::Get()->GetEditorManager()->Save(*val));
                return SC_RETURN_VALUE;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"EditorManager::Save\""));
    }

    /** \defgroup sq_project Squirrel Project Manager binding
     *  \ingroup Squirrel
     *  \brief The binding for the Project Manager to Squirrel.
     *
     *  Can be used to manage project and build targets
     */


    /** \ingroup sq_project
     *### RemoveFile(ProjectFile)
     *  Remove the _ProjectFile_ from the project
     *
     *  - __ProjectFile__     The identifier of the Editor you want to save [wxString | int]
     *
     */
    SQInteger cbProject_RemoveFile(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            if (sa.GetType(2) == OT_INTEGER)
            {
                return sa.ThrowError(_("Invalid arguments to \"cbProject::RemoveFile\""));
            }
            else
            {
                ProjectFile *val = sa.GetInstance<ProjectFile>(2);
                sa.PushValue<bool>(prj->RemoveFile(val));
                return SC_RETURN_VALUE;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::RemoveFile\""));
    }

    /** \ingroup sq_project
     *### AddFile(targetName,filename,compile,link,weight)
     *  Add a file to the _targetName_ build-target
     *
     *  - __targetName__    targetName The name of the build target to add this file to [wxString]
     *  - __filename__      The file's filename. This *must* be a filename relative to the project's path. [wxString]
     *  - __compile__       If true this file is compiled when building the project. [bool]
     *  - __link__          If true this file is linked when building the project. [bool]
     *  - __weight__        A value between zero and 100 (defaults to 50). Smaller weight, makes the file compile earlier than those with larger weight. [int]
     *
     *  return Project file
     */
    SQInteger cbProject_AddFile(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount >= 3)
        {
            cbProject* prj  =  sa.GetInstance<cbProject>(1);
            wxString str    = *sa.GetInstance<wxString>(3);
            bool b1 = true;
            bool b2 = true;
            int i =  50;
            if(paramCount >= 4)
            {
                b1 = sa.GetValue<bool>(4);
            }
            if(paramCount >= 5)
            {
                b2 = sa.GetValue<bool>(5);
            }
            if(paramCount == 6)
            {
                i = sa.GetValue<SQInteger>(6);
            }
            if(Sqrat::Error::Occurred(vm))
            {
                return sa.ThrowError(_("Invalid arguments to \"cbProject::AddFile\""));
            }

            ProjectFile* pf = nullptr;
            if (sq_gettype(vm,2) == OT_INTEGER)
            {
                SQInteger id = sa.GetValue<SQInteger>(2);
                pf = prj->AddFile(id, str, b1, b2, i);
            }
            else
                pf = prj->AddFile(*sa.GetInstance<wxString>(2), str, b1, b2, i);

            sa.PushValue<ProjectFile*>(pf);
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::AddFile\""));
    }

    SQInteger cbProject_GetBuildTarget(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            ProjectBuildTarget* bt = nullptr;
            try
            {
                if (sa.GetType(2) == OT_INTEGER)
                {
                    bt = prj->GetBuildTarget(sa.GetValue<SQInteger>(2));
                    //if(bt == nullptr)
                        //return sa.ThrowError(wxString::Format(_("Could not find build Targte Nr. %d"),sa.GetValue<SQInteger>(2)));
                }
                else
                {
                    bt = prj->GetBuildTarget(*sa.GetInstance<wxString>(2));
                    //if(bt == nullptr)
                        //return sa.ThrowError(_("Could not find build Targte") + *sa.GetInstance<wxString>(2));
                }
            } catch(CBScriptException &e)
            {
                // a error occurred, we return a null ptr
                bt = nullptr;
            }

            sa.PushInstance<ProjectBuildTarget>(bt);
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::GetBuildTarget\""));
    }

    SQInteger cbProject_RenameBuildTarget(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            if (sa.GetType(2) == OT_INTEGER)
            {
                sa.PushValue<bool>(prj->RenameBuildTarget(sa.GetValue<SQInteger>(2), *sa.GetInstance<wxString>(3)));
                return SC_RETURN_VALUE;
            }
            else
            {
                sa.PushValue<bool>(prj->RenameBuildTarget(*sa.GetInstance<wxString>(2), *sa.GetInstance<wxString>(3)));
                return SC_RETURN_VALUE;
            }

        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::RenameBuildTarget\""));
    }

    SQInteger cbProject_DuplicateBuildTarget(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 3)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            ProjectBuildTarget* bt = nullptr;
            if (sa.GetType(2) == OT_INTEGER)
            {
                bt = prj->DuplicateBuildTarget(sa.GetValue<SQInteger>(2), *sa.GetInstance<wxString>(3));
            }
            else
            {
                bt = prj->DuplicateBuildTarget(*sa.GetInstance<wxString>(2), *sa.GetInstance<wxString>(3));
            }
            sa.PushValue<ProjectBuildTarget*>(bt);
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::DuplicateBuildTarget\""));
    }


    SQInteger cbProject_RemoveBuildTarget(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            if (sa.GetType(2) == OT_INTEGER)
            {
                sa.PushValue<bool>(prj->RemoveBuildTarget(sa.GetValue<SQInteger>(2)));
            }
            else
            {
                sa.PushValue<bool>(prj->RemoveBuildTarget(*sa.GetInstance<wxString>(2)));
            }
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::RemoveBuildTarget\""));
    }

    SQInteger cbProject_ExportTargetAsProject(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbProject* prj = sa.GetInstance<cbProject>(1);
            if (sa.GetType(2) == OT_INTEGER)
            {
                sa.PushValue<bool>(prj->ExportTargetAsProject(sa.GetValue<SQInteger>(2)));
            }
            else
            {
                sa.PushValue<bool>(prj->ExportTargetAsProject(*sa.GetInstance<wxString>(2)));
            }
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbProject::ExportTargetAsProject\""));
    }

    SQInteger ProjectManager_AddFileToProject(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 4)
        {
            if (sa.GetType(4) == OT_INTEGER)
            {
                wxString fname = *sa.GetInstance<wxString>(2);
                cbProject* prj = sa.GetInstance<cbProject>(3);
                int idx = sa.GetValue<SQInteger>(4);
                sa.PushValue<SQInteger>(Manager::Get()->GetProjectManager()->AddFileToProject(fname, prj, idx));
                return SC_RETURN_VALUE;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"ProjectManager::AddFileToProject\""));
    }

    SQInteger ProjectManager_GetProjectCount(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int count = sa.GetParamCount();
        if (count != 1)
            return sa.ThrowError(_("Invalid arguments to \"ProjectManager::GetProjectCount\""));
        else
        {
            ProjectManager *manager = sa.GetInstance<ProjectManager>(1);
            int project_count = manager->GetProjects()->GetCount();
            sa.PushValue<SQInteger>(project_count);
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger ProjectManager_GetProject(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int count = sa.GetParamCount();
        if (count != 2)
            return sa.ThrowError(_("Invalid arguments to \"ProjectManager::GetProject\""));
        else
        {
            ProjectManager *manager = sa.GetInstance<ProjectManager>(1);
            int index = sa.GetValue<SQInteger>(2);
            int project_count = manager->GetProjects()->GetCount();
            if(index >= project_count)
                return sa.ThrowError(_("Index out of bounds in \"ProjectManager::GetProject\""));
            else
            {
                cbProject *project = (*manager->GetProjects())[index];
                sa.PushValue<cbProject*>(project);
                return SC_RETURN_VALUE;
            }
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }
    SQInteger ProjectManager_RebuildTree(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 1)
        {
            ProjectManager *manager = sa.GetInstance<ProjectManager>(1);
            if (manager)
            {
                manager->GetUI().RebuildTree();
                return SC_RETURN_OK;
            }
            return sa.ThrowError(_("'this' is NULL!?! (type of ProjectManager*)"));
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"ProjectManager::RebuildTree\""));
    }

    SQInteger cbEditor_SetText(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            cbEditor* self = sa.GetInstance<cbEditor>(1);
            if (self)
            {
                self->GetControl()->SetText(*sa.GetInstance<wxString>(2));
                return SC_RETURN_OK;
            }
            return sa.ThrowError(_("'this' is NULL!?! (type of cbEditor*)"));
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbEditor::SetText\""));
    }

    SQInteger cbEditor_GetText(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 1)
        {
            cbEditor* self = sa.GetInstance<cbEditor>(1);
            if (self)
            {
                wxString str = self->GetControl()->GetText();
                sa.PushInstanceCopy<wxString>(str);
                return SC_RETURN_VALUE;
            }
            return sa.ThrowError(_("'this' is NULL!?! (type of cbEditor*)"));
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"cbEditor::GetText\""));
    }

    SQInteger CompilerFactory_GetCompilerIndex(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
        int paramCount = sa.GetParamCount();
        if (paramCount == 2)
        {
            sa.PushValue<SQInteger>(CompilerFactory::GetCompilerIndex(*sa.GetInstance<wxString>(2)));
            return SC_RETURN_VALUE;
        }
        }
        catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
        return sa.ThrowError(_("Invalid arguments to \"CompilerFactory::GetCompilerIndex\""));
    }

    wxString CompilerFactory_GetCompilerIDByName(const wxString &name)
    {
        Compiler *compiler = CompilerFactory::GetCompilerByName(name);
        return (compiler ? compiler->GetID() : wxString(wxEmptyString));
    }


    /** \defgroup sq_cb_global Code::Blocks binding to internal classes and functions
     *  \ingroup Squirrel
     *  \brief Code::Blocks binding to internal classes and functions
     */
    void RegisterBindings(HSQUIRRELVM vm)
    {
        if (!vm)
            cbThrow(_T("Scripting engine not initialized!?"));

        Register_wxGlobals(vm);
        Register_wxTypes(vm);
        Register_Constants(vm);
        Register_Globals(vm);
        Register_IO(vm); // IO is enabled, but just for harmless functions
        Register_Dialog(vm);
        Register_ProgressDialog(vm);
        Register_UtilDialogs(vm);


        Sqrat::Class<ConfigManager> config_manager(vm,"ConfigManager");
                config_manager.
                SquirrelFunc("Read",    &ConfigManager_Read).
                SquirrelFunc("Write",   &ConfigManager_Write);

        Sqrat::RootTable(vm).Bind("ConfigManager",config_manager);

        Sqrat::Class<ProjectFile> project_file(vm,"ProjectFile");
                project_file.
                Func("AddBuildTarget",      &ProjectFile::AddBuildTarget).
                Func("RenameBuildTarget",   &ProjectFile::RenameBuildTarget).
                Func("RemoveBuildTarget",   &ProjectFile::RemoveBuildTarget).
                Func("GetbuildTargets",    &ProjectFile::GetBuildTargets).
                Func("GetBaseName",         &ProjectFile::GetBaseName).
                Func("GetObjName",          &ProjectFile::GetObjName).
                Func("SetObjName",          &ProjectFile::SetObjName).
                Func("GetParentProject",    &ProjectFile::GetParentProject).
                Func("SetUseCustomBuildCommand",    &ProjectFile::SetUseCustomBuildCommand).
                Func("SetCustomBuildCommand",       &ProjectFile::SetCustomBuildCommand).
                Func("GetUseCustomBuildCommand",    &ProjectFile::GetUseCustomBuildCommand).
                Func("GetCustomBuildCommand",       &ProjectFile::GetCustomBuildCommand).
                Var("file",                 &ProjectFile::file).
                Var("relativeFilename",     &ProjectFile::relativeFilename).
                Var("relativeToCommonTopLevelPath", &ProjectFile::relativeToCommonTopLevelPath).
                Var("compile",              &ProjectFile::compile).
                Var("link",                 &ProjectFile::link).
                Var("weight",               &ProjectFile::weight).
                Var("compilerVar",          &ProjectFile::compilerVar).
                Var("buildTargets",         &ProjectFile::buildTargets);

        Sqrat::RootTable(vm).Bind("ProjectFile",project_file);

        Sqrat::Class<CompileOptionsBase> compiler_options_base(vm,"CompileOptionsBase");
        compiler_options_base.
                Func("AddPlatform",     &CompileOptionsBase::AddPlatform).
                Func("RemovePlatform",  &CompileOptionsBase::RemovePlatform).
                Func("SetPlatforms",    &CompileOptionsBase::SetPlatforms).
                Func("GetPlatforms",    &CompileOptionsBase::GetPlatforms).
                Func("SupportsCurrentPlatform", &CompileOptionsBase::SupportsCurrentPlatform).
                Func("SetLinkerOptions",&CompileOptionsBase::SetLinkerOptions).
                Func("SetLinkLibs",     &CompileOptionsBase::SetLinkLibs).
                Func("SetCompilerOptions",      &CompileOptionsBase::SetCompilerOptions).
                Func("SetResourceCompilerOptions",      &CompileOptionsBase::SetResourceCompilerOptions).
                Func("SetIncludeDirs",  &CompileOptionsBase::SetIncludeDirs).
                Func("SetResourceIncludeDirs",  &CompileOptionsBase::SetResourceIncludeDirs).
                Func("SetLibDirs",      &CompileOptionsBase::SetLibDirs).
                Func("SetCommandsBeforeBuild",  &CompileOptionsBase::SetCommandsBeforeBuild).
                Func("SetCommandsAfterBuild",   &CompileOptionsBase::SetCommandsAfterBuild).
                Func("GetLinkerOptions",        &CompileOptionsBase::GetLinkerOptions).
                Func("GetLinkLibs",     &CompileOptionsBase::GetLinkLibs).
                Func("GetCompilerOptions",      &CompileOptionsBase::GetCompilerOptions).
                Func("GetResourceCompilerOptions",      &CompileOptionsBase::GetResourceCompilerOptions).
                Func("GetIncludeDirs",  &CompileOptionsBase::GetIncludeDirs).
                Func("GetResourceIncludeDirs",  &CompileOptionsBase::GetResourceIncludeDirs).
                Func("GetLibDirs",      &CompileOptionsBase::GetLibDirs).
                Func("GetCommandsBeforeBuild",  &CompileOptionsBase::GetCommandsBeforeBuild).
                Func("GetCommandsAfterBuild",   &CompileOptionsBase::GetCommandsAfterBuild).
                Func("GetModified",     &CompileOptionsBase::GetModified).
                Func("SetModified",     &CompileOptionsBase::SetModified).
                Func("AddLinkerOption", &CompileOptionsBase::AddLinkerOption).
                Func("AddLinkLib",      &CompileOptionsBase::AddLinkLib).
                Func("AddCompilerOption",       &CompileOptionsBase::AddCompilerOption).
                Func("AddResourceCompilerOption",       &CompileOptionsBase::AddResourceCompilerOption).
                Func("AddIncludeDir",   &CompileOptionsBase::AddIncludeDir).
                Func("AddResourceIncludeDir",   &CompileOptionsBase::AddResourceIncludeDir).
                Func("AddLibDir",       &CompileOptionsBase::AddLibDir).
                Func("AddCommandsBeforeBuild",  &CompileOptionsBase::AddCommandsBeforeBuild).
                Func("AddCommandsAfterBuild",   &CompileOptionsBase::AddCommandsAfterBuild).
                Func("ReplaceLinkerOption",   &CompileOptionsBase::ReplaceLinkerOption).
                Func("ReplaceLinkLib",   &CompileOptionsBase::ReplaceLinkLib).
                Func("ReplaceCompilerOption",   &CompileOptionsBase::ReplaceCompilerOption).
                Func("ReplaceResourceCompilerOption",   &CompileOptionsBase::ReplaceResourceCompilerOption).
                Func("ReplaceIncludeDir",   &CompileOptionsBase::ReplaceIncludeDir).
                Func("ReplaceResourceIncludeDir",   &CompileOptionsBase::ReplaceResourceIncludeDir).
                Func("ReplaceLibDir",   &CompileOptionsBase::ReplaceLibDir).
                Func("RemoveLinkerOption",      &CompileOptionsBase::RemoveLinkerOption).
                Func("RemoveLinkLib",           &CompileOptionsBase::RemoveLinkLib).
                Func("RemoveCompilerOption",    &CompileOptionsBase::RemoveCompilerOption).
                Func("RemoveIncludeDir",        &CompileOptionsBase::RemoveIncludeDir).
                Func("RemoveResourceCompilerOption",        &CompileOptionsBase::RemoveResourceCompilerOption).
                Func("RemoveResourceIncludeDir",&CompileOptionsBase::RemoveResourceIncludeDir).
                Func("RemoveLibDir",    &CompileOptionsBase::RemoveLibDir).
                Func("RemoveCommandsBeforeBuild",  &CompileOptionsBase::RemoveCommandsBeforeBuild).
                Func("RemoveCommandsAfterBuild",   &CompileOptionsBase::RemoveCommandsAfterBuild).
                Func("GetAlwaysRunPostBuildSteps", &CompileOptionsBase::GetAlwaysRunPostBuildSteps).
                Func("SetAlwaysRunPostBuildSteps", &CompileOptionsBase::SetAlwaysRunPostBuildSteps).
                Func("SetBuildScripts", &CompileOptionsBase::SetBuildScripts).
                Func("GetBuildScripts", &CompileOptionsBase::GetBuildScripts).
                Func("AddBuildScript",  &CompileOptionsBase::AddBuildScript).
                Func("RemoveBuildScript",       &CompileOptionsBase::RemoveBuildScript).
                Func("SetVar",          &CompileOptionsBase::SetVar).
                Func("GetVar",          &CompileOptionsBase::GetVar).
                Func("UnsetVar",        &CompileOptionsBase::UnsetVar).
                Func("UnsetAllVars",    &CompileOptionsBase::UnsetAllVars);

        Sqrat::RootTable(vm).Bind("CompileOptionsBase",compiler_options_base);

        Sqrat::DerivedClass<CompileTargetBase,CompileOptionsBase> compiler_target_base(vm,"CompileTargetBase");
        compiler_target_base.
                Func("SetTargetFilenameGenerationPolicy",&CompileTargetBase::SetTargetFilenameGenerationPolicy ).
//                Func(&CompileTargetBase::GetTargetFilenameGenerationPolicy "GetTargetFilenameGenerationPolicy"). // not exposed because its args are non-const references
                Func("GetFilename",     &CompileTargetBase::GetFilename ).
                Func("GetTitle",        &CompileTargetBase::GetTitle ).
                Func("SetTitle",        &CompileTargetBase::SetTitle ).
                Func("SetOutputFilename",   &CompileTargetBase::SetOutputFilename ).
                Func("SetWorkingDir",       &CompileTargetBase::SetWorkingDir ).
                Func("SetObjectOutput",     &CompileTargetBase::SetObjectOutput ).
                Func("SetDepsOutput",       &CompileTargetBase::SetDepsOutput ).
                Func("GetOptionRelation",   &CompileTargetBase::GetOptionRelation ).
                Func("SetOptionRelation",   &CompileTargetBase::SetOptionRelation ).
                Func("GetWorkingDir",       &CompileTargetBase::GetWorkingDir ).
                Func("GetObjectOutput",     &CompileTargetBase::GetObjectOutput ).
                Func("GetDepsOutput",       &CompileTargetBase::GetDepsOutput ).
                Func("GetOutputFilename",   &CompileTargetBase::GetOutputFilename ).
                Func("SuggestOutputFilename",   &CompileTargetBase::SuggestOutputFilename ).
                Func("GetExecutableFilename",   &CompileTargetBase::GetExecutableFilename ).
                Func("GetDynamicLibFilename",   &CompileTargetBase::GetDynamicLibFilename ).
                Func("GetDynamicLibDefFilename",&CompileTargetBase::GetDynamicLibDefFilename ).
                Func("GetStaticLibFilename",    &CompileTargetBase::GetStaticLibFilename ).
                Func("GetBasePath",         &CompileTargetBase::GetBasePath ).
                Func("SetTargetType",       &CompileTargetBase::SetTargetType ).
                Func("GetTargetType",       &CompileTargetBase::GetTargetType ).
                Func("GetExecutionParameters",  &CompileTargetBase::GetExecutionParameters ).
                Func("SetExecutionParameters",  &CompileTargetBase::SetExecutionParameters ).
                Func("GetHostApplication",  &CompileTargetBase::GetHostApplication ).
                Func("SetHostApplication",  &CompileTargetBase::SetHostApplication ).
                Func("SetCompilerID",       &CompileTargetBase::SetCompilerID ).
                Func("GetCompilerID",       &CompileTargetBase::GetCompilerID ).
                Func("GetMakeCommandFor",   &CompileTargetBase::GetMakeCommandFor ).
                Func("SetMakeCommandFor",   &CompileTargetBase::SetMakeCommandFor ).
                Func("MakeCommandsModified",&CompileTargetBase::MakeCommandsModified );

        Sqrat::RootTable(vm).Bind("CompileTargetBase",compiler_target_base);


        Sqrat::DerivedClass<ProjectBuildTarget,CompileTargetBase> project_build_target(vm,"ProjectBuildTarget");
        project_build_target.
                Func("GetParentProject",    &ProjectBuildTarget::GetParentProject).
                Func("GetFullTitle",        &ProjectBuildTarget::GetFullTitle).
                Func("GetExternalDeps",     &ProjectBuildTarget::GetExternalDeps).
                Func("SetExternalDeps",     &ProjectBuildTarget::SetExternalDeps).
                Func("SetAdditionalOutputFiles",    &ProjectBuildTarget::SetAdditionalOutputFiles).
                Func("GetAdditionalOutputFiles",    &ProjectBuildTarget::GetAdditionalOutputFiles).
                Func("GetIncludeInTargetAll",       &ProjectBuildTarget::GetIncludeInTargetAll).
                Func("SetIncludeInTargetAll",       &ProjectBuildTarget::SetIncludeInTargetAll).
                Func("GetCreateDefFile",    &ProjectBuildTarget::GetCreateDefFile).
                Func("SetCreateDefFile",    &ProjectBuildTarget::SetCreateDefFile).
                Func("GetCreateStaticLib",  &ProjectBuildTarget::GetCreateStaticLib).
                Func("SetCreateStaticLib",  &ProjectBuildTarget::SetCreateStaticLib).
                Func("GetUseConsoleRunner", &ProjectBuildTarget::GetUseConsoleRunner).
                Func("SetUseConsoleRunner", &ProjectBuildTarget::SetUseConsoleRunner).
                Func("GetFilesCount",       &ProjectBuildTarget::GetFilesCount).
                Func("GetFile",             &ProjectBuildTarget::GetFile);

        Sqrat::RootTable(vm).Bind("ProjectBuildTarget",project_build_target);

        Sqrat::DerivedClass<cbProject,CompileTargetBase> cb_project(vm,"cbProject");
        cb_project.
                Func("GetModified",     &cbProject::GetModified).
                Func("SetModified",     &cbProject::SetModified).
                Func("GetMakefile",     &cbProject::GetMakefile).
                Func("SetMakefile",     &cbProject::SetMakefile).
                Func("IsMakefileCustom",&cbProject::IsMakefileCustom).
                Func("SetMakefileCustom",&cbProject::SetMakefileCustom).
                Func("CloseAllFiles",   &cbProject::CloseAllFiles).
                Func("SaveAllFiles",    &cbProject::SaveAllFiles).
                Func("Save",            &cbProject::Save).
//                Func(&cbProject::SaveAs"SaveAs"). // *UNSAFE*
                Func("SaveLayout",      &cbProject::SaveLayout).
                Func("LoadLayout",      &cbProject::LoadLayout).
//                Func(&cbProject::ShowOptions"ShowOptions").
                Func("GetCommonTopLevelPath",&cbProject::GetCommonTopLevelPath).
                Func("GetFilesCount",   &cbProject::GetFilesCount).
                Func("GetFile",         &cbProject::GetFile).
                Func("GetFileByFilename",   &cbProject::GetFileByFilename).
                SquirrelFunc("RemoveFile",  &cbProject_RemoveFile).
                SquirrelFunc("AddFile",     &cbProject_AddFile).
                Func("GetBuildTargetsCount",&cbProject::GetBuildTargetsCount).
                SquirrelFunc("GetBuildTarget",  &cbProject_GetBuildTarget).
                Func("AddBuildTarget",          &cbProject::AddBuildTarget).
                SquirrelFunc("RenameBuildTarget",&cbProject_RenameBuildTarget).
                SquirrelFunc("DuplicateBuildTarget",&cbProject_DuplicateBuildTarget).
                SquirrelFunc("RemoveBuildTarget",&cbProject_RemoveBuildTarget).
                SquirrelFunc("ExportTargetAsProject",&cbProject_ExportTargetAsProject).
                Func("BuildTargetValid",        &cbProject::BuildTargetValid).
                Func("GetFirstValidBuildTargetName",&cbProject::GetFirstValidBuildTargetName).
                Func("SetDefaultExecuteTarget", &cbProject::SetDefaultExecuteTarget).
                Func("GetDefaultExecuteTarget", &cbProject::GetDefaultExecuteTarget).
                Func("SetActiveBuildTarget",    &cbProject::SetActiveBuildTarget).
                Func("GetActiveBuildTarget",    &cbProject::GetActiveBuildTarget).
                Func("SelectTarget",            &cbProject::SelectTarget).
                Func("GetCurrentlyCompilingTarget", &cbProject::GetCurrentlyCompilingTarget).
                Func("SetCurrentlyCompilingTarget", &cbProject::SetCurrentlyCompilingTarget).
                Func("GetModeForPCH",               &cbProject::GetModeForPCH).
                Func("SetModeForPCH",               &cbProject::SetModeForPCH).
                Func("SetExtendedObjectNamesGeneration",&cbProject::SetExtendedObjectNamesGeneration).
                Func("GetExtendedObjectNamesGeneration",&cbProject::GetExtendedObjectNamesGeneration).
                Func("SetNotes",            &cbProject::SetNotes).
                Func("GetNotes",            &cbProject::GetNotes).
                Func("SetShowNotesOnLoad",  &cbProject::SetShowNotesOnLoad).
                Func("GetShowNotesOnLoad",  &cbProject::GetShowNotesOnLoad).
                Func("SetCheckForExternallyModifiedFiles",  &cbProject::SetCheckForExternallyModifiedFiles).
                Func("GetCheckForExternallyModifiedFiles",  &cbProject::GetCheckForExternallyModifiedFiles).
                Func("ShowNotes",           &cbProject::ShowNotes).
                Func("AddToExtensions",     &cbProject::AddToExtensions).
                Func("DefineVirtualBuildTarget",    &cbProject::DefineVirtualBuildTarget).
                Func("HasVirtualBuildTarget",       &cbProject::HasVirtualBuildTarget).
                Func("RemoveVirtualBuildTarget",    &cbProject::RemoveVirtualBuildTarget).
                Func("GetVirtualBuildTargets",      &cbProject::GetVirtualBuildTargets).
                Func("GetVirtualBuildTargetGroup",  &cbProject::GetVirtualBuildTargetGroup).
                Func("GetExpandedVirtualBuildTargetGroup",&cbProject::GetExpandedVirtualBuildTargetGroup).
                Func("CanAddToVirtualBuildTarget",  &cbProject::CanAddToVirtualBuildTarget).
                Func("SetTitle",            &cbProject::SetTitle);

        Sqrat::RootTable(vm).Bind("cbProject",cb_project);



        Sqrat::Class<ProjectManager, Sqrat::NoConstructor<ProjectManager> > project_manager(vm,"ProjectManager");
        project_manager.
                Func("GetDefaultPath",      &ProjectManager::GetDefaultPath).
                Func("SetDefaultPath",      &ProjectManager::SetDefaultPath).
                Func("GetActiveProject",    &ProjectManager::GetActiveProject).
                SquirrelFunc("GetProjectCount",&ProjectManager_GetProjectCount).
                SquirrelFunc("GetProject",  &ProjectManager_GetProject).
                Func("SetProject",          &ProjectManager::SetProject).
                Func("LoadWorkspace",       &ProjectManager::LoadWorkspace).
                Func("SaveWorkspace",       &ProjectManager::SaveWorkspace).
                Func("SaveWorkspaceAs",     &ProjectManager::SaveWorkspaceAs).
                Func("CloseWorkspace",      &ProjectManager::CloseWorkspace).
                Func("IsOpen",              &ProjectManager::IsOpen).
                Func("LoadProject",         &ProjectManager::LoadProject).
                Func("SaveProject",         &ProjectManager::SaveProject).
                Func("SaveProjectAs",       &ProjectManager::SaveProjectAs).
                Func("SaveActiveProject",   &ProjectManager::SaveActiveProject).
                Func("SaveActiveProjectAs", &ProjectManager::SaveActiveProjectAs).
                Func("SaveAllProjects",     &ProjectManager::SaveAllProjects).
                Func("CloseProject",        &ProjectManager::CloseProject).
                Func("CloseActiveProject",  &ProjectManager::CloseActiveProject).
                Func("CloseAllProjects",    &ProjectManager::CloseAllProjects).
                Func("NewProject",          &ProjectManager::NewProject).
                SquirrelFunc("AddFileToProject",    &ProjectManager_AddFileToProject).
//                Func(&ProjectManager::AskForBuildTargetIndex"AskForBuildTargetIndex").
                Func("AddProjectDependency",        &ProjectManager::AddProjectDependency).
                Func("RemoveProjectDependency",     &ProjectManager::RemoveProjectDependency).
                Func("ClearProjectDependencies",    &ProjectManager::ClearProjectDependencies).
                Func("RemoveProjectFromAllDependencies",&ProjectManager::RemoveProjectFromAllDependencies).
                Func("GetDependenciesForProject",   &ProjectManager::GetDependenciesForProject).
//                Func(&ProjectManager::ConfigureProjectDependencies"ConfigureProjectDependencies");
                SquirrelFunc("RebuildTree",         &ProjectManager_RebuildTree);
        Sqrat::RootTable(vm).Bind("ProjectManager",project_manager);

        Sqrat::Class<EditorBase, Sqrat::NoCopy<EditorBase> > editor_base(vm,"EditorBase");
        editor_base.
                Func("GetFilename",     &EditorBase::GetFilename).
                Func("SetFilename",     &EditorBase::SetFilename).
                Func("GetShortName",    &EditorBase::GetShortName).
                Func("GetModified",     &EditorBase::GetModified).
                Func("SetModified",     &EditorBase::SetModified).
                Func("GetTitle",        &EditorBase::GetTitle).
                Func("SetTitle",        &EditorBase::SetTitle).
                Func("Activate",        &EditorBase::Activate).
                Func("Close",           &EditorBase::Close).
                Func("Save",            &EditorBase::Save).
                Func("IsBuiltinEditor", &EditorBase::IsBuiltinEditor).
                Func("ThereAreOthers",  &EditorBase::ThereAreOthers).
                Func("GotoLine",        &EditorBase::GotoLine).
                //Func("ToggleBreakpoint",&EditorBase::ToggleBreakpoint).
                //Func("HasBreakpoint",   &EditorBase::HasBreakpoint).
                //Func("GotoNextBreakpoint",&EditorBase::GotoNextBreakpoint).
                //Func("GotoPreviousBreakpoint",&EditorBase::GotoPreviousBreakpoint).
                //Func("ToggleBookmark",  &EditorBase::ToggleBookmark).
                //Func("HasBookmark",     &EditorBase::HasBookmark).
                //Func("GotoNextBookmark",&EditorBase::GotoNextBookmark).
                //Func("GotoPreviousBookmark",&EditorBase::GotoPreviousBookmark).
                Func("Undo",        &EditorBase::Undo).
                Func("Redo",        &EditorBase::Redo).
                Func("Cut",         &EditorBase::Cut).
                Func("Copy",        &EditorBase::Copy).
                Func("Paste",       &EditorBase::Paste).
                Func("CanUndo",     &EditorBase::CanUndo).
                Func("CanRedo",     &EditorBase::CanRedo).
                Func("CanPaste",    &EditorBase::CanPaste).
                Func("IsReadOnly",  &EditorBase::IsReadOnly).
                Func("HasSelection",&EditorBase::HasSelection);
        Sqrat::RootTable(vm).Bind("EditorBase",editor_base);

        Sqrat::DerivedClass<cbEditor,EditorBase, Sqrat::NoConstructor<cbEditor> > cb_editor(vm,"cbEditor");
        cb_editor.
                Func("SetEditorTitle",  &cbEditor::SetEditorTitle).
                Func("GetProjectFile",  &cbEditor::GetProjectFile).
                Func("Save",            &cbEditor::Save).
                Func("SaveAs",          &cbEditor::SaveAs).
                Func("FoldAll",         &cbEditor::FoldAll).
                Func("UnfoldAll",       &cbEditor::UnfoldAll).
                Func("ToggleAllFolds",  &cbEditor::ToggleAllFolds).
                Func("FoldBlockFromLine",   &cbEditor::FoldBlockFromLine).
                Func("UnfoldBlockFromLine", &cbEditor::UnfoldBlockFromLine).
                Func("ToggleFoldBlockFromLine",&cbEditor::ToggleFoldBlockFromLine).
                Func("GetLineIndentInSpaces",&cbEditor::GetLineIndentInSpaces).
                Func("GetLineIndentString", &cbEditor::GetLineIndentString).
                Func("Touch",           &cbEditor::Touch).
                Func("Reload",          &cbEditor::Reload).
                Func("Print",           &cbEditor::Print).
                Func("AutoComplete",    &cbEditor::AutoComplete).
                Func("AddBreakpoint",   &cbEditor::AddBreakpoint).
                Func("RemoveBreakpoint",&cbEditor::RemoveBreakpoint).
                // these are not present in cbEditor; included to help scripts edit text
                SquirrelFunc("SetText", &cbEditor_SetText).
                SquirrelFunc("GetText", &cbEditor_GetText);
        Sqrat::RootTable(vm).Bind("cbEditor",cb_editor);

        Sqrat::Class<EditorManager, Sqrat::NoConstructor<EditorManager> > editor_manager(vm,"EditorManager");
        editor_manager.
                Func("New",             &EditorManager::New).
                SquirrelFunc("Open",    &EditorManager_Open).
                Func("IsBuiltinOpen",   &EditorManager::IsBuiltinOpen).
                SquirrelFunc("GetBuiltinEditor",&EditorManager_GetBuiltinEditor).
                Func("GetBuiltinActiveEditor",&EditorManager::GetBuiltinActiveEditor).
                Func("GetActiveEditor", &EditorManager::GetActiveEditor).
                Func("ActivateNext",    &EditorManager::ActivateNext).
                Func("ActivatePrevious",&EditorManager::ActivatePrevious).
                Func("SwapActiveHeaderSource",&EditorManager::SwapActiveHeaderSource).
                Func("CloseActive",     &EditorManager::CloseActive).
                SquirrelFunc("Close",   &EditorManager_Close).
                Func("CloseAll",        &EditorManager::CloseAll).
                SquirrelFunc("Save",    &EditorManager_Save).
                Func("SaveActive",      &EditorManager::SaveActive).
                Func("SaveAs",          &EditorManager::SaveAs).
                Func("SaveActiveAs",    &EditorManager::SaveActiveAs).
                Func("SaveAll",         &EditorManager::SaveAll);
        //        Func(&EditorManager::ShowFindDialog"ShowFindDialog");
        Sqrat::RootTable(vm).Bind("EditorManager",editor_manager);


        Sqrat::Class<UserVariableManager, Sqrat::NoCopy<UserVariableManager> > user_variable_manager(vm,"UserVariableManager");
        user_variable_manager.
                Func("Exists",  &UserVariableManager::Exists);
        Sqrat::RootTable(vm).Bind("UserVariableManager",user_variable_manager);


        /** \brief ScriptingManager class binding.
         *  \ingroup sq_cb_global
         *
         *  ### ScriptingManager Function bound to squirrel
         *   | Name                     | parameter                                                     | description               | info  |
         *   | :-----------------------:| :-----------------------------------------------------------: | :-----------------------: | :----:|
         *   | RegisterScriptMenu       | [string] menu_path, [string] script_or_func,[bool] isFunc     | Register a menu entry     |   x   |
         *   | DisplayErrors            | [string] message, [bool] del_errors                           | Display scripting errors. Empty string if the cache should be called    |   x   |
         */

        Sqrat::Class<ScriptingManager, Sqrat::NoConstructor<ScriptingManager> > scripting_manager(vm,"ScriptingManager");
        scripting_manager.
                Func("RegisterScriptMenu",  &ScriptingManager::RegisterScriptMenu).
                Func("DisplayErrors",  &ScriptingManager::DisplayErrors);
        Sqrat::RootTable(vm).Bind("ScriptingManager",scripting_manager);

        typedef bool(*CF_INHERITSFROM)(const wxString&, const wxString&); // CompilerInheritsFrom

		// NOTE this is a remaining of the rebase process.
		// It can be removed if no problems were reported
        //Sqrat::RootTable(vm).Func("GetCompilerIDByName",CompilerFactory_GetCompilerIDByName);

        Sqrat::Class<CompilerFactory> compiler_factory(vm,"CompilerFactory");
        compiler_factory.
                StaticFunc("IsValidCompilerID",         &CompilerFactory::IsValidCompilerID).
                // FIXME (bluehazzard#1#): This should work, but i'm not sure...
                SquirrelFunc("GetCompilerIndex",        &CompilerFactory_GetCompilerIndex).
                StaticFunc("GetDefaultCompilerID",      &CompilerFactory::GetDefaultCompilerID).
                StaticFunc("GetCompilerVersionString",  &CompilerFactory::GetCompilerVersionString).
                StaticFunc<CF_INHERITSFROM>("CompilerInheritsFrom",&CompilerFactory::CompilerInheritsFrom).
                StaticFunc("GetCompilerIDByName",CompilerFactory_GetCompilerIDByName);
        Sqrat::RootTable(vm).Bind("CompilerFactory",compiler_factory);

        Sqrat::Class<PluginInfo> plugin_info(vm,"PluginInfo");
        plugin_info.
            //emptyCtor().
            Var("name",         &PluginInfo::name).
            Var("title",        &PluginInfo::title).
            Var("version",      &PluginInfo::version).
            Var("description",  &PluginInfo::description).
            Var("author",       &PluginInfo::author).
            Var("authorEmail",  &PluginInfo::authorEmail).
            Var("authorWebsite",&PluginInfo::authorWebsite).
            Var("thanksTo",     &PluginInfo::thanksTo).
            Var("license",      &PluginInfo::license);
        Sqrat::RootTable(vm).Bind("PluginInfo",plugin_info);


        Sqrat::Class<FileTreeData> file_tree_data(vm,"FileTreeData");
        file_tree_data.
            Func("GetKind",         &FileTreeData::GetKind).
            Func("GetProject",      &FileTreeData::GetProject).
            Func("GetFileIndex",    &FileTreeData::GetFileIndex).
            Func("GetProjectFile",  &FileTreeData::GetProjectFile).
            Func("GetFolder",       &FileTreeData::GetFolder).
            Func("SetKind",         &FileTreeData::SetKind).
            Func("SetProject",      &FileTreeData::SetProject).
            Func("SetFileIndex",    &FileTreeData::SetFileIndex).
            Func("SetProjectFile",  &FileTreeData::SetProjectFile).
            Func("SetFolder",       &FileTreeData::SetFolder);
        Sqrat::RootTable(vm).Bind("FileTreeData",file_tree_data);

/** \brief CodeBlocksEvent class binding.
 *  \ingroup sq_cb_global
 *
 * Function bound to squirrel:
 *
 *  ### CodeBlocksEvent Function bound to squirrel
 *   | Name                     | parameter             | description               | info  |
 *   | :-----------------------:| :-------------------: | :-----------------------: | :----:|
 *   | CodeBlocksEvent()        | &CodeBlocksEvent      |  Copy only constructor    |   x   |
 *   | GetEventType             |   x                   |    x                      |   x   |
 *   | GetProject               |   x                   |    x                      |   x   |
 *   | SetProject               |   x                   |    x                      |   x   |
 *   | GetEditor                |   x                   |    x                      |   x   |
 *   | SetEditor                |   x                   |    x                      |   x   |
 *   | GetOldEditor             |   x                   |    x                      |   x   |
 *   | SetOldEditor             |   x                   |    x                      |   x   |
 *   | GetX                     |   x                   |    x                      |   x   |
 *   | SetX                     |   x                   |    x                      |   x   |
 *   | GetY                     |   x                   |    x                      |   x   |
 *   | SetY                     |   x                   |    x                      |   x   |
 *   | GetBuildTargetName       |   x                   |    x                      |   x   |
 *   | GetOldBuildTargetName    |   x                   |    x                      |   x   |
 *   | SetOldBuildTargetName    |   x                   |    x                      |   x   |

 */

        Sqrat::Class<CodeBlocksEvent, Sqrat::CopyOnly<CodeBlocksEvent> > cb_event(vm,"CodeBlocksEvent");
        cb_event.
            Func("GetEventType",   &CodeBlocksEvent::GetEventType).
            Func("GetProject",     &CodeBlocksEvent::GetProject).
            Func("SetProject",     &CodeBlocksEvent::SetProject).
            Func("GetEditor",      &CodeBlocksEvent::GetEditor).
            Func("SetEditor",      &CodeBlocksEvent::SetEditor).
            Func("GetOldEditor",   &CodeBlocksEvent::GetOldEditor).
            Func("SetOldEditor",   &CodeBlocksEvent::SetOldEditor).
            //Func("GetPlugin",    &CodeBlocksEvent::GetPlugin).
            //Func("SetPlugin",    &CodeBlocksEvent::SetPlugin).
            Func("GetX",      &CodeBlocksEvent::GetX).
            Func("SetX",      &CodeBlocksEvent::SetX).
            Func("GetY",      &CodeBlocksEvent::GetY).
            Func("SetY",      &CodeBlocksEvent::SetY).
            Func("GetBuildTargetName",    &CodeBlocksEvent::GetBuildTargetName).
            Func("GetOldBuildTargetName", &CodeBlocksEvent::GetOldBuildTargetName).
            Func("SetOldBuildTargetName", &CodeBlocksEvent::SetOldBuildTargetName);
        Sqrat::RootTable(vm).Bind("CodeBlocksEvent",cb_event);

        // called last because it needs a few previously registered types
        Register_ScriptPlugin(vm);
    }
} // namespace ScriptBindings
