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
// RCS-ID: $Id$

#ifndef EDITSNIPPETFRAME_H
#define EDITSNIPPETFRAME_H

#include "scrollingdialog.h"
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/treectrl.h>
#include "cbauibook.h"

//class Edit;
class ScbEditor;
class SEditorManager;
class CodeBlocksEvent;

// ----------------------------------------------------------------------------
class EditSnippetFrame : public wxFrame
// ----------------------------------------------------------------------------
{
    friend class EditFrameDropTextTarget;
    friend class EditFrameDropFileTarget;

    public:
        //! constructor
		EditSnippetFrame(const wxTreeItemId SnippetItemId, int* pRetcode);

		wxString GetName();
		wxString GetText();
		wxString GetFileName(){return m_EditFileName;};
		wxTreeItemId GetSnippetId(){return m_SnippetItemId;}

        // edit object
        SEditorManager* GetEditorManager(){return m_pEditorManager;}

        //! destructor
        ~EditSnippetFrame ();

        //! event handlers
        //! common
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

        void OnFileSave (wxCommandEvent& event);
        void OnFileSaveAs (wxCommandEvent& event);
        void OncbEditorSave( CodeBlocksEvent& event );
        void OnMenuFileClose (wxCommandEvent& event);

        void OnCloseFrameOrWindow(wxCloseEvent &event);
        //-void OnCloseWindow (wxCloseEvent &event);
        ////void OnWindowDestroy( wxCommandEvent event );
        void OnPageClose( wxAuiNotebookEvent event );
        void OnFileCheckModified();

        //! properties
        void OnProperties (wxCommandEvent &event);
        //! print
        //void OnPrintSetup (wxCommandEvent &event);
        //void OnPrintPreview (wxCommandEvent &event);
        //void OnPrint (wxCommandEvent &event);
        void OnFilePrint (wxCommandEvent &event);
        //! edit events
        void OnEditEvent (wxCommandEvent &event);
        void OnEditEventUI (wxUpdateUIEvent& event);

        void On_cbEditorSaveEvent(CodeBlocksEvent& event);

    private:
        void InitEditSnippetFrame(const wxTreeItemId SnippetItemId, int* pRetcode);
		void EndSnippetDlg(int wxID_OKorCANCEL);
		////void OnOK(wxCommandEvent& event);
		////void OnCancel(wxCommandEvent& event);
		void OnHelp(wxCommandEvent& event);
        void FileOpen (wxString fname);
        // print preview position and size
        wxRect DeterminePrintSize ();
        void OnFrameActivated(wxActivateEvent& event);

        void SaveSnippetFramePosn();
        void CreateMenu ();
        void OnConvertEOL (wxCommandEvent &event);
        void OnEditHighlightMode(wxCommandEvent& event);
        void CreateMenuViewLanguage(wxMenu* menuHilight);
        void OpenDroppedFiles(const wxArrayString& arrayData);

        ScbEditor*      m_pScbEditor;
        wxColour        m_SysWinBkgdColour;    //(pecan 2007/3/27)
		wxString        m_EditFileName;        // filename if physical file
		wxString        m_TmpFileName;         // filename if file from xml text
        wxString        m_EditSnippetLabel;
        wxString        m_EditSnippetText;

		// our return code to be placed in m_pReturnCode;
		int             m_nReturnCode;
		wxTreeItemId    m_SnippetItemId;

        //! creates the application menu bar
        wxMenuBar       *m_menuBar;

        SEditorManager* m_pEditorManager;
        int             m_ActiveEventId;
        int             m_OncloseWindowEntries;
        int             m_bOnActivateBusy;

        int             m_bEditorSaveEvent;

        DECLARE_EVENT_TABLE()
};

#endif // EDITSNIPPETFRAME_H
