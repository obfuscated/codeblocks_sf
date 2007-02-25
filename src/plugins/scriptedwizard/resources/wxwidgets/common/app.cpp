/***************************************************************
 * Name:      [PROJECT_NAME]App.cpp
 * Purpose:   Code for Application Class
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:  
 **************************************************************/

#ifdef WX_PRECOMP //
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "[PROJECT_NAME]App.h"
#include "[PROJECT_NAME]Main.h"

IMPLEMENT_APP([PROJECT_NAME]App);

bool [PROJECT_NAME]App::OnInit()
{
	[IF WXFRAME][PROJECT_NAME]Frame* frame = new [PROJECT_NAME]Frame(0L[IF NONE], _("wxWidgets Application Template")[ENDIF NONE]);
	[IF WINDOWS]frame->SetIcon(wxICON(aaaa)); // To Set App Icon[ENDIF WINDOWS]
	frame->Show();[ENDIF WXFRAME]
	[IF WXDIALOG][PROJECT_NAME]Dialog* dlg = new [PROJECT_NAME]Dialog(0L[IF NONE], _("wxWidgets Application Template")[ENDIF NONE]);
	[IF WINDOWS]dlg->SetIcon(wxICON(aaaa)); // To Set App Icon[ENDIF WINDOWS]
	dlg->Show();[ENDIF WXDIALOG]
	return true;
}
