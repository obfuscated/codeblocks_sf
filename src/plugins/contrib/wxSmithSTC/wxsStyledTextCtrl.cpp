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

#include "wxsStyledTextCtrl.h"

// TODO: define markers?
// TODO: figure out the margins
//      left-edge and right-edge margins
//      gutter type, guttter width, 3 gutters
// TODO: set edge mode, edge color
// TODO: set zoom
// TODO: set selection mode
// TODO: set caret style
// TODO: set lexer

//------------------------------------------------------------------------------

namespace
{

// Loading images from xpm files

    #include "STC16.xpm"
    #include "STC32.xpm"

    wxsRegisterItem<wxsStyledTextCtrl> Reg(
        _T("wxStyledTextCtrl"),         // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T(""),                         // Author
        _T(""),                         // Author's email
        _T(""),                         // Item's homepage
        _T("Styled Text"),              // Category in palette
        55,                             // Priority in palette
        _T("STC"),                      // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        2, 0,                           // Version
        wxBitmap(STC32_xpm),            // 32x32 bitmap
        wxBitmap(STC16_xpm),            // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsStyledTextCtrlStyles,_T("wxRE_MULTILINE|wxRAISED_BORDER|wxWANTS_CHARS"))
        WXS_ST_CATEGORY("wxScintilla")
        WXS_ST(wxSTC_STYLE_LINENUMBER)
        WXS_ST(wxSTC_STYLE_BRACELIGHT)
        WXS_ST(wxSTC_STYLE_BRACEBAD)
        WXS_ST(wxSTC_STYLE_CONTROLCHAR)
        WXS_ST(wxSTC_STYLE_INDENTGUIDE)
        WXS_ST(wxSTC_STYLE_CALLTIP)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsStyledTextCtrlEvents)
        WXS_EVI(EVT_STC_AUTOCOMP_CANCELLED,     wxEVT_STC_AUTOCOMP_CANCELLED,       wxStyledTextEventFunction,  EvStcACompCancelled)
        WXS_EVI(EVT_STC_AUTOCOMP_CHAR_DELETED,  wxEVT_STC_AUTOCOMP_CHAR_DELETED,    wxStyledTextEventFunction,  EvStcACompCharDel)
    #if(wxCHECK_VERSION(3,1,1))
        WXS_EVI(EVT_STC_AUTOCOMP_COMPLETED,     wxEVT_STC_AUTOCOMP_COMPLETED,       wxStyledTextEventFunction,  EvStcACompCompleted)
    #endif
        WXS_EVI(EVT_STC_AUTOCOMP_SELECTION,     wxEVT_STC_AUTOCOMP_SELECTION,       wxStyledTextEventFunction,  EvStcACompSel)
        WXS_EVI(EVT_STC_CALLTIP_CLICK,          wxEVT_STC_CALLTIP_CLICK,            wxStyledTextEventFunction,  EvStcCallTipClick)
        WXS_EVI(EVT_STC_CHANGE,                 wxEVT_STC_CHANGE,                   wxStyledTextEventFunction,  EvStcChange)
        WXS_EVI(EVT_STC_CHARADDED,              wxEVT_STC_CHARADDED,                wxStyledTextEventFunction,  EvStcCharAdded)
    #if(wxCHECK_VERSION(3,1,0))
        WXS_EVI(EVT_STC_CLIPBOARD_COPY,         wxEVT_STC_CLIPBOARD_COPY,           wxStyledTextEventFunction,  EvStcClipCopy)
        WXS_EVI(EVT_STC_CLIPBOARD_PASTE,        EVT_STC_CLIPBOARD_PASTE,            wxStyledTextEventFunction,  EvStcClipPaste)
    #endif
        WXS_EVI(EVT_STC_DO_DROP,                wxEVT_STC_DO_DROP,                  wxStyledTextEventFunction,  EvStcDoDrop)
        WXS_EVI(EVT_STC_DOUBLECLICK,            wxEVT_STC_DOUBLECLICK,              wxStyledTextEventFunction,  EvStcDClick)
        WXS_EVI(EVT_STC_DRAG_OVER,              wxEVT_STC_DRAG_OVER,                wxStyledTextEventFunction,  EvStcDragOver)
        WXS_EVI(EVT_STC_DWELLEND,               wxEVT_STC_DWELLEND,                 wxStyledTextEventFunction,  EvStcDWelled)
        WXS_EVI(EVT_STC_DWELLSTART,             wxEVT_STC_DWELLSTART,               wxStyledTextEventFunction,  EvStcDWellStart)
        WXS_EVI(EVT_STC_HOTSPOT_CLICK,          wxEVT_STC_HOTSPOT_CLICK,            wxStyledTextEventFunction,  EvStcHotspotClick)
        WXS_EVI(EVT_STC_HOTSPOT_DCLICK,         wxEVT_STC_HOTSPOT_DCLICK,           wxStyledTextEventFunction,  EvStcHotspotDClick)
        WXS_EVI(EVT_STC_HOTSPOT_RELEASE_CLICK,  wxEVT_STC_HOTSPOT_RELEASE_CLICK,    wxStyledTextEventFunction,  EvStcHotspotReleaseClick)
        WXS_EVI(EVT_STC_INDICATOR_CLICK,        wxEVT_STC_INDICATOR_CLICK,          wxStyledTextEventFunction,  EvStcIndicatorClick)
        WXS_EVI(EVT_STC_INDICATOR_RELEASE,      wxEVT_STC_INDICATOR_RELEASE,        wxStyledTextEventFunction,  EvStcIndicatorRelease)
        WXS_EVI(EVT_STC_MACRORECORD,            wxEVT_STC_MACRORECORD,              wxStyledTextEventFunction,  EvStcMacroRecord)
    #if(wxCHECK_VERSION(3,1,1))
        WXS_EVI(EVT_STC_MARGIN_RIGHT_CLICK,     wxEVT_STC_MARGIN_RIGHT_CLICK,       wxStyledTextEventFunction,  EvStcMarginRClick)
    #endif
        WXS_EVI(EVT_STC_MARGINCLICK,            wxEVT_STC_MARGINCLICK,              wxStyledTextEventFunction,  EvStcMarginClick)
        WXS_EVI(EVT_STC_MODIFIED,               wxEVT_STC_MODIFIED,                 wxStyledTextEventFunction,  EvStcModified)
        WXS_EVI(EVT_STC_NEEDSHOWN,              wxEVT_STC_NEEDSHOWN,                wxStyledTextEventFunction,  EvStcNeedShow)
        WXS_EVI(EVT_STC_PAINTED,                wxEVT_STC_PAINTED,                  wxStyledTextEventFunction,  EvStcPainted)
        WXS_EVI(EVT_STC_ROMODIFYATTEMPT,        wxEVT_STC_ROMODIFYATTEMPT,          wxStyledTextEventFunction,  EvStcRomModifyAttempt)
        WXS_EVI(EVT_STC_SAVEPOINTLEFT,          wxEVT_STC_SAVEPOINTLEFT,            wxStyledTextEventFunction,  EvStcSavePointLeft)
        WXS_EVI(EVT_STC_SAVEPOINTREACHED,       wxEVT_STC_SAVEPOINTREACHED,         wxStyledTextEventFunction,  EvStcSavePointReached)
        WXS_EVI(EVT_STC_START_DRAG,             wxEVT_STC_START_DRAG,               wxStyledTextEventFunction,  EvStcStartDrag)
        WXS_EVI(EVT_STC_STYLENEEDED,            wxEVT_STC_STYLENEEDED,              wxStyledTextEventFunction,  EvStcStyleNeeded)
        WXS_EVI(EVT_STC_UPDATEUI,               wxEVT_STC_UPDATEUI,                 wxStyledTextEventFunction,  EvStcUpdateUI)
        WXS_EVI(EVT_STC_USERLISTSELECTION,      wxEVT_STC_USERLISTSELECTION,        wxStyledTextEventFunction,  EvStcUserListSelection)
        WXS_EVI(EVT_STC_ZOOM,                   wxEVT_STC_ZOOM,                     wxStyledTextEventFunction,  EvStcZoom)

//        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsStyledTextCtrl::wxsStyledTextCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStyledTextCtrlEvents,
        wxsStyledTextCtrlStyles)
{
    mSize.Set(32, 32);
    mText.Clear();
    mViewWS = 0;
    mEOL = 2;
    mBuffered = true;
    mTabWidth = 8;
    mCase = 0;
    mBlinkRate = 500;
    mInsert = true;
    mCaretWidth = 2;
    mIndent = 0;
    mReadOnly = false;
    mBSUndent = true;
    mWrapMode = 0;
    mWrapIndent = 0;
    mMarginLeft = 0;
    mMarginRight = 0;
    mGutterType1 = 0;
    mGutterWidth1 = 0;
    mGutterType2 = 0;
    mGutterWidth2 = 0;
    mGutterType3 = 0;
    mGutterWidth3 = 0;
    mZoom = 0;
    mSelMode = 0;
    mCaretStyle = 1;
    mLexer = 0;
    mVirtualSize.IsDefault = true;
}

//------------------------------------------------------------------------------

void wxsStyledTextCtrl::OnBuildCreatingCode() {
wxString            vname;
wxString            aname;
wxsBaseProperties   *bp;
wxColour            fg, bg;
wxColour            kfg, kbg;
wxColour            sfg, sbg;
wxColour            ffg, fbg;
wxColour            ebg;
wxFont              ff;
wxString            ss,tt;

// valid language?

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsStyledTextCtrl::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();

// include files

    AddHeader(_("<wx/stc/stc.h>"), GetInfo().ClassName, 0);

// make our own size specifier with our default values

    mSize.SetDefaults(wxSize(128, 128));
    ss.Printf(_T("wxSize(%d, %d)"), mSize.GetWidth(), mSize.GetHeight());

// create the panel

    Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"), ss.wx_str());

// other declarations

    BuildSetupWindowCode();

// a virtual size

    if (! mVirtualSize.IsDefault) {
        ss = mVirtualSize.GetSizeCode(GetCoderContext());
        Codef(_T("%ASetVirtualSize(%s);\n"), ss.wx_str());
    };

// see the pretty colors

    bp  = GetBaseProps();
    fg  = bp->m_Fg.GetColour();
    bg  = bp->m_Bg.GetColour();
    ff  = bp->m_Font.BuildFont();
    kfg = mCaretFG.GetColour();
    kbg = mCaretBG.GetColour();
    sfg = mSelFG.GetColour();
    sbg = mSelBG.GetColour();
    ffg = mFoldFG.GetColour();
    fbg = mFoldBG.GetColour();
    ebg = mEdgeBG.GetColour();

// basic font and colors

    if (ff.IsOk()) {
        ss = vname + _T("Font");
        Codef(_T("%AStyleSetFont(wxSTC_STYLE_DEFAULT, %s);\n"), ss.wx_str());
        Codef(_T("%AStyleSetFont(0, %s);\n"), ss.wx_str());
    };

    if (fg.Ok()) {
        Codef(_T("%AStyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(%d, %d, %d));\n"), fg.Red(), fg.Green(), fg.Blue());
        Codef(_T("%AStyleSetForeground(0, wxColour(%d, %d, %d));\n"), fg.Red(), fg.Green(), fg.Blue());
    };

    if (bg.Ok()) {
        Codef(_T("%AStyleSetBackground(wxSTC_STYLE_DEFAULT, wxColour(%d, %d, %d));\n"), bg.Red(), bg.Green(), bg.Blue());
        Codef(_T("%AStyleSetBackground(0, wxColour(%d, %d, %d));\n"), bg.Red(), bg.Green(), bg.Blue());
        Codef(_T("%ASetWhitespaceBackground(true, wxColour(%d, %d, %d));\n"), bg.Red(), bg.Green(), bg.Blue());
    };

// then user-selected options

    Codef(_T("%ASetCaretStyle(%d);\n"), mCaretStyle);
    Codef(_T("%ASetCaretPeriod(%d);\n"), mBlinkRate);
    Codef(_T("%ASetCaretWidth(%d);\n"), mCaretWidth);

    if (kfg.Ok()) {
        Codef(_T("%ASetCaretForeground(wxColour(%d, %d, %d));\n"), kfg.Red(), kfg.Green(), kfg.Blue());
    };

    if (kbg.Ok()) {
        Codef(_T("%ASetCaretLineVisible(true);\n"));
        Codef(_T("%ASetCaretLineBackground(wxColour(%d, %d, %d));\n"), kbg.Red(), kbg.Green(), kbg.Blue());
    };

    Codef(_T("%ASetSelectionMode(%d);\n"), mSelMode);

    if (sfg.Ok()) {
        Codef(_T("%ASetSelForeground(true, wxColour(%d, %d, %d));\n"), sfg.Red(), sfg.Green(), sfg.Blue());
    };

    if (sbg.Ok()) {
        Codef(_T("%ASetSelBackground(true, wxColour(%d, %d, %d));\n"), sbg.Red(), sbg.Green(), sbg.Blue());
    };


    Codef(_T("%ASetMargins(%d, %d);\n"), mMarginLeft, mMarginRight);

    Codef(_T("%ASetMarginWidth(1, %d);\n"), mGutterWidth1);
    Codef(_T("%ASetMarginType(1, %d);\n"), mGutterType1);
    Codef(_T("%ASetMarginWidth(2, %d);\n"), mGutterWidth2);
    Codef(_T("%ASetMarginType(2, %d);\n"), mGutterType2);
    Codef(_T("%ASetMarginWidth(3, %d);\n"), mGutterWidth3);
    Codef(_T("%ASetMarginType(3, %d);\n"), mGutterType3);

    Codef(_T("%ASetViewWhiteSpace(%d);\n"), mViewWS);
    Codef(_T("%ASetEOLMode(%d);\n"), mEOL);
    Codef(_T("%ASetTabWidth(%d);\n"), mTabWidth);

    Codef(_T("%AStyleSetCase(wxSTC_STYLE_DEFAULT, %d);\n"), mCase);
    Codef(_T("%AStyleSetCase(0, %d);\n"), mCase);

    Codef(_T("%ASetIndent(%d);\n"), mIndent);
    Codef(_T("%ASetBackSpaceUnIndents(%b);\n"), mBSUndent);
    Codef(_T("%ASetWrapMode(%d);\n"), mWrapMode);
    Codef(_T("%ASetWrapIndentMode(%d);\n"), mWrapIndent);

    Codef(_T("%ASetBufferedDraw(%b);\n"), mBuffered);
    Codef(_T("%ASetZoom(%d);\n"), mZoom);

/**
    if (ffg.Ok()) {
        Codef(_T("%ASetFoldMarginHiColour(true, wxColour(%d, %d, %d));\n"), ffg.Red(), ffg.Green(), ffg.Blue());
    };

    if (fbg.Ok()) {
        Codef(_T("%ASetFoldMarginColour(true, wxColour(%d, %d, %d));\n"), fbg.Red(), fbg.Green(), fbg.Blue());
    };
**/

/**
    Codef(_T("%ASetEdgeMode(%d);\n"), mEdgeMode);
    if (ebg.Ok()) {
        Codef(_T("%ASetEdgeColour(wxColour(%d, %d, %d));\n"), ebg.Red(), ebg.Green(), ebg.Blue());
    };
**/

// initial text

    for(size_t i=0; i<mText.GetCount(); i++) {
        ss  = mText.Item(i);
        ss += _T("\n");
        Codef(_T("%AAppendText(%t);\n"), ss.wx_str());
    };

// finish with insert mode and read-only flag

    Codef(_T("%ASetOvertype(%b);\n"), !mInsert);
    Codef(_T("%ASetReadOnly(%b);\n"), mReadOnly);
}

//------------------------------------------------------------------------------

wxObject* wxsStyledTextCtrl::OnBuildPreview(wxWindow* Parent, long Flags) {
wxStyledTextCtrl    *stc;
wxsBaseProperties   *bp;
wxColour            fg, bg;
wxColour            kfg, kbg;
wxColour            sfg, sbg;
wxColour            ffg, fbg;
wxColour            ebg;
wxFont              ff;
wxString            ss;
wxSize              zz;

// the default size of this widget is microscopic
// use this to make a reasonable default size

    mSize = Size(Parent);
    mSize.SetDefaults(wxSize(128, 128));

// make the basic widget

    stc = new wxStyledTextCtrl(Parent, GetId(), Pos(Parent), mSize, Style());

// the rest of the attributtes

    SetupWindow(stc, Flags);

// a virtual size

    if (! mVirtualSize.IsDefault) {
        zz = mVirtualSize.GetSize(Parent);
        stc->SetVirtualSize(zz);
    };

// get all the pretty colors

    bp  = GetBaseProps();
    fg  = bp->m_Fg.GetColour();
    bg  = bp->m_Bg.GetColour();
    ff  = bp->m_Font.BuildFont();
    kfg = mCaretFG.GetColour();
    kbg = mCaretBG.GetColour();
    sfg = mSelFG.GetColour();
    sbg = mSelBG.GetColour();
    ffg = mFoldFG.GetColour();
    fbg = mFoldBG.GetColour();
    ebg = mEdgeBG.GetColour();

// basic font, foreground, and background

    if (ff.IsOk()) {
        stc->StyleSetFont(wxSTC_STYLE_DEFAULT, ff);
        stc->StyleSetFont(0, ff);
    };

    if (fg.Ok()) {
        stc->StyleSetForeground(wxSTC_STYLE_DEFAULT, fg);
        stc->StyleSetForeground(0, fg);
    };

    if (bg.Ok()) {
        stc->StyleSetBackground(wxSTC_STYLE_DEFAULT, bg);
        stc->StyleSetBackground(0, bg);
        stc->SetWhitespaceBackground(true, bg);
    };

// fill in the other user-defined attributes

    //stc->SetCaretStyle(mCaretStyle);
    stc->SetCaretPeriod(mBlinkRate);
    stc->SetCaretWidth(mCaretWidth);
    if (kfg.Ok()) {
        stc->SetCaretForeground(kfg);
    };

    if (kbg.Ok()) {
        stc->SetCaretLineVisible(true);
        stc->SetCaretLineBackground(kbg);
    };

    stc->SetSelectionMode(mSelMode);


    if (sfg.Ok()) {
        stc->SetSelForeground(true, sfg);
    };

    if (sbg.Ok()) {
        stc->SetSelBackground(true, sbg);
    };

    stc->SetMargins(mMarginLeft, mMarginRight);

    stc->SetMarginWidth(1, mGutterWidth1);
    stc->SetMarginType(1, mGutterType1);
    stc->SetMarginWidth(2, mGutterWidth2);
    stc->SetMarginType(2, mGutterType2);
    stc->SetMarginWidth(3, mGutterWidth3);
    stc->SetMarginType(3, mGutterType3);

    stc->SetViewWhiteSpace(mViewWS);
    stc->SetEOLMode(mEOL);
    stc->SetTabWidth(mTabWidth);

    stc->StyleSetCase(wxSTC_STYLE_DEFAULT, mCase);
    stc->StyleSetCase(0, mCase);

    stc->SetIndent(mIndent);
    stc->SetBackSpaceUnIndents(mBSUndent);
    stc->SetWrapMode(mWrapMode);
    //stc->SetWrapIndentMode(mWrapIndent);

    stc->SetBufferedDraw(mBuffered);
    stc->SetZoom(mZoom);

/**
    if (ffg.Ok()) {
        stc->SetFoldMarginHiColour(true, ffg);
    };

    if (fbg.Ok()) {
        stc->SetFoldMarginColour(true, fbg);
    };
**/

/**
    stc->SetEdgeMode(mEdgeMode);
    if (ebg.Ok()) {
        stc->SetEdgeColour(ebg);
    };
**/

// add in initial text

    for(size_t i=0; i<mText.GetCount(); i++) {
        ss  = mText.Item(i);
        ss += _T("\n");
        stc->AppendText(ss);
    };

// and finally insert or read-only

    stc->SetOvertype(!mInsert);
    stc->SetReadOnly(mReadOnly);

// done

    return stc;
}

//------------------------------------------------------------------------------

void wxsStyledTextCtrl::OnEnumWidgetProperties(long Flags) {
static const long    ViewWS_Values[] = {    0,               1,                    2,                        0};
static const wxChar* ViewWS_Names[]  = {_T("Invisible"), _T("Always Visible"), _T("Visible After Indent"),   0};

static const long    EOL_Values[] = {    0,          1,        2,      0};
static const wxChar* EOL_Names[]  = {_T("CRLF"), _T("CR"), _T("LF"),   0};

static const long    Case_Values[] = {    0,                1,                    2,                  0};
static const wxChar* Case_Names[]  = {_T("Mixed Case"), _T("All Upper Case"), _T("All Lower Case"),   0};

static const long    Wrap_Values[] = {    0,             1,               2,                  0};
static const wxChar* Wrap_Names[]  = {_T("No Wrap"), _T("Word Wrap"), _T("Character Wrap"),   0};

static const long    WrapInd_Values[] = {    0,           1,          2,          0};
static const wxChar* WrapInd_Names[]  = {_T("Fixed"), _T("Same"), _T("Indent"),   0};

static const long    Gutter_Values[] = {    0,           1,          2,    3, 4, 5, 6,      0};
static const wxChar* Gutter_Names[]  = {_T("Symbol"), _T("Number"), _T("Back"), _T("Fore"), _T("Text"), _T("RText"), _T("Changed"),   0};

static const long    Edge_Values[] = {    0,           1,          2,          0};
static const wxChar* Edge_Names[]  = {_T("None"), _T("Line"), _T("Background Colour"),   0};

static const long    Select_Values[] = {    0,            1,               2,             3,    0};
static const wxChar* Select_Names[]  = {_T("Stream"), _T("Rectangle"), _T("Lines"),   _T("Thin"), 0};

static const long    Caret_Values[] = {    0,            1,               2,                0};
static const wxChar* Caret_Names[]  = {_T("Invisible"), _T("Line"), _T("Block"),   0};

static const long    Lex_Values[] = {
    wxSTC_LEX_CONTAINER, wxSTC_LEX_NULL, wxSTC_LEX_PYTHON, wxSTC_LEX_CPP, wxSTC_LEX_HTML,
    wxSTC_LEX_XML, wxSTC_LEX_PERL, wxSTC_LEX_SQL, wxSTC_LEX_VB, wxSTC_LEX_PROPERTIES,
    wxSTC_LEX_ERRORLIST, wxSTC_LEX_MAKEFILE, wxSTC_LEX_BATCH, wxSTC_LEX_XCODE, wxSTC_LEX_LATEX,
    wxSTC_LEX_LUA, wxSTC_LEX_DIFF, wxSTC_LEX_CONF, wxSTC_LEX_PASCAL, wxSTC_LEX_AVE,
    wxSTC_LEX_ADA, wxSTC_LEX_LISP, wxSTC_LEX_RUBY, wxSTC_LEX_EIFFEL, wxSTC_LEX_EIFFELKW,
    wxSTC_LEX_TCL, wxSTC_LEX_NNCRONTAB, wxSTC_LEX_BULLANT, wxSTC_LEX_VBSCRIPT, wxSTC_LEX_BAAN,
    wxSTC_LEX_MATLAB, wxSTC_LEX_SCRIPTOL, wxSTC_LEX_ASM, wxSTC_LEX_CPPNOCASE, wxSTC_LEX_FORTRAN,
    wxSTC_LEX_F77, wxSTC_LEX_CSS, wxSTC_LEX_POV, wxSTC_LEX_LOUT, wxSTC_LEX_ESCRIPT,
    wxSTC_LEX_PS, wxSTC_LEX_NSIS, wxSTC_LEX_MMIXAL, wxSTC_LEX_CLW, wxSTC_LEX_CLWNOCASE,
    wxSTC_LEX_LOT, wxSTC_LEX_YAML, wxSTC_LEX_TEX, wxSTC_LEX_METAPOST, wxSTC_LEX_POWERBASIC,
    wxSTC_LEX_FORTH, wxSTC_LEX_ERLANG, wxSTC_LEX_OCTAVE, wxSTC_LEX_MSSQL, wxSTC_LEX_VERILOG,
    wxSTC_LEX_KIX, wxSTC_LEX_GUI4CLI, wxSTC_LEX_SPECMAN, wxSTC_LEX_AU3, wxSTC_LEX_APDL,
    wxSTC_LEX_BASH, wxSTC_LEX_ASN1, wxSTC_LEX_VHDL, wxSTC_LEX_CAML, wxSTC_LEX_BLITZBASIC,
    wxSTC_LEX_PUREBASIC, wxSTC_LEX_HASKELL, wxSTC_LEX_PHPSCRIPT, wxSTC_LEX_TADS3, wxSTC_LEX_REBOL,
    wxSTC_LEX_SMALLTALK, wxSTC_LEX_FLAGSHIP, wxSTC_LEX_CSOUND, wxSTC_LEX_FREEBASIC, wxSTC_LEX_INNOSETUP,
    wxSTC_LEX_OPAL, wxSTC_LEX_SPICE, wxSTC_LEX_D, wxSTC_LEX_CMAKE, wxSTC_LEX_GAP,
    wxSTC_LEX_PLM, wxSTC_LEX_PROGRESS, wxSTC_LEX_ABAQUS, wxSTC_LEX_ASYMPTOTE, wxSTC_LEX_R,
    wxSTC_LEX_MAGIK, wxSTC_LEX_POWERSHELL, wxSTC_LEX_MYSQL, wxSTC_LEX_PO, wxSTC_LEX_TAL,
    wxSTC_LEX_COBOL, wxSTC_LEX_TACL, wxSTC_LEX_SORCUS, wxSTC_LEX_POWERPRO, wxSTC_LEX_NIMROD,
    wxSTC_LEX_SML, wxSTC_LEX_MARKDOWN,
    0};

static const wxChar* Lex_Names[]  = {
    _T("CONTAINER"), _T("NULL"), _T("PYTHON"), _T("CPP"), _T("HTML"),
    _T("XML"), _T("PERL"), _T("SQL"), _T("VB"), _T("PROPERTIES"),
    _T("ERRORLIST"), _T("MAKEFILE"), _T("BATCH"), _T("XCODE"), _T("LATEX"),
    _T("LUA"), _T("DIFF"), _T("CONF"), _T("PASCAL"), _T("AVE"),
    _T("ADA"), _T("LISP"), _T("RUBY"), _T("EIFFEL"), _T("EIFFELKW"),
    _T("TCL"), _T("NNCRONTAB"), _T("BULLANT"), _T("VBSCRIPT"), _T("BAAN"),
    _T("MATLAB"), _T("SCRIPTOL"), _T("ASM"), _T("CPPNOCASE"), _T("FORTRAN"),
    _T("F77"), _T("CSS"), _T("POV"), _T("LOUT"), _T("ESCRIPT"),
    _T("PS"), _T("NSIS"), _T("MMIXAL"), _T("CLW"), _T("CLWNOCASE"),
    _T("LOT"), _T("YAML"), _T("TEX"), _T("METAPOST"), _T("POWERBASIC"),
    _T("FORTH"), _T("ERLANG"), _T("OCTAVE"), _T("MSSQL"), _T("VERILOG"),
    _T("KIX"), _T("GUI4CLI"), _T("SPECMAN"), _T("AU3"), _T("APDL"),
    _T("BASH"), _T("ASN1"), _T("VHDL"), _T("CAML"), _T("BLITZBASIC"),
    _T("PUREBASIC"), _T("HASKELL"), _T("PHPSCRIPT"), _T("TADS3"), _T("REBOL"),
    _T("SMALLTALK"), _T("FLAGSHIP"), _T("CSOUND"), _T("FREEBASIC"), _T("INNOSETUP"),
    _T("OPAL"), _T("SPICE"), _T("D"), _T("CMAKE"), _T("GAP"),
    _T("PLM"), _T("PROGRESS"), _T("ABAQUS"), _T("ASYMPTOTE"), _T("R"),
    _T("MAGIK"), _T("POWERSHELL"), _T("MYSQL"), _T("PO"), _T("TAL"),
    _T("COBOL"), _T("TACL"), _T("SORCUS"), _T("POWERPRO"), _T("NIMROD"),
    _T("SML"), _T("MARKDOWN"),
    0};

// initial text contents

    WXS_ARRAYSTRING(wxsStyledTextCtrl, mText,          _T("Text"),                   _T("mText"),            _("text"));
    WXS_BOOL(       wxsStyledTextCtrl, mInsert,        _T("Insert Mode"),            _T("mInsert"),          true);
    WXS_BOOL(       wxsStyledTextCtrl, mReadOnly,      _T("Read Only?"),             _T("mReadOnly"),        false);

    WXS_ENUM(       wxsStyledTextCtrl, mCaretStyle,    _T("Caret Style"),            _T("mCaretStyle"),      Caret_Values,    Caret_Names,     1);
    WXS_LONG(       wxsStyledTextCtrl, mBlinkRate,     _T("Caret Blink Rate"),       _T("mBlinkRate"),       500);
    WXS_LONG(       wxsStyledTextCtrl, mCaretWidth,    _T("Width of Insert Caret"),  _T("mCaretWidth"),      2);
    WXS_COLOUR(     wxsStyledTextCtrl, mCaretFG,       _T("Caret Colour"),           _T("mCaretFG"));
    WXS_COLOUR(     wxsStyledTextCtrl, mCaretBG,       _T("Caret Line Colour"),      _T("mCaretBG"));
    WXS_ENUM(       wxsStyledTextCtrl, mSelMode,       _T("Selection Mode"),         _T("mSelMode"),         Select_Values,    Select_Names,     0);
    WXS_COLOUR(     wxsStyledTextCtrl, mSelFG,         _T("Selection Foreground"),   _T("mSelFG"));
    WXS_COLOUR(     wxsStyledTextCtrl, mSelBG,         _T("Selection Background"),   _T("mSelBG"));

    WXS_LONG(       wxsStyledTextCtrl, mMarginLeft,    _T("Left Margin Edge"),       _T("mMarginLeft"),     0);
    WXS_LONG(       wxsStyledTextCtrl, mMarginRight,   _T("Right Margin Edge"),      _T("mMarginRight"),    0);

    WXS_ENUM(       wxsStyledTextCtrl, mGutterType1,   _T("Gutter Type (1)"),        _T("mGutterType1"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsStyledTextCtrl, mGutterWidth1,  _T("Gutter Width (1)"),       _T("mGutterWidth1"),    0);
    WXS_ENUM(       wxsStyledTextCtrl, mGutterType2,   _T("Gutter Type (2)"),        _T("mGutterType2"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsStyledTextCtrl, mGutterWidth2,  _T("Gutter Width (2)"),       _T("mGutterWidth2"),    0);
    WXS_ENUM(       wxsStyledTextCtrl, mGutterType3,   _T("Gutter Type (3)"),        _T("mGutterType3"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsStyledTextCtrl, mGutterWidth3,  _T("Gutter Width (3)"),       _T("mGutterWidth3"),    0);

    WXS_ENUM(       wxsStyledTextCtrl, mViewWS,        _T("View WhiteS-Space"),      _T("mViewWS"),          ViewWS_Values,  ViewWS_Names,   0);
    WXS_ENUM(       wxsStyledTextCtrl, mEOL,           _T("End-Of-Line Mode"),       _T("mEOL"),             EOL_Values,     EOL_Names,      2);
    WXS_LONG(       wxsStyledTextCtrl, mTabWidth,      _T("Tab Width"),              _T("mTabWidth"),        8);
    WXS_ENUM(       wxsStyledTextCtrl, mCase,          _T("Letter Case"),            _T("mCase"),            Case_Values,    Case_Names,     0);
    WXS_LONG(       wxsStyledTextCtrl, mIndent,        _T("Indent Size"),            _T("mIndent"),          0);
    WXS_BOOL(       wxsStyledTextCtrl, mBSUndent,      _T("Backspace Un-Indents?"),  _T("mBSUndent"),        true);
    WXS_ENUM(       wxsStyledTextCtrl, mWrapMode,      _T("Wrap Mode"),              _T("mWrapMode"),        Wrap_Values,    Wrap_Names,     0);
    WXS_ENUM(       wxsStyledTextCtrl, mWrapIndent,    _T("Wrap Indent Mode"),       _T("mWrapIndent"),      WrapInd_Values,    WrapInd_Names,     0);

    WXS_BOOL(       wxsStyledTextCtrl, mBuffered,      _T("Buffered Drawing?"),      _T("mBuffered"),        true);
    WXS_LONG(       wxsStyledTextCtrl, mZoom,          _T("Zoom Factor"),            _T("mZoom"),            0);

    WXS_SIZE(       wxsStyledTextCtrl, mVirtualSize,   _T("Default Virtual Size?"),  _T("Virtual Width"),    _T("Virtual Height"), _T("Use Dialog Units?"), _T("mVirtualSize"));
/**
    WXS_ENUM(       wxsStyledTextCtrl, mEdgeMode,            _T("Edge Display Mode"),              _T("mEdgeMode"),        Edge_Values,    Edge_Names,     0);
    WXS_COLOUR(     wxsStyledTextCtrl, mEdgeBG,           _T("Edge Display Background"),     _T("mEdgeBG"));
**/
/**
    WXS_ENUM(       wxsStyledTextCtrl, mLexer,            _T("Lexer"),              _T("mLexer"),        Lex_Values,    Lex_Names,     1);
    WXS_ARRAYSTRING(wxsStyledTextCtrl, mKeywords1,            _T("Keywords Set 1"),                     _T("mKeywords1"),        _(""));
    WXS_ARRAYSTRING(wxsStyledTextCtrl, mKeywords2,            _T("Keywords Set 2"),                     _T("mKeywords2"),        _(""));
**/
/**
    WXS_COLOUR(     wxsStyledTextCtrl, mFoldFG,           _T("Fold-Margin Foreground"),     _T("mFoldFG"));
    WXS_COLOUR(     wxsStyledTextCtrl, mFoldBG,           _T("Fold-Margin Background"),     _T("mFoldBG"));
**/
};
