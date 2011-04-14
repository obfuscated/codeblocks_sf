#include "Addr2LineUIApp.h"
#include "wx_pch.h"

//(*AppHeaders
#include "Addr2LineUIMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(Addr2LineUIApp);

bool Addr2LineUIApp::OnInit()
{
  wxApp::SetAppName   (wxT("Addr2LineUI"));
  wxApp::SetVendorName(wxT("MortenMacFly"));

  //(*AppInitialize
  bool wxsOK = true;
  wxInitAllImageHandlers();
  if ( wxsOK )
  {
  	Addr2LineUIDialog Dlg(0);
  	SetTopWindow(&Dlg);
  	Dlg.ShowModal();
  	wxsOK = false;
  }
  //*)
  return wxsOK;
}
