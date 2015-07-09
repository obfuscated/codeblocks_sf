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

#include "wxssizerflagsproperty.h"

#include <wx/tokenzr.h>
#include <globals.h>

// Helper macro for fetching variables
#define FLAGS   wxsVARIABLE(Object,Offset,long)

#define BORDERIND   0x01
#define ALIGNHIND   0x02
#define ALIGNVIND   0x03
#define EXPANDIND   0x04
#define SHAPEDIND   0x05
#define FIXEDIND    0x06

wxsSizerFlagsProperty::wxsSizerFlagsProperty(long _Offset,int Priority):
        wxsProperty(_("Borders"),_T("flag"),Priority),
        Offset(_Offset)
{
}

void wxsSizerFlagsProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    #if !(wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0))
    wxPGId ID1, ID2, ID3, ID4, ID5, ID6;
    #endif
    // Creating border flags

    if ( (FLAGS & BorderMask) == BorderMask )
    {
        FLAGS |= BorderAll | BorderPrevAll;
    }
    else
    {
        FLAGS &= ~BorderAll & ~BorderPrevAll;
    }

    wxPGChoices PGC;
    PGC.Add(_("Top"),BorderTop);
    PGC.Add(_("Bottom"),BorderBottom);
    PGC.Add(_("Left"),BorderLeft);
    PGC.Add(_("Right"),BorderRight);
    PGC.Add(_("All"),BorderAll);
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    wxPGId ID1 = Grid->AppendIn(Parent,new wxFlagsProperty(_("Border"),wxPG_LABEL,PGC,FLAGS&(BorderMask|BorderAll)));
    PGRegister(Object,Grid,ID1,BORDERIND);
    #else
    PGRegister(Object,Grid,ID1 = Grid->AppendIn(Parent, wxFlagsProperty(_("Border"),wxPG_LABEL,PGC,FLAGS&(BorderMask|BorderAll))),BORDERIND);
    #endif

    wxPGChoices PGC2;
    PGC2.Add(_(""),AlignNot);
    PGC2.Add(_("Left"),AlignLeft);
    PGC2.Add(_("Center"),AlignCenterHorizontal);
    PGC2.Add(_("Right"),AlignRight);
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    wxPGId ID2 = Grid->AppendIn(Parent,new wxEnumProperty(_("Horizontal align"),wxPG_LABEL,PGC2,FLAGS&AlignHMask));
    PGRegister(Object,Grid,ID2,ALIGNHIND);
    #else
    PGRegister(Object,Grid,ID2 = Grid->AppendIn(Parent,NEW_IN_WXPG14X wxEnumProperty(_("Horizontal align"),wxPG_LABEL,PGC2,FLAGS&AlignHMask)),ALIGNHIND);
    #endif

    wxPGChoices PGC3;
    PGC3.Add(_(""),AlignNot);
    PGC3.Add(_("Top"),AlignTop);
    PGC3.Add(_("Center"),AlignCenterVertical);
    PGC3.Add(_("Bottom"),AlignBottom);
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    wxPGId ID3 = Grid->AppendIn(Parent,new wxEnumProperty(_("Vertical align"),wxPG_LABEL,PGC3,FLAGS&AlignVMask));
    PGRegister(Object,Grid,ID3,ALIGNVIND);
    #else
    PGRegister(Object,Grid,ID3 = Grid->AppendIn(Parent, wxEnumProperty(_("Vertical align"),wxPG_LABEL,PGC3,FLAGS&AlignVMask)),ALIGNVIND);
    #endif

    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    wxPGId ID4 = Grid->AppendIn(Parent,new wxBoolProperty(_("Expand"),wxPG_LABEL,(FLAGS&Expand)!=0));
    wxPGId ID5 = Grid->AppendIn(Parent,new wxBoolProperty(_("Shaped"),wxPG_LABEL,(FLAGS&Shaped)!=0));
    wxPGId ID6 = Grid->AppendIn(Parent,new wxBoolProperty(_("Fixed min size"),wxPG_LABEL,(FLAGS&FixedMinSize)!=0));
    PGRegister(Object,Grid,ID4,EXPANDIND);
    PGRegister(Object,Grid,ID5,SHAPEDIND);
    PGRegister(Object,Grid,ID6,FIXEDIND);
    #else
    PGRegister(Object,Grid,ID4 = Grid->AppendIn(Parent, wxBoolProperty(_("Expand"),wxPG_LABEL,(FLAGS&Expand)!=0)),EXPANDIND);
    PGRegister(Object,Grid,ID5 = Grid->AppendIn(Parent, wxBoolProperty(_("Shaped"),wxPG_LABEL,(FLAGS&Shaped)!=0)),SHAPEDIND);
    PGRegister(Object,Grid,ID6 = Grid->AppendIn(Parent, wxBoolProperty(_("Fixed min size"),wxPG_LABEL,(FLAGS&FixedMinSize)!=0)),FIXEDIND);
    #endif

    Grid->SetPropertyAttribute(ID1,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    Grid->SetPropertyAttribute(ID2,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    Grid->SetPropertyAttribute(ID3,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    Grid->SetPropertyAttribute(ID4,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    Grid->SetPropertyAttribute(ID5,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
    Grid->SetPropertyAttribute(ID6,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
}

bool wxsSizerFlagsProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case BORDERIND:
            {
                long NewVal = Grid->GetPropertyValue(Id).GetLong();
                bool ThisAll = (NewVal&BorderAll) != 0;
                bool PrevAll = (FLAGS&BorderPrevAll) != 0;
                // Checking if "all" flag has changed
                if ( ThisAll != PrevAll )
                {
                    if ( ThisAll )
                    {
                        FLAGS |= BorderMask | BorderAll | BorderPrevAll;
                    }
                    else
                    {
                        FLAGS &= ~BorderMask & ~BorderAll & ~BorderPrevAll;
                    }
                }
                else
                {
                    NewVal &= BorderMask;
                    FLAGS &= ~BorderMask;
                    FLAGS |= NewVal;
                    if ( NewVal == BorderMask )
                    {
                        FLAGS |= BorderAll | BorderPrevAll;
                    }
                    else
                    {
                        FLAGS &= ~BorderAll & ~BorderPrevAll;
                    }
                }
            }
            break;

        case ALIGNHIND:
            FLAGS &= ~AlignHMask;
            FLAGS |= (Grid->GetPropertyValue(Id).GetLong() & AlignHMask);
            break;

        case ALIGNVIND:
            FLAGS &= ~AlignVMask;
            FLAGS |= (Grid->GetPropertyValue(Id).GetLong() & AlignVMask);
            break;

        case EXPANDIND:
            if ( Grid->GetPropertyValue(Id).GetBool() )
            {
                FLAGS |= Expand;
            }
            else
            {
                FLAGS &= ~Expand;
            }
            break;

        case SHAPEDIND:
            if ( Grid->GetPropertyValue(Id).GetBool() )
            {
                FLAGS |= Shaped;
            }
            else
            {
                FLAGS &= ~Shaped;
            }
            break;

        case FIXEDIND:
            if ( Grid->GetPropertyValue(Id).GetBool() )
            {
                FLAGS |= FixedMinSize;
            }
            else
            {
                FLAGS &= ~FixedMinSize;
            }
            break;

        default:
            return false;
    }
    return true;
}

bool wxsSizerFlagsProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case BORDERIND:
            if ( (FLAGS & BorderMask) == BorderMask )
            {
                FLAGS |= BorderAll | BorderPrevAll;
            }
            else
            {
                FLAGS &= ~BorderAll & ~BorderPrevAll;
            }
            Grid->SetPropertyValue(Id,FLAGS&(BorderMask|BorderAll));
            break;

        case ALIGNHIND:
            Grid->SetPropertyValue(Id,FLAGS&AlignHMask);
            break;

        case ALIGNVIND:
            Grid->SetPropertyValue(Id,FLAGS&AlignVMask);
            break;

        case EXPANDIND:
            Grid->SetPropertyValue(Id,(FLAGS&Expand)!=0);
            break;

        case SHAPEDIND:
            Grid->SetPropertyValue(Id,(FLAGS&Shaped)!=0);
            break;

        case FIXEDIND:
            Grid->SetPropertyValue(Id,(FLAGS&FixedMinSize)!=0);
            break;

        default:
            return false;
    }
    return true;
}

long wxsSizerFlagsProperty::GetParentOrientation(TiXmlElement* Element)
{
    if ( Element->Parent() && Element->Parent()->Parent() )
    {
        TiXmlNode* p = Element->Parent()->Parent();
        TiXmlElement* e = p->ToElement();
        if ( e &&( !strcmp(e->Attribute("class"), "wxBoxSizer") || !strcmp(e->Attribute("class"), "wxStaticBoxSizer") ) )
        {
            if ( p->FirstChild("orient") && p->FirstChild("orient")->ToElement() )
            {
                const char* value = p->FirstChild("orient")->ToElement()->GetText();
                if ( !strcmp(value, "wxVERTICAL") )
                    return ParentAlignVertical;
                else if ( !strcmp(value, "wxHORIZONTAL") )
                    return ParentAlignHorizontal;
                else return 0;
            }
            else
                return ParentAlignHorizontal;
        }
    }
    return 0;
}

bool wxsSizerFlagsProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        FLAGS = AlignNot;
        return false;
    }

    FLAGS &= ~ParentAlignMask;
    FLAGS |= GetParentOrientation(Element);

    const char* Text = Element->GetText();
    if ( !Text )
    {
        FLAGS = AlignNot;
        return false;
    }
    FLAGS = ParseString(cbC2U(Text));
    return true;
}

bool wxsSizerFlagsProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( Element )
    {
        FLAGS &= ~ParentAlignMask;
        FLAGS |= GetParentOrientation(Element);
    }

    FixFlags(FLAGS);

    Element->InsertEndChild(TiXmlText(cbU2C(GetString(FLAGS))));

    return true;
}

bool wxsSizerFlagsProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    if ( Stream->GetLong(GetDataName(),FLAGS,AlignNot) )
    {
        FixFlags(FLAGS);
        return true;
    }
    return false;
}

bool wxsSizerFlagsProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),FLAGS,AlignNot);
}

long wxsSizerFlagsProperty::ParseString(const wxString& String)
{
    long Flags = 0;
    wxStringTokenizer Tkn(String, _T("| \t\n"), wxTOKEN_STRTOK);
    while ( Tkn.HasMoreTokens() )
    {
        wxString Flag = Tkn.GetNextToken();
             if ( Flag == _T("wxTOP")           ) Flags |= BorderTop;
        else if ( Flag == _T("wxNORTH")         ) Flags |= BorderTop;
        else if ( Flag == _T("wxBOTTOM")        ) Flags |= BorderBottom;
        else if ( Flag == _T("wxSOUTH")         ) Flags |= BorderBottom;
        else if ( Flag == _T("wxLEFT")          ) Flags |= BorderLeft;
        else if ( Flag == _T("wxWEST")          ) Flags |= BorderLeft;
        else if ( Flag == _T("wxRIGHT")         ) Flags |= BorderRight;
        else if ( Flag == _T("wxEAST")          ) Flags |= BorderLeft;
        else if ( Flag == _T("wxALL")           ) Flags |= BorderMask;
        else if ( Flag == _T("wxEXPAND")        ) Flags |= Expand;
        else if ( Flag == _T("wxGROW")          ) Flags |= Expand;
        else if ( Flag == _T("wxSHAPED")        ) Flags |= Shaped;
        else if ( Flag == _T("wxFIXED_MINSIZE") ) Flags |= FixedMinSize;
        else if ( Flag == _T("wxALIGN_CENTER")  ) Flags |= AlignCenterHorizontal | AlignCenterVertical;
        else if ( Flag == _T("wxALIGN_CENTRE")  ) Flags |= AlignCenterHorizontal | AlignCenterVertical;
        else if ( Flag == _T("wxALIGN_LEFT")    ) Flags |= AlignLeft;
        else if ( Flag == _T("wxALIGN_RIGHT")   ) Flags |= AlignRight;
        else if ( Flag == _T("wxALIGN_TOP")     ) Flags |= AlignTop;
        else if ( Flag == _T("wxALIGN_BOTTOM")  ) Flags |= AlignBottom;
        else if ( Flag == _T("wxALIGN_CENTER_HORIZONTAL") ) Flags |= AlignCenterHorizontal;
        else if ( Flag == _T("wxALIGN_CENTRE_HORIZONTAL") ) Flags |= AlignCenterHorizontal;
        else if ( Flag == _T("wxALIGN_CENTER_VERTICAL")   ) Flags |= AlignCenterVertical;
        else if ( Flag == _T("wxALIGN_CENTRE_VERTICAL")   ) Flags |= AlignCenterVertical;
    }
    FixFlags(Flags);
    return Flags;
}

wxString wxsSizerFlagsProperty::GetString(long Flags)
{
    wxString Result;

    if ( (Flags & BorderMask) == BorderMask )
    {
        Result = _T("wxALL|");
    }
    else
    {
        if ( Flags & BorderTop    ) Result.Append(_T("wxTOP|"));
        if ( Flags & BorderBottom ) Result.Append(_T("wxBOTTOM|"));
        if ( Flags & BorderLeft   ) Result.Append(_T("wxLEFT|"));
        if ( Flags & BorderRight  ) Result.Append(_T("wxRIGHT|"));
    }

    if ( Flags & Expand )
    {
        Result.Append(_T("wxEXPAND|"));
    }
    else
    {
        if ( Flags & AlignLeft              ) Result.Append(_T("wxALIGN_LEFT|"));
        if ( Flags & AlignRight             ) Result.Append(_T("wxALIGN_RIGHT|"));
        if ( Flags & AlignTop               ) Result.Append(_T("wxALIGN_TOP|"));
        if ( Flags & AlignBottom            ) Result.Append(_T("wxALIGN_BOTTOM|"));
        if ( Flags & AlignCenterHorizontal  ) Result.Append(_T("wxALIGN_CENTER_HORIZONTAL|"));
        if ( Flags & AlignCenterVertical    ) Result.Append(_T("wxALIGN_CENTER_VERTICAL|"));
    }
    if ( Flags & Shaped                 ) Result.Append(_T("wxSHAPED|"));
    if ( Flags & FixedMinSize           ) Result.Append(_T("wxFIXED_MINSIZE|"));

    if ( Result.empty() )
    {
        // do not return "0" to avoid asserts in xrc-files
        return _T("wxALIGN_NOT");
    }

    Result.RemoveLast();
    return Result;
}

long wxsSizerFlagsProperty::GetWxFlags(long Flags)
{
    long Result = 0;

    if ( Flags & BorderTop             ) Result |= wxTOP;
    if ( Flags & BorderBottom          ) Result |= wxBOTTOM;
    if ( Flags & BorderLeft            ) Result |= wxLEFT;
    if ( Flags & BorderRight           ) Result |= wxRIGHT;
    if ( Flags & Expand                ) Result |= wxEXPAND;
    if ( Flags & Shaped                ) Result |= wxSHAPED;
    if ( Flags & FixedMinSize          ) Result |= wxFIXED_MINSIZE;
    if ( Flags & AlignLeft             ) Result |= wxALIGN_LEFT;
    if ( Flags & AlignRight            ) Result |= wxALIGN_RIGHT;
    if ( Flags & AlignTop              ) Result |= wxALIGN_TOP;
    if ( Flags & AlignBottom           ) Result |= wxALIGN_BOTTOM;
    if ( Flags & AlignCenterHorizontal ) Result |= wxALIGN_CENTER_HORIZONTAL;
    if ( Flags & AlignCenterVertical   ) Result |= wxALIGN_CENTER_VERTICAL;

    return Result;
}

void wxsSizerFlagsProperty::FixFlags(long& Flags)
{
    if ( Flags & Expand )
    {
        Flags &= ~(AlignHMask|AlignVMask);
    }
    else
    {
        if ( Flags & ParentAlignVertical )
        {
            Flags &= ~AlignVMask;
        }

        if ( Flags & ParentAlignHorizontal )
        {
            Flags &= ~AlignHMask;
        }

        if ( Flags & AlignLeft )
        {
            Flags &= ~(AlignCenterHorizontal|AlignRight);
        }
        else if ( Flags & AlignCenterHorizontal )
        {
            Flags &= ~AlignRight;
        }

        if ( Flags & AlignTop )
        {
            Flags &= ~(AlignCenterVertical|AlignBottom);
        }
        else if ( Flags & AlignCenterVertical )
        {
            Flags &= ~AlignBottom;
        }
    }
}
