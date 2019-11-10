#include "avChangesDlg.h"

//(*InternalHeaders(avChangesDlg)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/ffile.h>
#include <wx/filefn.h>

//Choices to initialize the l_TypesArray
const wxString strTypes[] =
{
    _T("Added"), _T("Applied"), _T("Cleaned"),
    _T("Fixed"), _T("New"), _T("Removed"),
    _T("Typo"), _T("Updated")
};

//List of choices available for the data grid column [Type]
const wxArrayString g_TypesArray(sizeof(strTypes) / sizeof(wxString), strTypes);

//(*IdInit(avChangesDlg)
const long avChangesDlg::ID_ADD_BUTTON = wxNewId();
const long avChangesDlg::ID_EDIT_BUTTON = wxNewId();
const long avChangesDlg::ID_DELETE_BUTTON = wxNewId();
const long avChangesDlg::ID_CHANGES_GRID = wxNewId();
const long avChangesDlg::ID_SAVE_BUTTON = wxNewId();
const long avChangesDlg::ID_WRITE_BUTTON = wxNewId();
const long avChangesDlg::ID_CANCEL_BUTTON = wxNewId();
//*)

BEGIN_EVENT_TABLE(avChangesDlg,wxScrollingDialog)
    //(*EventTable(avChangesDlg)
    //*)
END_EVENT_TABLE()

avChangesDlg::avChangesDlg(wxWindow* parent,wxWindowID /*id*/)
{
    //(*Initialize(avChangesDlg)
    wxBoxSizer* sizerConfirmation;
    wxBoxSizer* sizerButtons;

    Create(parent, wxID_ANY, _("AutoVersioning :: Changes Log"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(700,300));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    sizerButtons = new wxBoxSizer(wxHORIZONTAL);
    btnAdd = new wxButton(this, ID_ADD_BUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ADD_BUTTON"));
    btnAdd->SetToolTip(_("Adds another row to the data grid"));
    sizerButtons->Add(btnAdd, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    btnEdit = new wxButton(this, ID_EDIT_BUTTON, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EDIT_BUTTON"));
    btnEdit->SetToolTip(_("Enters edit mode"));
    sizerButtons->Add(btnEdit, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    btnDelete = new wxButton(this, ID_DELETE_BUTTON, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DELETE_BUTTON"));
    btnDelete->SetToolTip(_("Deletes the selected row"));
    sizerButtons->Add(btnDelete, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(sizerButtons, 0, wxALL|wxEXPAND, 5);
    grdChanges = new wxGrid(this, ID_CHANGES_GRID, wxDefaultPosition, wxDefaultSize, 0, _T("ID_CHANGES_GRID"));
    BoxSizer1->Add(grdChanges, 1, wxALL|wxEXPAND, 5);
    sizerConfirmation = new wxBoxSizer(wxHORIZONTAL);
    btnSave = new wxButton(this, ID_SAVE_BUTTON, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SAVE_BUTTON"));
    btnSave->SetDefault();
    btnSave->SetToolTip(_("Save the changes for later use"));
    sizerConfirmation->Add(btnSave, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    btnWrite = new wxButton(this, ID_WRITE_BUTTON, _("Write"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_WRITE_BUTTON"));
    btnWrite->SetToolTip(_("Write the changes to the changeslog and clean the data grid"));
    sizerConfirmation->Add(btnWrite, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    btnCancel = new wxButton(this, ID_CANCEL_BUTTON, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CANCEL_BUTTON"));
    btnCancel->SetToolTip(_("Discards any change made"));
    sizerConfirmation->Add(btnCancel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(sizerConfirmation, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    SetSizer(BoxSizer1);
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_ADD_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnAddClick);
    Connect(ID_EDIT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnEditClick);
    Connect(ID_DELETE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnDeleteClick);
    Connect(ID_SAVE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnSaveClick);
    Connect(ID_WRITE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnWriteClick);
    Connect(ID_CANCEL_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnCancelClick);
    //*)

    grdChanges->CreateGrid(0,2);
    grdChanges->SetColLabelValue(0,_T("Type"));
    grdChanges->SetColLabelValue(1,_T("Description"));

    grdChanges->AutoSize();
	grdChanges->SetColSize(0, 60);
	grdChanges->SetColSize(1, 645);
}

avChangesDlg::~avChangesDlg()
{
    //(*Destroy(avChangesDlg)
    //*)
}

//{Event Functions
void avChangesDlg::OnBtnAddClick(wxCommandEvent& /*event*/)
{
    grdChanges->AppendRows(1);
    grdChanges->SetCellEditor(grdChanges->GetNumberRows()-1, 0, new wxGridCellChoiceEditor(g_TypesArray,true));
    grdChanges->SetGridCursor(grdChanges->GetNumberRows()-1, 0);
    grdChanges->EnableCellEditControl(true);
}

void avChangesDlg::OnBtnEditClick(wxCommandEvent& /*event*/)
{
    grdChanges->EnableCellEditControl(true);
}

void avChangesDlg::OnBtnDeleteClick(wxCommandEvent& /*event*/)
{
    if (grdChanges->GetNumberRows() > 0)
    {
#if wxCHECK_VERSION(3, 0, 0)
        int row = grdChanges->GetGridCursorRow();
#else
        int row = grdChanges->GetCursorRow();
#endif
        grdChanges->SelectRow(row);
        if (wxMessageBox(_("You are about to delete the selected row"), _("Warning"), wxICON_EXCLAMATION|wxOK|wxCANCEL, this) == wxOK)
        {
            grdChanges->DeleteRows(row, 1, true);
        }
    }
}

void avChangesDlg::OnBtnSaveClick(wxCommandEvent& /*event*/)
{
    if (grdChanges->GetNumberRows() > 0)
    {
        wxFFile saveTempChangesFile;
        saveTempChangesFile.Open(m_tempChangesFile, _T("w"));

        wxString tempChanges;

        for (int i=0; i<grdChanges->GetNumberRows(); ++i)
        {
            tempChanges += grdChanges->GetCellValue(i,0);
            tempChanges += _T("\t");

            tempChanges += grdChanges->GetCellValue(i,1);
            tempChanges += _T("\n");
        }

        saveTempChangesFile.Write(tempChanges);
        EndModal(0);
    }
    else
    {
        wxMessageBox(_("There are no rows in the data grid to write."), _("Error"), wxICON_ERROR );
    }
}

void avChangesDlg::OnBtnWriteClick(wxCommandEvent& /*event*/)
{
    if (grdChanges->GetNumberRows() > 0)
    {
        for (int i=0; i<grdChanges->GetNumberRows(); ++i)
        {
            if (grdChanges->GetCellValue(i,0) != _T(""))
            {
                m_changes += grdChanges->GetCellValue(i,0) + _T(": ");
            }

            m_changes += grdChanges->GetCellValue(i,1);

            if(i != grdChanges->GetNumberRows()-1)
            {
                m_changes += _T("\n");
            }
        }

        wxRemoveFile(m_tempChangesFile);
        EndModal(0);
    }
    else
    {
        wxMessageBox(_("There are no rows in the data grid to write."), _("Error"), wxICON_ERROR );
    }
}

void avChangesDlg::OnBtnCancelClick(wxCommandEvent& /*event*/)
{
    EndModal(0);
}
//}

//{Const Properties
void avChangesDlg::SetTemporaryChangesFile(const wxString& fileName)
{
    m_tempChangesFile = fileName;

    wxFFile tempChangesFile;
    if (tempChangesFile.Open(m_tempChangesFile))
    {
        wxString fileContent(_T("")), type(_T("")), description(_T(""));

        tempChangesFile.ReadAll(&fileContent);

        bool isType = true;
        grdChanges->BeginBatch();

        for( size_t i = 0; i < fileContent.Len(); ++i)
        {
            if (isType)
            {
                if (fileContent[i] != L'\t')
                {
                    type += fileContent[i];
                }
                else
                {
                    isType = false;
                }
            }
            else
            {
                if (fileContent[i] != L'\n')
                {
                    description += fileContent[i];
                }
                else
                {
                    grdChanges->AppendRows(1);
                    grdChanges->SetCellValue(grdChanges->GetNumberRows()-1, 0, type);
                    grdChanges->SetCellEditor(grdChanges->GetNumberRows()-1, 0, new wxGridCellChoiceEditor(g_TypesArray,true));
                    grdChanges->SetCellValue(grdChanges->GetNumberRows()-1, 1, description);
                    type = _T("");
                    description = _T("");
                    isType = true;
                }
            }
        }

        grdChanges->AutoSize();
        grdChanges->EndBatch();
    }
}

const wxString& avChangesDlg::Changes() const
{
    return m_changes;
}
//}
