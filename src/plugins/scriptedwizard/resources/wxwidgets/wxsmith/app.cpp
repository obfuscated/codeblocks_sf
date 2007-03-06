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

//(*AppHeaders
//*)

IMPLEMENT_APP([PROJECT_NAME]App);

bool [PROJECT_NAME]App::OnInit()
{
	//(*AppInitialize
	//*)
	return wxsOK;

}
