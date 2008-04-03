#ifndef LIBRARIESDLG_H
#define LIBRARIESDLG_H

//(*Headers(LibrariesDlg)
#include <wx/dialog.h>
class wxPanel;
class wxStdDialogButtonSizer;
class wxCheckBox;
class wxTextCtrl;
class wxListBox;
class wxStaticText;
class wxFlexGridSizer;
class wxBoxSizer;
class wxButton;
class wxStaticBoxSizer;
class wxFlatNotebook;
//*)

#include "resultmap.h"
#include "libraryconfigmanager.h"

class LibrariesDlg: public wxDialog
{
    public:

        LibrariesDlg(wxWindow* parent,TypedResults& knownLibraries);
        virtual ~LibrariesDlg();

    private:

        //(*Declarations(LibrariesDlg)
        wxStaticText* StaticText10;
        wxTextCtrl* m_ObjectsDir;
        wxStaticText* StaticText9;
        wxPanel* Panel5;
        wxTextCtrl* m_LinkerDir;
        wxCheckBox* m_ShowPkgConfig;
        wxTextCtrl* m_LFlags;
        wxTextCtrl* m_PkgConfigName;
        wxButton* m_ConfigDown;
        wxStaticText* StaticText2;
        wxPanel* Panel4;
        wxTextCtrl* m_Libs;
        wxButton* m_ConfDuplicate;
        wxTextCtrl* m_BasePath;
        wxButton* Button1;
        wxStaticText* StaticText6;
        wxTextCtrl* m_Headers;
        wxPanel* Panel9;
        wxStaticText* StaticText8;
        wxPanel* Panel8;
        wxTextCtrl* m_Compilers;
        wxPanel* Panel1;
        wxStaticText* StaticText1;
        wxStaticText* m_Type;
        wxStaticText* StaticText3;
        wxTextCtrl* m_Categories;
        wxTextCtrl* m_Defines;
        wxListBox* m_Libraries;
        wxButton* Button2;
        wxPanel* Panel6;
        wxTextCtrl* m_CompilerDirs;
        wxPanel* Panel3;
        wxButton* Button6;
        wxButton* m_ConfDelete;
        wxFlatNotebook* FlatNotebook1;
        wxButton* Button5;
        wxStaticText* StaticText5;
        wxButton* m_ConfigUp;
        wxButton* Button7;
        wxStaticText* StaticText7;
        wxPanel* Panel7;
        wxListBox* m_Configurations;
        wxTextCtrl* m_Description;
        wxPanel* Panel2;
        wxTextCtrl* m_CFlags;
        wxStaticText* StaticText4;
        wxButton* Button8;
        wxTextCtrl* m_Name;
        wxCheckBox* m_ShowPredefined;
        //*)

        //(*Identifiers(LibrariesDlg)
        static const long ID_LISTBOX1;
        static const long ID_CHECKBOX1;
        static const long ID_CHECKBOX2;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
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
        //*)

        TypedResults& m_KnownLibraries;
        TypedResults m_WorkingCopy;
        wxString m_SelectedShortcut;
        LibraryResult* m_SelectedConfig;
        bool m_WhileUpdating;

        bool LoadSearchFilters(LibraryConfigManager* CfgManager);
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
