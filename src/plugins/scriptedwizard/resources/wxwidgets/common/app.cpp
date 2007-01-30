/***************************************************************
 * Name:      [PROJECT_NAME]App.cpp
 * Purpose:   Code for Application Class
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:  
 **************************************************************/

#ifdef USE_PCH //
#include "[PROJECT_NAME]_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "[PROJECT_NAME]App.h"
#include "[PROJECT_NAME]Main.h"

IMPLEMENT_APP([PROJECT_NAME]App);

bool [PROJECT_NAME]App::OnInit()
{
	[PROJECT_NAME]Frame* frame = new [PROJECT_NAME]Frame(0L, _("wxWidgets Application Template"));
	[IF WINDOWS]frame->SetIcon(wxICON(aaaa)); // To Set App Icon[ENDIF WINDOWS]
	frame->Show();
	return true;
}
