/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"
#include "printing_types.h"

#ifndef CB_PRECOMP
    #include "manager.h"
    #include "configmanager.h"
#endif

// NOTE (Tiwag#1#): 061012 global wxPrinter, used in cbeditorprintout
//                  to get correct settings if changed in printer dialog
wxPrinter* g_printer = 0;

// TODO (Tiwag#1#): 061012 Page Setup not implemented
// wxPageSetupData* g_pageSetupData = 0;

void InitPrinting()
{
    if (!g_printer)
    {
        g_printer = new wxPrinter;
        int paperid = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/printerdialog/paperid"), wxPAPER_A4 );
        int paperorientation  = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/printerdialog/paperorientation"), wxPORTRAIT );
        wxPrintData* ppd = &(g_printer->GetPrintDialogData().GetPrintData());
        ppd->SetPaperId((wxPaperSize)paperid);
        ppd->SetOrientation(paperorientation);
    }

//    if (!g_pageSetupData)
//        g_pageSetupData = new wxPageSetupDialogData;
}

void DeInitPrinting()
{
    delete g_printer;
    g_printer = 0;
//    delete g_pageSetupData;
//    g_pageSetupData = 0;
}
