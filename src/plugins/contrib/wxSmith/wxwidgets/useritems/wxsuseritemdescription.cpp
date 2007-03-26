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

#include "wxsuseritemdescription.h"

#include <wx/mstream.h>
#include <wx/image.h>

namespace
{
    /** \brief Factory for user-defined items */
    class wxsUserItemFactory: public wxsItemFactory
    {
        public:

            /** \brief Ctor */
            wxsUserItemFactory(wxsUserItemDescription* Description):
                wxsItemFactory(Description->GetInfo(),Description->GetName()),
                m_Description(Description)
            {
            }

            /** \brief Dctor */
            virtual ~wxsUserItemFactory()
            {
            }

        private:

            virtual wxsItem* OnBuild(wxsItemResData* Data)
            {
                return m_Description->BuildUserItem(Data);
            }

            wxsUserItemDescription* m_Description;
    };
}


wxsUserItemDescription::wxsUserItemDescription(const wxString& Name): m_Name(Name), m_Factory(NULL), m_RefCount(1)
{
}

wxsUserItemDescription::~wxsUserItemDescription()
{
    delete m_TreeImage;
    m_TreeImage = NULL;
    delete m_Factory;
    m_Factory = NULL;
}

wxsUserItem* wxsUserItemDescription::BuildUserItem(wxsItemResData* Data)
{
    wxsGenericPropertyValueList* NewList = m_Properties.BuildList();
    wxsUserItem* Item = new wxsUserItem(Data,this,&m_Info,NewList,m_PreviewBitmap,m_DefaultSize);
    Item->AddLanguage(wxsCPP,m_CppCodeTemplate,m_CppDeclarationHeaders,m_CppDefinitionHeaders);
    return Item;
}

void wxsUserItemDescription::ReadFromConfig(ConfigManager* Manager,const wxString& BasePath)
{
    delete m_TreeImage;
    m_TreeImage = NULL;
    delete m_Factory;
    m_Factory = NULL;

    wxString BinaryBitmapMain;
    wxString BinaryBitmap16;
    wxString BinaryBitmap32;

    m_Info.ClassName        = m_Name;
    m_Info.Type             = wxsTWidget;
    m_Info.License          = Manager->Read(BasePath+_T("/Info/License"),wxEmptyString);
    m_Info.Author           = Manager->Read(BasePath+_T("/Info/Author"),wxEmptyString);
    m_Info.Email            = Manager->Read(BasePath+_T("/Info/EMail"),wxEmptyString);
    m_Info.Site             = Manager->Read(BasePath+_T("/Info/Site"),wxEmptyString);
    m_Info.Category         = Manager->Read(BasePath+_T("/Info/Category"),wxEmptyString);
    m_Info.Priority         = Manager->ReadInt(BasePath+_T("/Info/Priority"),50);
    m_Info.DefaultVarName   = Manager->Read(BasePath+_T("/Info/DefaultVarName"),wxEmptyString);
    m_Info.Languages        = Manager->ReadInt(BasePath+_T("/Info/Languages"),wxsCPP);
    m_Info.VerHi            = Manager->ReadInt(BasePath+_T("/Info/VerHi"),0);
    m_Info.VerLo            = Manager->ReadInt(BasePath+_T("/Info/VerLo"),0);
    BinaryBitmap16          = Manager->ReadBinary(BasePath+_T("/Info/Icon16"));
    BinaryBitmap32          = Manager->ReadBinary(BasePath+_T("/Info/Icon32"));
    m_Info.AllowInXRC       = Manager->ReadBool(BasePath+_T("/Info/AllowInXRC"),false);
    BinaryBitmapMain        = Manager->ReadBinary(BasePath+_T("/Bitmap"));
    wxString SizeStr        = Manager->Read(BasePath+_T("/DefaultSize"),_T("50,50"));
    m_CppCodeTemplate       = Manager->Read(BasePath+_T("/Cpp/CodeTemplate"),wxEmptyString);
    m_CppDeclarationHeaders = Manager->ReadArrayString(BasePath+_T("/Cpp/DeclarationHeaders"));
    m_CppDefinitionHeaders  = Manager->ReadArrayString(BasePath+_T("/Cpp/DefinitionHeaders"));

    wxMemoryInputStream StreamBitmap16(BinaryBitmap16.c_str(),BinaryBitmap16.Length());
    m_Info.Icon16 = wxBitmap(wxImage(StreamBitmap16));
    wxMemoryInputStream StreamBitmap32(BinaryBitmap32.c_str(),BinaryBitmap32.Length());
    m_Info.Icon32 = wxBitmap(wxImage(StreamBitmap32));
    m_TreeImage = new wxsAutoResourceTreeImage(m_Info.Icon16);
    m_Info.TreeIconId = m_TreeImage->GetIndex();
    wxMemoryInputStream StreamBitmapMain(BinaryBitmapMain,BinaryBitmapMain.Length());
    m_PreviewBitmap = wxBitmap(wxImage(StreamBitmapMain));
    m_Properties.ReadFromConfig(Manager,BasePath+_T("/Poperties"));
    long Width, Height;
    SizeStr.BeforeFirst(_T(',')).ToLong(&Width);
    SizeStr.AfterLast(_T(',')).ToLong(&Height);
    m_DefaultSize.SetWidth(Width);
    m_DefaultSize.SetHeight(Height);
    m_Factory = new wxsUserItemFactory(this);
}

void wxsUserItemDescription::WriteToConfig(ConfigManager* Manager,const wxString& BasePath)
{
    wxMemoryOutputStream StreamBitmapMain;
    wxMemoryOutputStream StreamBitmap16;
    wxMemoryOutputStream StreamBitmap32;

    m_PreviewBitmap.ConvertToImage().SaveFile(StreamBitmapMain,wxBITMAP_TYPE_PNG);
    m_Info.Icon16.ConvertToImage().SaveFile(StreamBitmap16,wxBITMAP_TYPE_PNG);
    m_Info.Icon32.ConvertToImage().SaveFile(StreamBitmap32,wxBITMAP_TYPE_PNG);

    void* BitmapMain = StreamBitmapMain.GetOutputStreamBuffer()->GetBufferStart();
    void* Bitmap16   = StreamBitmap16.GetOutputStreamBuffer()->GetBufferStart();
    void* Bitmap32   = StreamBitmap32.GetOutputStreamBuffer()->GetBufferStart();

    int BitmapMainLen = (char*)StreamBitmapMain.GetOutputStreamBuffer()->GetBufferPos() - (char*)BitmapMain;
    int Bitmap16Len   = (char*)StreamBitmap16.GetOutputStreamBuffer()->GetBufferPos() - (char*)Bitmap16;
    int Bitmap32Len   = (char*)StreamBitmap32.GetOutputStreamBuffer()->GetBufferPos() - (char*)Bitmap32;

    Manager->WriteBinary(BasePath+_T("/Info/Icon16"),Bitmap16,Bitmap16Len);
    Manager->WriteBinary(BasePath+_T("/Info/Icon32"),Bitmap32,Bitmap32Len);
    Manager->WriteBinary(BasePath+_T("/Bitmap"),BitmapMain,BitmapMainLen);

    Manager->Write(BasePath+_T("/Info/License"),m_Info.License);
    Manager->Write(BasePath+_T("/Info/Author"),m_Info.Author);
    Manager->Write(BasePath+_T("/Info/EMail"),m_Info.Email);
    Manager->Write(BasePath+_T("/Info/Site"),m_Info.Site);
    Manager->Write(BasePath+_T("/Info/Category"),m_Info.Category);
    Manager->Write(BasePath+_T("/Info/Priority"),(int)m_Info.Priority);
    Manager->Write(BasePath+_T("/Info/DefaultVarName"),m_Info.DefaultVarName);
    Manager->Write(BasePath+_T("/Info/Languages"),(int)m_Info.Languages);
    Manager->Write(BasePath+_T("/Info/VerHi"),(int)m_Info.VerHi);
    Manager->Write(BasePath+_T("/Info/VerLo"),(int)m_Info.VerLo);
    Manager->Write(BasePath+_T("/Info/AllowInXRC"),m_Info.AllowInXRC);
    Manager->Write(BasePath+_T("/DefaultSize"),wxString::Format(_T("%d,%d"),m_DefaultSize.GetWidth(),m_DefaultSize.GetHeight()));
    Manager->Write(BasePath+_T("/Cpp/CodeTemplate"),m_CppCodeTemplate);
    Manager->Write(BasePath+_T("/Cpp/DeclarationHeaders"),m_CppDeclarationHeaders);
    Manager->Write(BasePath+_T("/Cpp/DefinitionHeaders"),m_CppDefinitionHeaders);

    m_Properties.WriteToConfig(Manager,BasePath+_T("/Properties"));
}

void wxsUserItemDescription::AddReference()
{
    m_RefCount++;
}

void wxsUserItemDescription::DecReference(bool DeleteFactory)
{
    if ( DeleteFactory )
    {
        delete m_Factory;
        m_Factory = NULL;
    }

    if ( !--m_RefCount )
    {
        delete this;
    }
}

void wxsUserItemDescription::Unregister()
{
    delete m_Factory;
    m_Factory = NULL;
}

void wxsUserItemDescription::Reregister()
{
    Unregister();
    m_Factory = new wxsUserItemFactory(this);
}

void wxsUserItemDescription::SetName(const wxString& NewName)
{
    m_Name = NewName;
    m_Info.ClassName = NewName;
}

void wxsUserItemDescription::UpdateTreeIcon()
{
    delete m_TreeImage;
    m_TreeImage = new wxsAutoResourceTreeImage(m_Info.Icon16);
    m_Info.TreeIconId = m_TreeImage->GetIndex();
}
