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

	DoSetAssociation(CODEBLOCKS_EXT, g_AppName + _(" project file"), name, _T("1"));
	DoSetAssociation(WORKSPACE_EXT, g_AppName +  _("workspace file"), name, _T("1"));

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

void Associations::Set()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	DoSetAssociation(CODEBLOCKS_EXT, g_AppName + _(" project file"), name, _T("1"));
	DoSetAssociation(WORKSPACE_EXT, g_AppName +  _("workspace file"), name, _T("9"));
	DoSetAssociation(C_EXT, _("C source file"), name, _T("2"));
	DoSetAssociation(CPP_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CC_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(CXX_EXT, _("C++ source file"), name, _T("3"));
	DoSetAssociation(H_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HPP_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HH_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(HXX_EXT, _("C/C++ header file"), name, _T("4"));
	DoSetAssociation(D_EXT, _("D source file"), name, _T("6"));
    DoSetAssociation(_T("rc"), _T("resource file"), name, _T("8"));
    DoSetAssociation(_T("cg"), _T("cg source file"), name, _T("5"));
    DoSetAssociation(_T("F"), _T("Fortran source file"), name, _T("7"));

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

bool Associations::Check()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);

	return DoCheckAssociation(CODEBLOCKS_EXT, g_AppName + _(" project file"), name, _T("1")) &&
            DoCheckAssociation(WORKSPACE_EXT, g_AppName + _(" workspace file"), name, _T("9")) &&
            DoCheckAssociation(C_EXT, _T("C source file"), name, _T("2")) &&
            DoCheckAssociation(CPP_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(CC_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(CXX_EXT, _T("C++ source file"), name, _T("3")) &&
            DoCheckAssociation(H_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HPP_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HH_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(HXX_EXT, _T("C/C++ header file"), name, _T("4")) &&
            DoCheckAssociation(D_EXT, _T("D source file"), name, _T("6"));
            DoCheckAssociation(_T("rc"), _T("resource file"), name, _T("8"));
            DoCheckAssociation(_T("cg"), _T("cg source file"), name, _T("5"));
            DoCheckAssociation(_T("F"), _T("Fortran source file"), name, _T("7"));
}

void DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
	// first determine which key to use
	// win9x/ME/NT 4 ->ROOT, others USER
	wxString BaseKeyName(_T("HKEY_CURRENT_USER\\Software\\Classes"));
	int Major = 0;
	int WinFamily = wxGetOsVersion(&Major, NULL);
	if((WinFamily == wxWIN95) || ((WinFamily == wxWINDOWS_NT) && (Major < 5)))
	{
		BaseKeyName = _T("HKEY_CLASSES_ROOT");
	}
	BaseKeyName += _T("\\");

	wxRegKey key; // defaults to HKCR
	key.SetName(BaseKeyName + _T(".") + ext);
	key.Create();
	key = _T("CodeBlocks.") + ext;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext);
	key.Create();
	key = descr;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\DefaultIcon"));
	key.Create();
	key = exe + _T(",") + icoNum;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\command"));
	key.Create();
	key = _T("\"") + exe + _T("\" \"%1\"");

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec"));
	key.Create();
	key = _T("[Open(\"%1\")]");

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\Application"));
	key.Create();
	key = DDE_SERVICE;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\topic"));
	key.Create();
	key = DDE_TOPIC;

	if(ext.IsSameAs(CODEBLOCKS_EXT) || ext.IsSameAs(WORKSPACE_EXT))
	{
	    wxString batchbuildargs = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/batch_build_args"), g_DefaultBatchBuildArgs);
		key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\Build\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --build \"%1\"");

		key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\Rebuild (clean)\\command"));
		key.Create();
		key = _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --rebuild \"%1\"");
	}
}

bool DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
	// first determine which key to use
	// win9x/ME/NT 4 ->ROOT, others USER
	wxString BaseKeyName(_T("HKEY_CURRENT_USER\\Software\\Classes"));
	int Major = 0;
	int WinFamily = wxGetOsVersion(&Major, NULL);
	if((WinFamily == wxWIN95) || ((WinFamily == wxWINDOWS_NT) && (Major < 5)))
	{
		BaseKeyName = _T("HKEY_CLASSES_ROOT");
	}
	BaseKeyName += _T("\\");

	wxRegKey key; // defaults to HKCR
	key.SetName(BaseKeyName + _T(".") + ext);
	if (!key.Open())
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext);
	if (!key.Open())
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\DefaultIcon"));
	if (!key.Open())
        return false;
	wxString strVal;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("%s,%s"), exe.c_str(), icoNum.c_str()))
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\command"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != wxString::Format(_T("\"%s\" \"%%1\""), exe.c_str()))
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != _T("[Open(\"%1\")]"))
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\application"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_SERVICE)
        return false;

	key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\open\\ddeexec\\topic"));
	if (!key.Open())
        return false;
    if (!key.QueryValue(wxEmptyString, strVal))
        return false;
    if (strVal != DDE_TOPIC)
        return false;

	if(ext.IsSameAs(CODEBLOCKS_EXT) || ext.IsSameAs(WORKSPACE_EXT))
	{
	    wxString batchbuildargs = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/batch_build_args"), g_DefaultBatchBuildArgs);
		key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\Build\\command"));
        if (!key.Open())
            return false;
        if (!key.QueryValue(wxEmptyString, strVal))
            return false;
        if (strVal != _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --build \"%1\""))
            return false;

		key.SetName(BaseKeyName + _T("CodeBlocks.") + ext + _T("\\shell\\Rebuild (clean)\\command"));
        if (!key.Open())
            return false;
        if (!key.QueryValue(wxEmptyString, strVal))
            return false;
        if (strVal != _T("\"") + exe + _T("\" ") + batchbuildargs + _T(" --rebuild \"%1\""))
            return false;
	}

    return true;
}
