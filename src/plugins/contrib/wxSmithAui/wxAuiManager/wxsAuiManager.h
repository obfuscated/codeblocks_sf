/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WXSAUIMANAGER_H
#define WXSAUIMANAGER_H

#include "wxsAuiPaneInfoProperty.h"

#include <wxwidgets/wxsparent.h>
#include <wxwidgets/wxsflags.h>
#include "../wxAuiToolBar/wxsAuiToolBar.h"
#include "wxSmithAuiManager.h"

using namespace wxsFlags;

/** \brief Structure containing additional parameters for each widget (pane) inside AuiManager */
class wxsAuiPaneInfoExtra: public wxsPropertyContainer
{
    public:

        // Standard pane types
        static const long DefaultPane = 1;
        static const long CenterPane  = 2;
        static const long ToolbarPane = 3;

        // Properties
        wxString m_Name;
        long m_StandardPane;
        wxString m_Caption;
        bool m_CaptionVisible;
        bool m_MinimizeButton;
        bool m_MaximizeButton;
        bool m_PinButton;
        bool m_CloseButton;
        long m_Layer;
        long m_Row;
        long m_Position;
        bool m_Docked;
        long m_DockDirection;
        bool m_DockFixed;
        long m_DockableFlags;
        bool m_Floatable;
        wxsPositionData m_FloatingPosition;
        wxsSizeData m_FloatingSize;
        bool m_Resizable;
        bool m_Movable;
        long m_Gripper;
        bool m_PaneBorder;
        bool m_Visible;
        bool m_DestroyOnClose;
        bool m_FirstAdd;        //Allow changing properties when adding the item in the editor and
                                //being unchanged when the item is added because the .wxs file is open

        wxsAuiPaneInfoExtra()
        :
            m_Name(_("PaneName")),
            m_StandardPane(0),
            m_Caption(_("Pane caption")),
            m_CaptionVisible(true),
            m_MinimizeButton(false),
            m_MaximizeButton(false),
            m_PinButton(false),
            m_CloseButton(true),
            m_Layer(0),
            m_Row(0),
            m_Position(0),
            m_Docked(true),
            m_DockDirection(wxAUI_DOCK_LEFT),
            m_DockFixed(false),
            m_DockableFlags(wxALL),
            m_Floatable(true),
            m_Resizable(true),
            m_Movable(true),
            m_Gripper(0),
            m_PaneBorder(true),
            m_Visible(true),
            m_DestroyOnClose(false),
            m_FirstAdd(true),
            m_LastDockDirection(0)
        {
        }

        wxString AllParamsCode(wxsCoderContext* Ctx,wxsItem* ChildPane);
        wxAuiPaneInfo GetPaneInfoFlags(wxWindow* Parent,wxsItem* ChildPane,bool Exact);

    protected:

        virtual void OnEnumProperties(long Flags);

        /** \brief Function notifying that some property has changed. */
        virtual void OnPropertyChanged();

    private:

        long m_LastDockDirection;
        long m_LastStandardPane;
};


/** \brief Class for wxAuiManager
 */
class wxsAuiManager: public wxsParent
{
    public:

        /** \brief Ctor */
        wxsAuiManager(wxsItemResData* Data);

    protected:

        /** \brief Function building code generating AuiManager
         *
         * This function must append code generating AuiManager to the end of Code
         * param. Adding items into wxAuiManager is handled automatically.
         */
        virtual void OnBuildAuiManagerCreatingCode();

        /** \brief Adding extra QPP panel for AuiManager
         *
         * This function may be used to add some extra properties
         * to Quick Props panel. This should be used to properties
         * that are used in AuiManager.
         */
        virtual void OnAddAuiMangagerQPP(wxsAdvQPP* /*QPP*/) {}

    private:

        /** \brief Function building code
         *
         * Code is created using BuildAuiManagerCreatingCode function. It
         * automatically adds all children into auimanager.
         */
        virtual void OnBuildCreatingCode();

        /** \brief Function building preview
         *
         * Preview is generated using BuildAuiManagerPreview function. All child
         * items are added and wxAuiManager is binded to parent object automatically.
         *
         * When there's no exact mode, there's additional panel on which
         * guidelines are drawn.
         */
         wxObject* OnBuildPreview(wxWindow* Parent,long Flags);

        /** \brief Funciton creating additional data
         *
         * There's additional data configuring widget inside AuiManager - it does
         * define dock direction and some other flags.
         */
        virtual wxsPropertyContainer* OnBuildExtra();

        /** \brief Function check if item can be add */
        virtual bool OnCanAddChild(wxsItem* Item,bool ShowMessage);

        /** \brief Function check if wxAuiManager can be added to Parent */
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);

        /** \brief Function adding additional QPP child panel */
        virtual void OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);

        /** \brief Name of extra object node will be returned here */
        virtual wxString OnXmlGetExtraObjectClass();

        /** \brief Function enumerating properties for AuiManager */
        virtual void OnEnumItemProperties(long Flags);

        /** \brief Adding generic AuiManager properties to QPP */
        virtual void OnAddItemQPP(wxsAdvQPP* QPP);

        long StyleFlags;

        void OnClose(const wxCloseEvent & /*event*/) { wxMessageBox(_("AuiManager Closed"), _("Debug Me"), wxICON_INFORMATION); /*UnInit();*/ }
};

#endif
