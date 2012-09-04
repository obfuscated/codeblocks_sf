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

#include "wxsitemres.h"
#include "wxsitemeditor.h"
#include "wxsitemfactory.h"
#include "wxsitemresdata.h"
#include "wxsdeleteitemres.h"
#include "../wxscoder.h"
#include <manager.h>
#include <projectmanager.h>
#include <editormanager.h>

IMPLEMENT_CLASS(wxsItemRes,wxWidgetsRes)

namespace
{
    const wxString CppEmptySource =
        _T("$(PchCode)")
        _T("#include \"$(Include)\"\n")
        _T("\n")
        _T("$(InternalHeadersPch)")
        + wxsCodeMarks::Beg(wxsCPP,_T("InternalHeaders"),_T("$(ClassName)")) + _T("\n")
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        + wxsCodeMarks::Beg(wxsCPP,_T("IdInit"),_T("$(ClassName)")) + _T("\n")
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n")
        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("EventTable"),_T("$(ClassName)")) + _T("\n")
        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("END_EVENT_TABLE()\n")
        _T("\n")
        _T("$(ClassName)::$(ClassName)($(CtorArgs))\n")
        _T("{\n")
        _T("$(CtorInit)\t") + wxsCodeMarks::Beg(wxsCPP,_T("Initialize"),_T("$(ClassName)")) + _T("\n")
        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("}\n")
        _T("\n")
        _T("$(ClassName)::~$(ClassName)()\n")
        _T("{\n")
        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("Destroy"),_T("$(ClassName)")) + _T("\n")
        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("}\n")
        _T("\n");

    const wxString CppEmptyHeader =
        _T("#ifndef $(Guard)\n")
        _T("#define $(Guard)\n")
        _T("\n")
        _T("$(HeadersPch)")
        + wxsCodeMarks::Beg(wxsCPP,_T("Headers"),_T("$(ClassName)")) + _T("\n")
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("class $(ClassName): public $(BaseClassName)\n")
        _T("{\n")
        _T("\tpublic:\n")
        _T("\n")
        _T("\t\t$(ClassName)($(CtorArgs));\n")
        _T("\t\tvirtual ~$(ClassName)();\n")
        _T("\n")
        _T("$(MembersScope)")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Declarations"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("$(IdsScope)")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Identifiers"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("$(HandlersScope)")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Handlers"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("$(InitFuncDecl)")
        _T("\t\tDECLARE_EVENT_TABLE()\n")
        _T("};\n")
        _T("\n")
        _T("#endif\n");

    const wxString EmptyXrc =
        _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        _T("<resource xmlns=\"http://www.wxwidgets.org/wxxrc\">\n")
        _T("</resource>\n");

    const wxString EmptyWxs =
        _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        _T("<wxsmith>\n")
        _T("\t<object class=\"$(BaseClassName)\" name=\"$(ClassName)\"/>\n")
        _T("\t<resource_extra/>\n")
        _T("</wxsmith>\n");


    const long GoToHeaderId = wxNewId();
    const long GoToSourceId = wxNewId();
}

wxsItemRes::wxsItemRes(wxsProject* Owner,const wxString& Type,bool CanBeMain):
    wxWidgetsRes(Owner,Type),
    m_WxsFileName(wxEmptyString),
    m_SrcFileName(wxEmptyString),
    m_HdrFileName(wxEmptyString),
    m_XrcFileName(wxEmptyString),
    m_UseForwardDeclarations(false),
    m_UseI18n(true),
    m_CanBeMain(CanBeMain)
{
}


wxsItemRes::wxsItemRes(const wxString& FileName,const TiXmlElement* XrcElem,const wxString& Type):
    wxWidgetsRes(0,Type),
    m_WxsFileName(wxEmptyString),
    m_SrcFileName(wxEmptyString),
    m_HdrFileName(wxEmptyString),
    m_XrcFileName(FileName),
    m_UseForwardDeclarations(false),
    m_UseI18n(true)
{
    SetResourceName(cbC2U(XrcElem->Attribute("name")));
}

wxsItemRes::~wxsItemRes()
{
}

wxsEditor* wxsItemRes::OnCreateEditor(wxWindow* Parent)
{
    return new wxsItemEditor(Parent,this);
}

bool wxsItemRes::OnReadConfig(const TiXmlElement* Node)
{
    m_WxsFileName = cbC2U(Node->Attribute("wxs"));
    m_SrcFileName = cbC2U(Node->Attribute("src"));
    m_HdrFileName = cbC2U(Node->Attribute("hdr"));
    m_XrcFileName = cbC2U(Node->Attribute("xrc"));
    const wxString fwddecl = cbC2U(Node->Attribute("fwddecl"));
    if (!fwddecl.IsEmpty())
        m_UseForwardDeclarations = (fwddecl == _T("1") || fwddecl == _T("true"));
    const wxString i18n = cbC2U(Node->Attribute("i18n"));
    if (!i18n.IsEmpty())
        m_UseI18n = (i18n == _T("1") || i18n == _T("true"));

    // m_XrcFileName may be empty because it's not used when generating full source code
    return !m_WxsFileName.empty() &&
           !m_SrcFileName.empty() &&
           !m_HdrFileName.empty();
}

bool wxsItemRes::OnWriteConfig(TiXmlElement* Node)
{
    Node->SetAttribute("wxs",cbU2C(m_WxsFileName));
    Node->SetAttribute("src",cbU2C(m_SrcFileName));
    Node->SetAttribute("hdr",cbU2C(m_HdrFileName));
    if ( !m_XrcFileName.empty() )
    {
        Node->SetAttribute("xrc",cbU2C(m_XrcFileName));
    }

    Node->SetAttribute("fwddecl", m_UseForwardDeclarations ? "1" : "0");
    Node->SetAttribute("i18n", m_UseI18n ? "1" : "0");

    return true;
}

bool wxsItemRes::OnCanHandleFile(const wxString& FileName)
{
    wxFileName Normalized(GetProjectPath()+m_WxsFileName);
    Normalized.Normalize(wxPATH_NORM_DOTS);
    if ( Normalized.GetFullPath() == FileName )
    {
        return true;
    }
    if ( m_XrcFileName.empty() )
    {
        return false;
    }
    Normalized.Assign(GetProjectPath()+m_XrcFileName);
    Normalized.Normalize(wxPATH_NORM_DOTS);
    if ( Normalized.GetFullPath() == FileName )
    {
        return true;
    }
    return false;
}

wxString wxsItemRes::OnGetDeclarationFile()
{
    return m_HdrFileName;
}

bool wxsItemRes::OnGetUsingXRC()
{
    return !m_XrcFileName.empty();
}

bool wxsItemRes::OnGetCanBeMain()
{
    return m_CanBeMain;
}

bool wxsItemRes::CreateNewResource(NewResourceParams& Params)
{
    wxFileName HFN(GetProjectPath()+Params.Hdr);
    SetLanguage(wxsCodeMarks::IdFromExt(HFN.GetExt()));

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            SetResourceName(Params.Class);

            // Building arguments for constructor
            wxString CallArgs;
            wxString CtorArgs;
            wxString CtorArgsD;

            if ( Params.CtorParent )
            {
                CallArgs  << _T("parent");
                CtorArgs  << _T("wxWindow* parent");
                CtorArgsD << _T("wxWindow* parent");
                if ( Params.CtorParentDef )
                {
                    CtorArgsD << _T("=0");
                }
            }

            if ( Params.CtorId )
            {
                if ( !CtorArgs.IsEmpty() )
                {
                    CallArgs  << _T(",");
                    CtorArgs  << _T(",");
                    CtorArgsD << _T(",");
                }
                CallArgs  << _T("id");
                CtorArgs  << _T("wxWindowID id");
                CtorArgsD << _T("wxWindowID id");
                if ( Params.CtorIdDef )
                {
                    CtorArgsD << _T("=wxID_ANY");
                }
            }

            if ( Params.CtorPos )
            {
                if ( !CtorArgs.IsEmpty() )
                {
                    CallArgs  << _T(",");
                    CtorArgs  << _T(",");
                    CtorArgsD << _T(",");
                }
                CallArgs  << _T("pos");
                CtorArgs  << _T("const wxPoint& pos");
                CtorArgsD << _T("const wxPoint& pos");
                if ( Params.CtorPosDef )
                {
                    CtorArgsD << _T("=wxDefaultPosition");
                }
            }

            if ( Params.CtorSize )
            {
                if ( !CtorArgs.IsEmpty() )
                {
                    CallArgs  << _T(",");
                    CtorArgs  << _T(",");
                    CtorArgsD << _T(",");
                }
                CallArgs  << _T("size");
                CtorArgs  << _T("const wxSize& size");
                CtorArgsD << _T("const wxSize& size");
                if ( Params.CtorSizeDef )
                {
                    CtorArgsD << _T("=wxDefaultSize");
                }
            }

            wxString CtorArgsF = CtorArgs;

            if ( !Params.CustomCtorArgs.IsEmpty() )
            {
                if ( !CtorArgs.IsEmpty() )
                {
                    CtorArgs  << _T(",");
                    CtorArgsD << _T(",");
                }
                CtorArgs  << Params.CustomCtorArgs;
                CtorArgsD << Params.CustomCtorArgs;
            }

            // Generating code

            m_HdrFileName = Params.Hdr;
            if ( Params.GenHdr )
            {
                wxString Name = GetProjectPath()+Params.Hdr;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                wxString Guard = HFN.GetName().Upper() + _T("_H");
                wxString Header = CppEmptyHeader;
                wxString InitFuncDecl;
                wxString HeadersPch;
                if ( Params.UseInitFunc )
                {
                    InitFuncDecl <<
                        _T("\tprotected:\n\n")
                        _T("\t\tvoid ") << Params.InitFunc << _T("(") << CtorArgsF << _T(");\n\n");
                }
                if ( Params.UsePch && !Params.PchGuard.IsEmpty() )
                {
                    HeadersPch <<
                        _T("#ifndef ") + Params.PchGuard + _T("\n")
                        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("HeadersPCH"),Params.Class) + _T("\n")
                        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
                        _T("#endif\n");
                }
                Header.Replace(_T("$(CtorArgs)"),CtorArgsD);
                Header.Replace(_T("$(Guard)"),Guard);
                Header.Replace(_T("$(ClassName)"),Params.Class);
                Header.Replace(_T("$(BaseClassName)"),Params.BaseClass);
                Header.Replace(_T("$(InitFuncDecl)"),InitFuncDecl);
                Header.Replace(_T("$(HeadersPch)"),HeadersPch);

                wxString Scope = _T("");
                switch ( Params.ScopeMembers )
                {
                    case NewResourceParams::Private:   Scope = _T("\tprivate:\n\n"); break;
                    case NewResourceParams::Protected: Scope = _T("\tprotected:\n\n"); break;
                    default:;
                }
                Header.Replace(_T("$(MembersScope)"),Scope);
                Scope = _T("");
                if ( Params.ScopeIds != Params.ScopeMembers )
                {
                    switch ( Params.ScopeIds )
                    {
                        case NewResourceParams::Public:    Scope = _T("\tpublic:\n\n"); break;
                        case NewResourceParams::Private:   Scope = _T("\tprivate:\n\n"); break;
                        case NewResourceParams::Protected: Scope = _T("\tprotected:\n\n"); break;
                    }
                }
                Header.Replace(_T("$(IdsScope)"),Scope);
                Scope = _T("");
                if ( Params.ScopeHandlers != Params.ScopeIds )
                {
                    switch ( Params.ScopeHandlers )
                    {
                        case NewResourceParams::Public:    Scope = _T("\tpublic:\n\n"); break;
                        case NewResourceParams::Private:   Scope = _T("\tprivate:\n\n"); break;
                        case NewResourceParams::Protected: Scope = _T("\tprotected:\n\n"); break;
                    }
                }
                Header.Replace(_T("$(HandlersScope)"),Scope);

                // TODO: Use wxsCoder to save file's content, so it will
                //       have proper encoding and EOL stuff
                if ( !File.Write(Header) ) return false;
            }

            m_SrcFileName = Params.Src;
            if ( Params.GenSrc )
            {
                wxString Name = GetProjectPath()+Params.Src;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                HFN.MakeRelativeTo(wxFileName(Name).GetPath());
                wxString Include = HFN.GetFullPath(wxPATH_UNIX);
                wxString PchCode;
                if ( Params.UsePch )
                {
                    wxFileName PCH(GetProjectPath()+Params.Pch);
                    PCH.MakeRelativeTo(wxFileName(Name).GetPath());
                    PchCode << _T("#include \"") << PCH.GetFullPath(wxPATH_UNIX) << _T("\"\n");
                }
                wxString CtorInitCode;
                if ( Params.UseInitFunc )
                {
                    CtorInitCode << _T("\t") << Params.InitFunc << _T("(") << CallArgs << _T(");\n");
                    CtorInitCode << _T("}\n\n");
                    CtorInitCode << _T("void ") << Params.Class << _T("::") << Params.InitFunc << _T("(") << CtorArgsF << _T(")\n");
                    CtorInitCode << _T("{\n");
                }
                wxString IntHeadersPch;
                if ( Params.UsePch && !Params.PchGuard.IsEmpty() )
                {
                    IntHeadersPch <<
                        _T("#ifndef ") + Params.PchGuard + _T("\n")
                        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("InternalHeadersPCH"),Params.Class) + _T("\n")
                        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
                        _T("#endif\n");
                }

                wxString Source = CppEmptySource;
                Source.Replace(_T("$(PchCode)"),PchCode);
                Source.Replace(_T("$(CtorArgs)"),CtorArgs);
                Source.Replace(_T("$(Include)"),Include);
                Source.Replace(_T("$(ClassName)"),Params.Class);
                Source.Replace(_T("$(BaseClassName)"),Params.BaseClass);
                Source.Replace(_T("$(CtorInit)"),CtorInitCode);
                Source.Replace(_T("$(InternalHeadersPch)"),IntHeadersPch);
                // TODO: Use wxsCoder to save file's content, so it will
                //       have proper encoding and EOL stuff
                if ( !File.Write(Source) ) return false;
            }

            m_XrcFileName = Params.Xrc;
            if ( !Params.Xrc.IsEmpty() && Params.GenXrc )
            {
                wxString Name = GetProjectPath()+Params.Xrc;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                if ( !File.Write(EmptyXrc) ) return false;
            }

            if ( Params.Wxs.IsEmpty() )
            {
                // Searching for new wxs file name
                // TODO: Do not use constant folder name
                wxString WxsNameBase = _T("wxsmith");
                wxString WxsName;
                if ( !wxFileName::DirExists(GetProjectPath()+WxsNameBase) )
                {
                    if ( !wxFileName::Mkdir(GetProjectPath()+WxsNameBase,0777,wxPATH_MKDIR_FULL) )
                    {
                        return false;
                    }
                }
                WxsNameBase += _T("/") + Params.Class;
                WxsName = WxsNameBase + _T(".wxs");
                int Cnt = 0;
                for(;;)
                {
                    if ( !wxFileName::FileExists(GetProjectPath()+WxsName) &&
                         !wxFileName::DirExists(GetProjectPath()+WxsName) )
                    {
                        break;
                    }
                    Cnt++;
                    WxsName = wxString::Format(_T("%s%d.wxs"),WxsNameBase.wx_str(),Cnt);
                }

                m_WxsFileName = WxsName;
                Params.Wxs = WxsName;
                {
                    wxString Name = GetProjectPath()+m_WxsFileName;
                    wxFile File(Name,wxFile::write);
                    wxString Content = EmptyWxs;
                    Content.Replace(_T("$(ClassName)"),Params.Class);
                    Content.Replace(_T("$(BaseClassName)"),GetResourceType());
                    if ( !File.Write(Content) )
                    {
                        return false;
                    }
                }
            }
            else
            {
                m_WxsFileName = Params.Wxs;
            }
            m_UseForwardDeclarations = Params.UseFwdDecl;
            m_UseI18n = Params.UseI18n;
            return true;
        }

        default:;
    }

    SetLanguage(wxsUnknownLanguage);
    return false;
}

wxsItemRes::EditMode wxsItemRes::GetEditMode()
{
    if ( m_WxsFileName.empty() ) return File;
    if ( m_XrcFileName.empty() ) return Source;
    return Mixed;
}

int wxsItemRes::OnGetTreeIcon()
{
    const wxsItemInfo* Info = wxsItemFactory::GetInfo(GetResourceType());
    if ( Info ) return Info->TreeIconId;
    return wxsResource::OnGetTreeIcon();
}

wxsItemResData* wxsItemRes::BuildResData(wxsItemEditor* Editor)
{
    wxString ProjectPath = GetProjectPath();

    return new wxsItemResData(
        ProjectPath + GetWxsFileName(),
        ProjectPath + GetSrcFileName(),
        ProjectPath + GetHdrFileName(),
        GetXrcFileName().empty() ? _T("") : ProjectPath + GetXrcFileName(),
        GetResourceName(),
        GetResourceType(),
        GetLanguage(),
        m_UseForwardDeclarations,
        m_UseI18n,
        GetTreeItemId(),
        Editor,
        this);
}

bool wxsItemRes::OnDeleteCleanup(bool ShowDialog)
{
    bool PhysDeleteWXS = true;
    bool DeleteSources = false;
    bool PhysDeleteSources = false;

    if ( ShowDialog )
    {
        wxsDeleteItemRes Dlg;
        if ( Dlg.ShowModal() != wxID_OK )
        {
            return false;
        }
        PhysDeleteWXS = Dlg.m_PhysDeleteWXS->GetValue();
        DeleteSources = Dlg.m_DeleteSources->GetValue();
        PhysDeleteSources = Dlg.m_PhysDeleteSources->GetValue();
    }

    ProjectFile* Wxs = GetProject()->GetCBProject()->GetFileByFilename(m_WxsFileName,true);
    if ( Wxs )
    {
        GetProject()->GetCBProject()->RemoveFile(Wxs);
    }

    if ( PhysDeleteWXS )
    {
        wxRemoveFile(GetProjectPath() + m_WxsFileName);
    }

    if ( DeleteSources )
    {
        ProjectFile* Pf = GetProject()->GetCBProject()->GetFileByFilename(m_SrcFileName,true);
        if ( Pf )
        {
            GetProject()->GetCBProject()->RemoveFile(Pf);
        }
        Pf = GetProject()->GetCBProject()->GetFileByFilename(m_HdrFileName,true);
        if ( Pf )
        {
            GetProject()->GetCBProject()->RemoveFile(Pf);
        }

        if ( PhysDeleteSources )
        {
            wxRemoveFile(GetProjectPath() + m_SrcFileName);
            wxRemoveFile(GetProjectPath() + m_HdrFileName);
        }
    }

    Manager::Get()->GetProjectManager()->RebuildTree();

    // TODO: Check if we've deleted main resource of this app

    return true;
}

void wxsItemRes::OnFillPopupMenu(wxMenu* Menu)
{
    if ( !m_SrcFileName.IsEmpty() || !m_HdrFileName.IsEmpty() )
    {
        Menu->AppendSeparator();
        Menu->Append(GoToHeaderId,_("Go to header file"));
        Menu->Append(GoToSourceId,_("Go to source file"));
    }
}

bool wxsItemRes::OnPopupMenu(long Id)
{
    if ( Id == GoToHeaderId )
    {
        if ( !m_HdrFileName.IsEmpty() )
        {
            Manager::Get()->GetEditorManager()->Open(GetProjectPath() + m_HdrFileName);
        }
        return true;
    }
    if ( Id == GoToSourceId )
    {
        if ( !m_SrcFileName.IsEmpty() )
        {
            Manager::Get()->GetEditorManager()->Open(GetProjectPath() + m_SrcFileName);
        }
        return true;
    }
    return false;
}
