#ifndef HELPCONFIGDIALOG_H
#define HELPCONFIGDIALOG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include "help_common.h"

class HelpPlugin;

class HelpConfigDialog : public cbConfigurationPanel
{
  private:
    HelpCommon::HelpFilesVector m_Vector;
    int m_LastSel;

  public:
    HelpConfigDialog(wxWindow* parent, HelpPlugin* plugin);
    virtual ~HelpConfigDialog();

    virtual wxString GetTitle() const { return _("Help files"); }
    virtual wxString GetBitmapBaseName() const { return _T("help-plugin"); }
    virtual void OnApply();
    virtual void OnCancel(){}
  protected:
    void Add(wxCommandEvent &event);
    void Rename(wxCommandEvent &event);
    void Delete(wxCommandEvent &event);
    void Browse(wxCommandEvent &event);
    void OnUp(wxCommandEvent &event);
    void OnDown(wxCommandEvent &event);
    void ListChange(wxCommandEvent &event);
    void OnCheckbox(wxCommandEvent &event);
    void OnCheckboxExecute(wxCommandEvent &event);
    void OnCheckboxEmbeddedViewer(wxCommandEvent &event);
    // Patch by Yorgos Pagles: Add new gui elements' events for setting the new attributes
    void OnDefaultKeywordEntry(wxCommandEvent &event);
    void OnCaseChoice(wxCommandEvent &event);
    void UpdateUI(wxUpdateUIEvent &event);

  private:
    void UpdateEntry(int index);
    void ChooseFile();
    HelpPlugin* m_pPlugin;

    DECLARE_EVENT_TABLE()
};

#endif // HELPCONFIGDIALOG
