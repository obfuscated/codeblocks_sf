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
#ifndef CBP_UNIT_H
#define CBP_UNIT_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "cbptarget.h"
//------------------------------------------------------------------------------

class TiXmlNode;
class TiXmlElement;
class CCodeBlocksProject;

class CUnitWeightComparison;
class CBuildUnit
{
    friend class CUnitWeightComparison;
private:
    CString m_FileName;
    CStringList m_Targets;
    CString m_CompilerVariable;
    bool m_DoCompile;
    bool m_DoLink;
    int m_Weight;
    //
    CString m_ObjectFileName;
    CString m_CustomBuildCommand;
public:
    CString FileName(void) const
    {
        return m_FileName;
    }
    CString CustomBuildCommand() const
    {
        return m_CustomBuildCommand;
    }
    bool HasCustomBuild() const
    {
        return m_CustomBuildCommand.GetLength() != 0;
    }
    CString Extension(void) const;
    bool BelongToTarget(const CString& TargetName);
    CString CompilerVariable(void) const
    {
        return m_CompilerVariable;
    }
    bool DoCompile(void) const
    {
        return m_DoCompile;
    }
    bool DoLink(void) const
    {
        return m_DoLink;
    }
    int Weight(void) const
    {
        return m_Weight;
    }
public:
    void Clear(void);
    void Read(const TiXmlElement *UnitRoot);
    void Show(void);
public:
    CBuildUnit(void);
    ~CBuildUnit(void);
};

#endif
//------------------------------------------------------------------------------
