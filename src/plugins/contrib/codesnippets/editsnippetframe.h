#ifndef EDITSNIPPETFRAME_H
#define EDITSNIPPETFRAME_H

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/frame.h>

class Edit;

class EditSnippetFrame : public wxFrame
{
    friend class EditFrameDropTarget;

    public:
        //! constructor
		EditSnippetFrame(const wxString& snippetName, wxString* pSnippetText,
                            wxSemaphore* pWaitSem, int* pRetcode, wxString fileName=wxEmptyString);
    public:
		wxString GetName();
		wxString GetText();
        // edit object

        //! destructor
        ~EditSnippetFrame ();

        //! event handlers
        //! common
        void OnCloseWindow (wxCloseEvent &event);
        void OnAbout (wxCommandEvent &event);
        void OnExit (wxCommandEvent &event);
        //-void OnTimerEvent (wxTimerEvent &event);
        //! file
        void OnFileNew (wxCommandEvent &event);
        void OnFileNewFrame (wxCommandEvent &event);
        void OnFileOpen (wxCommandEvent &event);
        void OnFileOpenFrame (wxCommandEvent &event);
        void OnFileSave (wxCommandEvent &event);
        void OnFileSaveAs (wxCommandEvent &event);
        void OnFileClose (wxCommandEvent &event);
        //! properties
        void OnProperties (wxCommandEvent &event);
        //! print
        void OnPrintSetup (wxCommandEvent &event);
        void OnPrintPreview (wxCommandEvent &event);
        void OnPrint (wxCommandEvent &event);
        //! edit events
        void OnEditEvent (wxCommandEvent &event);
        void OnEditEventUI (wxUpdateUIEvent& event);

    private:
		void EndSnippetDlg(int wxID_OKorCANCEL);
		void OnOK(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnHelp(wxCommandEvent& event);
        void FileOpen (wxString fname);
        // print preview position and size
        wxRect DeterminePrintSize ();

        void End_SnippetFrame(int wxID_OKorCANCEL);
        void CreateMenu ();

		wxSemaphore*    pWaitingSemaphore;
        Edit*           m_pEdit;
        wxColour        m_SysWinBkgdColour;    //(pecan 2007/3/27)
		//wxStaticText*   m_NameLbl;
		//wxTextCtrl*     m_SnippetNameCtrl;
		//wxStaticText*   m_SnippetLbl;
		//wxButton*       m_OKBtn;
		//wxButton*       m_CancelBtn;
		//wxButton*       m_HelpBtn;
		wxString        m_EditFileName;
        wxString        m_EditSnippetLabel;
        wxString*       m_pEditSnippetText;
        // pointer to parents return code storage area
		int*            m_pReturnCode;
		// our return code to be placed in m_pReturnCode;
		int             m_nReturnCode;

        //! creates the application menu bar
        wxMenuBar       *m_menuBar;

        DECLARE_EVENT_TABLE()
};

#endif // EDITSNIPPETFRAME_H
