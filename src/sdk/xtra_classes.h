#ifndef xtra_classes_h
#define xtra_classes_h

/** xtra_classes
  * by Ricardo Garcia
  * Some extra classes derived from wxWidgets.
  * (Developed as an auxiliary library for project "Code::Blocks")
  * This file is distributed under the wxWindows license
  */

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/sizer.h>

/** A simple panel with included wxSplitterWindow and a sizer. Nothing more
  *
  */
class wxSplitPanel : public wxPanel
{
public:
    wxSplitPanel::wxSplitPanel() { /*nothing to init, really */ }
    wxSplitPanel::wxSplitPanel(wxWindow* parent, wxWindowID id = -1,
                               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                               long style = wxTAB_TRAVERSAL, const wxString& name = "splitpanel")
    {
        Create(parent, id, pos, size, style, name);
    }
    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL, const wxString& name = "splitpanel");

    /** Refreshes the splitter. Windows displayed are searched by Id.
      * If only one is found, the panel is not split.
      *
      */
    void wxSplitPanel::RefreshSplitter(int idtop,int idbottom,int sashPosition = 0);

    virtual wxSplitPanel::~wxSplitPanel();
    wxSplitterWindow* GetSplitter() { return (this) ? m_splitter : 0L; }
    void SetConfigEntryForSplitter(const wxString& splitterconfig){ m_SplitterConfig = splitterconfig; }
private:
    wxSplitterWindow* m_splitter;
    wxBoxSizer* m_sizer;
    wxString m_SplitterConfig;
    DECLARE_DYNAMIC_CLASS(wxSplitPanel)
};

#endif
