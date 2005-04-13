#include "printing_types.h"

wxPrintData* g_printData = 0;
wxPageSetupData* g_pageSetupData = 0;

void InitPrinting()
{
    if (!g_printData)
        g_printData = new wxPrintData;
    if (!g_pageSetupData)
        g_pageSetupData = new wxPageSetupDialogData;
}

void DeInitPrinting()
{
    delete g_pageSetupData;
    g_pageSetupData = 0;
    
    delete g_printData;
    g_printData = 0;
}
