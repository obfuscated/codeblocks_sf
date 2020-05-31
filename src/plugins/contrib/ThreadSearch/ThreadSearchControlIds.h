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
    enum IDs : long
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
        idOptionResetAll,
        idMenuViewThreadSearch,
        idMenuViewFocusThreadSearch,
        idMenuSearchThreadSearch,
        idMenuCtxThreadSearch,
        idMenuCtxDeleteItem,
        idMenuCtxDeleteAllItems,
        idMenuCtxCopy,
        idMenuCtxCopySelection,
        idMenuCtxCollapseFile,
        idMenuCtxCollapseSearch,
        idMenuCtxCollapseAll,
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
        idConfPanelColorPicker0,
        idConfPanelColorPicker1,
        idConfPanelColorPicker2,
        idConfPanelColorPicker3,
        idConfPanelColorPicker4,
        idConfPanelColorPicker5,
        idConfPanelColorPicker6,
        idConfPanelColorPicker7,
        idConfPanelColorPicker8,
        idDirDialogCombo,
        idDirDialogList,
        idDirDialogDirButton,
        idDirDialogAddButton,
        idDirDialogEditButton,
        idDirDialogDeleteButton,
        idDirDialogDeleteAllButton,
        idDirDialogCheckSelectedButton,

        lastValue
    };

    long Get(IDs id);
private:
    long ids[lastValue];
    bool initialized;
};

extern ControlIDs controlIDs;

#endif // THREAD_SEARCH_CONTROL_IDS_H
