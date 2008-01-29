/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSITEMEDITOR_H
#define WXSITEMEDITOR_H

#include "wxsitemres.h"
#include "../wxseditor.h"

#include <wx/hashset.h>
#include <wx/notebook.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>

class wxsItemEditorContent;
class wxsItemResData;
class wxsToolSpace;
class wxsItemInfo;
class wxsItem;

/** \brief This is root class for editing wxWidgets window resources
 */
class wxsItemEditor : public wxsEditor
{
	public:

        /** \brief Ctor */
		wxsItemEditor(wxWindow* parent,wxsItemRes* ItemRes);

        /** \brief Dctor */
		virtual ~wxsItemEditor();

		/** \brief Saving resource */
		virtual bool Save();

		/** \brief Notifying that configuration has been changed */
		static void ConfigChanged();

    protected:

		/** \brief Returns true if resource is modified, false otherwise */
		virtual bool GetModified() const;

		/** \brief Checking if can Undo */
		virtual bool CanUndo() const;

		/** \brief Ckecing if can Redo */
		virtual bool CanRedo() const;

		/** \brief Checking if we can cut */
		virtual bool HasSelection() const;

		/** \brief Checking if we can paste */
		virtual bool CanPaste() const;

        /** \brief testing if current resource is read-only */
        virtual bool IsReadOnly() const;

        /** \brief Undoing */
		virtual void Undo();

		/** \brief Redoing */
		virtual void Redo();

		/** \brief Cutting */
		virtual void Cut();

		/** \brief Copying */
		virtual void Copy();

		/** \brief Pasting */
		virtual void Paste();

    private:

        WX_DECLARE_HASH_SET(wxsItemEditor*,wxPointerHash,wxPointerEqual,WindowSet);

		/* Event handlers */
        void OnMouseClick(wxMouseEvent& event);
        void OnButton(wxCommandEvent& event);
        void OnInsPoint(wxCommandEvent& event);
        void OnInsInto(wxCommandEvent& event);
        void OnInsBefore(wxCommandEvent& event);
        void OnInsAfter(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);
        void OnQuickProps(wxCommandEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnPopup(wxCommandEvent& event);

        void InitializeVisualStuff();
        void InitializeResourceData();

        /** \brief Initializing editor images */
        static void InitializeImages();

        /** \brief Building palette */
        void BuildPalette(wxNotebook* Palette);

        inline wxsItemRes* GetItemRes() { return (wxsItemRes*)GetResource(); }
        inline const wxString& GetWxsFileName() { return GetItemRes()->GetWxsFileName(); }
        inline const wxString& GetSrcFileName() { return GetItemRes()->GetSrcFileName(); }
        inline const wxString& GetHdrFileName() { return GetItemRes()->GetHdrFileName(); }
        inline const wxString& GetXrcFileName() { return GetItemRes()->GetXrcFileName(); }

        /** \brief Function inserting new item */
        void InsertRequest(const wxString& Name);

        /** \brief Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);

        /** \brief Setting new insertion type */
        void SetInsertionType(int Type);

        /** \brief Rebuilding pictures on insertion type buttons */
        void RebuildInsTypeIcons();

        /** \brief Rebuilding picture on Quick Props button */
        void RebuildQuickPropsIcon();

        /** \brief Rebuilding all icons */
        void RebuildIcons();

        /** \brief Building icon for one button */
        void BuildInsTypeIcon(wxBitmapButton* Btn,const wxImage& Original,int ButtonType);

        /** \brief Opening or closiung Quick Props panel */
        void ToggleQuickPropsPanel(bool Open);

        /** \brief Refreshing content of Quick Props panel */
        void RebuildQuickProps(wxsItem* Selection);

        /** \brief Rebuilding preview (and updating selection inside preview) */
        void RebuildPreview();

        /** \brief Updating things related to current selection */
        void UpdateSelection();

        /** \brief Updating editor's title applying asterix before name of file when modified */
        void UpdateModified();

        /** \brief Getting item which will be used as reference item when adding new
         *         items
         */
        wxsItem* GetReferenceItem(int& InsertionType);

        /** \brief Starting sequence of adding new item when inserting by pointing with mouse */
        void StartInsertPointSequence(const wxsItemInfo* Info);

		/** \brief Reloading images in all editors */
		static void ReloadImages();

		/** \brief Causing editor's content to refresh */
		static void RefreshContents();

		/** \brief Showing popup menu */
		void ShowPopup(wxsItem* Item,wxMenu* Menu);

        wxsItemResData* m_Data;             ///< \brief Data managment object

        wxsItemEditorContent* m_Content;    ///< \brief Window with content area
        wxsToolSpace* m_ToolSpace;          ///< \brief Space for tools in resource
        wxNotebook* m_WidgetsSet;           ///< \brief Notebook with all widgets inside
        wxBoxSizer* m_VertSizer;            ///< \brief Root sizer of this editor
        wxBoxSizer* m_HorizSizer;           ///< \brief Horizontal sizer managing items below palette
        wxBoxSizer* m_QPSizer;              ///< \brief Sizer for quick properties
        wxBoxSizer* m_OpsSizer;             ///< \brief Sizer for operations pane
        wxScrolledWindow* m_QPArea;         ///< \brief Scrolled window containing all QuickProps sturr
        wxBitmapButton* m_InsPointBtn;
        wxBitmapButton* m_InsIntoBtn;
        wxBitmapButton* m_InsBeforeBtn;
        wxBitmapButton* m_InsAfterBtn;
        wxBitmapButton* m_DelBtn;
        wxBitmapButton* m_PreviewBtn;
        wxBitmapButton* m_QuickPanelBtn;
        wxWindow* m_TopPreview;             ///< \brief Top window of preview
        wxPanel* m_PreviewBackground;       ///< \brief Background panel used as background for peview
        int m_InsType;                      ///< \brief Current insertion type
        int m_InsTypeMask;                  ///< \brief Current insertion type mask
        bool m_QuickPropsOpen;              ///< \brief Set to true if quick properties panel is opened
        wxsItem* m_PopupCaller;             ///< \brief Item which requested popup to be shown

        static wxImage m_InsPointImg;
        static wxImage m_InsIntoImg;
        static wxImage m_InsBeforeImg;
        static wxImage m_InsAfterImg;
        static wxImage m_DelImg;
        static wxImage m_PreviewImg;
        static wxImage m_QuickPropsImgOpen;
        static wxImage m_QuickPropsImgClose;
        static wxImage m_SelectedImg;
        static WindowSet m_AllEditors;
        static bool m_ImagesLoaded;

        static const int itPoint  = 0x01;
        static const int itBefore = 0x02;
        static const int itAfter  = 0x04;
        static const int itInto   = 0x08;

        DECLARE_EVENT_TABLE()

        friend class wxsItemEditorContent;
        friend class wxsItemResData;
        friend class wxsItem;
};

#endif
