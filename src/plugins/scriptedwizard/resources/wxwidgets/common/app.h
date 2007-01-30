/***************************************************************
 * Name:      [PROJECT_NAME]App.h
 * Purpose:   Defines Application Class
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:   
 **************************************************************/
 
#ifndef [PROJECT_HDR]APP_H
#define [PROJECT_HDR]APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class [PROJECT_NAME]App : public wxApp
{
	public:
		virtual bool OnInit();
};

#endif // [PROJECT_NAME]APP_H
