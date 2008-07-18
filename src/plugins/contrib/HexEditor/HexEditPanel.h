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
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
//*)

#include <editorbase.h>
#include <vector>

#include "FileContentBase.h"

class HexEditPanel: public EditorBase
{
    public:

        HexEditPanel( const wxString& fileName, const wxString& title );

        virtual ~HexEditPanel();

    protected:

        virtual bool GetModified() const;
        virtual bool Save();

    private:

        //(*Declarations(HexEditPanel)
        wxStaticText* m_FloatVal;
        wxStaticText* m_ByteVal;
        wxStaticText* m_WordVal;
        wxStaticText* StaticText2;
        wxStaticText* m_DwordVal;
        wxStaticText* m_LDoubleVal;
        wxStaticText* StaticText6;
        wxStaticText* StaticText8;
        wxStaticText* StaticText1;
        wxBoxSizer* BoxSizer2;
        wxStaticText* StaticText3;
        wxScrollBar* m_ContentScroll;
        wxCheckBox* CheckBox1;
        wxStaticText* m_Status;
        wxBoxSizer* BoxSizer1;
        wxFlexGridSizer* FlexGridSizer1;
        wxStaticText* m_DoubleVal;
        wxBoxSizer* BoxSizer3;
        wxStaticText* StaticText4;
        wxPanel* m_DrawArea;
        //*)

        //(*Identifiers(HexEditPanel)
        static const long ID_STATICTEXT1;
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
        //*)

        enum CurrentType
        {
            curHexHi,
            curHexLo,
            curChar
        };

        wxString                     m_FileName;
        FileContentBase*             m_Content;
        wxString                     m_ErrorString;
        wxFont*                      m_DrawFont;
        wxCoord                      m_FontX;
        wxCoord                      m_FontY;
        int                          m_Cols;
        int                          m_Lines;
        unsigned char*               m_ScreenBuffer;
        FileContentBase::OffsetT     m_SelectionStart;
        FileContentBase::OffsetT     m_SelectionEnd;
        FileContentBase::OffsetT     m_Current;
        CurrentType                  m_CurrentType;
        bool                         m_MouseDown;



        void ReadContent();
        void SetFontSize( int size );

        void RecalculateCoefs();
        void PutLine( wxDC& dc, FileContentBase::OffsetT offs, unsigned char* buffer, int len, int x, int y, int selStart, int selEnd );
        FileContentBase::OffsetT DetectStartOffset();
        void RefreshStatus();
        void UpdateModified();

        void EnsureCarretVisible();
        void ClampCursorToVisibleArea();

        virtual bool CanUndo() const;
        virtual bool CanRedo() const;
        virtual void Undo();
        virtual void Redo();


        DECLARE_EVENT_TABLE()
};

#endif
