/***************************************************************
 * Name:      ThreadSearchConrolIds
 *
 * Purpose:   This file lists all ThreadSearch plugin control ids
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#ifndef THREAD_SEARCH_CONTROL_IDS_H
#define THREAD_SEARCH_CONTROL_IDS_H

#include <wx/defs.h>


struct ControlIDs
{
    enum IDs
    {

        idBtnDirSelectClick = 0,
        idBtnShowDirItemsClick,
        idBtnSearch,
        idBtnOptions,
        idOptionDialog,
        idOptionWholeWord,
        idOptionStartWord,
        idOptionMatchCase,
        idOptionRegEx,
        idMenuViewThreadSearch,
        idMenuViewFocusThreadSearch,
        idMenuSearchThreadSearch,
        idMenuCtxThreadSearch,
        idMenuCtxDeleteItem,
        idMenuCtxDeleteAllItems,
        idCboSearchExpr,
        idChkSearchDirRecurse,
        idChkSearchDirHidden,
        idBtnSearchOpenFiles,
        idBtnSearchTargetFiles,
        idBtnSearchProjectFiles,
        idBtnSearchWorkspaceFiles,
        idBtnSearchDirectoryFiles,
        idChkWholeWord,
        idChkStartWord,
        idChkMatchCase,
        idChkRegularExpression,
        idChkUseDefaultOptionsOnThreadSearch,
        idChkDisplayLogHeaders,
        idChkDrawLogLines,
        idChkAutosizeLogColumns,
        idChkViewThreadSearchToolBar,
        idChkShowCodePreview,
        idChkShowThreadSearchWidgets,
        idChkThreadSearchEnable,
        idSearchDirPath,
        idSearchMask,
        idTmrListCtrlUpdate,
        idWndLogger,
        idChkShowMissingFilesError,
        idChkShowCantOpenFileError,
        idChkChkDeletePreviousResults,

        lastValue
    };

    long Get(IDs id);
private:
    long ids[lastValue];
    bool initialized;
};

extern ControlIDs controlIDs;

#endif // THREAD_SEARCH_CONTROL_IDS_H
