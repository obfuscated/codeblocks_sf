#ifndef PROCESSINGDLG_H
#define PROCESSINGDLG_H

// NOTE : once the new wxSmith is up and running these includes should be
// moved to the cpp, forward declarations is what we need here

#include <wx/string.h>

//(*Headers(ProcessingDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include "libraryconfig.h"

class wxArrayString;

WX_DECLARE_STRING_HASH_MAP(wxArrayString,FileNamesMap);
WX_DECLARE_STRING_HASH_MAP(wxString,wxStringStringMap);

class ProcessingDlg: public wxDialog
{
	public:

		ProcessingDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~ProcessingDlg();

		//(*Identifiers(ProcessingDlg)
		static const long ID_STATICTEXT1;
		static const long ID_GAUGE1;
		static const long ID_BUTTON1;
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
        bool IsVariable(const wxString& NamePart) const;
        void CheckNextFileName(const wxString& BasePath,const wxStringStringMap& Vars,const LibraryConfig*Config,int WhichFile);
        void FoundLibrary(const wxString& BasePath,const wxStringStringMap& Vars,const LibraryConfig*Config);
        wxString FixVars(wxString Original,const wxStringStringMap& Vars);
        wxString FixPath(wxString Original);

        bool StopFlag;
        FileNamesMap Map;

		DECLARE_EVENT_TABLE()
};

#endif //PROCESSINGDLG_H
