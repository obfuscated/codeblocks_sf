#ifndef __WXSCOMBOBOX_H
#define __WXSCOMBOBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsComboBoxStyles)
WXS_EV_DECLARE(wxsComboBoxEvents)

/******************************************************************************/
/* There's bug in wx 2.6.1 - wxComboBox is not derived from wxControlWithItems*/
/* when compiling on GTK. Because of that, wxsComboBox must be threated       */
/* especially                                                                 */
/******************************************************************************/

wxsDWDeclareBegin(wxsComboBoxBase,wxsComboBoxId)

  // for default constructor
    //int n;  // Number of strings with which to initialise the control
    //wxString choices[3]; //An array of strings with which to initialise the control
  // for constructor 2
    wxArrayString arrayChoices;
    int defaultChoice;

wxsDWDeclareEnd()

class wxsComboBox: public wxsComboBoxBase
{
    public:
        wxsComboBox(wxsWidgetManager* Man,wxsWindowRes* Res):
            wxsComboBoxBase(Man,Res)
        {}

    protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        virtual wxString GetProducingCode(wxsCodeParams& Params);

};


#endif
