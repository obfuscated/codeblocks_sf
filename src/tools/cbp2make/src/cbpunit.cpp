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
#include <iostream>
//------------------------------------------------------------------------------
#include "macros.h"
#include "cbpunit.h"
#include "cbhelper.h"
#include "stlconvert.h"
#include "stlfutils.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CBuildUnit::CBuildUnit(void)
{
    Clear();
}

CBuildUnit::~CBuildUnit(void)
{
    Clear();
}

void CBuildUnit::Clear(void)
{
    m_FileName.Clear();
    m_Targets.Clear();
    m_DoCompile = true;
    m_DoLink = true;
//m_Type = utNone;
    m_Weight = 0;
}

CString CBuildUnit::Extension(void) const
{
    return ExtractFileExt(m_FileName);
}

bool CBuildUnit::BelongToTarget(const CString& TargetName)
{
    return ((m_Targets.GetCount()==0) || (m_Targets.FindString(TargetName)>=0));
}

void CBuildUnit::Read(const TiXmlElement *UnitRoot)
{
    char *value = 0;
    if ((value = (char *)UnitRoot->Attribute("filename"))) {
        m_FileName = value;
    }
    TiXmlNode *_option = (TiXmlNode *)UnitRoot->FirstChild("Option");
    while (0 != _option) {
        TiXmlElement* option = _option->ToElement();
        if (0 != option) {
            char *value = 0;
            if ((value = (char *)option->Attribute("compilerVar"))) {
                m_CompilerVariable = value;
            }
            if ((value = (char *)option->Attribute("compiler"))) {
                m_CompilerVariable = value;
            }
            if ((value = (char *)option->Attribute("compile"))) {
                m_DoCompile = StringToBoolean(value);
            }
            if ((value = (char *)option->Attribute("link"))) {
                m_DoLink = StringToBoolean(value);
            }
            if ((value = (char *)option->Attribute("target"))) {
                m_Targets.Insert(value);
            }
            if ((value = (char *)option->Attribute("weight"))) {
                m_Weight = StringToInteger(value);
            }
            if ((value = (char *)option->Attribute("buildCommand"))) {
                m_CustomBuildCommand = value;
                m_CustomBuildCommand = FindReplaceStr(m_CustomBuildCommand, "\\n", "\n\t");
            }
        }
        _option = (TiXmlNode *)UnitRoot->IterateChildren(_option);
    } // option
}

void CBuildUnit::Show(void)
{
#ifdef SHOW_MODE_ONELINE
    std::cout<<m_FileName.GetString();
    std::cout<<", Weight: "<<m_Weight;
    std::cout<<", Compile: "<<BooleanToYesNoString(m_DoCompile).GetString();
    std::cout<<", Link: "<<BooleanToYesNoString(m_DoLink).GetString();
    std::cout<<", Targets: ";
    if (m_Targets.GetCount()) {
        for (int i = 0, n = m_Targets.GetCount(); i < n; i++) {
            std::cout<<m_Targets[i].GetString()<<" ";
        }
    } else {
        std::cout<<"all";
    }
    std::cout<<std::endl;
#else
    std::cout<<"Filename: "<<m_Filename.GetString()<<std::endl;
    std::cout<<"Weight: "<<m_Weight<<std::endl;
    std::cout<<"Compile: "<<BooleanToYesNoString(m_DoCompile).GetString()<<std::endl;
    std::cout<<"Link: "<<BooleanToYesNoString(m_DoLink).GetString()<<std::endl;
    std::cout<<"Targets: ";
    if (m_Targets.GetCount()) {
        ShowStringList("Targets","Target",m_Targets);
    } else {
        std::cout<<"all";
    }
    std::cout<<std::endl;
#endif
}

//------------------------------------------------------------------------------
