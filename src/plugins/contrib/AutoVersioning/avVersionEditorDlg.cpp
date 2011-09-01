#include "avVersionEditorDlg.h"

//(*InternalHeaders(avVersionEditorDlg)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <globals.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

wxTextCtrl* l_FocusedControl = 0;

//{ID's

//(*IdInit(avVersionEditorDlg)
const long avVersionEditorDlg::ID_MAJOR_LABEL = wxNewId();
const long avVersionEditorDlg::ID_MAJOR_TEXT = wxNewId();
const long avVersionEditorDlg::ID_MINOR_LABEL = wxNewId();
const long avVersionEditorDlg::ID_MINOR_TEXT = wxNewId();
const long avVersionEditorDlg::ID_BUILD_LABEL = wxNewId();
const long avVersionEditorDlg::ID_BUILD_TEXT = wxNewId();
const long avVersionEditorDlg::ID_REVISION_LABEL = wxNewId();
const long avVersionEditorDlg::ID_REVISION_TEXT = wxNewId();
const long avVersionEditorDlg::ID_STATICLINE2 = wxNewId();
const long avVersionEditorDlg::ID_COUNT_LABEL = wxNewId();
const long avVersionEditorDlg::ID_COUNT_TEXT = wxNewId();
const long avVersionEditorDlg::ID_VALUES_PANEL = wxNewId();
const long avVersionEditorDlg::ID_SATUS_LABEL = wxNewId();
const long avVersionEditorDlg::ID_STATUS_COMBOBOX = wxNewId();
const long avVersionEditorDlg::ID_STATICLINE4 = wxNewId();
const long avVersionEditorDlg::ID_STATICTEXT1 = wxNewId();
const long avVersionEditorDlg::ID_ABBREVIATION_COMBOBOX = wxNewId();
const long avVersionEditorDlg::ID_STATUS_PANEL = wxNewId();
const long avVersionEditorDlg::ID_MINORMAXIMUN_LABEL = wxNewId();
const long avVersionEditorDlg::ID_MINORMAXIMUM_TEXT = wxNewId();
const long avVersionEditorDlg::ID_BUILDNUMBERMAX_LABEL = wxNewId();
const long avVersionEditorDlg::ID_BUILDNUMBERMAX_TEXT = wxNewId();
const long avVersionEditorDlg::ID_REVISIONMAX_LABEL = wxNewId();
const long avVersionEditorDlg::ID_REVISIONMAX_TEXT = wxNewId();
const long avVersionEditorDlg::ID_REVISIONRANDOM_LABEL = wxNewId();
const long avVersionEditorDlg::ID_REVISIONRANDOM_TEXT = wxNewId();
const long avVersionEditorDlg::ID_BUILDTIMES_LABEL = wxNewId();
const long avVersionEditorDlg::ID_BUILDTIMES_TEXT = wxNewId();
const long avVersionEditorDlg::ID_SCHEME_PANEL = wxNewId();
const long avVersionEditorDlg::ID_HEADER_GUARD_LABEL = wxNewId();
const long avVersionEditorDlg::ID_HEADER_GUARD_TEXT = wxNewId();
const long avVersionEditorDlg::ID_NAMESPACE_LABEL = wxNewId();
const long avVersionEditorDlg::ID_NAMESPACE_TEXT = wxNewId();
const long avVersionEditorDlg::ID_PREFIX_LABEL = wxNewId();
const long avVersionEditorDlg::ID_PREFIX_TEXT = wxNewId();
const long avVersionEditorDlg::ID_CODE_PANEL = wxNewId();
const long avVersionEditorDlg::ID_AUTO_CHECK = wxNewId();
const long avVersionEditorDlg::ID_DATES_CHECK = wxNewId();
const long avVersionEditorDlg::ID_UPDATE_MANIFEST = wxNewId();
const long avVersionEditorDlg::ID_COMMIT_CHECK = wxNewId();
const long avVersionEditorDlg::ID_ASKCOMMIT_CHECK = wxNewId();
const long avVersionEditorDlg::ID_STATICLINE3 = wxNewId();
const long avVersionEditorDlg::ID_HEADERPATH_LABEL = wxNewId();
const long avVersionEditorDlg::ID_HEADERPATH_TEXTCTRL = wxNewId();
const long avVersionEditorDlg::ID_HEADERPATH_BUTTON = wxNewId();
const long avVersionEditorDlg::ID_HEADERLANGUAGE_RADIOBOX = wxNewId();
const long avVersionEditorDlg::ID_STATICLINE1 = wxNewId();
const long avVersionEditorDlg::ID_SVN_CHECK = wxNewId();
const long avVersionEditorDlg::ID_SVNDIR_TEXT = wxNewId();
const long avVersionEditorDlg::ID_SVNDIR_BUTTON = wxNewId();
const long avVersionEditorDlg::ID_SETTINGS_PANEL = wxNewId();
const long avVersionEditorDlg::ID_GENERATECHANGES_CHECKBOX = wxNewId();
const long avVersionEditorDlg::ID_CHANGESPATH_STATICTEXT = wxNewId();
const long avVersionEditorDlg::ID_CHANGESLOGPATH_TEXTCTRL = wxNewId();
const long avVersionEditorDlg::ID_CHANGESLOGPATH_BUTTON = wxNewId();
const long avVersionEditorDlg::ID_FORMAT_STATICTEXT = wxNewId();
const long avVersionEditorDlg::ID_CHANGESTITLE_TEXTCTRL = wxNewId();
const long avVersionEditorDlg::ID_FORMATS_STATICTEXT = wxNewId();
const long avVersionEditorDlg::ID_CHANGES_PANEL = wxNewId();
const long avVersionEditorDlg::ID_AV_NOTEBOOK = wxNewId();
const long avVersionEditorDlg::ID_STATICTEXT2 = wxNewId();
const long avVersionEditorDlg::ID_ACCEPT = wxNewId();
const long avVersionEditorDlg::ID_CANCEL = wxNewId();
const long avVersionEditorDlg::ID_VALIDATE_TIMER = wxNewId();
//*)

//}

//{Events
BEGIN_EVENT_TABLE(avVersionEditorDlg,wxScrollingDialog)
    //(*EventTable(avVersionEditorDlg)
    //*)
END_EVENT_TABLE()
//}

//{Constructor and Destructor
avVersionEditorDlg::avVersionEditorDlg(wxWindow* parent,wxWindowID /*id*/)
{
    //(*Initialize(avVersionEditorDlg)
    wxBoxSizer* BoxSizer15;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer13;
    wxBoxSizer* BoxSizer11;
    wxBoxSizer* BoxSizer14;
    wxBoxSizer* codeSizer;
    
    Create(parent, wxID_ANY, _("Auto Versioning Editor"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(469,364));
    wxFont thisFont(10,wxDEFAULT,wxFONTSTYLE_NORMAL,wxNORMAL,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    SetFont(thisFont);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    nbAutoVersioning = new wxNotebook(this, ID_AV_NOTEBOOK, wxDefaultPosition, wxSize(456,283), 0, _T("ID_AV_NOTEBOOK"));
    nbAutoVersioning->SetMaxSize(wxSize(-1,-1));
    nbAutoVersioning->SetFocus();
    pnlVersionValues = new wxPanel(nbAutoVersioning, ID_VALUES_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_VALUES_PANEL"));
    valuesSizer = new wxBoxSizer(wxVERTICAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    lblMajor = new wxStaticText(pnlVersionValues, ID_MAJOR_LABEL, _("Major Version"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_MAJOR_LABEL"));
    BoxSizer3->Add(lblMajor, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtMajorVersion = new wxTextCtrl(pnlVersionValues, ID_MAJOR_TEXT, _("1"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_MAJOR_TEXT"));
    txtMajorVersion->SetToolTip(_("Increments by 1 when the \nminor version reaches its\nmaximun value."));
    BoxSizer3->Add(txtMajorVersion, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    valuesSizer->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    lblMinor = new wxStaticText(pnlVersionValues, ID_MINOR_LABEL, _("Minor Version"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_MINOR_LABEL"));
    BoxSizer6->Add(lblMinor, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtMinorVersion = new wxTextCtrl(pnlVersionValues, ID_MINOR_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_MINOR_TEXT"));
    txtMinorVersion->SetToolTip(_("Increments by one everytime \nthat the build number pass the\nbarrier of build times. The value \nis reset to 0 when it reaches its\nmaximun value."));
    BoxSizer6->Add(txtMinorVersion, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    valuesSizer->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    lblBuild = new wxStaticText(pnlVersionValues, ID_BUILD_LABEL, _("Build Number"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_BUILD_LABEL"));
    BoxSizer9->Add(lblBuild, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtBuildNumber = new wxTextCtrl(pnlVersionValues, ID_BUILD_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUILD_TEXT"));
    txtBuildNumber->SetToolTip(_("Increments by 1 everytime \nthat the revision number is\nincremented.\n\nEquivalent to \"Release\" on\nunix systems."));
    BoxSizer9->Add(txtBuildNumber, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    valuesSizer->Add(BoxSizer9, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    lblRevision = new wxStaticText(pnlVersionValues, ID_REVISION_LABEL, _("Revision"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_REVISION_LABEL"));
    BoxSizer12->Add(lblRevision, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtRevisionNumber = new wxTextCtrl(pnlVersionValues, ID_REVISION_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_REVISION_TEXT"));
    txtRevisionNumber->SetToolTip(_("Increments randomly when\nsome file is modified."));
    BoxSizer12->Add(txtRevisionNumber, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    valuesSizer->Add(BoxSizer12, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    StaticLine2 = new wxStaticLine(pnlVersionValues, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
    valuesSizer->Add(StaticLine2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    lblCount = new wxStaticText(pnlVersionValues, ID_COUNT_LABEL, _("Build Count"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_COUNT_LABEL"));
    BoxSizer19->Add(lblCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtBuildCount = new wxTextCtrl(pnlVersionValues, ID_COUNT_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_COUNT_TEXT"));
    txtBuildCount->SetToolTip(_("Increments every time the\nprogram is compiled. Also if \nno changes has been made."));
    BoxSizer19->Add(txtBuildCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    valuesSizer->Add(BoxSizer19, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    pnlVersionValues->SetSizer(valuesSizer);
    valuesSizer->Fit(pnlVersionValues);
    valuesSizer->SetSizeHints(pnlVersionValues);
    pnlStatus = new wxPanel(nbAutoVersioning, ID_STATUS_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_STATUS_PANEL"));
    statusSizer = new wxBoxSizer(wxVERTICAL);
    statusSizer->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    lblStatus = new wxStaticText(pnlStatus, ID_SATUS_LABEL, _("Software Status:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_SATUS_LABEL"));
    statusSizer->Add(lblStatus, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    cmbStatus = new wxComboBox(pnlStatus, ID_STATUS_COMBOBOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_STATUS_COMBOBOX"));
    cmbStatus->Append(_("Alpha"));
    cmbStatus->Append(_("Beta"));
    cmbStatus->Append(_("Release"));
    cmbStatus->Append(_("Release Candidate"));
    cmbStatus->Append(_("Custom"));
    cmbStatus->SetToolTip(_("Example: 1.0 Alpha"));
    statusSizer->Add(cmbStatus, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine4 = new wxStaticLine(pnlStatus, ID_STATICLINE4, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE4"));
    statusSizer->Add(StaticLine4, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText1 = new wxStaticText(pnlStatus, ID_STATICTEXT1, _("Abbreviation:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    statusSizer->Add(StaticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    cmbAbbreviation = new wxComboBox(pnlStatus, ID_ABBREVIATION_COMBOBOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_ABBREVIATION_COMBOBOX"));
    cmbAbbreviation->Append(_("a"));
    cmbAbbreviation->Append(_("b"));
    cmbAbbreviation->Append(_("r"));
    cmbAbbreviation->Append(_("rc"));
    cmbAbbreviation->SetToolTip(_("Example 1.0a"));
    statusSizer->Add(cmbAbbreviation, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    statusSizer->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    pnlStatus->SetSizer(statusSizer);
    statusSizer->Fit(pnlStatus);
    statusSizer->SetSizeHints(pnlStatus);
    pnlScheme = new wxPanel(nbAutoVersioning, ID_SCHEME_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_SCHEME_PANEL"));
    schemeSizer = new wxBoxSizer(wxVERTICAL);
    minorMaxSizer = new wxBoxSizer(wxHORIZONTAL);
    lblMinorMaximum = new wxStaticText(pnlScheme, ID_MINORMAXIMUN_LABEL, _("Minor maximum:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_MINORMAXIMUN_LABEL"));
    minorMaxSizer->Add(lblMinorMaximum, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtMinorMaximun = new wxTextCtrl(pnlScheme, ID_MINORMAXIMUM_TEXT, _("10"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_MINORMAXIMUM_TEXT"));
    txtMinorMaximun->SetToolTip(_("Resets the minor to\nzero when the specified\nvalue is reached."));
    minorMaxSizer->Add(txtMinorMaximun, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    schemeSizer->Add(minorMaxSizer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buildNumberMaxSizer = new wxBoxSizer(wxHORIZONTAL);
    lblBuildNumberMaximun = new wxStaticText(pnlScheme, ID_BUILDNUMBERMAX_LABEL, _("Build Number maximum:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_BUILDNUMBERMAX_LABEL"));
    buildNumberMaxSizer->Add(lblBuildNumberMaximun, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtBuildNumberMaximun = new wxTextCtrl(pnlScheme, ID_BUILDNUMBERMAX_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUILDNUMBERMAX_TEXT"));
    txtBuildNumberMaximun->SetToolTip(_("Reset the build number\nto zero when the specified\nvalue is reached.\n\n0 equals to unlimited."));
    buildNumberMaxSizer->Add(txtBuildNumberMaximun, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    schemeSizer->Add(buildNumberMaxSizer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    lblRevisionMax = new wxStaticText(pnlScheme, ID_REVISIONMAX_LABEL, _("Revision maximum:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_REVISIONMAX_LABEL"));
    BoxSizer1->Add(lblRevisionMax, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtRevisionMax = new wxTextCtrl(pnlScheme, ID_REVISIONMAX_TEXT, _("0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_REVISIONMAX_TEXT"));
    txtRevisionMax->SetToolTip(_("Reset the revision to zero\nwhen the specified value \nis reached.\n\n0 equals to unlimited."));
    BoxSizer1->Add(txtRevisionMax, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    schemeSizer->Add(BoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    lblRevisionRandom = new wxStaticText(pnlScheme, ID_REVISIONRANDOM_LABEL, _("Revision random maximum:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_REVISIONRANDOM_LABEL"));
    BoxSizer2->Add(lblRevisionRandom, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtRevisionRandom = new wxTextCtrl(pnlScheme, ID_REVISIONRANDOM_TEXT, _("10"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_REVISIONRANDOM_TEXT"));
    txtRevisionRandom->SetToolTip(_("Maximun number value\nused for the random\nincrementation of the\nrevision."));
    BoxSizer2->Add(txtRevisionRandom, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    schemeSizer->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    lblBuildTimes = new wxStaticText(pnlScheme, ID_BUILDTIMES_LABEL, _("Build times before incrementing Minor:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_BUILDTIMES_LABEL"));
    schemeSizer->Add(lblBuildTimes, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtBuildTimes = new wxTextCtrl(pnlScheme, ID_BUILDTIMES_TEXT, _("100"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUILDTIMES_TEXT"));
    txtBuildTimes->SetToolTip(_("Indicates the times\nthat the project have\nto be build before\nincrementing the minor."));
    schemeSizer->Add(txtBuildTimes, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    pnlScheme->SetSizer(schemeSizer);
    schemeSizer->Fit(pnlScheme);
    schemeSizer->SetSizeHints(pnlScheme);
    pnlCode = new wxPanel(nbAutoVersioning, ID_CODE_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_CODE_PANEL"));
    codeSizer = new wxBoxSizer(wxVERTICAL);
    lblHeaderGuard = new wxStaticText(pnlCode, ID_HEADER_GUARD_LABEL, _("Header guard:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_HEADER_GUARD_LABEL"));
    codeSizer->Add(lblHeaderGuard, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    txtHeaderGuard = new wxTextCtrl(pnlCode, ID_HEADER_GUARD_TEXT, _("VERSION_H"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_HEADER_GUARD_TEXT"));
    codeSizer->Add(txtHeaderGuard, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    lblNamespace = new wxStaticText(pnlCode, ID_NAMESPACE_LABEL, _("Namespace:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_NAMESPACE_LABEL"));
    codeSizer->Add(lblNamespace, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    txtNameSpace = new wxTextCtrl(pnlCode, ID_NAMESPACE_TEXT, _("AutoVersion"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_NAMESPACE_TEXT"));
    codeSizer->Add(txtNameSpace, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    lblPrefix = new wxStaticText(pnlCode, ID_PREFIX_LABEL, _("Variables prefix:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_PREFIX_LABEL"));
    codeSizer->Add(lblPrefix, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    txtPrefix = new wxTextCtrl(pnlCode, ID_PREFIX_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PREFIX_TEXT"));
    codeSizer->Add(txtPrefix, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    pnlCode->SetSizer(codeSizer);
    codeSizer->Fit(pnlCode);
    codeSizer->SetSizeHints(pnlCode);
    pnlSettings = new wxPanel(nbAutoVersioning, ID_SETTINGS_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_SETTINGS_PANEL"));
    settingsSizer = new wxBoxSizer(wxVERTICAL);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer13 = new wxBoxSizer(wxVERTICAL);
    chkAutoIncrement = new wxCheckBox(pnlSettings, ID_AUTO_CHECK, _("Autoincrement Major and Minor"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_AUTO_CHECK"));
    chkAutoIncrement->SetValue(true);
    chkAutoIncrement->SetToolTip(_("Use the default scheme for \nautoincrementing the major \nand minor versions."));
    BoxSizer13->Add(chkAutoIncrement, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    chkDates = new wxCheckBox(pnlSettings, ID_DATES_CHECK, _("Create date declarations"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DATES_CHECK"));
    chkDates->SetValue(true);
    chkDates->SetToolTip(_("Create variable declarations\nfor date, month and year. And \nalso an ubuntu version style."));
    BoxSizer13->Add(chkDates, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    chkUpdateManifest = new wxCheckBox(pnlSettings, ID_UPDATE_MANIFEST, _("Update manifest.xml"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_UPDATE_MANIFEST"));
    chkUpdateManifest->SetValue(false);
    chkUpdateManifest->SetToolTip(_("Update manifest.xml\'s version field with AutoVersion\'s\nvalues using the format MAJOR.MINOR.BUILD.\nThis setting is mainly useful for plugin developers."));
    BoxSizer13->Add(chkUpdateManifest, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer11->Add(BoxSizer13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer14 = new wxBoxSizer(wxVERTICAL);
    chkCommit = new wxCheckBox(pnlSettings, ID_COMMIT_CHECK, _("Do Autoincrement"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_COMMIT_CHECK"));
    chkCommit->SetValue(false);
    chkCommit->SetToolTip(_("If you check this the version\ninfo will increment when files have\nbeen modified every time before\ncompilation takes effect."));
    BoxSizer14->Add(chkCommit, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    chkAskCommit = new wxCheckBox(pnlSettings, ID_ASKCOMMIT_CHECK, _("Ask to Increment"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ASKCOMMIT_CHECK"));
    chkAskCommit->SetValue(false);
    chkAskCommit->Disable();
    chkAskCommit->SetToolTip(_("Ask you to increment every time\na change has been made to the \nsource code, before the compilation\ntakes effect."));
    BoxSizer14->Add(chkAskCommit, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer14->Add(-1,-1,1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer11->Add(BoxSizer14, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    settingsSizer->Add(BoxSizer11, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine3 = new wxStaticLine(pnlSettings, ID_STATICLINE3, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE3"));
    settingsSizer->Add(StaticLine3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer7 = new wxBoxSizer(wxVERTICAL);
    lblHeaderPath = new wxStaticText(pnlSettings, ID_HEADERPATH_LABEL, _("Header Path:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_HEADERPATH_LABEL"));
    BoxSizer7->Add(lblHeaderPath, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    txtHeaderPath = new wxTextCtrl(pnlSettings, ID_HEADERPATH_TEXTCTRL, _("version.h"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_HEADERPATH_TEXTCTRL"));
    txtHeaderPath->SetToolTip(_("Header path"));
    BoxSizer15->Add(txtHeaderPath, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnHeaderPath = new wxButton(pnlSettings, ID_HEADERPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(23,26), 0, wxDefaultValidator, _T("ID_HEADERPATH_BUTTON"));
    btnHeaderPath->SetToolTip(_("Select header path and filename"));
    BoxSizer15->Add(btnHeaderPath, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer7->Add(BoxSizer15, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    wxString __wxRadioBoxChoices_1[2] = 
    {
    _("C"),
    _("C++")
    };
    rbHeaderLanguage = new wxRadioBox(pnlSettings, ID_HEADERLANGUAGE_RADIOBOX, _("Header language"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_HEADERLANGUAGE_RADIOBOX"));
    rbHeaderLanguage->SetSelection(1);
    rbHeaderLanguage->SetToolTip(_("Sets the language output."));
    BoxSizer8->Add(rbHeaderLanguage, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    settingsSizer->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(pnlSettings, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    settingsSizer->Add(StaticLine1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    chkSvn = new wxCheckBox(pnlSettings, ID_SVN_CHECK, _("SVN enabled"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SVN_CHECK"));
    chkSvn->SetValue(false);
    chkSvn->SetToolTip(_("Check this if your project is controlled by svn\nto generate an SVN_REVISION declaration."));
    settingsSizer->Add(chkSvn, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    svnSizer = new wxBoxSizer(wxHORIZONTAL);
    txtSvnDir = new wxTextCtrl(pnlSettings, ID_SVNDIR_TEXT, wxEmptyString, wxDefaultPosition, wxSize(236,26), 0, wxDefaultValidator, _T("ID_SVNDIR_TEXT"));
    txtSvnDir->Disable();
    txtSvnDir->SetToolTip(_("Current SVN directory."));
    svnSizer->Add(txtSvnDir, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    btnSvnDir = new wxButton(pnlSettings, ID_SVNDIR_BUTTON, _("..."), wxDefaultPosition, wxSize(23,26), 0, wxDefaultValidator, _T("ID_SVNDIR_BUTTON"));
    btnSvnDir->Disable();
    btnSvnDir->SetToolTip(_("Choose current svn directory project\nif diffrent from the project default."));
    svnSizer->Add(btnSvnDir, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    settingsSizer->Add(svnSizer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    pnlSettings->SetSizer(settingsSizer);
    settingsSizer->Fit(pnlSettings);
    settingsSizer->SetSizeHints(pnlSettings);
    pnlChanges = new wxPanel(nbAutoVersioning, ID_CHANGES_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_CHANGES_PANEL"));
    changesSizer = new wxBoxSizer(wxVERTICAL);
    chkChanges = new wxCheckBox(pnlChanges, ID_GENERATECHANGES_CHECKBOX, _("Show changes editor when incrementing version"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GENERATECHANGES_CHECKBOX"));
    chkChanges->SetValue(false);
    chkChanges->SetToolTip(_("This will open a window\neverytime you commit or\nthe project version increments.\n\nThen you can enter the\nchanges made to the project."));
    changesSizer->Add(chkChanges, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    lblChangesPath = new wxStaticText(pnlChanges, ID_CHANGESPATH_STATICTEXT, _("File path;"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_CHANGESPATH_STATICTEXT"));
    changesSizer->Add(lblChangesPath, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    txtChangesLogPath = new wxTextCtrl(pnlChanges, ID_CHANGESLOGPATH_TEXTCTRL, _("ChangesLog.txt"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHANGESLOGPATH_TEXTCTRL"));
    txtChangesLogPath->SetToolTip(_("ChangesLog Path"));
    BoxSizer10->Add(txtChangesLogPath, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnChangesLogPath = new wxButton(pnlChanges, ID_CHANGESLOGPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(23,26), 0, wxDefaultValidator, _T("ID_CHANGESLOGPATH_BUTTON"));
    btnChangesLogPath->SetToolTip(_("Select ChangesLog path and filename"));
    BoxSizer10->Add(btnChangesLogPath, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    changesSizer->Add(BoxSizer10, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    lblChangesTitle = new wxStaticText(pnlChanges, ID_FORMAT_STATICTEXT, _("Title Format:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_FORMAT_STATICTEXT"));
    BoxSizer4->Add(lblChangesTitle, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtChangesTitle = new wxTextCtrl(pnlChanges, ID_CHANGESTITLE_TEXTCTRL, _("released version %M.%m.%b of %p"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHANGESTITLE_TEXTCTRL"));
    BoxSizer4->Add(txtChangesTitle, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    changesSizer->Add(BoxSizer4, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    lblChangesFormats = new wxStaticText(pnlChanges, ID_FORMATS_STATICTEXT, _("Major: %M, Minor: %m, Build: %b, Revision: %r, SVN Revision: %s, Date: %d, Month: %o, Year: %y, Ubuntu Style Version: %u, Status: %T, Status Short: %t, Project title: %p"), wxDefaultPosition, wxSize(413,80), 0, _T("ID_FORMATS_STATICTEXT"));
    changesSizer->Add(lblChangesFormats, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    pnlChanges->SetSizer(changesSizer);
    changesSizer->Fit(pnlChanges);
    changesSizer->SetSizeHints(pnlChanges);
    nbAutoVersioning->AddPage(pnlVersionValues, _("Version Values"), true);
    nbAutoVersioning->AddPage(pnlStatus, _("Status"), false);
    nbAutoVersioning->AddPage(pnlScheme, _("Scheme"), false);
    nbAutoVersioning->AddPage(pnlCode, _("Code"), false);
    nbAutoVersioning->AddPage(pnlSettings, _("Settings"), false);
    nbAutoVersioning->AddPage(pnlChanges, _("Changes Log"), false);
    mainSizer->Add(nbAutoVersioning, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    lblCurrentProject = new wxStaticText(this, ID_STATICTEXT2, _("Current Project:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
    mainSizer->Add(lblCurrentProject, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    btnAccept = new wxButton(this, ID_ACCEPT, _("&Accept"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ACCEPT"));
    btnAccept->SetDefault();
    btnAccept->SetToolTip(_("Accept changes made."));
    buttonsSizer->Add(btnAccept, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    buttonsSizer->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    btnCancel = new wxButton(this, ID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CANCEL"));
    btnCancel->SetToolTip(_("Cancel changes made\nto the values in the editor."));
    buttonsSizer->Add(btnCancel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    mainSizer->Add(buttonsSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    SetSizer(mainSizer);
    tmrValidateInput.SetOwner(this, ID_VALIDATE_TIMER);
    tmrValidateInput.Start(500, false);
    mainSizer->SetSizeHints(this);
    Center();
    
    Connect(ID_STATUS_COMBOBOX,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&avVersionEditorDlg::OnCmbStatusSelect);
    Connect(ID_COMMIT_CHECK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnChkCommitClick);
    Connect(ID_HEADERPATH_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnHeaderPathClick);
    Connect(ID_SVN_CHECK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnSvnCheck);
    Connect(ID_SVNDIR_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnSvnDirectoryClick);
    Connect(ID_CHANGESLOGPATH_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnChangesLogPathClick);
    Connect(ID_ACCEPT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnAcceptClick);
    Connect(ID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avVersionEditorDlg::OnCancelClick);
    Connect(ID_VALIDATE_TIMER,wxEVT_TIMER,(wxObjectEventFunction)&avVersionEditorDlg::OnTmrValidateInputTrigger);
    Connect(wxEVT_ENTER_WINDOW,(wxObjectEventFunction)&avVersionEditorDlg::OnMouseEnter);
    //*)

    tmrValidateInput.Stop();
}

avVersionEditorDlg::~avVersionEditorDlg()
{
    //(*Destroy(avVersionEditorDlg)
    //*)
}
//}


//{Version Values
void avVersionEditorDlg::SetMajor(long value)
{
    m_major = value;
    wxString strValue;
    strValue.Printf(_T("%ld"),value);
    txtMajorVersion->SetValue(strValue);
}

void avVersionEditorDlg::SetMinor(long value)
{
    m_minor = value;
    wxString strValue;
    strValue.Printf(_T("%ld"),value);
    txtMinorVersion->SetValue(strValue);
}

void avVersionEditorDlg::SetBuild(long value)
{
    m_build = value;
    wxString strValue;
    strValue.Printf(_T("%ld"),value);
    txtBuildNumber->SetValue(strValue);
}

void avVersionEditorDlg::SetRevision(long value)
{
    m_revision = value;
    wxString strValue;
    strValue.Printf(_T("%ld"),value);
    txtRevisionNumber->SetValue(strValue);
}

void avVersionEditorDlg::SetCount(long value)
{
    m_count = value;
    wxString strValue;
    strValue.Printf(_T("%ld"),value);
    txtBuildCount->SetValue(strValue);
}
//}

//{Settings
void avVersionEditorDlg::SetAuto(bool value)
{
	m_autoMajorMinor = value;
	chkAutoIncrement->SetValue(value);
}

void avVersionEditorDlg::SetDates(bool value)
{
	m_dates = value;
	chkDates->SetValue(value);
}

// GJH 03/03/10 Added manifest updating.
void avVersionEditorDlg::SetManifest(bool value)
{
	m_updateManifest = value;
	chkUpdateManifest->SetValue(value);
}

void avVersionEditorDlg::SetSvn(bool value)
{
	m_svn = value;
	chkSvn->SetValue(value);
    if(!value)
    {
        txtSvnDir->Disable();
        btnSvnDir->Disable();
    }
    else
    {
        txtSvnDir->Enable();
        btnSvnDir->Enable();
    }
}

void avVersionEditorDlg::SetSvnDirectory(const wxString& value)
{
    if (!value.IsEmpty())
    {
        m_svnDirectory = value;
        txtSvnDir->SetValue(m_svnDirectory);
    }
}

void avVersionEditorDlg::SetCommit(bool value)
{
	m_commit = value;
	chkCommit->SetValue(value);
    if(!value)
    {
        chkAskCommit->Disable();
    }
    else
    {
        chkAskCommit->Enable();
    }
}

void avVersionEditorDlg::SetCommitAsk(bool value)
{
	m_askCommit = value;
	chkAskCommit->SetValue(value);
}

void avVersionEditorDlg::SetLanguage(const wxString& value)
{
    if (!value.IsEmpty())
    {
        m_language = value;
        rbHeaderLanguage->SetStringSelection(value);
    }
}

void avVersionEditorDlg::SetHeaderPath(const wxString& value)
{
    m_headerPath = value;
    txtHeaderPath->SetValue(value);
}
//}

//{Software Status
void avVersionEditorDlg::SetStatus(const wxString& value)
{
    if (!value.IsEmpty())
    {
        m_status = value;
        cmbStatus->SetValue(value);
    }
}

void avVersionEditorDlg::SetStatusAbbreviation(const wxString& value)
{
    if (!value.IsEmpty())
    {
        m_statusAbbreviation = value;
        cmbAbbreviation->SetValue(value);
    }
}
//}

//{Scheme
void avVersionEditorDlg::SetMinorMaximum(long value)
{
	m_minorMaximun = value;
	wxString strValue;
	strValue.Printf(_T("%ld"),value);
	txtMinorMaximun->SetValue(strValue);
}

void avVersionEditorDlg::SetBuildMaximum(long value)
{
	m_buildMaximun = value;
	wxString strValue;
	strValue.Printf(_T("%ld"),value);
	txtBuildNumberMaximun->SetValue(strValue);
}

void avVersionEditorDlg::SetRevisionMaximum(long value)
{
	m_revisionMaximun = value;
	wxString strValue;
	strValue.Printf(_T("%ld"),value);
	txtRevisionMax->SetValue(strValue);
}

void avVersionEditorDlg::SetRevisionRandomMaximum(long value)
{
	m_revisionRandomMaximun = value;
	wxString strValue;
	strValue.Printf(_T("%ld"),value);
	txtRevisionRandom->SetValue(strValue);
}

void avVersionEditorDlg::SetBuildTimesToMinorIncrement(long value)
{
	m_buildTimesToMinorIncrement = value;
	wxString strValue;
	strValue.Printf(_T("%ld"),value);
	txtBuildTimes->SetValue(strValue);
}
//}

//{Changes Log
void avVersionEditorDlg::SetChanges(bool value)
{
    m_changes = value;
	chkChanges->SetValue(value);
}

void avVersionEditorDlg::SetChangesLogPath(const wxString& value)
{
    m_changesLogPath = value;
    txtChangesLogPath->SetValue(value);
}

void avVersionEditorDlg::SetChangesTitle(const wxString& value)
{
    if (!value.IsEmpty())
    {
        m_changesTitle = value;
        txtChangesTitle->SetValue(value);
    }
}
//}

//{Code
void avVersionEditorDlg::SetHeaderGuard(const wxString& value)
{
    m_headerGuard = value;
    txtHeaderGuard->SetValue(value);
}

void avVersionEditorDlg::SetNamespace(const wxString& value)
{
    m_namespace = value;
    txtNameSpace->SetValue(value);
}

void avVersionEditorDlg::SetPrefix(const wxString& value)
{
    m_prefix = value;
    txtPrefix->SetValue(value);
}
//}

void avVersionEditorDlg::SetCurrentProject(const wxString& projectName)
{
    lblCurrentProject->SetLabel(lblCurrentProject->GetLabel() + projectName);
}

//{Events
void avVersionEditorDlg::OnAcceptClick(wxCommandEvent&)
{
    tmrValidateInput.Stop();
    ValidateInput();

    txtMajorVersion->GetValue().ToLong(&m_major);
    txtMinorVersion->GetValue().ToLong(&m_minor);
    txtBuildNumber->GetValue().ToLong(&m_build);
    txtRevisionNumber->GetValue().ToLong(&m_revision);
    txtBuildCount->GetValue().ToLong(&m_count);

    m_autoMajorMinor = chkAutoIncrement->IsChecked();
    m_dates = chkDates->IsChecked();
	// GJH 03/03/10 Added manifest updating.
	m_updateManifest = chkUpdateManifest->IsChecked();
    m_svn = chkSvn->IsChecked();
    m_svnDirectory = txtSvnDir->GetValue();
    m_commit = chkCommit->IsChecked();
    m_askCommit = chkAskCommit->IsChecked();
    m_language = rbHeaderLanguage->GetStringSelection();
    m_headerPath = txtHeaderPath->GetValue();

    m_status = cmbStatus->GetValue();
    m_statusAbbreviation = cmbAbbreviation->GetValue();

    txtMinorMaximun->GetValue().ToLong(&m_minorMaximun);
    txtBuildNumberMaximun->GetValue().ToLong(&m_buildMaximun);
    txtRevisionMax->GetValue().ToLong(&m_revisionMaximun);
    txtRevisionRandom->GetValue().ToLong(&m_revisionRandomMaximun);
    txtBuildTimes->GetValue().ToLong(&m_buildTimesToMinorIncrement);

    m_changes = chkChanges->IsChecked();
    m_changesTitle = txtChangesTitle->GetValue();
    m_changesLogPath = txtChangesLogPath->GetValue();

    m_headerGuard = txtHeaderGuard->GetValue();
    m_namespace = txtNameSpace->GetValue();
    m_prefix = txtPrefix->GetValue();

    EndModal(0);
}

void avVersionEditorDlg::OnCancelClick(wxCommandEvent&)
{
    tmrValidateInput.Stop();
    EndModal(0);
}

void avVersionEditorDlg::OnSvnCheck(wxCommandEvent&)
{
    if (chkSvn->IsChecked())
    {
        txtSvnDir->Enable();
        txtSvnDir->SetValue(m_svnDirectory);
        btnSvnDir->Enable();
    }
    else
    {
        txtSvnDir->Disable();
        btnSvnDir->Disable();
    }
}

void avVersionEditorDlg::OnSvnDirectoryClick(wxCommandEvent&)
{
    wxString dir = wxDirSelector(wxDirSelectorPromptStr, m_svnDirectory, 0, wxDefaultPosition, this);
    if (!dir.IsEmpty())
    {
        txtSvnDir->SetValue(dir);
        m_svnDirectory = txtSvnDir->GetValue();
    }
}

void avVersionEditorDlg::OnChkCommitClick(wxCommandEvent&)
{
    if (chkCommit->IsChecked())
    {
        chkAskCommit->Enable();
    }
    else
    {
        chkAskCommit->Disable();
    }
}

void avVersionEditorDlg::OnCmbStatusSelect(wxCommandEvent&)
{
    int status = cmbStatus->GetCurrentSelection();
    if (status != 4)
    {
        cmbAbbreviation->SetSelection(status);
    }
    else
    {
        cmbAbbreviation->SetValue(_T(""));
        cmbStatus->SetValue(_T(""));
    }
}

void avVersionEditorDlg::ValidateInput()
{
    wxString type = _T("");

    if(this->FindFocus())
    {
        type = cbC2U(typeid(*(this->FindFocus())).name());
    }

    if (type.Find(_T("wxTextCtrl")) != wxNOT_FOUND)
    {
        ForceValidation:; //To force validation when the user clicks the Accept buttom or other control not of type wxTexCtrl

        if (l_FocusedControl != this->FindFocus() && l_FocusedControl != 0)
        {
            wxString ifBlank;
            wxString ifZero;

            if (l_FocusedControl->GetName() == _T("ID_MINORMAXIMUM_TEXT"))
            {
                ifBlank = _T("10");
                ifZero = _T("1");
            }
            else if(l_FocusedControl->GetName() == _T("ID_BUILDNUMBERMAX_TEXT"))
            {
                ifBlank = _T("0");
                ifZero = _T("0");
            }
            else if(l_FocusedControl->GetName() == _T("ID_REVISIONMAX_TEXT"))
            {
                ifBlank = _T("0");
                ifZero = _T("0");
            }
            else if(l_FocusedControl->GetName() == _T("ID_BUILDTIMES_TEXT"))
            {
                ifBlank = _T("100");
                ifZero = _T("1");
            }
            else if(l_FocusedControl->GetName() == _T("ID_REVISIONRANDOM_TEXT"))
            {
                ifBlank = _T("10");
                ifZero = _T("1");
            }

            if(l_FocusedControl->GetValue() == _T("0"))
            {
                l_FocusedControl->SetValue(ifZero);
            }
            else if(l_FocusedControl->GetValue().Trim() == _T(""))
            {
                l_FocusedControl->SetValue(ifBlank);
            }

            l_FocusedControl = 0;
        }

        if (this->FindFocus() == txtRevisionRandom)
        {
            l_FocusedControl = txtRevisionRandom;
        }
        else if (this->FindFocus() == txtMinorMaximun)
        {
            l_FocusedControl = txtMinorMaximun;
        }
        else if (this->FindFocus() == txtBuildNumberMaximun)
        {
            l_FocusedControl = txtBuildNumberMaximun;
        }
        else if (this->FindFocus() == txtRevisionMax)
        {
            l_FocusedControl = txtRevisionMax;
        }
        else if (this->FindFocus() == txtBuildTimes)
        {
            l_FocusedControl = txtBuildTimes;
        }

    }
    else
    {
        goto ForceValidation; // KILLERBOT : can we do without ugly goto : normal C++ rule : sure we can --> investigate and solve
        // weird / the goto target is the first part of the if clause of the else part, so that means that in both situations (if/else)
        // the smae thing should happen ==> either this is an error , or we can do without the if test and as such without the goto
        //  bye bye goto :-) :-) :-)
    }
}

void avVersionEditorDlg::OnTmrValidateInputTrigger(wxTimerEvent& /*event*/)
{
    ValidateInput();
}

void avVersionEditorDlg::OnMouseEnter(wxMouseEvent& /*event*/)
{
    tmrValidateInput.Start(250, false);
}

void avVersionEditorDlg::OnHeaderPathClick(wxCommandEvent& /*event*/)
{
    wxString path, filename, extension;
    wxFileName::SplitPath(m_headerPath, &path, &filename, &extension);

    wxString fullpath;
    fullpath = wxFileSelector(_("Select the header path and filename:"), path, filename, extension, _T("C/C++ Header (*.h)|*.h|All Files (*.*)|*.*"));

    if(!fullpath.IsEmpty())
    {
        wxFileName relativeFile(fullpath);
        relativeFile.MakeRelativeTo();

        txtHeaderPath->SetValue(relativeFile.GetFullPath());
    }
}

void avVersionEditorDlg::OnChangesLogPathClick(wxCommandEvent& /*event*/)
{
    wxString path, filename, extension;
    wxFileName::SplitPath(m_changesLogPath, &path, &filename, &extension);

    wxString fullpath;
    fullpath = wxFileSelector(_("Select the changeslog path and filename:"), path, filename, extension, _T("Text File (*.txt)|*.txt"));

    if(!fullpath.IsEmpty()){
        wxFileName relativeFile(fullpath);
        relativeFile.MakeRelativeTo();

        txtChangesLogPath->SetValue(relativeFile.GetFullPath());
    }
}
//}
