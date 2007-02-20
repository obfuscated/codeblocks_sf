#ifndef SC_BASE_TYPES_H
#define SC_BASE_TYPES_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <globals.h>
#include <settings.h>
#include <manager.h>
#include <messagemanager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include <uservarmanager.h>
#include <pluginmanager.h>
#include <scriptingmanager.h>
#include <compilerfactory.h>
#include <compiletargetbase.h>
#include <cbproject.h>
#include <cbeditor.h>
#include <sqplus.h>

DECLARE_INSTANCE_TYPE(wxString);
DECLARE_INSTANCE_TYPE(wxArrayString);
DECLARE_INSTANCE_TYPE(wxFileName);
DECLARE_INSTANCE_TYPE(wxColour);
DECLARE_INSTANCE_TYPE(ConfigManager);
DECLARE_INSTANCE_TYPE(EditorManager);
DECLARE_INSTANCE_TYPE(UserVariableManager);
DECLARE_INSTANCE_TYPE(ScriptingManager);
DECLARE_INSTANCE_TYPE(EditorBase);
DECLARE_INSTANCE_TYPE(cbEditor);
DECLARE_INSTANCE_TYPE(CompileOptionsBase);
DECLARE_INSTANCE_TYPE(CompileTargetBase);
DECLARE_INSTANCE_TYPE(ProjectBuildTarget);
DECLARE_INSTANCE_TYPE(cbProject);
DECLARE_INSTANCE_TYPE(ProjectFile);
DECLARE_INSTANCE_TYPE(ProjectManager);
DECLARE_INSTANCE_TYPE(CompilerFactory);
DECLARE_INSTANCE_TYPE(PluginInfo);
DECLARE_INSTANCE_TYPE(FileTreeData);

// make SqPlus aware of enum-type arguments
#define DECLARE_ENUM_TYPE(T) \
namespace SqPlus \
{ \
    inline void Push(HSQUIRRELVM v,T value) { sq_pushinteger(v,value); } \
    inline bool Match(TypeWrapper<T>, HSQUIRRELVM v, int idx) { return sq_gettype(v,idx) == OT_INTEGER; } \
    inline T Get(TypeWrapper<T>,HSQUIRRELVM v,int idx) { SQInteger i; SQPLUS_CHECK_GET(sq_getinteger(v,idx,&i)); return (T)i; } \
}

DECLARE_ENUM_TYPE(wxPathFormat);
DECLARE_ENUM_TYPE(wxPathNormalize);
DECLARE_ENUM_TYPE(PrintColourMode);
DECLARE_ENUM_TYPE(OptionsRelation);
DECLARE_ENUM_TYPE(OptionsRelationType);
DECLARE_ENUM_TYPE(TargetType);
DECLARE_ENUM_TYPE(PCHMode);
DECLARE_ENUM_TYPE(MakeCommand);
DECLARE_ENUM_TYPE(TemplateOutputType);
DECLARE_ENUM_TYPE(SearchDirs);
DECLARE_ENUM_TYPE(ModuleType);
DECLARE_ENUM_TYPE(FileTreeData::FileTreeDataKind);

namespace SqPlus
{
    // make SqPlus aware of wxString arguments
//    inline void Push(HSQUIRRELVM v,const wxString& value) { sq_pushstring(v,cbU2C(value),-1); }
//    inline bool Match(TypeWrapper<const wxString&>, HSQUIRRELVM v, int idx) { return sq_gettype(v,idx) == OT_STRING; }
//    inline wxString Get(TypeWrapper<const wxString&>,HSQUIRRELVM v,int idx) { const SQChar * s; SQPLUS_CHECK_GET(sq_getstring(v,idx,&s)); return cbC2U(s); }

    // type info for short unsigned int (missing from SqPlus)
    template<>
    struct TypeInfo<unsigned short>
    {
        const SQChar * typeName;
        TypeInfo() : typeName(sqT("int")) {}
        enum {TypeID=VAR_TYPE_INT,Size=sizeof(unsigned short)};
        operator ScriptVarType() { return ScriptVarType(TypeID); }
    };
};

#endif // SC_BASE_TYPES_H
