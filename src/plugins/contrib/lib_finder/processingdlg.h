#ifndef PROCESSINGDLG_H
#define PROCESSINGDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(ProcessingDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include "libraryconfig.h"

WX_DECLARE_STRING_HASH_MAP(wxArrayString,FileNamesMap);
WX_DECLARE_STRING_HASH_MAP(wxString,wxStringStringMap);

class ProcessingDlg: public wxDialog
{
	public:

		ProcessingDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~ProcessingDlg();

		//(*Identifiers(ProcessingDlg)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_GAUGE1,
		    ID_STATICTEXT1
		};
		//*)

		bool ReadDirs(const wxArrayString& Dirs);

		bool ProcessLibs();

	protected:

		//(*Handlers(ProcessingDlg)
		void OnButton1Click(wxCommandEvent& event);
		//*)

		//(*Declarations(ProcessingDlg)
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxStaticText* Status;
		wxGauge* Gauge1;
		wxButton* StopBtn;
		//*)

	private:

        void ReadDir(const wxString& DirName);
        void ProcessLibrary(const LibraryConfig* Config);
        void SplitPath(const wxString& FileName,wxArrayString& Split);
        bool IsVariable(const wxString& NamePart);
        void CheckNextFileName(const wxString& BasePath,const wxStringStringMap& Vars,const LibraryConfig*Config,int WhichFile);
        void FoundLibrary(const wxString& BasePath,const wxStringStringMap& Vars,const LibraryConfig*Config);
        wxString FixVars(wxString Original,const wxStringStringMap& Vars);
        wxString FixPath(wxString Original);

        bool StopFlag;
        FileNamesMap Map;

		DECLARE_EVENT_TABLE()
};

#endif
