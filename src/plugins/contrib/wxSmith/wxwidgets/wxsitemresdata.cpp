/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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

#include "wxsitemresdata.h"
#include "wxsparent.h"
#include "wxsitemfactory.h"
#include "wxsitemeditor.h"
#include "wxstool.h"
#include "../wxscoder.h"

#include <wx/clipbrd.h>

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
    wxsCodingLang        Language,
    wxsResourceItemId    TreeId,
    wxsItemEditor*       Editor,
    wxsItemResFunctions* Functions):
        m_WxsFileName(WxsFileName),
        m_SrcFileName(SrcFileName),
        m_HdrFileName(HdrFileName),
        m_XrcFileName(XrcFileName),
        m_ClassName(ClassName),
        m_ClassType(ClassType),
        m_Language(Language),
        m_TreeId(TreeId),
        m_Editor(Editor),
        m_Functions(Functions),
        m_RootItem(NULL),
        m_RootSelection(NULL),
        m_Preview(NULL),
        m_Corrector(this),
        m_IsOK(false),
        m_LockCount(0),
        m_CurrentCode(NULL)
{
    if (  WxsFileName.empty() &&
          SrcFileName.empty() &&
          HdrFileName.empty() &&
         !XrcFileName.empty() )
    {
        m_PropertiesFilter = wxsItem::flFile;
    }
    else if ( !WxsFileName.empty() &&
              !SrcFileName.empty() &&
              !HdrFileName.empty() )
    {
        if ( XrcFileName.empty() )
        {
            m_PropertiesFilter = wxsItem::flSource;
        }
        else
        {
            m_PropertiesFilter = wxsItem::flMixed;
        }
    }
    else
    {
        m_PropertiesFilter = 0;
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
    m_RootItem = NULL;
    m_RootSelection = NULL;
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        delete m_Tools[i];
    }
    m_Tools.clear();
    m_PropertiesFilter = 0;
    wxsResourceItemId ParentId = wxsResourceTree::Get()->GetItemParent(m_TreeId);
    // Selecting parent to prevent reopening resource on wxGTK
    wxsResourceTree::Get()->SelectItem(ParentId);
    wxsResourceTree::Get()->DeleteChildren(m_TreeId);

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
    m_Editor->UpdateModified();
    if ( Ret && (m_PropertiesFilter!=wxsItem::flFile) )
    {
        RebuildSourceCode();        // Yop, only source recreated, xrc if used not touched
    }
    RebuildTree();
    m_Editor->RebuildPreview();
    SelectItem(m_RootItem,true);

    return Ret;
}

bool wxsItemResData::SilentLoad()
{
    switch ( m_PropertiesFilter )
    {
        case wxsItem::flFile:
            m_IsOK = LoadInFileMode();
            break;

        case wxsItem::flMixed:
            m_IsOK = LoadInMixedMode();
            break;

        case wxsItem::flSource:
            m_IsOK = LoadInSourceMode();
            break;

        default:
            m_IsOK = false;
    }
    return m_IsOK;
}

bool wxsItemResData::LoadInFileMode()
{
    TiXmlDocument Doc(cbU2C(m_XrcFileName));
    if ( !Doc.LoadFile() ) return false;

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

    TiXmlDocument DocExtra(cbU2C(m_WxsFileName));
    if ( !DocExtra.LoadFile() ) return false;
    TiXmlDocument DocXrc(cbU2C(m_XrcFileName));
    if ( !DocXrc.LoadFile() ) return false;

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
    if ( Item->GetPropertiesFlags() & wxsItem::flId )
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

    TiXmlDocument Doc(cbU2C(m_WxsFileName));
    if ( !Doc.LoadFile() )
    {
        DBGLOG(_T("wxSmith: Error loading wxs file (Col: %d, Row:%d): ") + cbC2U(Doc.ErrorDesc()),Doc.ErrorCol(),Doc.ErrorRow());
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
        if ( GetPropertiesFilter()!=wxsItem::flSource && !Info->AllowInXRC ) continue;
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
    switch ( m_PropertiesFilter )
    {
        case wxsItem::flFile:
            return SaveInFileMode();

        case wxsItem::flMixed:
            return SaveInMixedMode();

        case wxsItem::flSource:
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

    TiXmlDocument Doc(cbU2C(m_WxsFileName));

    Doc.InsertEndChild(TiXmlDeclaration("1.0","utf-8",""));
    TiXmlElement* wxSmithNode = Doc.InsertEndChild(TiXmlElement("wxsmith"))->ToElement();

    // Now storing all extra data
    TiXmlElement* Extra = wxSmithNode->InsertEndChild(TiXmlElement("resource_extra"))->ToElement();
    SaveExtraDataReq(m_RootItem,Extra);
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        SaveExtraDataReq(m_Tools[i],Extra);
    }

    if ( Doc.SaveFile() )
    {
        m_Undo.Saved();
        return true;
    }
    return false;
}

void wxsItemResData::SaveExtraDataReq(wxsItem* Item,TiXmlElement* Node)
{
    if ( Item->GetPropertiesFlags() && wxsItem::flId )
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
    TiXmlDocument Doc(cbU2C(m_WxsFileName));

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

    if ( Doc.SaveFile() )
    {
        m_Undo.Saved();
        return true;
    }
    return false;
}

void wxsItemResData::RebuildFiles()
{
    switch ( m_PropertiesFilter )
    {
        case wxsItem::flSource:
            RebuildSourceCode();
            break;

        case wxsItem::flMixed:
            RebuildSourceCode();
            RebuildXrcFile();
            break;
    }
}

void wxsItemResData::RebuildSourceCode()
{
    switch ( m_Language )
    {
        case wxsCPP:
        {
            wxString InitializingCode;
            wxString GlobalVarsCode;
            wxString IdentifiersCode;
            wxString IdInitCode;
            wxString GlobalHeaders;
            wxString LocalHeaders;

            BuildVariablesCode(wxsCPP,InitializingCode,GlobalVarsCode);
            BuildCreatingCode(wxsCPP,InitializingCode);
            BuildEventHandlersCode(wxsCPP,InitializingCode);
            BuildIdentifiersCode(wxsCPP,IdentifiersCode,IdInitCode);
            BuildIncludesCode(wxsCPP,LocalHeaders,GlobalHeaders);

            // TODO: Maybe some group update ??
            wxsCoder::Get()->AddCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Declarations"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + GlobalVarsCode);

            wxsCoder::Get()->AddCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Identifiers"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + IdentifiersCode);

            wxsCoder::Get()->AddCode(
                m_HdrFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Headers"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + GlobalHeaders);

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("Initialize"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + InitializingCode);

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("IdInit"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + IdInitCode);

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("InternalHeaders"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n") + LocalHeaders);

            wxsCoder::Get()->AddCode(
                m_SrcFileName,
                wxsCodeMarks::Beg(wxsCPP,_T("EventTable"),m_ClassName),
                wxsCodeMarks::End(wxsCPP),
                _T("\n"));    // This clears previously used event table for event binding

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::RebuildSourceCode"),m_Language);
        }
    }

}

void wxsItemResData::BuildVariablesCode(wxsCodingLang Lang,wxString& LocalCode, wxString& GlobalCode)
{
    switch ( Lang )
    {
        case wxsCPP:
        {
            // Creating local and global declarations
            wxsParent* RootAsParent = m_RootItem->ConvertToParent();
            if ( RootAsParent )
            {
                for ( int i=0; i<RootAsParent->GetChildCount(); i++ )
                {
                    BuildVariablesCodeReq(wxsCPP,RootAsParent->GetChild(i),LocalCode,GlobalCode);
                }
            }
            for ( int i=0; i<GetToolsCount(); i++ )
            {
                BuildVariablesCodeReq(wxsCPP,m_Tools[i],LocalCode,GlobalCode);
            }

            if ( LocalCode.Length()>1 )
            {
                // Adding one empty line between local declarations and
                // the rest of initializing code
                LocalCode << _T("\n");
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::BuildVariablesCode"),Lang);
        }
    }
}

void wxsItemResData::BuildVariablesCodeReq(wxsCodingLang Lang,wxsItem* Item,wxString& LocalCode, wxString& GlobalCode)
{
    if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
    {
        if ( Item->GetIsMember() )
        {
            Item->BuildDeclarationCode(GlobalCode,Lang);
        }
        else if ( m_PropertiesFilter == wxsItem::flSource )
        {
            Item->BuildDeclarationCode(LocalCode,Lang);
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        for ( int i=0; i<Parent->GetChildCount(); i++ )
        {
            BuildVariablesCodeReq(Lang,Parent->GetChild(i),LocalCode,GlobalCode);
        }
    }
}

void wxsItemResData::BuildCreatingCode(wxsCodingLang Lang,wxString& Code)
{
    m_CurrentCode = &Code;
    switch ( m_PropertiesFilter )
    {
        case wxsItem::flSource:
            m_RootItem->BuildCreatingCode(Code,_T("parent"),Lang);
            break;

        case wxsItem::flMixed:
            BuildXrcLoadingCode(Lang,Code);
            BuildXrcItemsFetchingCode(Lang,Code);
            break;

        default:;
    }
    m_CurrentCode = NULL;
}

void wxsItemResData::BuildXrcLoadingCode(wxsCodingLang Language,wxString& Code)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << _T("wxXmlResource::Get()->LoadObject(this,parent,")
                 << wxsCodeMarks::WxString(wxsCPP,m_ClassName,false) << _T(",")
                 << wxsCodeMarks::WxString(wxsCPP,m_ClassType,false) << _T(");\n");
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::BuildXrcLoadingCode"),Language);
        }
    }
}

void wxsItemResData::BuildXrcItemsFetchingCode(wxsCodingLang Lang,wxString& Code)
{
    BuildXrcItemsFetchingCodeReq(Lang,m_RootItem,Code);
    // We do not fetch tools since they can not be windows and thus can not be
    // fetched using FindWindow()
}

void wxsItemResData::BuildXrcItemsFetchingCodeReq(wxsCodingLang Lang,wxsItem* Item,wxString& Code)
{
    switch ( Lang )
    {
        case wxsCPP:
        {
            wxsParent* Parent = Item->ConvertToParent();
            if ( !Parent )
            {
                return;
            }

            int Cnt = Parent->GetChildCount();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsItem* Child = Parent->GetChild(i);
                long Flags = Child->GetPropertiesFlags();
                if ( (Flags & (wxsItem::flVariable|wxsItem::flId)) == (wxsItem::flVariable|wxsItem::flId) )
                {
                    if ( Child->GetIsMember() )
                    {
                        Code << Child->GetVarName()
                             << _T(" = (") << Child->GetUserClass() << _T("*)")
                             << _T("FindWindow(XRCID(\"") + Child->GetIdName() + _T("\"));\n");
                    }
                }
                BuildXrcItemsFetchingCodeReq(Lang,Child,Code);
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::BuildXrcItemsFetchingCodeReq"),Lang);
        }
    }
}

void wxsItemResData::BuildEventHandlersCode(wxsCodingLang Language,wxString& Code)
{
    m_CurrentCode = &Code;
    BuildEventHandlersCodeReq(Language,m_RootItem,Code);
    for ( int i=0; i<GetToolsCount(); i++ )
    {
        BuildEventHandlersCodeReq(Language,m_Tools[i],Code);
    }
    m_CurrentCode = NULL;
}

void wxsItemResData::BuildEventHandlersCodeReq(wxsCodingLang Language,wxsItem* Item,wxString& Code)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxsEvents& Events = Item->GetEvents();
            wxString IdString;
            wxString VarNameString;
            if ( Item->IsRootItem() )
            {
                IdString = _T("wxID_ANY");
            }
            else if ( m_PropertiesFilter == wxsItem::flSource )
            {
                IdString = Item->GetIdName();
                VarNameString = Item->GetVarName();
            }
            else
            {
                IdString = _T("XRCID(\"") + Item->GetIdName() + _T("\")");
                if ( Item->GetIsMember() )
                {
                    VarNameString = Item->GetVarName();
                }
                else
                {
                    VarNameString = _T("FindWindow(XRCID(\"") + Item->GetIdName() + _T("\"))");
                }
            }
            Events.GenerateBindingCode(Code,IdString,VarNameString,Item->IsRootItem(),Language);

            wxsParent* Parent = Item->ConvertToParent();
            if ( Parent )
            {
                int Cnt = Parent->GetChildCount();
                for ( int i=0; i<Cnt; i++ )
                {
                    BuildEventHandlersCodeReq(Language,Parent->GetChild(i),Code);
                }
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::BuildEventHandlersCodeReq"),Language);
        }
    }
}

void wxsItemResData::BuildIdentifiersCode(wxsCodingLang Lang,wxString& IdCode,wxString& IdInitCode)
{
    if ( m_PropertiesFilter == wxsItem::flSource )
    {
        wxArrayString IdsArray;
        wxsParent* RootAsParent = m_RootItem->ConvertToParent();
        for ( int i=0; i<RootAsParent->GetChildCount(); i++ )
        {
            BuildIdsArrayReq(RootAsParent->GetChild(i),IdsArray);
        }
        for ( int i=0; i<GetToolsCount(); i++ )
        {
            BuildIdsArrayReq(m_Tools[i],IdsArray);
        }
        switch ( Lang )
        {
            case wxsCPP:
            {
                if ( IdsArray.Count() )
                {
                    for ( size_t i = 0; i<IdsArray.Count(); ++i )
                    {
                        const wxString Id = IdsArray[i];
                        if ( IdsArray.Index(Id) == (int)i )
                        {
                            IdCode << _T("static const long ") + IdsArray[i] + _T(";\n");
                            IdInitCode << _T("const long ") + m_ClassName + _T("::") + IdsArray[i] + _T(" = wxNewId();\n");
                        }
                    }
                }
                break;
            }

            default:
            {
                wxsCodeMarks::Unknown(_T("wxsItemResData::BuildIdentifiersCode"),Lang);
            }
        }
    }
}

void wxsItemResData::BuildIdsArrayReq(wxsItem* Item,wxArrayString& Array)
{
    if ( Item->GetPropertiesFlags() & wxsItem::flId )
    {
        const wxString& Name = Item->GetIdName();

        if ( !wxsPredefinedIDs::Check(Name) )
        {
            Array.Add(Name);
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( !Parent ) return;
	for ( int i=0; i<Parent->GetChildCount(); i++ )
	{
		BuildIdsArrayReq(Parent->GetChild(i),Array);
	}
}

void wxsItemResData::BuildIncludesCode(wxsCodingLang Language,wxString& LocalIncludes,wxString& GlobalIncludes)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxArrayString GlobalHeaders;
            wxArrayString LocalHeaders;
            BuildHeadersReq(wxsCPP,m_RootItem,LocalHeaders,GlobalHeaders);
            for ( int i=0; i<GetToolsCount(); i++ )
            {
                BuildHeadersReq(wxsCPP,m_Tools[i],LocalHeaders,GlobalHeaders);
            }

            LocalHeaders.Add(_T("<wx/intl.h>"));
            // TODO: Use these headers dynamically, not always
            LocalHeaders.Add(_T("<wx/bitmap.h>"));
            LocalHeaders.Add(_T("<wx/image.h>"));
            LocalHeaders.Add(_T("<wx/font.h>"));
            LocalHeaders.Add(_T("<wx/fontmap.h>"));
            LocalHeaders.Add(_T("<wx/fontenum.h>"));
            LocalHeaders.Add(_T("<wx/settings.h>"));

            if ( m_PropertiesFilter == wxsItem::flMixed )
            {
                LocalHeaders.Add(_T("<wx/xrc/xmlres.h>"));
            }

            GlobalHeaders.Sort();
            LocalHeaders.Sort();

            wxString Previous = _T("");
            for ( size_t i=0; i<GlobalHeaders.Count(); i++ )
            {
                if ( GlobalHeaders[i] != Previous )
                {
                    Previous = GlobalHeaders[i];
                    GlobalIncludes << _T("#include ") << Previous << _T("\n");
                }
            }

            Previous = _T("");
            for ( size_t i=0; i<LocalHeaders.Count(); i++ )
            {
                if ( LocalHeaders[i] != Previous )
                {
                    Previous = LocalHeaders[i];
                    if ( GlobalHeaders.Index(Previous) == wxNOT_FOUND )
                    {
                        LocalIncludes << _T("#include ") << Previous << _T("\n");
                    }
                }
            }

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItemResData::BuildIncludesCode"),Language);
        }
    }
}

void wxsItemResData::BuildHeadersReq(wxsCodingLang Lang,wxsItem* Item,wxArrayString& LocalHeaders,wxArrayString& GlobalHeaders)
{
    Item->EnumDeclFiles(GlobalHeaders,LocalHeaders,Lang);

    wxsParent* Parent = Item->ConvertToParent();
    if ( !Parent ) return;

	int Cnt = Parent->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
	    BuildHeadersReq(Lang,Parent->GetChild(i),LocalHeaders,GlobalHeaders);
	}
}

bool wxsItemResData::RebuildXrcFile()
{
    // First - opening file
    TiXmlDocument Doc;
    TiXmlElement* Resources;
    TiXmlElement* Object;

    if ( Doc.LoadFile(cbU2C(m_XrcFileName)) )
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
    return Doc.SaveFile(cbU2C(m_XrcFileName));
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
        m_Editor->UpdateModified();
        RebuildFiles();
        m_Editor->RebuildPreview();
        RebuildTree();
        if ( ValidateRootSelection() )
        {
            m_RootSelection->NotifyPropertyChange(false);
        }
        else
        {
            m_RootSelection->ShowInPropertyGrid();
            m_Editor->RebuildQuickProps(m_RootSelection);
        }
        wxsResourceTree::Get()->UnblockSelect();
    }
}

bool wxsItemResData::ValidateRootSelection()
{
    wxsItem* NewSelection = NULL;
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
            m_RootSelection = NULL;
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
    m_Editor->RebuildQuickProps(Item);
    m_Editor->UpdateSelection();

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

    if ( Changed )
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
    m_Editor->UpdateModified();
    m_Editor->RebuildPreview();
    m_Editor->UpdateSelection();
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
    m_Editor->RebuildPreview();
    // TODO: Fetch selection from xml data
    SelectItem(m_RootItem,true);
    m_Editor->UpdateModified();

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
    return m_Preview!=NULL;
}

bool wxsItemResData::HidePreview()
{
    if ( !m_Preview )
    {
        return false;
    }
    m_Preview->Destroy();
    m_Preview = NULL;
    return true;
}

void wxsItemResData::RebuildTree()
{
    wxsResourceTree::Get()->DeleteChildren(m_TreeId);
    m_RootItem->BuildItemTree(wxsResourceTree::Get(),m_TreeId,-1);
    if ( GetToolsCount() )
    {
        wxsResourceItemId ToolsId = wxsResourceTree::Get()->AppendItem(m_TreeId,_("Tools"),ToolsTreeImageId,ToolsTreeImageId);
        for ( int i=0; i<GetToolsCount(); i++ )
        {
            m_Tools[i]->BuildItemTree(wxsResourceTree::Get(),ToolsId,-1);
        }
    }
    StoreTreeIds();
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
