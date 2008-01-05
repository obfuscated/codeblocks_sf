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

enum eConrolIds
{
	idBtnDirSelectClick = wxID_HIGHEST,
	idBtnShowDirItemsClick,
	idBtnSearch,
	idBtnOptions,
	idMenuViewThreadSearch,
	idMenuSearchThreadSearch,
	idMenuCtxThreadSearch,
	idCboSearchExpr,
	idChkSearchDirRecurse,
	idChkSearchDirHidden,
	idChkSearchOpenFiles,
	idChkSearchProjectFiles,
	idChkSearchWorkspaceFiles,
	idChkSearchDirectoryFiles,
	idChkWholeWord,
	idChkStartWord,
	idChkMatchCase,
	idChkRegularExpression,
	idChkUseDefaultOptionsOnThreadSearch,
	idChkDisplayLogHeaders,
	idChkDrawLogLines,
	idChkViewThreadSearchToolBar,
	idChkShowCodePreview,
	idChkShowThreadSearchWidgets,
	idChkThreadSearchEnable,
	idTxtSearchDirPath,
	idTxtSearchMask,
	idTmrListCtrlUpdate,
	idWndLogger
};

#endif // THREAD_SEARCH_CONTROL_IDS_H
