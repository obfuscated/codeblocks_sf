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

wxsSizerFlagsProperty::wxsSizerFlagsProperty(long _Offset):
        wxsProperty(_("Borders"),_T("flag")),
        Offset(_Offset)
{
}

void wxsSizerFlagsProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId ID1, ID2, ID3, ID4, ID5, ID6;
    // Creating border flags

    if ( (FLAGS & BorderMask) == BorderMask )
    {
        FLAGS |= BorderAll | BorderPrevAll;
    }
    else
    {
        FLAGS &= ~BorderAll & ~BorderPrevAll;
    }

    wxPGConstants PGC;
    PGC.Add(_("Top"),BorderTop);
    PGC.Add(_("Bottom"),BorderBottom);
    PGC.Add(_("Left"),BorderLeft);
    PGC.Add(_("Right"),BorderRight);
    PGC.Add(_("All"),BorderAll);
    PGRegister(Object,Grid,ID1 = Grid->AppendIn(Parent,wxFlagsProperty(_("Border"),wxPG_LABEL,PGC,FLAGS&(BorderMask|BorderAll))),BORDERIND);

    wxPGConstants PGC2;
    PGC2.Add(_("Left"),AlignLeft);
    PGC2.Add(_("Center"),AlignCenterHorizontal);
    PGC2.Add(_("Right"),AlignRight);
    PGRegister(Object,Grid,ID2 = Grid->AppendIn(Parent,wxEnumProperty(_("Horizontal align"),wxPG_LABEL,PGC2,FLAGS&AlignHMask)),ALIGNHIND);

    wxPGConstants PGC3;
    PGC3.Add(_("Top"),AlignTop);
    PGC3.Add(_("Center"),AlignCenterVertical);
    PGC3.Add(_("Bottom"),AlignBottom);
    PGRegister(Object,Grid,ID3 = Grid->AppendIn(Parent,wxEnumProperty(_("Vertical align"),wxPG_LABEL,PGC3,FLAGS&AlignVMask)),ALIGNVIND);

    PGRegister(Object,Grid,ID4 = Grid->AppendIn(Parent,wxBoolProperty(_("Expand"),wxPG_LABEL,(FLAGS&Expand)!=0)),EXPANDIND);
    PGRegister(Object,Grid,ID5 = Grid->AppendIn(Parent,wxBoolProperty(_("Shaped"),wxPG_LABEL,(FLAGS&Shaped)!=0)),SHAPEDIND);
    PGRegister(Object,Grid,ID6 = Grid->AppendIn(Parent,wxBoolProperty(_("Fixed min size"),wxPG_LABEL,(FLAGS&FixedMinSize)!=0)),FIXEDIND);

    Grid->SetPropertyAttribute(ID1,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    Grid->SetPropertyAttribute(ID2,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    Grid->SetPropertyAttribute(ID3,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    Grid->SetPropertyAttribute(ID4,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    Grid->SetPropertyAttribute(ID5,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
    Grid->SetPropertyAttribute(ID6,wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
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

bool wxsSizerFlagsProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        FLAGS = AlignLeft | AlignTop;
        return false;
    }

    const char* Text = Element->GetText();
    wxString Str;
    if ( !Text )
    {
        FLAGS = AlignLeft | AlignTop;
        return false;
    }
    FLAGS = ParseString(cbC2U(Text));
    return true;
}

bool wxsSizerFlagsProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    Element->InsertEndChild(TiXmlText(cbU2C(GetString(FLAGS))));
    return true;
}

bool wxsSizerFlagsProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    if ( Stream->GetLong(GetDataName(),FLAGS,AlignTop|AlignLeft) )
    {
        FixFlags(FLAGS);
        return true;
    }
    return false;
}

bool wxsSizerFlagsProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return Stream->PutLong(GetDataName(),FLAGS,AlignTop|AlignLeft);
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

    if ( Flags & Expand                 ) Result.Append(_T("wxEXPAND|"));
    if ( Flags & Shaped                 ) Result.Append(_T("wxSHAPED|"));
    if ( Flags & FixedMinSize           ) Result.Append(_T("wxFIXED_MINSIZE|"));
    if ( Flags & AlignLeft              ) Result.Append(_T("wxALIGN_LEFT|"));
    if ( Flags & AlignRight             ) Result.Append(_T("wxALIGN_RIGHT|"));
    if ( Flags & AlignTop               ) Result.Append(_T("wxALIGN_TOP|"));
    if ( Flags & AlignBottom            ) Result.Append(_T("wxALIGN_BOTTOM|"));
    if ( Flags & AlignCenterHorizontal  ) Result.Append(_T("wxALIGN_CENTER_HORIZONTAL|"));
    if ( Flags & AlignCenterVertical    ) Result.Append(_T("wxALIGN_CENTER_VERTICAL|"));

    if ( Result.empty() )
    {
        return _T("0");
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
    if ( Flags & AlignLeft )
    {
        Flags &= ~(AlignCenterHorizontal|AlignRight);
    }
    else if ( Flags & AlignCenterHorizontal )
    {
        Flags &= ~AlignRight;
    }
    else if ( ! (Flags & AlignRight) )
    {
        Flags |= AlignLeft;
    }

    if ( Flags & AlignTop )
    {
        Flags &= ~(AlignCenterVertical|AlignBottom);
    }
    else if ( Flags & AlignCenterVertical )
    {
        Flags &= ~AlignBottom;
    }
    else if ( ! (Flags & AlignBottom) )
    {
        Flags |= AlignTop;
    }
}
