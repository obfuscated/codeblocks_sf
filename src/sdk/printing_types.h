#ifndef PRINTING_TYPES_H
#define PRINTING_TYPES_H

#include "settings.h"
#include <wx/print.h>

// Global print data, to remember settings during the session
extern DLLIMPORT wxPrintData* g_printData;

// Global page setup data
extern DLLIMPORT wxPageSetupData* g_pageSetupData;

// printing scope for print dialog
enum PrintScope
{
    psSelection,
    psActiveEditor,
    psAllOpenEditors
};

// printing colour mode
enum PrintColourMode
{
    pcmBlackAndWhite,
    pcmColourOnWhite,
    pcmInvertColours,
    pcmAsIs
};

#ifdef __cplusplus
extern "C" {
#endif
extern DLLIMPORT void InitPrinting();
extern DLLIMPORT void DeInitPrinting();
#ifdef __cplusplus
}
#endif

#endif // PRINTING_TYPES_H
