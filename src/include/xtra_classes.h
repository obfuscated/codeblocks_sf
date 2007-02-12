#ifndef xtra_classes_h
#define xtra_classes_h

/** xtra_classes
  * by Ricardo Garcia
  * Some extra classes derived from wxWidgets.
  * (Developed as an auxiliary library for project "Code::Blocks")
  * This file is distributed under the wxWindows license
  */

#include "settings.h"
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/sizer.h>

/** A simple panel with included wxSplitterWindow and a sizer. Nothing more
  *
  */
class DLLIMPORT wxSplitPanel : public wxPanel
{
public:
    wxSplitPanel() { /*nothing to init, really */ }
    wxSplitPanel(wxWindow* parent, wxWindowID id = -1,
                               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                               long style = wxTAB_TRAVERSAL, const wxString& name = _T("splitpanel"),const wxString configname = wxEmptyString,int defaultsashposition = 150)
    {
        Create(parent, id, pos, size, style, name,configname,defaultsashposition);
    }
    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL, const wxString& name = _T("splitpanel"),const wxString configname = wxEmptyString,int defaultsashposition = 150);

    /** Refreshes the splitter. Windows displayed are searched by Id.
      * If only one is found, the panel is not split.
      *
      */
    void RefreshSplitter(int idtop,int idbottom);

    virtual ~wxSplitPanel();
    wxSplitterWindow* GetSplitter() { return (this) ? m_splitter : 0L; }
    void SetConfigEntryForSplitter(const wxString& splitterconfig){ m_SplitterConfig = splitterconfig; }
protected:
    int m_IniSashPos,m_defaultsashposition,m_lastsashposition;
    wxString m_SplitterConfig;
private:
    virtual void OnUpdateUI(wxUpdateUIEvent& event);
    wxSplitterWindow* m_splitter;
    wxBoxSizer* m_sizer;
    DECLARE_DYNAMIC_CLASS(wxSplitPanel)
    DECLARE_EVENT_TABLE()
};

#endif
