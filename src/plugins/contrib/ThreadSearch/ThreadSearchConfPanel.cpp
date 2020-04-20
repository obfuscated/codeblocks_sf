/***************************************************************
 * Name:      ThreadSearchConfPanel
 * Purpose:   This class implements the configuration panel used
 *            in modal dialog called on "Options" button click
 *            and by C::B on "Environment" settings window.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/checkbox.h>
    #include <wx/notebook.h>
    #include <wx/radiobox.h>
    #include <wx/sizer.h>
    #include <wx/statbox.h>
    #include <wx/stattext.h>
    #include "configmanager.h"
#endif

#include <wx/clrpicker.h>
#include <wx/gbsizer.h>
#include "cbcolourmanager.h"
#include "ThreadSearch.h"
#include "SearchInPanel.h"
#include "DirectoryParamsPanel.h"
#include "ThreadSearchConfPanel.h"
#include "ThreadSearchControlIds.h"
#include "ThreadSearchViewManagerBase.h"
#include "ThreadSearchFindData.h"
#include "InsertIndexManager.h"

// begin wxGlade: ::extracode

// end wxGlade

/// Override the colour picker to implement a resetable control. This is possible with right click.
/// Unfortunately the colour picker doesn't send right down or context menu events to its parent, so
/// we have to derive from it.
struct ResetableColourPicker : wxColourPickerCtrl
{
    ResetableColourPicker(ThreadSearchConfPanel *panel, wxWindow *parent, ControlIDs::IDs id,
                          const wxColour &colour) :
        wxColourPickerCtrl(parent, controlIDs.Get(id), colour),
        m_panel(panel)
    {
        SetToolTip(_("Right click would reset the colour to its default value"));
    }
private:
    void OnContext(wxContextMenuEvent &event)
    {
        // Relay the event to the panel. It seems the id in the event is for some internal control,
        // so we just change it.
        wxContextMenuEvent e(event);
        e.SetId(GetId());
        m_panel->OnColourPickerContext(e);
    }

private:
    ThreadSearchConfPanel *m_panel;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ResetableColourPicker, wxColourPickerCtrl)
    EVT_CONTEXT_MENU(ResetableColourPicker::OnContext)
END_EVENT_TABLE()

ThreadSearchConfPanel::ThreadSearchConfPanel(ThreadSearch& threadSearchPlugin,
                                             cbConfigurationPanelColoursInterface *coloursInterface,
                                             wxWindow* parent) :
    m_ThreadSearchPlugin(threadSearchPlugin),
    m_ColoursInterface(coloursInterface)
{
    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // begin wxGlade: ThreadSearchConfPanel::ThreadSearchConfPanel
    m_Notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
    m_PageGeneral = new wxPanel(m_Notebook, wxID_ANY);
    m_PageLayout = new wxPanel(m_Notebook, wxID_ANY);

    SizerSearchIn_staticbox = new wxStaticBox(m_PageGeneral, -1, _("Search in files:"));
    SizerThreadSearchOptions_staticbox = new wxStaticBox(m_PageGeneral, -1, _("Thread search options"));

    m_pPnlSearchIn = new SearchInPanel(m_PageGeneral, wxID_ANY);
    m_pPnlDirParams = new DirectoryParamsPanel(&threadSearchPlugin.GetFindData(), m_PageGeneral, wxID_ANY);
    m_pChkWholeWord = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkWholeWord), _("Whole word"));
    m_pChkStartWord = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkStartWord), _("Start word"));
    m_pChkMatchCase = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkMatchCase), _("Match case"));
    m_pChkRegExp = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkRegularExpression), _("Regular expression"));
    m_pChkThreadSearchEnable = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkThreadSearchEnable),
                                                                   _("Enable 'Find occurrences' contextual menu item"));
    m_pChkUseDefaultOptionsForThreadSearch = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkUseDefaultOptionsOnThreadSearch),
                                                            _("Use default options when running 'Find occurrences' "));
    m_pChkShowMissingFilesError = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkShowMissingFilesError),
                                                 _("Show error message if file is missing"));
    m_pChkShowCantOpenFileError = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkShowCantOpenFileError),
                                                 _("Show error message if file cannot be opened"));
    m_pChkDeletePreviousResults = new wxCheckBox(m_PageGeneral, controlIDs.Get(ControlIDs::idChkChkDeletePreviousResults),
                                                 _("Delete previous results at search begin"));

    SizerThreadSearchLayoutGlobal_staticbox = new wxStaticBox(m_PageLayout, -1, _("Show/Hide"));
    SizerListControlOptions_staticbox = new wxStaticBox(m_PageLayout, -1, _("List control options"));

    m_pChkShowThreadSearchToolBar = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkViewThreadSearchToolBar),
                                                   _("Show ThreadSearch toolbar"));
    m_pChkShowThreadSearchWidgets = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkShowThreadSearchWidgets),
                                                   _("Show search widgets in ThreadSearch Messages panel"));
    m_pChkShowCodePreview = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkShowCodePreview),
                                           _("Show code preview editor"));
    m_pChkDisplayLogHeaders = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkDisplayLogHeaders),
                                             _("Display header in log window"));
    m_pChkDrawLogLines = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkDrawLogLines),
                                        _("Draw lines between log columns"));
    m_pChkAutosizeLogColumns = new wxCheckBox(m_PageLayout, controlIDs.Get(ControlIDs::idChkAutosizeLogColumns),
                                        _("Automatically resize log columns"));
    const wxString m_pRadSortBy_choices[] = {
        _("File path"),
        _("File name")
    };
    m_pRadSortBy = new wxRadioBox(m_PageGeneral, wxID_ANY, _("Sort results by"), wxDefaultPosition, wxDefaultSize, 2, m_pRadSortBy_choices, 1, wxRA_SPECIFY_ROWS);

    const wxString m_pRadPanelManagement_choices[] = {
        _("Messages notebook"),
        _("Layout")
    };
    m_pRadPanelManagement = new wxRadioBox(m_PageLayout, wxID_ANY, _("ThreadSearch panel management by"), wxDefaultPosition, wxDefaultSize, 2, m_pRadPanelManagement_choices, 1, wxRA_SPECIFY_ROWS);
    const wxString m_pRadLoggerType_choices[] = {
        _("List"),
        _("Tree"),
        _("List STC")
    };
    m_pRadLoggerType = new wxRadioBox(m_PageLayout, wxID_ANY, _("Logger type"), wxDefaultPosition, wxDefaultSize, 3, m_pRadLoggerType_choices, 1, wxRA_SPECIFY_ROWS);
    const wxString m_pRadSplitterWndMode_choices[] = {
        _("Horizontal"),
        _("Vertical")
    };
    m_pRadSplitterWndMode = new wxRadioBox(m_PageLayout, wxID_ANY, _("Splitter window mode"), wxDefaultPosition, wxDefaultSize, 2, m_pRadSplitterWndMode_choices, 1, wxRA_SPECIFY_ROWS);

    {

        STCColours_staticbox = new wxStaticBox(m_PageLayout, -1, _("STC Logger colours"));

        int labelIdx = 0;
        m_STCColoursLabels[labelIdx++] = new wxStaticText(m_PageLayout, wxID_ANY, _("Text (fg/bg)"));
        m_STCColoursLabels[labelIdx++] = new wxStaticText(m_PageLayout, wxID_ANY, _("File (fg/bg)"));
        m_STCColoursLabels[labelIdx++] = new wxStaticText(m_PageLayout, wxID_ANY, _("LineNo (fg/bg)"));
        m_STCColoursLabels[labelIdx++] = new wxStaticText(m_PageLayout, wxID_ANY, _("Match (fg/bg)"));
        m_STCColoursLabels[labelIdx++] = new wxStaticText(m_PageLayout, wxID_ANY,_("Selected line background"));

        int pickerIdx = 0;
        if (!m_ColoursInterface)
        {
            ColourManager *colours = Manager::Get()->GetColourManager();
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker0, colours->GetColour(wxT("thread_search_text_fore")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker1, colours->GetColour(wxT("thread_search_text_back")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker2, colours->GetColour(wxT("thread_search_file_fore")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker3, colours->GetColour(wxT("thread_search_file_back")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker4, colours->GetColour(wxT("thread_search_lineno_fore")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker5, colours->GetColour(wxT("thread_search_lineno_back")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker6, colours->GetColour(wxT("thread_search_match_fore")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker7, colours->GetColour(wxT("thread_search_match_back")));
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker8, colours->GetColour(wxT("thread_search_selected_line_back")));
            m_STCColourPickers[pickerIdx] = nullptr;
        }
        else
        {
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker0, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker1, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker2, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker3, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker4, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker5, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker6, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker7, *wxBLACK);
            m_STCColourPickers[pickerIdx++] = new ResetableColourPicker(this, m_PageLayout, ControlIDs::idConfPanelColorPicker8, *wxBLACK);
            m_STCColourPickers[pickerIdx] = nullptr;

            for (int jj = 0; jj < pickerIdx; ++jj)
            {
                const ControlIDs::IDs id = ControlIDs::IDs(ControlIDs::idConfPanelColorPicker0 + jj);
                Connect(controlIDs.Get(id), wxEVT_COLOURPICKER_CHANGED,
                        wxObjectEventFunction(&ThreadSearchConfPanel::OnColourPickerChanged));
            }
        }
    }

    set_properties();
    do_layout();
    // end wxGlade
}

void ThreadSearchConfPanel::SetSearchAndMaskHistory(const wxArrayString &dirHistory,
                                                    const wxArrayString &maskHistory)
{
    m_pPnlDirParams->SetSearchHistory(dirHistory, maskHistory);
}

BEGIN_EVENT_TABLE(ThreadSearchConfPanel, wxPanel)
    // begin wxGlade: ThreadSearchConfPanel::event_table
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkThreadSearchEnable), ThreadSearchConfPanel::OnThreadSearchEnable)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkShowMissingFilesError),
                 ThreadSearchConfPanel::OnChkShowMissingFilesErrorClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkShowCantOpenFileError),
                 ThreadSearchConfPanel::OnChkShowCantOpenFileErrorClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkViewThreadSearchToolBar),
                 ThreadSearchConfPanel::OnChkShowThreadSearchToolBarClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkShowThreadSearchWidgets),
                 ThreadSearchConfPanel::OnChkShowThreadSearchWidgetsClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkShowCodePreview), ThreadSearchConfPanel::OnChkCodePreview)
    // end wxGlade
END_EVENT_TABLE();

void ThreadSearchConfPanel::OnThreadSearchEnable(wxCommandEvent &event)
{
    m_pChkUseDefaultOptionsForThreadSearch->Enable(event.IsChecked());
    event.Skip();
}

void ThreadSearchConfPanel::OnChkShowThreadSearchToolBarClick(wxCommandEvent &event)
{
    if ( (event.IsChecked() == false) && (m_pChkShowThreadSearchWidgets->GetValue() == false) )
    {
        if ( wxID_YES != cbMessageBox(_("Do you really want to hide both ThreadSearch toolbar and widgets ?"),
                                      _("Sure ?"), wxICON_QUESTION|wxYES_NO) )
        {
            m_pChkShowThreadSearchToolBar->SetValue(true);
        }
    }
    event.Skip();
}

void ThreadSearchConfPanel::OnChkCodePreview(wxCommandEvent &event)
{
    m_pRadSplitterWndMode->Enable(event.IsChecked());
    event.Skip();
}

void ThreadSearchConfPanel::OnChkShowThreadSearchWidgetsClick(wxCommandEvent &event)
{
    if ( (event.IsChecked() == false) && (m_pChkShowThreadSearchToolBar->GetValue() == false) )
    {
        if ( wxID_YES != cbMessageBox(_("Do you really want to hide both ThreadSearch toolbar and widgets ?"),
                                      _("Sure ?"), wxICON_QUESTION|wxYES_NO) )
        {
            m_pChkShowThreadSearchWidgets->SetValue(true);
        }
    }
    event.Skip();
}

void ThreadSearchConfPanel::OnChkShowMissingFilesErrorClick(wxCommandEvent &event)
{
    Manager::Get()->GetConfigManager(_T("ThreadSearch"))->Write(wxT("/ShowFileMissingError"),event.IsChecked());
    event.Skip();
}

void ThreadSearchConfPanel::OnChkShowCantOpenFileErrorClick(wxCommandEvent &event)
{
    Manager::Get()->GetConfigManager(_T("ThreadSearch"))->Write(wxT("/ShowCantOpenFileError"),event.IsChecked());
    event.Skip();
}

// wxGlade: add ThreadSearchConfPanel event handlers
void ThreadSearchConfPanel::set_properties()
{
    // begin wxGlade: ThreadSearchConfPanel::set_properties
    m_Notebook->AddPage(m_PageGeneral, _("General"), true);
    m_Notebook->AddPage(m_PageLayout, _("Layout"), false);

    m_pChkWholeWord->SetToolTip(_("Search text matches only whole words"));
    m_pChkWholeWord->SetValue(1);
    m_pChkStartWord->SetToolTip(_("Matches only word starting with search expression"));
    m_pChkMatchCase->SetToolTip(_("Case sensitive search."));
    m_pChkMatchCase->SetValue(1);
    m_pChkRegExp->SetToolTip(_("Search expression is a regular expression"));
    m_pChkThreadSearchEnable->SetValue(1);
    m_pChkUseDefaultOptionsForThreadSearch->SetValue(1);
    m_pChkShowMissingFilesError->SetValue(1);
    m_pChkShowCantOpenFileError->SetValue(1);
    m_pChkDeletePreviousResults->SetValue(1);
    m_pChkShowThreadSearchToolBar->SetValue(1);
    m_pChkShowThreadSearchWidgets->SetValue(1);
    m_pChkShowCodePreview->SetValue(1);
    m_pChkDisplayLogHeaders->SetValue(1);
    m_pRadPanelManagement->SetSelection(0);
    m_pRadLoggerType->SetSelection(0);
    m_pRadSplitterWndMode->SetSelection(0);
    m_pRadSortBy->SetSelection(0);
    // end wxGlade
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("ThreadSearch"));
    m_pChkShowMissingFilesError->SetValue(pCfg->ReadBool(wxT("/ShowFileMissingError"),true));
    m_pChkShowCantOpenFileError->SetValue(pCfg->ReadBool(wxT("/ShowCantOpenFileError"),true));

    ThreadSearchFindData findData;
    m_ThreadSearchPlugin.GetFindData(findData);

    m_pChkWholeWord->SetValue(findData.GetMatchWord());
    m_pChkStartWord->SetValue(findData.GetStartWord());
    m_pChkMatchCase->SetValue(findData.GetMatchCase());
    m_pChkRegExp->SetValue(findData.GetRegEx());
    m_pChkThreadSearchEnable->SetValue(m_ThreadSearchPlugin.GetCtxMenuIntegration());
    m_pChkUseDefaultOptionsForThreadSearch->SetValue(m_ThreadSearchPlugin.GetUseDefValsForThreadSearch());
    m_pChkUseDefaultOptionsForThreadSearch->Enable(m_ThreadSearchPlugin.GetCtxMenuIntegration());
    m_pChkDisplayLogHeaders->SetValue(m_ThreadSearchPlugin.GetDisplayLogHeaders());
    m_pChkDrawLogLines->SetValue(m_ThreadSearchPlugin.GetDrawLogLines());
    m_pChkAutosizeLogColumns->SetValue(m_ThreadSearchPlugin.GetAutosizeLogColumns());
    m_pChkShowThreadSearchToolBar->SetValue(m_ThreadSearchPlugin.IsToolbarVisible());
    m_pChkShowThreadSearchWidgets->SetValue(m_ThreadSearchPlugin.GetShowSearchControls());
    m_pChkShowCodePreview->SetValue(m_ThreadSearchPlugin.GetShowCodePreview());
    m_pChkDeletePreviousResults->SetValue(m_ThreadSearchPlugin.GetDeletePreviousResults());

    int radIndex = 0;
    switch (m_ThreadSearchPlugin.GetManagerType())
    {
        case ThreadSearchViewManagerBase::TypeLayout :
        {
            radIndex = 1;
            break;
        }
        case ThreadSearchViewManagerBase::TypeMessagesNotebook : // fall through
        default:
        {
            radIndex = 0;
            break;
        }
    }
    m_pRadPanelManagement->SetSelection(radIndex);

    m_pRadLoggerType->SetSelection(m_ThreadSearchPlugin.GetLoggerType());

    radIndex = 0;
    switch (m_ThreadSearchPlugin.GetSplitterMode())
    {
        case wxSPLIT_HORIZONTAL:
        {
            radIndex = 0;
            break;
        }
        default :
        // case wxSPLIT_VERTICAL:
        {
            radIndex = 1;
            break;
        }
    }
    m_pRadSplitterWndMode->SetSelection(radIndex);
    m_pRadSplitterWndMode->Enable(m_ThreadSearchPlugin.GetShowCodePreview());

    radIndex = 0;
    switch (m_ThreadSearchPlugin.GetFileSorting())
    {
        case InsertIndexManager::SortByFileName:
        {
            radIndex = 1;
            break;
        }
        default:
        // case ThreadSearch::SortByFilePath :
        {
            radIndex = 0;
            break;
        }
    }
    m_pRadSortBy->SetSelection(radIndex);

    m_pPnlDirParams->SetSearchDirHidden(findData.GetHiddenSearch());
    m_pPnlDirParams->SetSearchDirRecursively(findData.GetRecursiveSearch());
    m_pPnlDirParams->SetSearchDirPath(findData.GetSearchPath());
    m_pPnlDirParams->SetSearchMask(findData.GetSearchMask());

    m_pPnlSearchIn->SetSearchInOpenFiles(findData.MustSearchInOpenFiles());
    m_pPnlSearchIn->SetSearchInTargetFiles(findData.MustSearchInTarget());
    m_pPnlSearchIn->SetSearchInProjectFiles(findData.MustSearchInProject());
    m_pPnlSearchIn->SetSearchInWorkspaceFiles(findData.MustSearchInWorkspace());
    m_pPnlSearchIn->SetSearchInDirectory(findData.MustSearchInDirectory());
}

void ThreadSearchConfPanel::do_layout()
{
    // begin wxGlade: ThreadSearchConfPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxVERTICAL);
    SizerTop->Add(m_Notebook, 1, wxEXPAND | wxALL, 4);

    {
        // Sizers for page general
        wxBoxSizer* SizerTop = new wxBoxSizer(wxVERTICAL);

        wxStaticBoxSizer* SizerSearchIn = new wxStaticBoxSizer(SizerSearchIn_staticbox, wxVERTICAL);
        wxBoxSizer* SizerOptions = new wxBoxSizer(wxHORIZONTAL);
        SizerSearchIn->Add(m_pPnlSearchIn, 0, wxALL|wxEXPAND, 2);
        SizerSearchIn->Add(m_pPnlDirParams, 0, wxALL|wxEXPAND, 2);
        SizerOptions->Add(m_pChkWholeWord, 0, wxLEFT | wxRIGHT, 4);
        SizerOptions->Add(m_pChkStartWord, 0, wxLEFT | wxRIGHT, 4);
        SizerOptions->Add(m_pChkMatchCase, 0, wxLEFT | wxRIGHT, 4);
        SizerOptions->Add(m_pChkRegExp, 0, wxLEFT | wxRIGHT, 4);
        SizerSearchIn->Add(SizerOptions, 0, wxALL|wxEXPAND, 4);
        SizerTop->Add(SizerSearchIn, 0, wxALL|wxEXPAND, 4);

        wxStaticBoxSizer* SizerThreadSearchOptions = new wxStaticBoxSizer(SizerThreadSearchOptions_staticbox, wxVERTICAL);
        SizerThreadSearchOptions->Add(m_pChkThreadSearchEnable, 0, wxALL, 4);
        SizerThreadSearchOptions->Add(m_pChkUseDefaultOptionsForThreadSearch, 0, wxALL, 4);
        wxStaticText* m_pStaDefaultOptions = new wxStaticText(m_PageGeneral, wxID_ANY, _("       ('Whole word' = true, 'Start word' = false, 'Match case' = true, 'Regular expression' = false)"));
        SizerThreadSearchOptions->Add(m_pStaDefaultOptions, 0, 0, 0);
        SizerThreadSearchOptions->Add(m_pChkShowMissingFilesError, 0, wxALL, 4);
        SizerThreadSearchOptions->Add(m_pChkShowCantOpenFileError, 0, wxALL, 4);
        SizerThreadSearchOptions->Add(m_pChkDeletePreviousResults, 0, wxALL, 4);
        SizerTop->Add(SizerThreadSearchOptions, 0, wxALL|wxEXPAND, 4);

        SizerTop->Add(m_pRadSortBy, 0, wxALL|wxEXPAND, 4);

        m_PageGeneral->SetSizer(SizerTop);
    }

    {
        // Sizers for page layout
        wxBoxSizer* SizerTop = new wxBoxSizer(wxVERTICAL);
        wxGridBagSizer *SizerThreadSearchGridLayout = new wxGridBagSizer;
        SizerThreadSearchGridLayout->SetCols(2);
        SizerThreadSearchGridLayout->AddGrowableCol(0);
        SizerThreadSearchGridLayout->AddGrowableCol(1);

        wxStaticBoxSizer* SizerListControlOptions = new wxStaticBoxSizer(SizerListControlOptions_staticbox, wxVERTICAL);
        wxStaticBoxSizer* SizerThreadSearchLayoutGlobal = new wxStaticBoxSizer(SizerThreadSearchLayoutGlobal_staticbox, wxVERTICAL);
        SizerThreadSearchLayoutGlobal->Add(m_pChkShowThreadSearchToolBar, 0, wxALL, 4);
        SizerThreadSearchLayoutGlobal->Add(m_pChkShowThreadSearchWidgets, 0, wxALL, 4);
        SizerThreadSearchLayoutGlobal->Add(m_pChkShowCodePreview, 0, wxALL, 4);
        SizerListControlOptions->Add(m_pChkDisplayLogHeaders, 0, wxALL, 4);
        SizerListControlOptions->Add(m_pChkDrawLogLines, 0, wxALL, 4);
        SizerListControlOptions->Add(m_pChkAutosizeLogColumns, 0, wxALL, 4);

        SizerThreadSearchGridLayout->Add(SizerThreadSearchLayoutGlobal, wxGBPosition(0, 0),
                                         wxDefaultSpan, wxALL|wxEXPAND, 4);
        SizerThreadSearchGridLayout->Add(SizerListControlOptions, wxGBPosition(0, 1), wxDefaultSpan,
                                         wxALL|wxEXPAND, 4);
        SizerThreadSearchGridLayout->Add(m_pRadPanelManagement, wxGBPosition(1, 0), wxDefaultSpan,
                                         wxALL|wxEXPAND, 4);
        SizerThreadSearchGridLayout->Add(m_pRadLoggerType, wxGBPosition(2, 0), wxDefaultSpan,
                                         wxALL|wxEXPAND, 4);
        SizerThreadSearchGridLayout->Add(m_pRadSplitterWndMode, wxGBPosition(3, 0), wxDefaultSpan,
                                         wxALL|wxEXPAND, 4);

        wxStaticBoxSizer* SizerThreadSearchLayoutSTCColours = new wxStaticBoxSizer(STCColours_staticbox, wxVERTICAL);
        wxFlexGridSizer* SizerThreadSearchLayoutSTCColoursGrid = new wxFlexGridSizer(5, 3, 0, 0);
        SizerThreadSearchLayoutSTCColours->Add(SizerThreadSearchLayoutSTCColoursGrid, 1, wxEXPAND | wxALL, 4);
        for (int ii = 0; ii < STCColoursCount; ii ++)
        {
            SizerThreadSearchLayoutSTCColoursGrid->Add(m_STCColoursLabels[ii], 1,
                                                       wxEXPAND |wxLEFT | wxTOP | wxALIGN_LEFT| wxALIGN_CENTRE_VERTICAL,
                                                       4);
            if (m_STCColourPickers[ii * 2] != nullptr)
                SizerThreadSearchLayoutSTCColoursGrid->Add(m_STCColourPickers[ii * 2], 1, wxEXPAND | wxLEFT | wxTOP, 4);
            if (m_STCColourPickers[ii * 2 + 1] != nullptr)
                SizerThreadSearchLayoutSTCColoursGrid->Add(m_STCColourPickers[ii * 2 + 1], 1, wxEXPAND | wxLEFT | wxTOP, 4);
        }

        SizerThreadSearchGridLayout->Add(SizerThreadSearchLayoutSTCColours, wxGBPosition(1, 1),
                                         wxGBSpan(3, 1), wxEXPAND | wxALL, 4);

        SizerTop->Add(SizerThreadSearchGridLayout, 1, wxALL|wxEXPAND, 4);

        m_PageLayout->SetSizer(SizerTop);
    }

    SetSizer(SizerTop);
    SizerTop->Fit(this);
    // end wxGlade
}

void ThreadSearchConfPanel::OnApply()
{
    ThreadSearchFindData findData;

    // Inits variable data from user values
    findData.SetFindText       (wxEmptyString);
    findData.SetHiddenSearch   (m_pPnlDirParams->GetSearchDirHidden());
    findData.SetRecursiveSearch(m_pPnlDirParams->GetSearchDirRecursively());
    findData.SetSearchPath     (m_pPnlDirParams->GetSearchDirPath());
    findData.SetSearchMask     (m_pPnlDirParams->GetSearchMask());
    findData.SetMatchWord      (m_pChkWholeWord->IsChecked());
    findData.SetStartWord      (m_pChkStartWord->IsChecked());
    findData.SetMatchCase      (m_pChkMatchCase->IsChecked());
    findData.SetRegEx          (m_pChkRegExp->IsChecked());

    findData.UpdateSearchScope(ScopeOpenFiles,      m_pPnlSearchIn->GetSearchInOpenFiles());
    findData.UpdateSearchScope(ScopeTargetFiles,    m_pPnlSearchIn->GetSearchInTargetFiles());
    findData.UpdateSearchScope(ScopeProjectFiles,   m_pPnlSearchIn->GetSearchInProjectFiles());
    findData.UpdateSearchScope(ScopeWorkspaceFiles, m_pPnlSearchIn->GetSearchInWorkspaceFiles());
    findData.UpdateSearchScope(ScopeDirectoryFiles, m_pPnlSearchIn->GetSearchInDirectory());

    // Updates plug-in with new datas
    m_ThreadSearchPlugin.SetFindData(findData);
    m_ThreadSearchPlugin.SetCtxMenuIntegration(m_pChkThreadSearchEnable->IsChecked());
    m_ThreadSearchPlugin.SetUseDefValsForThreadSearch(m_pChkUseDefaultOptionsForThreadSearch->IsChecked());
    m_ThreadSearchPlugin.SetShowSearchControls(m_pChkShowThreadSearchWidgets->IsChecked());
    m_ThreadSearchPlugin.SetShowCodePreview(m_pChkShowCodePreview->IsChecked());
    m_ThreadSearchPlugin.SetDeletePreviousResults(m_pChkDeletePreviousResults->IsChecked());
    m_ThreadSearchPlugin.SetDisplayLogHeaders(m_pChkDisplayLogHeaders->IsChecked());
    m_ThreadSearchPlugin.SetDrawLogLines(m_pChkDrawLogLines->IsChecked());
    m_ThreadSearchPlugin.SetAutosizeLogColumns(m_pChkAutosizeLogColumns->IsChecked());

    int radIndex = m_pRadPanelManagement->GetSelection();
    ThreadSearchViewManagerBase::eManagerTypes mgrType;
    switch (radIndex)
    {
        case 1 :
        {
            mgrType = ThreadSearchViewManagerBase::TypeLayout;
            break;
        }
        default:
        // case 0 :
        {
            mgrType = ThreadSearchViewManagerBase::TypeMessagesNotebook;
            break;
        }
    }
    m_ThreadSearchPlugin.SetManagerType(mgrType);

    const int radioLoggerType = m_pRadLoggerType->GetSelection();
    if (radioLoggerType >= 0 && radioLoggerType < ThreadSearchLoggerBase::TypeLast)
    {
        m_ThreadSearchPlugin.SetLoggerType(ThreadSearchLoggerBase::eLoggerTypes(radioLoggerType));
    }
    else
    {
        m_ThreadSearchPlugin.SetLoggerType(ThreadSearchLoggerBase::TypeList);
    }

    radIndex = m_pRadSortBy->GetSelection();
    InsertIndexManager::eFileSorting sortingType;
    switch (radIndex)
    {
        case 1 :
        {
            sortingType = InsertIndexManager::SortByFileName;
            break;
        }
        default:
        // case 0 :
        {
            sortingType = InsertIndexManager::SortByFilePath;
            break;
        }
    }
    m_ThreadSearchPlugin.SetFileSorting(sortingType);

    radIndex = m_pRadSplitterWndMode->GetSelection();
    wxSplitMode splitterMode;
    switch (radIndex)
    {
        case 1 :
        {
            splitterMode = wxSPLIT_VERTICAL;
            break;
        }
        default:
        // case 0 :
        {
            splitterMode = wxSPLIT_HORIZONTAL;
            break;
        }
    }
    m_ThreadSearchPlugin.SetSplitterMode(splitterMode);

    if (!m_ColoursInterface)
    {
        ColourManager *colours = Manager::Get()->GetColourManager();
        int ii = 0;
        colours->SetColour(wxT("thread_search_text_fore"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_text_back"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_file_fore"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_file_back"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_lineno_fore"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_lineno_back"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_match_fore"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_match_back"), m_STCColourPickers[ii++]->GetColour());
        colours->SetColour(wxT("thread_search_selected_line_back"),
                           m_STCColourPickers[ii++]->GetColour());
    }

    // Updates toolbar visibility
    m_ThreadSearchPlugin.ShowToolBar(m_pChkShowThreadSearchToolBar->IsChecked());

    // Notifies plug-in observers (ThreadSearchView)
    m_ThreadSearchPlugin.Notify();
}

void ThreadSearchConfPanel::OnPageChanging()
{
    if (m_ColoursInterface == nullptr)
        return;

    int ii = 0;
    cbConfigurationPanelColoursInterface *colours = m_ColoursInterface;
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_text_fore")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_text_back")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_file_fore")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_file_back")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_lineno_fore")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_lineno_back")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_match_fore")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_match_back")));
    m_STCColourPickers[ii++]->SetColour(colours->GetValue(wxT("thread_search_selected_line_back")));
}

static wxString findColourIDFromControlID(long controlID)
{
    if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker0))
        return "thread_search_text_fore";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker1))
        return "thread_search_text_back";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker2))
        return "thread_search_file_fore";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker3))
        return "thread_search_file_back";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker4))
        return "thread_search_lineno_fore";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker5))
        return "thread_search_lineno_back";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker6))
        return "thread_search_match_fore";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker7))
        return "thread_search_match_back";
    else if (controlID == controlIDs.Get(ControlIDs::idConfPanelColorPicker8))
        return "thread_search_selected_line_back";
    else
        return wxString();
}

void ThreadSearchConfPanel::OnColourPickerChanged(wxColourPickerEvent &event)
{
    if (!m_ColoursInterface)
        return;

    const long id = event.GetId();
    const wxString colourID = findColourIDFromControlID(id);
    if (!colourID.empty())
    {
        m_ColoursInterface->SetValue(colourID, event.GetColour());
    }
}

void ThreadSearchConfPanel::OnColourPickerContext(wxContextMenuEvent &event)
{
    const long id = event.GetId();
    const wxString colourID = findColourIDFromControlID(id);
    if (!colourID.empty())
    {
        wxColour defaultColour;
        if (m_ColoursInterface)
        {
            m_ColoursInterface->ResetDefault(colourID);
            defaultColour = m_ColoursInterface->GetValue(colourID);
        }
        else
        {
            ColourManager *colours = Manager::Get()->GetColourManager();
            defaultColour = colours->GetDefaultColour(colourID);
        }

        wxWindow *control = FindWindow(id);
        if (control)
            static_cast<wxColourPickerCtrl*>(control)->SetColour(defaultColour);
    }
}
