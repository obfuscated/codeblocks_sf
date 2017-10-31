#ifndef LIBRARIESDLG_H
#define LIBRARIESDLG_H

//(*Headers(LibrariesDlg)
#include "scrollingdialog.h"
class wxTextCtrl;
class wxStaticBoxSizer;
class wxFlatNotebook;
class wxListBox;
class wxFlexGridSizer;
class wxButton;
class wxStdDialogButtonSizer;
class wxBoxSizer;
class wxStaticText;
class wxPanel;
class wxCheckBox;
//*)

#include "resultmap.h"
#include "librarydetectionmanager.h"

class LibrariesDlg: public wxScrollingDialog
{
    public:

        LibrariesDlg(wxWindow* parent,TypedResults& knownLibraries);
        virtual ~LibrariesDlg();

    private:

        //(*Declarations(LibrariesDlg)
        wxTextCtrl* m_ObjectsDir;
        wxCheckBox* m_ShowPredefined;
        wxButton* m_ConfDelete;
        wxTextCtrl* m_LinkerDir;
        wxTextCtrl* m_PkgConfigName;
        wxPanel* Panel6;
        wxPanel* Panel1;
        wxPanel* Panel7;
        wxTextCtrl* m_Name;
        wxTextCtrl* m_LFlags;
        wxTextCtrl* m_CFlags;
        wxTextCtrl* m_Required;
        wxButton* Button1;
        wxButton* Button2;
        wxButton* Button3;
        wxPanel* Panel9;
        wxPanel* Panel8;
        wxTextCtrl* m_Compilers;
        wxListBox* m_Configurations;
        wxStaticText* StaticText1;
        wxStaticText* StaticText10;
        wxPanel* Panel10;
        wxPanel* Panel2;
        wxStaticText* m_Type;
        wxButton* Button8;
        wxStaticText* StaticText3;
        wxPanel* Panel4;
        wxTextCtrl* m_Categories;
        wxPanel* Panel5;
        wxButton* m_ConfigUp;
        wxTextCtrl* m_Headers;
        wxTextCtrl* m_CompilerDirs;
        wxButton* Button5;
        wxStaticText* StaticText8;
        wxCheckBox* m_ShowPkgConfig;
        wxTextCtrl* m_Defines;
        wxPanel* Panel3;
        wxStaticText* StaticText7;
        wxTextCtrl* m_BasePath;
        wxStaticText* StaticText4;
        wxListBox* m_Libraries;
        wxButton* m_ConfigDown;
        wxStaticText* StaticText5;
        wxStaticText* StaticText2;
        wxButton* m_ConfDuplicate;
        wxTextCtrl* m_Description;
        wxStaticText* StaticText6;
        wxTextCtrl* m_Libs;
        wxButton* Button7;
        wxStaticText* StaticText9;
        wxButton* Button6;
        wxStaticText* StaticText11;
        wxFlatNotebook* FlatNotebook1;
        //*)

        //(*Identifiers(LibrariesDlg)
        static const long ID_LISTBOX1;
        static const long ID_CHECKBOX1;
        static const long ID_CHECKBOX2;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON11;
        static const long ID_BUTTON8;
        static const long ID_LISTBOX2;
        static const long ID_BUTTON9;
        static const long ID_BUTTON10;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL1;
        static const long ID_STATICTEXT5;
        static const long ID_TEXTCTRL4;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL3;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL2;
        static const long ID_PANEL1;
        static const long ID_TEXTCTRL13;
        static const long ID_PANEL8;
        static const long ID_TEXTCTRL5;
        static const long ID_PANEL6;
        static const long ID_TEXTCTRL8;
        static const long ID_PANEL3;
        static const long ID_TEXTCTRL12;
        static const long ID_PANEL5;
        static const long ID_STATICTEXT6;
        static const long ID_TEXTCTRL9;
        static const long ID_BUTTON5;
        static const long ID_STATICTEXT7;
        static const long ID_TEXTCTRL10;
        static const long ID_BUTTON6;
        static const long ID_STATICTEXT8;
        static const long ID_TEXTCTRL11;
        static const long ID_BUTTON7;
        static const long ID_PANEL4;
        static const long ID_TEXTCTRL6;
        static const long ID_PANEL7;
        static const long ID_TEXTCTRL7;
        static const long ID_PANEL2;
        static const long ID_STATICTEXT11;
        static const long ID_TEXTCTRL14;
        static const long ID_PANEL9;
        static const long ID_STATICTEXT12;
        static const long ID_TEXTCTRL15;
        static const long ID_PANEL10;
        static const long ID_FLATNOTEBOOK1;
        //*)

        //(*Handlers(LibrariesDlg)
        void OnInit(wxInitDialogEvent& event);
        void OnButton8Click(wxCommandEvent& event);
        void Onm_ShowPredefinedClick(wxCommandEvent& event);
        void Onm_ShowPkgConfigClick(wxCommandEvent& event);
        void Onm_LibrariesSelect(wxCommandEvent& event);
        void Onm_ConfDeleteClick(wxCommandEvent& event);
        void OnWrite(wxCommandEvent& event);
        void Onm_ConfigurationsSelect(wxCommandEvent& event);
        void Onm_ConfDuplicateClick(wxCommandEvent& event);
        void Onm_NameText(wxCommandEvent& event);
        void Onm_CompilersText(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void Onm_ConfigPosChangeDown(wxCommandEvent& event);
        void Onm_ConfigPosChangeUp(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        //*)

        TypedResults& m_KnownLibraries;
        TypedResults m_WorkingCopy;
        wxString m_SelectedShortcut;
        LibraryResult* m_SelectedConfig;
        bool m_WhileUpdating;

        bool LoadSearchFilters(LibraryDetectionManager* CfgManager);
        void RecreateLibrariesList(const wxString& Selection);
        void RecreateLibrariesListForceRefresh();
        void SelectLibrary(const wxString& Shortcut);
        void SelectConfiguration(LibraryResult* Configuration);
        void CopyFromKnown();
        void CopyToKnown();
        wxString GetDesc(LibraryResult* Configuration);
        void StoreConfiguration();
        void RefreshConfigurationName();

        DECLARE_EVENT_TABLE()
};

#endif
