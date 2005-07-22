#ifndef HELPCONFIGDIALOG_H
#define HELPCONFIGDIALOG_H

#include <wx/dialog.h>
#include "help_common.h"

class HelpConfigDialog : public wxDialog
{
  private:
    HelpFilesMap m_Map;
    int m_LastSel;
  
  public:
    HelpConfigDialog();
    virtual ~HelpConfigDialog();
  
  protected:
    void Add(wxCommandEvent &event);
    void Rename(wxCommandEvent &event);
    void Delete(wxCommandEvent &event);
    void Browse(wxCommandEvent &event);
    void ListChange(wxCommandEvent &event);
    void UpdateUI(wxUpdateUIEvent &event);
    void Ok(wxCommandEvent &event);
    void Cancel(wxCommandEvent &event);
  
  private:
    void UpdateEntry(int index);
    void ChooseFile();
    
    DECLARE_EVENT_TABLE()
};

#endif // HELPCONFIGDIALOG
