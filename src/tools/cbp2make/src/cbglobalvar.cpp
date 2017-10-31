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
#include "tinyxml.h"
#include "stlfutils.h"
#include "cbglobalvar.h"
#include "cbhelper.h"
//------------------------------------------------------------------------------

CGlobalVariable::CGlobalVariable(void)
{
    Clear();
}

CGlobalVariable::~CGlobalVariable(void)
{
    Clear();
}

int CGlobalVariable::Count(void)
{
    return m_Fields.GetCount();
}

CString CGlobalVariable::GetField(const int Index)
{
    return m_Fields.Variable(Index).GetName();
}

CString CGlobalVariable::GetValue(const int Index)
{
    return m_Fields.Variable(Index).GetString();
}

CString CGlobalVariable::Convert(const CString& Value, const int Case)
{
    CString result, tmp;
    int state = 0;
    for (int i = 0, n = Value.GetLength(); i < n; i++) {
        switch (state) {
        case 0: {
            if (Value[i]=='$') {
                state = 1;
                tmp += Value[i];
            } else result += Value[i];
            break;
        }
        case 1: {
            tmp += Value[i];
            if (Value[i]=='(') {
                state = 2;
            } else {
                state = 0;
                result += tmp;
                tmp.Clear();
            }
            break;
        }
        case 2: {
            tmp += Value[i];
            if (Value[i]=='#') {
                state = 3;
            } else {
                state = 0;
                result += tmp;
                tmp.Clear();
            }
            break;
        }
        case 3: {
            tmp += Value[i];
            if (Value[i]==')') {
                state = 0;
                CString gcv_name = SubStr(tmp,3,tmp.GetLength()-2);
                //CString mfv_name = UpperCase(MakefileFriendly(gcv_name));
                CString mfv_name = MakefileFriendly(gcv_name);
                switch (Case) {
                case 1: {
                    mfv_name = LowerCase(mfv_name);
                    break;
                }
                case 2: {
                    mfv_name = UpperCase(mfv_name);
                    break;
                }
                default:
                    break;
                }
                result += "$("+mfv_name+")";
                tmp.Clear();
            }
            break;
        }
        }
    }
    return result;
}

CString CGlobalVariable::Base(void)
{
    if (m_Base.IsEmpty()) {
        return ".";
    }
    return m_Base;
}

CString CGlobalVariable::Include(void)
{
    if (m_Include.IsEmpty()) {
        if (m_Base.IsEmpty()) {
            return ".";
        } else {
            return JoinPaths(Base(),"/include");
        }
    }
    return m_Include;
}

CString CGlobalVariable::Lib(void)
{
    if (m_Lib.IsEmpty()) {
        if (m_Base.IsEmpty()) {
            return ".";
        } else {
            return JoinPaths(Base(),"/lib");
        }
    }
    return m_Lib;
}

void CGlobalVariable::Clear(void)
{
    m_Name.Clear();
    m_Base.Clear();
    m_Include.Clear();
    m_Lib.Clear();
    m_Obj.Clear();
    m_Cflags.Clear();
    m_Lflags.Clear();
    m_Fields.Clear();
}

void CGlobalVariable::Add(const CString& Name, const CString& Value)
{
    CString name = Name;
    int field = GuessStr(Name,"base include lib obj cflags lflags",
                         name,false);
    switch (field) {
    case 0: {
        m_Base = Value;
        break;
    }
    case 1: {
        m_Include = Value;
        break;
    }
    case 2: {
        m_Lib = Value;
        break;
    }
    case 3: {
        m_Obj = Value;
        break;
    }
    case 4: {
        m_Cflags = Value;
        break;
    }
    case 5: {
        m_Lflags = Value;
        break;
    }
    default: {
        field = m_Fields.VarIndex(Name);
        if (field >= 0) {
            m_Fields.Variable(field).SetString(Value);
        } else {
            m_Fields.InsertStringVariable(Name,Value);
        }
    }
    }
}

void CGlobalVariable::Remove(const CString& Name)
{
    m_Fields.RemoveVariable(Name);
}

void CGlobalVariable::Read(const TiXmlElement *GlobalVariableRoot)
{
    char *value = 0;
    if ((value = (char *)GlobalVariableRoot->Attribute("name"))) {
        m_Name = value;
    }
    if ((value = (char *)GlobalVariableRoot->Attribute("description"))) {
        m_Description = value;
    }
    const TiXmlNode *_built_in = GlobalVariableRoot->FirstChild("builtin");
    if (0!=_built_in) {
        TiXmlElement *built_in = (TiXmlElement *)_built_in->ToElement();
        if (0!=built_in) {
            if ((value = (char *)built_in->Attribute("base"))) {
                m_Base = value;
            }
            if ((value = (char *)built_in->Attribute("include"))) {
                m_Include = value;
            }
            if ((value = (char *)built_in->Attribute("lib"))) {
                m_Lib = value;
            }
            if ((value = (char *)built_in->Attribute("obj"))) {
                m_Obj = value;
            }
            if ((value = (char *)built_in->Attribute("cflags"))) {
                m_Cflags = value;
            }
            if ((value = (char *)built_in->Attribute("lflags"))) {
                m_Lflags = value;
            }
            /*
            if ((value = (char *)built_in->Attribute("")))
            {
             m_ = value;
            }
            */
        } // built_in
    } // _built_in
    const TiXmlNode *_user = GlobalVariableRoot->FirstChild("user");
    if (0!=_user) {
        TiXmlElement *user = (TiXmlElement *)_user->ToElement();
        if (0!=user) {
            TiXmlNode *_field = user->FirstChild("field");
            while (0!=_field) {
                TiXmlElement *field = (TiXmlElement *)_field->ToElement();
                if (0!=field) {
                    CString field_name, field_value;
                    char *value = 0;
                    if ((value = (char *)field->Attribute("name"))) {
                        field_name = value;
                    }
                    if ((value = (char *)field->Attribute("value"))) {
                        field_value = value;
                    }
                    m_Fields.InsertStringVariable(field_name,field_value);
                    /*
                    if ((value = (char *)field->Attribute("")))
                    {
                     m_ = value;
                    }
                    */
                }
                _field = user->IterateChildren(_field);
            }
        } // user
    } // _user
}

void CGlobalVariable::Write(TiXmlElement *GlobalVariableRoot)
{
    GlobalVariableRoot->SetAttribute("name",m_Name.GetCString());
    GlobalVariableRoot->SetAttribute("description",m_Description.GetCString());
    TiXmlElement *built_in = new TiXmlElement("builtin");
    //built_in->SetAttribute("name",m_Name.GetCString());
    built_in->SetAttribute("base",m_Base.GetCString());
    built_in->SetAttribute("include",m_Include.GetCString());
    built_in->SetAttribute("lib",m_Lib.GetCString());
    built_in->SetAttribute("obj",m_Obj.GetCString());
    built_in->SetAttribute("cflags",m_Cflags.GetCString());
    built_in->SetAttribute("lflags",m_Lflags.GetCString());
    //built_in->SetAttribute("",m_.GetCString());
    GlobalVariableRoot->LinkEndChild(built_in);
    TiXmlElement *fields = new TiXmlElement("user");
    GlobalVariableRoot->LinkEndChild(fields);
    for (int i = 0, n = m_Fields.GetCount(); i < n; i++) {
        CVariable& v = m_Fields.Variable(i);
        TiXmlElement *field = new TiXmlElement("field");
        field->SetAttribute("name",v.GetName().GetCString());
        field->SetAttribute("value",v.GetString().GetCString());
        fields->LinkEndChild(field);
    }
}

void CGlobalVariable::Show(void)
{
    std::cout<<"Name: "<<m_Name.GetString()<<std::endl;
    std::cout<<"Descriprion: "<<m_Description.GetString()<<std::endl;
    std::cout<<"Built-in fields: "<<std::endl;
    std::cout<<"base: "<<m_Base.GetString()<<std::endl;
    std::cout<<"include: "<<m_Include.GetString()<<std::endl;
    std::cout<<"lib: "<<m_Lib.GetString()<<std::endl;
    std::cout<<"obj: "<<m_Obj.GetString()<<std::endl;
    std::cout<<"cflags: "<<m_Cflags.GetString()<<std::endl;
    std::cout<<"lflags: "<<m_Lflags.GetString()<<std::endl;
    if (m_Fields.GetCount()) {
        std::cout<<"User fields: "<<std::endl;
        for (int i = 0, n = m_Fields.GetCount(); i < n; i++) {
            CVariable& field = m_Fields.Variable(i);
            std::cout<<field.GetName().GetString()<<": "<<field.GetString().GetString()<<std::endl;
        }
    }
//std::cout<<": "<<m_.GetString()<<std::endl;
}

//------------------------------------------------------------------------------

CGlobalVariableSet::CGlobalVariableSet(void)
{
    Clear();
}

CGlobalVariableSet::~CGlobalVariableSet(void)
{
    Clear();
}

void CGlobalVariableSet::Clear(void)
{
    m_Name = "default";
    for (size_t i = 0; i < m_Variables.size(); i++) delete m_Variables[i];
    m_Variables.clear();
    m_Active = false;
}

CGlobalVariable* CGlobalVariableSet::Get(const size_t Index)
{
    if (Index<m_Variables.size()) {
        return m_Variables[Index];
    }
    return 0;
}

CGlobalVariable *CGlobalVariableSet::Find(const CString& Name)
{
    for (size_t i = 0; i < m_Variables.size(); i++) {
        CGlobalVariable *v = m_Variables[i];
        if (v->Name() == Name) return v;
    }
    return 0;
}

CGlobalVariable *CGlobalVariableSet::Add(const CString& Name, const CString& Description)
{
    CGlobalVariable *v = Find(Name);
    if (0==v) {
        v = new CGlobalVariable();
        v->Name() = Name;
        v->Description() = Description;
        m_Variables.push_back(v);
    }
    return v;
}

void CGlobalVariableSet::Remove(const CString& Name)
{
    CGlobalVariable *v = Find(Name);
    if (0!=v) {
        m_Variables.erase(std::find(m_Variables.begin(),m_Variables.end(),v));
        delete v;
    }
}

void CGlobalVariableSet::Read(const TiXmlElement *GlobalVariableSetRoot)
{
    char *value = 0;
    if ((value = (char *)GlobalVariableSetRoot->Attribute("name"))) {
        m_Name = value;
    }
    TiXmlNode *_v_root = (TiXmlNode *)GlobalVariableSetRoot->FirstChild("variable");
    while (0!=_v_root) {
        TiXmlElement *v_root = _v_root->ToElement();
        if (0!=v_root) {
            CGlobalVariable *v = new CGlobalVariable();
            v->Read(v_root);
            m_Variables.push_back(v);
        }
        _v_root = (TiXmlNode *)GlobalVariableSetRoot->IterateChildren(_v_root);
    }
}

void CGlobalVariableSet::Write(TiXmlElement *GlobalVariableSetRoot)
{
    GlobalVariableSetRoot->SetAttribute("name",m_Name.GetCString());
    for (size_t i = 0, n = m_Variables.size(); i < n; i++) {
        CGlobalVariable *v = m_Variables[i];
        TiXmlElement *v_root = new TiXmlElement("variable");
        v_root->SetAttribute("name",m_Name.GetCString());
        v->Write(v_root);
        GlobalVariableSetRoot->LinkEndChild(v_root);
    }
}

void CGlobalVariableSet::Show(void)
{
    std::cout<<"Variable set name: "<<m_Name.GetString()<<std::endl;
    std::cout<<"Variables: "<<m_Variables.size()<<std::endl;
    for (size_t i = 0, n = m_Variables.size(); i < n; i++) {
        CGlobalVariable *v = m_Variables[i];
        std::cout<<"Variable #"<<(i+1)<<":"<<std::endl;
        v->Show();
    }
}

//------------------------------------------------------------------------------

CGlobalVariableConfig::CGlobalVariableConfig(void)
{
    Clear();
}

CGlobalVariableConfig::~CGlobalVariableConfig(void)
{
    Clear();
}

void CGlobalVariableConfig::Clear(void)
{
    for (size_t i = 0; i < m_VariableSets.size(); i++) delete m_VariableSets[i];
    m_VariableSets.clear();
//m_HaveDefaults = false;
}

CGlobalVariableSet* CGlobalVariableConfig::Get(const size_t Index)
{
    if (Index<m_VariableSets.size()) {
        return m_VariableSets[Index];
    }
    return 0;
}

CGlobalVariableSet* CGlobalVariableConfig::Find(const CString& Name)
{
    for (size_t i = 0, n = m_VariableSets.size(); i < n; i++) {
        CGlobalVariableSet *vset = m_VariableSets[i];
        if (vset->Name() == Name) return vset;
    }
    return 0;
}

CGlobalVariableSet* CGlobalVariableConfig::Add(const CString& Name)
{
    CGlobalVariableSet *vset = Find(Name);
    if (0==vset) {
        vset = new CGlobalVariableSet();
        vset->Name() = Name;
        m_VariableSets.push_back(vset);
    }
    return vset;
}

void CGlobalVariableConfig::AddDefault(void)
{
//if (m_HaveDefaults) return;
    CGlobalVariableSet *vset = Find("default");
    if (0==vset) {
        vset = new CGlobalVariableSet();
        vset->Name() = "default";
        m_VariableSets.push_back(vset);
    }
//m_HaveDefaults = true;
}

void CGlobalVariableConfig::Remove(const CString& Name)
{
    CGlobalVariableSet *vset = Find(Name);
    if (0!=vset) {
        m_VariableSets.erase(std::find(m_VariableSets.begin(),m_VariableSets.end(),vset));
        delete vset;
    }
}

void CGlobalVariableConfig::Read(const TiXmlElement *GlobalVariableConfigRoot)
{
    TiXmlNode *_vset_root = (TiXmlNode *)GlobalVariableConfigRoot->FirstChild("variableset");
    while (0!=_vset_root) {
        TiXmlElement *vset_root = _vset_root->ToElement();
        if (0!=vset_root) {
            char *value = 0;
            CString name;
            if ((value = (char *)vset_root->Attribute("name"))) {
                name = value;
            }
            CGlobalVariableSet *vset = Find(name);
            bool custom_vset = (0==vset);
            if (custom_vset) {
                vset = new CGlobalVariableSet();
            }
            vset->Read(vset_root);
            if (custom_vset) {
                m_VariableSets.push_back(vset);
            }
        }
        _vset_root = (TiXmlNode *)GlobalVariableConfigRoot->IterateChildren(_vset_root);
    }
    AddDefault();
}

void CGlobalVariableConfig::Write(TiXmlElement *GlobalVariableConfigRoot)
{
    for (size_t i = 0, n = m_VariableSets.size(); i < n; i++) {
        CGlobalVariableSet *vset = m_VariableSets[i];
        TiXmlElement *vset_root = new TiXmlElement("variableset");
        vset->Write(vset_root);
        GlobalVariableConfigRoot->LinkEndChild(vset_root);
    }
}

void CGlobalVariableConfig::Show(void)
{
    std::cout<<"Global compiler variables: "<<m_VariableSets.size()<<" set(s)"<<std::endl;
    for (size_t i = 0, n = m_VariableSets.size(); i < n; i++) {
        CGlobalVariableSet *vset = m_VariableSets[i];
        std::cout<<"Variable set #"<<(i+1)<<": "<<std::endl;
        vset->Show();
    }
}

//------------------------------------------------------------------------------
