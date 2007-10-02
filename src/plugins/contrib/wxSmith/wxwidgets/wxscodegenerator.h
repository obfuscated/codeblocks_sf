/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSCODEGENERATOR_H
#define WXSCODEGENERATOR_H

#include "wxscodercontext.h"

/** \brief Base class for items which generate source code */
class wxsCodeGenerator
{
    public:

        /** \brief Ctor */
        wxsCodeGenerator();

        /** \brief Dctor */
        virtual ~wxsCodeGenerator();

        /** \brief Request to build the code */
        void BuildCode(wxsCoderContext* Context);

    protected:

        virtual void OnBuildCreatingCode() = 0;         ///< \brief Building code manually creating resource, it may also be used to add header files
        virtual void OnBuildHeadersCode() {}            ///< \brief Building headers arrays, it's not required since headers can be added in OnBuildCreatingCode()
        virtual void OnBuildDeclarationsCode() = 0;     ///< \brief Building variable declaration / forward declaration
        virtual void OnBuildEventsConnectingCode() = 0; ///< \brief Building code connecting events
        virtual void OnBuildIdCode() = 0;               ///< \brief Building code declaring class members and values of identifiers
        virtual void OnBuildXRCFetchingCode() = 0;      ///< \brief Building code which fetches this item from xrc resource
        virtual void OnUpdateFlags(long& Flags) {}      ///< \brief Function called to update context flags just before the code is generated, previous flags are restored after code is generated

        /** \brief Getting current coding context */
        inline wxsCoderContext* GetCoderContext() { return m_Context; }

        /** \brief Getting current language */
        inline wxsCodingLang GetLanguage() { return m_Context ? m_Context->m_Language : wxsUnknownLanguage; }

        /** \brief Getting flags of current coding context */
        inline long GetCoderFlags() { return m_Context ? m_Context->m_Flags : 0; }

        /** \brief Adding header file into current context */
        inline void AddHeader(const wxString& Header,const wxString& DeclaredClass,long HeaderFlags=0)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddHeader(Header,DeclaredClass,HeaderFlags);
        }

        /*** \brief Adding forward declaration of item's class */
        inline void AddDeclaration(const wxString& Declaration)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddDeclaration(Declaration);
        }

        /** \brief Adding XRC fetching code */
        inline void AddXRCFetchingCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddXRCFetchingCode(Code);
        }

        /** \brief Adding Manually building code */
        inline void AddBuildingCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddBuildingCode(Code);
        }

        /** \brief Adding event connecting code */
        inline void AddEventCode(const wxString& Code)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddEventCode(Code);
        }

        /** \brief Adding id-generating code */
        inline void AddIdCode(const wxString& Enumeration,const wxString& Initialization)
        {
            if ( GetCoderContext() ) GetCoderContext()->AddIdCode(Enumeration,Initialization);
        }

    private:

        wxsCoderContext* m_Context;
};

#endif
