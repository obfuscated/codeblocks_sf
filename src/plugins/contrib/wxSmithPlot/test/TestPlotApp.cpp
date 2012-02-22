/***************************************************************
 * Name:      TestPlotApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-02-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "TestPlotApp.h"

//(*AppHeaders
#include "TestPlotMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(TestPlotApp);

bool TestPlotApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	TestPlotFrame* Frame = new TestPlotFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
