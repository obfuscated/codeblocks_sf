#ifndef WXSGRIDPANEL_H
#define WXSGRIDPANEL_H

#include <wx/panel.h>

/** \brief Class used for containers like wxPanel, where items can be freely placed,
 *         it shows extra grid and helps visual editor to snap items to grid
 */
class wxsGridPanel: public wxPanel
{
    public:

        /** \brief Empty ctor */
        wxsGridPanel();

        /** \brief Ctor */
        wxsGridPanel(wxWindow* parent, wxWindowID id=wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxTAB_TRAVERSAL, const wxString& name=_T("wxsGridPanel"));

        /** \brief Dctor */
        virtual ~wxsGridPanel();

        /** \brief Reading grid size from system configuration */
        static int GetGridSize();

    protected:

        /** \brief Paint procedure */
        void OnPaint(wxPaintEvent& event);

        /** \brief Checking whether we should draw border around client area */
        virtual bool DrawBorder() { return false; }

        DECLARE_CLASS(wxsGridPanel)
        DECLARE_EVENT_TABLE()
};

#endif
