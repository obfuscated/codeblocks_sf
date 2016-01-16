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

#include "wxsitemresdata.h"
#include "wxsparent.h"
#include "wxsitemfactory.h"
#include "wxsitemeditor.h"
#include "wxstool.h"
#include "wxsflags.h"
#include "../wxscoder.h"

#include <globals.h>
#include <logmanager.h>
#include <wx/clipbrd.h>
#include <tinyxml/tinywxuni.h>

#if defined(__WXMSW__) && defined(LoadImage)
    // Fix Windows winuser.h Header define of LoadImage.
    #undef LoadImage
#endif


using namespace wxsFlags;

namespace
{
    const int ToolsTreeImageId = wxsResourceTree::LoadImage(_T("/images/misc_16x16.png"));
}

wxsItemResData::wxsItemResData(
    const wxString& WxsFileName,
    const wxString& SrcFileName,
    const wxString& HdrFileName,
    const wxString& XrcFileName,
    const wxString& ClassName,
    const wxString& ClassType,
    wxsCodingLang Language,
    bool UseForwardDeclarations,
    bool WithTranslation,
    wxsResourceItemId TreeId,
    wxsItemEditor* Editor,
    wxsItemResFunctions* Functions):
        m_WxsFileName(WxsFileName),
        m_SrcFileName(SrcFileName),
        m_HdrFileName(HdrFileName),
        m_XrcFileName(XrcFileName),
        m_Translation(WithTranslation),
        m_ClassName(ClassName),
        m_ClassType(ClassType),
        m_Language(Language),
        m_TreeId(TreeId),
        m_ToolsId(),
        m_ToolsNodeIsExpanded(false),
        m_Editor(Editor),
        m_Functions(Functions),
        m_RootItem(0),
        m_RootSelection(0),
        m_Preview(0),
        m_Corrector(this),
        m_IsOK(false),
        m_LockCount(0),
        m_ReadOnly(false)
{
    if (  WxsFileName.empty() &&
          SrcFileName.empty() &&
          HdrFileName.empty() &&
         !XrcFileName.empty() )
    {
        m_PropertiesFilter = flFile;
    }
    else if ( !WxsFileName.empty() &&
              !SrcFileName.empty() &&
              !HdrFileName.empty() )
    {
        if ( XrcFileName.empty() )
        {
            m_PropertiesFilter = flSource;
        }
        else
        {
            m_PropertiesFilter = flMixed;
        }
    }
    else
    {
        m_PropertiesFilter = 0;
    }

    DetectAutoCodeBlocks();
    if ( UseForwardDeclarations )
    {
        m_PropertiesFilter |= flFwdDeclar;
    }

    Load();

    if ( !m_RootItem )
    {
        RecreateRootItem();
        m_IsOK = false;
    }
}

wxsItemResData::~wxsItemResData()
{
    HidePreview();

    if ( GetModified() )
    {
        // Restoring previous content of files
        // kept up-to-date
        SilentLoad();
        RebuildFiles();
    }
    delete m_RootItem;
    m_RootItem = 0;
    m_RootSelection = 0;
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        delete m_Tools[i];
    }
    m_Tools.clear();
    m_PropertiesFilter = 0;
    if ( m_Editor && wxsResourceTree::Get() )
    {
        wxsResourceItemId ParentId = wxsResourceTree::Get()->GetItemParent(m_TreeId);
        // Selecting parent to prevent reopening resource on wxGTK
        wxsResourceTree::Get()->SelectItem(ParentId);
        wxsResourceTree::Get()->DeleteChildren(m_TreeId);
    }
}

bool wxsItemResData::Load()
{
    bool Ret = SilentLoad();
    if ( !m_RootItem )
    {
        RecreateRootItem();
        if ( !m_RootItem )
        {
            return false;
        }
    }
    if ( !m_Corrector.GlobalCheck() )
    {
        // TODO: Some notification here ? (May be not a good idea, maybe three-vale return should be better)
    }
    StoreUndo();
    m_Undo.Saved();
    if ( m_Editor )
    {
        m_Editor->UpdateModified();
    }
    if ( Ret && !(m_PropertiesFilter&flFile) )
    {
        RebuildSourceCode();        // Yop, only source recreated, xrc if used not touched
    }
    RebuildTree();
    if ( m_Editor )
    {
        m_Editor->RebuildPreview();
    }
    SelectItem(m_RootItem,true);

    return Ret;
}

bool wxsItemResData::SilentLoad()
{
    switch ( m_PropertiesFilter & (flFile|flMixed|flSource) )
    {
        case flFile:
            m_IsOK = LoadInFileMode();
            break;

        case flMixed:
            m_IsOK = LoadInMixedMode();
            break;

        case flSource:
            m_IsOK = LoadInSourceMode();
            break;

        default:
            m_IsOK = false;
    }
    return m_IsOK;
}

bool wxsItemResData::LoadInFileMode()
{
    TiXmlDocument Doc;
    if ( !TinyXML::LoadDocument(m_XrcFileName,&Doc) ) return false;

    TiXmlElement* Resource = Doc.FirstChildElement("resource");
    if ( !Resource ) return false;

    TiXmlElement* Object = Resource->FirstChildElement("object");
    while ( Object )
    {
        if ( cbC2U(Object->Attribute("name")) == m_ClassName ) break;
        Object = Object->NextSiblingElement("object");
    }

    if ( !Object ) return false;
    if ( cbC2U(Object->Attribute("class")) != m_ClassType ) return false;

    RecreateRootItem();
    if ( !m_RootItem ) return false;
    m_RootItem->XmlRead(Object,true,false);
    LoadToolsReq(Object,true,false);

    return true;
}

bool wxsItemResData::LoadInMixedMode()
{
    // TODO: Check if source / header files have required blocks of code

    TiXmlDocument DocExtra;
    if ( !TinyXML::LoadDocument(m_WxsFileName,&DocExtra) ) return false;
    TiXmlDocument DocXrc;
    if ( !TinyXML::LoadDocument(m_XrcFileName,&DocXrc) ) return false;

    // Loading XRC data from Xrc file

    TiXmlElement* Resource = DocXrc.FirstChildElement("resource");
    if ( !Resource ) return false;

    TiXmlElement* Object = Resource->FirstChildElement("object");
    while ( Object )
    {
        if ( cbC2U(Object->Attribute("name")) == m_ClassName ) break;
        Object = Object->NextSiblingElement("object");
    }

    if ( !Object )
    {
        // We don't thread this as error since it may be new
        // resource not yet being updated
        return true;
    }
    if ( cbC2U(Object->Attribute("class")) != m_ClassType ) return false;

    RecreateRootItem();
    if ( !m_RootItem ) return false;
    m_RootItem->XmlRead(Object,true,false);
    LoadToolsReq(Object,true,false);

    // Loading extra data from wxs file

    TiXmlElement* wxSmithNode = DocExtra.FirstChildElement("wxsmith");
    if ( wxSmithNode )
    {
        TiXmlElement* Extra = wxSmithNode->FirstChildElement("resource_extra");
        if ( Extra )
        {
            IdToXmlMapT IdToXmlMap;

            TiXmlElement* Object = Extra->FirstChildElement("object");
            while ( Object )
            {
                wxString IdName = cbC2U(Object->Attribute("name"));
                if ( !IdName.empty() )
                {
                    IdToXmlMap[IdName] = Object;
                }
                else if ( Object->Attribute("root") )
                {
                    // Empty id simulates root node
                    IdToXmlMap[_T("")] = Object;
                }
                Object = Object->NextSiblingElement("object");
            }

            UpdateExtraDataReq(m_RootItem,IdToXmlMap);
            for ( int i=0; i<GetToolsCount(); i++ )
            {
                UpdateExtraDataReq(m_Tools[i],IdToXmlMap);
            }
        }
    }

    return true;
}

void wxsItemResData::UpdateExtraDataReq(wxsItem* Item,IdToXmlMapT& Map)
{
    if ( Item->GetPropertiesFlags() & flId )
    {
        wxString Id = Item->GetIdName();
        if ( !Id.empty() )
        {
            if ( !Item->GetParent() )
            {
                // Empty id simlates root node
                Id = _T("");
            }
            if ( Map.find(Id) != Map.end() )
            {
                TiXmlElement* Object = Map[Id];
                if ( cbC2U(Object->Attribute("class"))==Item->GetClassName() )
                {
                    Item->XmlRead(Object,false,true);
                }
            }
        }
    }

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            UpdateExtraDataReq(AsParent->GetChild(i),Map);
        }
    }
}

bool wxsItemResData::LoadInSourceMode()
{
    // TODO: Check if source / header files have required blocks of code

    TiXmlDocument Doc;
    if ( !TinyXML::LoadDocument(m_WxsFileName,&Doc)  )
    {
        Manager::Get()->GetLogManager()->DebugLog(F(_T("wxSmith: Error loading wxs file (Col: %d, Row:%d): ") + cbC2U(Doc.ErrorDesc()),Doc.ErrorCol(),Doc.ErrorRow()));
        return false;
    }

    TiXmlElement* wxSmithNode = Doc.FirstChildElement("wxsmith");
    if ( !wxSmithNode ) return false;

    TiXmlElement* Object = wxSmithNode->FirstChildElement("object");
    if ( !Object ) return false;

    /*
    if ( cbC2U(Object->Attribute("name")) != m_ClassName ) return false;
    if ( cbC2U(Object->Attribute("class")) != m_ClassType ) return false;
    */

    RecreateRootItem();
    if ( !m_RootItem ) return false;
    m_RootItem->XmlRead(Object,true,true);
    LoadToolsReq(Object,true,true);

    return true;
}

void wxsItemResData::RecreateRootItem()
{
    delete m_RootItem;
    m_RootItem = wxsItemFactory::Build(m_ClassType,this);
}

void wxsItemResData::LoadToolsReq(TiXmlElement* Node,bool IsXRC,bool IsExtra)
{
    for ( TiXmlElement* Object = Node->FirstChildElement("object"); Object; Object = Object->NextSiblingElement("object") )
    {
        wxString Class = cbC2U(Object->Attribute("class"));
        if ( Class.IsEmpty() ) continue;
        const wxsItemInfo* Info = wxsItemFactory::GetInfo(Class);
        if ( !Info ) continue;
        if ( Info->Type != wxsTTool )
        {
            LoadToolsReq(Object,IsXRC,IsExtra);
            continue;
        }
        if ( !(GetPropertiesFilter()&flSource) && !Info->AllowInXRC ) continue;
        wxsItem* Item = wxsItemFactory::Build(Class,this);
        if ( !Item ) continue;
        wxsTool* Tool = Item->ConvertToTool();
        if ( !Tool )
        {
            delete Item;
            continue;
        }
        if ( InsertNewTool(Tool) )
        {
            Tool->XmlRead(Object,IsXRC,IsExtra);
        }
    }
}

bool wxsItemResData::Save()
{
    m_IsOK = true;
    switch ( m_PropertiesFilter & (flFile|flMixed|flSource) )
    {
        case flFile:
            return SaveInFileMode();

        case flMixed:
            return SaveInMixedMode();

        case flSource:
            return SaveInSourceMode();
    }

    return false;
}

bool wxsItemResData::SaveInFileMode()
{
    // Using routine recreating XRC in mixed mode
    if ( RebuildXrcFile() )
    {
        m_Undo.Saved();
        return true;
    }
    return false;
}

bool wxsItemResData::SaveInMixedMode()
{
    // Should be currently up to date, but just for sure udpating it once again
    if ( !RebuildXrcFile() ) return false;

    // Storing extra data into Wxs file

    TiXmlDocument Doc;

    Doc.InsertEndChild(TiXmlDeclaration("1.0","utf-8",""));
    TiXmlElement* wxSmithNode = Doc.InsertEndChild(TiXmlElement("wxsmith"))->ToElement();

    // Now storing all extra data
    TiXmlElement* Extra = wxSmithNode->InsertEndChild(TiXmlElement("resource_extra"))->ToElement();
    SaveExtraDataReq(m_RootItem,Extra);
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        SaveExtraDataReq(m_Tools[i],Extra);
    }

    if ( TinyXML::SaveDocument(m_WxsFileName,&Doc) )
    {
        m_Undo.Saved();
        return true;
    }
    return false;
}

void wxsItemResData::SaveExtraDataReq(wxsItem* Item,TiXmlElement* Node)
{
    if ( Item->GetPropertiesFlags() && flId )
    {
        wxString Id = Item->GetIdName();
        if ( !Id.empty() )
        {
            TiXmlElement* Object = Node->InsertEndChild(TiXmlElement("object"))->ToElement();
            if ( Item!=m_RootItem )
            {
                Object->SetAttribute("name",cbU2C(Id));
                Object->SetAttribute("class",cbU2C(Item->GetClassName()));
            }
            else
            {
                // TOP-level items have different attributes in extra node
                Object->SetAttribute("root","1");
            }
            Item->XmlWrite(Object,false,true);
        }
    }

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            SaveExtraDataReq(AsParent->GetChild(i),Node);
        }
    }
}

bool wxsItemResData::SaveInSourceMode()
{
    TiXmlDocument Doc;

    Doc.InsertEndChild(TiXmlDeclaration("1.0","utf-8",""));
    TiXmlElement* wxSmithNode = Doc.InsertEndChild(TiXmlElement("wxsmith"))->ToElement();

    // Storing xml data
    TiXmlElement* Object = wxSmithNode->InsertEndChild(TiXmlElement("object"))->ToElement();
    m_RootItem->XmlWrite(Object,true,true);
    Object->SetAttribute("name",cbU2C(m_ClassName));
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        TiXmlElement* ToolElement = Object->InsertEndChild(TiXmlElement("object"))->ToElement();
        m_Tools[i]->XmlWrite(ToolElement,true,true);
    }

    if ( TinyXML::SaveDocument(m_WxsFileName,&Doc) )
    {
        m_Undo.Saved();
        return true;
    }
    return false;
}

void wxsItemResData::RebuildFiles()
{
    switch ( m_PropertiesFilter & (flSource|flMixed|flSource) )
    {
        case flSource:
            RebuildSourceCode();
            break;

        case flMixed:
            RebuildSourceCode();
            RebuildXrcFile();
            break;
    }
}

void wxsItemResData::DetectAutoCodeBlocks()
{
    // PCH filter is present when we can find HeadersPCH() and InternalHeadersPCH() code blocks

    do
    {
        if ( wxsCoder::Get()->GetCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("InternalHeadersPCH"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                true,false).IsEmpty() )
        {
            break;
        }

        if ( wxsCoder::Get()->GetCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("HeadersPCH"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                true,false).IsEmpty() )
        {
            break;
        }

        m_PropertiesFilter |= flPchFilter;
    }
    while ( false );

    // Check if we should clear event table
    m_IsEventTable = !wxsCoder::Get()->GetCode(
        m_HdrFileName,
        wxsCodeMarks::Beg(wxsCPP,_T("EventTable"),m_ClassName),
        wxsCodeMarks::End(wxsCPP),
        true,false).IsEmpty();
}

void wxsItemResData::RebuildSourceCode()
{
    switch ( m_Language )
    {
        case wxsCPP:
        {
            wxStopWatch SW;

            wxsCoderContext Context;

            // Setup language
            Context.m_Language = m_Language;
            Context.m_Flags = GetPropertiesFilter();

            // Set-up parent for root item
            if ( m_RootItem->GetBaseProps()->m_ParentFromArg )
            {
                // use procedure's argument if there's any used...
                Context.m_WindowParent = _T("parent");
            }
            else
            {
                // ...or null if there's none
                Context.m_WindowParent = _T("0");
            }

            // Add some initial headers
            if (m_Translation)
                Context.AddHeader(_T("<wx/intl.h>"),_T(""),hfLocal|hfInPCH);
            Context.AddHeader(_T("<wx/string.h>"),_T(""),hfLocal|hfInPCH);
            if ( m_PropertiesFilter & flMixed )
            {
                Context.m_LocalHeadersNonPCH.insert(_T("<wx/xrc/xmlres.h>"));
            }

            // Creating local and global declarations
            // Root item will automatically iterate thorough all tools so don't need to do it here
            m_RootItem->BuildCode(&Context);

            wxsCoder::Get()->AddCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Declarations"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                DeclarationsCode(&Context),
                false );

            wxsCoder::Get()->AddCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Identifiers"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                IdentifiersCode(&Context),
                false );

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Initialize"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                InitializeCode(&Context),
                false );

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("IdInit"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                IdInitCode(&Context),
                false );

            if ( m_IsEventTable )
            {
                wxsCoder::Get()->AddCode(
                    m_SrcFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("EventTable"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    _T("\n"),
                    false );    // This clears previously used event table for event binding
            }

            if ( m_PropertiesFilter & flPchFilter )
            {

                wxsCoder::Get()->AddCode(
                    m_SrcFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("InternalHeadersPCH"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    InternalHeadersCode(&Context),
                    false );

                wxsCoder::Get()->AddCode(
                    m_SrcFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("InternalHeaders"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    InternalHeadersNoPCHCode(&Context),
                    false );

                wxsCoder::Get()->AddCode(
                    m_HdrFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("HeadersPCH"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    HeadersCode(&Context),
                    false );

                wxsCoder::Get()->AddCode(
                    m_HdrFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("Headers"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    HeadersNoPCHCode(&Context),
                    false );
            }
            else
            {
                wxsCoder::Get()->AddCode(
                    m_SrcFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("InternalHeaders"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    InternalHeadersAllCode(&Context),
                    false );

                wxsCoder::Get()->AddCode(
                    m_HdrFileName,
                    wxsCodeMarks::Beg(wxsCPP,_T("Headers"),m_ClassName),
                    wxsCodeMarks::End(wxsCPP),
                    HeadersAllCode(&Context),
                    false );
            }

            wxsCoder::Get()->Flush(500);
            //Manager::Get()->GetLogManager()->DebugLog(F(_T("wxSmith: New code built in %d milis"),SW.Time()));

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::RebuildSourceCode"),m_Language);
        }
    }

}

wxString wxsItemResData::DeclarationsCode(wxsCoderContext* Ctx)
{
    // Enumerate all class members
    wxString Code = _T("\n");
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_GlobalDeclarations.begin(); i!=Ctx->m_GlobalDeclarations.end(); ++i )
    {
        Code += *i;
        Code += _T("\n");
    }
    return Code;
}

wxString wxsItemResData::IdentifiersCode(wxsCoderContext* Ctx)
{
    // Enumerate all ids
    wxString Code = _T("\n");
    for ( size_t Count = Ctx->m_IdEnumerations.Count(), Index=0; Count>0; Index++, Count-- )
    {
        Code += Ctx->m_IdEnumerations[Index];
        Code += _T("\n");
    }
    return Code;
}

wxString wxsItemResData::InitializeCode(wxsCoderContext* Ctx)
{
    wxString Code = _T("\n");

    // First there are local variables
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_LocalDeclarations.begin(); i!=Ctx->m_LocalDeclarations.end(); ++i )
    {
        Code += *i;
        Code += _T("\n");
    }

    if ( Code.Length()>1 )
    {
        Code += _T("\n");
    }

    // Next load resource's content
    if ( Ctx->m_Flags & flSource )
    {
        // If in source mode, add building code
        Code += Ctx->m_BuildingCode;
    }
    else
    {
        // If in XRC mode, add XRC loading code
        Code += XRCLoadingCode();
        Code += Ctx->m_XRCFetchingCode;
    }

    if ( !Ctx->m_EventsConnectingCode.IsEmpty() )
    {
        // And finally attach event handlers
        Code += _T("\n");
        Code += Ctx->m_EventsConnectingCode;
    }

    return Code;
}

wxString wxsItemResData::IdInitCode(wxsCoderContext* Ctx)
{
    wxString Code = _T("\n");
    for ( size_t Count = Ctx->m_IdInitializions.Count(), Index=0; Count>0; Index++, Count-- )
    {
        Code += Ctx->m_IdInitializions[Index];
        Code += _T("\n");
    }
    return Code;
}

wxString wxsItemResData::HeadersCode(wxsCoderContext* Ctx)
{
    wxString Code;
    // Enumerate global includes (those in header file)
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_GlobalHeaders.begin(); i!=Ctx->m_GlobalHeaders.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_ForwardDeclarations.begin(); i!=Ctx->m_ForwardDeclarations.end(); ++i )
    {
        Code += _T("\nclass ");
        Code += *i;
        Code += _T(";");
    }
    return Code + _T("\n");
}

wxString wxsItemResData::HeadersNoPCHCode(wxsCoderContext* Ctx)
{
    wxString Code;
    // Enumerate global includes (those in header file)
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_GlobalHeadersNonPCH.begin(); i!=Ctx->m_GlobalHeadersNonPCH.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_ForwardDeclarationsNonPCH.begin(); i!=Ctx->m_ForwardDeclarationsNonPCH.end(); ++i )
    {
        Code += _T("\nclass ");
        Code += *i;
        Code += _T(";");
    }
    return Code + _T("\n");
}

wxString wxsItemResData::HeadersAllCode(wxsCoderContext* Ctx)
{
    wxString Code;
    // Enumerate global includes (those in header file)
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_GlobalHeaders.begin(); i!=Ctx->m_GlobalHeaders.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_GlobalHeadersNonPCH.begin(); i!=Ctx->m_GlobalHeadersNonPCH.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }

    // Enumerate all forward declarations
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_ForwardDeclarations.begin(); i!=Ctx->m_ForwardDeclarations.end(); ++i )
    {
        Code += _T("\nclass ");
        Code += *i;
        Code += _T(";");
    }
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_ForwardDeclarationsNonPCH.begin(); i!=Ctx->m_ForwardDeclarationsNonPCH.end(); ++i )
    {
        Code += _T("\nclass ");
        Code += *i;
        Code += _T(";");
    }
    return Code + _T("\n");
}

wxString wxsItemResData::InternalHeadersCode(wxsCoderContext* Ctx)
{
    wxString Code;
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_LocalHeaders.begin(); i!=Ctx->m_LocalHeaders.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    return Code + _T("\n");
}

wxString wxsItemResData::InternalHeadersNoPCHCode(wxsCoderContext* Ctx)
{
    wxString Code;
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_LocalHeadersNonPCH.begin(); i!=Ctx->m_LocalHeadersNonPCH.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    return Code + _T("\n");
}

wxString wxsItemResData::InternalHeadersAllCode(wxsCoderContext* Ctx)
{
    wxString Code;
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_LocalHeaders.begin(); i!=Ctx->m_LocalHeaders.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    for ( wxsCoderContext::wxStringSet::iterator i = Ctx->m_LocalHeadersNonPCH.begin(); i!=Ctx->m_LocalHeadersNonPCH.end(); ++i )
    {
        Code += _T("\n#include ");
        Code += *i;
    }
    return Code + _T("\n");
}

wxString wxsItemResData::XRCLoadingCode()
{
    wxString Parent = _T("0");
    if ( m_RootItem->GetBaseProps()->m_ParentFromArg )
    {
        Parent = _T("parent");
    }

    return _T("wxXmlResource::Get()->LoadObject(this,") + Parent + _T(",") +
           wxsCodeMarks::WxString(wxsCPP,m_ClassName,false) + _T(",") +
           wxsCodeMarks::WxString(wxsCPP,m_ClassType,false) + _T(");\n");
}

bool wxsItemResData::RebuildXrcFile()
{
    // First - opening file
    TiXmlDocument Doc;
    TiXmlElement* Resources;
    TiXmlElement* Object;

    if ( TinyXML::LoadDocument(m_XrcFileName,&Doc) )
    {
        Resources = Doc.FirstChildElement("resource");
    }

    if ( !Resources )
    {
        Doc.Clear();
        Doc.InsertEndChild(TiXmlDeclaration("1.0","utf-8",""));
        Resources = Doc.InsertEndChild(TiXmlElement("resource"))->ToElement();
        Resources->SetAttribute("xmlns","http://www.wxwidgets.org/wxxrc");
    }

    // Searching for object representing this resource
    for ( Object = Resources->FirstChildElement("object"); Object; Object = Object->NextSiblingElement("object") )
    {
        if ( cbC2U(Object->Attribute("name")) == m_ClassName )
        {
            Object->Clear();
            while ( Object->FirstAttribute() )
            {
                Object->RemoveAttribute(Object->FirstAttribute()->Name());
            }
            break;
        }
    }

    if ( !Object )
    {
        Object = Resources->InsertEndChild(TiXmlElement("object"))->ToElement();
    }

    // The only things left are: to dump item into object ...
    m_RootItem->XmlWrite(Object,true,false);
    Object->SetAttribute("name",cbU2C(m_ClassName));
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        TiXmlElement* ToolElement = Object->InsertEndChild(TiXmlElement("object"))->ToElement();
        m_Tools[i]->XmlWrite(ToolElement,true,false);
    }

    // ... and save back the file
    return TinyXML::SaveDocument(m_XrcFileName,&Doc);
}

void wxsItemResData::BeginChange()
{
    if ( !m_LockCount++ )
    {
        StoreTreeExpandState();
        wxsResourceTree::Get()->BlockSelect();
    }
}

void wxsItemResData::EndChange()
{
    if ( !--m_LockCount )
    {
        m_Corrector.GlobalCheck();
        StoreUndo();
        if ( m_Editor )
        {
            m_Editor->UpdateModified();
        }
        if ( m_Editor )
        {
            m_Editor->RebuildPreview();
        }
        if ( ValidateRootSelection() )
        {
            m_RootSelection->NotifyPropertyChange(false);
        }
        else
        {
            m_RootSelection->ShowInPropertyGrid();
            if ( m_Editor )
            {
                m_Editor->RebuildQuickProps(m_RootSelection);
            }
        }
        RebuildFiles();
        RebuildTree();
        wxsResourceTree::Get()->UnblockSelect();
    }
}

bool wxsItemResData::ValidateRootSelection()
{
    wxsItem* NewSelection = 0;
    if ( ValidateRootSelectionReq(m_RootItem,NewSelection) )
    {
        return true;
    }

    for ( int i=0; i<GetToolsCount(); i++ )
    {
        if ( ValidateRootSelectionReq(m_Tools[i],NewSelection) )
        {
            return true;
        }
    }
    m_RootSelection = NewSelection ? NewSelection : m_RootItem;
    return false;
}

bool wxsItemResData::ValidateRootSelectionReq(wxsItem* Item,wxsItem*& NewSelection)
{
    if ( Item == m_RootSelection ) return true;

    if ( Item->GetIsSelected() && !NewSelection )
    {
        NewSelection = Item;
    }

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            if ( ValidateRootSelectionReq(AsParent->GetChild(i),NewSelection) ) return true;
        }
    }
    return false;
}

bool wxsItemResData::CanPaste()
{
    if ( !m_RootItem->ConvertToParent() ) return false;
    if ( !wxTheClipboard->Open() ) return false;
    bool Res = wxTheClipboard->IsSupported(wxsDF_WIDGET);
    // FIXME (SpOoN#1#): Add support for text (XRC) data
    wxTheClipboard->Close();
    return Res;
}

void wxsItemResData::Cut()
{
    Copy();
    BeginChange();
    DeleteSelected();
    EndChange();
}

void wxsItemResData::Copy()
{
    if ( !wxTheClipboard->Open() ) return;
    wxsItemResDataObject* Data = new wxsItemResDataObject;
    CopyReq(m_RootItem,Data);
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        CopyReq(m_Tools[i],Data);
    }

    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();
}

void wxsItemResData::CopyReq(wxsItem* Item,wxsItemResDataObject* Data)
{
    if ( Item->GetIsSelected() )
    {
        Data->AddItem(Item);
        // We do not process children - they will be included
        // inside this item
        return;
    }

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            CopyReq(AsParent->GetChild(i),Data);
        }
    }
}

void wxsItemResData::Paste(wxsParent* Parent,int Position)
{
    if ( !m_RootItem ) return;
    if ( !wxTheClipboard->Open() ) return;

    wxsItemResDataObject Data;
    if ( wxTheClipboard->GetData(Data) )
    {
        int Cnt = Data.GetItemCount();
        if ( Cnt )
        {
            BeginChange();
            m_RootItem->ClearSelection();
            m_RootSelection = 0;
            for ( int i=0; i<Cnt; i++ )
            {
                wxsItem* Insert = Data.BuildItem(this,i);
                if ( Insert )
                {
                    if ( Insert->ConvertToTool() )
                    {
                        if ( InsertNewTool(Insert->ConvertToTool()) )
                        {
                            Insert->SetIsSelected(true);
                            if ( !m_RootSelection )
                            {
                                m_RootSelection = Insert;
                            }
                        }
                    }
                    else
                    {
                        if ( InsertNew(Insert,Parent,Position++) )
                        {
                            Insert->SetIsSelected(true);
                            if ( !m_RootSelection )
                            {
                                m_RootSelection = Insert;
                            }
                        }
                    }
                }
            }

            if ( !m_RootSelection )
            {
                m_RootSelection = m_RootItem;
            }
            EndChange();
        }
    }
    wxTheClipboard->Close();
}

bool wxsItemResData::AnySelected()
{
    if ( AnySelectedReq(m_RootItem) ) return true;
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        if ( m_Tools[i]->GetIsSelected() ) return true;
    }
    return false;
}

bool wxsItemResData::AnySelectedReq(wxsItem* Item)
{
    if ( Item->GetIsSelected() ) return true;

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            if ( AnySelectedReq(AsParent->GetChild(i)) )
            {
                return true;
            }
        }
    }
    return false;
}

void wxsItemResData::StoreTreeExpandState()
{
    StoreTreeExpandStateReq(m_RootItem);
    if ( GetToolsCount() && m_ToolsId.IsOk() )
    {
        m_ToolsNodeIsExpanded = wxsResourceTree::Get()->IsExpanded(m_ToolsId);
    }
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        StoreTreeExpandStateReq(m_Tools[i]);
    }
}

void wxsItemResData::StoreTreeExpandStateReq(wxsItem* Item)
{
    if ( m_IdMap.find(Item) != m_IdMap.end() )
    {
        wxTreeItemId Id = m_IdMap[Item];
        if ( Id.IsOk() )
        {
            Item->SetIsExpanded(wxsResourceTree::Get()->IsExpanded(Id));
        }
    }

    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            StoreTreeExpandStateReq(AsParent->GetChild(i));
        }
    }
}

void wxsItemResData::RestoreTreeExpandAndSelectionState()
{
    RestoreTreeExpandAndSelectionStateReq(m_RootItem);
    if ( GetToolsCount() && m_ToolsId.IsOk() )
    {
        if ( m_ToolsNodeIsExpanded )
        {
            wxsResourceTree::Get()->Expand(m_ToolsId);
        }
        else
        {
            wxsResourceTree::Get()->Collapse(m_ToolsId);
        }
    }

    for ( int i=0; i<GetToolsCount(); i++ )
    {
        RestoreTreeExpandAndSelectionStateReq(m_Tools[i]);
    }

    wxsResourceItemId Id;
    if ( FindId(Id,m_RootSelection) )
    {
        wxsResourceTree::Get()->SelectItem(Id,true);
    }
}

void wxsItemResData::RestoreTreeExpandAndSelectionStateReq(wxsItem* Item)
{
    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            RestoreTreeExpandAndSelectionStateReq(AsParent->GetChild(i));
        }
    }

    if ( m_IdMap.find(Item) != m_IdMap.end() )
    {
        wxTreeItemId Id = m_IdMap[Item];
        if ( Id.IsOk() )
        {
            if ( Item->GetIsExpanded() )
            {
                wxsResourceTree::Get()->Expand(Id);
            }
            else
            {
                wxsResourceTree::Get()->Collapse(Id);
            }
        }
    }
}

bool wxsItemResData::SelectItem(wxsItem* Item,bool UnselectOther)
{
    if ( UnselectOther )
    {
        if ( m_RootItem )
        {
            m_RootItem->ClearSelection();
        }
        for ( int i=0; i<GetToolsCount(); i++ )
        {
            m_Tools[i]->ClearSelection();
        }
    }

    if ( !Item )
    {
        Item = m_RootItem;
    }

    if ( Item )
    {
        Item->SetIsSelected(true);
        Item->ShowInPropertyGrid();
    }
    m_RootSelection = Item;
    if ( m_Editor )
    {
        m_Editor->RebuildQuickProps(Item);
        m_Editor->UpdateSelection();
    }

    wxsResourceItemId Id;
    if ( FindId(Id,Item) )
    {
        if ( wxsResourceTree::Get()->GetSelection() != Id )
        {
            wxsResourceTree::Get()->SelectItem(Id,true);
        }
    }

    bool Changed = false;
    wxsItem* Child = Item;
    for ( wxsParent* Parent = Item->GetParent(); Parent; Child = Parent, Parent = Parent->GetParent() )
    {
        if ( Parent->EnsureChildPreviewVisible(Child) )
        {
            Changed = true;
        }
    }

    if ( Changed && m_Editor )
    {
        m_Editor->RebuildPreview();
    }

    return true;
}

void wxsItemResData::NotifyChange(wxsItem* Changed)
{
    m_Corrector.AfterChange(Changed);
    Changed->NotifyPropertyChange(false);
    StoreUndo();
    RebuildFiles();
    if ( m_Editor )
    {
        m_Editor->UpdateModified();
        m_Editor->RebuildPreview();
        m_Editor->UpdateSelection();
    }
}

wxString wxsItemResData::GetXmlData()
{
    wxsItemResDataObject Object;
    Object.AddItem(m_RootItem);
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        Object.AddItem(m_Tools[i]);
    }
    return Object.GetXmlData();
}

bool wxsItemResData::SetXmlData(const wxString& XmlData)
{
    if ( m_LockCount ) return false;

    wxsItemResDataObject Object;
    Object.SetXmlData(XmlData);

    // Recreating root item
    wxsItem* NewRoot = Object.BuildItem(this,0);
    if ( NewRoot->GetClassName() != m_ClassType )
    {
        delete NewRoot;
        return false;
    }
    delete m_RootItem;
    m_RootItem = NewRoot;

    // Recreating tools
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        delete m_Tools[i];
    }
    m_Tools.Clear();
    for ( int i=1; i<Object.GetItemCount(); i++ )
    {
        wxsItem* NewItem = Object.BuildItem(this,i);
        if ( !NewItem->ConvertToTool() )
        {
            delete NewItem;
            continue;
        }
        wxsTool* Tool = NewItem->ConvertToTool();
        if ( !Tool->CanAddToResource(this,false) )
        {
            delete Tool;
            continue;
        }
        InsertNewTool(Tool);
    }

    RebuildFiles();
    RebuildTree();
    // TODO: Fetch selection from xml data
    SelectItem(m_RootItem,true);
    if ( m_Editor )
    {
        m_Editor->RebuildPreview();
        m_Editor->UpdateModified();
    }

    return true;
}

bool wxsItemResData::InsertNew(wxsItem* New,wxsParent* Parent,int Position)
{
    if ( !New )
    {
        return false;
    }

    if ( New->ConvertToTool() )
    {
        return InsertNewTool(New->ConvertToTool());
    }

    m_Corrector.BeforePaste(New);
    if ( !Parent || !Parent->AddChild(New,Position) )
    {
        delete New;
        return false;
    }
    return true;
}

bool wxsItemResData::InsertNewTool(wxsTool* Tool)
{
    if ( !Tool )
    {
        return false;
    }
    if ( !Tool->CanAddToResource(this,false) )
    {
        delete Tool;
        return false;
    }

    // TODO: Check if this tool can be added to this resource
    m_Corrector.BeforePaste(Tool);
    m_Tools.Add(Tool);
    return true;
}

void wxsItemResData::DeleteSelected()
{
    // Clearing directory structure
    DeleteSelectedReq(m_RootItem);

    // Clearing tools
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        if ( m_Tools[i]->GetIsSelected() )
        {
            delete m_Tools[i];
            m_Tools.RemoveAt(i);
            i--;
        }
    }

    // Changing selection
    m_RootSelection = m_RootItem;
    m_RootSelection->SetIsSelected(true);
}

void wxsItemResData::DeleteSelectedReq(wxsItem* Item)
{
    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            wxsItem* Child = AsParent->GetChild(i);
            if ( Child->GetIsSelected() )
            {
                AsParent->UnbindChild(i);
                delete Child;
                i--;    // Update index due to shifted child list
            }
            else
            {
                DeleteSelectedReq(Child);
            }
        }
    }
}

bool wxsItemResData::ShowPreview()
{
    if ( m_Preview )
    {
        return false;
    }

    m_Preview = BuildExactPreview(m_Editor);
    return m_Preview!=0;
}

bool wxsItemResData::HidePreview()
{
    if ( !m_Preview )
    {
        return false;
    }
    m_Preview->Destroy();
    m_Preview = 0;
    return true;
}

void wxsItemResData::RebuildTree()
{
    // We DO NOT create resource tree if there's no editor
    if ( !m_Editor ) return;
    wxsResourceTree::Get()->DeleteChildren(m_TreeId);
    m_RootItem->BuildItemTree(wxsResourceTree::Get(),m_TreeId,-1);
    if ( GetToolsCount() )
    {
        m_ToolsId = wxsResourceTree::Get()->AppendItem(m_TreeId,_("Tools"),ToolsTreeImageId,ToolsTreeImageId);
        for ( int i=0; i<GetToolsCount(); i++ )
        {
            m_Tools[i]->BuildItemTree(wxsResourceTree::Get(),m_ToolsId,-1);
        }
    }
    StoreTreeIds();
    RestoreTreeExpandAndSelectionState();
}

void wxsItemResData::StoreTreeIds()
{
    m_IdMap.clear();
    if ( m_RootItem )
    {
        StoreTreeIdsReq(m_RootItem);
    }
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        StoreTreeIdsReq(m_Tools[i]);
    }
}

void wxsItemResData::StoreTreeIdsReq(wxsItem* Item)
{
    m_IdMap[Item] = Item->GetLastTreeItemId();
    wxsParent* AsParent = Item->ConvertToParent();
    if ( AsParent )
    {
        for ( int i=0; i<AsParent->GetChildCount(); i++ )
        {
            StoreTreeIdsReq(AsParent->GetChild(i));
        }
    }
}

bool wxsItemResData::FindId(wxsResourceItemId& Id,wxsItem* Item)
{
    ItemToIdMapT::iterator it =  m_IdMap.find(Item);
    if ( it == m_IdMap.end() )
    {
        return false;
    }

    Id = it->second;
    return true;
}
