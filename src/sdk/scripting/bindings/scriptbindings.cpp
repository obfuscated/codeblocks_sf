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
    #include "cbeditor.h"
    #include "cbproject.h"
    #include "compilerfactory.h"
    #include "compiletargetbase.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "projectbuildtarget.h"
    #include "scriptingmanager.h"
    #include "uservarmanager.h"
#endif // CB_PRECOMP

#include "scripting/bindings/sc_utils.h"
#include "scripting/bindings/sc_typeinfo_all.h"

#include "cbstyledtextctrl.h"

namespace ScriptBindings
{
    SQInteger ConfigManager_Read(HSQUIRRELVM v)
    {
        // this, key, default value
        ExtractParams3<SkipParam, const wxString *, SkipParam> extractor(v);
        if (!extractor.Process("ConfigManager::Read"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) We don't use this for some unknown reason.
        ConfigManager *cfg = Manager::Get()->GetConfigManager("scripts");
        if (cfg == nullptr)
            return sq_throwerror(v, _SC("ConfigManager::Read cannot get manager pointer!"));

        const wxString &key = *extractor.p1;
        const SQObjectType type = sq_gettype(v, 3);
        switch (type)
        {
        case OT_INTEGER:
            sq_pushinteger(v, cfg->ReadInt(key, extractor.GetParamInt(3)));
            return 1;
        case OT_BOOL:
            sq_pushbool(v, cfg->ReadBool(key, extractor.GetParamBool(3)));
            return 1;
        case OT_FLOAT:
            sq_pushfloat(v, cfg->ReadDouble(key, extractor.GetParamFloat(3)));
            return 1;
        case OT_INSTANCE:
            {
                const wxString *defaultValue;
                if (!extractor.ProcessParam(defaultValue, 3, "ConfigManager::Read"))
                    return extractor.ErrorMessage();
                const wxString &result = cfg->Read(key, *defaultValue);
                return ConstructAndReturnInstance(v, result);
            }
        default:
            return sq_throwerror(v, _SC("ConfigManager::Read given unsupported type!"));
        }
    }

    SQInteger ConfigManager_Write(HSQUIRRELVM v)
    {
        // FIXME (squirrel) We don't use this for some unknown reason.
        ConfigManager *cfg = Manager::Get()->GetConfigManager("scripts");
        if (cfg == nullptr)
            return sq_throwerror(v, _SC("ConfigManager::Write cannot get manager pointer!"));

        const int numArgs = sq_gettop(v);
        if (numArgs == 4)
        {
            // this, key, value, ignoreEmpty
            ExtractParams4<SkipParam, const wxString*, const wxString*, bool> extractor(v);
            if (!extractor.Process("ConfigManager::Write"))
                return extractor.ErrorMessage();
            cfg->Write(*extractor.p1, *extractor.p2, extractor.p3);
            return 0;
        }
        else
        {
            // this, key, value
            ExtractParams3<SkipParam, const wxString *, SkipParam> extractor(v);
            if (!extractor.Process("ConfigManager::Write"))
                return extractor.ErrorMessage();

            const wxString &key = *extractor.p1;
            const SQObjectType type = sq_gettype(v, 3);
            switch (type)
            {
            case OT_INTEGER:
                cfg->Write(key, int(extractor.GetParamInt(3)));
                return 0;
            case OT_BOOL:
                cfg->Write(key, extractor.GetParamBool(3));
                return 0;
            case OT_FLOAT:
                cfg->Write(key, extractor.GetParamFloat(3));
                return 0;
            case OT_INSTANCE:
                {
                    const wxString *value;
                    if (!extractor.ProcessParam(value, 3, "ConfigManager::Read"))
                        return extractor.ErrorMessage();
                    cfg->Write(key, *value);
                    return 0;
                }
            default:
                ;
            }
            return sq_throwerror(v, _SC("ConfigManager::Read given unsupported type!"));
        }
    }

    template<void (ProjectFile::*func)(const wxString &)>
    SQInteger ProjectFile_SingleWxStringParam(HSQUIRRELVM v)
    {
        // this, targetName
        ExtractParams2<ProjectFile*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectFile_SingleWxStringParam"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    SQInteger ProjectFile_RenameBuildTarget(HSQUIRRELVM v)
    {
        // this, oldTargetName, newTargetName
        ExtractParams3<ProjectFile*, const wxString*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectFile::RenameBuildTarget"))
            return extractor.ErrorMessage();
        extractor.p0->RenameBuildTarget(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_GetBuildTargets(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetBuildTargets"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>(extractor.p0->GetBuildTargets());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_GetBaseName(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetBaseName"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetBaseName());
    }

    SQInteger ProjectFile_GetObjName(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetObjName"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxString *result = &const_cast<wxString&>(extractor.p0->GetObjName());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_GetParentProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectFile*> extractor(v);
        if (!extractor.Process("ProjectFile::GetParentProject"))
            return extractor.ErrorMessage();
        cbProject *result = extractor.p0->GetParentProject();
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger ProjectFile_SetUseCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId, useCustomBuildCommand
        ExtractParams3<ProjectFile*, const wxString *, bool> extractor(v);
        if (!extractor.Process("ProjectFile::SetUseCustomBuildCommand"))
            return extractor.ErrorMessage();
        extractor.p0->SetUseCustomBuildCommand(*extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_SetCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId, newBuildCommand
        ExtractParams3<ProjectFile*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::SetCustomBuildCommand"))
            return extractor.ErrorMessage();
        extractor.p0->SetCustomBuildCommand(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger ProjectFile_GetUseCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId
        ExtractParams2<ProjectFile*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::GetUseCustomBuildCommand"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->GetUseCustomBuildCommand(*extractor.p1));
        return 1;
    }

    SQInteger ProjectFile_GetCustomBuildCommand(HSQUIRRELVM v)
    {
        // this, compilerId
        ExtractParams2<ProjectFile*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectFile::GetCustomBuildCommand"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetCustomBuildCommand(*extractor.p1));
    }

    template<void (CompileOptionsBase::*func)(int)>
    SQInteger CompileOptionsBase_Platform(HSQUIRRELVM v)
    {
        // this, platform(s)
        ExtractParams2<CompileOptionsBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileOptionsBase_Platform"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(extractor.p1);
        return 0;
    }

    SQInteger CompileOptionsBase_GetPlatforms(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetPlatforms"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetPlatforms());
        return 1;
    }

    SQInteger CompileOptionsBase_SupportsCurrentPlatform(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SupportsCurrentPlatform"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SupportsCurrentPlatform());
        return 1;
    }

    template<void (CompileOptionsBase::*func)(const wxArrayString&)>
    SQInteger CompileOptionsBase_SetArrayString(HSQUIRRELVM v)
    {
        // this, linkerOpts
        ExtractParams2<CompileOptionsBase*, const wxArrayString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_SetArrayString"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<const wxArrayString& (CompileOptionsBase::*func)() const>
    SQInteger CompileOptionsBase_GetArrayString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase_GetArrayString"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>((extractor.p0->*func)());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    template<void (CompileOptionsBase::*func)(const wxString &)>
    SQInteger CompileOptionsBase_StringParam(HSQUIRRELVM v)
    {
        // this, option
        ExtractParams2<CompileOptionsBase*, const wxString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_AddOption"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<void (CompileOptionsBase::*func)(const wxString&, const wxString&)>
    SQInteger CompileOptionsBase_ReplaceOption(HSQUIRRELVM v)
    {
        // this, option, new_option
        ExtractParams3<CompileOptionsBase*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("CompileOptionsBase_ReplaceOption"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(*extractor.p1, *extractor.p2);
        return 0;
    }

    SQInteger CompileOptionsBase_SetLinkerExecutable(HSQUIRRELVM v)
    {
        // this, option (actual type LinkerExecutableOption)
        ExtractParams2<CompileOptionsBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetLinkerExecutable"))
            return extractor.ErrorMessage();

        if (extractor.p1 < int32_t(LinkerExecutableOption::First) ||
            extractor.p1 >= int32_t(LinkerExecutableOption::Last))
        {
            return sq_throwerror(v, _SC("CompileOptionsBase::SetLinkerExecutable: option value out of range!"));
        }
        extractor.p0->SetLinkerExecutable(LinkerExecutableOption(extractor.p1));
        return 0;
    }

    SQInteger CompileOptionsBase_GetLinkerExecutable(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetLinkerExecutable"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, int32_t(extractor.p0->GetLinkerExecutable()));
        return 1;
    }

    SQInteger CompileOptionsBase_GetAlwaysRunPostBuildSteps(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetAlwaysRunPostBuildSteps"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->GetAlwaysRunPostBuildSteps());
        return 1;
    }

    SQInteger CompileOptionsBase_SetAlwaysRunPostBuildSteps(HSQUIRRELVM v)
    {
        // this, always
        ExtractParams2<CompileOptionsBase*, bool> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetAlwaysRunPostBuildSteps"))
            return extractor.ErrorMessage();
        extractor.p0->SetAlwaysRunPostBuildSteps(extractor.p1);
        return 0;
    }

    SQInteger CompileOptionsBase_SetVar(HSQUIRRELVM v)
    {
        // this, key, value, onlyIfExists
        ExtractParams4<CompileOptionsBase*, const wxString*, const wxString*, bool> extractor(v);
        if (!extractor.Process("CompileOptionsBase::SetVar"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SetVar(*extractor.p1, *extractor.p2, extractor.p3));
        return 1;
    }

    SQInteger CompileOptionsBase_GetVar(HSQUIRRELVM v)
    {
        // this, key
        ExtractParams2<const CompileOptionsBase*, const wxString*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::GetVar"))
            return extractor.ErrorMessage();

        const wxString &result = extractor.p0->GetVar(*extractor.p1);
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        return ConstructAndReturnNonOwnedPtr(v, &const_cast<wxString&>(result));
    }

    SQInteger CompileOptionsBase_UnsetVar(HSQUIRRELVM v)
    {
        // this, key
        ExtractParams2<CompileOptionsBase*, const wxString*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::UnsetVar"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->UnsetVar(*extractor.p1));
        return 1;
    }

    SQInteger CompileOptionsBase_UnsetAllVars(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<CompileOptionsBase*> extractor(v);
        if (!extractor.Process("CompileOptionsBase::UnsetAllVars"))
            return extractor.ErrorMessage();
        extractor.p0->UnsetAllVars();
        return 0;
    }

    SQInteger CompileTargetBase_SetTargetFilenameGenerationPolicy(HSQUIRRELVM v)
    {
        // this, prefix, extension (type is TargetFilenameGenerationPolicy)
        ExtractParams3<CompileTargetBase*, SQInteger, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetTargetFilenameGenerationPolicy"))
            return extractor.ErrorMessage();
        extractor.p0->SetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy(extractor.p1),
                                                        TargetFilenameGenerationPolicy(extractor.p2));
        return 0;
    }

    SQInteger CompileTargetBase_GetTargetFilenameGenerationPolicy(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileTargetBase*> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetTargetFilenameGenerationPolicy"))
            return extractor.ErrorMessage();

        TargetFilenameGenerationPolicy prefixOut, extensionOut;
        extractor.p0->GetTargetFilenameGenerationPolicy(prefixOut, extensionOut);

        // Create a table with two slots "prefix" and "extension". This emulates multiple returns.
        sq_newtableex(v, 2);

        sq_pushstring(v, _SC("prefix"), -1);
        sq_pushinteger(v, int(prefixOut));
        sq_newslot(v, -3, SQFalse);

        sq_pushstring(v, _SC("extension"), -1);
        sq_pushinteger(v, int(extensionOut));
        sq_newslot(v, -3, SQFalse);
        return 1;
    }

    template<typename Type, const wxString& (Type::*func)() const>
    SQInteger Generic_GetCString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetCString"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxString *result = &const_cast<wxString&>((extractor.p0->*func)());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    template<typename Type, void (Type::*func)(const wxString&)>
    SQInteger Generic_SetString(HSQUIRRELVM v)
    {
        // this, string
        ExtractParams2<Type*, const wxString *> extractor(v);
        if (!extractor.Process("Generic_SetString"))
            return extractor.ErrorMessage();

        (extractor.p0->*func)(*extractor.p1);
        return 0;
    }

    template<typename Type, wxString (Type::*func)()>
    SQInteger Generic_GetString(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<Type*> extractor(v);
        if (!extractor.Process("Generic_GetString"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, (extractor.p0->*func)());
    }

    template<typename Type, wxString (Type::*func)() const>
    SQInteger Generic_GetStringConst(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetStringConst"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, (extractor.p0->*func)());
    }

    template<typename Type, bool (Type::*func)() const>
    SQInteger Generic_GetBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const Type*> extractor(v);
        if (!extractor.Process("Generic_GetBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    template<typename Type, bool (Type::*func)()>
    SQInteger Generic_DoSomethingGetBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<Type*> extractor(v);
        if (!extractor.Process("Generic_DoSomethingGetBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    template<typename Type, void (Type::*func)()>
    SQInteger Generic_DoSomethingGetVoid(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<Type*> extractor(v);
        if (!extractor.Process("Generic_DoSomethingGetVoid"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)();
        return 0;
    }

    template<typename Type, void (Type::*func)(bool)>
    SQInteger Generic_SetBool(HSQUIRRELVM v)
    {
        // this, value
        ExtractParams2<Type*, bool> extractor(v);
        if (!extractor.Process("Generic_SetBool"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(extractor.p1);
        return 0;
    }

    SQInteger CompileTargetBase_GetOptionRelation(HSQUIRRELVM v)
    {
        // this, type (OptionsRelationType)
        ExtractParams2<const CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetOptionRelation"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ortCompilerOptions || extractor.p1 >= ortLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetOptionRelation: parameter type out of range!"));

        const OptionsRelation result = extractor.p0->GetOptionRelation(OptionsRelationType(extractor.p1));
        sq_pushinteger(v, int(result));
        return 1;
    }

    SQInteger CompileTargetBase_SetOptionRelation(HSQUIRRELVM v)
    {
        // this, type (OptionsRelationType), rel (OptionsRelation)
        ExtractParams3<CompileTargetBase*, SQInteger, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetOptionRelation"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ortCompilerOptions || extractor.p1 >= ortLast)
            return sq_throwerror(v, _SC("CompileTargetBase::SetOptionRelation: parameter 'type' out of range!"));
        if (extractor.p2 < orUseParentOptionsOnly || extractor.p2 > orAppendToParentOptions)
            return sq_throwerror(v, _SC("CompileTargetBase::SetOptionRelation: parameter 'rel' out of range!"));

        extractor.p0->SetOptionRelation(OptionsRelationType(extractor.p1),
                                        OptionsRelation(extractor.p2));
        return 0;
    }

    SQInteger CompileTargetBase_SetTargetType(HSQUIRRELVM v)
    {
        // this, pt (TargetType)
        ExtractParams2<CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetTargetType"))
            return extractor.ErrorMessage();
        if (extractor.p1 < ttExecutable || extractor.p1 > ttNative)
            return sq_throwerror(v, _SC("CompileTargetBase::SetTargetType: parameter 'pt' out of range!"));

        extractor.p0->SetTargetType(TargetType(extractor.p1));
        return 0;
    }

    SQInteger CompileTargetBase_GetTargetType(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const CompileTargetBase*> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetTargetType"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, int(extractor.p0->GetTargetType()));
        return 1;
    }

    SQInteger CompileTargetBase_GetMakeCommandFor(HSQUIRRELVM v)
    {
        // this, cmd (type MakeCommand)
        ExtractParams2<const CompileTargetBase*, SQInteger> extractor(v);
        if (!extractor.Process("CompileTargetBase::GetMakeCommandFor"))
            return extractor.ErrorMessage();
        if (extractor.p1 < mcClean || extractor.p1 >= mcLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetMakeCommandFor: parameter 'cmd' out of range!"));
        return ConstructAndReturnInstance(v, extractor.p0->GetMakeCommandFor(MakeCommand(extractor.p1)));
    }

    SQInteger CompileTargetBase_SetMakeCommandFor(HSQUIRRELVM v)
    {
        // this, cmd (type MakeCommand), make
        ExtractParams3<CompileTargetBase*, SQInteger, const wxString *> extractor(v);
        if (!extractor.Process("CompileTargetBase::SetMakeCommandFor"))
            return extractor.ErrorMessage();
        if (extractor.p1 < mcClean || extractor.p1 >= mcLast)
            return sq_throwerror(v, _SC("CompileTargetBase::GetMakeCommandFor: parameter 'cmd' out of range!"));
        extractor.p0->SetMakeCommandFor(MakeCommand(extractor.p1), *extractor.p2);
        return 0;
    }

    SQInteger ProjectBuildTarget_GetParentProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectBuildTarget*> extractor(v);
        if (!extractor.Process("ProjectBuildTarget::GetParentProject"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, extractor.p0->GetParentProject());
    }

    template<typename ClassType>
    SQInteger Generic_GetFilesCount(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ClassType*> extractor(v);
        if (!extractor.Process("Generic_GetFilesCount"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetFilesCount());
        return 1;
    }

    template<typename ClassType>
    SQInteger Generic_GetFile(HSQUIRRELVM v)
    {
        // this, index
        ExtractParams2<ClassType*, SQInteger> extractor(v);
        if (!extractor.Process("Generic_GetFile"))
            return extractor.ErrorMessage();
        ProjectFile *file = extractor.p0->GetFile(extractor.p1);
        return ConstructAndReturnNonOwnedPtrOrNull(v, file);
    }

    SQInteger cbProject_CloseAllFiles(HSQUIRRELVM v)
    {
        // this, dontSave
        ExtractParams2<cbProject*, bool> extractor(v);
        if (!extractor.Process("cbProject::CloseAllFiles"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->CloseAllFiles(extractor.p1));
        return 1;
    }

    SQInteger cbProject_GetFileByFilename(HSQUIRRELVM v)
    {
        // this, filename, isRelative, isUnixFilename
        ExtractParams4<cbProject*, const wxString*, bool, bool> extractor(v);
        if (!extractor.Process("cbProject::GetFileByFilename"))
            return extractor.ErrorMessage();
        ProjectFile *file = extractor.p0->GetFileByFilename(*extractor.p1, extractor.p2,
                                                            extractor.p3);
        return ConstructAndReturnNonOwnedPtrOrNull(v, file);
    }

    SQInteger cbProject_RemoveFile(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Add support for pf being nullptr
        // this, pf
        ExtractParams2<cbProject*, ProjectFile *> extractor(v);
        if (!extractor.Process("cbProject::RemoveFile"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->RemoveFile(extractor.p1));
        return 1;
    }

    struct ParamIntOrWxString
    {
        bool Parse(ExtractParamsBase &extractor, int stackIndex, const char *funcStr)
        {
            switch (sq_gettype(extractor.GetVM(), stackIndex))
            {
                case OT_INTEGER:
                    intValue = extractor.GetParamInt(stackIndex);
                    isInt = true;
                    return true;
                case OT_INSTANCE:
                {
                    if (!extractor.ProcessParam(strValue, stackIndex, funcStr))
                    {
                        extractor.ErrorMessage();
                        return false;
                    }
                    isInt = false;
                    return true;
                }
                default:
                    return false;
            }
        }

        bool isInt;
        union {
            int intValue;
            const wxString *strValue;
        };
    };

    SQInteger cbProject_AddFile(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Add support for default parameters in ExtractParamsN
        // this, targetName or targetIndex, filename, compile=true, link=true, weight=50
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(3, 6, "cbProject::AddFile"))
            return extractor.ErrorMessage();

        cbProject *self;
        if (!extractor.ProcessParam(self, 1, "cbProject::AddFile"))
            return extractor.ErrorMessage();
        const wxString *filename;
        if (!extractor.ProcessParam(filename, 3, "cbProject::AddFile"))
            return extractor.ErrorMessage();
        bool compile = true;
        const int numArgs = sq_gettop(v);
        if (numArgs >= 4)
        {
            if (!extractor.ProcessParam(compile, 4, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }
        bool link = true;
        if (numArgs >= 5)
        {
            if (!extractor.ProcessParam(link, 5, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }
        SQInteger weight = 50;
        if (numArgs == 6)
        {
            if (!extractor.ProcessParam(weight, 6, "cbProject::AddFile"))
                return extractor.ErrorMessage();
        }

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::AddFile"))
            return sq_throwerror(v, _SC("cbProject::AddFile: Expects an integer or wxString as first argument!"));

        ProjectFile *result;
        if (targetNameOrIndex.isInt)
            result = self->AddFile(targetNameOrIndex.intValue, *filename, compile, link, weight);
        else
            result = self->AddFile(*targetNameOrIndex.strValue, *filename, compile, link, weight);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger cbProject_GetBuildTargetsCount(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetBuildTargetsCount"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->GetBuildTargetsCount());
        return 1;
    }

    SQInteger cbProject_GetBuildTarget(HSQUIRRELVM v)
    {
        // this, index or targetName
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::GetBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::GetBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::GetBuildTarget: Expects an integer or wxString as first argument!"));

        ProjectBuildTarget *result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->GetBuildTarget(targetNameOrIndex.intValue);
        else
            result = extractor.p0->GetBuildTarget(*targetNameOrIndex.strValue);
        if (result)
        {
            // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
            return ConstructAndReturnNonOwnedPtr(v, result);
        }
        else
        {
            // FIXME (squirrel) Write a test for this.
            sq_pushnull(v);
            return 1;
        }
    }

    SQInteger cbProject_AddBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName
        ExtractParams2<cbProject*, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::AddBuildTarget"))
            return extractor.ErrorMessage();

        ProjectBuildTarget *result = extractor.p0->AddBuildTarget(*extractor.p1);
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger cbProject_RenameBuildTarget(HSQUIRRELVM v)
    {
        // this, oldTargetName or index, newTargetName
        ExtractParams3<cbProject*, SkipParam, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::RenameBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::RenameBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::RenameBuildTarget: Expects an integer or wxString as first argument!"));

        if (targetNameOrIndex.isInt)
        {
            sq_pushbool(v, extractor.p0->RenameBuildTarget(targetNameOrIndex.intValue,
                                                           *extractor.p2));
        }
        else
        {
            sq_pushbool(v, extractor.p0->RenameBuildTarget(*targetNameOrIndex.strValue,
                                                           *extractor.p2));
        }
        return 1;
    }

    SQInteger cbProject_DuplicateBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName or index, newTargetName
        ExtractParams3<cbProject*, SkipParam, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::DuplicateBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::DuplicateBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::DuplicateBuildTarget: Expects an integer or wxString as first argument!"));

        ProjectBuildTarget *result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->DuplicateBuildTarget(targetNameOrIndex.intValue, *extractor.p2);
        else
            result = extractor.p0->DuplicateBuildTarget(*targetNameOrIndex.strValue, *extractor.p2);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger cbProject_RemoveBuildTarget(HSQUIRRELVM v)
    {
        // this, targetName or index
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::RemoveBuildTarget"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::RemoveBuildTarget"))
            return sq_throwerror(v, _SC("cbProject::RemoveBuildTarget: Expects an integer or wxString as first argument!"));

        bool result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->RemoveBuildTarget(targetNameOrIndex.intValue);
        else
            result = extractor.p0->RemoveBuildTarget(*targetNameOrIndex.strValue);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger cbProject_ExportTargetAsProject(HSQUIRRELVM v)
    {
        // this, targetName or index
        ExtractParams2<cbProject*, SkipParam> extractor(v);
        if (!extractor.Process("cbProject::ExportTargetAsProject"))
            return extractor.ErrorMessage();

        ParamIntOrWxString targetNameOrIndex;
        if (!targetNameOrIndex.Parse(extractor, 2, "cbProject::ExportTargetAsProject"))
            return sq_throwerror(v, _SC("cbProject::ExportTargetAsProject: Expects an integer or wxString as first argument!"));

        bool result;
        if (targetNameOrIndex.isInt)
            result = extractor.p0->ExportTargetAsProject(targetNameOrIndex.intValue);
        else
            result = extractor.p0->ExportTargetAsProject(*targetNameOrIndex.strValue);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger cbProject_BuildTargetValid(HSQUIRRELVM v)
    {
        // this, name, virtuals_too
        ExtractParams3<const cbProject*, const wxString *, bool> extractor(v);
        if (!extractor.Process("cbProject::BuildTargetValid"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->BuildTargetValid(*extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbProject_GetFirstValidBuildTargetName(HSQUIRRELVM v)
    {
        // this, virtuals_too
        ExtractParams2<const cbProject*, bool> extractor(v);
        if (!extractor.Process("cbProject::GetFirstValidBuildTargetName"))
            return extractor.ErrorMessage();

        const wxString &result = extractor.p0->GetFirstValidBuildTargetName(extractor.p1);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger cbProject_SetActiveBuildTarget(HSQUIRRELVM v)
    {
        // this, name
        ExtractParams2<cbProject*, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::SetActiveBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->SetActiveBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_SelectTarget(HSQUIRRELVM v)
    {
        // this, initial, evenIfOne
        ExtractParams3<cbProject*, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbProject::SelectTarget"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->SelectTarget(extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbProject_GetCurrentlyCompilingTarget(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetCurrentlyCompilingTarget"))
            return extractor.ErrorMessage();

        return ConstructAndReturnNonOwnedPtrOrNull(v, extractor.p0->GetCurrentlyCompilingTarget());
    }

    SQInteger cbProject_SetCurrentlyCompilingTarget(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Implement something like a wrapper, which makes it easier to handle the
        //  ProjectBuildTarget being nullptr. Currently ProcessParams requires this to be non-null
        // and so we cannot use it.

        // this, bt
        ExtractParams2<cbProject*, SkipParam/*ProjectBuildTarget**/> extractor(v);
        if (!extractor.Process("cbProject::SetCurrentlyCompilingTarget"))
            return extractor.ErrorMessage();

        ProjectBuildTarget *target = nullptr;
        if (sq_gettype(v, 2) != OT_NULL)
        {
            if (!extractor.ProcessParam(target, 2, "cbProject::SetCurrentlyCompilingTarget"))
                return extractor.ErrorMessage();
        }

        extractor.p0->SetCurrentlyCompilingTarget(target);
        return 0;
    }

    SQInteger cbProject_GetModeForPCH(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetModeForPCH"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, extractor.p0->GetModeForPCH());
        return 1;
    }

    SQInteger cbProject_SetModeForPCH(HSQUIRRELVM v)
    {
        // this, mode
        ExtractParams2<cbProject*, SQInteger> extractor(v);
        if (!extractor.Process("cbProject::SetModeForPCH"))
            return extractor.ErrorMessage();

        if (extractor.p1 < pchSourceDir || extractor.p1 > pchSourceFile)
            return sq_throwerror(v, _SC("cbProject::SetModeForPCH: The value of 'mode' parameter is out of range!"));
        extractor.p0->SetModeForPCH(PCHMode(extractor.p1));
        return 0;
    }

    SQInteger cbProject_ShowNotes(HSQUIRRELVM v)
    {
        // this, nonEmptyONly, editable
        ExtractParams3<cbProject*, bool, bool> extractor(v);
        if (!extractor.Process("cbProject::ShowNotes"))
            return extractor.ErrorMessage();

        extractor.p0->ShowNotes(extractor.p1, extractor.p2);
        return 0;
    }

    struct FindXmlElementResult
    {
        TiXmlElement *element = nullptr;
        wxString errorStr;
    };

    static FindXmlElementResult FindExtensionElement(cbProject *project, const wxString &query)
    {
        TiXmlNode *extensionNode = project->GetExtensionsNode();
        if (!extensionNode)
            return FindXmlElementResult();
        TiXmlElement *currentElem = extensionNode->ToElement();
        if (!currentElem)
            return FindXmlElementResult();

        // Note: This is slow!
        const wxArrayString names = GetArrayFromString(query, wxT("/"), false);
        for (const wxString &name : names)
        {
            const wxString::size_type openBracePos = name.find_first_of(wxT("[("));
            if (openBracePos != wxString::npos)
            {
                if (name[openBracePos] == wxT('['))
                {
                    const wxString::size_type closeBracePos = name.find(wxT(']'), openBracePos + 1);
                    if (closeBracePos == wxString::npos || closeBracePos != name.length() - 1)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid index format in '%s'!"), name.wx_str());
                        return result;
                    }

                    const wxString nameStripped = name.substr(0, openBracePos);
                    long lIndex;
                    const wxString indexStr = name.substr(openBracePos + 1,
                                                          closeBracePos - openBracePos - 1);
                    if (!indexStr.ToLong(&lIndex))
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Can't convert '%s' to integer!"),
                                               indexStr.wx_str());
                        return result;
                    }

                    const int index = int(lIndex);

                    int currentIndex = -1;
                    for (TiXmlNode *child = currentElem->FirstChild();
                         child;
                         child = child->NextSibling())
                    {
                        TiXmlElement *childElement = child->ToElement();
                        if (wxString(childElement->Value(), wxConvUTF8) != nameStripped)
                            continue;
                        ++currentIndex;
                        if (currentIndex == index)
                        {
                            currentElem = childElement;
                            break;
                        }
                    }
                    if (currentIndex != index)
                        return FindXmlElementResult();
                }
                else if (name[openBracePos] == wxT('('))
                {
                    const wxString::size_type closeBracePos = name.find(wxT(')'), openBracePos + 1);
                    if (closeBracePos == wxString::npos || closeBracePos != name.length() - 1)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid attribute format in '%s'!"),
                                               name.wx_str());
                        return result;
                    }

                    const wxString nameStripped = name.substr(0, openBracePos);
                    const wxString attributeStr = name.substr(openBracePos + 1,
                                                           closeBracePos - openBracePos - 1);
                    const wxString::size_type equalPos = attributeStr.find(wxT('='));
                    if (equalPos == wxString::npos)
                    {
                        FindXmlElementResult result;
                        result.errorStr.Printf(wxT("Invalid attribute format in '%s'!"),
                                               attributeStr.wx_str());
                        return result;
                    }

                    const std::string attributeName = wxString(attributeStr.substr(0, equalPos)).utf8_str().data();
                    const std::string attributeValue = wxString(attributeStr.substr(equalPos + 1)).utf8_str().data();
                    for (TiXmlNode *child = currentElem->FirstChild();
                         child;
                         child = child->NextSibling())
                    {
                        TiXmlElement *childElement = child->ToElement();
                        if (wxString(childElement->Value(), wxConvUTF8) != nameStripped)
                            continue;

                        const char *value = childElement->Attribute(attributeName.c_str());
                        if (value != nullptr && attributeValue == value)
                        {
                            currentElem = childElement;
                            break;
                        }
                    }
                }
            }
            else
                currentElem = currentElem->FirstChildElement(name.utf8_str().data());

            if (!currentElem)
                return FindXmlElementResult();
        }

        FindXmlElementResult result;
        result.element = currentElem;
        return result;
    }

    static wxString FindFullExtensionPathForNode(const TiXmlNode *node)
    {
        if (!node)
            return wxString();

        struct StackItem
        {
            const char *name;
            int index;
        };
        std::vector<StackItem> extensionStack;

        while (node)
        {
            const char *nodeValue = node->ToElement()->Value();
            if (strcmp(nodeValue, "Extensions") == 0)
                break;

            int index = 0;
            // Find index by going back through the siblings and matching the names.
            for (const TiXmlNode *sibling = node->PreviousSibling();
                 sibling;
                 sibling = sibling->PreviousSibling())
            {
                const char *value = sibling->ToElement()->Value();
                if (strcmp(nodeValue, value) == 0)
                    index++;
            }

            StackItem item;
            item.name = nodeValue;
            item.index = index;
            extensionStack.push_back(item);

            node = node->Parent();
        }

        wxString result;
        for (std::vector<StackItem>::reverse_iterator it = extensionStack.rbegin();
             it != extensionStack.rend();
             ++it)
        {
            if (!result.empty())
                result << wxT('/');
            result << wxString(it->name, wxConvUTF8) << wxT('[') << it->index << wxT(']');
        }
        return result;
    }

    /// Squirrel function would expect a cbProject and an extension string. It will return a
    /// wxArrayString object.
    SQInteger cbProject_ExtensionListNodes(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionListNodes"))
            return extractor.ErrorMessage();
        const wxString &extension = *extractor.p1;

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, extension);
        if (queryResult.element == nullptr)
        {
            // FIXME (squirrel) Modify the API to not throw errors if something is missing.
            // Return null probably?
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxArrayString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        int index = 0;
        for (const TiXmlNode *child = queryResult.element->FirstChild();
             child;
             child = child->NextSibling())
        {
            wxString msg = extension + wxT("/") + wxString(child->Value(), wxConvUTF8);
            msg << wxT('[') << index << wxT(']');
            result.Add(msg);
            ++index;
        }
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject and an extension string. It will return a
    /// wxArrayString object.
    SQInteger cbProject_ExtensionListNodeAttributes(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionListNodeAttributes"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxArrayString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxArrayString result;
        for (const TiXmlAttribute *attr = queryResult.element->FirstAttribute();
             attr;
             attr = attr->Next())
        {
            result.Add(wxString(attr->Name(), wxConvUTF8));
        }
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string and attribute name. It will
    /// return a wxString object.
    SQInteger cbProject_ExtensionGetNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionGetNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
                return ConstructAndReturnInstance(v, wxString());
            else
                return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        wxString result;
        const char *attributeValue = queryResult.element->Attribute(extractor.p2->utf8_str().data());
        if (attributeValue)
            result = wxString(attributeValue, wxConvUTF8);
        return ConstructAndReturnInstance(v, result);
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name and
    /// attribute value.
    SQInteger cbProject_ExtensionSetNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams4<cbProject*, const wxString *, const wxString *, const wxString*> extractor(v);
        if (!extractor.Process("cbProject::ExtensionSetNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->SetAttribute(extractor.p2->utf8_str().data(),
                                          extractor.p3->utf8_str().data());
        extractor.p0->SetModified(true);
        return 0;
    }

    /// Squirrel function would expect a cbProject, an extension string, attribute name.
    SQInteger cbProject_ExtensionRemoveNodeAttribute(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionRemoveNodeAttribute"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        queryResult.element->RemoveAttribute(extractor.p2->utf8_str().data());
        extractor.p0->SetModified(true);
        return 0;
    }

    /// Squirrel function would expect a cbProject, an extension string and node name.
    /// It will return the extension of the newly created node, so it could be used in other
    /// node calls.
    SQInteger cbProject_ExtensionAddNode(HSQUIRRELVM v)
    {
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionAddNode"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) Simplify this. This code is too repetitive.
        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *newNode = queryResult.element->InsertEndChild(TiXmlElement(cbU2C(*extractor.p2)));
        const wxString resultExtension = FindFullExtensionPathForNode(newNode);
        extractor.p0->SetModified(true);
        return ConstructAndReturnInstance(v, resultExtension);
    }

    /// Squirrel function would expect a cbProject and extension string.
    SQInteger cbProject_ExtensionRemoveNode(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::ExtensionRemoveNode"))
            return extractor.ErrorMessage();

        FindXmlElementResult queryResult = FindExtensionElement(extractor.p0, *extractor.p1);
        if (queryResult.element == nullptr)
        {
            if (queryResult.errorStr.empty())
            {
                queryResult.errorStr.Printf(wxT("Can't find extension node '%s'!"),
                                            extractor.p1->wx_str());
            }
            return sq_throwerror(v, queryResult.errorStr.utf8_str().data());
        }

        TiXmlNode *parent = queryResult.element->Parent();
        parent->RemoveChild(queryResult.element);
        extractor.p0->SetModified(true);
        return 0;
    }

    SQInteger cbProject_DefineVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias, targets
        ExtractParams3<cbProject*, const wxString *, const wxArrayString *> extractor(v);
        if (!extractor.Process("cbProject::DefineVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->DefineVirtualBuildTarget(*extractor.p1, *extractor.p2));
        return 1;
    }

    SQInteger cbProject_HasVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::HasVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->HasVirtualBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_RemoveVirtualBuildTarget(HSQUIRRELVM v)
    {
        ExtractParams2<cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::RemoveVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->RemoveVirtualBuildTarget(*extractor.p1));
        return 1;
    }

    SQInteger cbProject_GetVirtualBuildTargets(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const cbProject*> extractor(v);
        if (!extractor.Process("cbProject::GetVirtualBuildTargets"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetVirtualBuildTargets());
    }

    SQInteger cbProject_GetVirtualBuildTargetGroup(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::GetVirtualBuildTargetGroup"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString *result = &const_cast<wxArrayString&>(extractor.p0->GetVirtualBuildTargetGroup(*extractor.p1));
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger cbProject_GetExpandedVirtualBuildTargetGroup(HSQUIRRELVM v)
    {
        // this, alias
        ExtractParams2<const cbProject*, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::GetExpandedVirtualBuildTargetGroup"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetExpandedVirtualBuildTargetGroup(*extractor.p1));
    }

    SQInteger cbProject_CanAddToVirtualBuildTarget(HSQUIRRELVM v)
    {
        // this, alias, target
        ExtractParams3<cbProject*, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("cbProject::CanAddToVirtualBuildTarget"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->CanAddToVirtualBuildTarget(*extractor.p1, *extractor.p2));
        return 1;
    }

    SQInteger ProjectManager_GetDefaultPath(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::GetDefaultPath"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetDefaultPath());
    }

    SQInteger ProjectManager_SetDefaultPath(HSQUIRRELVM v)
    {
        // this, path
        ExtractParams2<ProjectManager*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectManager::SetDefaultPath"))
            return extractor.ErrorMessage();
        extractor.p0->SetDefaultPath(*extractor.p1);
        return 0;
    }

    SQInteger ProjectManager_GetActiveProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::GetActiveProject"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtrOrNull(v, extractor.p0->GetActiveProject());
    }

    SQInteger ProjectManager_GetProjectCount(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::GetProjectCount"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetProjects()->GetCount());
        return 1;
    }

    SQInteger ProjectManager_GetProject(HSQUIRRELVM v)
    {
        // this, index
        ExtractParams2<ProjectManager*, SQInteger> extractor(v);
        if (!extractor.Process("ProjectManager::GetProject"))
            return extractor.ErrorMessage();

        ProjectsArray *projects = extractor.p0->GetProjects();
        if (extractor.p1 < 0 || extractor.p1>= SQInteger(projects->GetCount()))
            return sq_throwerror(v, _SC("ProjectManager::GetProject: Index out of bounds!"));
        cbProject *project = (*projects)[extractor.p1];
        return ConstructAndReturnNonOwnedPtr(v, project);
    }

    SQInteger ProjectManager_SetProject(HSQUIRRELVM v)
    {
        // this, project, refresh
        ExtractParams3<ProjectManager*, cbProject*, bool> extractor(v);
        if (!extractor.Process("ProjectManager::SetProject"))
            return extractor.ErrorMessage();
        extractor.p0->SetProject(extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger ProjectManager_LoadWorkspace(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<ProjectManager*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectManager::LoadWorkspace"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->LoadWorkspace(*extractor.p1));
        return 1;
    }

    SQInteger ProjectManager_SaveWorkspace(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::SaveWorkspace"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SaveWorkspace());
        return 1;
    }

    SQInteger ProjectManager_SaveWorkspaceAs(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<ProjectManager*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectManager::SaveWorkspaceAs"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SaveWorkspaceAs(*extractor.p1));
        return 1;
    }

    SQInteger ProjectManager_CloseWorkspace(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::CloseWorkspace"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->CloseWorkspace());
        return 1;
    }

    SQInteger ProjectManager_IsOpen(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<ProjectManager*, const wxString*> extractor(v);
        if (!extractor.Process("ProjectManager::IsOpen"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtrOrNull(v, extractor.p0->IsOpen(*extractor.p1));
    }

    SQInteger ProjectManager_LoadProject(HSQUIRRELVM v)
    {
        // this, filename, activateIt
        ExtractParams3<ProjectManager*, const wxString*, bool> extractor(v);
        if (!extractor.Process("ProjectManager::LoadProject"))
            return extractor.ErrorMessage();
        cbProject *project = extractor.p0->LoadProject(*extractor.p1, extractor.p2);
        return ConstructAndReturnNonOwnedPtrOrNull(v, project);
    }

    template<bool (ProjectManager::*func)(cbProject *)>
    SQInteger ProjectManager_SaveProject(HSQUIRRELVM v)
    {
        // this, project
        ExtractParams2<ProjectManager*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::SaveProject"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)(extractor.p1));
        return 1;
    }

    template<bool (ProjectManager::*func)()>
    SQInteger ProjectManager_DoSomethingReturnBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager_DoSomethingReturnBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    SQInteger ProjectManager_NewProject(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<ProjectManager*, const wxString *> extractor(v);
        if (!extractor.Process("ProjectManager::NewProject"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, extractor.p0->NewProject(*extractor.p1));
    }

    SQInteger ProjectManager_AddFileToProject(HSQUIRRELVM v)
    {
        // this, filename, project, target
        ExtractParams4<ProjectManager*, const wxString *, cbProject *, SQInteger> extractor(v);
        if (!extractor.Process("ProjectManager::AddFileToProject"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->AddFileToProject(*extractor.p1, extractor.p2, extractor.p3));
        return 1;
    }

    SQInteger ProjectManager_CloseProject(HSQUIRRELVM v)
    {
        // this, project, dontsave, refresh
        ExtractParams4<ProjectManager*, cbProject *, bool, bool> extractor(v);
        if (!extractor.Process("ProjectManager::CloseProject"))
            return extractor.ErrorMessage();
        const bool result = extractor.p0->CloseProject(extractor.p1, extractor.p2, extractor.p3);
        sq_pushbool(v, result);
        return 1;
    }

    template<bool (ProjectManager::*func)(bool)>
    SQInteger ProjectManager_CloseDontSave(HSQUIRRELVM v)
    {
        // this, dontsave
        ExtractParams2<ProjectManager*, bool> extractor(v);
        if (!extractor.Process("ProjectManager_CloseDontSave"))
            return extractor.ErrorMessage();
        const bool result = (extractor.p0->*func)(extractor.p1);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger ProjectManager_AddProjectDependency(HSQUIRRELVM v)
    {
        // this, base, dependsOn
        ExtractParams3<ProjectManager*, cbProject*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::AddProjectDependency"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->AddProjectDependency(extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger ProjectManager_RemoveProjectDependency(HSQUIRRELVM v)
    {
        // this, base, doesNotDependOn
        ExtractParams3<ProjectManager*, cbProject*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::RemoveProjectDependency"))
            return extractor.ErrorMessage();
        extractor.p0->RemoveProjectDependency(extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger ProjectManager_ClearProjectDependencies(HSQUIRRELVM v)
    {
        // this, base
        ExtractParams2<ProjectManager*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::ClearProjectDependencies"))
            return extractor.ErrorMessage();
        extractor.p0->ClearProjectDependencies(extractor.p1);
        return 0;
    }

    // FIXME (squirrel) Compress with previous
    SQInteger ProjectManager_RemoveProjectFromAllDependencies(HSQUIRRELVM v)
    {
        // this, base
        ExtractParams2<ProjectManager*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::RemoveProjectFromAllDependencies"))
            return extractor.ErrorMessage();
        extractor.p0->RemoveProjectFromAllDependencies(extractor.p1);
        return 0;
    }

    SQInteger ProjectManager_GetDependenciesForProject(HSQUIRRELVM v)
    {
        // this, base
        ExtractParams2<ProjectManager*, cbProject*> extractor(v);
        if (!extractor.Process("ProjectManager::GetDependenciesForProject"))
            return extractor.ErrorMessage();
        const ProjectsArray *deps = extractor.p0->GetDependenciesForProject(extractor.p1);
        if (deps)
        {
            sq_newarray(v, 0);

            PreserveTop preserver(v);

            const size_t count = deps->GetCount();
            for (size_t ii = 0; ii < count; ++ii)
            {
                // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
                cbProject *project = (*deps)[ii];
                if (ConstructAndReturnNonOwnedPtrOrNull(v, project) == -1)
                    return -1; // An error should have been logged already.
                sq_arrayappend(v, -2);
            }
            return 1;
        }
        else
        {
            sq_pushnull(v);
            return 1;
        }
    }

    SQInteger ProjectManager_RebuildTree(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<ProjectManager*> extractor(v);
        if (!extractor.Process("ProjectManager::RebuildTree"))
            return extractor.ErrorMessage();
        extractor.p0->GetUI().RebuildTree();
        return 0;
    }

    SQInteger EditorBase_Activate(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<EditorBase*> extractor(v);
        if (!extractor.Process("EditorBase::Activate"))
            return extractor.ErrorMessage();
        extractor.p0->Activate();
        return 0;
    }

    SQInteger EditorBase_Close(HSQUIRRELVM v)
    {
        ExtractParams1<EditorBase*> extractor(v);
        if (!extractor.Process("EditorBase::Close"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->Close());
        return 1;
    }

    SQInteger EditorBase_GotoLine(HSQUIRRELVM v)
    {
        // this, line, centerOnScreen
        ExtractParams3<EditorBase*, SQInteger, bool> extractor(v);
        if (!extractor.Process("EditorBase::GotoLine"))
            return extractor.ErrorMessage();
        extractor.p0->GotoLine(extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger cbEditor_GetProjectFile(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const cbEditor*> extractor(v);
        if (!extractor.Process("cbEditor::GetProjectFile"))
            return extractor.ErrorMessage();
        ProjectFile *file = extractor.p0->GetProjectFile();
        return ConstructAndReturnNonOwnedPtrOrNull(v, file);
    }

    template<void (cbEditor::*func)(int)>
    SQInteger cbEditor_DoFromLine(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor_DoFromLine"))
            return extractor.ErrorMessage();
        (extractor.p0->*func)(extractor.p1);
        return 0;
    }

    SQInteger cbEditor_GetLineIndentInSpaces(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<const cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor::GetLineIndentInSpaces"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, extractor.p0->GetLineIndentInSpaces(extractor.p1));
        return 1;
    }

    SQInteger cbEditor_GetLineIndentString(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<const cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor::GetLineIndentString"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetLineIndentString(extractor.p1));
    }

    SQInteger cbEditor_Reload(HSQUIRRELVM v)
    {
        // this, detectEncoding
        ExtractParams2<cbEditor*, bool> extractor(v);
        if (!extractor.Process("cbEditor::Reload"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->Reload(extractor.p1));
        return 1;
    }

    SQInteger cbEditor_Print(HSQUIRRELVM v)
    {
        // this, selectionOnly, pcm, line_numbers
        ExtractParams4<cbEditor*, bool, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbEditor::Print"))
            return extractor.ErrorMessage();

        if (extractor.p2 < pcmBlackAndWhite || extractor.p2 > pcmAsIs)
            return sq_throwerror(v, _SC("cbEditor::Print: The value of the pcm parameter is out of range!"));

        const PrintColourMode pcm = PrintColourMode(extractor.p2);
        extractor.p0->Print(extractor.p1, pcm, extractor.p3);
        return 0;
    }

    SQInteger cbEditor_AddBreakpoint(HSQUIRRELVM v)
    {
        // this, line, notifyDebugger
        ExtractParams3<cbEditor*, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbEditor::AddBreakpoint"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->AddBreakpoint(extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbEditor_RemoveBreakpoint(HSQUIRRELVM v)
    {
        // this, line, notifyDebugger
        ExtractParams3<cbEditor*, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbEditor::RemoveBreakpoint"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->RemoveBreakpoint(extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger cbEditor_ToggleBreakpoint(HSQUIRRELVM v)
    {
        // this, line, notifyDebugger
        ExtractParams3<cbEditor*, SQInteger, bool> extractor(v);
        if (!extractor.Process("cbEditor::ToggleBreakpoint"))
            return extractor.ErrorMessage();
        extractor.p0->ToggleBreakpoint(extractor.p1, extractor.p2);
        return 0;
    }

    SQInteger cbEditor_HasBreakpoint(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<const cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor::HasBreakpoint"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->HasBreakpoint(extractor.p1));
        return 1;
    }

    SQInteger cbEditor_HasBookmark(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<const cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor::HasBookmark"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->HasBookmark(extractor.p1));
        return 1;
    }

    SQInteger cbEditor_ToggleBookmark(HSQUIRRELVM v)
    {
        // this, line
        ExtractParams2<cbEditor*, SQInteger> extractor(v);
        if (!extractor.Process("cbEditor::ToggleBookmark"))
            return extractor.ErrorMessage();
        extractor.p0->ToggleBookmark(extractor.p1);
        return 0;
    }

    SQInteger cbEditor_SetText(HSQUIRRELVM v)
    {
        ExtractParams2<cbEditor*, const wxString *> extractor(v);
        if (!extractor.Process("cbEditor::SetText"))
            return extractor.ErrorMessage();

        extractor.p0->GetControl()->SetText(*extractor.p1);
        return 0;
    }

    SQInteger cbEditor_GetText(HSQUIRRELVM v)
    {
        ExtractParams1<const cbEditor*> extractor(v);
        if (!extractor.Process("cbEditor::GetText"))
            return extractor.ErrorMessage();
        return ConstructAndReturnInstance(v, extractor.p0->GetControl()->GetText());
    }

    template<cbEditor* (EditorManager::*func)(const wxString &filename)>
    SQInteger EditorManager_FilenameTocbEditor(HSQUIRRELVM v)
    {
        // this, filename
        ExtractParams2<EditorManager*, const wxString *> extractor(v);
        if (!extractor.Process("EditorManager_FilenameTocbEditor"))
            return extractor.ErrorMessage();
        cbEditor *result = (extractor.p0->*func)(*extractor.p1);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger EditorManager_Open(HSQUIRRELVM v)
    {
        // Cannot be done with EditorManager_FilenameTocbEditor, because the method type won't
        // match (there are default parameter values).
        // this, filename
        ExtractParams2<EditorManager*, const wxString *> extractor(v);
        if (!extractor.Process("EditorManager::Open"))
            return extractor.ErrorMessage();
        cbEditor *result = extractor.p0->Open(*extractor.p1);
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger EditorManager_GetBuiltinEditor(HSQUIRRELVM v)
    {
        // this, filename or index
        ExtractParams2<EditorManager*, SkipParam> extractor(v);
        if (!extractor.Process("EditorManager::GetBuiltinEditor"))
            return extractor.ErrorMessage();

        const SQObjectType type = sq_gettype(v, 2);
        switch (type)
        {
        case OT_INTEGER:
            {
                const int index = extractor.GetParamInt(2);
                cbEditor *result = extractor.p0->GetBuiltinEditor(index);
                return ConstructAndReturnNonOwnedPtrOrNull(v, result);
            }
        case OT_INSTANCE:
            {
                const wxString *filename;
                if (!extractor.ProcessParam(filename, 2, "EditorManager::GetBuiltinEditor"))
                    return extractor.ErrorMessage();
                cbEditor *result = extractor.p0->GetBuiltinEditor(*filename);
                return ConstructAndReturnNonOwnedPtrOrNull(v, result);
            }
        default:
            return sq_throwerror(v, _SC("EditorManager::GetBuiltinEditor given unsupported type - takes index or filename!"));
        }
    }

    SQInteger EditorManager_GetBuiltinActiveEditor(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<EditorManager*> extractor(v);
        if (!extractor.Process("EditorManager::GetBuiltinActiveEditor"))
            return extractor.ErrorMessage();
        cbEditor *result = extractor.p0->GetBuiltinActiveEditor();
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger EditorManager_GetActiveEditor(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<EditorManager*> extractor(v);
        if (!extractor.Process("EditorManager::GetActiveEditor"))
            return extractor.ErrorMessage();
        EditorBase *result = extractor.p0->GetActiveEditor();
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger EditorManager_SwapActiveHeaderSource(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<EditorManager*> extractor(v);
        if (!extractor.Process("EditorManager::SwapActiveHeaderSource"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SwapActiveHeaderSource());
        return 1;
    }

    template<bool (EditorManager::*func)(bool)>
    SQInteger Editor_ParamBoolReturnBool(HSQUIRRELVM v)
    {
        // this, dontsave
        ExtractParams2<EditorManager*, bool> extractor(v);
        if (!extractor.Process("Editor_ParamBoolReturnBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)(extractor.p1));
        return 1;
    }

    template<bool (EditorManager::*func)()>
    SQInteger Editor_DoSomethingReturnBool(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<EditorManager*> extractor(v);
        if (!extractor.Process("Editor_DoSomethingReturnBool"))
            return extractor.ErrorMessage();
        sq_pushbool(v, (extractor.p0->*func)());
        return 1;
    }

    SQInteger EditorManager_SaveAs(HSQUIRRELVM v)
    {
        // this, index
        ExtractParams2<EditorManager*, SQInteger> extractor(v);
        if (!extractor.Process("EditorManager::SaveAs"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->SaveAs(extractor.p1));
        return 1;
    }

    SQInteger EditorManager_Close(HSQUIRRELVM v)
    {
        // FIXME (squirrel) Add dontsave=false support
        // this, filename or index
        ExtractParams2<EditorManager*, SkipParam> extractor(v);
        if (!extractor.Process("EditorManager::Close"))
            return extractor.ErrorMessage();

        const SQObjectType type = sq_gettype(v, 2);
        switch (type)
        {
        case OT_INTEGER:
            {
                const int index = extractor.GetParamInt(2);
                sq_pushbool(v, extractor.p0->Close(index));
                return 1;
            }
        case OT_INSTANCE:
            {
                const wxString *filename;
                if (!extractor.ProcessParam(filename, 2, "EditorManager::Close"))
                    return extractor.ErrorMessage();
                sq_pushbool(v, extractor.p0->Close(*filename));
                return 1;
            }
        default:
            return sq_throwerror(v, _SC("EditorManager::Close given unsupported type - takes index or filename!"));
        }
    }

    SQInteger EditorManager_Save(HSQUIRRELVM v)
    {
        // this, filename or index
        ExtractParams2<EditorManager*, SkipParam> extractor(v);
        if (!extractor.Process("EditorManager::Save"))
            return extractor.ErrorMessage();

        const SQObjectType type = sq_gettype(v, 2);
        switch (type)
        {
        case OT_INTEGER:
            {
                const int index = extractor.GetParamInt(2);
                sq_pushbool(v, extractor.p0->Save(index));
                return 1;
            }
        case OT_INSTANCE:
            {
                const wxString *filename;
                if (!extractor.ProcessParam(filename, 2, "EditorManager::Save"))
                    return extractor.ErrorMessage();
                sq_pushbool(v, extractor.p0->Save(*filename));
                return 1;
            }
        default:
            return sq_throwerror(v, _SC("EditorManager::Save given unsupported type - takes index or filename!"));
        }
    }

    SQInteger UserVariableManager_Exists(HSQUIRRELVM v)
    {
        // this, variable
        ExtractParams2<UserVariableManager*, const wxString *> extractor(v);
        if (!extractor.Process("UserVariableManager::Exists"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->Exists(*extractor.p1));
        return 1;
    }

    SQInteger ScriptingManager_RegisterScriptMenu(HSQUIRRELVM v)
    {
        // this, menuPath, scriptOrFunc, isFunction
        ExtractParams4<ScriptingManager*, const wxString *, const wxString*, bool> extractor(v);
        if (!extractor.Process("ScriptingManager::RegisterScriptMenu"))
            return extractor.ErrorMessage();
        sq_pushbool(v, extractor.p0->RegisterScriptMenu(*extractor.p1, *extractor.p2, extractor.p3));
        return 1;
    }

    SQInteger CompilerFactory_IsValidCompilerID(HSQUIRRELVM v)
    {
        // env table, id
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("CompilerFactory::IsValidCompilerID"))
            return extractor.ErrorMessage();
        sq_pushbool(v, CompilerFactory::IsValidCompilerID(*extractor.p1));
        return 1;
    }

    SQInteger CompilerFactory_GetCompilerIndex(HSQUIRRELVM v)
    {
        // env table, id
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("CompilerFactory::GetCompilerIndex"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, CompilerFactory::GetCompilerIndex(*extractor.p1));
        return 1;
    }

    SQInteger CompilerFactory_GetDefaultCompilerID(HSQUIRRELVM v)
    {
        // env table
        ExtractParams1<SkipParam> extractor(v);
        if (!extractor.Process("CompilerFactory::GetDefaultCompilerID"))
            return extractor.ErrorMessage();
        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxString &result = const_cast<wxString&>(CompilerFactory::GetDefaultCompilerID());
        return ConstructAndReturnNonOwnedPtr(v, &result);
    }

    SQInteger CompilerFactory_GetCompilerVersionString(HSQUIRRELVM v)
    {
        // env table, id
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("CompilerFactory::GetCompilerVersionString"))
            return extractor.ErrorMessage();
        const wxString result = CompilerFactory::GetCompilerVersionString(*extractor.p1);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger CompilerFactory_CompilerInheritsFrom(HSQUIRRELVM v)
    {
        // env table, id, from_id
        ExtractParams3<SkipParam, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("CompilerFactory::CompilerInheritsFrom"))
            return extractor.ErrorMessage();
        sq_pushbool(v, CompilerFactory::CompilerInheritsFrom(*extractor.p1, *extractor.p2));
        return 1;
    }

    SQInteger CompilerFactory_GetCompilerIDByName(HSQUIRRELVM v)
    {
        // env table, name
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("CompilerFactory::GetCompilerIDByName"))
            return extractor.ErrorMessage();
        Compiler *compiler = CompilerFactory::GetCompilerByName(*extractor.p1);
        wxString result = (compiler ? compiler->GetID() : wxString());
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger FileTreeData_GetKind(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const FileTreeData*> extractor(v);
        if (!extractor.Process("FileTreeData::GetKind"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, SQInteger(extractor.p0->GetKind()));
        return 1;
    }

    SQInteger FileTreeData_GetProject(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const FileTreeData*> extractor(v);
        if (!extractor.Process("FileTreeData::GetProject"))
            return extractor.ErrorMessage();
        cbProject *result = extractor.p0->GetProject();
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger FileTreeData_GetFileIndex(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const FileTreeData*> extractor(v);
        if (!extractor.Process("FileTreeData::GetFileIndex"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, SQInteger(extractor.p0->GetFileIndex()));
        return 1;
    }

    SQInteger FileTreeData_GetProjectFile(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const FileTreeData*> extractor(v);
        if (!extractor.Process("FileTreeData::GetProjectFile"))
            return extractor.ErrorMessage();
        ProjectFile *result = extractor.p0->GetProjectFile();
        return ConstructAndReturnNonOwnedPtrOrNull(v, result);
    }

    SQInteger FileTreeData_GetFolder(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<const FileTreeData*> extractor(v);
        if (!extractor.Process("FileTreeData::GetFolder"))
            return extractor.ErrorMessage();
        wxString *result = &const_cast<wxString&>(extractor.p0->GetFolder());
        return ConstructAndReturnNonOwnedPtr(v, result);
    }

    SQInteger FileTreeData_SetKind(HSQUIRRELVM v)
    {
        // this, kind
        ExtractParams2<FileTreeData*, SQInteger> extractor(v);
        if (!extractor.Process("FileTreeData::SetKind"))
            return extractor.ErrorMessage();
        if (extractor.p1 < FileTreeData::ftdkUndefined
            || extractor.p1 > FileTreeData::ftdkVirtualFolder)
        {
            return sq_throwerror(v, _SC("FileTreeData::SetKind: The value of the 'kind' parameter is out of range!"));
        }
        extractor.p0->SetKind(FileTreeData::FileTreeDataKind(extractor.p1));
        return 0;
    }

    SQInteger FileTreeData_SetProject(HSQUIRRELVM v)
    {
        // this, project
        ExtractParams2<FileTreeData*, cbProject*> extractor(v);
        if (!extractor.Process("FileTreeData::SetProject"))
            return extractor.ErrorMessage();
        extractor.p0->SetProject(extractor.p1);
        return 0;
    }

    SQInteger FileTreeData_SetFileIndex(HSQUIRRELVM v)
    {
        // this, index
        ExtractParams2<FileTreeData*, SQInteger> extractor(v);
        if (!extractor.Process("FileTreeData::SetFileIndex"))
            return extractor.ErrorMessage();
        extractor.p0->SetFileIndex(extractor.p1);
        return 0;
    }

    SQInteger FileTreeData_SetProjectFile(HSQUIRRELVM v)
    {
        // this, file
        ExtractParams2<FileTreeData*, ProjectFile*> extractor(v);
        if (!extractor.Process("FileTreeData::SetProjectFile"))
            return extractor.ErrorMessage();
        extractor.p0->SetProjectFile(extractor.p1);
        return 0;
    }

    SQInteger FileTreeData_SetFolder(HSQUIRRELVM v)
    {
        // this, folder
        ExtractParams2<FileTreeData*, const wxString*> extractor(v);
        if (!extractor.Process("FileTreeData::SetFolder"))
            return extractor.ErrorMessage();
        extractor.p0->SetFolder(*extractor.p1);
        return 0;
    }

    template<>
    MembersType<PluginInfo> FindMembers<PluginInfo>::members{};
    template<>
    MembersType<ProjectFile> FindMembers<ProjectFile>::members{};

    void Register_Constants(ScriptingManager *manager);
    void Register_Globals(HSQUIRRELVM v);
    void Register_IO(HSQUIRRELVM v);
    void Register_wxTypes(HSQUIRRELVM v);
    void Register_ProgressDialog(HSQUIRRELVM v);
    void Register_UtilDialogs(HSQUIRRELVM v, ScriptingManager *manager);
    void Register_IO(HSQUIRRELVM v, ScriptingManager *manager);
    void Register_ScriptPlugin(HSQUIRRELVM v, ScriptingManager *manager);
    void Unregister_ScriptPlugin();

    void RegisterBindings(HSQUIRRELVM v, ScriptingManager *manager)
    {
        Register_wxTypes(v);
        Register_Constants(manager);
        Register_Globals(v);
        Register_IO(v, manager); // IO is enabled, but just for harmless functions
        Register_ProgressDialog(v);
        Register_UtilDialogs(v, manager);

        PreserveTop preserveTop(v);
        sq_pushroottable(v);

        // FIXME (squirrel) Add a way to prevent instances from being constructed for a particular
        // class type.

        {
            // Register ConfigManager
            const SQInteger classDecl = CreateClassDecl<ConfigManager>(v);
            BindMethod(v, _SC("Read"), ConfigManager_Read, _SC("ConfigManager::Read"));
            BindMethod(v, _SC("Write"), ConfigManager_Write, _SC("ConfigManager::Write"));

            BindDefaultInstanceCmp<ConfigManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectFile
            const SQInteger classDecl = CreateClassDecl<ProjectFile>(v);
            BindMethod(v, _SC("AddBuildTarget"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::AddBuildTarget>,
                       _SC("ProjectFile::AddBuildTarget"));
            BindMethod(v, _SC("RenameBuildTarget"), ProjectFile_RenameBuildTarget,
                       _SC("ProjectFile::RenameBuildTarget"));
            BindMethod(v, _SC("RemoveBuildTarget"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::RemoveBuildTarget>,
                       _SC("ProjectFile::RemoveBuildTarget"));
            BindMethod(v, _SC("GetBuildTargets"), ProjectFile_GetBuildTargets,
                       _SC("ProjectFile::GetBuildTargets"));
            BindMethod(v, _SC("GetBaseName"), ProjectFile_GetBaseName,
                       _SC("ProjectFile::GetBaseName"));
            BindMethod(v, _SC("GetObjName"), ProjectFile_GetObjName,
                       _SC("ProjectFile::GetObjName"));
            BindMethod(v, _SC("SetObjName"),
                       ProjectFile_SingleWxStringParam<&ProjectFile::SetObjName>,
                       _SC("ProjectFile::SetObjName"));
            BindMethod(v, _SC("GetParentProject"), ProjectFile_GetParentProject,
                       _SC("ProjectFile::GetParentProject"));
            BindMethod(v, _SC("SetUseCustomBuildCommand"), ProjectFile_SetUseCustomBuildCommand,
                       _SC("ProjectFile::SetUseCustomBuildCommand"));
            BindMethod(v, _SC("SetCustomBuildCommand"), ProjectFile_SetCustomBuildCommand,
                       _SC("ProjectFile::SetCustomBuildCommand"));
            BindMethod(v, _SC("GetUseCustomBuildCommand"), ProjectFile_GetUseCustomBuildCommand,
                       _SC("ProjectFile::GetUseCustomBuildCommand"));
            BindMethod(v, _SC("GetCustomBuildCommand"), ProjectFile_GetCustomBuildCommand,
                       _SC("ProjectFile::GetCustomBuildCommand"));

            MembersType<ProjectFile> &members = BindMembers<ProjectFile>(v);
            addMemberRef(members, _SC("file"), &ProjectFile::file);
            addMemberRef(members, _SC("relativeFilename"), &ProjectFile::relativeFilename);
            addMemberRef(members, _SC("relativeToCommonTopLevelPath"),
                         &ProjectFile::relativeToCommonTopLevelPath);
            addMemberBool(members, _SC("compile"), &ProjectFile::compile);
            addMemberBool(members, _SC("link"), &ProjectFile::link);
            addMemberUInt(members, _SC("weight"), &ProjectFile::weight);
            addMemberRef(members, _SC("compilerVar"), &ProjectFile::compilerVar);
            addMemberRef(members, _SC("buildTargets"), &ProjectFile::buildTargets);

            BindDefaultInstanceCmp<ProjectFile>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompileOptionsBase
            const SQInteger classDecl = CreateClassDecl<CompileOptionsBase>(v);
            BindMethod(v, _SC("AddPlatform"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::AddPlatform>,
                       _SC("CompileOptionsBase::AddPlatform"));
            BindMethod(v, _SC("RemovePlatform"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::RemovePlatform>,
                       _SC("CompileOptionsBase::RemovePlatform"));
            BindMethod(v, _SC("SetPlatforms"),
                       CompileOptionsBase_Platform<&CompileOptionsBase::SetPlatforms>,
                       _SC("CompileOptionsBase::SetPlatforms"));
            BindMethod(v, _SC("GetPlatforms"), CompileOptionsBase_GetPlatforms,
                       _SC("CompileOptionsBase::GetPlatforms"));
            BindMethod(v, _SC("SupportsCurrentPlatform"),
                         CompileOptionsBase_SupportsCurrentPlatform,
                         _SC("CompileOptionsBase::SupportsCurrentPlatform"));
            BindMethod(v, _SC("SetLinkerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLinkerOptions>,
                       _SC("CompileOptionsBase::SetLinkerOptions"));
            BindMethod(v, _SC("SetLinkLibs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLinkLibs>,
                       _SC("CompileOptionsBase::SetLinkLibs"));
            BindMethod(v, _SC("SetLinkerExecutable"), CompileOptionsBase_SetLinkerExecutable,
                       _SC("CompileOptionsBase::SetLinkerExecutable"));
            BindMethod(v, _SC("GetLinkerExecutable"), CompileOptionsBase_GetLinkerExecutable,
                       _SC("CompileOptionsBase::GetLinkerExecutable"));
            BindMethod(v, _SC("SetCompilerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCompilerOptions>,
                       _SC("CompileOptionsBase::SetCompilerOptions"));
            BindMethod(v, _SC("SetResourceCompilerOptions"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetResourceCompilerOptions>,
                       _SC("CompileOptionsBase::SetResourceCompilerOptions"));
            BindMethod(v, _SC("SetIncludeDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetIncludeDirs>,
                       _SC("CompileOptionsBase::SetIncludeDirs"));
            BindMethod(v, _SC("SetResourceIncludeDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetResourceIncludeDirs>,
                       _SC("CompileOptionsBase::SetResourceIncludeDirs"));
            BindMethod(v, _SC("SetLibDirs"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetLibDirs>,
                       _SC("CompileOptionsBase::SetLibDirs"));
            BindMethod(v, _SC("SetCommandsBeforeBuild"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::SetCommandsBeforeBuild"));
            BindMethod(v, _SC("SetCommandsAfterBuild"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetCommandsAfterBuild>,
                       _SC("CompileOptionsBase::SetCommandsAfterBuild"));
            BindMethod(v, _SC("GetLinkerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLinkerOptions>,
                       _SC("CompileOptionsBase::GetLinkerOptions"));
            BindMethod(v, _SC("GetLinkLibs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLinkLibs>,
                       _SC("CompileOptionsBase::GetLinkLibs"));
            BindMethod(v, _SC("GetCompilerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCompilerOptions>,
                       _SC("CompileOptionsBase::GetCompilerOptions"));
            BindMethod(v, _SC("GetResourceCompilerOptions"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetResourceCompilerOptions>,
                       _SC("CompileOptionsBase::GetResourceCompilerOptions"));
            BindMethod(v, _SC("GetIncludeDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetIncludeDirs>,
                       _SC("CompileOptionsBase::GetIncludeDirs"));
            BindMethod(v, _SC("GetResourceIncludeDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetResourceIncludeDirs>,
                       _SC("CompileOptionsBase::GetResourceIncludeDirs"));
            BindMethod(v, _SC("GetLibDirs"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetLibDirs>,
                       _SC("CompileOptionsBase::GetLibDirs"));
            BindMethod(v, _SC("GetCommandsBeforeBuild"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::GetCommandsBeforeBuild"));
            BindMethod(v, _SC("GetCommandsAfterBuild"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetCommandsAfterBuild>,
                       _SC("CompileOptionsBase::GetCommandsAfterBuild"));
            BindMethod(v, _SC("GetModified"),
                       Generic_GetBool<CompileOptionsBase, &CompileOptionsBase::GetModified>,
                       _SC("CompileOptionsBase::GetModified"));
            BindMethod(v, _SC("SetModified"),
                       Generic_SetBool<CompileOptionsBase, &CompileOptionsBase::SetModified>,
                       _SC("CompileOptionsBase::SetModified"));
            BindMethod(v, _SC("AddLinkerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLinkerOption>,
                       _SC("CompileOptionsBase::AddLinkerOption"));
            BindMethod(v, _SC("AddLinkLib"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLinkLib>,
                       _SC("CompileOptionsBase::AddLinkLib"));
            BindMethod(v, _SC("AddCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCompilerOption>,
                       _SC("CompileOptionsBase::AddCompilerOption"));
            BindMethod(v, _SC("AddResourceCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddResourceCompilerOption>,
                       _SC("CompileOptionsBase::AddResourceCompilerOption"));
            BindMethod(v, _SC("AddIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddIncludeDir>,
                       _SC("CompileOptionsBase::AddIncludeDir"));
            BindMethod(v, _SC("AddResourceIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddResourceIncludeDir>,
                       _SC("CompileOptionsBase::AddResourceIncludeDir"));
            BindMethod(v, _SC("AddLibDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddLibDir>,
                       _SC("CompileOptionsBase::AddLibDir"));
            BindMethod(v, _SC("AddCommandsBeforeBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::AddCommandsBeforeBuild"));
            BindMethod(v, _SC("AddCommandsAfterBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddCommandsAfterBuild>,
                       _SC("CompileOptionsBase::AddCommandsAfterBuild"));
            BindMethod(v, _SC("ReplaceLinkerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLinkerOption>,
                       _SC("CompileOptionsBase::ReplaceLinkerOption"));
            BindMethod(v, _SC("ReplaceLinkLib"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLinkLib>,
                       _SC("CompileOptionsBase::ReplaceLinkLib"));
            BindMethod(v, _SC("ReplaceCompilerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceCompilerOption>,
                       _SC("CompileOptionsBase::ReplaceCompilerOption"));
            BindMethod(v, _SC("ReplaceResourceCompilerOption"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceResourceCompilerOption>,
                       _SC("CompileOptionsBase::ReplaceResourceCompilerOption"));
            BindMethod(v, _SC("ReplaceIncludeDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceIncludeDir>,
                       _SC("CompileOptionsBase::ReplaceIncludeDir"));
            BindMethod(v, _SC("ReplaceResourceIncludeDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceResourceIncludeDir>,
                       _SC("CompileOptionsBase::ReplaceResourceIncludeDir"));
            BindMethod(v, _SC("ReplaceLibDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLibDir>,
                       _SC("CompileOptionsBase::ReplaceLibDir"));
            BindMethod(v, _SC("ReplaceLibDir"),
                       CompileOptionsBase_ReplaceOption<&CompileOptionsBase::ReplaceLibDir>,
                       _SC("CompileOptionsBase::ReplaceLibDir"));
            BindMethod(v, _SC("RemoveLinkerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLinkerOption>,
                       _SC("CompileOptionsBase::RemoveLinkerOption"));
            BindMethod(v, _SC("RemoveLinkLib"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLinkLib>,
                       _SC("CompileOptionsBase::RemoveLinkLib"));
            BindMethod(v, _SC("RemoveCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCompilerOption>,
                       _SC("CompileOptionsBase::RemoveCompilerOption"));
            BindMethod(v, _SC("RemoveCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCompilerOption>,
                       _SC("CompileOptionsBase::RemoveCompilerOption"));
            BindMethod(v, _SC("RemoveIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveIncludeDir>,
                       _SC("CompileOptionsBase::RemoveIncludeDir"));
            BindMethod(v, _SC("RemoveResourceCompilerOption"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveResourceCompilerOption>,
                       _SC("CompileOptionsBase::RemoveResourceCompilerOption"));
            BindMethod(v, _SC("RemoveResourceIncludeDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveResourceIncludeDir>,
                       _SC("CompileOptionsBase::RemoveResourceIncludeDir"));
            BindMethod(v, _SC("RemoveLibDir"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveLibDir>,
                       _SC("CompileOptionsBase::RemoveLibDir"));
            BindMethod(v, _SC("RemoveCommandsBeforeBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCommandsBeforeBuild>,
                       _SC("CompileOptionsBase::RemoveCommandsBeforeBuild"));
            BindMethod(v, _SC("RemoveCommandsAfterBuild"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveCommandsAfterBuild>,
                       _SC("CompileOptionsBase::RemoveCommandsAfterBuild"));
            BindMethod(v, _SC("GetAlwaysRunPostBuildSteps"),
                       CompileOptionsBase_GetAlwaysRunPostBuildSteps,
                       _SC("CompileOptionsBase::GetAlwaysRunPostBuildSteps"));
            BindMethod(v, _SC("SetAlwaysRunPostBuildSteps"),
                       CompileOptionsBase_SetAlwaysRunPostBuildSteps,
                       _SC("CompileOptionsBase::SetAlwaysRunPostBuildSteps"));
            BindMethod(v, _SC("SetBuildScripts"),
                       CompileOptionsBase_SetArrayString<&CompileOptionsBase::SetBuildScripts>,
                       _SC("CompileOptionsBase::SetBuildScripts"));
            BindMethod(v, _SC("GetBuildScripts"),
                       CompileOptionsBase_GetArrayString<&CompileOptionsBase::GetBuildScripts>,
                       _SC("CompileOptionsBase::GetBuildScripts"));
            BindMethod(v, _SC("AddBuildScript"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::AddBuildScript>,
                       _SC("CompileOptionsBase::AddBuildScript"));
            BindMethod(v, _SC("RemoveBuildScript"),
                       CompileOptionsBase_StringParam<&CompileOptionsBase::RemoveBuildScript>,
                       _SC("CompileOptionsBase::RemoveBuildScript"));
            BindMethod(v, _SC("SetVar"), CompileOptionsBase_SetVar,
                       _SC("CompileOptionsBase::SetVar"));
            BindMethod(v, _SC("GetVar"), CompileOptionsBase_GetVar,
                       _SC("CompileOptionsBase::GetVar"));
            BindMethod(v, _SC("UnsetVar"), CompileOptionsBase_UnsetVar,
                       _SC("CompileOptionsBase::UnsetVar"));
            BindMethod(v, _SC("UnsetAllVars"), CompileOptionsBase_UnsetAllVars,
                       _SC("CompileOptionsBase::UnsetAllVars"));

            BindDefaultInstanceCmp<CompileOptionsBase>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompileTargetBase
            const SQInteger classDecl = CreateClassDecl<CompileTargetBase>(v);
            BindMethod(v, _SC("SetTargetFilenameGenerationPolicy"),
                       CompileTargetBase_SetTargetFilenameGenerationPolicy,
                       _SC("CompileTargetBase::SetTargetFilenameGenerationPolicy"));
            // FIXME (squirrel) Add this to the wiki!
            BindMethod(v, _SC("GetTargetFilenameGenerationPolicy"),
                       CompileTargetBase_GetTargetFilenameGenerationPolicy,
                       _SC("CompileTargetBase::GetTargetFilenameGenerationPolicy"));
            BindMethod(v, _SC("GetFilename"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetFilename>,
                       _SC("CompileTargetBase::GetFilename"));
            BindMethod(v, _SC("GetTitle"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetTitle>,
                       _SC("CompileTargetBase::GetTitle"));
            BindMethod(v, _SC("SetTitle"), Generic_SetString<CompileTargetBase, &CompileTargetBase::SetTitle>,
                       _SC("CompileTargetBase::SetTitle"));
            BindMethod(v, _SC("SetOutputFilename"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetOutputFilename>,
                       _SC("CompileTargetBase::SetOutputFilename"));
            BindMethod(v, _SC("SetWorkingDir"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetWorkingDir>,
                       _SC("CompileTargetBase::SetWorkingDir"));
            BindMethod(v, _SC("SetObjectOutput"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetObjectOutput>,
                       _SC("CompileTargetBase::SetObjectOutput"));
            BindMethod(v, _SC("SetDepsOutput"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetDepsOutput>,
                       _SC("CompileTargetBase::SetDepsOutput"));
            BindMethod(v, _SC("GetOptionRelation"), CompileTargetBase_GetOptionRelation,
                       _SC("CompileTargetBase::GetOptionRelation"));
            BindMethod(v, _SC("SetOptionRelation"), CompileTargetBase_SetOptionRelation,
                       _SC("CompileTargetBase::SetOptionRelation"));
            BindMethod(v, _SC("GetWorkingDir"),
                       Generic_GetString<CompileTargetBase, &CompileTargetBase::GetWorkingDir>,
                       _SC("CompileTargetBase::GetWorkingDir"));
            BindMethod(v, _SC("GetObjectOutput"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetObjectOutput>,
                       _SC("CompileTargetBase::GetObjectOutput"));
            BindMethod(v, _SC("GetDepsOutput"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetDepsOutput>,
                       _SC("CompileTargetBase::GetDepsOutput"));
            BindMethod(v, _SC("GetOutputFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetOutputFilename>,
                       _SC("CompileTargetBase::GetOutputFilename"));
            BindMethod(v, _SC("SuggestOutputFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::SuggestOutputFilename>,
                       _SC("CompileTargetBase::SuggestOutputFilename"));
            BindMethod(v, _SC("GetExecutableFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetExecutableFilename>,
                       _SC("CompileTargetBase::GetExecutableFilename"));
            BindMethod(v, _SC("GetDynamicLibFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetDynamicLibFilename>,
                       _SC("CompileTargetBase::GetDynamicLibFilename"));
            BindMethod(v, _SC("GetDynamicLibDefFilename"),
                       Generic_GetString<CompileTargetBase, &CompileTargetBase::GetDynamicLibDefFilename>,
                       _SC("CompileTargetBase::GetDynamicLibDefFilename"));
            BindMethod(v, _SC("GetStaticLibFilename"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetStaticLibFilename>,
                       _SC("CompileTargetBase::GetStaticLibFilename"));
            BindMethod(v, _SC("GetBasePath"),
                       Generic_GetStringConst<CompileTargetBase, &CompileTargetBase::GetBasePath>,
                       _SC("CompileTargetBase::GetBasePath"));
            BindMethod(v, _SC("SetTargetType"), CompileTargetBase_SetTargetType,
                       _SC("CompileTargetBase::SetTargetType"));
            BindMethod(v, _SC("GetTargetType"), CompileTargetBase_GetTargetType,
                         _SC("CompileTargetBase::GetTargetType"));
            BindMethod(v, _SC("GetExecutionParameters"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetExecutionParameters>,
                       _SC("CompileTargetBase::GetExecutionParameters"));
            BindMethod(v, _SC("SetExecutionParameters"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetExecutionParameters>,
                       _SC("CompileTargetBase::SetExecutionParameters"));
            BindMethod(v, _SC("GetHostApplication"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetHostApplication>,
                       _SC("CompileTargetBase::GetHostApplication"));
            BindMethod(v, _SC("SetHostApplication"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetHostApplication>,
                       _SC("CompileTargetBase::SetHostApplication"));
            BindMethod(v, _SC("SetCompilerID"),
                       Generic_SetString<CompileTargetBase, &CompileTargetBase::SetCompilerID>,
                       _SC("CompileTargetBase::SetCompilerID"));
            BindMethod(v, _SC("GetCompilerID"),
                       Generic_GetCString<CompileTargetBase, &CompileTargetBase::GetCompilerID>,
                       _SC("CompileTargetBase::GetCompilerID"));
            BindMethod(v, _SC("GetMakeCommandFor"), CompileTargetBase_GetMakeCommandFor,
                       _SC("CompileTargetBase::GetMakeCommandFor"));
            BindMethod(v, _SC("SetMakeCommandFor"), CompileTargetBase_SetMakeCommandFor,
                       _SC("CompileTargetBase::SetMakeCommandFor"));
            BindMethod(v, _SC("MakeCommandsModified"),
                       Generic_GetBool<CompileTargetBase, &CompileTargetBase::MakeCommandsModified>,
                       _SC("CompileTargetBase::MakeCommandsModified"));

            BindDefaultInstanceCmp<CompileTargetBase>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectBuildTarget
            const SQInteger classDecl = CreateClassDecl<ProjectBuildTarget>(v);
            BindMethod(v, _SC("GetParentProject"), ProjectBuildTarget_GetParentProject,
                       _SC("ProjectBuildTarget::GetParentProject"));
            BindMethod(v, _SC("GetFullTitle"),
                       Generic_GetStringConst<ProjectBuildTarget, &ProjectBuildTarget::GetFullTitle>,
                       _SC("ProjectBuildTarget::GetFullTitle"));
            BindMethod(v, _SC("GetExternalDeps"),
                       Generic_GetCString<ProjectBuildTarget, &ProjectBuildTarget::GetExternalDeps>,
                       _SC("ProjectBuildTarget::GetExternalDeps"));
            BindMethod(v, _SC("SetExternalDeps"),
                       Generic_SetString<ProjectBuildTarget, &ProjectBuildTarget::SetExternalDeps>,
                       _SC("ProjectBuildTarget::SetExternalDeps"));
            BindMethod(v, _SC("SetAdditionalOutputFiles"),
                       Generic_SetString<ProjectBuildTarget, &ProjectBuildTarget::SetAdditionalOutputFiles>,
                       _SC("ProjectBuildTarget::SetAdditionalOutputFiles"));
            BindMethod(v, _SC("GetAdditionalOutputFiles"),
                       Generic_GetCString<ProjectBuildTarget, &ProjectBuildTarget::GetAdditionalOutputFiles>,
                       _SC("ProjectBuildTarget::GetAdditionalOutputFiles"));
            BindMethod(v, _SC("GetIncludeInTargetAll"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetIncludeInTargetAll>,
                       _SC("ProjectBuildTarget::GetIncludeInTargetAll"));
            BindMethod(v, _SC("SetIncludeInTargetAll"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetIncludeInTargetAll>,
                       _SC("ProjectBuildTarget::SetIncludeInTargetAll"));
            BindMethod(v, _SC("GetCreateDefFile"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetCreateDefFile>,
                       _SC("ProjectBuildTarget::GetCreateDefFile"));
            BindMethod(v, _SC("SetCreateDefFile"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetCreateDefFile>,
                       _SC("ProjectBuildTarget::SetCreateDefFile"));
            BindMethod(v, _SC("GetCreateStaticLib"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetCreateStaticLib>,
                       _SC("ProjectBuildTarget::GetCreateStaticLib"));
            BindMethod(v, _SC("SetCreateStaticLib"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetCreateStaticLib>,
                       _SC("ProjectBuildTarget::SetCreateStaticLib"));
            BindMethod(v, _SC("GetUseConsoleRunner"),
                       Generic_GetBool<ProjectBuildTarget, &ProjectBuildTarget::GetUseConsoleRunner>,
                       _SC("ProjectBuildTarget::GetUseConsoleRunner"));
            BindMethod(v, _SC("SetUseConsoleRunner"),
                       Generic_SetBool<ProjectBuildTarget, &ProjectBuildTarget::SetUseConsoleRunner>,
                       _SC("ProjectBuildTarget::SetUseConsoleRunner"));

            BindMethod(v, _SC("GetFilesCount"), Generic_GetFilesCount<ProjectBuildTarget>,
                       _SC("ProjectBuildTarget::GetFilesCount"));
            BindMethod(v, _SC("GetFile"), Generic_GetFile<ProjectBuildTarget>,
                       _SC("ProjectBuildTarget::GetFile"));

            BindDefaultInstanceCmp<ProjectBuildTarget>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register cbProject
            const SQInteger classDecl = CreateClassDecl<cbProject>(v);
            BindMethod(v, _SC("GetModified"), Generic_GetBool<cbProject, &cbProject::GetModified>,
                       _SC("cbProject::GetModified"));
            BindMethod(v, _SC("SetModified"), Generic_SetBool<cbProject, &cbProject::SetModified>,
                       _SC("cbProject::SetModified"));
            BindMethod(v, _SC("GetMakefile"), Generic_GetCString<cbProject, &cbProject::GetMakefile>,
                       _SC("cbProject::GetMakefile"));
            BindMethod(v, _SC("SetMakefile"), Generic_SetString<cbProject, &cbProject::SetMakefile>,
                       _SC("cbProject::SetMakefile"));
            BindMethod(v, _SC("IsMakefileCustom"), Generic_GetBool<cbProject, &cbProject::IsMakefileCustom>,
                       _SC("cbProject::IsMakefileCustom"));
            BindMethod(v, _SC("SetMakefileCustom"), Generic_SetBool<cbProject, &cbProject::SetMakefileCustom>,
                       _SC("cbProject::SetMakefileCustom"));
            BindMethod(v, _SC("CloseAllFiles"), cbProject_CloseAllFiles,
                       _SC("cbProject::CloseAllFiles"));
            BindMethod(v, _SC("SaveAllFiles"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::SaveAllFiles>,
                       _SC("cbProject::SaveAllFiles"));
            BindMethod(v, _SC("Save"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::Save>,
                       _SC("cbProject::Save"));
            // SaveAs is deemed unsafe, so it is not bound
            BindMethod(v, _SC("SaveLayout"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::SaveLayout>,
                       _SC("cbProject::SaveLayout"));
            BindMethod(v, _SC("LoadLayout"),
                       Generic_DoSomethingGetBool<cbProject, &cbProject::LoadLayout>,
                       _SC("cbProject::LoadLayout"));
            BindMethod(v, _SC("GetCommonTopLevelPath"),
                       Generic_GetStringConst<cbProject, &cbProject::GetCommonTopLevelPath>,
                       _SC("cbProject::GetCommonTopLevelPath"));
            BindMethod(v, _SC("GetFilesCount"), Generic_GetFilesCount<cbProject>,
                       _SC("cbProject::GetFilesCount"));
            BindMethod(v, _SC("GetFile"), Generic_GetFile<cbProject>, _SC("cbProject::GetFile"));
            BindMethod(v, _SC("GetFileByFilename"), cbProject_GetFileByFilename,
                       _SC("cbProject::GetFileByFilename"));
            BindMethod(v, _SC("RemoveFile"), cbProject_RemoveFile, _SC("cbProject::RemoveFile"));
            BindMethod(v, _SC("AddFile"), cbProject_AddFile, _SC("cbProject::AddFile"));
            BindMethod(v, _SC("GetBuildTargetsCount"), cbProject_GetBuildTargetsCount,
                       _SC("cbProject::GetBuildTargetsCount"));
            BindMethod(v, _SC("GetBuildTarget"), cbProject_GetBuildTarget,
                       _SC("cbProject::GetBuildTarget"));
            BindMethod(v, _SC("AddBuildTarget"), cbProject_AddBuildTarget,
                       _SC("cbProject::AddBuildTarget"));
            BindMethod(v, _SC("RenameBuildTarget"), cbProject_RenameBuildTarget,
                       _SC("cbProject::RenameBuildTarget"));
            BindMethod(v, _SC("DuplicateBuildTarget"), cbProject_DuplicateBuildTarget,
                       _SC("cbProject::DuplicateBuildTarget"));
            BindMethod(v, _SC("RemoveBuildTarget"), cbProject_RemoveBuildTarget,
                       _SC("cbProject::RemoveBuildTarget"));
            BindMethod(v, _SC("ExportTargetAsProject"), cbProject_ExportTargetAsProject,
                       _SC("cbProject::ExportTargetAsProject"));
            BindMethod(v, _SC("BuildTargetValid"), cbProject_BuildTargetValid,
                       _SC("cbProject::BuildTargetValid"));
            BindMethod(v, _SC("GetFirstValidBuildTargetName"),
                       cbProject_GetFirstValidBuildTargetName,
                       _SC("cbProject::GetFirstValidBuildTargetName"));
            BindMethod(v, _SC("SetDefaultExecuteTarget"),
                       Generic_SetString<cbProject, &cbProject::SetDefaultExecuteTarget>,
                       _SC("cbProject::SetDefaultExecuteTarget"));
            BindMethod(v, _SC("GetDefaultExecuteTarget"),
                       Generic_GetCString<cbProject, &cbProject::GetDefaultExecuteTarget>,
                       _SC("cbProject::GetDefaultExecuteTarget"));
            BindMethod(v, _SC("SetActiveBuildTarget"), cbProject_SetActiveBuildTarget,
                       _SC("cbProject::SetActiveBuildTarget"));
            BindMethod(v, _SC("GetActiveBuildTarget"),
                       Generic_GetCString<cbProject, &cbProject::GetActiveBuildTarget>,
                       _SC("cbProject::GetActiveBuildTarget"));
            BindMethod(v, _SC("SelectTarget"), cbProject_SelectTarget,
                       _SC("cbProject::SelectTarget"));
            BindMethod(v, _SC("GetCurrentlyCompilingTarget"), cbProject_GetCurrentlyCompilingTarget,
                       _SC("cbProject::GetCurrentlyCompilingTarget"));
            BindMethod(v, _SC("SetCurrentlyCompilingTarget"), cbProject_SetCurrentlyCompilingTarget,
                       _SC("cbProject::SetCurrentlyCompilingTarget"));
            BindMethod(v, _SC("GetModeForPCH"), cbProject_GetModeForPCH,
                       _SC("cbProject::GetModeForPCH"));
            BindMethod(v, _SC("SetModeForPCH"), cbProject_SetModeForPCH,
                       _SC("cbProject::SetModeForPCH"));
            BindMethod(v, _SC("SetExtendedObjectNamesGeneration"),
                       Generic_SetBool<cbProject, &cbProject::SetExtendedObjectNamesGeneration>,
                       _SC("cbProject::SetExtendedObjectNamesGeneration"));
            BindMethod(v, _SC("GetExtendedObjectNamesGeneration"),
                       Generic_GetBool<cbProject, &cbProject::GetExtendedObjectNamesGeneration>,
                       _SC("cbProject::GetExtendedObjectNamesGeneration"));
            BindMethod(v, _SC("SetNotes"), Generic_SetString<cbProject, &cbProject::SetNotes>,
                       _SC("cbProject::SetNotes"));
            BindMethod(v, _SC("GetNotes"), Generic_GetCString<cbProject, &cbProject::GetNotes>,
                       _SC("cbProject::GetNotes"));
            BindMethod(v, _SC("SetShowNotesOnLoad"),
                       Generic_SetBool<cbProject, &cbProject::SetShowNotesOnLoad>,
                       _SC("cbProject::SetShowNotesOnLoad"));
            BindMethod(v, _SC("GetShowNotesOnLoad"),
                       Generic_GetBool<cbProject, &cbProject::GetShowNotesOnLoad>,
                       _SC("cbProject::GetShowNotesOnLoad"));
            BindMethod(v, _SC("SetCheckForExternallyModifiedFiles"),
                       Generic_SetBool<cbProject, &cbProject::SetCheckForExternallyModifiedFiles>,
                       _SC("cbProject::SetCheckForExternallyModifiedFiles"));
            BindMethod(v, _SC("GetCheckForExternallyModifiedFiles"),
                       Generic_GetBool<cbProject, &cbProject::GetCheckForExternallyModifiedFiles>,
                       _SC("cbProject::GetCheckForExternallyModifiedFiles"));
            BindMethod(v, _SC("ShowNotes"), cbProject_ShowNotes, _SC("cbProject::ShowNotes"));
            BindMethod(v, _SC("AddToExtensions"),
                       Generic_SetString<cbProject, &cbProject::AddToExtensions>,
                       _SC("cbProject::AddToExtensions"));
            BindMethod(v, _SC("ExtensionListNodes"), cbProject_ExtensionListNodes,
                       _SC("cbProject::ExtensionListNodes"));
            BindMethod(v, _SC("ExtensionListNodeAttributes"), cbProject_ExtensionListNodeAttributes,
                       _SC("cbProject::ExtensionListNodeAttributes"));
            BindMethod(v, _SC("ExtensionGetNodeAttribute"),
                       cbProject_ExtensionGetNodeAttribute,
                       _SC("cbProject::ExtensionGetNodeAttribute"));
            BindMethod(v, _SC("ExtensionSetNodeAttribute"), cbProject_ExtensionSetNodeAttribute,
                       _SC("cbProject::ExtensionSetNodeAttribute"));
            BindMethod(v, _SC("ExtensionRemoveNodeAttribute"),
                       cbProject_ExtensionRemoveNodeAttribute,
                       _SC("cbProject::ExtensionRemoveNodeAttribute"));
            BindMethod(v, _SC("ExtensionAddNode"), cbProject_ExtensionAddNode,
                       _SC("cbProject::ExtensionAddNode"));
            BindMethod(v, _SC("ExtensionRemoveNode"), cbProject_ExtensionRemoveNode,
                       _SC("cbProject::ExtensionRemoveNode"));
            BindMethod(v, _SC("DefineVirtualBuildTarget"), cbProject_DefineVirtualBuildTarget,
                       _SC("cbProject::DefineVirtualBuildTarget"));
            BindMethod(v, _SC("HasVirtualBuildTarget"), cbProject_HasVirtualBuildTarget,
                       _SC("cbProject::HasVirtualBuildTarget"));
            BindMethod(v, _SC("RemoveVirtualBuildTarget"), cbProject_RemoveVirtualBuildTarget,
                       _SC("cbProject::RemoveVirtualBuildTarget"));
            BindMethod(v, _SC("GetVirtualBuildTargets"), cbProject_GetVirtualBuildTargets,
                       _SC("cbProject::GetVirtualBuildTargets"));
            BindMethod(v, _SC("GetVirtualBuildTargetGroup"), cbProject_GetVirtualBuildTargetGroup,
                       _SC("cbProject::GetVirtualBuildTargetGroup"));
            BindMethod(v, _SC("GetExpandedVirtualBuildTargetGroup"),
                       cbProject_GetExpandedVirtualBuildTargetGroup,
                       _SC("cbProject::GetExpandedVirtualBuildTargetGroup"));
            BindMethod(v, _SC("CanAddToVirtualBuildTarget"), cbProject_CanAddToVirtualBuildTarget,
                       _SC("cbProject::CanAddToVirtualBuildTarget"));

            BindDefaultInstanceCmp<cbProject>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ProjectManager
            const SQInteger classDecl = CreateClassDecl<ProjectManager>(v);
            BindMethod(v, _SC("GetDefaultPath"), ProjectManager_GetDefaultPath,
                       _SC("ProjectManager::GetDefaultPath"));
            BindMethod(v, _SC("SetDefaultPath"), ProjectManager_SetDefaultPath,
                       _SC("ProjectManager::SetDefaultPath"));
            BindMethod(v, _SC("GetActiveProject"), ProjectManager_GetActiveProject,
                       _SC("ProjectManager::GetActiveProject"));
            BindMethod(v, _SC("GetProjectCount"), ProjectManager_GetProjectCount,
                       _SC("ProjectManager::GetProjectCount"));
            BindMethod(v, _SC("GetProject"), ProjectManager_GetProject,
                       _SC("ProjectManager::GetProject"));
            BindMethod(v, _SC("SetProject"), ProjectManager_SetProject,
                       _SC("ProjectManager::SetProject"));
            BindMethod(v, _SC("LoadWorkspace"), ProjectManager_LoadWorkspace,
                       _SC("ProjectManager::LoadWorkspace"));
            BindMethod(v, _SC("SaveWorkspace"), ProjectManager_SaveWorkspace,
                       _SC("ProjectManager::SaveWorkspace"));
            BindMethod(v, _SC("SaveWorkspaceAs"), ProjectManager_SaveWorkspaceAs,
                       _SC("ProjectManager::SaveWorkspaceAs"));
            BindMethod(v, _SC("CloseWorkspace"), ProjectManager_CloseWorkspace,
                       _SC("ProjectManager::CloseWorkspace"));
            BindMethod(v, _SC("IsOpen"), ProjectManager_IsOpen, _SC("ProjectManager::IsOpen"));
            BindMethod(v, _SC("LoadProject"), ProjectManager_LoadProject,
                       _SC("ProjectManager::LoadProject"));
            BindMethod(v, _SC("SaveProject"),
                       ProjectManager_SaveProject<&ProjectManager::SaveProject>,
                       _SC("ProjectManager::SaveProject"));
            BindMethod(v, _SC("SaveProjectAs"),
                       ProjectManager_SaveProject<&ProjectManager::SaveProjectAs>,
                       _SC("ProjectManager::SaveProjectAs"));
            BindMethod(v, _SC("SaveActiveProject"),
                       ProjectManager_DoSomethingReturnBool<&ProjectManager::SaveActiveProject>,
                       _SC("ProjectManager::SaveActiveProject"));
            BindMethod(v, _SC("SaveActiveProjectAs"),
                       ProjectManager_DoSomethingReturnBool<&ProjectManager::SaveActiveProjectAs>,
                       _SC("ProjectManager::SaveActiveProjectAs"));
            BindMethod(v, _SC("SaveAllProjects"),
                       ProjectManager_DoSomethingReturnBool<&ProjectManager::SaveAllProjects>,
                       _SC("ProjectManager::SaveAllProjects"));
            BindMethod(v, _SC("CloseProject"), ProjectManager_CloseProject,
                       _SC("ProjectManager::CloseProject"));
            BindMethod(v, _SC("CloseActiveProject"),
                       ProjectManager_CloseDontSave<&ProjectManager::CloseActiveProject>,
                       _SC("ProjectManager::CloseActiveProject"));
            BindMethod(v, _SC("CloseAllProjects"),
                       ProjectManager_CloseDontSave<&ProjectManager::CloseAllProjects>,
                       _SC("ProjectManager::CloseAllProjects"));
            BindMethod(v, _SC("NewProject"), ProjectManager_NewProject,
                       _SC("ProjectManager::NewProject"));
            BindMethod(v, _SC("AddFileToProject"), ProjectManager_AddFileToProject,
                       _SC("ProjectManager::AddFileToProject"));
            BindMethod(v, _SC("AddProjectDependency"), ProjectManager_AddProjectDependency,
                       _SC("ProjectManager::AddProjectDependency"));
            BindMethod(v, _SC("RemoveProjectDependency"), ProjectManager_RemoveProjectDependency,
                       _SC("ProjectManager::RemoveProjectDependency"));
            BindMethod(v, _SC("ClearProjectDependencies"), ProjectManager_ClearProjectDependencies,
                       _SC("ProjectManager::ClearProjectDependencies"));
            BindMethod(v, _SC("RemoveProjectFromAllDependencies"),
                       ProjectManager_RemoveProjectFromAllDependencies,
                       _SC("ProjectManager::RemoveProjectFromAllDependencies"));
            BindMethod(v, _SC("GetDependenciesForProject"),
                       ProjectManager_GetDependenciesForProject,
                       _SC("ProjectManager::GetDependenciesForProject"));
            BindMethod(v, _SC("RebuildTree"), ProjectManager_RebuildTree,
                       _SC("ProjectManager::RebuildTree"));

            BindDefaultInstanceCmp<ProjectManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditorBase
            const SQInteger classDecl = CreateClassDecl<EditorBase>(v);
            BindMethod(v, _SC("GetFilename"),
                       Generic_GetCString<EditorBase, &EditorBase::GetFilename>,
                       _SC("EditorBase::GetFilename"));
            BindMethod(v, _SC("SetFilename"),
                       Generic_SetString<EditorBase, &EditorBase::SetFilename>,
                       _SC("EditorBase::SetFilename"));
            BindMethod(v, _SC("GetShortName"),
                       Generic_GetCString<EditorBase, &EditorBase::GetShortName>,
                       _SC("EditorBase::GetShortName"));
            BindMethod(v, _SC("GetModified"), Generic_GetBool<EditorBase, &EditorBase::GetModified>,
                       _SC("EditorBase::GetModified"));
            BindMethod(v, _SC("SetModified"), Generic_SetBool<EditorBase, &EditorBase::SetModified>,
                       _SC("EditorBase::SetModified"));
            BindMethod(v, _SC("GetTitle"), Generic_GetCString<EditorBase, &EditorBase::GetTitle>,
                       _SC("EditorBase::GetTitle"));
            BindMethod(v, _SC("SetTitle"), Generic_SetString<EditorBase, &EditorBase::SetTitle>,
                       _SC("EditorBase::SetTitle"));
            BindMethod(v, _SC("Activate"), EditorBase_Activate, _SC("EditorBase::Activate"));
            BindMethod(v, _SC("Close"), EditorBase_Close, _SC("EditorBase::Close"));
            BindMethod(v, _SC("Save"), Generic_DoSomethingGetBool<EditorBase, &EditorBase::Save>,
                       _SC("EditorBase::Save"));
            // FIXME (squirrel) Add this in the wiki
            BindMethod(v, _SC("SaveAs"),
                       Generic_DoSomethingGetBool<EditorBase, &EditorBase::SaveAs>,
                       _SC("EditorBase::SaveAs"));
            BindMethod(v, _SC("IsBuiltinEditor"),
                       Generic_GetBool<EditorBase, &EditorBase::IsBuiltinEditor>,
                       _SC("EditorBase::IsBuiltinEditor"));
            BindMethod(v, _SC("ThereAreOthers"),
                       Generic_GetBool<EditorBase, &EditorBase::ThereAreOthers>,
                       _SC("EditorBase::ThereAreOthers"));
            BindMethod(v, _SC("GotoLine"), EditorBase_GotoLine, _SC("EditorBase::GotoLine"));
            BindMethod(v, _SC("Undo"), Generic_DoSomethingGetVoid<EditorBase, &EditorBase::Undo>,
                       _SC("EditorBase::Undo"));
            BindMethod(v, _SC("Redo"), Generic_DoSomethingGetVoid<EditorBase, &EditorBase::Redo>,
                       _SC("EditorBase::Redo"));
            BindMethod(v, _SC("Cut"), Generic_DoSomethingGetVoid<EditorBase, &EditorBase::Cut>,
                       _SC("EditorBase::Cut"));
            BindMethod(v, _SC("Copy"), Generic_DoSomethingGetVoid<EditorBase, &EditorBase::Copy>,
                       _SC("EditorBase::Copy"));
            BindMethod(v, _SC("Paste"), Generic_DoSomethingGetVoid<EditorBase, &EditorBase::Paste>,
                       _SC("EditorBase::Paste"));
            BindMethod(v, _SC("CanUndo"), Generic_GetBool<EditorBase, &EditorBase::CanUndo>,
                       _SC("EditorBase::CanUndo"));
            BindMethod(v, _SC("CanRedo"), Generic_GetBool<EditorBase, &EditorBase::CanRedo>,
                       _SC("EditorBase::CanRedo"));
            BindMethod(v, _SC("CanPaste"), Generic_GetBool<EditorBase, &EditorBase::CanPaste>,
                       _SC("EditorBase::CanPaste"));
            BindMethod(v, _SC("IsReadOnly"), Generic_GetBool<EditorBase, &EditorBase::IsReadOnly>,
                       _SC("EditorBase::IsReadOnly"));
            BindMethod(v, _SC("HasSelection"),
                       Generic_GetBool<EditorBase, &EditorBase::HasSelection>,
                       _SC("EditorBase::HasSelection"));

            BindDefaultInstanceCmp<EditorBase>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register cbEditor
            const SQInteger classDecl = CreateClassDecl<cbEditor>(v);
            BindMethod(v, _SC("SetEditorTitle"),
                       Generic_SetString<cbEditor, &cbEditor::SetEditorTitle>,
                       _SC("cbEditor::SetEditorTitle"));
            BindMethod(v, _SC("GetProjectFile"), cbEditor_GetProjectFile,
                       _SC("cbEditor::GetProjectFile"));
            BindMethod(v, _SC("Save"), Generic_DoSomethingGetBool<cbEditor, &cbEditor::Save>,
                       _SC("cbEditor::Save"));
            BindMethod(v, _SC("SaveAs"), Generic_DoSomethingGetBool<cbEditor, &cbEditor::SaveAs>,
                       _SC("cbEditor::SaveAs"));
            BindMethod(v, _SC("FoldAll"), Generic_DoSomethingGetVoid<cbEditor, &cbEditor::FoldAll>,
                       _SC("cbEditor::FoldAll"));
            BindMethod(v, _SC("UnfoldAll"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::UnfoldAll>,
                       _SC("cbEditor::UnfoldAll"));
            BindMethod(v, _SC("ToggleAllFolds"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::ToggleAllFolds>,
                       _SC("cbEditor::ToggleAllFolds"));
            BindMethod(v, _SC("FoldBlockFromLine"),
                       cbEditor_DoFromLine<&cbEditor::FoldBlockFromLine>,
                       _SC("cbEditor::FoldBlockFromLine"));
            BindMethod(v, _SC("UnfoldBlockFromLine"),
                       cbEditor_DoFromLine<&cbEditor::UnfoldBlockFromLine>,
                       _SC("cbEditor::UnfoldBlockFromLine"));
            BindMethod(v, _SC("ToggleFoldBlockFromLine"),
                       cbEditor_DoFromLine<&cbEditor::ToggleFoldBlockFromLine>,
                       _SC("cbEditor::ToggleFoldBlockFromLine"));
            BindMethod(v, _SC("GetLineIndentInSpaces"), cbEditor_GetLineIndentInSpaces,
                       _SC("cbEditor::GetLineIndentInSpaces"));
            BindMethod(v, _SC("GetLineIndentString"), cbEditor_GetLineIndentString,
                       _SC("cbEditor::GetLineIndentString"));
            BindMethod(v, _SC("Touch"), Generic_DoSomethingGetVoid<cbEditor, &cbEditor::Touch>,
                       _SC("cbEditor::Touch"));
            BindMethod(v, _SC("Reload"), cbEditor_Reload, _SC("cbEditor::Reload"));
            BindMethod(v, _SC("Print"), cbEditor_Print, _SC("cbEditor::Print"));
            BindMethod(v, _SC("AutoComplete"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::AutoComplete>,
                       _SC("cbEditor::AutoComplete"));
            BindMethod(v, _SC("AddBreakpoint"), cbEditor_AddBreakpoint,
                       _SC("cbEditor::AddBreakpoint"));
            BindMethod(v, _SC("RemoveBreakpoint"), cbEditor_RemoveBreakpoint,
                       _SC("cbEditor::RemoveBreakpoint"));
            BindMethod(v, _SC("ToggleBreakpoint"), cbEditor_ToggleBreakpoint,
                       _SC("cbEditor::ToggleBreakpoint"));
            BindMethod(v, _SC("HasBreakpoint"), cbEditor_HasBreakpoint,
                       _SC("cbEditor::HasBreakpoint"));
            BindMethod(v, _SC("HasBookmark"), cbEditor_HasBookmark, _SC("cbEditor::HasBookmark"));
            BindMethod(v, _SC("ToggleBookmark"), cbEditor_ToggleBookmark,
                       _SC("cbEditor::ToggleBookmark"));
            BindMethod(v, _SC("GotoNextBookmark"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::GotoNextBookmark>,
                       _SC("cbEditor::GotoNextBookmark"));
            BindMethod(v, _SC("GotoPreviousBookmark"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::GotoPreviousBookmark>,
                       _SC("cbEditor::GotoPreviousBookmark"));
            BindMethod(v, _SC("ClearAllBookmarks"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::ClearAllBookmarks>,
                       _SC("cbEditor::ClearAllBookmarks"));
            BindMethod(v, _SC("GotoNextBreakpoint"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::GotoNextBreakpoint>,
                       _SC("cbEditor::GotoNextBreakpoint"));
            BindMethod(v, _SC("GotoPreviousBreakpoint"),
                       Generic_DoSomethingGetVoid<cbEditor, &cbEditor::GotoPreviousBreakpoint>,
                       _SC("cbEditor::GotoPreviousBreakpoint"));
            // These are not present in cbEditor. Included to help scripts edit text.
            BindMethod(v, _SC("SetText"), cbEditor_SetText, _SC("cbEditor::SetText"));
            BindMethod(v, _SC("GetText"), cbEditor_GetText, _SC("cbEditor::GetText"));

            BindDefaultInstanceCmp<cbEditor>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditorManager
            const SQInteger classDecl = CreateClassDecl<EditorManager>(v);
            BindMethod(v, _SC("New"), EditorManager_FilenameTocbEditor<&EditorManager::New>,
                       _SC("EditorManager::New"));
            BindMethod(v, _SC("Open"), EditorManager_Open, _SC("EditorManager::Open"));
            BindMethod(v, _SC("IsBuiltinOpen"),
                       EditorManager_FilenameTocbEditor<&EditorManager::IsBuiltinOpen>,
                       _SC("EditorManager::IsBuiltinOpen"));
            BindMethod(v, _SC("GetBuiltinEditor"), EditorManager_GetBuiltinEditor,
                       _SC("EditorManager::GetBuiltinEditor"));
            BindMethod(v, _SC("GetBuiltinActiveEditor"), EditorManager_GetBuiltinActiveEditor,
                       _SC("EditorManager::GetBuiltinActiveEditor"));
            BindMethod(v, _SC("GetActiveEditor"), EditorManager_GetActiveEditor,
                       _SC("EditorManager::GetActiveEditor"));
            BindMethod(v, _SC("ActivateNext"),
                       Generic_DoSomethingGetVoid<EditorManager, &EditorManager::ActivateNext>,
                       _SC("EditorManager::ActivateNext"));
            BindMethod(v, _SC("ActivatePrevious"),
                       Generic_DoSomethingGetVoid<EditorManager, &EditorManager::ActivatePrevious>,
                       _SC("EditorManager::ActivatePrevious"));
            BindMethod(v, _SC("SwapActiveHeaderSource"), EditorManager_SwapActiveHeaderSource,
                       _SC("EditorManager::SwapActiveHeaderSource"));
            BindMethod(v, _SC("CloseActive"),
                       Editor_ParamBoolReturnBool<&EditorManager::CloseActive>,
                       _SC("EditorManager::CloseActive"));
            BindMethod(v, _SC("Close"), EditorManager_Close, _SC("EditorManager::Close"));
            BindMethod(v, _SC("CloseAll"), Editor_ParamBoolReturnBool<&EditorManager::CloseAll>,
                       _SC("EditorManager::CloseAll"));
            BindMethod(v, _SC("Save"), EditorManager_Save, _SC("EditorManager::Save"));
            BindMethod(v, _SC("SaveActive"),
                       Editor_DoSomethingReturnBool<&EditorManager::SaveActive>,
                       _SC("EditorManager::SaveActive"));
            BindMethod(v, _SC("SaveAs"), EditorManager_SaveAs, _SC("EditorManager::SaveAs"));
            BindMethod(v, _SC("SaveActiveAs"),
                       Editor_DoSomethingReturnBool<&EditorManager::SaveActiveAs>,
                       _SC("EditorManager::SaveActiveAs"));
            BindMethod(v, _SC("SaveAll"),
                       Editor_DoSomethingReturnBool<&EditorManager::SaveAll>,
                       _SC("EditorManager::SaveAll"));

            BindDefaultInstanceCmp<EditorManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register UserVariableManager
            const SQInteger classDecl = CreateClassDecl<UserVariableManager>(v);
            BindMethod(v, _SC("Exists"), UserVariableManager_Exists, _SC("UserVariableManager::Exists"));

            BindDefaultInstanceCmp<UserVariableManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register ScriptingManager
            const SQInteger classDecl = CreateClassDecl<ScriptingManager>(v);
            BindMethod(v, _SC("RegisterScriptMenu"), ScriptingManager_RegisterScriptMenu,
                       _SC("ScriptingManager::RegisterScriptMenu"));

            BindDefaultInstanceCmp<ScriptingManager>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register CompilerFactory
            const SQInteger classDecl = CreateClassDecl<CompilerFactory>(v);
            BindStaticMethod(v, _SC("IsValidCompilerID"), CompilerFactory_IsValidCompilerID,
                             _SC("CompilerFactory::IsValidCompilerID"));
            BindStaticMethod(v, _SC("GetCompilerIndex"), CompilerFactory_GetCompilerIndex,
                             _SC("CompilerFactory::GetCompilerIndex"));
            BindStaticMethod(v, _SC("GetDefaultCompilerID"), CompilerFactory_GetDefaultCompilerID,
                             _SC("CompilerFactory::GetDefaultCompilerID"));
            BindStaticMethod(v, _SC("GetCompilerVersionString"),
                             CompilerFactory_GetCompilerVersionString,
                             _SC("CompilerFactory::GetCompilerVersionString"));
            BindStaticMethod(v, _SC("CompilerInheritsFrom"), CompilerFactory_CompilerInheritsFrom,
                             _SC("CompilerFactory::CompilerInheritsFrom"));
            BindStaticMethod(v, _SC("GetCompilerIDByName"), CompilerFactory_GetCompilerIDByName,
                             _SC("CompilerFactory::GetCompilerIDByName"));

            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register PluginInfo
            const SQInteger classDecl = CreateClassDecl<PluginInfo>(v);
            BindEmptyCtor<PluginInfo>(v);

            MembersType<PluginInfo> &members = BindMembers<PluginInfo>(v);
            addMemberRef(members, _SC("name"), &PluginInfo::name);
            addMemberRef(members, _SC("title"), &PluginInfo::title);
            addMemberRef(members, _SC("version"), &PluginInfo::version);
            addMemberRef(members, _SC("description"), &PluginInfo::description);
            addMemberRef(members, _SC("author"), &PluginInfo::author);
            addMemberRef(members, _SC("authorEmail"), &PluginInfo::authorEmail);
            addMemberRef(members, _SC("authorWebsite"), &PluginInfo::authorWebsite);
            addMemberRef(members, _SC("thanksTo"), &PluginInfo::thanksTo);
            addMemberRef(members, _SC("license"), &PluginInfo::license);

            BindDefaultInstanceCmp<PluginInfo>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register FileTreeData
            const SQInteger classDecl = CreateClassDecl<FileTreeData>(v);

            BindMethod(v, _SC("GetKind"), FileTreeData_GetKind, _SC("FileTreeData::GetKind"));
            BindMethod(v, _SC("GetProject"), FileTreeData_GetProject,
                       _SC("FileTreeData::GetProject"));
            BindMethod(v, _SC("GetFileIndex"), FileTreeData_GetFileIndex,
                       _SC("FileTreeData::GetFileIndex"));
            BindMethod(v, _SC("GetProjectFile"), FileTreeData_GetProjectFile,
                       _SC("FileTreeData::GetProjectFile"));
            BindMethod(v, _SC("GetFolder"), FileTreeData_GetFolder,
                       _SC("FileTreeData::GetFolder"));
            BindMethod(v, _SC("SetKind"), FileTreeData_SetKind, _SC("FileTreeData::SetKind"));
            BindMethod(v, _SC("SetProject"), FileTreeData_SetProject,
                       _SC("FileTreeData::SetProject"));
            BindMethod(v, _SC("SetFileIndex"), FileTreeData_SetFileIndex,
                       _SC("FileTreeData::SetFileIndex"));
            BindMethod(v, _SC("SetProjectFile"), FileTreeData_SetProjectFile,
                       _SC("FileTreeData::SetProjectFile"));
            BindMethod(v, _SC("SetFolder"), FileTreeData_SetFolder,
                       _SC("FileTreeData::SetFolder"));

            BindDefaultInstanceCmp<FileTreeData>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        sq_pop(v, 1); // Pop root table.

        // called last because it needs a few previously registered types
        Register_ScriptPlugin(v, manager);
    }

    void UnregisterBindings()
    {
        Unregister_ScriptPlugin();
    }
} // namespace ScriptBindings
