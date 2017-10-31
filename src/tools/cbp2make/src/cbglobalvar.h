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
#ifndef CBGLOBALVAR_H
#define CBGLOBALVAR_H
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stlconfig.h"
//------------------------------------------------------------------------------

class TiXmlElement;
class CGlobalVariableSet;

class CGlobalVariable
{
private:
    CString m_Name;
    CString m_Description;
    // standard fields
    CString m_Base;
    CString m_Include;
    CString m_Lib;
    CString m_Obj;
    CString m_Cflags;
    CString m_Lflags;
    // user fields
    CConfiguration m_Fields;
public:
    CString& Name(void)
    {
        return m_Name;
    }
    CString& Description(void)
    {
        return m_Description;
    }
    CString  Base(void);
    CString  Include(void);
    CString  Lib(void);
    CString& Obj(void)
    {
        return m_Obj;
    }
    CString& Cflags(void)
    {
        return m_Cflags;
    }
    CString& Lflags(void)
    {
        return m_Lflags;
    }
public:
    static CString Convert(const CString& Value, const int Case = 0);
    int Count(void);
    CString GetField(const int Index);
    CString GetValue(const int Index);
    void Clear(void);
    void Add(const CString& Name, const CString& Value);
    void Remove(const CString& Name);
    void Read(const TiXmlElement *GlobalVariableRoot);
    void Write(TiXmlElement *GlobalVariableRoot);
    void Show(void);
public:
    CGlobalVariable(void);
    ~CGlobalVariable(void);
};

class CGlobalVariableSet
{
private:
    CString m_Name;
    std::vector<CGlobalVariable *> m_Variables;
    bool m_Active;
    //bool m_HaveDefaults;
public:
    CString& Name(void)
    {
        return m_Name;
    }
    bool& Active(void)
    {
        return m_Active;
    }
public:
    void Clear(void);
    size_t Count(void) const
    {
        return m_Variables.size();
    };
    CGlobalVariable *Get(const size_t Index);
    CGlobalVariable *Find(const CString& Name);
    CGlobalVariable *Add(const CString& Name, const CString& Description = "");
    void Remove(const CString& Name);
    void Read(const TiXmlElement *GlobalVariableSetRoot);
    void Write(TiXmlElement *GlobalVariableSetRoot);
    void Show(void);
public:
    CGlobalVariableSet(void);
    ~CGlobalVariableSet(void);
};

class CGlobalVariableConfig
{
private:
    std::vector<CGlobalVariableSet *> m_VariableSets;
public:
public:
    void Clear(void);
    size_t Count(void) const
    {
        return m_VariableSets.size();
    };
    CGlobalVariableSet *Get(const size_t Index);
    CGlobalVariableSet *Find(const CString& Name);
    CGlobalVariableSet *Add(const CString& Name);
    void AddDefault(void);
    void Remove(const CString& Name);
    void Read(const TiXmlElement *GlobalVariableConfigRoot);
    void Write(TiXmlElement *GlobalVariableConfigRoot);
    void Show(void);
public:
    CGlobalVariableConfig(void);
    ~CGlobalVariableConfig(void);
};

#endif
//------------------------------------------------------------------------------
