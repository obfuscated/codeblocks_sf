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

#ifndef WXSSIZER_H
#define WXSSIZER_H

#include "wxsparent.h"
#include "wxsflags.h"

#include <prep.h>

using namespace wxsFlags;

/** \brief Structure containing additional parameters for each widget insidee sizer */
class wxsSizerExtra: public wxsPropertyContainer
{
    public:
        long Proportion;                ///< \brief Proportion param (see wxW documentation for details)
        long Flags;                     ///< \brief Sizer item flags
        wxsDimensionData Border;        ///< \brief Size of additional border
        wxsSizeData MinSize;            ///< \brief Minimal size
//        wxSizeData Ratio;               ///< \brief Ratio

        wxsSizerExtra():
            Proportion(1),
            Flags(wxsSizerFlagsProperty::AlignCenterHorizontal|
                  wxsSizerFlagsProperty::AlignCenterVertical|
                  wxsSizerFlagsProperty::BorderTop|
                  wxsSizerFlagsProperty::BorderBottom|
                  wxsSizerFlagsProperty::BorderLeft|
                  wxsSizerFlagsProperty::BorderRight)
        {
            ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));
            Proportion         = cfg->ReadInt  ( _T("/defsizer/proportion"), Proportion );
            Flags              = cfg->ReadInt  ( _T("/defsizer/flags"),      Flags );
            Border.Value       = cfg->ReadInt  ( _T("/defsizer/border"),     5 );
            Border.DialogUnits = cfg->ReadBool ( _T("/defsizer/borderdu"),   false );
        }

        wxString AllParamsCode(wxsCoderContext* Ctx);

    protected:

        virtual void OnEnumProperties(long Flags);
};


/** \brief Base class for sizers
 *
 * \note sizers doesn't have identifier by default. But because of some
 *       extra information it requires, random identifier will be generated
 *       for it when editing in source mode.
 */
class wxsSizer: public wxsParent
{
    public:

        /** \brief Ctor */
        wxsSizer(wxsItemResData* Data,const wxsItemInfo* Info);

    protected:

        /** \brief Function generating sizer object used in preview
         *
         * Sizer created here will be used to generate previews. Adding items
         * into sizer are handled automatically. Binding sizer into container
         * must be done in container.
         */
        virtual wxSizer* OnBuildSizerPreview(wxWindow* Parent) = 0;

        /** \brief Function building code generating sizer
         *
         * This function must append code generating sizer to the end of Code
         * param. Adding items into sizer is handled automatically.
         */
        virtual void OnBuildSizerCreatingCode() = 0;

        /** \brief Function enumerating all properties for specified sizer only
         *
         * Create custom implementation of this function to add properties
         * which are not common for all sizers but are used in one
         * type of sizer only
         */
        virtual void OnEnumSizerProperties(long Flags) = 0;

        /** \brief Adding extra QPP panel for specified sizer only
         *
         * This function may be used to add some extra properties
         * to Quick Props panel. This should be used to properties
         * that are used in one sizer type only.
         */
        virtual void OnAddSizerQPP(cb_unused wxsAdvQPP* QPP) {}

    private:

        /** \brief Getting properties availability flags
         *
         * This function will be overridden here because
         * sizers doesn't use variable when using XRC mode
         */
        virtual long OnGetPropertiesFlags();

        /** \brief Function building code
         *
         * Code is created using BuindSizerCreatingCode function. It
         * automatically adds all children into sizer.
         */
        virtual void OnBuildCreatingCode();

        /** \brief Function generating declarations
         *
         * When using XRC file, sizers can not be declared
         * so this function does disable them.
         */
        virtual void OnBuildDeclarationsCode();


        /** \brief Function building preview
         *
         * Preview is generated using BuildSizerPreview function. All child
         * items are added and sizer is binded to parent object automatically.
         *
         * When there's no exact mode, there's additional panel on which
         * guidelines are drawn.
         */
         wxObject* OnBuildPreview(wxWindow* Parent,long Flags);

        /** \brief Function creating additional data
         *
         * There's additional data configuring widget inside sizer - it does
         * define placement, borders and some other flags.
         */
        virtual wxsPropertyContainer* OnBuildExtra();

        /** \brief Function adding additional QPP child panel */
        virtual void OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP);

        /** \brief Custom child loading function - needed to support Spacer exception */
        virtual bool OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Custom child writing function - needed to support Spacer exception */
        virtual bool OnXmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra);

        /** \brief Name of extra object node will be returned here */
        virtual wxString OnXmlGetExtraObjectClass();

        /** \brief Function enumerating properties for sizers*/
        virtual void OnEnumItemProperties(long Flags);

        /** \brief Adding generic sizer properties to QPP */
        virtual void OnAddItemQPP(wxsAdvQPP* QPP);

};

#endif
