#include <sdk_precomp.h>
#include <settings.h>
#include <manager.h>
#include <messagemanager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include <compilerfactory.h>
#include <cbproject.h>
#include <cbeditor.h>
#include "scriptbindings.h"
#include "sc_wxstring.h"
#include "sc_wxarraystring.h"
#include "sc_io.h"
#include "const_bindings.h"
#include <globals.h>

#ifdef offsetof
    #undef offsetof
#endif
#define offsetof(T, M) ( reinterpret_cast <size_t> ( & reinterpret_cast <const volatile char &>(reinterpret_cast<T *> (1000)->M) ) - 1000u)

// In Code::Blocks nothing is refcounted.
// In order to use objects as handles (aka pointers), we must provide
// these two dummy functions.
#define ADD_DUMMY_REFCOUNT(engine,api) \
    engine->RegisterObjectBehaviour(#api, asBEHAVE_ADDREF, "void f()", asFUNCTION(DummyAddRef), asCALL_CDECL_OBJLAST); \
    engine->RegisterObjectBehaviour(#api, asBEHAVE_RELEASE, "void f()", asFUNCTION(DummyRelease), asCALL_CDECL_OBJLAST)

void DummyAddRef(cbProject& p){}
void DummyRelease(cbProject& p){}

//------------------------------------------------------------------------------
// Forwards
//------------------------------------------------------------------------------
void Register_ConfigManager(asIScriptEngine* engine);
void Register_Editor(asIScriptEngine* engine);
void Register_EditorManager(asIScriptEngine* engine);
void Register_ProjectFile(asIScriptEngine* engine);
void Register_ProjectBuildTarget(asIScriptEngine* engine);
void Register_Project(asIScriptEngine* engine);
void Register_ProjectManager(asIScriptEngine* engine);
void Register_CompilerFactory(asIScriptEngine* engine);

template <class T> void Register_CompileOptionsBase(asIScriptEngine* engine, const wxString& classname);

//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------
// message functions
int gMessage(const wxString& msg, const wxString& caption, int buttons){ return cbMessageBox(msg, caption, buttons); }
void gShowMessage(const wxString& msg){ cbMessageBox(msg, _("Script message")); }
void gShowMessageWarn(const wxString& msg){ cbMessageBox(msg, _("Script message (warning)"), wxICON_WARNING); }
void gShowMessageError(const wxString& msg){ cbMessageBox(msg, _("Script message (error)"), wxICON_ERROR); }
void gShowMessageInfo(const wxString& msg){ cbMessageBox(msg, _("Script message (information)"), wxICON_INFORMATION); }
void gDebugLog(const wxString& msg){ DBGLOG(msg); }

// macros
wxString gReplaceMacros(const wxString& buffer, bool envVarsToo)
{
    return Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer, envVarsToo);
}

// casts
// (help scripts to cast to base types)
CompileOptionsBase* gCastToCompileOptionsBase(cbProject* p){ return reinterpret_cast<CompileOptionsBase*>(p); }
CompileOptionsBase* gCastToCompileOptionsBase(ProjectBuildTarget* p){ return reinterpret_cast<CompileOptionsBase*>(p); }

//------------------------------------------------------------------------------
// Actual registration
//------------------------------------------------------------------------------
void RegisterBindings(asIScriptEngine* engine)
{
    // register wx types in script
    Register_wxString(engine);
    Register_wxArrayString(engine);

    // IO is enabled, but just for harmless functions
    Register_IO(engine);

    // register types
    engine->RegisterObjectType("Editor", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, Editor);
    engine->RegisterObjectType("ProjectFile", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, ProjectFile);
    engine->RegisterObjectType("CompileOptionsBase", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, CompileOptionsBase);
    engine->RegisterObjectType("BuildTarget", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, BuildTarget);
    engine->RegisterObjectType("Project", 0, asOBJ_CLASS);
    ADD_DUMMY_REFCOUNT(engine, Project);
    engine->RegisterObjectType("ProjectManagerClass", 0, asOBJ_CLASS);
    engine->RegisterObjectType("EditorManagerClass", 0, asOBJ_CLASS);
    engine->RegisterObjectType("ConfigManagerClass", 0, asOBJ_CLASS);
    engine->RegisterObjectType("CompilerFactoryClass", 0, asOBJ_CLASS);

    // register member functions
    Register_ConfigManager(engine);
    Register_Editor(engine);
    Register_EditorManager(engine);
    Register_ProjectFile(engine);
    Register_CompileOptionsBase<CompileOptionsBase>(engine, _T("CompileOptionsBase"));
    Register_ProjectBuildTarget(engine);
    Register_Project(engine);
    Register_ProjectManager(engine);
    Register_CompilerFactory(engine);

    // register global functions
    engine->RegisterGlobalFunction("int Message(const wxString& in, const wxString& in, int)", asFUNCTION(gMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowMessage(const wxString& in)", asFUNCTION(gShowMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowWarning(const wxString& in)", asFUNCTION(gShowMessageWarn), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowError(const wxString& in)", asFUNCTION(gShowMessageError), asCALL_CDECL);
    engine->RegisterGlobalFunction("void ShowInfo(const wxString& in)", asFUNCTION(gShowMessageInfo), asCALL_CDECL);
    engine->RegisterGlobalFunction("void Log(const wxString& in)", asFUNCTION(gDebugLog), asCALL_CDECL);
    engine->RegisterGlobalFunction("wxString ReplaceMacros(const wxString& in, bool)", asFUNCTION(gReplaceMacros), asCALL_CDECL);
    engine->RegisterGlobalFunction("CompileOptionsBase@ Cast_CompileOptionsBase(Project@)", asFUNCTIONPR(gCastToCompileOptionsBase, (cbProject*), CompileOptionsBase*), asCALL_CDECL);
    engine->RegisterGlobalFunction("CompileOptionsBase@ Cast_CompileOptionsBase(BuildTarget@)", asFUNCTIONPR(gCastToCompileOptionsBase, (ProjectBuildTarget*), CompileOptionsBase*), asCALL_CDECL);

    // Register constants
    RegisterConstBindings(engine);
}

//------------------------------------------------------------------------------
// ConfigManager
//------------------------------------------------------------------------------
void Register_ConfigManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ConfigManagerClass", "wxString Read(const wxString& in,const wxString& in)", asMETHODPR(ConfigManager, Read, (const wxString&,const wxString&), wxString), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "int Read(const wxString& in,int)", asMETHODPR(ConfigManager, ReadInt, (const wxString&,int), int), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "bool Read(const wxString& in,bool)", asMETHODPR(ConfigManager, ReadBool, (const wxString&,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "double Read(const wxString& in,double)", asMETHODPR(ConfigManager, ReadDouble, (const wxString&,double), double), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,int)", asMETHODPR(ConfigManager, Write, (const wxString&,int), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,double)", asMETHODPR(ConfigManager, Write, (const wxString&,double), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,bool)", asMETHODPR(ConfigManager, Write, (const wxString&,bool), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("ConfigManagerClass", "void Write(const wxString& in,const wxString& in,bool)", asMETHODPR(ConfigManager, Write, (const wxString&,const wxString&,bool), void), asCALL_THISCALL);

    // actually bind ConfigManager's instance
    engine->RegisterGlobalProperty("ConfigManagerClass ConfigManager", Manager::Get()->GetConfigManager(_T("scripts")));
}

//------------------------------------------------------------------------------
// EditorBase
//------------------------------------------------------------------------------
template <class T> void Register_EditorBase(asIScriptEngine* engine, const wxString& classname)
{
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetFilename() const", asMETHOD(T, GetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetFilename(const wxString& in)", asMETHOD(T, SetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetShortName() const", asMETHOD(T, GetShortName), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool GetModified() const", asMETHOD(T, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetModified(bool)", asMETHOD(T, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetTitle()", asMETHOD(T, GetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetTitle(const wxString& in)", asMETHOD(T, SetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Activate()", asMETHOD(T, Activate), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool Save()", asMETHOD(T, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool IsBuiltinEditor()", asMETHOD(T, IsBuiltinEditor), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool ThereAreOthers()", asMETHOD(T, ThereAreOthers), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool Close()", asMETHOD(T, Close), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void GotoLine(int,bool)", asMETHOD(T, GotoLine), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void ToggleBreakpoint(int,bool)", asMETHOD(T, ToggleBreakpoint), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool HasBreakpoint(int) const", asMETHOD(T, HasBreakpoint), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void GotoNextBreakpoint()", asMETHOD(T, GotoNextBreakpoint), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void GotoPreviousBreakpoint()", asMETHOD(T, GotoPreviousBreakpoint), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void ToggleBookmark(int)", asMETHOD(T, ToggleBookmark), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool HasBookmark(int) const", asMETHOD(T, HasBookmark), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void GotoNextBookmark()", asMETHOD(T, GotoNextBookmark), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void GotoPreviousBookmark()", asMETHOD(T, GotoPreviousBookmark), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Undo()", asMETHOD(T, Undo), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Redo()", asMETHOD(T, Redo), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Cut()", asMETHOD(T, Cut), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Copy()", asMETHOD(T, Copy), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void Paste()", asMETHOD(T, Paste), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool CanUndo() const", asMETHOD(T, CanUndo), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool CanRedo() const", asMETHOD(T, CanRedo), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool CanPaste() const", asMETHOD(T, CanPaste), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool HasSelection() const", asMETHOD(T, HasSelection), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// cbEditor
//------------------------------------------------------------------------------
void Register_Editor(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_EditorBase<cbEditor>(engine, _T("Editor"));

    engine->RegisterObjectMethod("Editor", "void SetEditorTitle(const wxString& in)", asMETHOD(cbEditor, SetEditorTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "ProjectFile@ GetProjectFile() const", asMETHOD(cbEditor, GetProjectFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool Save()", asMETHOD(cbEditor, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool SaveAs()", asMETHOD(cbEditor, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void FoldAll()", asMETHOD(cbEditor, FoldAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void UnfoldAll()", asMETHOD(cbEditor, UnfoldAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void ToggleAllFolds()", asMETHOD(cbEditor, ToggleAllFolds), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void FoldBlockFromLine(int)", asMETHOD(cbEditor, FoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void UnfoldBlockFromLine(int)", asMETHOD(cbEditor, UnfoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void ToggleFoldBlockFromLine(int)", asMETHOD(cbEditor, ToggleFoldBlockFromLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "int GetLineIndentInSpaces(int)", asMETHOD(cbEditor, GetLineIndentInSpaces), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "wxString GetLineIndentString(int)", asMETHOD(cbEditor, GetLineIndentString), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void Touch()", asMETHOD(cbEditor, Touch), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool Reload()", asMETHOD(cbEditor, Reload), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void Print(bool,int)", asMETHOD(cbEditor, Print), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "void AutoComplete()", asMETHOD(cbEditor, AutoComplete), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool AddBreakpoint(int,bool)", asMETHOD(cbEditor, AddBreakpoint), asCALL_THISCALL);
    engine->RegisterObjectMethod("Editor", "bool RemoveBreakpoint(int,bool)", asMETHOD(cbEditor, RemoveBreakpoint), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// EditorManager
//------------------------------------------------------------------------------
void Register_EditorManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("EditorManagerClass", "void Configure()", asMETHOD(EditorManager, Configure), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ New()", asMETHOD(EditorManager, New), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ Open(const wxString& in,int,ProjectFile@)", asMETHOD(EditorManager, Open), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ IsBuiltinOpen(const wxString& in)", asMETHOD(EditorManager, IsBuiltinOpen), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinEditor(const wxString& in)", asMETHODPR(EditorManager, GetBuiltinEditor, (const wxString&), cbEditor*), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinEditor(int)", asMETHODPR(EditorManager, GetBuiltinEditor, (int), cbEditor*), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "Editor@ GetBuiltinActiveEditor()", asMETHOD(EditorManager, GetBuiltinActiveEditor), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "void ActivateNext()", asMETHOD(EditorManager, ActivateNext), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "void ActivatePrevious()", asMETHOD(EditorManager, ActivatePrevious), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SwapActiveHeaderSource()", asMETHOD(EditorManager, SwapActiveHeaderSource), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool CloseActive()", asMETHOD(EditorManager, CloseActive), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Close(const wxString& in)", asMETHODPR(EditorManager, Close, (const wxString&,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Close(int)", asMETHODPR(EditorManager, Close, (int,bool), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool CloseAll()", asMETHOD(EditorManager, CloseAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Save(const wxString& in)", asMETHODPR(EditorManager, Save, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool Save(int)", asMETHODPR(EditorManager, Save, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveActive()", asMETHOD(EditorManager, SaveActive), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveAs(int)", asMETHOD(EditorManager, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveActiveAs()", asMETHOD(EditorManager, SaveActiveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "bool SaveAll()", asMETHOD(EditorManager, SaveAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("EditorManagerClass", "int ShowFindDialog(bool,bool)", asMETHOD(EditorManager, ShowFindDialog), asCALL_THISCALL);

    // actually bind EditorManager's instance
    engine->RegisterGlobalProperty("EditorManagerClass EditorManager", Manager::Get()->GetEditorManager());
}

//------------------------------------------------------------------------------
// CompileOptionsBase
//------------------------------------------------------------------------------
template <class T> void Register_CompileOptionsBase(asIScriptEngine* engine, const wxString& classname)
{
    engine->RegisterObjectMethod(cbU2C(classname), "void SetLinkerOptions(const wxArrayString& in)", asMETHOD(T, SetLinkerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetLinkLibs(const wxArrayString& in)", asMETHOD(T, SetLinkLibs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetCompilerOptions(const wxArrayString& in)", asMETHOD(T, SetCompilerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetIncludeDirs(const wxArrayString& in)", asMETHOD(T, SetIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetResourceIncludeDirs(const wxArrayString& in)", asMETHOD(T, SetResourceIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetLibDirs(const wxArrayString& in)", asMETHOD(T, SetLibDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetCommandsBeforeBuild(const wxArrayString& in)", asMETHOD(T, SetCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetCommandsAfterBuild(const wxArrayString& in)", asMETHOD(T, SetCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetLinkerOptions() const", asMETHOD(T, GetLinkerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetLinkLibs() const", asMETHOD(T, GetLinkLibs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetCompilerOptions() const", asMETHOD(T, GetCompilerOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetIncludeDirs() const", asMETHOD(T, GetIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetResourceIncludeDirs() const", asMETHOD(T, GetResourceIncludeDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetLibDirs() const", asMETHOD(T, GetLibDirs), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetCommandsBeforeBuild() const", asMETHOD(T, GetCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxArrayString& GetCommandsAfterBuild() const", asMETHOD(T, GetCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool GetModified()", asMETHOD(T, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetModified(bool) const", asMETHOD(T, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddLinkerOption(const wxString& in)", asMETHOD(T, AddLinkerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddLinkLib(const wxString& in)", asMETHOD(T, AddLinkLib), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddCompilerOption(const wxString& in)", asMETHOD(T, AddCompilerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddIncludeDir(const wxString& in)", asMETHOD(T, AddIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddResourceIncludeDir(const wxString& in)", asMETHOD(T, AddResourceIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddLibDir(const wxString& in)", asMETHOD(T, AddLibDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddCommandsBeforeBuild(const wxString& in)", asMETHOD(T, AddCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void AddCommandsAfterBuild(const wxString& in)", asMETHOD(T, AddCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveLinkerOption(const wxString& in)", asMETHOD(T, RemoveLinkerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveLinkLib(const wxString& in)", asMETHOD(T, RemoveLinkLib), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveCompilerOption(const wxString& in)", asMETHOD(T, RemoveCompilerOption), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveIncludeDir(const wxString& in)", asMETHOD(T, RemoveIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveResourceIncludeDir(const wxString& in)", asMETHOD(T, RemoveResourceIncludeDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveLibDir(const wxString& in)", asMETHOD(T, RemoveLibDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveCommandsBeforeBuild(const wxString& in)", asMETHOD(T, RemoveCommandsBeforeBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void RemoveCommandsAfterBuild(const wxString& in)", asMETHOD(T, RemoveCommandsAfterBuild), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool GetAlwaysRunPostBuildSteps() const", asMETHOD(T, GetAlwaysRunPostBuildSteps), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetAlwaysRunPostBuildSteps(bool)", asMETHOD(T, SetAlwaysRunPostBuildSteps), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool SetVar(const wxString& in, const wxString& in, bool)", asMETHOD(T, SetVar), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "bool UnsetVar(const wxString& in)", asMETHOD(T, UnsetVar), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void UnsetAllVars()", asMETHOD(T, UnsetAllVars), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxString& GetVar(const wxString& in) const", asMETHOD(T, GetVar), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// CompileTargetBase
//------------------------------------------------------------------------------
template <class T> void Register_CompileTargetBase(asIScriptEngine* engine, const wxString& classname)
{
    // add CompileOptionsBase methods/properties
    Register_CompileOptionsBase<T>(engine, classname);

    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetFilename() const", asMETHOD(T, GetFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetTitle()", asMETHOD(T, GetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetTitle(const wxString& in)", asMETHOD(T, SetTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetOutputFilename(const wxString& in)", asMETHOD(T, SetOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetWorkingDir(const wxString& in)", asMETHOD(T, SetWorkingDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetObjectOutput(const wxString& in)", asMETHOD(T, SetObjectOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetDepsOutput(const wxString& in)", asMETHOD(T, SetDepsOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "int GetOptionRelation(int)", asMETHOD(T, GetOptionRelation), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetOptionRelation(int,int)", asMETHOD(T, SetOptionRelation), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetWorkingDir()", asMETHOD(T, GetWorkingDir), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetObjectOutput() const", asMETHOD(T, GetObjectOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetDepsOutput()", asMETHOD(T, GetDepsOutput), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetOutputFilename()", asMETHOD(T, GetOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString SuggestOutputFilename()", asMETHOD(T, SuggestOutputFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetExecutableFilename() const", asMETHOD(T, GetExecutableFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetDynamicLibFilename()", asMETHOD(T, GetDynamicLibFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetStaticLibFilename()", asMETHOD(T, GetStaticLibFilename), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString GetBasePath() const", asMETHOD(T, GetBasePath), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetTargetType(const int& in)", asMETHOD(T, SetTargetType), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "int& GetTargetType() const", asMETHOD(T, GetTargetType), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetExecutionParameters() const", asMETHOD(T, GetExecutionParameters), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetExecutionParameters(const wxString& in)", asMETHOD(T, SetExecutionParameters), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "wxString& GetHostApplication() const", asMETHOD(T, GetHostApplication), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetHostApplication(const wxString& in)", asMETHOD(T, SetHostApplication), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "void SetCompilerID(const wxString& in)", asMETHOD(T, SetCompilerID), asCALL_THISCALL);
    engine->RegisterObjectMethod(cbU2C(classname), "const wxString& GetCompilerID() const", asMETHOD(T, GetCompilerID), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// ProjectFile
//------------------------------------------------------------------------------
void Register_ProjectFile(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ProjectFile", "void AddBuildTarget(const wxString& in)", asMETHOD(ProjectFile, AddBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void RenameBuildTarget(const wxString& in, const wxString& in)", asMETHOD(ProjectFile, RenameBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void RemoveBuildTarget(const wxString& in)", asMETHOD(ProjectFile, RemoveBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "wxString GetBaseName()", asMETHOD(ProjectFile, GetBaseName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "wxString& GetObjName()", asMETHOD(ProjectFile, GetObjName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "void SetObjName(const wxString& in)", asMETHOD(ProjectFile, SetObjName), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectFile", "Project@ GetParentProject()", asMETHOD(ProjectFile, GetParentProject), asCALL_THISCALL);

    engine->RegisterObjectProperty("ProjectFile", "wxString relativeFilename", offsetof(ProjectFile, relativeFilename));
    engine->RegisterObjectProperty("ProjectFile", "wxString relativeToCommonTopLevelPath", offsetof(ProjectFile, relativeToCommonTopLevelPath));
    engine->RegisterObjectProperty("ProjectFile", "bool compile", offsetof(ProjectFile, compile));
    engine->RegisterObjectProperty("ProjectFile", "bool link", offsetof(ProjectFile, link));
    engine->RegisterObjectProperty("ProjectFile", "uint16 weight", offsetof(ProjectFile, weight));
//    engine->RegisterObjectProperty("ProjectFile", "wxString buildCommand", offsetof(ProjectFile, buildCommand));
//    engine->RegisterObjectProperty("ProjectFile", "bool useCustomBuildCommand", offsetof(ProjectFile, useCustomBuildCommand));
    engine->RegisterObjectProperty("ProjectFile", "wxString compilerVar", offsetof(ProjectFile, compilerVar));
}

//------------------------------------------------------------------------------
// ProjectBuildTarget
//------------------------------------------------------------------------------
void Register_ProjectBuildTarget(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_CompileTargetBase<ProjectBuildTarget>(engine, _T("BuildTarget"));

    engine->RegisterObjectMethod("BuildTarget", "Project@ GetParentProject()", asMETHOD(ProjectBuildTarget, GetParentProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString GetFullTitle()", asMETHOD(ProjectBuildTarget, GetFullTitle), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString& GetExternalDeps()", asMETHOD(ProjectBuildTarget, GetExternalDeps), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetExternalDeps(const wxString& in)", asMETHOD(ProjectBuildTarget, SetExternalDeps), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetAdditionalOutputFiles(const wxString& in)", asMETHOD(ProjectBuildTarget, SetAdditionalOutputFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "wxString& GetAdditionalOutputFiles()", asMETHOD(ProjectBuildTarget, GetAdditionalOutputFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetIncludeInTargetAll()", asMETHOD(ProjectBuildTarget, GetIncludeInTargetAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetIncludeInTargetAll(bool)", asMETHOD(ProjectBuildTarget, SetIncludeInTargetAll), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetCreateDefFile()", asMETHOD(ProjectBuildTarget, GetCreateDefFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetCreateDefFile(bool)", asMETHOD(ProjectBuildTarget, SetCreateDefFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetCreateStaticLib()", asMETHOD(ProjectBuildTarget, GetCreateStaticLib), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetCreateStaticLib(bool)", asMETHOD(ProjectBuildTarget, SetCreateStaticLib), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "bool GetUseConsoleRunner()", asMETHOD(ProjectBuildTarget, GetUseConsoleRunner), asCALL_THISCALL);
    engine->RegisterObjectMethod("BuildTarget", "void SetUseConsoleRunner(bool)", asMETHOD(ProjectBuildTarget, SetUseConsoleRunner), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// Project
//------------------------------------------------------------------------------
void Register_Project(asIScriptEngine* engine)
{
    // add CompileTargetBase methods/properties
    Register_CompileTargetBase<cbProject>(engine, _T("Project"));

    engine->RegisterObjectMethod("Project", "bool GetModified()", asMETHOD(cbProject, GetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetModified(bool)", asMETHOD(cbProject, SetModified), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetMakefile(const wxString& in)", asMETHOD(cbProject, SetMakefile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "wxString& GetMakefile()", asMETHOD(cbProject, GetMakefile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetMakefileCustom(bool)", asMETHOD(cbProject, SetMakefileCustom), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool IsMakefileCustom()", asMETHOD(cbProject, IsMakefileCustom), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool CloseAllFiles()", asMETHOD(cbProject, CloseAllFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveAllFiles()", asMETHOD(cbProject, SaveAllFiles), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool Save()", asMETHOD(cbProject, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveAs()", asMETHOD(cbProject, SaveAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SaveLayout()", asMETHOD(cbProject, SaveLayout), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool LoadLayout()", asMETHOD(cbProject, LoadLayout), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool ShowOptions()", asMETHOD(cbProject, ShowOptions), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "wxString GetCommonTopLevelPath()", asMETHOD(cbProject, GetCommonTopLevelPath), asCALL_THISCALL);

    engine->RegisterObjectMethod("Project", "int GetFilesCount()", asMETHOD(cbProject, GetFilesCount), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "ProjectFile@ GetFile(int)", asMETHOD(cbProject, GetFile), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveFile(int)", asMETHODPR(cbProject, RemoveFile, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveFile(ProjectFile@)", asMETHODPR(cbProject, RemoveFile, (ProjectFile*), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "ProjectFile@ AddFile(int,const wxString& in,bool,bool,uint16)", asMETHODPR(cbProject, AddFile, (int,const wxString&,bool,bool,unsigned short int), ProjectFile*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "ProjectFile@ AddFile(const wxString& in,const wxString& in,bool,bool,uint16)", asMETHODPR(cbProject, AddFile, (const wxString&,const wxString&,bool,bool,unsigned short int), ProjectFile*), asCALL_THISCALL);

    engine->RegisterObjectMethod("Project", "int GetBuildTargetsCount()", asMETHOD(cbProject, GetBuildTargetsCount), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ GetBuildTarget(int)", asMETHODPR(cbProject, GetBuildTarget, (int), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ GetBuildTarget(const wxString& in)", asMETHODPR(cbProject, GetBuildTarget, (const wxString&), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ AddBuildTarget(const wxString& in)", asMETHOD(cbProject, AddBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RenameBuildTarget(int,const wxString& in)", asMETHODPR(cbProject, RenameBuildTarget, (int, const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RenameBuildTarget(const wxString& in,const wxString& in)", asMETHODPR(cbProject, RenameBuildTarget, (const wxString&, const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ DuplicateBuildTarget(int,const wxString& in)", asMETHODPR(cbProject, DuplicateBuildTarget, (int, const wxString&), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ DuplicateBuildTarget(const wxString& in,const wxString& in)", asMETHODPR(cbProject, DuplicateBuildTarget, (const wxString&, const wxString&), ProjectBuildTarget*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveBuildTarget(int)", asMETHODPR(cbProject, RemoveBuildTarget, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool RemoveBuildTarget(const wxString& in)", asMETHODPR(cbProject, RemoveBuildTarget, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool SetActiveBuildTarget(int)", asMETHOD(cbProject, SetActiveBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "int GetActivedBuildTarget()", asMETHOD(cbProject, GetActiveBuildTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool ExportTargetAsProject(int)", asMETHODPR(cbProject, ExportTargetAsProject, (int), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "bool ExportTargetAsProject(const wxString& in)", asMETHODPR(cbProject, ExportTargetAsProject, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "int SelectTarget(int,bool)", asMETHOD(cbProject, SelectTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "BuildTarget@ GetCurrentlyCompilingTarget()", asMETHOD(cbProject, GetCurrentlyCompilingTarget), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "void SetCurrentlyCompilingTarget(BuildTarget@)", asMETHOD(cbProject, SetCurrentlyCompilingTarget), asCALL_THISCALL);

    engine->RegisterObjectMethod("Project", "void SetModeForPCH(int)", asMETHOD(cbProject, SetModeForPCH), asCALL_THISCALL);
    engine->RegisterObjectMethod("Project", "int GetModeForPCH()", asMETHOD(cbProject, GetModeForPCH), asCALL_THISCALL);
}

//------------------------------------------------------------------------------
// ProjectManager
//------------------------------------------------------------------------------
void Register_ProjectManager(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("ProjectManagerClass", "wxString GetDefaultPath()", asMETHOD(ProjectManager, GetDefaultPath), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "void SetDefaultPath(const wxString& in)", asMETHOD(ProjectManager, SetDefaultPath), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ GetActiveProject()", asMETHOD(ProjectManager, GetActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "void SetProject(Project@, bool)", asMETHOD(ProjectManager, SetProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool LoadWorkspace(const wxString& in)", asMETHOD(ProjectManager, LoadWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveWorkspace()", asMETHOD(ProjectManager, SaveWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveWorkspaceAs(const wxString& in)", asMETHOD(ProjectManager, SaveWorkspaceAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseWorkspaceAs()", asMETHOD(ProjectManager, CloseWorkspace), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ IsOpen(const wxString& in)", asMETHOD(ProjectManager, IsOpen), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ LoadProject(const wxString& in, bool)", asMETHOD(ProjectManager, LoadProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveProject(Project@)", asMETHOD(ProjectManager, SaveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveProjectAs(Project@)", asMETHOD(ProjectManager, SaveProjectAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveActiveProject()", asMETHOD(ProjectManager, SaveActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveActiveProjectAs()", asMETHOD(ProjectManager, SaveActiveProjectAs), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool SaveAllProjects()", asMETHOD(ProjectManager, SaveAllProjects), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseProject(Project@)", asMETHOD(ProjectManager, CloseProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseActiveProject()", asMETHOD(ProjectManager, CloseActiveProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "bool CloseAllProjects()", asMETHOD(ProjectManager, CloseAllProjects), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "Project@ NewProject(const wxString& in)", asMETHOD(ProjectManager, NewProject), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "int AddFileToProject(const wxString& in, Project@, int)", asMETHODPR(ProjectManager, AddFileToProject, (const wxString&,cbProject*,int), int), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProjectManagerClass", "int AskForBuildTargetIndex(Project@)", asMETHOD(ProjectManager, AskForBuildTargetIndex), asCALL_THISCALL);

    // actually bind ProjectManager's instance
    engine->RegisterGlobalProperty("ProjectManagerClass ProjectManager", Manager::Get()->GetProjectManager());
}

//------------------------------------------------------------------------------
// CompilerFactory
//------------------------------------------------------------------------------
void Register_CompilerFactory(asIScriptEngine* engine)
{
    engine->RegisterObjectMethod("CompilerFactoryClass", "bool IsValidCompilerID(const wxString& in)", asFUNCTIONPR(CompilerFactory::IsValidCompilerID, (const wxString&), bool), asCALL_THISCALL);
    engine->RegisterObjectMethod("CompilerFactoryClass", "int GetCompilerIndex(const wxString& in)", asFUNCTIONPR(CompilerFactory::GetCompilerIndex, (const wxString&), int), asCALL_THISCALL);
    engine->RegisterObjectMethod("CompilerFactoryClass", "wxString& GetDefaultCompilerID()", asFUNCTION(CompilerFactory::GetDefaultCompilerID), asCALL_THISCALL);

    // bind CompilerFactory's pseudo-instance (all its members are static anyway)
    // this is a trick to present a normal object to the scripts
    static CompilerFactory cf;
    engine->RegisterGlobalProperty("CompilerFactoryClass CompilerFactory", &cf);
}
