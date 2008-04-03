#ifndef HEADERSDETECTORDLG_H
#define HEADERSDETECTORDLG_H

//(*Headers(HeadersDetectorDlg)
#include <wx/dialog.h>
#include <wx/timer.h>
class wxGauge;
class wxStdDialogButtonSizer;
class wxStaticText;
class wxFlexGridSizer;
class wxBoxSizer;
class wxStaticBoxSizer;
//*)

#include <cbproject.h>

class HeadersDetectorDlg: public wxDialog
{
	public:

		HeadersDetectorDlg(wxWindow* parent,cbProject* project,wxArrayString& headers);
		virtual ~HeadersDetectorDlg();

	private:

		//(*Declarations(HeadersDetectorDlg)
		wxStaticText* StaticText2;
		wxStaticText* StaticText1;
		wxStaticText* m_FileNameTxt;
		wxGauge* m_ProgressBar;
		wxStaticText* m_ProjectName;
		wxTimer Timer1;
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
