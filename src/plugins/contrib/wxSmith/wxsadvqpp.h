#ifndef WXSADVQPP_H
#define WXSADVQPP_H

#include "properties/wxsquickpropspanel.h"
#include <wx/button.h>
#include <wx/sizer.h>

class wxsAdvQPPChild;

/** \brief Advanced quick properties panel
 *
 * This panel is extension of standard wxsQuickPropsPanel.
 * It consist of set of wxsAdvQPPChild panels, each one groups set of
 * properties. All wxsAdvQPPChild classes mustbe manually registered using
 * wxsAdvQPP::Register function
 *
 * Each property group has it's title and can be hidden when it's unnecessary.
 */
class wxsAdvQPP : public wxsQuickPropsPanel
{
    public:
        /** \brief Ctor
         *
         * Number of parameters was reduced comparing to wxsQuickPropsPanel's
         * constructor. The reason is that this panel will be used in editor
         * area and some parameters must have specified values.
         */
        wxsAdvQPP(wxWindow* Parent,wxsPropertyContainer* Container);

        /** \brief Dctor */
        virtual ~wxsAdvQPP();

        /** \brief Function registering child panel */
        void Register(wxsAdvQPPChild* Child,const wxString& Title);

    private:

        /** \brief Funnction updating content of all child panels */
        void Update();

        /** \brief Overriding NotifyChange funnction to give access from
         *         wxsAdvQPPChild
         */
        inline void NotifyChange() { wxsQuickPropsPanel::NotifyChange(); }

        /** \brief Notification on toggling button press */
        void OnToggleButton(wxCommandEvent& event);

        WX_DEFINE_ARRAY(wxsAdvQPPChild*,wxArrayAdvQPPChild);
        WX_DEFINE_ARRAY(wxButton*,wxArrayButton);

        wxArrayAdvQPPChild Children;    ///< \brief Array of child panels
        wxArrayButton Buttons;          ///< \brief Buttons used to toggle
        wxBoxSizer* Sizer;              ///< \brief Sizer layouting panels

        DECLARE_EVENT_TABLE()

        friend class wxsAdvQPPChild;
};

#endif
