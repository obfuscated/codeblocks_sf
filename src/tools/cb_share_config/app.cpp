#include "app.h"

//(*AppHeaders
#include "mainframe.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{

	//(*AppInitialize
	bool wxsOK = true;
	::wxInitAllImageHandlers();
	if ( wxsOK )
	{
	  MainFrame* MainResource = new MainFrame(0L);
	  if ( MainResource ) MainResource->Show();
	}
	//*)

	return wxsOK;
}
