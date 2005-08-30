#ifndef __WXSLISTBOX_H
#define __WXSLISTBOX_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsListBoxStyles)
WXS_EV_DECLARE(wxsListBoxEvents)

wxsDWDeclareBegin(wxsListBox,propWidget,wxsListBoxId)

  // for default constructor
    //int n;  // Number of strings with which to initialise the control 
    //wxString choices[3]; //An array of strings with which to initialise the control
  // for constructor 2  
    wxArrayString arrayChoices;
    int defaultChoice;
    
wxsDWDeclareEnd()


#endif
