/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef AUI_COMPAT_H
#define AUI_COMPAT_H

#if !wxCHECK_VERSION(2,8,0)
	#include "wxAUI/manager.h"

	#define wxAUI_DOCKART_PANE_BORDER_SIZE 					wxAUI_ART_PANE_BORDER_SIZE
	#define wxAUI_DOCKART_SASH_SIZE 						wxAUI_ART_SASH_SIZE
	#define wxAUI_DOCKART_CAPTION_SIZE 						wxAUI_ART_CAPTION_SIZE
	#define wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR 			wxAUI_ART_ACTIVE_CAPTION_COLOUR
	#define wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR 	wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR
	#define wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR 		wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR
	#define wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR 			wxAUI_ART_INACTIVE_CAPTION_COLOUR
	#define wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR	wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR
	#define wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR 		wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR

	typedef wxFrameManager wxAuiManager;
	typedef wxPaneInfo wxAuiPaneInfo;
	typedef wxPaneInfoArray wxAuiPaneInfoArray;
	typedef wxDockArt wxAuiDockArt;
#else
	#include <wx/aui/aui.h>
#endif


#endif // AUI_COMPAT_H
