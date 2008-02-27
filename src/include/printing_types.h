/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PRINTING_TYPES_H
#define PRINTING_TYPES_H

#include "settings.h"
#include <wx/print.h>

// Global printer
extern wxPrinter* g_printer;

// Global page setup data
extern wxPageSetupData* g_pageSetupData;

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
