/***************************************************************
 * Name:      IETestApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "IETestApp.h"

//(*AppHeaders
#include "IETestMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(IETestApp);

bool IETestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	IETestFrame* Frame = new IETestFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
