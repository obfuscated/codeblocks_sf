//{Info
/*
 ** Purpose:   Code::Blocks - Autoversioning Plugin
 ** Author:    JGM
 ** Created:   06/29/07 02:48:59 p.m.
 ** Copyright: (c) JGM
 ** License:   GPL
 */
//}

#ifndef DLGVERSIONINTIALIZER_H
#define DLGVERSIONINTIALIZER_H

#include <typeinfo>

//(*Headers(avVersionEditorDlg)
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/panel.h>
#include "scrollingdialog.h"
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobox.h>
#include <wx/timer.h>
//*)

#include <wx/event.h>

class avVersionEditorDlg: public wxScrollingDialog
{
private:
    long m_major;
    long m_minor;
    long m_build;
    long m_count;
    long m_revision;
    bool m_autoMajorMinor;
    bool m_dates;
	// GJH 03/03/10 Added manifest updating.
    bool m_updateManifest;
    bool m_useDefine;
    bool m_svn;
    bool m_commit;
    bool m_askCommit;
    long m_minorMaximun;
    long m_buildMaximun;
    long m_revisionMaximun;
    long m_revisionRandomMaximun;
    long m_buildTimesToMinorIncrement;
    long m_changes;
    wxString m_headerGuard;
    wxString m_namespace;
    wxString m_prefix;
    wxString m_svnDirectory;
    wxString m_status;
    wxString m_statusAbbreviation;
    wxString m_changesTitle;
    wxString m_language;
    wxString m_headerPath;
    wxString m_changesLogPath;

	void ValidateInput();

	DECLARE_EVENT_TABLE()


public:
		avVersionEditorDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~avVersionEditorDlg();

		//(*Identifiers(avVersionEditorDlg)
		static const long ID_MAJOR_LABEL;
		static const long ID_MAJOR_TEXT;
		static const long ID_MINOR_LABEL;
		static const long ID_MINOR_TEXT;
		static const long ID_BUILD_LABEL;
		static const long ID_BUILD_TEXT;
		static const long ID_REVISION_LABEL;
		static const long ID_REVISION_TEXT;
		static const long ID_STATICLINE2;
		static const long ID_COUNT_LABEL;
		static const long ID_COUNT_TEXT;
		static const long ID_VALUES_PANEL;
		static const long ID_SATUS_LABEL;
		static const long ID_STATUS_COMBOBOX;
		static const long ID_STATICLINE4;
		static const long ID_STATICTEXT1;
		static const long ID_ABBREVIATION_COMBOBOX;
		static const long ID_STATUS_PANEL;
		static const long ID_MINORMAXIMUN_LABEL;
		static const long ID_MINORMAXIMUM_TEXT;
		static const long ID_BUILDNUMBERMAX_LABEL;
		static const long ID_BUILDNUMBERMAX_TEXT;
		static const long ID_REVISIONMAX_LABEL;
		static const long ID_REVISIONMAX_TEXT;
		static const long ID_REVISIONRANDOM_LABEL;
		static const long ID_REVISIONRANDOM_TEXT;
		static const long ID_BUILDTIMES_LABEL;
		static const long ID_BUILDTIMES_TEXT;
		static const long ID_SCHEME_PANEL;
		static const long ID_HEADER_GUARD_LABEL;
		static const long ID_HEADER_GUARD_TEXT;
		static const long ID_NAMESPACE_LABEL;
		static const long ID_NAMESPACE_TEXT;
		static const long ID_PREFIX_LABEL;
		static const long ID_PREFIX_TEXT;
		static const long ID_CODE_PANEL;
		static const long ID_AUTO_CHECK;
		static const long ID_DATES_CHECK;
		static const long ID_DEFINE_CHECK;
		static const long ID_UPDATE_MANIFEST;
		static const long ID_COMMIT_CHECK;
		static const long ID_ASKCOMMIT_CHECK;
		static const long ID_STATICLINE3;
		static const long ID_HEADERPATH_LABEL;
		static const long ID_HEADERPATH_TEXTCTRL;
		static const long ID_HEADERPATH_BUTTON;
		static const long ID_HEADERLANGUAGE_RADIOBOX;
		static const long ID_STATICLINE1;
		static const long ID_SVN_CHECK;
		static const long ID_SVNDIR_TEXT;
		static const long ID_SVNDIR_BUTTON;
		static const long ID_SETTINGS_PANEL;
		static const long ID_GENERATECHANGES_CHECKBOX;
		static const long ID_CHANGESPATH_STATICTEXT;
		static const long ID_CHANGESLOGPATH_TEXTCTRL;
		static const long ID_CHANGESLOGPATH_BUTTON;
		static const long ID_FORMAT_STATICTEXT;
		static const long ID_CHANGESTITLE_TEXTCTRL;
		static const long ID_FORMATS_STATICTEXT;
		static const long ID_CHANGES_PANEL;
		static const long ID_AV_NOTEBOOK;
		static const long ID_STATICTEXT2;
		static const long ID_ACCEPT;
		static const long ID_CANCEL;
		static const long ID_VALIDATE_TIMER;
		//*)

	protected:

		//(*Handlers(avVersionEditorDlg)
		void OnAcceptClick(wxCommandEvent& event);
		void OnCancelClick(wxCommandEvent& event);
		void OnSvnCheck(wxCommandEvent& event);
		void OnSvnDirectoryClick(wxCommandEvent& event);
		void OnChkCommitClick(wxCommandEvent& event);
		void OnChoStatusSelect(wxCommandEvent& event);
		void OnChoAbbreviationSelect(wxCommandEvent& event);
		void OnCmbStatusSelect(wxCommandEvent& event);
		void OnCmbAbbreviationSelect(wxCommandEvent& event);
		void OnChkChangesClick(wxCommandEvent& event);
		void OnnbAutoVersioningPageChanged(wxNotebookEvent& event);
		void OnTxtRevisionRandomText(wxCommandEvent& event);
		void OnTmrValidateInputTrigger(wxTimerEvent& event);
		void OnTextChanged(wxCommandEvent& event);
		void OnMouseEnter(wxMouseEvent& event);
		void OnHeaderPathClick(wxCommandEvent& event);
		void OnChangesLogPathClick(wxCommandEvent& event);
		//*)

		//(*Declarations(avVersionEditorDlg)
		wxPanel* pnlCode;
		wxButton* btnAccept;
		wxBoxSizer* buttonsSizer;
		wxCheckBox* chkAutoIncrement;
		wxStaticText* lblBuildTimes;
		wxRadioBox* rbHeaderLanguage;
		wxNotebook* nbAutoVersioning;
		wxStaticText* lblChangesFormats;
		wxButton* btnChangesLogPath;
		wxBoxSizer* BoxSizer3;
		wxStaticLine* StaticLine2;
		wxTextCtrl* txtChangesLogPath;
		wxStaticText* lblRevisionMax;
		wxBoxSizer* statusSizer;
		wxPanel* pnlVersionValues;
		wxTextCtrl* txtChangesTitle;
		wxCheckBox* chkSvn;
		wxBoxSizer* BoxSizer7;
		wxStaticText* lblCount;
		wxComboBox* cmbStatus;
		wxCheckBox* chkChanges;
		wxCheckBox* chkAskCommit;
		wxPanel* pnlSettings;
		wxBoxSizer* minorMaxSizer;
		wxBoxSizer* BoxSizer2;
		wxStaticText* lblMinorMaximum;
		wxTextCtrl* txtNameSpace;
		wxStaticText* StaticText1;
		wxTimer tmrValidateInput;
		wxStaticText* lblRevisionRandom;
		wxBoxSizer* mainSizer;
		wxCheckBox* chkCommit;
		wxButton* btnHeaderPath;
		wxBoxSizer* changesSizer;
		wxBoxSizer* BoxSizer9;
		wxPanel* pnlStatus;
		wxBoxSizer* settingsSizer;
		wxTextCtrl* txtPrefix;
		wxBoxSizer* buildNumberMaxSizer;
		wxStaticText* lblStatus;
		wxStaticText* lblPrefix;
		wxTextCtrl* txtRevisionRandom;
		wxTextCtrl* txtBuildNumber;
		wxPanel* pnlChanges;
		wxStaticLine* StaticLine1;
		wxBoxSizer* valuesSizer;
		wxStaticText* lblChangesPath;
		wxCheckBox* chkUpdateManifest;
		wxStaticText* lblHeaderPath;
		wxBoxSizer* BoxSizer19;
		wxBoxSizer* BoxSizer4;
		wxStaticLine* StaticLine3;
		wxStaticText* lblNamespace;
		wxTextCtrl* txtMajorVersion;
		wxStaticText* lblBuildNumberMaximun;
		wxBoxSizer* BoxSizer8;
		wxButton* btnSvnDir;
		wxPanel* pnlScheme;
		wxTextCtrl* txtBuildCount;
		wxTextCtrl* txtBuildNumberMaximun;
		wxTextCtrl* txtMinorVersion;
		wxCheckBox* chkDefine;
		wxStaticText* lblHeaderGuard;
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblBuild;
		wxComboBox* cmbAbbreviation;
		wxStaticText* lblMinor;
		wxTextCtrl* txtRevisionMax;
		wxButton* btnCancel;
		wxTextCtrl* txtHeaderPath;
		wxStaticLine* StaticLine4;
		wxBoxSizer* BoxSizer12;
		wxTextCtrl* txtHeaderGuard;
		wxTextCtrl* txtBuildTimes;
		wxTextCtrl* txtMinorMaximun;
		wxStaticText* lblCurrentProject;
		wxBoxSizer* BoxSizer6;
		wxBoxSizer* svnSizer;
		wxTextCtrl* txtRevisionNumber;
		wxBoxSizer* schemeSizer;
		wxStaticText* lblMajor;
		wxTextCtrl* txtSvnDir;
		wxBoxSizer* BoxSizer5;
		wxStaticText* lblRevision;
		wxCheckBox* chkDates;
		wxStaticText* lblChangesTitle;
		//*)

public:
	void SetCurrentProject(const wxString& projectName);

	void SetMajor(long value);
	void SetMinor(long value);
	void SetBuild(long value);
	void SetRevision(long value);
	void SetCount(long value);

	void SetStatus(const wxString& value);
	void SetStatusAbbreviation(const wxString& value);

    long GetMajor() const {return m_major;}
    long GetMinor() const {return m_minor;}
    long GetBuild() const {return m_build;}
    long GetRevision() const {return m_revision;}
    long GetCount() const {return m_count;}

	wxString GetStatus() const {return m_status;}
	wxString GetStatusAbbreviation() const {return m_statusAbbreviation;}


	void SetSvn(bool value);
	void SetSvnDirectory(const wxString& value);
	void SetAuto(bool value);
	void SetDates(bool value);
	void SetDefine(bool value);
	// GJH 03/03/10 Added manifest updating.
	void SetManifest(bool value);
	void SetCommit(bool value);
	void SetCommitAsk(bool value);
	void SetLanguage(const wxString& value);
	void SetHeaderPath(const wxString& value);

	void SetMinorMaximum(long value);
	void SetBuildMaximum(long value);
	void SetRevisionMaximum(long value);
	void SetRevisionRandomMaximum(long value);
	void SetBuildTimesToMinorIncrement(long value);

	void SetChanges(bool value);
	void SetChangesLogPath(const wxString& value);
	void SetChangesTitle(const wxString& value);

	bool GetSvn() const {return m_svn;}
	wxString GetSvnDirectory() const {return m_svnDirectory;}
	bool GetAuto() const {return m_autoMajorMinor;}
	bool GetDates() const {return m_dates;}
	bool GetDefine() const {return m_useDefine;}
	// GJH 03/03/10 Added manifest updating.
	bool GetManifest() const {return m_updateManifest;}
	bool GetCommit() const {return m_commit;}
	bool GetCommitAsk() const {return m_askCommit;}
	wxString GetLanguage() const {return m_language;}
	wxString GetHeaderPath() const {return m_headerPath;}

	long GetMinorMaximum() const {return m_minorMaximun;}
	long GetBuildMaximum() const {return m_buildMaximun;}
	long GetRevisionMaximum() const {return m_revisionMaximun;}
	long GetRevisionRandomMaximum() const {return m_revisionRandomMaximun;}
	long GetBuildTimesToMinorIncrement() const {return m_buildTimesToMinorIncrement;}

	bool GetChanges() const {return m_changes;}
	wxString GetChangesLogPath() const {return m_changesLogPath;}
	wxString GetChangesTitle() const {return m_changesTitle;}

	void SetHeaderGuard(const wxString& value);
	void SetNamespace(const wxString& value);
	void SetPrefix(const wxString& value);

	wxString GetHeaderGuard() const {return m_headerGuard;}
	wxString GetNamespace() const {return m_namespace;}
	wxString GetPrefix() const {return m_prefix;}


};

#endif
