#ifndef HEADERSDETECTORDLG_H
#define HEADERSDETECTORDLG_H

#include <wx/gauge.h>   // Fix MSW bug which forbids using fwd declaration for wxGauge

//(*Headers(HeadersDetectorDlg)
#include "scrollingdialog.h"
#include <wx/timer.h>
class wxStaticBoxSizer;
class wxFlexGridSizer;
class wxGauge;
class wxStdDialogButtonSizer;
class wxBoxSizer;
class wxStaticText;
//*)

#include <cbproject.h>

class HeadersDetectorDlg: public wxScrollingDialog
{
	public:

		HeadersDetectorDlg(wxWindow* parent,cbProject* project,wxArrayString& headers);
		virtual ~HeadersDetectorDlg();

	private:

		//(*Declarations(HeadersDetectorDlg)
		wxStaticText* m_ProjectName;
		wxStaticText* m_FileNameTxt;
		wxStaticText* StaticText1;
		wxTimer Timer1;
		wxGauge* m_ProgressBar;
		wxStaticText* StaticText2;
		//*)

		//(*Identifiers(HeadersDetectorDlg)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT4;
		static const long ID_GAUGE1;
		static const long ID_TIMER1;
		//*)

		//(*Handlers(HeadersDetectorDlg)
		void OnTimer1Trigger(wxTimerEvent& event);
		void Cancel(wxCommandEvent& event);
		//*)

		class WorkThread: public wxThread
		{
		    public:
                WorkThread(): wxThread(wxTHREAD_JOINABLE) {}
                ExitCode Entry() { m_Dlg->ThreadProc(); return 0; }
                HeadersDetectorDlg* m_Dlg;
		};

		void ThreadProc();
		void ProcessFile( ProjectFile* file, wxArrayString& includes );

        WorkThread        m_Thread;
		cbProject*        m_Project;
		wxArrayString&    m_Headers;
		wxCriticalSection m_Section;
		wxString          m_FileName;
		int               m_Progress;
		bool              m_Finished;
		bool              m_Cancel;

		DECLARE_EVENT_TABLE()
};

#endif
