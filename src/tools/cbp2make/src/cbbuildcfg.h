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
#ifndef CBBUILDCFG_H
#define CBBUILDCFG_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "platforms.h"
#include "toolchains.h"
#include "cbglobalvar.h"
#include "cbhelper.h"
//------------------------------------------------------------------------------

class TiXmlElement;

class CCodeBlocksBuildConfig
{
private:
    CPlatformSet m_Platforms;
    CToolChainSet m_ToolChains;
    CGlobalVariableConfig m_GlobalVariables;
    CStringList m_Targets;
    CString m_DefaultOptions;
    int m_TargetNameCase;
    int m_MacroVariableCase;
    int m_QuotePathMode;
    bool m_NumericToolchainSuffix;
    bool m_FlatObjectNames;
    bool m_FlatObjectPaths;
    bool m_MultilineObjects;
    bool m_MultilineOptions;
    bool m_IncludeDependencies;
    bool m_KeepObjectDirectories;
    bool m_KeepOutputDirectories;
    bool m_BeQuiet;
    bool m_BeVerbose;
public:
    CPlatformSet& Platforms(void)
    {
        return m_Platforms;
    }
    CToolChainSet& ToolChains(void)
    {
        return m_ToolChains;
    }
    CGlobalVariableConfig& GlobalVariables(void)
    {
        return m_GlobalVariables;
    }
    CStringList& Targets(void)
    {
        return m_Targets;
    }
    CString& DefaultOptions(void)
    {
        return m_DefaultOptions;
    }
    int& TargetNameCase(void)
    {
        return m_TargetNameCase;
    }
    int& MacroVariableCase(void)
    {
        return m_MacroVariableCase;
    }
    int& QuotePathMode(void)
    {
        return m_QuotePathMode;
    }
    bool& NumericToolchainSuffix(void)
    {
        return m_NumericToolchainSuffix;
    }
    bool& FlatObjectNames(void)
    {
        return m_FlatObjectNames;
    }
    bool& FlatObjectPaths(void)
    {
        return m_FlatObjectPaths;
    }
    bool& MultilineObjects(void)
    {
        return m_MultilineObjects;
    }
    bool& MultilineOptions(void)
    {
        return m_MultilineOptions;
    }
    bool& IncludeDependencies(void)
    {
        return m_IncludeDependencies;
    }
    bool& KeepObjectDirectories(void)
    {
        return m_KeepObjectDirectories;
    }
    bool& KeepOutputDirectories(void)
    {
        return m_KeepOutputDirectories;
    }
    bool& BeQuiet(void)
    {
        return m_BeQuiet;
    }
    bool& BeVerbose(void)
    {
        return m_BeVerbose;
    }
public:
    void Clear(void);
    bool Load(const CString& FileName);
    bool Save(const CString& FileName);
    void Show(void);
public:
    CCodeBlocksBuildConfig(void);
    ~CCodeBlocksBuildConfig(void);
};

#endif
//------------------------------------------------------------------------------
