#ifndef WXSNEWWINDOWDLG_H
#define WXSNEWWINDOWDLG_H

//(*Headers(wxsNewWindowDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class wxsItemRes;
class wxsItemResData;
class wxsProject;

class wxsNewWindowDlg : public wxDialog
{
	public:
		wxsNewWindowDlg(wxWindow* parent,const wxString& ResType,wxsProject* Project);
		virtual ~wxsNewWindowDlg();

        //(*Identifiers(wxsNewWindowDlg)
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL1;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL3;
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL4;
        static const long ID_CHECKBOX3;
        static const long ID_BUTTON1;
        static const long ID_CHECKBOX2;
        static const long ID_COMBOBOX1;
        static const long ID_CHECKBOX4;
        static const long ID_TEXTCTRL5;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL6;
        static const long ID_STATICTEXT5;
        static const long ID_CHECKBOX5;
        static const long ID_CHECKBOX9;
        static const long ID_CHECKBOX6;
        static const long ID_CHECKBOX10;
        static const long ID_CHECKBOX7;
        static const long ID_CHECKBOX11;
        static const long ID_CHECKBOX8;
        static const long ID_CHECKBOX12;
        static const long ID_STATICTEXT6;
        static const long ID_TEXTCTRL7;
        static const long ID_CHECKBOX13;
        //*)

	protected:

        //(*Handlers(wxsNewWindowDlg)
        void OnCreate(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnClassChanged(wxCommandEvent& event);
        void OnSourceChanged(wxCommandEvent& event);
        void OnHeaderChanged(wxCommandEvent& evend);
        void OnUseXrcChange(wxCommandEvent& event);
        void OnXrcChanged(wxCommandEvent& event);
        void OnUsePCHClick(wxCommandEvent& event);
        void OnCtorParentClick(wxCommandEvent& event);
        void OnCtorIdClick(wxCommandEvent& event);
        void OnCtorPosClick(wxCommandEvent& event);
        void OnCtorSizeClick(wxCommandEvent& event);
        void OnCtorParentDefClick(wxCommandEvent& event);
        void OnCtorIdDefClick(wxCommandEvent& event);
        void OnCtorPosDefClick(wxCommandEvent& event);
        void OnCtorSizeDefClick(wxCommandEvent& event);
        void OnUseInitFuncClick(wxCommandEvent& event);
        void OnAdvOpsClick(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsNewWindowDlg)
        wxBoxSizer* m_RootSizer;
        wxStaticBoxSizer* StaticBoxSizer3;
        wxFlexGridSizer* FlexGridSizer1;
        wxTextCtrl* m_Class;
        wxTextCtrl* m_Header;
        wxTextCtrl* m_Source;
        wxCheckBox* m_UseXrc;
        wxTextCtrl* m_Xrc;
        wxCheckBox* m_XRCAutoload;
        wxButton* m_AdvOps;
        wxStaticBoxSizer* m_AdvancedOptionsSizer;
        wxFlexGridSizer* FlexGridSizer2;
        wxCheckBox* m_UsePCH;
        wxComboBox* m_Pch;
        wxCheckBox* m_UseInitFunc;
        wxTextCtrl* m_InitFunc;
        wxStaticText* StaticText4;
        wxTextCtrl* m_BaseClass;
        wxStaticText* StaticText5;
        wxFlexGridSizer* FlexGridSizer3;
        wxCheckBox* m_CtorParent;
        wxCheckBox* m_CtorParentDef;
        wxCheckBox* m_CtorId;
        wxCheckBox* m_CtorIdDef;
        wxCheckBox* m_CtorPos;
        wxCheckBox* m_CtorPosDef;
        wxCheckBox* m_CtorSize;
        wxCheckBox* m_CtorSizeDef;
        wxStaticText* StaticText6;
        wxTextCtrl* m_CtorCustom;
        wxCheckBox* m_AddWxs;
        wxStdDialogButtonSizer* StdDialogButtonSizer1;
        //*)

        virtual bool PrepareResource(wxsItemRes* Res,wxsItemResData* Data);

        wxString DetectPchFile();

	private:

        bool m_SourceNotTouched;
        bool m_HeaderNotTouched;
        bool m_XrcNotTouched;
        bool m_BlockText;
        bool m_AdvOpsShown;
        bool m_AppManaged;
        wxString m_Type;
        wxsProject* m_Project;

        DECLARE_EVENT_TABLE()
};

#endif
