/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: editsnippetframe.h 70 2007-04-28 16:04:53Z Pecan $

#ifndef EDITSNIPPETFRAME_H
#define EDITSNIPPETFRAME_H

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/treectrl.h>

class Edit;

// ----------------------------------------------------------------------------
class EditSnippetFrame : public wxFrame
// ----------------------------------------------------------------------------
{
    friend class EditFrameDropTarget;

    public:
        //! constructor
		EditSnippetFrame(const wxTreeItemId SnippetItemId, int* pRetcode);

		wxString GetName();
		wxString GetText();
		wxString GetFileName(){return m_EditFileName;};
		wxTreeItemId GetSnippetId(){return m_SnippetItemId;}
        // edit object

        //! destructor
        ~EditSnippetFrame ();

        //! event handlers
        //! common
        void OnCloseWindow (wxCloseEvent &event);
        void OnFocusWindow (wxFocusEvent &event);
        void OnKillFocusWindow (wxFocusEvent &event);
        void OnLeaveWindow (wxMouseEvent &event);
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

		//-wxSemaphore*    pWaitingSemaphore;
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
        wxString        m_EditSnippetText;
        // pointer to parents return code storage area
		int*            m_pReturnCode;
		// our return code to be placed in m_pReturnCode;
		int             m_nReturnCode;
		wxTreeItemId    m_SnippetItemId;

        //! creates the application menu bar
        wxMenuBar       *m_menuBar;

        DECLARE_EVENT_TABLE()
};

#endif // EDITSNIPPETFRAME_H
