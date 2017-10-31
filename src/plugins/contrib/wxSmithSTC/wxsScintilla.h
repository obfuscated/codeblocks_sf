/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef wxsSCINTILLA_H
#define wxsSCINTILLA_H

#include    <wxswidget.h>
#include    <wxsitemresdata.h>
#include    <wxsarraystringproperty.h>
#include    <wxsenumproperty.h>
#include    <wxsboolproperty.h>
#include    <wxsbaseproperties.h>
#include    <wxspositionsizeproperty.h>
#include    <wxscolourproperty.h>

#include    <wx/wxscintilla.h>

class wxsScintilla : public wxsWidget
{
    public:
        wxsScintilla(wxsItemResData* Data);

    protected:
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnBuildCreatingCode();
        virtual void OnEnumWidgetProperties(long Flags);


        wxSize          mSize;                          // used to check for default size
        wxArrayString   mText;                          // initial text contents
        wxsColourData   mCaretFG, mCaretBG;             // line with caret background color
        long            mViewWS;                        // when to view white-space
        long            mEOL;                           // type of end-of-line chars
        bool            mBuffered;                      // double-buffered drawing?
        long            mTabWidth;                      // size of a tab in spaces
        long            mCase;                          // set letter case
        wxsColourData   mSelFG, mSelBG;                 // selection are colors
        long            mBlinkRate;                     // caret period
        bool            mInsert;                        // insert or over-write mode
        long            mCaretWidth;                    // size of the insert caret
        long            mIndent;                        // indent size in pixels
        bool            mReadOnly;                      // allowed to modify?
        bool            mBSUndent;                      // backspace un-indents?
        long            mWrapMode;                      // type of wrapping to allow
        long            mWrapIndent;                    // how to indent wrapped lines
        wxsColourData   mFoldFG, mFoldBG;               // fold margin colors
        long            mMarginLeft, mMarginRight;      // lef and right wrap-margins
        long            mGutterType1, mGutterWidth1;    // far-left margin
        long            mGutterType2, mGutterWidth2;    // far-left margin
        long            mGutterType3, mGutterWidth3;    // far-left margin
        long            mEdgeMode;                      // how to display right edge (margin?)
        wxsColourData   mEdgeBG;                        // color of the edge
        long            mZoom;                          // zoom the text
        long            mSelMode;                       // selection mode
        long            mCaretStyle;                    // what the caret looks like
        long            mLexer;                         // language lexer to use
        wxArrayString   mKeywords1;                     // set of lexer keywords
        wxArrayString   mKeywords2;                     // set of lexer keywords
        wxsPositionSizeData     mVirtualSize;
};

#endif      // wxsSCINTILLA_H
