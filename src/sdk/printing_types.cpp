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
