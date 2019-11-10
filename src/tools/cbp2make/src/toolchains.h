/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

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
#ifndef TOOLCHAINS_H
#define TOOLCHAINS_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "platforms.h"
#include "buildtools.h"
//------------------------------------------------------------------------------

class TiXmlElement;
class CToolChainSet;

class CToolChain
{
private:
    CPlatform::OS_Type m_Platform;
    CString m_Alias;
protected:
    CString m_GenericSwitch;
    CString m_DefineSwitch;
    CString m_IncludeDirSwitch;
    CString m_LibraryDirSwitch;
    CString m_LinkLibrarySwitch;
    //CString m_;
protected:
    std::vector<CBuildTool *>        m_BuildTools;
    std::vector<CPreprocessor *>     m_Preprocessors;
    std::vector<CAssembler *>        m_Assemblers;
    std::vector<CCompiler *>         m_Compilers;
    std::vector<CResourceCompiler *> m_ResourceCompilers;
    std::vector<CStaticLinker *>     m_StaticLinkers;
    std::vector<CDynamicLinker *>    m_DynamicLinkers;
    std::vector<CExecutableLinker *> m_ExecutableLinkers;
protected:
    void GatherBuildTools(std::vector<CBuildTool *>* Source,
                          std::vector<CBuildTool *>* Target);
    void GatherBuildTools(void);
    CBuildTool *FindBuildTool(const CString& FileExtension,
                              const std::vector<CBuildTool *>* Tools);
    std::vector<CBuildTool *>* GetTools(const CBuildTool::ToolType Type = CBuildTool::btOther);
    void RemoveTool(const CBuildTool* BuildTool, std::vector<CBuildTool *>* Tools);
    void RemoveTool(const CBuildTool* BuildTool);
protected:
    void Read(const TiXmlElement *Root, const CString& Name, CString& Value);
    void Read(const TiXmlElement *Root, const CString& Name, bool& Value);
    void Write(TiXmlElement *Root, const CString& Name, const CString& Value);
    void Write(TiXmlElement *Root, const CString& Name, const bool Value);
public:
    CPlatform::OS_Type OS(void) const
    {
        return m_Platform;
    }
    CString Alias(void) const
    {
        return m_Alias;
    }
    CString& GenericSwitch(void)
    {
        return m_GenericSwitch;
    }
    CString& DefineSwitch(void)
    {
        return m_DefineSwitch;
    }
    CString& IncludeDirSwitch(void)
    {
        return m_IncludeDirSwitch;
    }
    CString& LibraryDirSwitch(void)
    {
        return m_LibraryDirSwitch;
    }
    CString& LinkLibrarySwitch(void)
    {
        return m_LinkLibrarySwitch;
    }
    CString sw(void) const
    {
        return m_GenericSwitch;
    }
    CString sw(const CString& ASwitch) const;
    //CString (void) const { return m_; }
    CBuildTool *CreateBuildTool(const CBuildTool::ToolType Type = CBuildTool::btOther);
public:
    virtual void Assign(const CToolChain& ToolChain);
    virtual CToolChain *CreateInstance(void) const;
    virtual void Clear(void);
    virtual void Reset(const CPlatform::OS_Type OS);
    virtual bool Supports(const CPlatform::OS_Type OS) const;
    virtual void Read(const TiXmlElement *ToolChainRoot);
    virtual void Write(TiXmlElement *ToolChainRoot);
    virtual void Show(void);
    size_t ToolsCount(const CBuildTool::ToolType Type = CBuildTool::btOther);
    CBuildTool *GetBuildTool(const size_t index, const CBuildTool::ToolType Type = CBuildTool::btOther);
    CBuildTool *FindBuildToolByName(const CString& ToolName);
    CBuildTool *FindBuildTool(const CString& FileExtension);
    CAssembler *FindAssembler(const CString& FileExtension);
    CCompiler *FindCompiler(const CString& FileExtension);
    //std::vector<CBuildTool *> FindTools(const CString& FileExtension);
    bool RemoveToolByName(const CString& ToolName);
public:
    explicit CToolChain(const CString& Alias);
    CToolChain(const CToolChain& ToolChain);
    virtual ~CToolChain(void);
};

//------------------------------------------------------------------------------

class CGNUToolChain: public CToolChain
{
public:
    virtual CToolChain *CreateInstance(void) const;
    virtual void Reset(const CPlatform::OS_Type OS);
public:
    CGNUToolChain(void);
    virtual ~CGNUToolChain(void);
};

class CBorlandToolChain: public CToolChain
{
public:
    virtual CToolChain *CreateInstance(void) const;
    virtual void Reset(const CPlatform::OS_Type OS);
public:
    CBorlandToolChain(void);
    virtual ~CBorlandToolChain(void);
};

class CIntelToolChain: public CToolChain
{
public:
    virtual CToolChain *CreateInstance(void) const;
    virtual void Reset(const CPlatform::OS_Type OS);
public:
    CIntelToolChain(void);
    virtual ~CIntelToolChain(void);
};

class CMSVCToolChain: public CToolChain
{
public:
    virtual CToolChain *CreateInstance(void) const;
    virtual void Reset(const CPlatform::OS_Type OS);
    bool Supports(const CPlatform::OS_Type OS) const;
public:
    CMSVCToolChain(void);
    virtual ~CMSVCToolChain(void);
};

//------------------------------------------------------------------------------

class CToolChainSet
{
private:
    std::vector<std::vector<CToolChain *> > m_ToolChains;
    bool m_Locked;
    bool m_HaveDefaults;
protected:
    void AddToolChain(const CToolChain *AToolChain);
public:
    void Lock(void);
    void Unlock(void);
    void Clear(void);
    size_t GetCount(const CPlatform::OS_Type OS) const;
    CToolChain *ToolChain(const CPlatform::OS_Type OS, const size_t Index) const;
    CToolChain *Find(const CPlatform::OS_Type OS, const CString& Alias);
    void AddDefault(void);
    CToolChain *Add(const CPlatform::OS_Type OS, const CString& Alias);
    void Remove(const CPlatform::OS_Type OS, const CString& Alias);
    void Read(const TiXmlElement *ConfigRoot);
    void Write(TiXmlElement *ConfigRoot);
    //bool Load(const CString& FileName);
    //bool Save(const CString& FileName);
    void Show(void);
public:
    CToolChainSet(void);
    ~CToolChainSet(void);
};

#endif
//------------------------------------------------------------------------------
