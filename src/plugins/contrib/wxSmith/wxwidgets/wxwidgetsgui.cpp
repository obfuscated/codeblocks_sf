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

#include "wxwidgetsgui.h"
#include "wxwidgetsguiconfigpanel.h"
#include "wxwidgetsguiappadoptingdlg.h"
#include "wxwidgetsres.h"
#include "../wxscoder.h"
#include "../wxsproject.h"

IMPLEMENT_CLASS(wxWidgetsGUI,wxsGUI)

wxWidgetsGUI::wxWidgetsGUI(wxsProject* Project):
    wxsGUI(_T("wxWidgets"),Project),
    m_AppFile(),
    m_LoadedResources(),
    m_MainResource(),
    m_CallInitAll(true),
    m_CallInitAllNecessary(true),
    m_AppLanguage(wxsCPP)
{
}

wxWidgetsGUI::~wxWidgetsGUI()
{
}

void wxWidgetsGUI::SetAppSourceFile(const wxString& NewAppFile)
{
    m_AppFile = NewAppFile;
    m_AppLanguage = wxsCodeMarks::IdFromExt(wxFileName(NewAppFile).GetExt());
}

cbConfigurationPanel* wxWidgetsGUI::OnBuildConfigurationPanel(wxWindow* Parent)
{
    return new wxWidgetsGUIConfigPanel(Parent,this);
}

void wxWidgetsGUI::OnRebuildApplicationCode()
{
    bool IsAnyXRC = false;
    wxWidgetsRes* MainResPtr = 0;

    size_t Count = GetProject()->GetResourcesCount();
    for ( size_t i=0; i<Count; i++ )
    {
        wxWidgetsRes* Res = wxDynamicCast(GetProject()->GetResource(i),wxWidgetsRes);
        if ( !Res ) continue;
        if ( Res->GetGUI() != GetName() ) continue;

        if ( m_MainResource==Res->GetResourceName() && Res->GetLanguage()==m_AppLanguage )
        {
            MainResPtr = Res;
        }

        if ( Res->OnGetUsingXRC() )
        {
            IsAnyXRC = true;
        }
    }

    wxString NewCode;
    switch ( m_AppLanguage )
    {
        case wxsCPP: NewCode = _T("\nbool wxsOK = true;\n")
                               _T("wxInitAllImageHandlers();\n"); break;
        default:;
    }

    bool InitAllXRCHandlers = m_CallInitAll && ( IsAnyXRC || !m_CallInitAllNecessary );
    if ( InitAllXRCHandlers )
    {
        switch ( m_AppLanguage )
        {
            case wxsCPP: NewCode.Append(_T("wxXmlResource::Get()->InitAllHandlers();\n")); break;
            default:;
        }
    }

    for ( size_t i = 0; i<m_LoadedResources.Count(); ++i )
    {
        switch ( m_AppLanguage )
        {
            case wxsCPP:
                NewCode.Append(_T("wxsOK = wxsOK && wxXmlResource::Get()->Load(_T(\""));
                NewCode.Append(m_LoadedResources[i]);
                NewCode.Append(_T("\"));\n"));
                break;
            default:;
        }
    }

    if ( MainResPtr )
    {
        switch ( m_AppLanguage )
        {
            case wxsCPP:
                NewCode << _T("if ( wxsOK )\n{\n");
                NewCode << MainResPtr->GetAppBuildingCode();
                NewCode << _T("}\n");
                break;
            default:;
        }
    }

    wxsCoder::Get()->AddCode(
        GetProjectPath() + m_AppFile,
        wxsCodeMarks::Beg(m_AppLanguage,_("AppInitialize")),
        wxsCodeMarks::End(m_AppLanguage),
        NewCode);

    NewCode = _T("\n");

    if ( MainResPtr )
    {
        switch ( m_AppLanguage )
        {
            case wxsCPP:
            {
                wxString IncludeFile = MainResPtr->GetDeclarationFile();
                wxFileName IncludeFileName(GetProjectPath()+IncludeFile);
                if ( IncludeFileName.MakeRelativeTo(GetProjectPath()) )
                {
                    // We will use unix path format. Because include is relative path
                    // we can to it in Win environment. Using Unix format will make
                    // sources more cross-platform
                    IncludeFile = IncludeFileName.GetFullPath(wxPATH_UNIX);
                }

                NewCode << _T("#include \"") << IncludeFile << _T("\"\n");
                break;
            }
            default:;
        }
    }
    if ( InitAllXRCHandlers || m_LoadedResources.Count() )
    {
        switch ( m_AppLanguage )
        {
            case wxsCPP: NewCode.Append(_T("#include <wx/xrc/xmlres.h>\n")); break;
            default:;
        }
    }

    switch ( m_AppLanguage )
    {
        case wxsCPP: NewCode.Append(_T("#include <wx/image.h>\n")); break;
        default:;
    }

    wxsCoder::Get()->AddCode(
        GetProjectPath() + m_AppFile,
        wxsCodeMarks::Beg(m_AppLanguage,_T("AppHeaders")),
        wxsCodeMarks::End(m_AppLanguage),
        NewCode);

}

bool wxWidgetsGUI::OnCheckIfApplicationManaged()
{
    return IsAppSourceManaged(m_AppFile,m_AppLanguage);
}

bool wxWidgetsGUI::OnCreateApplicationBinding()
{
    wxWidgetsGUIAppAdoptingDlg Dlg(0,this);
    Dlg.ShowModal();
    return OnCheckIfApplicationManaged();
}

void wxWidgetsGUI::OnReadConfig(TiXmlElement* element)
{
    m_AppFile      = cbC2U(element->Attribute("src"));
    m_MainResource = cbC2U(element->Attribute("main"));
    wxString InAll = cbC2U(element->Attribute("init_handlers"));
    if ( InAll == _T("never") )
    {
        m_CallInitAll = false;
        m_CallInitAllNecessary = false;
    }
    else if ( InAll == _T("always") )
    {
        m_CallInitAll = true;
        m_CallInitAllNecessary = false;
    }
    else
    {
        m_CallInitAll = true;
        m_CallInitAllNecessary = true;
    }

    wxString Lang  = cbC2U(element->Attribute("language"));
    m_AppLanguage = wxsCodeMarks::Id(Lang);

    TiXmlElement* LoadRes = element->FirstChildElement("load_resource");
    while ( LoadRes )
    {
        wxString FileName = cbC2U(LoadRes->Attribute("file"));
        if ( !FileName.empty() )
        {
            m_LoadedResources.Add(FileName);
        }
        LoadRes = LoadRes->NextSiblingElement("load_resource");
    }

}

void wxWidgetsGUI::OnWriteConfig(TiXmlElement* element)
{
    element->SetAttribute("src",cbU2C(m_AppFile));
    element->SetAttribute("main",cbU2C(m_MainResource));
    if ( m_CallInitAll && m_CallInitAllNecessary )
    {
        element->SetAttribute("init_handlers","necessary");
    }
    else if ( m_CallInitAll )
    {
        element->SetAttribute("init_handlers","always");
    }
    else
    {
        element->SetAttribute("init_handlers","never");
    }

    element->SetAttribute("language",cbU2C(wxsCodeMarks::Name(m_AppLanguage)));

    for ( size_t i=0; i<m_LoadedResources.GetCount(); ++i )
    {
        TiXmlElement* LoadRes = element->InsertEndChild(TiXmlElement("load_resource"))->ToElement();
        LoadRes->SetAttribute("file",cbU2C(m_LoadedResources[i]));
    }
}

bool wxWidgetsGUI::IsAppSourceManaged(const wxString& FileName,wxsCodingLang Lang)
{
    if ( FileName.empty() ) return false;

    if ( wxsCoder::Get()->GetCode(
            GetProjectPath()+FileName,
            wxsCodeMarks::Beg(Lang,_T("AppInitialize")),
            wxsCodeMarks::End(Lang)
            ).empty() )
    {
        return false;
    }

    if ( wxsCoder::Get()->GetCode(
            GetProjectPath()+FileName,
            wxsCodeMarks::Beg(Lang,_T("AppHeaders")),
            wxsCodeMarks::End(Lang)
            ).empty() )
    {
        return false;
    }
    return true;
}

bool wxWidgetsGUI::ScanForApp(ProjectFile* File)
{
    // Detcting language of this file
    wxsCodingLang Lang = wxsCodeMarks::IdFromExt(File->file.GetExt());
    if ( Lang == wxsUnknownLanguage ) return false;

    // If this file is already managed, returning true
    if ( IsAppSourceManaged(File->relativeFilename,Lang) ) return true;

    // Fetching source code
    wxFontEncoding Encoding;
    bool UseBOM;
    wxString Source = wxsCoder::Get()->GetFullCode(File->file.GetFullPath(),Encoding,UseBOM);

    // Searching for app class
    if ( GetAppClassName(Source,Lang).empty() ) return false;

    switch ( Lang )
    {
        case wxsCPP:
        {
            // Searching for OnInit() function
            int Pos = Source.Find(_T("OnInit"));
            if ( Pos < 0 ) return false;

            // TODO: Do extra checks of this OnInit
            return true;
        }
        default:;
    }
    return false;
}

bool wxWidgetsGUI::AddSmithToApp(const wxString& RelativeFileName,wxsCodingLang Lang)
{
    if ( !IsAppSourceManaged(RelativeFileName,Lang) )
    {
        wxString FullPath = GetProjectPath()+RelativeFileName;
        // Need to create some extra blocks of code.
        wxFontEncoding Encoding;
        bool UseBOM;
        wxString Source = wxsCoder::Get()->GetFullCode(FullPath,Encoding,UseBOM);
        if ( Source.empty() ) return false;

        switch ( Lang )
        {
            case wxsCPP:
            {
                // First thing we need is to add new includes section
                // It is added right before IMPLEMENT_APP() macro
                int Pos = Source.Find(_T("IMPLEMENT_APP"));
                while ( Pos>0 && Source[Pos]!=_T('\n') ) Pos--;
                if ( Pos>0 ) Pos++;

                // TODO: Get valid EOL mode
                Source.insert(Pos,
                    wxsCodeMarks::Beg(wxsCPP,_T("AppHeaders")) + _T("\n") +
                    wxsCodeMarks::End(wxsCPP) + _T("\n\n"));

                wxString ClassName = GetAppClassName(Source,wxsCPP);
                if ( ClassName.empty() ) return false;

                // Searching for ::OnInit member
                wxString SourceCpy = Source;
                Pos = 0;
                while ( !SourceCpy.empty() )
                {
                    int ClassPos = SourceCpy.Find(ClassName);
                    if ( ClassPos<0 ) return false;
                    ClassPos += ClassName.Length();
                    Pos += ClassPos;
                    SourceCpy.Remove(0,ClassPos);
                    int MemberPos = 0;
                    while ( IsWhite(SourceCpy,MemberPos) ) MemberPos++;
                    if ( MemberPos+1>=(int)(SourceCpy.Length()) ) continue;
                    if ( SourceCpy[MemberPos]!=_T(':') && SourceCpy[MemberPos+1]!=_T(':') ) continue;
                    MemberPos+=2;
                    while ( IsWhite(SourceCpy,MemberPos) ) MemberPos++;
                    if ( SourceCpy.Mid(MemberPos,6) != _T("OnInit") ) continue;
                    MemberPos += 6;
                    while ( IsWhite(SourceCpy,MemberPos) ) MemberPos++;
                    if ( !Match(SourceCpy,MemberPos,_T('(')) ) continue;
                    MemberPos++;
                    while ( IsWhite(SourceCpy,MemberPos) ) MemberPos++;
                    if ( !Match(SourceCpy,MemberPos,_T(')')) ) continue;
                    MemberPos++;
                    while ( IsWhite(SourceCpy,MemberPos) ) MemberPos++;
                    if ( !Match(SourceCpy,MemberPos,_T('{')) ) continue;
                    MemberPos++;
                    // Ok, we're at function beginning, calculating indentation of {
                    Pos += MemberPos;
                    break;
                }

                if ( SourceCpy.empty() ) return false;

                // Calculating indentation of source
                int IndentPos = Pos;
                while ( IndentPos>0 && Source[IndentPos-1]!=_T('\n') && Source[IndentPos-1]!='\r' ) IndentPos--;
                wxString Indent;
                while ( IndentPos<Pos && (Source[IndentPos]==_T(' ') || Source[IndentPos]==_T('\t')) ) Indent += Source[IndentPos++];
                Indent.Append(_T("\t"));

                // Inserting AppInitializeBlock
                Source = Source(0,Pos) +
                    _T("\n") +
                    Indent + wxsCodeMarks::Beg(wxsCPP,_T("AppInitialize")) + _T("\n") +
                    Indent + wxsCodeMarks::End(wxsCPP) + _T("\n") +
                    Indent + _T("return wxsOK;\n") +
                    Indent
                    + Source.Mid(Pos);

                // Writing new source back to files / editor
                wxsCoder::Get()->PutFullCode(FullPath,Source,Encoding,UseBOM);
                break;
            }
            default:;
        }
    }

    // Applying default configuration
    m_AppFile = RelativeFileName;
    m_LoadedResources.Clear();
    m_MainResource.Clear();
    m_CallInitAll = true;
    m_CallInitAllNecessary = true;
    m_AppLanguage = Lang;
    OnRebuildApplicationCode();
    NotifyChange();
    return true;
}

wxString wxWidgetsGUI::GetAppClassName(const wxString& Source,wxsCodingLang Lang)
{
    switch ( Lang )
    {
        case wxsCPP:
        {
            // Doing some trick - searching for IMPLEMENT_APP macro followed
            // by '(' and class name - here we can fetch name of application class
            int Pos = Source.Find(_T("IMPLEMENT_APP"));
            if ( Pos<0 ) return wxEmptyString;
            Pos += 13;// strlen("IMPLEMENT_APP")
            while ( IsWhite(Source,Pos) ) Pos++;
            if ( Pos >= (int)Source.Length() ) return wxEmptyString;
            if ( Source[Pos++] != _T('(') ) return wxEmptyString;
            while ( IsWhite(Source,Pos) ) Pos++;
            wxString ClassName;
            static const wxString AllowedChars(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"));
            while ( (Pos < (int)Source.Length()) && (AllowedChars.Find(Source[Pos])>=0) )
            {
                ClassName += Source[Pos];
                Pos++;
            }
            while ( IsWhite(Source,Pos) ) Pos++;
            if ( Pos >= (int)Source.Length() ) return wxEmptyString;
            if ( Source[Pos] != _T(')') ) return wxEmptyString;
            return ClassName;
        }
        default:;
    }
    return wxEmptyString;
}

bool wxWidgetsGUI::CreateNewApp(const wxString& FileName)
{
    wxsCodingLang Lang = wxsCodeMarks::IdFromExt(wxFileName(FileName).GetExt());
    if ( Lang == wxsUnknownLanguage ) return false;

    wxFile Fl(FileName,wxFile::write);
    if ( !Fl.IsOpened() )
    {
        wxMessageBox(_("Couldn't overwrite file"));
        return false;
    }

    switch ( Lang )
    {
        case wxsCPP:
        {
            Fl.Write(
                _T("#include <wx/wxprec.h>\n")
                _T("\n")
                _T("#ifdef __BORLANDC__\n")
                _T("    #pragma hdrstop\n")
                _T("#endif\n")
                _T("\n")
                _T("#ifndef WX_PRECOMP\n")
                _T("    #include <wx/app.h>\n")
                _T("#endif\n")
                _T("\n")
                _T("//(*AppHeaders\n")
                _T("//*)\n")
                _T("\n")
                _T("\n")
                _T("class MyApp : public wxApp\n")
                _T("{\n")
                _T("    public:\n")
                _T("        virtual bool OnInit();\n")
                _T("};\n")
                _T("\n")
                _T("IMPLEMENT_APP(MyApp);\n")
                _T("\n")
                _T("bool MyApp::OnInit()\n")
                _T("{\n")
                _T("    //(*AppInitialize\n")
                _T("    //*)\n")
                _T("    return wxsOK;\n")
                _T("}\n")
                _T("\n"));
            break;
        }
        default:;
    }

    // Applying default configuration
    wxFileName FN(FileName);
    FN.MakeRelativeTo(GetProjectPath());
    m_AppFile = FN.GetFullPath();
    m_LoadedResources.Clear();
    m_MainResource.Clear();
    m_CallInitAll = true;
    m_CallInitAllNecessary = true;
    m_AppLanguage = Lang;
    OnRebuildApplicationCode();
    return true;

}

void wxWidgetsGUI::EnumerateMainResources(wxArrayString& Names)
{
    int Count = GetProject()->GetResourcesCount();
    for ( int i=0; i<Count; i++ )
    {
        wxWidgetsRes* Res = wxDynamicCast(GetProject()->GetResource(i),wxWidgetsRes);
        if ( Res == 0 ) continue;
        if ( Res->GetGUI() != GetName() ) continue;
        if ( !Res->OnGetCanBeMain() ) continue;
        if ( Res->GetLanguage() != m_AppLanguage ) continue;
        Names.Add(Res->GetResourceName());
    }
}

inline bool wxWidgetsGUI::IsWhite(wxChar Ch)
{
    return (Ch==_T(' ')) || (Ch==_T('\n')) || (Ch==_T('\r')) || (Ch==_T('\t'));
}

inline bool wxWidgetsGUI::IsWhite(const wxString& Str,int Pos)
{
    if ( Pos>=(int)Str.Length() ) return false;
    return IsWhite(Str[Pos]);
}

inline bool wxWidgetsGUI::Match(const wxString& Str,int Pos,wxChar Ch)
{
    if ( Pos>=(int)Str.Length() ) return false;
    return Str[Pos]==Ch;
}

static wxsRegisterGUI<wxWidgetsGUI> Reg(_T("wxWidgets"));
