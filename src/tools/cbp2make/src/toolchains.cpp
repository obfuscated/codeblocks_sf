/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)
    Copyright (C) 2014      Sergey "dmpas" Batanov (sergey.batanov (at) dmpas (dot) ru)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#include "toolchains.h"
#include "stlconvert.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CToolChain::CToolChain(const CString& Alias)
{
    Clear();
    m_Alias = Alias;
}

CToolChain::CToolChain(const CToolChain& ToolChain)
{
    Clear();
    Assign(ToolChain);
}

CToolChain::~CToolChain(void)
{
    Clear();
}

void CToolChain::Clear(void)
{
    m_Platform = CPlatform::OS_Other;
    m_Alias.Clear();
    m_GenericSwitch     = "-";
    m_DefineSwitch      = "-D";
    m_IncludeDirSwitch  = "-I";
    m_LibraryDirSwitch  = "-L";
    m_LinkLibrarySwitch = "-l";
}

CString CToolChain::sw(const CString& ASwitch) const
{
    if (!ASwitch.IsEmpty()) {
        if (LeftStr(ASwitch,m_GenericSwitch.GetLength())==m_GenericSwitch) {
            return m_GenericSwitch+ASwitch;
        }
    }
    return ASwitch;
}

CBuildTool *CToolChain::CreateBuildTool(const CBuildTool::ToolType Type)
{
    switch (Type) {
    default:
    case CBuildTool::btCount:
    case CBuildTool::btOther: {
        return 0; //break;
    }
    case CBuildTool::btPreprocessor: {
        CPreprocessor *bt = new CPreprocessor;
        m_Preprocessors.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btCompiler: {
        CCompiler *bt = new CCompiler;
        m_Compilers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btAssembler: {
        CAssembler *bt = new CAssembler;
        m_Assemblers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btStaticLinker: {
        CStaticLinker *bt = new CStaticLinker;
        m_StaticLinkers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btDynamicLinker: {
        CDynamicLinker *bt = new CDynamicLinker;
        m_DynamicLinkers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btExecutableLinker: {
        CExecutableLinker *bt = new CExecutableLinker;
        m_ExecutableLinkers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
    case CBuildTool::btResourceCompiler: {
        CResourceCompiler *bt = new CResourceCompiler;
        m_ResourceCompilers.push_back(bt);
        m_BuildTools.push_back(bt);
        return bt; //break;
    }
        //case CBuildTool::btBuildManager,
    }
    return 0;
}

void CToolChain::Assign(const CToolChain& ToolChain)
{
    m_Platform = ToolChain.m_Platform;
    m_Alias    = ToolChain.m_Alias;
    m_GenericSwitch     = ToolChain.m_GenericSwitch;
    m_DefineSwitch      = ToolChain.m_DefineSwitch;
    m_IncludeDirSwitch  = ToolChain.m_IncludeDirSwitch;
    m_LibraryDirSwitch  = ToolChain.m_LibraryDirSwitch;
    m_LinkLibrarySwitch = ToolChain.m_LinkLibrarySwitch;

    for (size_t i = 0; i < ToolChain.m_Preprocessors.size(); i++) {
        m_Preprocessors.push_back(ToolChain.m_Preprocessors[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_Assemblers.size(); i++) {
        m_Assemblers.push_back(ToolChain.m_Assemblers[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_Compilers.size(); i++) {
        m_Compilers.push_back(ToolChain.m_Compilers[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_ResourceCompilers.size(); i++) {
        m_ResourceCompilers.push_back(ToolChain.m_ResourceCompilers[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_StaticLinkers.size(); i++) {
        m_StaticLinkers.push_back(ToolChain.m_StaticLinkers[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_DynamicLinkers.size(); i++) {
        m_DynamicLinkers.push_back(ToolChain.m_DynamicLinkers[i]->CreateInstance());
    }
    for (size_t i = 0; i < ToolChain.m_ExecutableLinkers.size(); i++) {
        m_ExecutableLinkers.push_back(ToolChain.m_ExecutableLinkers[i]->CreateInstance());
    }
    GatherBuildTools();
}

CToolChain *CToolChain::CreateInstance(void) const
{
    return new CToolChain(*this);
}

void CToolChain::Reset(const CPlatform::OS_Type OS)
{
    if (Supports(OS)) m_Platform = OS;
    else m_Platform = CPlatform::OS_Other;
    for (size_t i = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        //bt->Show();
        bt->Reset(OS);
    }
}

bool CToolChain::Supports(const CPlatform::OS_Type OS) const
{
    return ((CPlatform::OS_Unix==OS)||
            (CPlatform::OS_Windows==OS)||
            (CPlatform::OS_MSys==OS)||
            (CPlatform::OS_Mac==OS));
}

void CToolChain::Read(const TiXmlElement *Root, const CString& Name, CString& Value)
{
    TiXmlNode *_option = (TiXmlNode *)Root->FirstChild("option");
    while (0!=_option) {
        TiXmlElement* option = _option->ToElement();
        //if (strcmp(option->Value(),"option")!=0) break;
        if (0!=option) {
            char *value = 0;
            if ((value = (char *)option->Attribute(Name.GetCString()))) {
                Value = value;
                break;
            }
        }
        _option = (TiXmlNode *)Root->IterateChildren(_option);
    } // option
}

void CToolChain::Read(const TiXmlElement *Root, const CString& Name, bool& Value)
{
    CString value;
    Read(Root,Name,value);
    Value = StringToBoolean(value);
}

void CToolChain::Read(const TiXmlElement *ToolChainRoot)
{
    char *value = 0;
    if ((value = (char *)ToolChainRoot->Attribute("alias"))) {
        m_Alias = value;
    }
    if ((value = (char *)ToolChainRoot->Attribute("platform"))) {
        CString platform = value;
        m_Platform = CPlatform::OS(platform);
    }
    /*
    if ((value = (char *)ToolChainRoot->Attribute("")))
    {
     m_ = value;
    }
    */
    TiXmlNode *_tool_root = (TiXmlNode *)(ToolChainRoot->FirstChild("tool"));
    while (0!=_tool_root) {
        const TiXmlElement *tool_root = _tool_root->ToElement();
        if (0!=tool_root) {
            char *value = 0;
            CString type_name, alias;
            if ((value = (char *)tool_root->Attribute("type"))) {
                type_name = value;
            }
            if ((value = (char *)tool_root->Attribute("alias"))) {
                alias = value;
            }
            Read(ToolChainRoot, "generic_switch", m_GenericSwitch);
            Read(ToolChainRoot, "define_switch", m_DefineSwitch);
            Read(ToolChainRoot, "include_dir_switch", m_IncludeDirSwitch);
            Read(ToolChainRoot, "library_dir_switch", m_LibraryDirSwitch);
            Read(ToolChainRoot, "link_library_switch", m_LinkLibrarySwitch);

            bool custom_tool = true;
            for (size_t i = 0; i < m_BuildTools.size(); i++) {
                CBuildTool *bt = m_BuildTools[i];
                if (bt->Supports(m_Platform)) {
                    if (bt->Alias() == alias) {
                        bt->Read(tool_root);
                        custom_tool = false;
                    }
                }
            }
            if (custom_tool) {
                CBuildTool::ToolType tool_type = CBuildTool::Type(type_name);
                CBuildTool *bt = CreateBuildTool(tool_type);
                if (0!=bt) {
                    bt->Read(tool_root);
                }
            }
        }
        _tool_root = (TiXmlNode *)(ToolChainRoot->IterateChildren(_tool_root));
    }
}

void CToolChain::Write(TiXmlElement *Root, const CString& Name, const CString& Value)
{
    TiXmlElement *option = new TiXmlElement("option");
    option->SetAttribute(Name.GetCString(),Value.GetCString());
    Root->LinkEndChild(option);
}

void CToolChain::Write(TiXmlElement *Root, const CString& Name, const bool Value)
{
    TiXmlElement *option = new TiXmlElement("option");
    option->SetAttribute(Name.GetCString(),Value);
    Root->LinkEndChild(option);
}

void CToolChain::Write(TiXmlElement *ToolChainRoot)
{
    ToolChainRoot->SetAttribute("platform",CPlatform::Name(m_Platform).GetCString());
    ToolChainRoot->SetAttribute("alias",m_Alias.GetCString());
    Write(ToolChainRoot, "generic_switch", m_GenericSwitch);
    Write(ToolChainRoot, "define_switch", m_DefineSwitch);
    Write(ToolChainRoot, "include_dir_switch", m_IncludeDirSwitch);
    Write(ToolChainRoot, "library_dir_switch", m_LibraryDirSwitch);
    Write(ToolChainRoot, "link_library_switch", m_LinkLibrarySwitch);
    //ToolChainRoot->SetAttribute("",m_.GetCString());
    for (size_t i = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        if (bt->Supports(m_Platform)) {
            TiXmlElement *bt_root = new TiXmlElement("tool");
            bt->Write(bt_root);
            ToolChainRoot->LinkEndChild(bt_root);
        }
    }
}

void CToolChain::Show(void)
{
    std::cout<<"Platform: "<<CPlatform::Name(m_Platform).GetString()
             /*
             <<" (supported:";
             for (int i = (int)CPlatform::OS_Other; i < (int)CPlatform::OS_Count; i++)
             {
              if (Supports((CPlatform::OS_Type)i))
              {
               std::cout<<" "<<CPlatform::Name((CPlatform::OS_Type)i).GetCString();
              }
             }
             std::cout<<")"
             */
             <<std::endl;
    std::cout<<"Alias: "<<m_Alias.GetString()<<std::endl;
    std::cout<<"Generic switch: "<<m_GenericSwitch.GetCString()<<std::endl;
    std::cout<<"Define switch: "<<m_DefineSwitch.GetCString()<<std::endl;
    std::cout<<"Include directory switch: "<<m_IncludeDirSwitch.GetCString()<<std::endl;
    std::cout<<"Library directory switch: "<<m_LibraryDirSwitch.GetCString()<<std::endl;
    std::cout<<"Link library switch: "<<m_LinkLibrarySwitch.GetCString()<<std::endl;

//std::cout<<": "<<m_.GetString()<<std::endl;
    int bt_count = 0;
    for (size_t i = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        if (bt->Supports(m_Platform)) bt_count++;
    }
    std::cout<<"Toolchain has "<<bt_count<<" configured build tool(s)."<<std::endl;
    for (size_t i = 0, j = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        if (bt->Supports(m_Platform)) {
            std::cout<<"Build tool #"<<++j<<std::endl;
            bt->Show();
        }
        std::cout<<std::endl;
    }
}

void CToolChain::GatherBuildTools(std::vector<CBuildTool *>* Source,
                                  std::vector<CBuildTool *>* Target)
{
    for (size_t i = 0; i < Source->size(); i++) {
        Target->push_back(Source->at(i));
    }
}

void CToolChain::GatherBuildTools(void)
{
    m_BuildTools.clear();
    GatherBuildTools((std::vector<CBuildTool *>*)&m_Preprocessors,    &m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_Assemblers,       &m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_Compilers,        &m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_StaticLinkers,    &m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_DynamicLinkers,   &m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_ExecutableLinkers,&m_BuildTools);
    GatherBuildTools((std::vector<CBuildTool *>*)&m_ResourceCompilers,&m_BuildTools);
}

CBuildTool *CToolChain::FindBuildTool(const CString& FileExtension,
                                      const std::vector<CBuildTool *>* Tools)
{
    for (size_t i = 0; i < Tools->size(); i++) {
        CBuildTool *bt = Tools->at(i);
        if (bt->ExpectedSourceExtension(FileExtension)) return bt;
    }
    return 0;
}

std::vector<CBuildTool *>* CToolChain::GetTools(const CBuildTool::ToolType Type)
{
    switch (Type) {
    case CBuildTool::btPreprocessor:
        return (std::vector<CBuildTool *>*)&m_Preprocessors;
    case CBuildTool::btAssembler:
        return (std::vector<CBuildTool *>*)&m_Assemblers;
    case CBuildTool::btCompiler:
        return (std::vector<CBuildTool *>*)&m_Compilers;
    case CBuildTool::btStaticLinker:
        return (std::vector<CBuildTool *>*)&m_StaticLinkers;
    case CBuildTool::btDynamicLinker:
        return (std::vector<CBuildTool *>*)&m_DynamicLinkers;
    case CBuildTool::btExecutableLinker:
        return (std::vector<CBuildTool *>*)&m_ExecutableLinkers;
    case CBuildTool::btResourceCompiler:
        return (std::vector<CBuildTool *>*)&m_ResourceCompilers;
    //case CBuildTool::btBuildManager,
    case CBuildTool::btOther: // fall-through
    case CBuildTool::btCount: // fall-through
    default:
        break;
    }
    return &m_BuildTools;
}

size_t CToolChain::ToolsCount(const CBuildTool::ToolType Type)
{
    return GetTools(Type)->size();
}

CBuildTool *CToolChain::GetBuildTool(const size_t index, const CBuildTool::ToolType Type)
{
    std::vector<CBuildTool *>* tools = GetTools(Type);
    if (index<tools->size()) {
        return tools->at(index);
    }
    return 0;
}

CBuildTool *CToolChain::FindBuildToolByName(const CString& ToolName)
{
    for (size_t i = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        if (bt->Alias()==ToolName) return bt;
    }
    return 0;
}

CBuildTool *CToolChain::FindBuildTool(const CString& FileExtension)
{
    return dynamic_cast<CBuildTool *>(FindBuildTool(FileExtension,(std::vector<CBuildTool *>*)&m_BuildTools));
}

CAssembler *CToolChain::FindAssembler(const CString& FileExtension)
{
    return dynamic_cast<CAssembler *>(FindBuildTool(FileExtension,(std::vector<CBuildTool *>*)&m_Assemblers));
}

CCompiler *CToolChain::FindCompiler(const CString& FileExtension)
{
    CCompiler *result = 0;

    result = dynamic_cast<CCompiler *>(FindBuildTool(FileExtension,(std::vector<CBuildTool *>*)&m_Compilers));
    if (0 != result)
        return result;

    result = dynamic_cast<CCompiler *>(FindBuildTool(FileExtension,(std::vector<CBuildTool *>*)&m_Assemblers));
    if (0 != result)
        return result;

    result = dynamic_cast<CCompiler *>(FindBuildTool(FileExtension,(std::vector<CBuildTool *>*)&m_ResourceCompilers));
    return result;
}

void CToolChain::RemoveTool(const CBuildTool* BuildTool, std::vector<CBuildTool *>* Tools)
{
    Tools->erase(std::find(Tools->begin(),Tools->end(),BuildTool));
    delete BuildTool;
}

void CToolChain::RemoveTool(const CBuildTool* BuildTool)
{
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_Preprocessors);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_Assemblers);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_Compilers);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_StaticLinkers);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_DynamicLinkers);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_ExecutableLinkers);
    RemoveTool(BuildTool,(std::vector<CBuildTool *>*)&m_ResourceCompilers);
}

bool CToolChain::RemoveToolByName(const CString& ToolName)
{
    for (size_t i = 0; i < m_BuildTools.size(); i++) {
        CBuildTool *bt = m_BuildTools[i];
        if (bt->Alias()==ToolName) {
            RemoveTool(bt);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

CGNUToolChain::CGNUToolChain(void): CToolChain("gcc")
{
    m_Compilers.push_back(new CGNUCCompiler);
    m_Compilers.push_back(new CGNUCppCompiler);
//
    m_ResourceCompilers.push_back(new CGNUWindowsResourceCompiler);
//
    m_StaticLinkers.push_back(new CGNUStaticLinker);
//
    m_DynamicLinkers.push_back(new CGNUDynamicLinker);
//
    m_ExecutableLinkers.push_back(new CGNUExecutableLinker);
//
    GatherBuildTools();
}

CGNUToolChain::~CGNUToolChain(void)
{
//
}

CToolChain *CGNUToolChain::CreateInstance(void) const
{
    return new CGNUToolChain(*this);
}

void CGNUToolChain::Reset(const CPlatform::OS_Type OS)
{
    CToolChain::Reset(OS);
    m_GenericSwitch = "-";
    m_DefineSwitch = "-D";
    m_IncludeDirSwitch = "-I";
    m_LibraryDirSwitch = "-L";
    m_LinkLibrarySwitch = "-l";
}

//------------------------------------------------------------------------------

CBorlandToolChain::CBorlandToolChain(void): CToolChain("bcc")
{
    m_Compilers.push_back(new CBorlandCppCompiler);
//m_ResourceCompilers.push_back(new CIntelResourceCompiler);
    m_StaticLinkers.push_back(new CIntelStaticLinker);
    m_DynamicLinkers.push_back(new CIntelDynamicLinker);
    m_ExecutableLinkers.push_back(new CIntelExecutableLinker);
//
    GatherBuildTools();
}

CToolChain *CBorlandToolChain::CreateInstance(void) const
{
    return new CBorlandToolChain(*this);
}

void CBorlandToolChain::Reset(const CPlatform::OS_Type OS)
{
    CToolChain::Reset(OS);
    m_GenericSwitch = "-";
    m_DefineSwitch = "-D";
    m_IncludeDirSwitch = "-I";
    m_LibraryDirSwitch = "-L";
    m_LinkLibrarySwitch = "";
}

CBorlandToolChain::~CBorlandToolChain(void)
{
//
}

//------------------------------------------------------------------------------

CIntelToolChain::CIntelToolChain(void): CToolChain("icc")
{
    m_Compilers.push_back(new CIntelCCompiler);
    m_Compilers.push_back(new CIntelCppCompiler);
//m_ResourceCompilers.push_back(new CIntelResourceCompiler);
    m_StaticLinkers.push_back(new CIntelStaticLinker);
    m_DynamicLinkers.push_back(new CIntelDynamicLinker);
    m_ExecutableLinkers.push_back(new CIntelExecutableLinker);
//
    GatherBuildTools();
}

CToolChain *CIntelToolChain::CreateInstance(void) const
{
    return new CIntelToolChain(*this);
}

void CIntelToolChain::Reset(const CPlatform::OS_Type OS)
{
    CToolChain::Reset(OS);
    if (CPlatform::OS_Windows==OS) {
        m_GenericSwitch = "/";
        m_DefineSwitch = "/D";
        m_IncludeDirSwitch = "/I";
        m_LibraryDirSwitch = "/LIBPATH:";
        m_LinkLibrarySwitch = "";
    } else {
        m_GenericSwitch = "-";
        m_DefineSwitch = "-D";
        m_IncludeDirSwitch = "-I";
        m_LibraryDirSwitch = "-L";
        m_LinkLibrarySwitch = "-l";
    }
}

CIntelToolChain::~CIntelToolChain(void)
{
//
}

//------------------------------------------------------------------------------

CMSVCToolChain::CMSVCToolChain(void): CToolChain("msvc")
{
    m_Compilers.push_back(new CMSVCCompiler);
    m_Compilers.push_back(new CMSVCppCompiler);
    m_ResourceCompilers.push_back(new CMSVCResourceCompiler);
    m_StaticLinkers.push_back(new CMSVCStaticLinker);
    m_DynamicLinkers.push_back(new CMSVCDynamicLinker);
    m_ExecutableLinkers.push_back(new CMSVCExecutableLinker);
    m_ExecutableLinkers.push_back(new CMSVCConsoleExecutableLinker);
    m_ExecutableLinkers.push_back(new CMSVCNativeExecutableLinker);
//
    GatherBuildTools();
}

CToolChain *CMSVCToolChain::CreateInstance(void) const
{
    return new CMSVCToolChain(*this);
}

void CMSVCToolChain::Reset(const CPlatform::OS_Type OS)
{
    CToolChain::Reset(OS);
//if (CPlatform::OS_Windows==OS)
    {
        m_GenericSwitch = "/";
        m_DefineSwitch = "/D";
        m_IncludeDirSwitch = "/I";
        m_LibraryDirSwitch = "/LIBPATH:";
        m_LinkLibrarySwitch = "";
    }
}

CMSVCToolChain::~CMSVCToolChain(void)
{
//
}

bool CMSVCToolChain::Supports(const CPlatform::OS_Type OS) const
{
    return (CPlatform::OS_Windows==OS);
}

//------------------------------------------------------------------------------

CToolChainSet::CToolChainSet(void)
{
    Unlock();
    Clear();
}

CToolChainSet::~CToolChainSet(void)
{
    Unlock();
    Clear();
}

void CToolChainSet::Lock(void)
{
    m_Locked = true;
}

void CToolChainSet::Unlock(void)
{
    m_Locked = false;
}

void CToolChainSet::Clear(void)
{
    if (m_Locked) return;
    for (size_t i = 0; i < m_ToolChains.size(); i++) {
        for (size_t j = 0; j < m_ToolChains[i].size(); j++) {
            //std::cout<<"delete "<<m_ToolChains[i][j]<<std::endl<<std::flush;
            delete m_ToolChains[i][j];
        }
        m_ToolChains[i].clear();
    }
    m_ToolChains.clear();
    m_ToolChains.resize(CPlatform::OS_Count);
    m_HaveDefaults = false;
}

size_t CToolChainSet::GetCount(const CPlatform::OS_Type OS) const
{
    return m_ToolChains[OS].size();
}

CToolChain *CToolChainSet::ToolChain(const CPlatform::OS_Type OS, const size_t Index) const
{
    if ((OS!=CPlatform::OS_Count)&&Index<m_ToolChains[OS].size()) {
        return m_ToolChains[OS][Index];
    } else {
        return 0;
    }
}

CToolChain *CToolChainSet::Find(const CPlatform::OS_Type OS, const CString& Alias)
{
    if (OS!=CPlatform::OS_Count) for (int i = 0, n = m_ToolChains[OS].size(); i < n; i++) {
            CToolChain *tc = m_ToolChains[OS][i];
            if ((tc->Alias() == Alias)&&(tc->OS() == OS)) return tc;
        }
    return 0;
}

void CToolChainSet::AddToolChain(const CToolChain *AToolChain)
{
    CToolChain *tc = (CToolChain *)AToolChain;
    for (int i = (int)CPlatform::OS_Other; i < (int)CPlatform::OS_Count; i++) {
        CPlatform::OS_Type os_type = (CPlatform::OS_Type)i;
        if (AToolChain->Supports(os_type)) {
            if (0==tc) {
                tc = AToolChain->CreateInstance();
            }
            tc->Reset(os_type);
            m_ToolChains[os_type].push_back(tc);
            tc = 0;
        }
    }
}

void CToolChainSet::AddDefault(void)
{
    if (m_Locked || m_HaveDefaults) return;
//
    AddToolChain(new CGNUToolChain);
    AddToolChain(new CIntelToolChain);
    AddToolChain(new CMSVCToolChain);
//
    m_HaveDefaults = true;
//
}

CToolChain *CToolChainSet::Add(const CPlatform::OS_Type OS, const CString& Alias)
{
    if (m_Locked || (OS==CPlatform::OS_Count)) return 0;
    CToolChain *tc = new CToolChain(Alias);
    tc->Reset(OS);
    m_ToolChains[OS].push_back(tc);
    return tc;
}

void CToolChainSet::Remove(const CPlatform::OS_Type OS, const CString& Alias)
{
    if (m_Locked || (OS==CPlatform::OS_Count)) return;
    CToolChain *tc = Find(OS,Alias);
    if (0!=tc) {
        m_ToolChains[OS].erase(std::find(m_ToolChains[OS].begin(),m_ToolChains[OS].end(),tc));
        delete tc;
    }
}

void CToolChainSet::Read(const TiXmlElement *ConfigRoot)
{
    TiXmlNode *_tool_chain = (TiXmlNode *)ConfigRoot->FirstChild("toolchain");
    while (0!=_tool_chain) {
        TiXmlElement* tool_chain = _tool_chain->ToElement();
        if (0!=tool_chain) {
            if (strcmp(tool_chain->Value(),"toolchain")!=0) break;
            //
            char *value = 0;
            CString alias, platform_name;
            CPlatform::OS_Type platform = CPlatform::OS_Other;
            if ((value = (char *)tool_chain->Attribute("alias"))) {
                alias = value;
            }
            if ((value = (char *)tool_chain->Attribute("platform"))) {
                platform_name = value;
                platform = CPlatform::OS(platform_name);
            }
            //
            CToolChain *tc = Find(platform,alias);
            //
            //CToolChain *tc = new CToolChain("other");
            bool custom_toolchain = (0==tc);
            if (custom_toolchain) {
                tc = new CToolChain("other");
            }
            tc->Read(tool_chain);
            if (custom_toolchain) {
                m_ToolChains[tc->OS()].push_back(tc);
            }
        }
        _tool_chain = (TiXmlNode *)ConfigRoot->IterateChildren(_tool_chain);
    } // tool_chain
}

void CToolChainSet::Write(TiXmlElement *ConfigRoot)
{
    for (int i = 0, n = m_ToolChains.size(); i < n; i++) {
        for (int j = 0, m = m_ToolChains[i].size(); j < m; j++) {
            CToolChain *tc = m_ToolChains[i][j];
            TiXmlElement *tc_root = new TiXmlElement("toolchain");
            tc->Write(tc_root);
            ConfigRoot->LinkEndChild(tc_root);
        }
    }
}

/*
bool CToolChainSet::Load(const CString& FileName)
{
 if (m_Locked) return false;
 TiXmlDocument cfg;
 if (!cfg.LoadFile(FileName.GetCString())) return false;
 const TiXmlElement *root = cfg.RootElement();
 if (0==strcmp(root->Value(),"cbp2make"))
 {
  Read(root);
 } // root
 if (0==m_ToolChains.size()) AddDefault();
 return true;
}

bool CToolChainSet::Save(const CString& FileName)
{
 TiXmlDocument cfg;
 TiXmlDeclaration *xmld = new TiXmlDeclaration("1.0", "", "");
	cfg.LinkEndChild(xmld);
	TiXmlElement *root = new TiXmlElement("cbp2make");
	cfg.LinkEndChild(root);
 Write(root);
 return cfg.SaveFile(FileName.GetCString());
}
*/

void CToolChainSet::Show(void)
{
    if (m_ToolChains.size()) {
        int pl_count = 0, tc_count = 0;
        for (int i = 0, n = m_ToolChains.size(); i < n; i++) {
            if (m_ToolChains[i].size()>0) {
                tc_count += m_ToolChains[i].size();
                pl_count++;
            }
        }
        std::cout<<"Configued "<<tc_count<<" toolchain(s) for "<<pl_count<<" platform(s):"<<std::endl;
        for (int i = 0, n = m_ToolChains.size(); i < n; i++) {
            for (int j = 0, m = m_ToolChains[i].size(); j < m; j++) {
                std::cout<<CPlatform::Name((CPlatform::OS_Type)i).GetCString()
                         <<" Toolchain #"<<(j+1)<<": "<<std::endl;
                CToolChain *tc = m_ToolChains[i][j];
                tc->Show();
                std::cout<<std::endl;
            }
        }
    } else {
        std::cout<<"No toolchains configured"<<std::endl;
    }
}

//------------------------------------------------------------------------------
