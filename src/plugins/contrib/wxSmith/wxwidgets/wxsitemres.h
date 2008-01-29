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

#ifndef WXSITEMRES_H
#define WXSITEMRES_H

#include "wxwidgetsres.h"

class wxsItemEditor;
class wxsItemResData;

/** \brief Some abstract interface allowing wxsItemResData to access some resource-specific functinos easily */
class wxsItemResFunctions
{
    public:

        /** \brief Ctor */
        wxsItemResFunctions() {};

        /** \brief Dctor */
        virtual ~wxsItemResFunctions() {};

        /** \brief Generating exact preview used in editor after pressing preview button */
        virtual wxWindow* OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data) = 0;
};

/** \brief Base class for resources using item as root element
 *
 * This class implements most of functions in wxsResource and wxWidgetsRes,
 * only need to write this function: \code virtual wxString OnGetAppBuildingCode() \endcode
 */
class wxsItemRes: public wxWidgetsRes, public wxsItemResFunctions
{
    DECLARE_CLASS(wxsItemRes)
    public:

        /** \brief Available edit modes for item resources */
        enum EditMode { File, Source, Mixed };

        /** \brief Structure containing all arguments required when creating new resource */
        struct NewResourceParams
        {
            enum Scope
            {
                Public,
                Protected,
                Private
            };

            wxString Class;
            wxString Src;
            wxString Hdr;
            wxString Xrc;
            wxString Pch;
            wxString Wxs;
            wxString InitFunc;
            wxString BaseClass;
            wxString CustomCtorArgs;
            wxString PchGuard;
            bool GenSrc;
            bool GenHdr;
            bool GenXrc;
            bool UsePch;
            bool UseInitFunc;
            bool CtorParent;
            bool CtorParentDef;
            bool CtorId;
            bool CtorIdDef;
            bool CtorPos;
            bool CtorPosDef;
            bool CtorSize;
            bool CtorSizeDef;
            bool UseFwdDecl;
            Scope ScopeIds;
            Scope ScopeMembers;
            Scope ScopeHandlers;

            NewResourceParams():
                GenSrc(false), GenHdr(false), GenXrc(false), UsePch(false),
                UseInitFunc(false), CtorParent(false), CtorParentDef(false),
                CtorId(false), CtorIdDef(false), CtorPos(false), CtorPosDef(false),
                CtorSize(false), CtorSizeDef(false), UseFwdDecl(false),
                ScopeIds(Protected), ScopeMembers(Public), ScopeHandlers(Private)
            {}
        };

        /** \brief Ctor */
        wxsItemRes(wxsProject* Owner,const wxString& ResourceType,bool CanBeMain);

        /** \brief Ctor for external resource
         *  \param FileName name of XRC file
         *  \param Object Xml node with XRC resource
         */
        wxsItemRes(const wxString& FileName,const TiXmlElement* XrcElem,const wxString& ResourceType);

        /** \brief Dctor */
        virtual ~wxsItemRes();

        /** \brief Creating new resource and building files if necessarry */
        virtual bool CreateNewResource(NewResourceParams& Params);

        inline const wxString& GetWxsFileName() { return m_WxsFileName; }
        inline const wxString& GetSrcFileName() { return m_SrcFileName; }
        inline const wxString& GetHdrFileName() { return m_HdrFileName; }
        inline const wxString& GetXrcFileName() { return m_XrcFileName; }

        /** \brief Getting current edit mode */
        EditMode GetEditMode();

        /** \brief Building data object for this resource */
        wxsItemResData* BuildResData(wxsItemEditor* Editor);

    protected:

        virtual wxsEditor* OnCreateEditor(wxWindow* Parent);
        virtual bool OnReadConfig(const TiXmlElement* Node);
        virtual bool OnWriteConfig(TiXmlElement* Node);
        virtual bool OnCanHandleFile(const wxString& FileName);
        virtual wxString OnGetDeclarationFile();
        virtual bool OnGetUsingXRC();
        virtual bool OnGetCanBeMain();
        virtual void OnFillPopupMenu(wxMenu* Menu);
        virtual bool OnPopupMenu(long Id);
        virtual bool OnDeleteCleanup(bool ShowDialog);

    private:

        virtual int OnGetTreeIcon();

        wxString m_WxsFileName;
        wxString m_SrcFileName;
        wxString m_HdrFileName;
        wxString m_XrcFileName;
        bool     m_UseForwardDeclarations;
        bool     m_CanBeMain;
};

#endif
