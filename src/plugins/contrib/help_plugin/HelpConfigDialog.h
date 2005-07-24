#ifndef HELPCONFIGDIALOG_H
#define HELPCONFIGDIALOG_H

#include <wx/dialog.h>
#include "help_common.h"

class HelpConfigDialog : public wxDialog
{
  private:
    HelpCommon::HelpFilesVector m_Vector;
    int m_LastSel;
  
  public:
    HelpConfigDialog();
    virtual ~HelpConfigDialog();
  
  protected:
    void Add(wxCommandEvent &event);
    void Rename(wxCommandEvent &event);
    void Delete(wxCommandEvent &event);
    void Browse(wxCommandEvent &event);
    void OnUp(wxCommandEvent &event);
    void OnDown(wxCommandEvent &event);
    void ListChange(wxCommandEvent &event);
    void OnCheckbox(wxCommandEvent &event);
    void UpdateUI(wxUpdateUIEvent &event);
    void Ok(wxCommandEvent &event);
  
  private:
    void UpdateEntry(int index);
    void ChooseFile();
  
    DECLARE_EVENT_TABLE()
};

#endif // HELPCONFIGDIALOG
