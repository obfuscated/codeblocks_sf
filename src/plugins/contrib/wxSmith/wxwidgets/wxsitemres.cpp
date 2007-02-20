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

#include "wxsitemres.h"
#include "wxsitemeditor.h"
#include "wxsitemfactory.h"
#include "../wxscoder.h"

IMPLEMENT_CLASS(wxsItemRes,wxWidgetsRes)

namespace
{
    const wxString CppEmptySource =
        _T("#include \"$(Include)\"\n")
        _T("\n")
        + wxsCodeMarks::Beg(wxsCPP,_T("InternalHeaders"),_T("$(ClassName)")) + _T("\n") +
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        + wxsCodeMarks::Beg(wxsCPP,_T("IdInit"),_T("$(ClassName)")) + _T("\n") +
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n")
        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("EventTable"),_T("$(ClassName)")) + _T("\n")
        _T("\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("END_EVENT_TABLE()\n")
        _T("\n")
        _T("$(ClassName)::$(ClassName)(wxWindow* parent,wxWindowID id)\n")
        _T("{\n")
        _T("\t") + wxsCodeMarks::Beg(wxsCPP,_T("Initialize"),_T("$(ClassName)")) + _T("\n")
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
        _T("#include <wx/wxprec.h>\n")
        _T("\n")
        _T("#ifdef __BORLANDC__\n")
        _T("    #pragma hdrstop\n")
        _T("#endif\n")
        _T("\n")
        + wxsCodeMarks::Beg(wxsCPP,_T("Headers"),_T("$(ClassName)")) + _T("\n")
        + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("class $(ClassName): public $(BaseClassName)\n")
        _T("{\n")
        _T("\tpublic:\n")
        _T("\n")
        _T("\t\t$(ClassName)(wxWindow* parent,wxWindowID id = -1);\n")
        _T("\t\tvirtual ~$(ClassName)();\n")
        _T("\n")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Identifiers"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("\tprotected:\n")
        _T("\n")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Handlers"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("\t\t") + wxsCodeMarks::Beg(wxsCPP,_T("Declarations"),_T("$(ClassName)")) + _T("\n")
        _T("\t\t") + wxsCodeMarks::End(wxsCPP) + _T("\n")
        _T("\n")
        _T("\tprivate:\n")
        _T("\n")
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
}

wxsItemRes::wxsItemRes(wxsProject* Owner,const wxString& Type,bool CanBeMain):
    wxWidgetsRes(Owner,Type),
    m_WxsFileName(wxEmptyString),
    m_SrcFileName(wxEmptyString),
    m_HdrFileName(wxEmptyString),
    m_XrcFileName(wxEmptyString),
    m_CanBeMain(CanBeMain)
{
}


wxsItemRes::wxsItemRes(const wxString& FileName,const TiXmlElement* XrcElem,const wxString& Type):
    wxWidgetsRes(NULL,Type),
    m_WxsFileName(wxEmptyString),
    m_SrcFileName(wxEmptyString),
    m_HdrFileName(wxEmptyString),
    m_XrcFileName(FileName)
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
    return !m_WxsFileName.empty() &&
           !m_SrcFileName.empty() &&
           !m_HdrFileName.empty();
    // m_XrcFileName may be empty because it's not used when generating full source code
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

bool wxsItemRes::CreateNewResource(const wxString& Class,const wxString& Src, bool GenSrc,const wxString& Hdr, bool GenHdr,const wxString& Xrc, bool GenXrc)
{
    wxFileName HFN(GetProjectPath()+Hdr);
    SetLanguage(wxsCodeMarks::IdFromExt(HFN.GetExt()));

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            SetResourceName(Class);

            m_HdrFileName = Hdr;
            if ( GenHdr )
            {
                wxString Name = GetProjectPath()+Hdr;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                wxString Guard = HFN.GetName().Upper() + _T("_H");
                wxString Header = CppEmptyHeader;
                Header.Replace(_T("$(Guard)"),Guard);
                Header.Replace(_T("$(ClassName)"),Class);
                Header.Replace(_T("$(BaseClassName)"),GetResourceType());
                if ( !File.Write(Header) ) return false;
            }

            m_SrcFileName = Src;
            if ( GenSrc )
            {
                wxString Name = GetProjectPath()+Src;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                HFN.MakeRelativeTo(wxFileName(Name).GetPath());
                wxString Include = HFN.GetFullPath(wxPATH_UNIX);
                wxString Source = CppEmptySource;
                Source.Replace(_T("$(Include)"),Include);
                Source.Replace(_T("$(ClassName)"),Class);
                Source.Replace(_T("$(BaseClassName)"),GetResourceType());
                if ( !File.Write(Source) ) return false;
            }

            m_XrcFileName = Xrc;
            if ( !Xrc.empty() && GenXrc )
            {
                wxString Name = GetProjectPath()+Xrc;
                wxFileName::Mkdir(wxFileName(Name).GetPath(),0777,wxPATH_MKDIR_FULL);
                wxFile File(Name,wxFile::write);
                if ( !File.Write(EmptyXrc) ) return false;
            }

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
            WxsNameBase += _T("/") + Class.Lower();
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
                WxsName = wxString::Format(_T("%s%d.wxs"),WxsNameBase.c_str(),Cnt);
            }

            m_WxsFileName = WxsName;
            {
                wxString Name = GetProjectPath()+m_WxsFileName;
                wxFile File(Name,wxFile::write);
                wxString Content = EmptyWxs;
                Content.Replace(_T("$(ClassName)"),Class);
                Content.Replace(_T("$(BaseClassName)"),GetResourceType());
                if ( !File.Write(Content) )
                {
                    return false;
                }
            }
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
