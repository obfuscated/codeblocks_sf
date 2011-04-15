/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef PROCESSINGDLG_H
#define PROCESSINGDLG_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/arrstr.h>

//(*Headers(ProcessingDlg)
#include "scrollingdialog.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/gauge.h>
//*)

#include "librarydetectionconfig.h"
#include "librarydetectionmanager.h"
#include "resultmap.h"
#include "pkgconfigmanager.h"

WX_DECLARE_STRING_HASH_MAP(wxArrayString,FileNamesMap);
WX_DECLARE_STRING_HASH_MAP(wxString,wxStringStringMap);

class ProcessingDlg: public wxScrollingDialog
{
	public:

		ProcessingDlg(wxWindow* parent, LibraryDetectionManager& Manager, TypedResults& KnownResults, wxWindowID id = -1);
		virtual ~ProcessingDlg();

		//(*Identifiers(ProcessingDlg)
		static const long ID_STATICTEXT1;
		static const long ID_GAUGE1;
		static const long ID_BUTTON1;
		//*)

		bool ReadDirs(const wxArrayString& Dirs);

		bool ProcessLibs();

		bool ProcessLibs(const wxArrayString& Shortcuts);

		void ApplyResults(bool addOnly);

	protected:

		//(*Handlers(ProcessingDlg)
		void OnButton1Click(wxCommandEvent& event);
		//*)

		//(*Declarations(ProcessingDlg)
		wxStaticText* Status;
		wxButton* StopBtn;
		wxGauge* Gauge1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer1;
		//*)

	private:

        void ReadDir(const wxString& DirName);
        void ProcessLibrary(const LibraryDetectionConfig* Config, const LibraryDetectionConfigSet* Set);
        void SplitPath(const wxString& FileName,wxArrayString& Split);
        bool IsVariable(const wxString& NamePart) const;
        void CheckFilter(const wxString& BasePath,const wxStringStringMap& Vars,const wxArrayString& CompilerList,const LibraryDetectionConfig *Config,const LibraryDetectionConfigSet* Set,int WhichFilter);
        void FoundLibrary(const wxString& BasePath,const wxStringStringMap& Vars,const wxArrayString& CompilerList,const LibraryDetectionConfig *Config,const LibraryDetectionConfigSet* Set);
        wxString FixVars(wxString Original,const wxStringStringMap& Vars);
        wxString FixPath(wxString Original);

        bool StopFlag;
        FileNamesMap Map;
        LibraryDetectionManager& m_Manager;
        TypedResults& m_KnownResults;
        ResultMap m_FoundResults;

		DECLARE_EVENT_TABLE()
};

#endif
