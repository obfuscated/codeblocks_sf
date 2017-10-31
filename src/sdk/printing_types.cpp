/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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
wxPrinter* g_printer = nullptr;

// TODO (Tiwag#1#): 061012 Page Setup not implemented
// wxPageSetupData* g_pageSetupData = 0;

void InitPrinting()
{
    if (!g_printer)
    {
        g_printer = new wxPrinter;
        int paperid = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/printerdialog/paperid"), wxPAPER_A4 );
        #if wxCHECK_VERSION(3, 0, 0)
        wxPrintOrientation paperorientation  = static_cast<wxPrintOrientation>( Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/printerdialog/paperorientation"), wxPORTRAIT ) );
        #else
        int paperorientation  = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/printerdialog/paperorientation"), wxPORTRAIT );
        #endif
        wxPrintData* ppd = &(g_printer->GetPrintDialogData().GetPrintData());
        ppd->SetPaperId((wxPaperSize)paperid);
        #if wxCHECK_VERSION(3, 0, 0)
        if (paperorientation == wxPORTRAIT)
            ppd->SetOrientation(wxPORTRAIT);
        else
            ppd->SetOrientation(wxLANDSCAPE);
        #else
        ppd->SetOrientation(paperorientation);
        #endif
    }

//    if (!g_pageSetupData)
//        g_pageSetupData = new wxPageSetupDialogData;
}

void DeInitPrinting()
{
    delete g_printer;
    g_printer = nullptr;
//    delete g_pageSetupData;
//    g_pageSetupData = 0;
}
