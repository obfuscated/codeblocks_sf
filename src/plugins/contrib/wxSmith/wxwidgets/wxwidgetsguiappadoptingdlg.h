#ifndef WXWIDGETSGUIAPPADOPTINGDLG_H
#define WXWIDGETSGUIAPPADOPTINGDLG_H

//(*Headers(wxWidgetsGUIAppAdoptingDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
//*)
#include <wx/timer.h>
#include <projectfile.h>

class wxWidgetsGUI;

/** \brief Dialog used when adopting wxApp class
 * \note most of work while scanning sources and
 *       adding wxApp support is done in wxGUI class
 *       actually, this dialog is only to represent
 *       results and get user choice what to do after
 *       scanning
 */
class wxWidgetsGUIAppAdoptingDlg: public wxDialog
{
	public:

		wxWidgetsGUIAppAdoptingDlg(wxWindow* parent,wxWidgetsGUI* GUI,wxWindowID id = -1);
		virtual ~wxWidgetsGUIAppAdoptingDlg();

		//(*Identifiers(wxWidgetsGUIAppAdoptingDlg)
		static const long ID_LISTBOX1;
		static const long ID_GAUGE1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_BUTTON5;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_STATICLINE2;
		static const long ID_BUTTON4;
		static const long ID_STATICLINE1;
		static const long ID_BUTTON6;
		//*)

		wxString m_RelativeFileName;

	protected:

		//(*Handlers(wxWidgetsGUIAppAdoptingDlg)
		void OnButton6Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnUseFileBtnClick(wxCommandEvent& event);
		void OnSelectBtnClick(wxCommandEvent& event);
		void OnCreateBtnClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxWidgetsGUIAppAdoptingDlg)
		wxBoxSizer*  BoxSizer1;
		wxBoxSizer*  BoxSizer4;
		wxStaticBoxSizer*  StaticBoxSizer1;
		wxListBox*  FoundFiles;
		wxGauge*  Progress;
		wxBoxSizer*  BoxSizer2;
		wxStaticText*  ScanningTxt;
		wxStaticText*  ScanningFile;
		wxBoxSizer*  BoxSizer3;
		wxButton*  UseFileBtn;
		wxButton*  SelectBtn;
		wxButton*  CreateBtn;
		wxStaticLine*  StaticLine2;
		wxButton*  Button4;
		wxStaticLine*  StaticLine1;
		wxButton*  Button6;
		//*)

	private:

        /** \brief Starting scanning of files
         *  \note This function creates it's own
         *        event-dispatching loop while searching inside files
         */
        void Run();

        /** \brief Scanning one file
         *  \return true when file can be used as wxApp container, false otherwise
         */
        bool ScanFile(ProjectFile* File);

        /** \brief Adding smith bindings for given file */
        void AddSmith(wxString RelativeFileName);

        cbProject* m_Project;       ///< \brief Helper pointer to C::B project
        wxWidgetsGUI* m_GUI;        ///< \brief GUI which has requested the scan
        wxTimer m_Timer;            ///< \brief Timer used to call Run() after dialog is shown
        bool m_Run;                 ///< \brief Flag used to break scanning of files after closing dialog

		DECLARE_EVENT_TABLE()
};

#endif
