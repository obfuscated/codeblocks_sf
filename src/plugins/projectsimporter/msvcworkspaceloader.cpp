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

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <wx/intl.h>
    #include <wx/txtstrm.h>
    #include <wx/dynarray.h>
    #include <wx/filename.h>
    #include <wx/msgdlg.h>
    #include <wx/wfstream.h>
    #include "globals.h"
    #include "manager.h"
    #include "messagemanager.h"
    #include "projectmanager.h"
    #include "compilerfactory.h"
    #include "compiler.h"
    #include "cbproject.h"
#endif

#include <wx/stream.h>
#include <wx/progdlg.h>

#include "msvcworkspaceloader.h"
#include "importers_globals.h"

MSVCWorkspaceLoader::MSVCWorkspaceLoader()
{
	//ctor
}

MSVCWorkspaceLoader::~MSVCWorkspaceLoader()
{
	//dtor
}

bool MSVCWorkspaceLoader::Open(const wxString& filename, wxString& Title)
{
    bool askForCompiler = false;
    bool askForTargets = false;
    switch (cbMessageBox(_("Do you want the imported projects to use the default compiler?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which compiler to use...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxID_YES: askForCompiler = false; break;
        case wxID_NO: askForCompiler = true; break;
        case wxID_CANCEL: return false;
    }
    switch (cbMessageBox(_("Do you want to import all configurations (e.g. Debug/Release) from the "
                        "imported projects?\n"
                        "(If you answer No, you will be asked for each and every project"
                        " which configurations to import...)"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
    {
        case wxID_YES: askForTargets = false; break;
        case wxID_NO: askForTargets = true; break;
        case wxID_CANCEL: return false;
    }

    wxFileInputStream file(filename);
    if (!file.Ok())
        return false; // error opening file???

    wxArrayString comps;
    wxTextInputStream input(file);

    // read "header"
    if (!file.Eof())
    {
        wxString line = input.ReadLine();
        if (line.IsEmpty())
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("Unsupported format."));
            return false;
        }
        comps = GetArrayFromString(line, _T(","));
        line = comps[0];
        line.Trim(true);
        line.Trim(false);
        if (line != _T("Microsoft Developer Studio Workspace File"))
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("Unsupported format."));
            return false;
        }
        line = comps.GetCount() > 1 ? comps[1] : wxString(wxEmptyString);
        line.Trim(true);
        line.Trim(false);
        if (line != _T("Format Version 6.00"))
            Manager::Get()->GetMessageManager()->DebugLog(_T("Format not recognized. Will try to parse though..."));
    }

    ImportersGlobals::UseDefaultCompiler = !askForCompiler;
    ImportersGlobals::ImportAllTargets = !askForTargets;

    wxProgressDialog progress(_("Importing MSVC 6 workspace"), _("Please wait while importing MSVC 6 workspace..."), 100, 0, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);

    int count = 0;
    cbProject* project = 0;
    cbProject* firstproject = 0;
    wxFileName wfname = filename;
    wfname.Normalize();
    Manager::Get()->GetMessageManager()->DebugLog(_T("Workspace dir: %s"), wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR).c_str());
    while (!file.Eof())
    {
        wxString line = input.ReadLine();

        line.Trim(true);
        line.Trim(false);

        // example wanted line:
        //Project: "Demo_BSP"=.\Samples\BSP\scripts\Demo_BSP.dsp - Package Owner=<4>
        if (line.StartsWith(_T("Project:"))) {
          line.Remove(0, 8); // remove "Project:"
          // now we need to find the equal sign (=) that separates the
          // project title from the filename, and the minus sign (-)
          // that separates the filename from junk info - at least to this importer ;)
          int equal = line.Find(_T('='));
          int minus = line.Find(_T('-'), true); // search from end

          if (equal == -1 || minus == -1)
            continue;

          // read project title and trim quotes
          wxString prjTitle = line.Left(equal);
          prjTitle.Trim(true);
          prjTitle.Trim(false);
          if (prjTitle.IsEmpty())
            continue;
          if (prjTitle.GetChar(0) == _T('\"'))
          {
            prjTitle.Truncate(prjTitle.Length() - 1);
            prjTitle.Remove(0, 1);
          }

          // read project filename and trim quotes
          ++equal;
          wxString prjFile = line.Mid(equal, minus - equal);
          prjFile.Trim(true);
          prjFile.Trim(false);
          if (prjFile.IsEmpty())
            continue;
          if (prjFile.GetChar(0) == _T('\"'))
          {
            prjFile.Truncate(prjFile.Length() - 1);
            prjFile.Remove(0, 1);
          }

          ++count;
          wxFileName fname(UnixFilename(prjFile));
          fname.Normalize(wxPATH_NORM_ALL, wfname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), wxPATH_NATIVE);
          Manager::Get()->GetMessageManager()->DebugLog(_T("Found project '%s' in '%s'"), prjTitle.c_str(), fname.GetFullPath().c_str());
          if (!progress.Pulse(_("Importing project: ") + prjTitle))
			break;
          project = Manager::Get()->GetProjectManager()->LoadProject(fname.GetFullPath(), false);
          if (!firstproject) firstproject = project;
          if (project) registerProject(project->GetTitle(), project);
        }
        /*
        * exemple wanted line:
        * Project_Dep_Name VstSDK
        * and add the dependency/link of the VstSDK project to the current project
        * be carefull, the dependent projects could not have already been read, so we have to remember them
        */
        else if (line.StartsWith(_T("Project_Dep_Name"))) {
          line.Remove(0, 16);
          line.Trim(false);
          if (project) addDependency(project->GetTitle(), line);
        }
    }

	if (firstproject)
		Manager::Get()->GetProjectManager()->SetProject(firstproject);
	updateProjects();
    ImportersGlobals::ResetDefaults();

    Title = wxFileName(filename).GetName() + _(" workspace");
    return count != 0;
}

bool MSVCWorkspaceLoader::Save(const wxString& /*title*/, const wxString& /*filename*/)
{
    // no support for saving workspace files (.dsw)
    return false;
}
