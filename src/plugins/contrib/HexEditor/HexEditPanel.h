/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#ifndef HEXEDITPANEL_H
#define HEXEDITPANEL_H

//(*Headers(HexEditPanel)
#include <wx/scrolbar.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

#include <editorbase.h>
#include <vector>
#include <set>

#include "FileContentBase.h"
#include "ExpressionPreprocessed.h"
#include "HexEditViewBase.h"

class DigitView;

class HexEditPanel: public EditorBase
{
    public:

        HexEditPanel( const wxString& fileName, const wxString& title );

        virtual ~HexEditPanel();

        /** \brief Check if given editor is HexEditor */
        static bool IsHexEditor( EditorBase* editor );

        /** \brief Close all editors */
        static void CloseAllEditors();

    protected:

        virtual bool GetModified() const;
        virtual void SetModified( bool modified );
        virtual bool Save();

    private:

        typedef FileContentBase::OffsetT OffsetT;

        //(*Declarations(HexEditPanel)
        wxMenuItem* MenuItem2;
        wxBoxSizer* PreviewSizer;
        wxMenuItem* MenuItem1;
        wxFlexGridSizer* FlexGridSizer1;
        wxTextCtrl* m_Expression;
        wxBoxSizer* BoxSizer3;
        wxMenu m_EndianessMenu;
        wxStaticText* m_ExpressionVal;
        wxStaticText* m_LDoubleVal;
        wxButton* m_BlockSize;
        wxScrollBar* m_ContentScroll;
        wxStaticText* m_FloatVal;
        wxStaticText* m_DwordVal;
        wxButton* Button1;
        wxPanel* m_DrawArea;
        wxCheckBox* CheckBox1;
        wxButton* Button2;
        wxButton* Button3;
        wxMenu m_BaseMenu;
        wxBoxSizer* BoxSizer2;
        wxStaticText* StaticText1;
        wxStaticText* StaticText3;
        wxMenu m_BlockSizeMenu;
        wxMenuItem* MenuItem3;
        wxStaticLine* StaticLine1;
        wxStaticText* StaticText8;
        wxStaticText* m_WordVal;
        wxBoxSizer* BoxSizer4;
        wxMenuItem* MenuItem5;
        wxStaticText* StaticText4;
        wxTimer ReparseTimer;
        wxBoxSizer* BoxSizer1;
        wxStaticText* m_DoubleVal;
        wxStaticText* StaticText5;
        wxStaticText* StaticText2;
        wxMenuItem* MenuItem7;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem4;
        wxStaticText* StaticText6;
        wxStaticText* m_ByteVal;
        wxStaticText* m_Status;
        wxMenuItem* MenuItem8;
        wxButton* m_Endianess;
        wxButton* m_DigitBits;
        //*)

        //(*Identifiers(HexEditPanel)
        static const long ID_STATICTEXT1;
        static const long ID_BUTTON4;
        static const long ID_BUTTON6;
        static const long ID_BUTTON5;
        static const long ID_STATICLINE1;
        static const long ID_BUTTON1;
        static const long ID_CHECKBOX1;
        static const long ID_PANEL1;
        static const long ID_SCROLLBAR1;
        static const long ID_STATICTEXT2;
        static const long ID_STATICTEXT3;
        static const long ID_STATICTEXT4;
        static const long ID_STATICTEXT5;
        static const long ID_STATICTEXT6;
        static const long ID_STATICTEXT7;
        static const long ID_STATICTEXT8;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT11;
        static const long ID_STATICTEXT12;
        static const long ID_STATICTEXT13;
        static const long ID_STATICTEXT14;
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON3;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT15;
        static const long ID_TIMER1;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long ID_MENUITEM6;
        static const long ID_MENUITEM7;
        static const long ID_MENUITEM8;
        //*)

        //(*Handlers(HexEditPanel)
        void OnContentPaint( wxPaintEvent& event );
        void OnContentSize( wxSizeEvent& event );
        void OnContentScroll( wxScrollEvent& event );
        void OnContentMouseWheel(wxMouseEvent& event);
        void OnDrawAreaKeyDown(wxKeyEvent& event);
        void OnDrawAreaEraseBackground(wxEraseEvent& event);
        void OnForwardFocus(wxFocusEvent& event);
        void OnDrawAreaLeftDown(wxMouseEvent& event);
        void OnDrawAreaLeftUp(wxMouseEvent& event);
        void OnDrawAreaMouseMove(wxMouseEvent& event);
        void OnCheckBox1Click(wxCommandEvent& event);
        void OnSpecialKeyDown(wxKeyEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnReparseTimerTrigger(wxTimerEvent& event);
        void Onm_ExpressionText(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnExpressionTextEnter(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnButton3Click1(wxCommandEvent& event);
        void OnButton4Click(wxCommandEvent& event);
        void OnSetBaseHex(wxCommandEvent& event);
        void OnSetBaseBin(wxCommandEvent& event);
        void Onm_EndianessClick(wxCommandEvent& event);
        void OnSetEndianessBig(wxCommandEvent& event);
        void OnSetEndianessLittle(wxCommandEvent& event);
        void Onm_BlockSizeClick(wxCommandEvent& event);
        void OnSetBlockSize1(wxCommandEvent& event);
        void OnSetBlockSize2(wxCommandEvent& event);
        void OnSetBlockSize4(wxCommandEvent& event);
        void OnSetBlockSize8(wxCommandEvent& event);
        //*)

        typedef std::set< EditorBase* > EditorsSet;

        enum
        {
            VIEW_DIGIT,
            VIEW_CHARS,
            MAX_VIEWS
        };

        wxString                 m_FileName;                    ///< \brief Name of the file being edited
        FileContentBase*         m_Content;                     ///< \brief Content structure of file being edited
        wxString                 m_ErrorString;                 ///< \brief String containg error description (if couldn't read the content)
        wxFont*                  m_DrawFont;                    ///< \brief Font used for drawing
        wxCoord                  m_FontX;                       ///< \brief Width of character
        wxCoord                  m_FontY;                       ///< \brief Height of character
        unsigned int             m_Cols;                        ///< \brief Number of available characters in row in the editor
        unsigned int             m_Lines;                       ///< \brief Number of available lines of characters in the editor
        unsigned int             m_LineBytes;                   ///< \brief Number of bytes shown in a line

        OffsetT                  m_Current;                     ///< \brief Offset of current character
        OffsetT                  m_CurrentBlockStart;           ///< \brief Offset of start of current character block
        OffsetT                  m_CurrentBlockEnd;             ///< \brief Offset of end of current character block

        wxString                 m_ExpressionError;             ///< \brief Last error reported while compiling user-defined expression in preview bar
        Expression::Preprocessed m_ExpressionCode;              ///< \brief Bytecode of user-defined expression in preview bar

        HexEditViewBase*         m_Views[ MAX_VIEWS ];          ///< \brief List of all used views
        unsigned                 m_ViewsCols[ MAX_VIEWS ];      ///< \brief Size of each view in characters
        HexEditViewBase*         m_ActiveView;                  ///< \brief Index of active view
        DigitView*               m_DigitView;                   ///< \brief Cached pointer to digit view

        bool                     m_ViewNotifyContentChange;
        bool                     m_ViewNotifyOffsetChange;
        bool                     m_MouseDown;

        static EditorsSet        m_AllEditors;                  ///< \brief Set of all opened editors, used to close all editors when plugin is being unloaded

        /** \brief Open the file content structure and check for errors */
        void ReadContent();

        /** \brief Set font of given size as used font, does not recalculate coefficients nor font size */
        void SetFontSize( int size );

        /** \brief Recalculate all coefficients (width, height, block sizes eetc) */
        void RecalculateCoefs();

        /** \brief Returns offset of first byte displayed on the screen from the scroll bar */
        FileContentBase::OffsetT DetectStartOffset();

        /** \brief Refresh status line (position, percentage etc) */
        void RefreshStatus();

        /** \brief Update the state of "modified" document */
        void UpdateModified();

        /** \brief Adjust the scrollbar to make sure that m_Current byte is shown on the screen */
        void EnsureCarretVisible();

        /** \brief Adjust the m_Current position to make sure that it points to some byte shown on the screen */
        void ClampCursorToVisibleArea();

        /** \brief Process GOTO request */
        void ProcessGoto();

        /** \brief Reparse expression entered by the user in preview bar */
        void ReparseExpression();

        /** \brief Create views of the editor */
        inline void CreateViews();

        /** \brief Activate view at given index */
        inline void ActivateView( HexEditViewBase* view );

        /** \brief Propagate change of the offset to all views */
        void PropagateOffsetChange( int flagsForCurrentView = -1 );

        /** \brief Notification about content change from the view */
        inline void ViewNotifyContentChange() { m_ViewNotifyContentChange = true; }

        /** \brief Notification about offset change from the view */
        inline void ViewNotifyOffsetChange( OffsetT newOffset ) { m_Current = newOffset; m_ViewNotifyOffsetChange = true; }

        /** \brief Called to adjust views due to display settings change */
        void DisplayChanged();

        friend class HexEditViewBase;

        virtual bool CanUndo() const;
        virtual bool CanRedo() const;
        virtual void Undo();
        virtual void Redo();

        DECLARE_EVENT_TABLE()
};

#endif
