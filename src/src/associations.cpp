#include <sdk.h>
#include "associations.h"
#include "appglobals.h"
#include <manager.h>
#include <configmanager.h>

void DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum);
bool DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum);

void Associations::SetBatchBuildOnly()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	DoSetAssociation(CODEBLOCKS_EXT, wxString(APP_NAME) + _(" project file"), name, _T("1"));
	DoSetAssociation(WORKSPACE_EXT, wxString(APP_NAME) +  _("workspace file"), name, _T("1"));

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

void Associations::Set()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	DoSetAssociation(CODEBLOCKS_EXT, wxString(APP_NAME) + _(" project file"), name, _T("1"));
	DoSetAssociation(WORKSPACE_EXT, wxString(APP_NAME) +  _("workspace file"), name, _T("1"));
	DoSetAssociation(C_EXT, _("C source file"), name, _T("2"));
	DoSetAssociation(CPP_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CC_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CXX_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(H_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HPP_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HH_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HXX_EXT, _("C/C++ header file"), name, _T("4"));

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

bool Associations::Check()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	return DoCheckAssociation(CODEBLOCKS_EXT, wxString(APP_NAME) + _(" project file"), name, _T("1")) &&
            DoCheckAssociation(WORKSPACE_EXT, wxString(APP_NAME) + _(" workspace file"), name, _T("1")) &&
            DoCheckAssociation(C_EXT, _T("C source file"), name, _T("2")) &&
            DoCheckAssociation(CPP_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(CC_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(CXX_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(H_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HPP_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HH_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HXX_EXT, _T("C/C++ header file"), name, _T("4"));
}

void DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
	wxRegKey key; // defaults to HKCR

	key.SetName(_T("HKEY_CLASSES_ROOT\\.") + ext);
	key.Create();
	key = _T("CodeBlocks.") + ext;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext);
	key.Create();
	key = descr;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\DefaultIcon"));
	key.Create();
	key = exe + _T(",") + icoNum;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\command"));
	key.Create();
	key = _T("\"") + exe + _T("\" \"%1\"");

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec"));
	key.Create();
	key = _T("[Open(\"%1\")]");

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\Application"));
	key.Create();
	key = DDE_SERVICE;

	key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\topic"));
	key.Create();
	key = DDE_TOPIC;

	if(ext.IsSameAs(CODEBLOCKS_EXT) || ext.IsSameAs(WORKSPACE_EXT))
	{
	    wxString batchbuildargs = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/batch_build_args"), DEFAULT_BATCH_BUILD_ARGS);
		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Build\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --build \"%1\"");

		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Rebuild (clean)\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --rebuild \"%1\"");
	}
}

bool DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
    wxLogNull no_log_here;
	wxRegKey key; // defaults to HKCR
	wxString strVal;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\.")) + ext);
	if (!key.Open())
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext);
	if (!key.Open())
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\DefaultIcon"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("%s,%s"), exe.c_str(), icoNum.c_str()))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\command"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("\"%s\" \"%%1\""), exe.c_str()))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != _T("[Open(\"%1\")]"))
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec\\application"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_SERVICE)
        return false;

	key.SetName(wxString(_T("HKEY_CLASSES_ROOT\\CodeBlocks.")) + ext + _T("\\shell\\open\\ddeexec\\topic"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_TOPIC)
        return false;

	if(ext.IsSameAs(CODEBLOCKS_EXT) || ext.IsSameAs(WORKSPACE_EXT))
	{
	    wxString batchbuildargs = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/batch_build_args"), DEFAULT_BATCH_BUILD_ARGS);
		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Build\\command"));
        if (!key.Open())
            return false;
        if (!key.QueryValue(wxEmptyString, strVal))
            return false;
        if (strVal != _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --build \"%1\""))
            return false;

		key.SetName(_T("HKEY_CLASSES_ROOT\\CodeBlocks.") + ext + _T("\\shell\\Rebuild (clean)\\command"));
        if (!key.Open())
            return false;
        if (!key.QueryValue(wxEmptyString, strVal))
            return false;
        if (strVal != _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --rebuild \"%1\""))
            return false;
	}

    return true;
}
