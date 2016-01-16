#ifndef PROJECTMISSINGLIBS_H
#define PROJECTMISSINGLIBS_H

//(*Headers(ProjectMissingLibs)
#include "scrollingdialog.h"
class wxStaticBoxSizer;
class wxFlexGridSizer;
class wxButton;
class wxStaticLine;
class wxStaticText;
class wxPanel;
//*)

#include <wx/stattext.h>
#include <wx/choice.h>

#include "libraryresult.h"
#include "librarydetectionmanager.h"
#include "webresourcesmanager.h"

class ProjectMissingLibs: public wxScrollingDialog, public WebResourcesManager::ProgressHandler
{
	public:

		ProjectMissingLibs( wxWindow* parent,wxArrayString& missingList, TypedResults& currentResults );
		virtual ~ProjectMissingLibs();

	private:

		//(*Declarations(ProjectMissingLibs)
		wxPanel* m_LibsBack;
		wxStaticText* m_StatusText;
		wxFlexGridSizer* m_LibsContainer;
		wxButton* Button1;
		wxButton* m_MissingDefsBtn;
		//*)

		//(*Identifiers(ProjectMissingLibs)
		static const long ID_STATICTEXT1;
		static const long ID_STATICLINE2;
		static const long ID_STATICTEXT2;
		static const long ID_STATICLINE3;
		static const long ID_STATICTEXT3;
		static const long ID_STATICLINE10;
		static const long ID_STATICLINE11;
		static const long ID_STATICLINE12;
		static const long ID_STATICLINE13;
		static const long ID_STATICLINE14;
		static const long ID_PANEL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT4;
		//*)

		//(*Handlers(ProjectMissingLibs)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton1Click1(wxCommandEvent& event);
		//*)

		void InsertLibEntry( const wxString& lib, bool hasSearchFilter, bool isDetected );
		void TryDownloadMissing();
		bool StoreLibraryConfig( const wxString& lib, const std::vector< char >& content );
		bool AreMissingSearchFilters();
		void RecreateLibsList();

        // Implementation of download progress handler
        virtual int  StartDownloading( const wxString& Url );
        virtual void SetProgress( float progress, int id );
        virtual void JobFinished( int id );
        virtual void Error( const wxString& info, int id );
        wxString m_CurrentUrl;
        int      m_CurrentUrlId;

		wxArrayString           m_Libs;
		TypedResults&           m_CurrentResults;
		LibraryDetectionManager m_DetectionManager;
		wxWindowList            m_SearchFlags;

		DECLARE_EVENT_TABLE()
};

#endif
