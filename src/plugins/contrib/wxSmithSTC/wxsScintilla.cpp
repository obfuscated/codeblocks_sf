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

#include "wxsScintilla.h"

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

    wxsRegisterItem<wxsScintilla> Reg(
        _T("wxScintilla"),              // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("Styled Text"),              // Category in palette
        55,                             // Priority in palette
        _T("STC"),                      // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(STC32_xpm),            // 32x32 bitmap
        wxBitmap(STC16_xpm),            // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsScintillaStyles,_T("wxRE_MULTILINE|wxRAISED_BORDER|wxWANTS_CHARS"))
        WXS_ST_CATEGORY("wxScintilla")
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsScintillaEvents)
        WXS_EVI(EVT_SCI_CHANGE,                  wxEVT_SCI_CHANGE,                wxScintillaEventFunction,  EvSciChange)
        WXS_EVI(EVT_SCI_STYLENEEDED,             wxEVT_SCI_STYLENEEDED,           wxScintillaEventFunction,  EvSciStyleNeeded)
        WXS_EVI(EVT_SCI_CHARADDED,               wxEVT_SCI_CHARADDED,             wxScintillaEventFunction,  EvSciCharAdded)
        WXS_EVI(EVT_SCI_SAVEPOINTREACHED,        wxEVT_SCI_SAVEPOINTREACHED,      wxScintillaEventFunction,  EvSciSavePointReached)
        WXS_EVI(EVT_SCI_SAVEPOINTLEFT,           wxEVT_SCI_SAVEPOINTLEFT,         wxScintillaEventFunction,  EvSciSavePointLeft)
        WXS_EVI(EVT_SCI_ROMODIFYATTEMPT,         wxEVT_SCI_ROMODIFYATTEMPT,       wxScintillaEventFunction,  EvSciROModifyAttempt)
        WXS_EVI(EVT_SCI_KEY,                     wxEVT_SCI_KEY,                   wxScintillaEventFunction,  EvSciKey)
        WXS_EVI(EVT_SCI_DOUBLECLICK,             wxEVT_SCI_DOUBLECLICK,           wxScintillaEventFunction,  EvSciDoubleClick)
        WXS_EVI(EVT_SCI_UPDATEUI,                wxEVT_SCI_UPDATEUI,              wxScintillaEventFunction,  EvSciUpdateUI)
        WXS_EVI(EVT_SCI_MODIFIED,                wxEVT_SCI_MODIFIED,              wxScintillaEventFunction,  EvSciModified)
        WXS_EVI(EVT_SCI_MACRORECORD,             wxEVT_SCI_MACRORECORD,           wxScintillaEventFunction,  EvSciMacroRecord)
        WXS_EVI(EVT_SCI_MARGINCLICK,             wxEVT_SCI_MARGINCLICK,           wxScintillaEventFunction,  EvSciMarginClick)
        WXS_EVI(EVT_SCI_NEEDSHOWN,               wxEVT_SCI_NEEDSHOWN,             wxScintillaEventFunction,  EvSciNeedShown)
        WXS_EVI(EVT_SCI_PAINTED,                 wxEVT_SCI_PAINTED,               wxScintillaEventFunction,  EvSciPainted)
        WXS_EVI(EVT_SCI_USERLISTSELECTION,       wxEVT_SCI_USERLISTSELECTION,     wxScintillaEventFunction,  EvSciUserListSelection)
        WXS_EVI(EVT_SCI_URIDROPPED,              wxEVT_SCI_URIDROPPED,            wxScintillaEventFunction,  EvSciURIDropped)
        WXS_EVI(EVT_SCI_DWELLSTART,              wxEVT_SCI_DWELLSTART,            wxScintillaEventFunction,  EvSciDwellStart)
        WXS_EVI(EVT_SCI_DWELLEND,                wxEVT_SCI_DWELLEND,              wxScintillaEventFunction,  EvSciDwellEnd)
        WXS_EVI(EVT_SCI_START_DRAG,              wxEVT_SCI_START_DRAG,            wxScintillaEventFunction,  EvSciStartDrag)
        WXS_EVI(EVT_SCI_DRAG_OVER,               wxEVT_SCI_DRAG_OVER,             wxScintillaEventFunction,  EvSciDragOver)
        WXS_EVI(EVT_SCI_DO_DROP,                 wxEVT_SCI_DO_DROP,               wxScintillaEventFunction,  EvSciDoDrop)
        WXS_EVI(EVT_SCI_ZOOM,                    wxEVT_SCI_ZOOM,                  wxScintillaEventFunction,  EvSciZoom)
        WXS_EVI(EVT_SCI_HOTSPOT_CLICK,           wxEVT_SCI_HOTSPOT_CLICK,         wxScintillaEventFunction,  EvSciHotSpotClick)
        WXS_EVI(EVT_SCI_HOTSPOT_DCLICK,          wxEVT_SCI_HOTSPOT_DCLICK,        wxScintillaEventFunction,  EvSciHotSpotDClick)
        WXS_EVI(EVT_SCI_CALLTIP_CLICK,           wxEVT_SCI_CALLTIP_CLICK,         wxScintillaEventFunction,  EvSciCallTipClick)
        WXS_EVI(EVT_SCI_AUTOCOMP_SELECTION,      wxEVT_SCI_AUTOCOMP_SELECTION,    wxScintillaEventFunction,  EvSciAutoCompSelection)
        WXS_EVI(EVT_SCI_INDICATOR_CLICK,         wxEVT_SCI_INDICATOR_CLICK,       wxScintillaEventFunction,  EvSciIndicatorClick)
        WXS_EVI(EVT_SCI_INDICATOR_RELEASE,       wxEVT_SCI_INDICATOR_RELEASE,     wxScintillaEventFunction,  EvSciIndicatorRelease)
        WXS_EVI(EVT_SCI_AUTOCOMP_CANCELLED,      wxEVT_SCI_AUTOCOMP_CANCELLED,    wxScintillaEventFunction,  EvSciAutoCompCancelled)
        WXS_EVI(EVT_SCI_AUTOCOMP_CHARDELETED,    wxEVT_SCI_AUTOCOMP_CHARDELETED,  wxScintillaEventFunction,  EvSciAutoCompCharDeleted)
        WXS_EVI(EVT_SCI_SETFOCUS,                wxEVT_SCI_SETFOCUS,              wxScintillaEventFunction,  EvSciSetFocus)
        WXS_EVI(EVT_SCI_KILLFOCUS,               wxEVT_SCI_KILLFOCUS,             wxScintillaEventFunction,  EvSciKillFocus)

//        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsScintilla::wxsScintilla(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsScintillaEvents,
        wxsScintillaStyles)
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

void wxsScintilla::OnBuildCreatingCode() {
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

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsScintilla::OnBuildCreatingCode"),GetLanguage());

// who we are

    vname = GetVarName();

// include files

    AddHeader(_("\"wxscintilla.h\""), GetInfo().ClassName, 0);

// make our own size specifier with our default values

    mSize.SetDefaults(wxSize(128, 128));
    ss.Printf(_T("wxSize(%d, %d)"), mSize.GetWidth(), mSize.GetHeight());

// create the panel

    Codef(_T("%C(%W, %I, %P, %s, %T, %N);\n"), ss.c_str());

// other declarations

    BuildSetupWindowCode();

// a virtual size

    if (! mVirtualSize.IsDefault) {
        ss = mVirtualSize.GetSizeCode(GetCoderContext());
        Codef(_T("%ASetVirtualSize(%s);\n"), ss.c_str());
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
        Codef(_T("%AStyleSetFont(wxSCI_STYLE_DEFAULT, %s);\n"), ss.c_str());
        Codef(_T("%AStyleSetFont(0, %s);\n"), ss.c_str());
    };

    if (fg.Ok()) {
        Codef(_T("%AStyleSetForeground(wxSCI_STYLE_DEFAULT, wxColour(%d, %d, %d));\n"), fg.Red(), fg.Green(), fg.Blue());
        Codef(_T("%AStyleSetForeground(0, wxColour(%d, %d, %d));\n"), fg.Red(), fg.Green(), fg.Blue());
    };

    if (bg.Ok()) {
        Codef(_T("%AStyleSetBackground(wxSCI_STYLE_DEFAULT, wxColour(%d, %d, %d));\n"), bg.Red(), bg.Green(), bg.Blue());
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

    Codef(_T("%AStyleSetCase(wxSCI_STYLE_DEFAULT, %d);\n"), mCase);
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
        Codef(_T("%AAppendText(%t);\n"), ss.c_str());
    };

// finish with insert mode and read-only flag

    Codef(_T("%ASetOvertype(%b);\n"), !mInsert);
    Codef(_T("%ASetReadOnly(%b);\n"), mReadOnly);
}

//------------------------------------------------------------------------------

wxObject* wxsScintilla::OnBuildPreview(wxWindow* Parent, long Flags) {
wxScintilla         *stc;
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

    stc = new wxScintilla(Parent, GetId(), Pos(Parent), mSize, Style());

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
        stc->StyleSetFont(wxSCI_STYLE_DEFAULT, ff);
        stc->StyleSetFont(0, ff);
    };

    if (fg.Ok()) {
        stc->StyleSetForeground(wxSCI_STYLE_DEFAULT, fg);
        stc->StyleSetForeground(0, fg);
    };

    if (bg.Ok()) {
        stc->StyleSetBackground(wxSCI_STYLE_DEFAULT, bg);
        stc->StyleSetBackground(0, bg);
        stc->SetWhitespaceBackground(true, bg);
    };

// fill in the other user-defined attributes

    stc->SetCaretStyle(mCaretStyle);
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

    stc->StyleSetCase(wxSCI_STYLE_DEFAULT, mCase);
    stc->StyleSetCase(0, mCase);

    stc->SetIndent(mIndent);
    stc->SetBackSpaceUnIndents(mBSUndent);
    stc->SetWrapMode(mWrapMode);
    stc->SetWrapIndentMode(mWrapIndent);

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

void wxsScintilla::OnEnumWidgetProperties(long Flags) {
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
    wxSCI_LEX_CONTAINER, wxSCI_LEX_NULL, wxSCI_LEX_PYTHON, wxSCI_LEX_CPP, wxSCI_LEX_HTML,
    wxSCI_LEX_XML, wxSCI_LEX_PERL, wxSCI_LEX_SQL, wxSCI_LEX_VB, wxSCI_LEX_PROPERTIES,
    wxSCI_LEX_ERRORLIST, wxSCI_LEX_MAKEFILE, wxSCI_LEX_BATCH, wxSCI_LEX_XCODE, wxSCI_LEX_LATEX,
    wxSCI_LEX_LUA, wxSCI_LEX_DIFF, wxSCI_LEX_CONF, wxSCI_LEX_PASCAL, wxSCI_LEX_AVE,
    wxSCI_LEX_ADA, wxSCI_LEX_LISP, wxSCI_LEX_RUBY, wxSCI_LEX_EIFFEL, wxSCI_LEX_EIFFELKW,
    wxSCI_LEX_TCL, wxSCI_LEX_NNCRONTAB, wxSCI_LEX_BULLANT, wxSCI_LEX_VBSCRIPT, wxSCI_LEX_BAAN,
    wxSCI_LEX_MATLAB, wxSCI_LEX_SCRIPTOL, wxSCI_LEX_ASM, wxSCI_LEX_CPPNOCASE, wxSCI_LEX_FORTRAN,
    wxSCI_LEX_F77, wxSCI_LEX_CSS, wxSCI_LEX_POV, wxSCI_LEX_LOUT, wxSCI_LEX_ESCRIPT,
    wxSCI_LEX_PS, wxSCI_LEX_NSIS, wxSCI_LEX_MMIXAL, wxSCI_LEX_CLW, wxSCI_LEX_CLWNOCASE,
    wxSCI_LEX_LOT, wxSCI_LEX_YAML, wxSCI_LEX_TEX, wxSCI_LEX_METAPOST, wxSCI_LEX_POWERBASIC,
    wxSCI_LEX_FORTH, wxSCI_LEX_ERLANG, wxSCI_LEX_OCTAVE, wxSCI_LEX_MSSQL, wxSCI_LEX_VERILOG,
    wxSCI_LEX_KIX, wxSCI_LEX_GUI4CLI, wxSCI_LEX_SPECMAN, wxSCI_LEX_AU3, wxSCI_LEX_APDL,
    wxSCI_LEX_BASH, wxSCI_LEX_ASN1, wxSCI_LEX_VHDL, wxSCI_LEX_CAML, wxSCI_LEX_BLITZBASIC,
    wxSCI_LEX_PUREBASIC, wxSCI_LEX_HASKELL, wxSCI_LEX_PHPSCRIPT, wxSCI_LEX_TADS3, wxSCI_LEX_REBOL,
    wxSCI_LEX_SMALLTALK, wxSCI_LEX_FLAGSHIP, wxSCI_LEX_CSOUND, wxSCI_LEX_FREEBASIC, wxSCI_LEX_INNOSETUP,
    wxSCI_LEX_OPAL, wxSCI_LEX_SPICE, wxSCI_LEX_D, wxSCI_LEX_CMAKE, wxSCI_LEX_GAP,
    wxSCI_LEX_PLM, wxSCI_LEX_PROGRESS, wxSCI_LEX_ABAQUS, wxSCI_LEX_ASYMPTOTE, wxSCI_LEX_R,
    wxSCI_LEX_MAGIK, wxSCI_LEX_POWERSHELL, wxSCI_LEX_MYSQL, wxSCI_LEX_PO, wxSCI_LEX_TAL,
    wxSCI_LEX_COBOL, wxSCI_LEX_TACL, wxSCI_LEX_SORCUS, wxSCI_LEX_POWERPRO, wxSCI_LEX_NIMROD,
    wxSCI_LEX_SML, wxSCI_LEX_MARKDOWN,
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

    WXS_ARRAYSTRING(wxsScintilla, mText,          _T("Text"),                   _T("mText"),            _("text"));
    WXS_BOOL(       wxsScintilla, mInsert,        _T("Insert Mode"),            _T("mInsert"),          true);
    WXS_BOOL(       wxsScintilla, mReadOnly,      _T("Read Only?"),             _T("mReadOnly"),        false);

    WXS_ENUM(       wxsScintilla, mCaretStyle,    _T("Caret Style"),            _T("mCaretStyle"),      Caret_Values,    Caret_Names,     1);
    WXS_LONG(       wxsScintilla, mBlinkRate,     _T("Caret Blink Rate"),       _T("mBlinkRate"),       500);
    WXS_LONG(       wxsScintilla, mCaretWidth,    _T("Width of Insert Caret"),  _T("mCaretWidth"),      2);
    WXS_COLOUR(     wxsScintilla, mCaretFG,       _T("Caret Colour"),           _T("mCaretFG"));
    WXS_COLOUR(     wxsScintilla, mCaretBG,       _T("Caret Line Colour"),      _T("mCaretBG"));
    WXS_ENUM(       wxsScintilla, mSelMode,       _T("Selection Mode"),         _T("mSelMode"),         Select_Values,    Select_Names,     0);
    WXS_COLOUR(     wxsScintilla, mSelFG,         _T("Selection Foreground"),   _T("mSelFG"));
    WXS_COLOUR(     wxsScintilla, mSelBG,         _T("Selection Background"),   _T("mSelBG"));

    WXS_LONG(       wxsScintilla, mMarginLeft,    _T("Left Margin Edge"),       _T("mMarginLeft"),     0);
    WXS_LONG(       wxsScintilla, mMarginRight,   _T("Right Margin Edge"),      _T("mMarginRight"),    0);

    WXS_ENUM(       wxsScintilla, mGutterType1,   _T("Gutter Type (1)"),        _T("mGutterType1"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsScintilla, mGutterWidth1,  _T("Gutter Width (1)"),       _T("mGutterWidth1"),    0);
    WXS_ENUM(       wxsScintilla, mGutterType2,   _T("Gutter Type (2)"),        _T("mGutterType2"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsScintilla, mGutterWidth2,  _T("Gutter Width (2)"),       _T("mGutterWidth2"),    0);
    WXS_ENUM(       wxsScintilla, mGutterType3,   _T("Gutter Type (3)"),        _T("mGutterType3"),     Gutter_Values,    Gutter_Names,     0);
    WXS_LONG(       wxsScintilla, mGutterWidth3,  _T("Gutter Width (3)"),       _T("mGutterWidth3"),    0);

    WXS_ENUM(       wxsScintilla, mViewWS,        _T("View WhiteS-Space"),      _T("mViewWS"),          ViewWS_Values,  ViewWS_Names,   0);
    WXS_ENUM(       wxsScintilla, mEOL,           _T("End-Of-Line Mode"),       _T("mEOL"),             EOL_Values,     EOL_Names,      2);
    WXS_LONG(       wxsScintilla, mTabWidth,      _T("Tab Width"),              _T("mTabWidth"),        8);
    WXS_ENUM(       wxsScintilla, mCase,          _T("Letter Case"),            _T("mCase"),            Case_Values,    Case_Names,     0);
    WXS_LONG(       wxsScintilla, mIndent,        _T("Indent Size"),            _T("mIndent"),          0);
    WXS_BOOL(       wxsScintilla, mBSUndent,      _T("Backspace Un-Indents?"),  _T("mBSUndent"),        true);
    WXS_ENUM(       wxsScintilla, mWrapMode,      _T("Wrap Mode"),              _T("mWrapMode"),        Wrap_Values,    Wrap_Names,     0);
    WXS_ENUM(       wxsScintilla, mWrapIndent,    _T("Wrap Indent Mode"),       _T("mWrapIndent"),      WrapInd_Values,    WrapInd_Names,     0);

    WXS_BOOL(       wxsScintilla, mBuffered,      _T("Buffered Drawing?"),      _T("mBuffered"),        true);
    WXS_LONG(       wxsScintilla, mZoom,          _T("Zoom Factor"),            _T("mZoom"),            0);

    WXS_SIZE(       wxsScintilla, mVirtualSize,   _T("Default Virtual Size?"),  _T("Virtual Width"),    _T("Virtual Height"), _T("Use Dialog Units?"), _T("mVirtualSize"));
/**
    WXS_ENUM(       wxsScintilla, mEdgeMode,            _T("Edge Display Mode"),              _T("mEdgeMode"),        Edge_Values,    Edge_Names,     0);
    WXS_COLOUR(     wxsScintilla, mEdgeBG,           _T("Edge Display Background"),     _T("mEdgeBG"));
**/
/**
    WXS_ENUM(       wxsScintilla, mLexer,            _T("Lexer"),              _T("mLexer"),        Lex_Values,    Lex_Names,     1);
    WXS_ARRAYSTRING(wxsScintilla, mKeywords1,            _T("Keywords Set 1"),                     _T("mKeywords1"),        _(""));
    WXS_ARRAYSTRING(wxsScintilla, mKeywords2,            _T("Keywords Set 2"),                     _T("mKeywords2"),        _(""));
**/
/**
    WXS_COLOUR(     wxsScintilla, mFoldFG,           _T("Fold-Margin Foreground"),     _T("mFoldFG"));
    WXS_COLOUR(     wxsScintilla, mFoldBG,           _T("Fold-Margin Background"),     _T("mFoldBG"));
**/
};
