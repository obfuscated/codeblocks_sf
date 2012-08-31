/***************************************************************
 * Name:      wxSmithDemoMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Gary Harris (garyjharris@sourceforge.net)
 * Created:   2010-06-01
 * Copyright: Gary Harris (http://cryogen.66ghz.com/)
 * License:
 **************************************************************/

#include "wxSmithDemoMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(wxSmithDemoFrame)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/paper.h>					// wxPrintPaperDatabase.
#include <wx/aboutdlg.h>
#include <wx/log.h>
#include <wx/dcmemory.h>
#include "version.h"

#include "media/smiley.xpm"
#include "media/zebra.xpm"


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

// The application icon.
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "media/wxsmith.xpm"
#endif

//(*IdInit(wxSmithDemoFrame)
const long wxSmithDemoFrame::ID_ANIMATIONCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_BUTTON1 = wxNewId();
const long wxSmithDemoFrame::ID_BUTTON2 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL7 = wxNewId();
const long wxSmithDemoFrame::ID_SIMPLEHTMLLISTBOX1 = wxNewId();
const long wxSmithDemoFrame::ID_LISTVIEW1 = wxNewId();
const long wxSmithDemoFrame::ID_BITMAPCOMBOBOX1 = wxNewId();
const long wxSmithDemoFrame::ID_STATICBITMAP1 = wxNewId();
const long wxSmithDemoFrame::ID_SEARCHCTRL = wxNewId();
const long wxSmithDemoFrame::ID_PANEL4 = wxNewId();
const long wxSmithDemoFrame::ID_MEDIACTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL8 = wxNewId();
const long wxSmithDemoFrame::ID_COLOURPICKERCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_COLOURPICKERCTRL2 = wxNewId();
const long wxSmithDemoFrame::ID_COLOURPICKERCTRL3 = wxNewId();
const long wxSmithDemoFrame::ID_FILEPICKERCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_DIRPICKERCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_FONTPICKERCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL2 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL3 = wxNewId();
const long wxSmithDemoFrame::ID_STATICTEXT1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL12 = wxNewId();
const long wxSmithDemoFrame::ID_STATICTEXT5 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL13 = wxNewId();
const long wxSmithDemoFrame::ID_STATICTEXT6 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL14 = wxNewId();
const long wxSmithDemoFrame::ID_TREEBOOK1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL11 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTSTYLELISTCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTSTYLECOMBOCTRL1 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTSTYLELISTBOX1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL5 = wxNewId();
const long wxSmithDemoFrame::ID_NOTEBOOK1 = wxNewId();
const long wxSmithDemoFrame::ID_LISTBOX1 = wxNewId();
const long wxSmithDemoFrame::ID_PANEL1 = wxNewId();
const long wxSmithDemoFrame::ID_DIALUPMANAGER1 = wxNewId();
const long wxSmithDemoFrame::ID_MESSAGEDIALOG1 = wxNewId();
const long wxSmithDemoFrame::ID_PASSWORDENTRYDIALOG1 = wxNewId();
const long wxSmithDemoFrame::ID_PROGRESSDIALOG1 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTFORMATTINGDIALOG1 = wxNewId();
const long wxSmithDemoFrame::ID_RICHTEXTSTYLEORGANISERDIALOG1 = wxNewId();
const long wxSmithDemoFrame::ID_TEXTENTRYDIALOG1 = wxNewId();
const long wxSmithDemoFrame::idMenuQuit = wxNewId();
const long wxSmithDemoFrame::idFindReplaceDlg = wxNewId();
const long wxSmithDemoFrame::idFontDlg = wxNewId();
const long wxSmithDemoFrame::idMessageDlg = wxNewId();
const long wxSmithDemoFrame::idPageSetupDlg = wxNewId();
const long wxSmithDemoFrame::idPasswordEntryDialog = wxNewId();
const long wxSmithDemoFrame::idPrintDlg = wxNewId();
const long wxSmithDemoFrame::idProgressDlg = wxNewId();
const long wxSmithDemoFrame::idRichTextFormattingDlg = wxNewId();
const long wxSmithDemoFrame::idRichTextStyleOrganiserDlg = wxNewId();
const long wxSmithDemoFrame::idTextEntryDlg = wxNewId();
const long wxSmithDemoFrame::idCheckNetworkStatus = wxNewId();
const long wxSmithDemoFrame::idHtmlEasyPrint = wxNewId();
const long wxSmithDemoFrame::idHtmlEasyPageSetup = wxNewId();
const long wxSmithDemoFrame::idMenuAbout = wxNewId();
const long wxSmithDemoFrame::ID_STATUSBAR1 = wxNewId();
//*)
const long wxSmithDemoFrame::ID_PANEL99 = wxNewId();

BEGIN_EVENT_TABLE(wxSmithDemoFrame,wxFrame)
    //(*EventTable(wxSmithDemoFrame)
    //*)
END_EVENT_TABLE()

wxSmithDemoFrame::wxSmithDemoFrame(wxWindow* parent,wxWindowID id)
{
    // Set the frame icon
//	SetIcon(wxICON(wxsmith));

	//  A help provider must be created before the wxContextHelpButton control and any controls that use context-sensitive help are declared.
	wxHelpProvider::Set(new wxSimpleHelpProvider);

    //(*Initialize(wxSmithDemoFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer8;
    wxMenuItem* MenuItem2;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer11;
    wxMenu* Menu1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxGridSizer* GridSizer1;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer9;
    wxMenuBar* MenuBar1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;
    wxStaticBoxSizer* StaticBoxSizer5;

    Create(parent, wxID_ANY, _("wxSmith Demonstration Application"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(500,500));
    SetMinSize(wxSize(500,500));
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE, _T("ID_NOTEBOOK1"));
    Panel7 = new wxPanel(Notebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    BoxSizer6 = new wxBoxSizer(wxVERTICAL);
    wxAnimation anim_1(_T("media/bird_44.gif"));
    AnimationCtrl1 = new wxAnimationCtrl(Panel7, ID_ANIMATIONCTRL1, anim_1, wxDefaultPosition, wxDefaultSize, wxAC_DEFAULT_STYLE, _T("ID_ANIMATIONCTRL1"));
    BoxSizer6->Add(AnimationCtrl1, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    ButtonPlayAnimation = new wxButton(Panel7, ID_BUTTON1, _("Play Animation"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer8->Add(ButtonPlayAnimation, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    ButtonStopAnimation = new wxButton(Panel7, ID_BUTTON2, _("Stop Animation"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer8->Add(ButtonStopAnimation, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer6->Add(BoxSizer8, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    Panel7->SetSizer(BoxSizer6);
    BoxSizer6->Fit(Panel7);
    BoxSizer6->SetSizeHints(Panel7);
    Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    GridSizer1 = new wxGridSizer(3, 2, 0, 0);
    SimpleHtmlListBox1 = new wxSimpleHtmlListBox(Panel4, ID_SIMPLEHTMLLISTBOX1, wxDefaultPosition, wxSize(331,220), 0, 0, wxHLB_DEFAULT_STYLE, wxDefaultValidator, _T("ID_SIMPLEHTMLLISTBOX1"));
    SimpleHtmlListBox1->Append(_("<b>Bold text</b>"));
    SimpleHtmlListBox1->Append(_("<u>Underlined text</u>"));
    SimpleHtmlListBox1->Append(_("<h2>Heading 2 text</h2>"));
    SimpleHtmlListBox1->Append(_("<font color=\"green\">Green text</font>"));
    SimpleHtmlListBox1->Append(_("<font color=\"blue\"><i>Blue italic text</i></font>"));
    SimpleHtmlListBox1->Append(_("A link: <a href=\"http://www.codeblocks.org/\">http://www.codeblocks.org/</a>"));
    SimpleHtmlListBox1->Append(_("Some HTML entities: &copy; &lt; &reg;  &gt; &pound; &cent;  \t&sup2; &frac34; &iquest;"));
    SimpleHtmlListBox1->Append(_("Nice, isn\'t it\?"));
    SimpleHtmlListBox1->Append(_("<img src=\"media/smiley.xpm\">"));
    SimpleHtmlListBox1->SetToolTip(_("wxSimpleHtmlListBox\n\nAn implementation of wxHtmlListBox which shows HTML content in\nthe listbox rows. The entries in this list are styled using HTML."));
    SimpleHtmlListBox1->SetHelpText(_("This is sample context-sensitive help for the \nwxSimpleHtmlListBox in its simplest form."));
    GridSizer1->Add(SimpleHtmlListBox1, 1, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    ListView1 = new wxListView(Panel4, ID_LISTVIEW1, wxDefaultPosition, wxSize(263,291), wxLC_LIST, wxDefaultValidator, _T("ID_LISTVIEW1"));
    ListView1->SetToolTip(_("wxListView\n\nA facade for wxListCtrl in report mode."));
    ListView1->SetHelpText(_("This is sample context-sensitive help\nfor the wxListView in its simplest form."));
    GridSizer1->Add(ListView1, 1, wxALL|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BitmapComboBox1 = new wxBitmapComboBox(Panel4, ID_BITMAPCOMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_BITMAPCOMBOBOX1"));
    BitmapComboBox1->SetSelection( BitmapComboBox1->Append(_("Item 1")) );
    BitmapComboBox1->Append(_("Item 2"));
    BitmapComboBox1->Append(_("Item 3"));
    BitmapComboBox1->Append(_("Item 4"));
    BitmapComboBox1->Append(_("Item 5"));
    BitmapComboBox1->Append(_("Item 6"));
    BitmapComboBox1->SetToolTip(_("wxBitmapComboBox\n\nAn enhanced ComboBox wich allows images to be associated with\nentries. This control is taking it\'s images from a wxImageList created\nin wxSmith."));
    BitmapComboBox1->SetHelpText(_("This is sample context-sensitive help\nfor the wxBitmapComboBox in its simplest form."));
    GridSizer1->Add(BitmapComboBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap1 = new wxStaticBitmap(Panel4, ID_STATICBITMAP1, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    StaticBitmap1->SetToolTip(_("wxImage\n\nThis wxStaticBitmap control is getting it\'s image from a wxImage\ncreated in wxSmith.\n\n"));
    StaticBitmap1->SetHelpText(_("This is sample context-sensitive help\nfor the wxImage in its simplest form."));
    GridSizer1->Add(StaticBitmap1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ContextHelpButton1 = new wxContextHelpButton(Panel4, wxID_CONTEXT_HELP, wxDefaultPosition, wxSize(54,21), wxBU_AUTODRAW);
    ContextHelpButton1->SetToolTip(_("wxContextHelpButton\n\nPuts the application into context-help mode. Click on the button\nand then click on the other controls on this page to see context help in\naction."));
    GridSizer1->Add(ContextHelpButton1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SearchCtrl1 = new wxSearchCtrl(Panel4, ID_SEARCHCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL"));
    SearchCtrl1->SetToolTip(_("wxSearchCtrl\n\nA composite control with a search button, a text control, and a cancel button. "));
    SearchCtrl1->SetHelpText(_("This is sample context-sensitive help\nfor the wxSearchCtrl in its simplest form."));
    GridSizer1->Add(SearchCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel4->SetSizer(GridSizer1);
    GridSizer1->Fit(Panel4);
    GridSizer1->SetSizeHints(Panel4);
    Panel8 = new wxPanel(Notebook1, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    BoxSizer10 = new wxBoxSizer(wxVERTICAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    MediaCtrl1 = new wxMediaCtrl(Panel8, ID_MEDIACTRL1, wxEmptyString, wxDefaultPosition, wxSize(326,101), 0);
    MediaCtrl1->Load(_T("media/EarthImpactSml_512kb.mp4"));
    MediaCtrl1->ShowPlayerControls(wxMEDIACTRLPLAYERCONTROLS_DEFAULT);
    MediaCtrl1->SetVolume(0.5);
    MediaCtrl1->SetToolTip(_("wxMediaCtrl\n\nA class for displaying types of media, such as videos, audio files,\nnatively through native codecs.wxMediaCtrl\n\nA class for displaying types of media, such as videos, audio files,\nnatively through native codecs."));
    BoxSizer4->Add(MediaCtrl1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer10->Add(BoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel8->SetSizer(BoxSizer10);
    BoxSizer10->Fit(Panel8);
    BoxSizer10->SetSizeHints(Panel8);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("wxColourPickerCtrl"));
    ColourPickerCtrl1 = new wxColourPickerCtrl(Panel2, ID_COLOURPICKERCTRL1, wxColour(0,0,0), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_COLOURPICKERCTRL1"));
    ColourPickerCtrl1->SetToolTip(_("wxColourPickerCtrl\n\nA button which brings up a wxColourDialog when clicked. This control\nis using the default style."));
    StaticBoxSizer3->Add(ColourPickerCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ColourPickerCtrl2 = new wxColourPickerCtrl(Panel2, ID_COLOURPICKERCTRL2, wxColour(0,0,0), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL, wxDefaultValidator, _T("ID_COLOURPICKERCTRL2"));
    ColourPickerCtrl2->SetToolTip(_("wxColourPickerCtrl\n\nA button which brings up a wxColourDialog when clicked. This control\nis using the wxCLRP_SHOW_LABEL style."));
    StaticBoxSizer3->Add(ColourPickerCtrl2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ColourPickerCtrl3 = new wxColourPickerCtrl(Panel2, ID_COLOURPICKERCTRL3, wxColour(0,0,0), wxDefaultPosition, wxDefaultSize, wxCLRP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_COLOURPICKERCTRL3"));
    ColourPickerCtrl3->SetToolTip(_("wxColourPickerCtrl\n\nwxColourPickerCtrl\n\nA button which brings up a wxColourDialog when clicked. This control\nis using the wxCLRP_USE_TEXTCTRL style."));
    StaticBoxSizer3->Add(ColourPickerCtrl3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("wxFilePickerCtrl"));
    FilePickerCtrl1 = new wxFilePickerCtrl(Panel2, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
    FilePickerCtrl1->SetToolTip(_("wxFilePickerCtrl\n\nA button which brings up a wxFileDialog when clicked."));
    StaticBoxSizer5->Add(FilePickerCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer5, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("wxDirPickerCtrl"));
    DirPickerCtrl1 = new wxDirPickerCtrl(Panel2, ID_DIRPICKERCTRL1, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_DIRPICKERCTRL1"));
    DirPickerCtrl1->SetToolTip(_("wxDirPickerCtrl\n\nA button which brings up a wxDirDialog when clicked."));
    StaticBoxSizer4->Add(DirPickerCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("wxFontPickerCtrl"));
    wxFont PickerFont_1(12,wxSWISS,wxFONTSTYLE_ITALIC,wxNORMAL,false,_T("Tahoma"),wxFONTENCODING_DEFAULT);
    FontPickerCtrl1 = new wxFontPickerCtrl(Panel2, ID_FONTPICKERCTRL1, PickerFont_1, wxDefaultPosition, wxDefaultSize, wxFNTP_FONTDESC_AS_LABEL|wxFNTP_USEFONT_FOR_LABEL|wxFNTP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FONTPICKERCTRL1"));
    FontPickerCtrl1->SetToolTip(_("wxFontPickerCtrl\n\nA button which brings up a wxFontDialog when clicked. This control\nis using the wxFNTP_FONTDESC_AS_LABEL, \nwxFNTP_USEFONT_FOR_LABEL and wxFNTP_USE_TEXTCTRL styles."));
    StaticBoxSizer6->Add(FontPickerCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer2->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer2->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    Panel2->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel2);
    BoxSizer2->SetSizeHints(Panel2);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    RichTextCtrl1 = new wxRichTextCtrl(Panel3, ID_RICHTEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    wxRichTextAttr rchtxtAttr_1;
    RichTextCtrl1->SetToolTip(_("wxRichTextCtrl\n\nProvides a generic, ground-up implementation of a text control\ncapable of showing multiple styles and images."));
    BoxSizer7->Add(RichTextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    Panel3->SetSizer(BoxSizer7);
    BoxSizer7->Fit(Panel3);
    BoxSizer7->SetSizeHints(Panel3);
    Panel11 = new wxPanel(Notebook1, ID_PANEL11, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL11"));
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    Treebook1 = new wxTreebook(Panel11, ID_TREEBOOK1, wxDefaultPosition, wxSize(237,167), wxBK_DEFAULT, _T("ID_TREEBOOK1"));
    Treebook1->SetToolTip(_("wxTreebook\n\nAn extension of the wxNotebook class that allows a tree structured\nset of pages to be shown in a control.\nwxSmith can currently only enter pages at the root level of the tree.\nPages at lower levels have been added manually."));
    Panel12 = new wxPanel(Treebook1, ID_PANEL12, wxPoint(108,0), wxSize(218,224), wxSTATIC_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL12"));
    StaticText1 = new wxStaticText(Panel12, ID_STATICTEXT1, _("wxTreebook\n\nAn extension of the wxNotebook\nclass that allows a tree structure\nset of pages to be shown in a control.\nwxSmith can currently only enter\npages at the root level of the tree.\nPages at lower levels have been\nadded manually."), wxPoint(88,80), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    Panel13 = new wxPanel(Treebook1, ID_PANEL13, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL13"));
    StaticText5 = new wxStaticText(Panel13, ID_STATICTEXT5, _("This is page 2."), wxPoint(88,96), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    Panel14 = new wxPanel(Treebook1, ID_PANEL14, wxPoint(116,0), wxSize(305,224), wxSTATIC_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL14"));
    StaticText6 = new wxStaticText(Panel14, ID_STATICTEXT6, _("This is page 3."), wxPoint(88,96), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    Treebook1->AddPage(Panel12, _("Page Number 1"), false);
    Treebook1->AddPage(Panel13, _("Page Number 2"), false);
    Treebook1->AddPage(Panel14, _("Page Number 3"), false);
    BoxSizer5->Add(Treebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel11->SetSizer(BoxSizer5);
    BoxSizer5->Fit(Panel11);
    BoxSizer5->SetSizeHints(Panel11);
    Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    RichTextStyleListCtrl1 = new wxRichTextStyleListCtrl(Panel5, ID_RICHTEXTSTYLELISTCTRL1, wxDefaultPosition, wxDefaultSize, 0);
    RichTextStyleListCtrl1->SetToolTip(_("wxRichTextStyleListCtrl\n\nThis class incorporates a wxRichTextStyleListBox and a choice control\nthat allows the user to select the category of style to view. "));
    BoxSizer11->Add(RichTextStyleListCtrl1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer3->Add(BoxSizer11, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer9 = new wxBoxSizer(wxVERTICAL);
    RichTextStyleComboCtrl1 = new wxRichTextStyleComboCtrl(Panel5, ID_RICHTEXTSTYLECOMBOCTRL1, wxDefaultPosition, wxSize(221,25), 0);
    RichTextStyleComboCtrl1->SetRichTextCtrl(RichTextCtrl1);
    RichTextStyleComboCtrl1->SetToolTip(_("wxRichTextStyleComboCtrl\n\nA combo control that can display the styles in a wxRichTextStyleSheet,\nand apply the selection to an associated wxRichTextCtrl."));
    BoxSizer9->Add(RichTextStyleComboCtrl1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    RichTextStyleListBox1 = new wxRichTextStyleListBox(Panel5, ID_RICHTEXTSTYLELISTBOX1, wxDefaultPosition, wxSize(221,184), wxSUNKEN_BORDER);
    RichTextStyleListBox1->SetStyleType(wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL);
    RichTextStyleListBox1->SetToolTip(_("wxRichTextStyleListBox\n\nA listbox that can display the styles in a wxRichTextStyleSheet, and\napply the selection to an associated wxRichTextCtrl."));
    BoxSizer9->Add(RichTextStyleListBox1, 6, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer3->Add(BoxSizer9, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel5->SetSizer(BoxSizer3);
    BoxSizer3->Fit(Panel5);
    BoxSizer3->SetSizeHints(Panel5);
    Notebook1->AddPage(Panel7, _("wxAnimationCtrl"), false);
    Notebook1->AddPage(Panel4, _("wxControls"), false);
    Notebook1->AddPage(Panel8, _("wxMediaCtrl"), false);
    Notebook1->AddPage(Panel2, _("wxPickers"), false);
    Notebook1->AddPage(Panel3, _("wxRichTextCtrl"), false);
    Notebook1->AddPage(Panel11, _("wxTreebook"), false);
    Notebook1->AddPage(Panel5, _("Rich Text Controls"), false);
    BoxSizer1->Add(Notebook1, 3, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    ListBoxLog = new wxListBox(Panel1, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    BoxSizer1->Add(ListBoxLog, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel1);
    BoxSizer1->SetSizeHints(Panel1);
    DialUpManager1 = wxDialUpManager::Create();
    // NOTE: In order for events to connect properly you must set the ID of this wxFindReplaceDialog to -1 in wxSmith.
    FindReplaceDialog1 = new wxFindReplaceDialog(this, &findReplaceData_1, _("wxFindReplaceDialog"), wxFR_REPLACEDIALOG);
    wxFontData fontData_1;
    fontData_1.SetInitialFont(*wxNORMAL_FONT);
    fontData_1.SetAllowSymbols(true);
    FontDialog1 = new wxFontDialog(this, fontData_1);
    HtmlEasyPrinting1 = new wxHtmlEasyPrinting(_T("wxHtmlEasyPrinting"), this);
    MessageDialog1 = new wxMessageDialog(this, _("Exciting, isn\'t it\? :-)"), _("wxMessageDialog"), wxOK, wxDefaultPosition);
    pageSetupDialogData_1 = new wxPageSetupDialogData;
    PageSetupDialog1 = new wxPageSetupDialog(this, pageSetupDialogData_1);
    PasswordEntryDialog1 = new wxPasswordEntryDialog(this, _("Enter some text."), _("wxPasswordEntryDialog"), _("Password"), wxCANCEL|wxCENTRE|wxOK, wxDefaultPosition);
    printDialogData_1 = new wxPrintDialogData;
    PrintDialog1 = new wxPrintDialog(this, printDialogData_1);
    RichTextFormattingDialog1 = new wxRichTextFormattingDialog(wxRICHTEXT_FORMAT_BULLETS|wxRICHTEXT_FORMAT_FONT|wxRICHTEXT_FORMAT_INDENTS_SPACING|wxRICHTEXT_FORMAT_TABS, this, _("wxRichTextFormattingDialog"), ID_RICHTEXTFORMATTINGDIALOG1, wxDefaultPosition, wxDefaultSize);
    richTextStyleSheet_1 = new wxRichTextStyleSheet;
    RichTextStyleOrganiserDialog1 = new wxRichTextStyleOrganiserDialog(wxRICHTEXT_ORGANISER_APPLY_STYLES|wxRICHTEXT_ORGANISER_CREATE_STYLES|wxRICHTEXT_ORGANISER_DELETE_STYLES|wxRICHTEXT_ORGANISER_EDIT_STYLES|wxRICHTEXT_ORGANISER_RENAME_STYLES|wxRICHTEXT_ORGANISER_SHOW_ALL, richTextStyleSheet_1, NULL, this, ID_RICHTEXTSTYLEORGANISERDIALOG1, _("wxRichTextStyleOrganiserDialog"), SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_POSITION, SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_SIZE, SYMBOL_WXRICHTEXTSTYLEORGANISERDIALOG_STYLE);
    TextEntryDialog1 = new wxTextEntryDialog(this, _("Enter some text."), _("wxTextEntryDialog"), _("Text"), wxOK|wxCANCEL|wxCENTRE|wxWS_EX_VALIDATE_RECURSIVELY, wxDefaultPosition);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application."), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu5 = new wxMenu();
    MenuItemFindReplaceDlg = new wxMenuItem(Menu5, idFindReplaceDlg, _("wxFindReplaceDialog"), _("Run the find/replace dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemFindReplaceDlg);
    MenuItemFontDlg = new wxMenuItem(Menu5, idFontDlg, _("wxFontDialog"), _("Run the font dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemFontDlg);
    MenuItemMessageDlg = new wxMenuItem(Menu5, idMessageDlg, _("wxMessageDialog"), _("Run the message dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemMessageDlg);
    MenuItemPageSetupDlg = new wxMenuItem(Menu5, idPageSetupDlg, _("wxPageSetupDialog"), _("Run the page setup dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemPageSetupDlg);
    MenuItemPasswordEntryDialog = new wxMenuItem(Menu5, idPasswordEntryDialog, _("wxPasswordEntryDialog"), _("Run the password entry dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemPasswordEntryDialog);
    MenuItemPrintDlg = new wxMenuItem(Menu5, idPrintDlg, _("wxPrintDialog"), _("Run the print dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemPrintDlg);
    MenuItemProgressDlg = new wxMenuItem(Menu5, idProgressDlg, _("wxProgressDialog"), _("Run the progress dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemProgressDlg);
    MenuItemRichTextFormattingDlg = new wxMenuItem(Menu5, idRichTextFormattingDlg, _("wxRichTextFormattingDialog"), _("Run the rich text formatting dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemRichTextFormattingDlg);
    MenuItemRichTextStyleOrganiserDlg = new wxMenuItem(Menu5, idRichTextStyleOrganiserDlg, _("wxRichTextStyleOrganiserDialog"), _("Run the rich text style organiser dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemRichTextStyleOrganiserDlg);
    MenuItemTextEntryDlg = new wxMenuItem(Menu5, idTextEntryDlg, _("wxTextEntryDialog"), _("Run the text entry dialogue."), wxITEM_NORMAL);
    Menu5->Append(MenuItemTextEntryDlg);
    MenuBar1->Append(Menu5, _("Dialogues"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, idCheckNetworkStatus, _("Check network status"), _("wxDialUpManager: Check network status."), wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuBar1->Append(Menu3, _("wx&DialUpmanager"));
    Menu4 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu4, idHtmlEasyPrint, _("Print"), _("wxHtmlEasyPrinting: Run the print dialogue."), wxITEM_NORMAL);
    Menu4->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(Menu4, idHtmlEasyPageSetup, _("Page &Setup..."), _("wxHtmlEasyPrinting: Run the page setup dialogue."), wxITEM_NORMAL);
    Menu4->Append(MenuItem5);
    MenuBar1->Append(Menu4, _("wx&HtmlEasyPrinting"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application."), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[3] = { -25, -10, -10 };
    int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(3,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(3,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    ImageList1 = new wxImageList(32, 32, 7);
    ImageList1->Add(wxBitmap(ImageList1_0_XPM));
    ImageList1->Add(wxBitmap(ImageList1_1_XPM));
    ImageList1->Add(wxBitmap(ImageList1_2_XPM));
    ImageList1->Add(wxBitmap(ImageList1_3_XPM));
    ImageList1->Add(wxBitmap(ImageList1_4_XPM));
    ImageList1->Add(wxBitmap(ImageList1_5_XPM));
    Image1 = new wxImage(Image1_XPM);
    Image1_BMP = new wxBitmap(Image1_XPM);
    Center();

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxSmithDemoFrame::OnButtonPlayAnimationClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxSmithDemoFrame::OnButtonStopAnimationClick);
    Connect(ID_SIMPLEHTMLLISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnSimpleHtmlListBox1Select);
    Connect(ID_SIMPLEHTMLLISTBOX1,wxEVT_COMMAND_HTML_CELL_CLICKED,(wxObjectEventFunction)&wxSmithDemoFrame::OnSimpleHtmlListBox1CellClicked);
    Connect(ID_SIMPLEHTMLLISTBOX1,wxEVT_COMMAND_HTML_CELL_HOVER,(wxObjectEventFunction)&wxSmithDemoFrame::OnSimpleHtmlListBox1CellHover);
    Connect(ID_SIMPLEHTMLLISTBOX1,wxEVT_COMMAND_HTML_LINK_CLICKED,(wxObjectEventFunction)&wxSmithDemoFrame::OnSimpleHtmlListBox1LinkClicked);
    Connect(ID_LISTVIEW1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnListView1ItemSelect);
    // Set the bitmaps for BitmapComboBox1.
    BitmapComboBox1->SetItemBitmap(0, ImageList1->GetBitmap(0));
    BitmapComboBox1->SetItemBitmap(1, ImageList1->GetBitmap(1));
    BitmapComboBox1->SetItemBitmap(2, ImageList1->GetBitmap(2));
    BitmapComboBox1->SetItemBitmap(3, ImageList1->GetBitmap(3));
    BitmapComboBox1->SetItemBitmap(4, ImageList1->GetBitmap(4));
    BitmapComboBox1->SetItemBitmap(5, ImageList1->GetBitmap(5));
    Connect(ID_BITMAPCOMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnBitmapComboBox1Selected);
    Connect(ID_BITMAPCOMBOBOX1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxSmithDemoFrame::OnBitmapComboBox1TextEnter);
    Connect(ID_MEDIACTRL1,wxEVT_MEDIA_PLAY,(wxObjectEventFunction)&wxSmithDemoFrame::OnMediaCtrl1Play);
    Connect(ID_MEDIACTRL1,wxEVT_MEDIA_PAUSE,(wxObjectEventFunction)&wxSmithDemoFrame::OnMediaCtrl1Pause);
    Connect(ID_MEDIACTRL1,wxEVT_MEDIA_STOP,(wxObjectEventFunction)&wxSmithDemoFrame::OnMediaCtrl1Stop);
    Connect(ID_MEDIACTRL1,wxEVT_MEDIA_FINISHED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMediaCtrl1Finished);
    Connect(ID_RICHTEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxSmithDemoFrame::OnRichTextCtrl1Text);
    Connect(ID_RICHTEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&wxSmithDemoFrame::OnRichTextCtrl1TextEnter);
    Connect(ID_RICHTEXTCTRL1,wxEVT_COMMAND_TEXT_URL,(wxObjectEventFunction)&wxSmithDemoFrame::OnRichTextCtrl1TextUrl);
    // Set the wxRichtTextCtrl for RichTextStyleListCtrl1.
    RichTextStyleListCtrl1->SetRichTextCtrl(RichTextCtrl1);
    // Set the wxRichTextStyleSheet for RichTextStyleListCtrl1.
    RichTextStyleListCtrl1->SetStyleSheet(richTextStyleSheet_1);
    RichTextStyleListCtrl1->UpdateStyles();
    // Set the wxRichtTextCtrl for RichTextStyleListBox1.
    RichTextStyleListBox1->SetRichTextCtrl(RichTextCtrl1);
    // Set the wxRichTextStyleSheet for RichTextStyleListBox1.
    RichTextStyleListBox1->SetStyleSheet(richTextStyleSheet_1);
    RichTextStyleListBox1->UpdateStyles();
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&wxSmithDemoFrame::OnNotebook1PageChanged);
    Connect(-1,wxEVT_COMMAND_FIND,(wxObjectEventFunction)&wxSmithDemoFrame::OnFindReplaceDialog1Find);
    Connect(-1,wxEVT_COMMAND_FIND_NEXT,(wxObjectEventFunction)&wxSmithDemoFrame::OnFindReplaceDialog1FindNext);
    Connect(-1,wxEVT_COMMAND_FIND_CLOSE,(wxObjectEventFunction)&wxSmithDemoFrame::OnFindReplaceDialog1Close);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnQuit);
    Connect(idFindReplaceDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemFindReplaceDlgSelected);
    Connect(idFontDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemFontDlgSelected);
    Connect(idMessageDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemMessageDlgSelected);
    Connect(idPageSetupDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemPageSetupDlgSelected);
    Connect(idPasswordEntryDialog,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemPasswordEntryDialogSelected);
    Connect(idPrintDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemPrintDlgSelected);
    Connect(idProgressDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemProgressDlgSelected);
    Connect(idRichTextFormattingDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemRichTextFormattingDlgSelected);
    Connect(idRichTextStyleOrganiserDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemRichTextStyleOrganiserDlgSelected);
    Connect(idTextEntryDlg,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemTextEntryDlgSelected);
    Connect(idCheckNetworkStatus,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnMenuItemCheckNetworkStatus);
    Connect(idHtmlEasyPrint,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnHtmlEasyPrint);
    Connect(idHtmlEasyPageSetup,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnHtmlEasyPageSetup);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&wxSmithDemoFrame::OnAbout);
    //*)

	Init();
}

wxSmithDemoFrame::~wxSmithDemoFrame()
{
}

void wxSmithDemoFrame::Init()
{
	CreateStyles();
	WriteInitialText();
	InitWithListItems();
	InitGridBag();
	InitTreebook();

#if wxUSE_STATUSBAR
    SetStatusText(wxT("wxDialUpManager:"), 1);
    SetStatusText(DialUpManager1->IsAlwaysOnline() ? _("LAN") : _("No LAN"), 2);
#endif

	// Let's use the new wxImage tool to obtain a bitmap for the static bitmap.
	StaticBitmap1->SetBitmap(wxBitmap(*Image1));
}

void wxSmithDemoFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void wxSmithDemoFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    info.SetName(wxT("wxSmithDemo"));
    info.SetVersion(wxString::Format(wxT("%d.%d.%d.%d %s"), AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::REVISION, wxString(AutoVersion::STATUS, wxConvUTF8).c_str()));
    info.SetDescription(_("This program demonstrates controls\nrecently added to wxSmith, the GUI\neditor plug-in for Code::Blocks IDE."));
    info.SetCopyright(_T("Copyright (C) 2010 Gary Harris"));
	info.AddDeveloper(wxT("Gary Harris"));
//	info.AddTranslator(wxT("Gary Harris"));
	info.SetName(wxTheApp->GetAppName());
	info.SetWebSite(wxT("http://wxsmithaddons.sourceforge.net/"));
	wxString sLicence(_("wxDemoApp is released under the\nterms of the wxWidgets Licence.\n\n"));
	sLicence.Append(_("Parts of the wxWidgets sample\napplications are used under\nthe terms of the\nwxWidgets licence.\n\n"));
	sLicence.Append(_("EarthImpactSml_512kb.mp4 is used\nunder the terms of the Creative\nCommons licence."));
	info.SetLicence(sLicence);

	wxAboutBox(info);
}

// -- Utility functions.
void wxSmithDemoFrame::LogString(wxString s)
{
	ListBoxLog->Append(s);
    ListBoxLog->SetFirstItem(ListBoxLog->GetCount() - 1);
}

// Create styles and add them to the pre-created wxRichTextStyleSheet and the wxRichTextCtrl.
void wxSmithDemoFrame::CreateStyles()
{
    // Paragraph styles
    wxFont romanFont(12, wxROMAN, wxNORMAL, wxNORMAL);
    wxFont swissFont(12, wxSWISS, wxNORMAL, wxNORMAL);

    wxRichTextParagraphStyleDefinition* normalPara = new wxRichTextParagraphStyleDefinition(wxT("Normal"));
    wxRichTextAttr normalAttr;
    normalAttr.SetFontFaceName(romanFont.GetFaceName());
    normalAttr.SetFontSize(12);
    // Let's set all attributes for this style
    normalAttr.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|
                            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|
                            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER);
    normalPara->SetStyle(normalAttr);

    richTextStyleSheet_1->AddParagraphStyle(normalPara);

    wxRichTextParagraphStyleDefinition* indentedPara = new wxRichTextParagraphStyleDefinition(wxT("Indented"));
    wxRichTextAttr indentedAttr;
    indentedAttr.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr.SetFontSize(12);
    indentedAttr.SetLeftIndent(100, 0);
    // We only want to affect indentation
    indentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    indentedPara->SetStyle(indentedAttr);

    richTextStyleSheet_1->AddParagraphStyle(indentedPara);

    wxRichTextParagraphStyleDefinition* indentedPara2 = new wxRichTextParagraphStyleDefinition(wxT("Red Bold Indented"));
    wxRichTextAttr indentedAttr2;
    indentedAttr2.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetFontWeight(wxBOLD);
    indentedAttr2.SetTextColour(*wxRED);
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetLeftIndent(100, 0);
    // We want to affect indentation, font and text colour
    indentedAttr2.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_FONT|wxTEXT_ATTR_TEXT_COLOUR);
    indentedPara2->SetStyle(indentedAttr2);

    richTextStyleSheet_1->AddParagraphStyle(indentedPara2);

    wxRichTextParagraphStyleDefinition* flIndentedPara = new wxRichTextParagraphStyleDefinition(wxT("First Line Indented"));
    wxRichTextAttr flIndentedAttr;
    flIndentedAttr.SetFontFaceName(swissFont.GetFaceName());
    flIndentedAttr.SetFontSize(12);
    flIndentedAttr.SetLeftIndent(100, -100);
    // We only want to affect indentation
    flIndentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    flIndentedPara->SetStyle(flIndentedAttr);

    richTextStyleSheet_1->AddParagraphStyle(flIndentedPara);

    // Character styles

    wxRichTextCharacterStyleDefinition* boldDef = new wxRichTextCharacterStyleDefinition(wxT("Bold"));
    wxRichTextAttr boldAttr;
    boldAttr.SetFontFaceName(romanFont.GetFaceName());
    boldAttr.SetFontSize(12);
    boldAttr.SetFontWeight(wxBOLD);
    // We only want to affect boldness
    boldAttr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
    boldDef->SetStyle(boldAttr);

    richTextStyleSheet_1->AddCharacterStyle(boldDef);

    wxRichTextCharacterStyleDefinition* italicDef = new wxRichTextCharacterStyleDefinition(wxT("Italic"));
    wxRichTextAttr italicAttr;
    italicAttr.SetFontFaceName(romanFont.GetFaceName());
    italicAttr.SetFontSize(12);
    italicAttr.SetFontStyle(wxITALIC);
    // We only want to affect italics
    italicAttr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
    italicDef->SetStyle(italicAttr);

    richTextStyleSheet_1->AddCharacterStyle(italicDef);

    wxRichTextCharacterStyleDefinition* redDef = new wxRichTextCharacterStyleDefinition(wxT("Red Bold"));
    wxRichTextAttr redAttr;
    redAttr.SetFontFaceName(romanFont.GetFaceName());
    redAttr.SetFontSize(12);
    redAttr.SetFontWeight(wxBOLD);
    redAttr.SetTextColour(*wxRED);
    // We only want to affect colour, weight and face
    redAttr.SetFlags(wxTEXT_ATTR_FONT_FACE|wxTEXT_ATTR_FONT_WEIGHT|wxTEXT_ATTR_TEXT_COLOUR);
    redDef->SetStyle(redAttr);

    richTextStyleSheet_1->AddCharacterStyle(redDef);

    wxRichTextListStyleDefinition* bulletList = new wxRichTextListStyleDefinition(wxT("Bullet List 1"));
    int i;
    for (i = 0; i < 10; i++)
    {
        wxString bulletText;
        if (i == 0)
            bulletText = wxT("standard/circle");
        else if (i == 1)
            bulletText = wxT("standard/square");
        else if (i == 2)
            bulletText = wxT("standard/circle");
        else if (i == 3)
            bulletText = wxT("standard/square");
        else
            bulletText = wxT("standard/circle");

        bulletList->SetAttributes(i, (i+1)*60, 60, wxTEXT_ATTR_BULLET_STYLE_STANDARD, bulletText);
    }

    richTextStyleSheet_1->AddListStyle(bulletList);

    wxRichTextListStyleDefinition* numberedList = new wxRichTextListStyleDefinition(wxT("Numbered List 1"));
    for (i = 0; i < 10; i++)
    {
        long numberStyle;
        if (i == 0)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;
        else if (i == 1)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 2)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 3)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        numberStyle |= wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT;

        numberedList->SetAttributes(i, (i+1)*60, 60, numberStyle);
    }

    richTextStyleSheet_1->AddListStyle(numberedList);

    wxRichTextListStyleDefinition* outlineList = new wxRichTextListStyleDefinition(wxT("Outline List 1"));
    for (i = 0; i < 10; i++)
    {
        long numberStyle;
        if (i < 4)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_OUTLINE|wxTEXT_ATTR_BULLET_STYLE_PERIOD;
        else
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        outlineList->SetAttributes(i, (i+1)*120, 120, numberStyle);
    }

    richTextStyleSheet_1->AddListStyle(outlineList);

    RichTextCtrl1->SetStyleSheet(richTextStyleSheet_1);
    //
    RichTextStyleComboCtrl1->SetStyleSheet(richTextStyleSheet_1);
    RichTextStyleComboCtrl1->UpdateStyles();
    RichTextStyleListBox1->SetStyleSheet(richTextStyleSheet_1);
    RichTextStyleListBox1->UpdateStyles();
}

// Initialise the wxRichTextCtrl.
void wxSmithDemoFrame::WriteInitialText()
{
    wxRichTextCtrl& r = *RichTextCtrl1;

    r.Newline();
	r.WriteText(wxT("This is "));
    r.WriteText(wxT("wxRichTextCtrl."));
    r.Newline();
    r.Newline();
	r.WriteText(wxT("The styles you set in wxSmith are used to set the basic styles for the control. Other styles are layered on top of them."));
	r.WriteText(wxT("You will still want to set and apply your detailed styles as per the wxWidgets Rich Text sample. The text below the line that follows is from Julian's wxWidgets sample application. "));
	r.WriteText(wxT("You can play around with the settings in wxSmith to see how they affect the display."));
    r.Newline();
    r.Newline();
    r.Newline();
    r.WriteText(wxT("====================================================="));
    r.Newline();
    r.Newline();

    r.SetDefaultStyle(wxRichTextAttr());

    r.BeginSuppressUndo();

    r.BeginParagraphSpacing(0, 20);

    r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    r.BeginBold();

    r.BeginFontSize(14);

    wxString lineBreak = (wxChar) 29;

    r.WriteText(wxString(wxT("Welcome to wxRichTextCtrl, a wxWidgets control")) + lineBreak + wxT("for editing and presenting styled text and images\n"));
    r.EndFontSize();
    //r.Newline();

    r.BeginItalic();
    r.WriteText(wxT("by Julian Smart"));
    r.EndItalic();

    r.EndBold();
    r.Newline();

    r.WriteImage(wxBitmap(zebra_xpm));

    r.Newline();
    r.Newline();

    r.EndAlignment();

    r.WriteText(wxT("What can you do with this thing? "));

    r.WriteImage(wxBitmap(smiley_xpm));
    r.WriteText(wxT(" Well, you can change text "));

    r.BeginTextColour(wxColour(255, 0, 0));
    r.WriteText(wxT("colour, like this red bit."));
    r.EndTextColour();

    wxRichTextAttr backgroundColourAttr;
    backgroundColourAttr.SetBackgroundColour(*wxGREEN);
    backgroundColourAttr.SetTextColour(wxColour(0, 0, 255));
    r.BeginStyle(backgroundColourAttr);
    r.WriteText(wxT(" And this blue on green bit."));
    r.EndStyle();

    r.WriteText(wxT(" Naturally you can make things "));
    r.BeginBold();
    r.WriteText(wxT("bold "));
    r.EndBold();
    r.BeginItalic();
    r.WriteText(wxT("or italic "));
    r.EndItalic();
    r.BeginUnderline();
    r.WriteText(wxT("or underlined."));
    r.EndUnderline();

    r.BeginFontSize(14);
    r.WriteText(wxT(" Different font sizes on the same line is allowed, too."));
    r.EndFontSize();

    r.WriteText(wxT(" Next we'll show an indented paragraph."));

    r.Newline();

    r.BeginLeftIndent(60);
    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.Newline();

    r.EndLeftIndent();

    r.WriteText(wxT("Next, we'll show a first-line indent, achieved using BeginLeftIndent(100, -40)."));

    r.Newline();

    r.BeginLeftIndent(100, -40);

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.Newline();

    r.EndLeftIndent();

    r.WriteText(wxT("Numbered bullets are possible, again using subindents:"));
    r.Newline();

    r.BeginNumberedBullet(1, 100, 60);
    r.WriteText(wxT("This is my first item. Note that wxRichTextCtrl can apply numbering and bullets automatically based on list styles, but this list is formatted explicitly by setting indents."));
    r.Newline();

    r.EndNumberedBullet();

    r.BeginNumberedBullet(2, 100, 60);
    r.WriteText(wxT("This is my second item."));
    r.Newline();

    r.EndNumberedBullet();

    r.WriteText(wxT("The following paragraph is right-indented:"));
    r.Newline();

    r.BeginRightIndent(200);

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.Newline();

    r.EndRightIndent();

    r.WriteText(wxT("The following paragraph is right-aligned with 1.5 line spacing:"));
    r.Newline();

    r.BeginAlignment(wxTEXT_ALIGNMENT_RIGHT);
    r.BeginLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);
    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.Newline();
    r.EndLineSpacing();
    r.EndAlignment();

    wxArrayInt tabs;
    tabs.Add(400);
    tabs.Add(600);
    tabs.Add(800);
    tabs.Add(1000);
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_TABS);
    attr.SetTabs(tabs);
    r.SetDefaultStyle(attr);

    r.WriteText(wxT("This line contains tabs:\tFirst tab\tSecond tab\tThird tab"));
    r.Newline();

    r.WriteText(wxT("Other notable features of wxRichTextCtrl include:"));
    r.Newline();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("Compatibility with wxTextCtrl API"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("Easy stack-based BeginXXX()...EndXXX() style setting in addition to SetStyle()"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("XML loading and saving"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("Undo/Redo, with batching option and Undo suppressing"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("Clipboard copy and paste"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("wxRichTextStyleSheet with named character and paragraph styles, and control for applying named styles"));
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.WriteText(wxT("A design that can easily be extended to other content types, ultimately with text boxes, tables, controls, and so on"));
    r.Newline();
    r.EndSymbolBullet();

    // Make a style suitable for showing a URL
    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

    r.WriteText(wxT("wxRichTextCtrl can also display URLs, such as this one: "));
    r.BeginStyle(urlStyle);
    r.BeginURL(wxT("http://www.wxwidgets.org"));
    r.WriteText(wxT("The wxWidgets Web Site"));
    r.EndURL();
    r.EndStyle();
    r.WriteText(wxT(". Click on the URL to generate an event."));

    r.Newline();

    r.WriteText(wxT("Note: this sample content was generated programmatically from within the MyFrame constructor in the demo. The images were loaded from inline XPMs. Enjoy wxRichTextCtrl!"));

    r.Newline();

    r.EndParagraphSpacing();

    r.EndSuppressUndo();
}

// Initialise the wxListView control.
void wxSmithDemoFrame::InitWithListItems()
{
    for(int i = 0; i < 30; i++ ){
        ListView1->InsertItem(i, wxString::Format(_T("Item %d"), i));
    }
}

// Initialise the wxTreebook.
void wxSmithDemoFrame::InitTreebook()
{
    wxPanel *Panel99 = new wxPanel(Treebook1 , ID_PANEL99, wxPoint(0, 0), wxDefaultSize, wxTAB_TRAVERSAL|wxSTATIC_BORDER, _T("ID_PANEL99"));
//	wxStaticText *text = new wxStaticText(Panel99, wxNewId(), wxString(_("This is a sub-page.")), wxPoint(88, 96));
	Treebook1->InsertSubPage(0, Panel99, wxT("1-1"));
	Treebook1->InsertSubPage(0, Panel99, wxT("1-2"));
	Treebook1->InsertSubPage(3, Panel99, wxT("2-1"));
	Treebook1->InsertSubPage(4, Panel99, wxT("2-1-1"));
	Treebook1->InsertSubPage(5, Panel99, wxT("2-1-1-1"));
}

// -- wxGridBagSizer
/*
// some simple macros to help make the sample code below more clear
#define TEXTCTRL(text)   new wxTextCtrl(Panel6, wxID_ANY, _T(text))
#define MLTEXTCTRL(text) new wxTextCtrl(Panel6, wxID_ANY, _T(text), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE)
#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

const wxChar gbsDescription[] =_T("\
The wxGridBagSizer is similar to the wxFlexGridSizer except the items are explicitly positioned\n\
in a virtual cell of the layout grid, and column or row spanning is allowed.  For example, this\n\
static text is positioned at (0,0) and it spans 7 columns.");

// Some IDs
enum {
    GBS_HIDE_BTN = 1212,
    GBS_SHOW_BTN,
    GBS_MOVE_BTN1,
    GBS_MOVE_BTN2,

    GBS_MAX
};
*/
void wxSmithDemoFrame::InitGridBag()
{
//    GridBagSizer2->Add( new wxStaticText(Panel6, wxID_ANY, gbsDescription), POS(0,0), SPAN(1, 7), wxALIGN_CENTER | wxALL, 5);
//
//    GridBagSizer2->Add( TEXTCTRL("pos(1,0)"),   POS(1,0) );
//    GridBagSizer2->Add( TEXTCTRL("pos(1,1)"),   POS(1,1) );
//    GridBagSizer2->Add( TEXTCTRL("pos(2,0)"),   POS(2,0) );
//    GridBagSizer2->Add( TEXTCTRL("pos(2,1)"),   POS(2,1) );
//    GridBagSizer2->Add( MLTEXTCTRL("pos(3,2), span(1,2)\nthis row and col are growable"), POS(3,2), SPAN(1,2), wxEXPAND );
//    GridBagSizer2->Add( MLTEXTCTRL("pos(4,3)\nspan(3,1)"), POS(4,3), SPAN(3,1), wxEXPAND );
//    GridBagSizer2->Add( TEXTCTRL("pos(5,4)"),   POS(5,4), wxDefaultSpan, wxEXPAND );
//    GridBagSizer2->Add( TEXTCTRL("pos(6,5)"),   POS(6,5), wxDefaultSpan, wxEXPAND );
//    GridBagSizer2->Add( TEXTCTRL("pos(7,6)"),   POS(7,6) );
//
//    //GridBagSizer2->Add( TEXTCTRL("bad position"), POS(4,3) );  // Test for assert
//    //GridBagSizer2->Add( TEXTCTRL("bad position"), POS(5,3) );  // Test for assert
//
//
//    wxButton *m_moveBtn1 = new wxButton(Panel6, GBS_MOVE_BTN1, _T("Move this to (3,6)"));
//    wxButton *m_moveBtn2 = new wxButton(Panel6, GBS_MOVE_BTN2, _T("Move this to (3,6)"));
//    GridBagSizer2->Add( m_moveBtn1, POS(10,2) );
//    GridBagSizer2->Add( m_moveBtn2, POS(10,3) );
//
//    wxButton *m_hideBtn = new wxButton(Panel6, GBS_HIDE_BTN, _T("Hide this item -->"));
//    GridBagSizer2->Add(m_hideBtn, POS(12, 3));
//
//    wxTextCtrl *m_hideTxt = new wxTextCtrl(Panel6, wxID_ANY, _T("pos(12,4), size(150, wxDefaultCoord)"), wxDefaultPosition, wxSize(150,wxDefaultCoord));
//    GridBagSizer2->Add( m_hideTxt, POS(12,4) );
//
//    wxButton *m_showBtn = new wxButton(Panel6, GBS_SHOW_BTN, _T("<-- Show it again"));
//    GridBagSizer2->Add(m_showBtn, POS(12, 5));
//    m_showBtn->Disable();
//
//    GridBagSizer2->Add(10,10, POS(14,0));
//
//    GridBagSizer2->AddGrowableRow(3);
//    GridBagSizer2->AddGrowableCol(2);
//
//    Panel6->SetSizerAndFit(GridBagSizer2);
////    SetClientSize(Panel6->GetSize());
    }

// -- wxNotebook

void wxSmithDemoFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
	if(event.GetSelection() == 6){
		LogString(wxString::Format(_("Rich Text Controls: Styles will be applied to the wxRichTextCtrl tab.")));
	}
}

//  -- wxDialUpManager --

void wxSmithDemoFrame::OnDialUpManager1Connected(wxDialUpEvent& event)
{
	wxMessageBox(_("Network connected."));
}

void wxSmithDemoFrame::OnDialUpManager1Disconnected(wxDialUpEvent& event)
{
	wxMessageBox(_("Network disconnected."));
}

void wxSmithDemoFrame::OnMenuItemCheckNetworkStatus(wxCommandEvent& event)
{
	if(DialUpManager1->IsOnline())
      wxLogMessage(_("Network is online."));
   else
      wxLogMessage(_("Network is offline."));
}

//  -- wxBitmapComboBox --

void wxSmithDemoFrame::OnBitmapComboBox1Selected(wxCommandEvent& event)
{
	wxString s = BitmapComboBox1->GetString(event.GetSelection());
	LogString(wxString::Format(_("wxBitmapComboBox: You selected \"%s\"."), s.c_str()));
}

void wxSmithDemoFrame::OnBitmapComboBox1TextEnter(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxBitmapComboBox: You pressed \"Enter\". Value: %s"), event.GetString().c_str()));
}

//  -- wxSimpleHtmlListBox --

void wxSmithDemoFrame::OnSimpleHtmlListBox1Select(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxSimpleHtmlListBox: Mouse clicked over item %d."), event.GetSelection()));
}

void wxSmithDemoFrame::OnSimpleHtmlListBox1CellClicked(wxHtmlCellEvent& event)
{
	LogString(wxString::Format(_("wxSimpleHtmlListBox: Click over cell %p at %d;%d."), event.GetCell(), event.GetPoint().x, event.GetPoint().y));

    // if we don't skip the event, OnSimpleHtmlListBox1LinkClicked() won't be called!
    event.Skip();
}

void wxSmithDemoFrame::OnSimpleHtmlListBox1CellHover(wxHtmlCellEvent& event)
{
	LogString(wxString::Format(_("wxSimpleHtmlListBox: The mouse moved over cell %p at %d;%d."), event.GetCell(), event.GetPoint().x, event.GetPoint().y));
}

void wxSmithDemoFrame::OnSimpleHtmlListBox1LinkClicked(wxHtmlLinkEvent& event)
{
	LogString(wxString::Format(_("wxSimpleHtmlListBox: The url '%s' was clicked."), event.GetLinkInfo().GetHref().c_str()));
}

//  -- wxSearchCtrl --

void wxSmithDemoFrame::OnSearchCtrl1SearchClicked(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxSearchCtrl: You clicked the Search button.")));
}

void wxSmithDemoFrame::OnSearchCtrl1CancelClicked(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxSearchCtrl: You clicked the Cancel button.")));
}

void wxSmithDemoFrame::OnSearchCtrl1TextEnter(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxSearchCtrl: You presses Enter.")));
}

void wxSmithDemoFrame::OnSearchCtrl1Text(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxSearchCtrl: You entered some text.")));
}

//  -- wxHtmlEasyPrinting --

void wxSmithDemoFrame::OnHtmlEasyPrint(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxHtmlEasyPrinting: Printing output to a directory called 'help' in the application directory.")));
	HtmlEasyPrinting1->PrintFile(wxT("html/subsup.html"));
}

void wxSmithDemoFrame::OnHtmlEasyPageSetup(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxHtmlEasyPrinting: Running the page setup dialogue.")));
	HtmlEasyPrinting1->PageSetup();
}

// -- wxRichTextCtrl

void wxSmithDemoFrame::OnRichTextCtrl1Text(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxRichTextCtrl: You entered some text.")));
}

void wxSmithDemoFrame::OnRichTextCtrl1TextEnter(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxRichTextCtrl: You pressed Enter.")));
}

void wxSmithDemoFrame::OnRichTextCtrl1TextUrl(wxTextUrlEvent& event)
{
	LogString(wxString::Format(_("wxRichTextCtrl: You clicked the URL.")));
}

// -- Menus
// - wxFindReplaceDialog

void wxSmithDemoFrame::OnMenuItemFindReplaceDlgSelected(wxCommandEvent& event)
{
	// You don't actually gain a lot by creating a wxFindReplaceDialog as it has to be recycled, anyway.
	// It does create the wxFindReplaceData object for you, though.
	// wxFindReplaceDialog is always, by design and implementation, modeless.
	LogString(wxString::Format(_("wxFindReplaceDialog: Running.")));
	if(!FindReplaceDialog1){
		FindReplaceDialog1 = new wxFindReplaceDialog(this, &findReplaceData_1, _("wxFindReplaceDialog"), wxFR_REPLACEDIALOG);
	}
	FindReplaceDialog1->Show();
}

// ***** IMPORTANT ***** IMPORTANT***** IMPORTANT***** IMPORTANT	***** IMPORTANT *****
//
// For events to work the id of your  wxFindReplaceDialog MUST be set to -1.
//
// ***** IMPORTANT ***** IMPORTANT***** IMPORTANT***** IMPORTANT	***** IMPORTANT *****

void wxSmithDemoFrame::OnFindReplaceDialog1Find(wxFindDialogEvent& event)
{
	LogString(wxString::Format(_("wxFindReplaceDialog: You clicked Find.")));
}

void wxSmithDemoFrame::OnFindReplaceDialog1FindNext(wxFindDialogEvent& event)
{
	LogString(wxString::Format(_("wxFindReplaceDialog: You clicked Find Next.")));
}

void wxSmithDemoFrame::OnFindReplaceDialog1Close(wxFindDialogEvent& event)
{
	LogString(wxString::Format(_("wxFindReplaceDialog: You pressed Cancel.")));
	wxDELETE(FindReplaceDialog1);
	wxASSERT(!FindReplaceDialog1);
}

// - wxFontDialog

void wxSmithDemoFrame::OnMenuItemFontDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxFontDialog: Running.")));
	FontDialog1->ShowModal();
}

// - wxMessageDialog

void wxSmithDemoFrame::OnMenuItemMessageDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxMessageDialog: Running.")));
	MessageDialog1->ShowModal();
}

// - wxPageSetupDialog

// Note: wxPageSetupDialog prefs set in wxSmith are only  valid for the first run. You  need to
// reconfigure the associated wxPageSetupDialogData object for subsequent runs.
void wxSmithDemoFrame::OnMenuItemPageSetupDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxPageSetupDialog: Running.")));
	if(PageSetupDialog1->ShowModal() == wxID_OK){
		wxPageSetupDialogData setupData = PageSetupDialog1->GetPageSetupData();
		if(!setupData.IsOk()){
			LogString(wxString::Format(_("wxPageSetupDialog: Error getting setup data.")));
			return;
		}
		wxPoint ptTL = setupData.GetMarginTopLeft();
		wxPoint ptBR = setupData.GetMarginBottomRight();
		LogString(wxString::Format(_("wxPageSetupDialog: Margins: Top %d Left %d Bottom %d  Right %d."), ptTL.y, ptTL.x, ptBR.y, ptBR.x));
		wxPaperSize paperID = setupData.GetPaperId();
		wxPrintPaperDatabase *paperDB = new wxPrintPaperDatabase;
		paperDB->CreateDatabase();
		wxString sPaper = paperDB->ConvertIdToName(paperID);
		if(!sPaper.IsEmpty()){
			LogString(wxString::Format(_("wxPageSetupDialog: Paper size: %s."), sPaper.c_str()));
		}
		else{
			LogString(wxString::Format(_("wxPageSetupDialog: Paper size: Not found in wxPrintPaperDatabase.")));
		}
		wxPrintData printData = setupData.GetPrintData();
		wxString sOrientation = printData.GetOrientation() == wxPORTRAIT ? wxString(_("Portrait")) : wxString(_("Landscape"));
		LogString(wxString::Format(_("wxPageSetupDialog: Paper size: %s."), sOrientation.c_str() ));

		wxDELETE(paperDB);
	}
	else{
		LogString(wxString::Format(_("wxPageSetupDialog: You pressed Cancel.")));
	}
}

// - wxPasswordEntryDialog

void wxSmithDemoFrame::OnMenuItemPasswordEntryDialogSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxPasswordEntryDialog: Running.")));
	if(PasswordEntryDialog1->ShowModal() == wxID_OK){
		LogString(wxString::Format(_("wxPasswordEntryDialog: You entered '%s'."), PasswordEntryDialog1->GetValue().c_str()));
	}
	else{
		LogString(wxString::Format(_("wxPasswordEntryDialog: You pressed Cancel.")));
	}
}

// - wxPrintDialog

// You use wxPageSetupDialog and wxPrintDialog in applications that print documents. If you use the printing framework
// (including wxPrintout, wxPrinter, and other classes), you won't need to invoke these dialogues explicitly in your code.
void wxSmithDemoFrame::OnMenuItemPrintDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxPrintDialog: Running.")));
	if(PrintDialog1->ShowModal() == wxID_OK){
		wxDC *dc = PrintDialog1->GetPrintDC();           // Get DC data from Printer Dialog.
		wxMemoryDC *dcMem = new wxMemoryDC;

		dc->SetDeviceOrigin(1000, 100); 						// Set the origin of the printed bitmap on the paper.
		dc->SetUserScale(10, 10);       							// Set printer scale: if (1, 1), each pixel will correspond to a dot on the printer; so,
																							// if your printer prints at 300 DPI, a 300 pixel wide image will be printed 1 inch wide.
		dc->StartDoc(_("printing..."));           						// Begin new document to be printed.
		dc->StartPage();                        								// Begin new page to be printed.

		dc->DrawText(wxString(_("wxSmithDemo Test Page")), 100, 20);
		dc->DrawLine(20, 30, 300, 30);
		wxBitmap bmp(*Image1);
		dcMem->SelectObject(bmp);               					// "Enable" bitmap for "blitting".
		dc->DrawText(wxString(_("This image was created in wxSmith using the new wxImage tool.")), 20, 50);
		dc->Blit(20, 70, 400, 400, dcMem, 0, 0); 			// Copy bitmap into printer DC.
		dc->DrawText(wxString(_("Lets boogie...")), 20, 120);
		wxPen pen(*wxRED, 2);
		dc->SetPen(pen);
		dc->DrawCircle(100, 300, 100);
		dc->SetPen(*wxCYAN_PEN);
		dc->SetBrush(*wxCYAN_BRUSH);
		dc->DrawRoundedRectangle(wxPoint(180, 200), wxSize(150, 80), 30);
		dc->SetPen(*wxLIGHT_GREY_PEN);
		dc->SetBrush(*wxLIGHT_GREY_BRUSH);
		dc->DrawEllipse(wxPoint(-40, 320), wxSize(180, 100));
		dc->SetTextForeground(*wxGREEN);
		dc->DrawRotatedText(wxString(_("Some text rotated to 57 degrees")), 100, 300, 57);
		dc->DrawBitmap(wxBitmap(wxT("media/zebra.xpm"), wxBITMAP_TYPE_XPM), 50, 450);
		dc->SetTextForeground(*wxBLACK);
		dc->DrawText(wxString(_("That's all folks.")), 20, 650);

		dcMem->SelectObject(wxNullBitmap);      			// Free the bitmap.
		dc->EndPage();
		dc->EndDoc();

		wxDELETE(dc);
		wxDELETE(dcMem);
	}
	else{
		LogString(wxString::Format(_("wxPrintDialog: You pressed Cancel.")));
	}
}

// - wxProgressDialog

void wxSmithDemoFrame::OnMenuItemProgressDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxProgressDialog: Running.")));

    int iMax = 50;

	// ***** IMPORTANT ***** IMPORTANT***** IMPORTANT***** IMPORTANT	***** IMPORTANT *****
	//
	// wxSmith either creates an uninitialised pointer or initialises and runs the dialogue at start-up,
	// depending on the state of m_bRunAtStartup.
	// There is no way to initialise the dialogue without having it run at the same time, by design.
	//
	// IN THIS EXAMPLE, THE DLG IS NOT SET TO RUN AT STARTUP SO IT MUST BE INITIALISED HERE.
	//
	// ***** IMPORTANT ***** IMPORTANT***** IMPORTANT***** IMPORTANT	***** IMPORTANT *****
	ProgressDialog1 = new wxProgressDialog(_T("wxProgressDialog"),
                            _T("An informative message"),
                            iMax,
                            this,
                            wxPD_AUTO_HIDE |
                            wxPD_APP_MODAL |
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME);

    for ( int i = 0; i <= iMax; i++ )
    {
        wxMilliSleep(200);

        wxString sMsg;

        // test both modes of wxProgressDialog behaviour: start in
        // indeterminate mode but switch to the determinate one later
        const bool bDeterminate = i > iMax/2;

        if ( i == iMax )
        {
            sMsg = _T("That's all, folks!");
        }
        else if ( !bDeterminate )
        {
            sMsg = _T("Testing indeterminate mode");
        }
        else if ( bDeterminate )
        {
            sMsg = _T("Now in standard determinate mode");
        }

        // will be set to true if "Skip" button was pressed
        bool bSkip = false;
        if ( bDeterminate )
        {
            ProgressDialog1->Update(i, sMsg, &bSkip);
        }
        else
        {
            ProgressDialog1->Pulse(sMsg, &bSkip);
        }
    }

    delete ProgressDialog1;
}

// - wxRichTextFormattingDialog

void wxSmithDemoFrame::OnMenuItemRichTextFormattingDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxRichTextFormattingDialog: Running. Styles will be applied to the wxRichTextCtrl tab.")));

	wxRichTextRange range;
	if(RichTextCtrl1->HasSelection()){
		range = RichTextCtrl1->GetSelectionRange();
	}
	else{
		range = wxRichTextRange(0, RichTextCtrl1->GetLastPosition()+1);
	}
	RichTextFormattingDialog1->GetStyle(RichTextCtrl1, range);

	if(RichTextFormattingDialog1->ShowModal() == wxID_OK){
		LogString(wxString::Format(_("wxRichTextFormattingDialog: Applying format.")));
		RichTextFormattingDialog1->ApplyStyle(RichTextCtrl1, range);
	}
	else{
		LogString(wxString::Format(_("wxRichTextFormattingDialog: You pressed Cancel.")));
	}
}

// - wxRichTextStyleOrganiserDialog

// The required wxRichTextStyleSheet is declared for you by wxSmith as richTextStyleSheet_1.
// Styles are created in CreateStyles() and added to the style list and the control there.
// wxRichTextStyleOrganiserDialog seems to have some issues in wx2.8.10.
// The list of styles doesn't display until you change the selection in the drop-down list at the bottom of the dlg
// and I can't get SetStyleSheet() to work at all.
void wxSmithDemoFrame::OnMenuItemRichTextStyleOrganiserDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxRichTextStyleOrganiserDialog: Running. Styles will be applied to the wxRichTextCtrl tab.")));
	RichTextStyleOrganiserDialog1->SetRichTextCtrl(RichTextCtrl1);
	RichTextStyleOrganiserDialog1->ShowModal();
}

// - wxTextEntryDialog

void wxSmithDemoFrame::OnMenuItemTextEntryDlgSelected(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxTextEntryDialog: Running.")));
	if(TextEntryDialog1->ShowModal() == wxID_OK){
		LogString(wxString::Format(_("wxTextEntryDialog: You entered '%s'."), TextEntryDialog1->GetValue().c_str()));
	}
	else{
		LogString(wxString::Format(_("wxTextEntryDialog: You pressed Cancel.")));
	}
}

// -- wxAnimationCtrl

void wxSmithDemoFrame::OnButtonPlayAnimationClick(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxAnimationCtrl: Playing animation.")));
	AnimationCtrl1->Play();
}

void wxSmithDemoFrame::OnButtonStopAnimationClick(wxCommandEvent& event)
{
	LogString(wxString::Format(_("wxAnimationCtrl: Stopping animation.")));
	AnimationCtrl1->Stop();
}

// -- wxListView

void wxSmithDemoFrame::OnListView1ItemSelect(wxListEvent& event)
{
	LogString(wxString::Format(_("wxListView: You clicked %s."), event.GetItem().GetText().c_str()));
}

// -- wxMediaCtrl

void wxSmithDemoFrame::OnMediaCtrl1Play(wxMediaEvent& event)
{
	LogString(wxString::Format(_("wxMediaCtrl: Starting playback.")));
}

void wxSmithDemoFrame::OnMediaCtrl1Pause(wxMediaEvent& event)
{
	LogString(wxString::Format(_("wxMediaCtrl: Pausing playback.")));
}

void wxSmithDemoFrame::OnMediaCtrl1Stop(wxMediaEvent& event)
{
	LogString(wxString::Format(_("wxMediaCtrl: Stopping playback.")));
}

void wxSmithDemoFrame::OnMediaCtrl1Finished(wxMediaEvent& event)
{
	LogString(wxString::Format(_("wxMediaCtrl: Playback finished.")));
}
