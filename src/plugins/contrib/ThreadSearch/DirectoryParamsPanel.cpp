/***************************************************************
 * Name:      DirectoryParamsPanel
 *
 * Purpose:   This class is a panel that allows the user to
 *            set the directory search parameters (dir,
 *            extensions...).
 *            It is used in the ThreadSearchView and the
 *            ThreadSearchConfPanel.
 *            It does nothing but forwarding events to the
 *            parent window.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/artprov.h>
    #include <wx/button.h>
    #include <wx/bmpbuttn.h>
    #include <wx/checkbox.h>
    #include <wx/combobox.h>
    #include <wx/dir.h>
    #include <wx/dirdlg.h>
    #include <wx/filename.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>

    #include <algorithm>

    #include "globals.h"
#endif

#include "DirectoryParamsPanel.h"
#include "ThreadSearchCommon.h"
#include "ThreadSearchControlIds.h"
#include "ThreadSearchFindData.h"

#include <wx/checklst.h>
#include <wx/statline.h>
#include <wx/textcompleter.h>
#include <mutex>

// Max number of items in search history combo box
const unsigned int MAX_NB_SEARCH_ITEMS = 20;

void AddItemToCombo(wxComboBox *combo, const wxString &str)
{
    // Removes item if already in the combo box
    while (1)
    {
        const int index = combo->FindString(str);
        if (index == wxNOT_FOUND)
            break;
        combo->Delete(index);
    }

    // Removes last item if max nb item is reached
    if (combo->GetCount() >= MAX_NB_SEARCH_ITEMS)
        combo->Delete(combo->GetCount()-1);

    // Adds it to combos
    combo->Insert(str, 0);
    combo->SetSelection(0);
}

/// Directory traverser which finds the sub-directories in a given directory.
/// It ignores everything else and doesn't do more than on 1 level.
struct DirTraverserSingleLevel : wxDirTraverser
{
    DirTraverserSingleLevel(wxArrayString &files) : m_files(files) {}

    wxDirTraverseResult OnFile(cb_unused const wxString &filename) override
    {
        return wxDIR_IGNORE;
    }

    wxDirTraverseResult OnDir(const wxString &dirname) override
    {
        m_files.push_back(dirname);
        return wxDIR_IGNORE;
    }

private:
    wxArrayString& m_files;
};

struct DirTextCompleter : wxTextCompleter
{
    bool Start(const wxString &prefix) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        wxString directory;

        // Extract the directory from the given string.
        if (wxDir::Exists(prefix))
            directory = prefix;
        else
            wxFileName::SplitPath(prefix, &directory, nullptr, nullptr);

        if (directory.empty())
            return false;

        // If the directory differs then we cannot use our old results and so we have to regenerate
        // them. If the directory is the same reuse the results, to make the completer a bit more
        // responsive.
        if (m_dirName != directory)
        {
            m_dirName = directory;
            m_filesInDir.clear();

            wxDir dir(directory);
            if (dir.IsOpened())
            {
                DirTraverserSingleLevel traverser(m_filesInDir);
                dir.Traverse(traverser, wxString(), wxDIR_DIRS);
            }

            std::sort(m_filesInDir.begin(), m_filesInDir.end());
        }

        m_index = 0;

        return true;
    }

    wxString GetNext() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_index >= int(m_filesInDir.size()))
            return wxString();
        else
            return m_filesInDir[m_index++];
    }

private:
    std::mutex m_mutex;
    wxString m_dirName;
    wxArrayString m_filesInDir;
    int m_index;
};

struct DirectorySelectDialog : wxDialog
{
    DirectorySelectDialog(wxWindow *parent, const wxString &paths, wxArrayString fullPathList,
                          const wxArrayString &entryPathList) :
        wxDialog(parent, wxID_ANY, _("Select directories"), wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        const int sizerBorder = 8;

        // Setup controls
        wxStaticText *entryText = new wxStaticText(this, wxID_ANY, _("Path:"));
        m_entry = new wxComboBox(this, controlIDs.Get(ControlIDs::idDirDialogCombo), wxString(),
                                 wxDefaultPosition, wxDefaultSize, wxArrayString(),
                                 wxTE_PROCESS_ENTER);
        m_entry->AutoComplete(new DirTextCompleter);
        m_entry->SetMinSize(wxSize(m_entry->GetCharWidth() * 60, -1));

        {
            // Fill the items in the list of the entry combo box.
            // Every item in the array might contain multiple items, so we need to split them
            wxArrayString fullEntryPathList;

            // This loop is not actually really efficient, but I hope we can get away with it,
            // because there is a limit to the number of entries in the list.
            const int count = int(entryPathList.size());
            for (int item = count - 1; item >= 0; --item)
            {
                const wxString &path = entryPathList[item];
                if (path.find(';') == wxString::npos)
                    AddItemToCombo(m_entry, path);
                else
                {
                    const wxArrayString &splitted = GetArrayFromString(path, ";", true);
                    for (const wxString &s : splitted)
                        AddItemToCombo(m_entry, s);
                }
            }

            // We have to reset the value because AddItemToCombo might have set it.
            m_entry->SetValue(wxString());
        }

        wxBitmapButton *buttonDirSelect = new wxBitmapButton(this, controlIDs.Get(ControlIDs::idDirDialogDirButton),
                                                             wxArtProvider::GetBitmap("core/folder_open", wxART_BUTTON));
        wxPanel *panelList = new wxPanel(this);
        {
            m_list = new wxCheckListBox(panelList, controlIDs.Get(ControlIDs::idDirDialogList),
                                        wxDefaultPosition, wxDefaultSize, wxArrayString(),
                                        wxLB_EXTENDED);
            m_list->SetMinSize(wxSize(m_list->GetCharWidth() * 70, m_list->GetCharHeight() * 20));

            const std::vector<wxString> &splittedPaths = GetVectorFromString(paths, ";", true);
            // Make sure all paths already selected are present in the full path list. If not add
            // the missing ones.
            for (const wxString &selected : splittedPaths)
            {
                bool found = false;
                for (const wxString &path : fullPathList)
                {
                    if (path == selected)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    fullPathList.push_back(selected);
            }

            std::sort(fullPathList.begin(), fullPathList.end());

            // Fill the list control. Selected paths are checked.
            for (const wxString &path : fullPathList)
            {
                const int newIndex = m_list->Append(path);

                bool check = false;
                for (const wxString &selected : splittedPaths)
                {
                    if (selected == path)
                    {
                        check = true;
                        break;
                    }
                }

                if (check)
                    m_list->Check(newIndex, true);
            }

            wxButton *buttonUnCheckUnselected = new wxButton(panelList, controlIDs.Get(ControlIDs::idDirDialogCheckSelectedButton),
                                                             "Check only selected");
            wxStaticLine *separator0 = new wxStaticLine(panelList);
            wxButton *buttonAdd = new wxButton(panelList,
                                               controlIDs.Get(ControlIDs::idDirDialogAddButton),
                                               _("&Add"));
            wxButton *buttonEdit = new wxButton(panelList,
                                                controlIDs.Get(ControlIDs::idDirDialogEditButton),
                                                _("&Edit"));
            wxStaticLine *separator1 = new wxStaticLine(panelList);
            wxButton *buttonDelete = new wxButton(panelList,
                                                  controlIDs.Get(ControlIDs::idDirDialogDeleteButton),
                                                  _("&Delete"));
            wxButton *buttonDeleteAll = new wxButton(panelList,
                                                     controlIDs.Get(ControlIDs::idDirDialogDeleteAllButton),
                                                     _("Delete a&ll"));

            wxBoxSizer *listButtonSizer = new wxBoxSizer(wxVERTICAL);
            listButtonSizer->Add(buttonUnCheckUnselected, 0, wxTOP | wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(separator0, 0, wxTOP | wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(buttonAdd, 0, wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(buttonEdit, 0, wxTOP | wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(separator1, 0, wxTOP | wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(buttonDelete, 0, wxTOP | wxEXPAND, sizerBorder / 2);
            listButtonSizer->Add(buttonDeleteAll, 0, wxTOP | wxEXPAND, sizerBorder / 2);

            wxBoxSizer *listSizer = new wxBoxSizer(wxHORIZONTAL);
            listSizer->Add(m_list, 1, wxRIGHT | wxEXPAND, sizerBorder / 2);
            listSizer->Add(listButtonSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL, sizerBorder / 2);

            listSizer->SetSizeHints(panelList);
            panelList->SetSizer(listSizer);
        }

        // Setup sizers
        wxBoxSizer *entrySizer = new wxBoxSizer(wxHORIZONTAL);
        entrySizer->Add(entryText, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerBorder / 2);
        entrySizer->Add(m_entry, 1, wxRIGHT | wxEXPAND, sizerBorder / 2);
        entrySizer->Add(buttonDirSelect, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerBorder / 2);


        wxStdDialogButtonSizer *buttonSizer = new wxStdDialogButtonSizer();
        buttonSizer->AddButton(new wxButton(this, wxID_CANCEL, _("&Cancel")));
        buttonSizer->AddButton(new wxButton(this, wxID_OK, _("&OK")));
        buttonSizer->Realize();

        wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
        topSizer->Add(entrySizer, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, sizerBorder);
        topSizer->Add(panelList, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, sizerBorder);
        topSizer->Add(buttonSizer, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, sizerBorder);
        topSizer->SetSizeHints(this);

        SetSizer(topSizer);
        Layout();
    }

    wxString GetPaths() const
    {
        wxString result;
        const unsigned int length = m_list->GetCount();
        for (unsigned int ii = 0; ii < length; ++ii)
        {
            if (!m_list->IsChecked(ii))
                continue;

            if (!result.empty())
                result += ';';
            result += m_list->GetString(ii);
        }
        return result;
    }

    wxArrayString GetFullPathList() const
    {
        return m_list->GetStrings();
    }

private:
    /// Inserts the path in the list. The list must be sorted and after the function returns it will
    /// remain sorted (the path would be inserted at the correct place). The item would be checked
    /// no matter if it is already present in the list or the function adds it.
    int InsertItemInList(const wxString &path)
    {
        int insertedIndex = -1;
        bool found = false;

        const unsigned listCount = m_list->GetCount();
        for (unsigned item = 0; item < listCount; ++item)
        {
            const wxString &itemStr = m_list->GetString(item);
            const int compare = itemStr.compare(path);
            if (compare >= 0)
            {
                found = (compare == 0);
                insertedIndex = item;
                break;
            }
        }

        if (!found)
        {
            if (insertedIndex == -1)
                insertedIndex = m_list->Append(path);
            else
                m_list->Insert(path, insertedIndex);
        }

        m_list->Check(insertedIndex, true);
        return insertedIndex;
    }

    void OnEnter(cb_unused wxCommandEvent &event)
    {
        const wxString &path = m_entry->GetValue();
        if (!path.empty())
        {
            AddItemToCombo(m_entry, path);
            InsertItemInList(path);
            m_entry->SetValue(wxString());
        }
    }

    void OnEdit(cb_unused wxCommandEvent &event)
    {
        wxArrayInt selected;
        m_list->GetSelections(selected);

        if (selected.empty())
            return;

        const int selectedItem = selected.front();
        const wxString &selectedPath = m_list->GetString(selectedItem);

        wxDirDialog dialog(this, _("Select directory"), selectedPath);
        if (dialog.ShowModal() == wxID_OK)
        {
            const wxString &newPath = dialog.GetPath();
            if (selectedPath != newPath)
            {
                // We want to keep the sorted property of the list, so we need to remove the old
                // item and insert a new one. It seems expected that the new item would be
                // automatically in a checked stated, no matter if the old item was checked or not.
                m_list->Delete(selectedItem);
                const int newItem = InsertItemInList(newPath);
                m_list->SetSelection(newItem);
            }
        }
    }

    void OnDirDialog(cb_unused wxCommandEvent &event)
    {
        wxString initialPath = m_entry->GetValue();

        if (initialPath.empty() && m_list->GetCount() > 0)
        {
            wxArrayInt selections;
            m_list->GetSelections(selections);
            if (!selections.empty())
                initialPath = m_list->GetString(selections[0]);
            else
                initialPath = m_list->GetString(0);
        }

        if (initialPath.empty())
            initialPath = wxGetCwd();

        wxDirDialog dialog(this, _("Select directory"), initialPath);
        if (dialog.ShowModal() == wxID_OK)
        {
            const wxString &newPath = dialog.GetPath();
            AddItemToCombo(m_entry, newPath);
            m_entry->SetValue(wxString()); // this should be after the call to AddItemToCombo
            InsertItemInList(newPath);
        }
    }

    void OnDelete(cb_unused wxCommandEvent &event)
    {
        wxArrayInt selected;
        m_list->GetSelections(selected);

        std::sort(selected.begin(), selected.end());

        int count = int(selected.size());
        for (int item = count - 1; item >= 0; --item)
        {
            m_list->Delete(selected[item]);
        }
    }

    void OnDeleteAll(cb_unused wxCommandEvent &event)
    {
        const unsigned int count = m_list->GetCount();
        for (unsigned int item = 0; item < count; ++item)
            m_list->Delete(0);
    }

    void OnCheckSelected(cb_unused wxCommandEvent &event)
    {
        const unsigned int count = m_list->GetCount();
        for (unsigned int item = 0; item < count; ++item)
        {
            m_list->Check(item, m_list->IsSelected(item));
        }
    }

    void OnUpdateUIHasSelected(wxUpdateUIEvent &event)
    {
        bool hasSelection = false;
        const unsigned count = m_list->GetCount();
        for (unsigned item = 0; item < count; ++item)
        {
            if (m_list->IsSelected(item))
            {
                hasSelection = true;
                break;
            }
        }
        event.Enable(hasSelection);
    }

    void OnUpdateUIHasText(wxUpdateUIEvent &event)
    {
        event.Enable(!m_entry->GetValue().empty());
    }

    void OnUpdateUIHasItems(wxUpdateUIEvent &event)
    {
        event.Enable(!m_list->IsEmpty());
    }
private:
    wxComboBox *m_entry;
    wxCheckListBox *m_list;
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DirectorySelectDialog, wxDialog)
    EVT_TEXT_ENTER(controlIDs.Get(ControlIDs::idDirDialogCombo), DirectorySelectDialog::OnEnter)

    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogAddButton), DirectorySelectDialog::OnEnter)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogEditButton), DirectorySelectDialog::OnEdit)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogDirButton), DirectorySelectDialog::OnDirDialog)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogDeleteButton), DirectorySelectDialog::OnDelete)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogDeleteAllButton), DirectorySelectDialog::OnDeleteAll)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idDirDialogCheckSelectedButton), DirectorySelectDialog::OnCheckSelected)

    EVT_LISTBOX_DCLICK(controlIDs.Get(ControlIDs::idDirDialogList), DirectorySelectDialog::OnEdit)

    EVT_UPDATE_UI(controlIDs.Get(ControlIDs::idDirDialogAddButton), DirectorySelectDialog::OnUpdateUIHasText)
    EVT_UPDATE_UI(controlIDs.Get(ControlIDs::idDirDialogEditButton), DirectorySelectDialog::OnUpdateUIHasSelected)
    EVT_UPDATE_UI(controlIDs.Get(ControlIDs::idDirDialogDeleteButton), DirectorySelectDialog::OnUpdateUIHasSelected)
    EVT_UPDATE_UI(controlIDs.Get(ControlIDs::idDirDialogCheckSelectedButton), DirectorySelectDialog::OnUpdateUIHasSelected)
    EVT_UPDATE_UI(controlIDs.Get(ControlIDs::idDirDialogDeleteAllButton), DirectorySelectDialog::OnUpdateUIHasItems)
END_EVENT_TABLE();

DirectoryParamsPanel::DirectoryParamsPanel(ThreadSearchFindData *findData, wxWindow* parent, int id, const wxPoint& pos,
                                           const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL),
    m_pFindData(findData)
{
    const wxString choices[] = {};

    // begin wxGlade: DirectoryParamsPanel::DirectoryParamsPanel
    m_pSearchDirPath = new wxComboBox(this, controlIDs.Get(ControlIDs::idSearchDirPath), wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize, 0, choices, wxCB_DROPDOWN|wxTE_PROCESS_ENTER);
    SetWindowMinMaxSize(*m_pSearchDirPath, 80, 180);
    m_pSearchDirPath->AutoComplete(new DirTextCompleter);

    m_pBtnSelectDir = new wxButton(this, controlIDs.Get(ControlIDs::idBtnDirSelectClick), _("..."));
    m_pChkSearchDirRecursively = new wxCheckBox(this, controlIDs.Get(ControlIDs::idChkSearchDirRecurse), _("Recurse"));
    m_pChkSearchDirHiddenFiles = new wxCheckBox(this, controlIDs.Get(ControlIDs::idChkSearchDirHidden), _("Hidden"));
    m_pMask = new wxComboBox(this, controlIDs.Get(ControlIDs::idSearchMask), findData->GetSearchMask(),
                             wxDefaultPosition, wxDefaultSize, 0, choices, wxCB_DROPDOWN|wxTE_PROCESS_ENTER);
    SetWindowMinMaxSize(*m_pMask, 30, 180);

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(DirectoryParamsPanel, wxPanel)
    // begin wxGlade: DirectoryParamsPanel::event_table
    EVT_TEXT_ENTER(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_TEXT(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_COMBOBOX(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idBtnDirSelectClick), DirectoryParamsPanel::OnBtnDirSelectClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkSearchDirRecurse), DirectoryParamsPanel::OnChkSearchDirRecurse)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkSearchDirHidden), DirectoryParamsPanel::OnChkSearchDirHidden)
    EVT_TEXT_ENTER(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    EVT_TEXT(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    EVT_COMBOBOX(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    // end wxGlade
END_EVENT_TABLE();


void DirectoryParamsPanel::OnSearchDirTextEvent(wxCommandEvent &event)
{
    m_pFindData->SetSearchPath(event.GetString());
    event.Skip();
}

void DirectoryParamsPanel::OnSearchMaskTextEvent(wxCommandEvent &event)
{
    m_pFindData->SetSearchMask(event.GetString());
    event.Skip();
}


void DirectoryParamsPanel::OnBtnDirSelectClick(wxCommandEvent &event)
{
    DirectorySelectDialog dialog(this, m_pSearchDirPath->GetValue(),
                                 m_pFindData->GetSearchPathFullList(),
                                 m_pSearchDirPath->GetStrings());
    if (dialog.ShowModal() == wxID_OK)
    {
        const wxString &result = dialog.GetPaths();
        m_pSearchDirPath->SetValue(result);
        AddItemToCombo(m_pSearchDirPath, result);
        m_pFindData->SetSearchPath(result);
        m_pFindData->SetSearchPathFullList(dialog.GetFullPathList());
    }

    event.Skip();
}


void DirectoryParamsPanel::OnChkSearchDirRecurse(wxCommandEvent &event)
{
    m_pFindData->SetRecursiveSearch(event.IsChecked());
    event.Skip();
}
void DirectoryParamsPanel::OnChkSearchDirHidden(wxCommandEvent &event)
{
    m_pFindData->SetHiddenSearch(event.IsChecked());
    event.Skip();
}


// wxGlade: add DirectoryParamsPanel event handlers


void DirectoryParamsPanel::set_properties()
{
    // begin wxGlade: DirectoryParamsPanel::set_properties
    m_pSearchDirPath->SetToolTip(_("Directory to search in files"));
    m_pBtnSelectDir->SetToolTip(_("Browse for directory to search in"));
    m_pChkSearchDirRecursively->SetToolTip(_("Search in directory files recursively"));
    m_pChkSearchDirRecursively->SetValue(1);
    m_pChkSearchDirHiddenFiles->SetToolTip(_("Search in directory hidden files"));
    m_pChkSearchDirHiddenFiles->SetValue(1);
    m_pMask->SetToolTip(wxT("*.cpp;*.c;*.h"));
    // end wxGlade
}


void DirectoryParamsPanel::do_layout()
{
    // begin wxGlade: DirectoryParamsPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pSearchDirPath, 2, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    SizerTop->Add(m_pBtnSelectDir, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    SizerTop->Add(m_pChkSearchDirRecursively, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    SizerTop->Add(m_pChkSearchDirHiddenFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    SizerTop->Add(m_pMask, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    wxStaticText* m_pStatTxtMask = new wxStaticText(this, -1, _("mask"));
    SizerTop->Add(m_pStatTxtMask, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}

// Getters
wxString DirectoryParamsPanel::GetSearchDirPath()        const {return m_pSearchDirPath->GetValue();}
bool     DirectoryParamsPanel::GetSearchDirRecursively() const {return m_pChkSearchDirRecursively->IsChecked();}
bool     DirectoryParamsPanel::GetSearchDirHidden()      const {return m_pChkSearchDirHiddenFiles->IsChecked();}
wxString DirectoryParamsPanel::GetSearchMask()           const {return m_pMask->GetValue();}

// Setters
void     DirectoryParamsPanel::SetSearchDirPath(const wxString& sDirPath) {m_pSearchDirPath->SetValue(sDirPath);}
void     DirectoryParamsPanel::SetSearchDirRecursively(bool bRecurse)     {m_pChkSearchDirRecursively->SetValue(bRecurse);}
void     DirectoryParamsPanel::SetSearchDirHidden(bool bSearchHidden)     {m_pChkSearchDirHiddenFiles->SetValue(bSearchHidden);}
void     DirectoryParamsPanel::SetSearchMask(const wxString& sMask)       {m_pMask->SetValue(sMask);}

void DirectoryParamsPanel::SetSearchHistory(const wxArrayString& searchDirs, const wxArrayString& searchMasks)
{
    for (wxArrayString::const_iterator it = searchDirs.begin(); it != searchDirs.end(); ++it)
    {
        if (!it->empty())
            m_pSearchDirPath->Append(*it);
    }
    for (wxArrayString::const_iterator it = searchMasks.begin(); it != searchMasks.end(); ++it)
    {
        if (!it->empty())
            m_pMask->Append(*it);
    }
}

wxArrayString DirectoryParamsPanel::GetSearchDirsHistory() const
{
    return m_pSearchDirPath->GetStrings();
}

wxArrayString DirectoryParamsPanel::GetSearchMasksHistory() const
{
    return m_pMask->GetStrings();
}

void DirectoryParamsPanel::AddExpressionToCombos(const wxString& path, const wxString& mask)
{
    AddItemToCombo(m_pSearchDirPath, path);
    AddItemToCombo(m_pMask, mask);
}
