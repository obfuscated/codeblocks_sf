/***************************************************************
 * Name:      wxSmithDemoApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Gary Harris (garyjharris@sourceforge.net)
 * Created:   2010-06-01
 * Copyright: Gary Harris (http://cryogen.66ghz.com/)
 * License:
 **************************************************************/

#include "wxSmithDemoApp.h"

//(*AppHeaders
#include "wxSmithDemoMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(wxSmithDemoApp);

bool wxSmithDemoApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	wxSmithDemoFrame* Frame = new wxSmithDemoFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
