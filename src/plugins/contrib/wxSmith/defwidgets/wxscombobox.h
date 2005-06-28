#ifndef __WXSCOMBOBOX_H
#define __WXSCOMBOBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsComboBoxStyles)

wxsDWDeclareBegin(wxsComboBox,propWidget,wxsComboBoxId)

  // for default constructor
    //int n;  // Number of strings with which to initialise the control 
    //wxString choices[3]; //An array of strings with which to initialise the control
  // for constructor 2  
    wxArrayString arrayChoices;
    int defaultChoice;
    
wxsDWDeclareEnd()


#endif
