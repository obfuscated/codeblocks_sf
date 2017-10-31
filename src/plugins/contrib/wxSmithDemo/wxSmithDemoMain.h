/***************************************************************
 * Name:      wxSmithDemoMain.h
 * Purpose:   Defines Application Frame
 * Author:    Gary Harris (garyjharris@sourceforge.net)
 * Created:   2010-06-01
 * Copyright: Gary Harris (http://cryogen.66ghz.com/)
 * License:
 **************************************************************/

#ifndef WXSMITHDEMOMAIN_H
#define WXSMITHDEMOMAIN_H

//(*Headers(wxSmithDemoFrame)
#include <wx/listctrl.h>
#include <wx/srchctrl.h>
#include <wx/fdrepdlg.h>
#include <wx/fontdlg.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/richtext/richtextformatdlg.h>
#include <wx/printdlg.h>
#include <wx/cshelp.h>
#include <wx/menu.h>
#include <wx/htmllbox.h>
#include <wx/filepicker.h>
#include "wximages/wxSmithDemoMain_ImageList1.xpm"
#include <wx/listbox.h>
#include <wx/bitmap.h>
#include <wx/textdlg.h>
#include <wx/richtext/richtextstyles.h>
#include <wx/html/htmprint.h>
#include <wx/treebook.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/mediactrl.h>
#include <wx/bmpcbox.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/dialup.h>
#include <wx/imaglist.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include "wximages/wxSmithDemoMain_Image1_XPM.xpm"
#include  <wx/richtext/richtextstyledlg.h>
#include <wx/image.h>
#include <wx/animate.h>
#include <wx/frame.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/statusbr.h>
//*)

#include <wx/msgdlg.h>

class wxSmithDemoFrame: public wxFrame
{
    public:

        wxSmithDemoFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxSmithDemoFrame();

    private:

        //(*Handlers(wxSmithDemoFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnDialUpManager1Connected(wxDialUpEvent& event);
        void OnDialUpManager1Disconnected(wxDialUpEvent& event);
        void OnMenuItemCheckNetworkStatus(wxCommandEvent& event);
        void OnBitmapComboBox1Selected(wxCommandEvent& event);
        void OnBitmapComboBox1TextEnter(wxCommandEvent& event);
        void OnSimpleHtmlListBox1Select(wxCommandEvent& event);
        void OnSimpleHtmlListBox1CellClicked(wxHtmlCellEvent& event);
        void OnSimpleHtmlListBox1CellHover(wxHtmlCellEvent& event);
        void OnSimpleHtmlListBox1LinkClicked(wxHtmlLinkEvent& event);
        void OnSearchCtrl1SearchClicked(wxCommandEvent& event);
        void OnSearchCtrl1CancelClicked(wxCommandEvent& event);
        void OnSearchCtrl1TextEnter(wxCommandEvent& event);
        void OnSearchCtrl1Text(wxCommandEvent& event);
        void OnHtmlEasyPrint(wxCommandEvent& event);
        void OnHtmlEasyPageSetup(wxCommandEvent& event);
        void OnRichTextCtrl1Text(wxCommandEvent& event);
        void OnRichTextCtrl1TextEnter(wxCommandEvent& event);
        void OnRichTextCtrl1TextUrl(wxTextUrlEvent& event);
        void OnMenuItemFindReplaceDlgSelected(wxCommandEvent& event);
        void OnMenuItemFontDlgSelected(wxCommandEvent& event);
        void OnMenuItemMessageDlgSelected(wxCommandEvent& event);
        void OnMenuItemPageSetupDlgSelected(wxCommandEvent& event);
        void OnMenuItemPasswordEntryDialogSelected(wxCommandEvent& event);
        void OnMenuItemPrintDlgSelected(wxCommandEvent& event);
        void OnMenuItemProgressDlgSelected(wxCommandEvent& event);
        void OnMenuItemRichTextFormattingDlgSelected(wxCommandEvent& event);
        void OnMenuItemRichTextStyleOrganiserDlgSelected(wxCommandEvent& event);
        void OnMenuItemTextEntryDlgSelected(wxCommandEvent& event);
        void OnFindReplaceDialog1Close(wxFindDialogEvent& event);
        void OnFindReplaceDialog1FindNext(wxFindDialogEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton14Click(wxCommandEvent& event);
        void OnButtonPlayAnimationClick(wxCommandEvent& event);
        void OnButtonStopAnimationClick(wxCommandEvent& event);
        void OnListView1ItemSelect(wxListEvent& event);
        void OnPanel8LeftDown(wxMouseEvent& event);
        void OnMediaCtrl1Play(wxMediaEvent& event);
        void OnMediaCtrl1Pause(wxMediaEvent& event);
        void OnMediaCtrl1Stop(wxMediaEvent& event);
        void OnMediaCtrl1Finished(wxMediaEvent& event);
        void OnFindReplaceDialog1Find(wxFindDialogEvent& event);
        void OnNotebook1PageChanged(wxNotebookEvent& event);
        //*)

        //(*Identifiers(wxSmithDemoFrame)
        static const long ID_ANIMATIONCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_PANEL7;
        static const long ID_SIMPLEHTMLLISTBOX1;
        static const long ID_LISTVIEW1;
        static const long ID_BITMAPCOMBOBOX1;
        static const long ID_STATICBITMAP1;
        static const long ID_SEARCHCTRL;
        static const long ID_PANEL4;
        static const long ID_MEDIACTRL1;
        static const long ID_PANEL8;
        static const long ID_COLOURPICKERCTRL1;
        static const long ID_COLOURPICKERCTRL2;
        static const long ID_COLOURPICKERCTRL3;
        static const long ID_FILEPICKERCTRL1;
        static const long ID_DIRPICKERCTRL1;
        static const long ID_FONTPICKERCTRL1;
        static const long ID_PANEL2;
        static const long ID_RICHTEXTCTRL1;
        static const long ID_PANEL3;
        static const long ID_STATICTEXT1;
        static const long ID_PANEL12;
        static const long ID_STATICTEXT5;
        static const long ID_PANEL13;
        static const long ID_STATICTEXT6;
        static const long ID_PANEL14;
        static const long ID_TREEBOOK1;
        static const long ID_PANEL11;
        static const long ID_RICHTEXTSTYLELISTCTRL1;
        static const long ID_RICHTEXTSTYLECOMBOCTRL1;
        static const long ID_RICHTEXTSTYLELISTBOX1;
        static const long ID_PANEL5;
        static const long ID_NOTEBOOK1;
        static const long ID_LISTBOX1;
        static const long ID_PANEL1;
        static const long ID_DIALUPMANAGER1;
        static const long ID_MESSAGEDIALOG1;
        static const long ID_PASSWORDENTRYDIALOG1;
        static const long ID_PROGRESSDIALOG1;
        static const long ID_RICHTEXTFORMATTINGDIALOG1;
        static const long ID_RICHTEXTSTYLEORGANISERDIALOG1;
        static const long ID_TEXTENTRYDIALOG1;
        static const long idMenuQuit;
        static const long idFindReplaceDlg;
        static const long idFontDlg;
        static const long idMessageDlg;
        static const long idPageSetupDlg;
        static const long idPasswordEntryDialog;
        static const long idPrintDlg;
        static const long idProgressDlg;
        static const long idRichTextFormattingDlg;
        static const long idRichTextStyleOrganiserDlg;
        static const long idTextEntryDlg;
        static const long idCheckNetworkStatus;
        static const long idHtmlEasyPrint;
        static const long idHtmlEasyPageSetup;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)
        static const long ID_PANEL99;

        //(*Declarations(wxSmithDemoFrame)
        wxDirPickerCtrl* DirPickerCtrl1;
        wxMenuItem* MenuItemPageSetupDlg;
        wxRichTextStyleComboCtrl* RichTextStyleComboCtrl1;
        wxBitmapComboBox* BitmapComboBox1;
        wxMenuItem* MenuItemFontDlg;
        wxPanel* Panel5;
        wxBitmap              *Image1_BMP;
        wxFontPickerCtrl* FontPickerCtrl1;
        wxMenuItem* MenuItemRichTextStyleOrganiserDlg;
        wxNotebook* Notebook1;
        wxRichTextStyleSheet  *richTextStyleSheet_1;
        wxPrintDialog* PrintDialog1;
        wxMediaCtrl* MediaCtrl1;
        wxMenuItem* MenuItemTextEntryDlg;
        wxFilePickerCtrl* FilePickerCtrl1;
        wxMenuItem* MenuItemFindReplaceDlg;
        wxMenuItem* MenuItem5;
        wxPanel* Panel4;
        wxRichTextCtrl* RichTextCtrl1;
        wxFindReplaceDialog* FindReplaceDialog1;
        wxDialUpManager* DialUpManager1;
        wxMenu* Menu3;
        wxStaticText* StaticText6;
        wxRichTextStyleListCtrl* RichTextStyleListCtrl1;
        wxStaticBitmap* StaticBitmap1;
        wxMenuItem* MenuItemProgressDlg;
        wxMenuItem* MenuItem4;
        wxPanel* Panel11;
        wxPageSetupDialogData  *pageSetupDialogData_1;
        wxFindReplaceData findReplaceData_1;
        wxImageList* ImageList1;
        wxPanel* Panel8;
        wxButton* ButtonStopAnimation;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxMenuItem* MenuItemPrintDlg;
        wxColourPickerCtrl* ColourPickerCtrl1;
        wxFontDialog* FontDialog1;
        wxPasswordEntryDialog* PasswordEntryDialog1;
        wxPanel* Panel3;
        wxMenuItem* MenuItemPasswordEntryDialog;
        wxColourPickerCtrl* ColourPickerCtrl3;
        wxListBox* ListBoxLog;
        wxHtmlEasyPrinting* HtmlEasyPrinting1;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItemMessageDlg;
        wxPrintDialogData  *printDialogData_1;
        wxStaticText* StaticText5;
        wxPanel* Panel7;
        wxContextHelpButton* ContextHelpButton1;
        wxStatusBar* StatusBar1;
        wxListView* ListView1;
        wxColourPickerCtrl* ColourPickerCtrl2;
        wxTreebook* Treebook1;
        wxTextEntryDialog* TextEntryDialog1;
        wxPageSetupDialog* PageSetupDialog1;
        wxRichTextStyleListBox* RichTextStyleListBox1;
        wxAnimationCtrl* AnimationCtrl1;
        wxPanel* Panel14;
        wxProgressDialog* ProgressDialog1;
        wxPanel* Panel2;
        wxSimpleHtmlListBox* SimpleHtmlListBox1;
        wxMessageDialog* MessageDialog1;
        wxRichTextFormattingDialog* RichTextFormattingDialog1;
        wxRichTextStyleOrganiserDialog* RichTextStyleOrganiserDialog1;
        wxPanel* Panel12;
        wxMenuItem* MenuItemRichTextFormattingDlg;
        wxImage               *Image1;
        wxMenu* Menu5;
        wxMenu* Menu4;
        wxSearchCtrl* SearchCtrl1;
        wxPanel* Panel13;
        wxButton* ButtonPlayAnimation;
        //*)

		void LogString(wxString s);
		void CreateStyles();

		void WriteInitialText();
		void Init();
		void InitWithListItems();
		void InitGridBag();
		void InitTreebook();

        DECLARE_EVENT_TABLE()
};

#endif // WXSMITHDEMOMAIN_H
