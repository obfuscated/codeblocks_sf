/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#include "wxschart.h"

#include <wx/barchartpoints.h>
#include <wx/bar3dchartpoints.h>
#include <wx/piechartpoints.h>
#include <wx/pie3dchartpoints.h>

namespace
{
    // Loading images from xpm files
    #include "wxchart16.xpm"
    #include "wxchart32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsChart> Reg(
        _T("wxChartCtrl"),              // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Paolo Gava"),               // Author
        _T("paolo_gava@hotmail.com"),   // Author's email
        _T("http://wxcode.sourceforge.net/components/wxchart/"),    // Item's homepage
        _T("Contrib"),                  // Category in palette
        100,                             // Priority in palette
        _T("Chart"),                    // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(wxchart32_xpm),        // 32x32 bitmap
        wxBitmap(wxchart16_xpm),        // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files

    // Defining styles
    WXS_ST_BEGIN(wxsChartStyles,_T("wxSIMPLE_BORDER"))
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    static const long DEFAULT_STYLE_FIX = 0x1000;
    static const long Values[] = { USE_AXIS_X, USE_AXIS_Y, USE_LEGEND, USE_ZOOM_BUT, USE_DEPTH_BUT, USE_GRID, DEFAULT_STYLE_FIX };
    static const wxChar* Names[] = { _T("USE_AXIS_X"), _T("USE_AXIS_Y"), _T("USE_LEGEND"), _T("USE_ZOOM_BUT"), _T("USE_DEPTH_BUT"), _T("USE_GRID"), _T("DEFAULT_STYLE"), NULL };

}

wxsChart::wxsChart(wxsItemResData* Data):
    wxsWidget(
        Data,               // Data passed to constructor
        &Reg.Info,          // Info taken from Registering object previously created
        NULL,               // Structure describing events, we have no events for wxChart
        wxsChartStyles)     // Structure describing styles
{
    m_Flags = DEFAULT_STYLE_FIX;
}

wxsChart::~wxsChart()
{
    for ( size_t i=0; i<m_ChartPointsDesc.Count(); i++ )
    {
        delete m_ChartPointsDesc[i];
    }
    m_ChartPointsDesc.Clear();
}

void wxsChart::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/chartctrl.h>"),GetInfo().ClassName);
            AddHeader(_T("<wx/barchartpoints.h>"),_T(""),hfLocal);
            AddHeader(_T("<wx/bar3dchartpoints.h>"),_T(""),hfLocal);
            AddHeader(_T("<wx/piechartpoints.h>"),_T(""),hfLocal);
            AddHeader(_T("<wx/pie3dchartpoints.h>"),_T(""),hfLocal);

            wxString StyleCode;
            for ( int i=0; Names[i]; i++ )
            {
                if ( m_Flags & Values[i] ) StyleCode << Names[i] << _T("|");
            }

            if ( StyleCode.IsEmpty() ) StyleCode = _T("0");
            else                       StyleCode.RemoveLast();

            Codef(_T("%C(%W,%I,(wxChartStyle)(%s),%P,%S,%T);\n"),StyleCode.wx_str());

            Codef(_T("{\n"));
            for ( size_t i=0; i<m_ChartPointsDesc.Count(); i++ )
            {
                ChartPointsDesc* Desc = m_ChartPointsDesc[i];

                wxString GenStr;
                switch ( Desc->Type )
                {
                    case Bar:      GenStr = _T("wxBarChartPoints::CreateWxBarChartPoints"); break;
                    case Bar3D:    GenStr = _T("wxBar3DChartPoints::CreateWxBar3DChartPoints"); break;
                    case Pie:      GenStr = _T("wxPieChartPoints::CreateWxPieChartPoints"); break;
                    case Pie3D:    GenStr = _T("wxPie3DChartPoints::CreateWxPie3DChartPoints"); break;
                    case Points:   GenStr = _T("wxPointsCharPoints::CreateWxPointsChartPoints"); break;
                    case Points3D: GenStr = _T("wxPoints3DCharPoints::CreateWxPoints3DChartPoints"); break;
                    case Line:     GenStr = _T("wxLineCharPoints::CreateWxLineChartPoints"); break;
                    case Line3D:   GenStr = _T("wxLine3DCharPoints::CreateWxLine3DChartPoints"); break;
                    case Area:     GenStr = _T("wxAreaCharPoints::CreateWxAreaChartPoints"); break;
                    case Area3D:   GenStr = _T("wxArea3DCharPoints::CreateWxArea3DChartPoints"); break;
                    default:       GenStr = _T("wxBarChartPoints::CreateWxBarChartPoints"); break;
                }

                wxString VarStr = wxString::Format(_T("PointSet%d"),(int)i);

                Codef(_T("\twxChartPoints* %v = %s(%t);\n"),VarStr.wx_str(),GenStr.wx_str(),Desc->Name.wx_str());

                for ( size_t j=0; j<Desc->Points.Count(); j++ )
                {
                    wxString PointStr = wxString::Format(_T("%lf,%lf"),Desc->Points[j]->X,Desc->Points[j]->Y);
                    Codef(_T("\t%v->Add(%t,%s);\n"),VarStr.wx_str(),Desc->Points[j]->Name.wx_str(),PointStr.wx_str());
                }

                Codef(_T("\t%AAdd(%v);\n"),VarStr.wx_str());
            }
            Codef(_T("}\n"));

            break;
        }

        default:
            wxsCodeMarks::Unknown(_T("wxsChart::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsChart::OnBuildPreview(wxWindow* Parent,long Flags)
{
    long RealFlags = m_Flags;
    if ( RealFlags & DEFAULT_STYLE_FIX ) RealFlags |= DEFAULT_STYLE;
    wxChartCtrl* Chart = new wxChartCtrl(Parent,GetId(),(wxChartStyle)RealFlags,Pos(Parent),Size(Parent),Style());

    for ( size_t i=0; i<m_ChartPointsDesc.Count(); i++ )
    {
        ChartPointsDesc* Desc = m_ChartPointsDesc[i];
        wxChartPoints* Points = NULL;

        switch ( Desc->Type )
        {
            case Bar:      Points = wxBarChartPoints::CreateWxBarChartPoints(Desc->Name); break;
            case Bar3D:    Points = wxBar3DChartPoints::CreateWxBar3DChartPoints(Desc->Name); break;
            case Pie:      Points = wxPieChartPoints::CreateWxPieChartPoints(Desc->Name); break;
            case Pie3D:    Points = wxPie3DChartPoints::CreateWxPie3DChartPoints(Desc->Name); break;
            /*
            case Points:   Points = wxPointsCharPoints::CreateWxPointsChartPoints(Desc->Name); break;
            case Points3D: Points = wxPoints3DCharPoints::CreateWxPoints3DChartPoints(Desc->Name); break;
            case Line:     Points = wxLineCharPoints::CreateWxLineChartPoints(Desc->Name); break;
            case Line3D:   Points = wxLine3DCharPoints::CreateWxLine3DChartPoints(Desc->Name); break;
            case Area:     Points = wxAreaCharPoints::CreateWxAreaChartPoints(Desc->Name); break;
            case Area3D:   Points = wxArea3DCharPoints::CreateWxArea3DChartPoints(Desc->Name); break;
            */
            default:       Points = wxBarChartPoints::CreateWxBarChartPoints(Desc->Name); break;
        }

        for ( size_t j=0; j<Desc->Points.Count(); j++ )
        {
            Points->Add(Desc->Points[j]->Name,Desc->Points[j]->X,Desc->Points[j]->Y);
        }

        Chart->Add(Points);
    }

    return Chart;
}

void wxsChart::OnEnumWidgetProperties(long Flags)
{
    WXS_FLAGS(wxsChart,m_Flags,_("wxChart style"),_T("wxchart_style"),Values,Names, DEFAULT_STYLE_FIX )
}

void wxsChart::OnAddExtraProperties(wxsPropertyGridManager* Grid)
{
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
    #else
    Grid->SetTargetPage(0);
    #endif


    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    m_ChartPointsCountId = Grid->Append(new wxIntProperty(_("Number of data sets"),wxPG_LABEL,
    #else
    m_ChartPointsCountId = Grid->Append(wxIntProperty(_("Number of data sets"),wxPG_LABEL,
    #endif
                                                      (int)m_ChartPointsDesc.Count()));

    for ( int i=0; i<(int)m_ChartPointsDesc.Count(); i++ )
    {
        AppendPropertyForSet(Grid,i);
    }

    wxsWidget::OnAddExtraProperties(Grid);
}

void wxsChart::OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id)
{
    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
    #else
    Grid->SetTargetPage(0);
    #endif

    if ( Id == m_ChartPointsCountId )
    {
        int OldValue = (int)m_ChartPointsDesc.Count();
        int NewValue = Grid->GetPropertyValueAsInt(Id);

        if ( NewValue<0 )
        {
            NewValue = 0;
            Grid->SetPropertyValue(Id,NewValue);
        }

        if ( NewValue > OldValue )
        {
            // We have to generate new entries
            for ( int i=OldValue; i<NewValue; i++ )
            {
                m_ChartPointsDesc.Add(new ChartPointsDesc());
                AppendPropertyForSet(Grid,i);
            }
        }
        else if ( NewValue < OldValue )
        {
            // We have to remove some entries
            for ( int i=NewValue; i<OldValue; i++ )
            {
                #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_ChartPointsDesc[i]->Id);
                #else
                Grid->Delete(m_ChartPointsDesc[i]->Id);
                #endif
                delete m_ChartPointsDesc[i];
            }

            m_ChartPointsDesc.RemoveAt(NewValue,OldValue-NewValue);
        }

        NotifyPropertyChange(true);
        return;
    }

    for ( int i=0; i<(int)m_ChartPointsDesc.Count(); i++ )
    {
        if ( HandleChangeInSet(Grid,Id,i) ) return;
    }

    wxsWidget::OnExtraPropertyChanged(Grid,Id);
}

bool wxsChart::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    for ( size_t i=0; i<m_ChartPointsDesc.Count(); i++ )
    {
        delete m_ChartPointsDesc[i];
    }
    m_ChartPointsDesc.Clear();

    for ( TiXmlElement* DescElem = Element->FirstChildElement("chartpointset");
          DescElem;
          DescElem = DescElem->NextSiblingElement("chartpointset") )
    {
        ChartPointsDesc* Desc = new ChartPointsDesc;
        Desc->Name    = cbC2U(DescElem->Attribute("name"));
        wxString Type = cbC2U(DescElem->Attribute("type"));

        if ( Type == _T("bar") )      Desc->Type = Bar;      else
        if ( Type == _T("bar3d") )    Desc->Type = Bar3D;    else
        if ( Type == _T("pie") )      Desc->Type = Pie;      else
        if ( Type == _T("pie3d") )    Desc->Type = Pie3D;    else
        /*
        if ( Type == _T("points") )   Desc->Type = Points;   else
        if ( Type == _T("points3d") ) Desc->Type = Points3D; else
        if ( Type == _T("line") )     Desc->Type = Line;     else
        if ( Type == _T("line3d") )   Desc->Type = Line3D;   else
        if ( Type == _T("area") )     Desc->Type = Area;     else
        if ( Type == _T("area3d") )   Desc->Type = Area3D;   else
        */
                                      Desc->Type = Bar;

        for ( TiXmlElement* PointElem = DescElem->FirstChildElement("point");
              PointElem;
              PointElem = PointElem->NextSiblingElement("point") )
        {
            PointDesc* Point = new PointDesc;
            Point->Name = cbC2U(PointElem->Attribute("name"));
            (PointElem->QueryDoubleAttribute("x",&Point->X) == TIXML_SUCCESS) || (Point->X = 0.0);
            (PointElem->QueryDoubleAttribute("y",&Point->Y) == TIXML_SUCCESS) || (Point->Y = 0.0);

            Desc->Points.Add(Point);
        }

        m_ChartPointsDesc.Add(Desc);
    }

    return wxsWidget::OnXmlRead(Element,IsXRC,IsExtra);
}

bool wxsChart::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    for ( size_t i=0; i<m_ChartPointsDesc.Count(); i++ )
    {
        ChartPointsDesc* Desc = m_ChartPointsDesc[i];
        TiXmlElement* DescElem = Element->InsertEndChild(TiXmlElement("chartpointset"))->ToElement();

        DescElem->SetAttribute("name",cbU2C(Desc->Name));
        switch ( Desc->Type )
        {
            case Bar:      DescElem->SetAttribute("type","bar");      break;
            case Bar3D:    DescElem->SetAttribute("type","bar3d");    break;
            case Pie:      DescElem->SetAttribute("type","pie");      break;
            case Pie3D:    DescElem->SetAttribute("type","pie3d");    break;
            case Points:   DescElem->SetAttribute("type","points");   break;
            case Points3D: DescElem->SetAttribute("type","points3d"); break;
            case Line:     DescElem->SetAttribute("type","line");     break;
            case Line3D:   DescElem->SetAttribute("type","line3d");   break;
            case Area:     DescElem->SetAttribute("type","area");     break;
            case Area3D:   DescElem->SetAttribute("type","area3d");   break;
        }

        for ( size_t j=0; j<Desc->Points.Count(); j++ )
        {
            PointDesc* PDesc = Desc->Points[j];
            TiXmlElement* PointElem = DescElem->InsertEndChild(TiXmlElement("point"))->ToElement();
            PointElem->SetAttribute("name",cbU2C(PDesc->Name));
            PointElem->SetDoubleAttribute("x",PDesc->X);
            PointElem->SetDoubleAttribute("y",PDesc->Y);
        }
    }

    return wxsWidget::OnXmlWrite(Element,IsXRC,IsExtra);
}

void wxsChart::AppendPropertyForSet(wxsPropertyGridManager* Grid,int Position)
{
    ChartPointsDesc* Desc = m_ChartPointsDesc[Position];
    wxString SetName = wxString::Format(_("Set %d"),Position+1);

    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Desc->Id = Grid->Append(new wxParentProperty(SetName,wxPG_LABEL));
    #else
    Desc->Id = Grid->Append(wxParentProperty(SetName,wxPG_LABEL));
    #endif

    static const wxChar* Types[] =
    {
        _T("Bar"),    _T("Bar3D"),    _T("Pie"),  _T("Pie3D"),
        NULL, // wxChartCtrl doesn't support all types yet
        _T("Points"), _T("Points3D"), _T("Line"), _T("Line3D"),
        _T("Area"),   _T("Area3D"),   NULL
    };

    static const long Values[] =
    {
        Bar, Bar3D, Pie, Pie3D, Points, Points3D, Line, Line3D, Area, Area3D
    };

    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Desc->TypeId = Grid->AppendIn(Desc->Id,new wxEnumProperty(_("Type"),wxPG_LABEL,Types,Values,Desc->Type));
    Desc->NameId = Grid->AppendIn(Desc->Id,new wxStringProperty(_("Name"),wxPG_LABEL,Desc->Name));
    Desc->PointsCountId = Grid->AppendIn(Desc->Id,new wxIntProperty(_("Number of points"),wxPG_LABEL,(int)Desc->Points.Count()));
    #else
    Desc->TypeId = Grid->AppendIn(Desc->Id,wxEnumProperty(_("Type"),wxPG_LABEL,Types,Values,Desc->Type));
    Desc->NameId = Grid->AppendIn(Desc->Id,wxStringProperty(_("Name"),wxPG_LABEL,Desc->Name));
    Desc->PointsCountId = Grid->AppendIn(Desc->Id,wxIntProperty(_("Number of points"),wxPG_LABEL,(int)Desc->Points.Count()));
    #endif

    for ( int i=0; i<(int)Desc->Points.Count(); i++ )
    {
        AppendPropertyForPoint(Grid,Desc,i);
    }
}

bool wxsChart::HandleChangeInSet(wxsPropertyGridManager* Grid,wxPGId Id,int Position)
{
    ChartPointsDesc* Desc = m_ChartPointsDesc[Position];

    bool Changed = false;
    bool Global = Id==Desc->Id;

    if ( Global || Id == Desc->TypeId )
    {
        Desc->Type = (PointsType)Grid->GetPropertyValueAsInt(Desc->TypeId);
        Changed = true;
    }

    if ( Global || Id == Desc->NameId )
    {
        Desc->Name = Grid->GetPropertyValueAsString(Desc->NameId);
        Changed = true;
    }

    if ( Global || Id == Desc->PointsCountId )
    {
        int OldValue = (int)Desc->Points.Count();
        int NewValue = Grid->GetPropertyValueAsInt(Desc->PointsCountId);

        if ( NewValue<0 )
        {
            NewValue = 0;
            Grid->SetPropertyValue(Desc->PointsCountId,NewValue);
        }

        if ( NewValue > OldValue )
        {
            for ( int i=OldValue; i<NewValue; i++ )
            {
                PointDesc* NewPoint = new PointDesc;
                NewPoint->X = 0.0;
                NewPoint->Y = 0.0;
                NewPoint->Name = wxString::Format(_("Point %d"),i+1);
                Desc->Points.Add(NewPoint);
                AppendPropertyForPoint(Grid,Desc,i);
            }
        }
        else if ( NewValue < OldValue )
        {
            for ( int i=NewValue; i<OldValue; i++ )
            {
                #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty((Desc->Points[i])->Id);
                #else
                Grid->Delete((Desc->Points[i])->Id);
                #endif
                delete Desc->Points[i];
            }

            Desc->Points.RemoveAt(NewValue,OldValue-NewValue);
        }

        Changed = true;
    }

    if ( !Changed )
    {
        for ( int i=0; i<(int)Desc->Points.Count(); i++ )
        {
            if ( HandleChangeInPoint(Grid,Id,Desc,i,Global) )
            {
                Changed = true;
                if ( !Global ) break;
            }
        }
    }

    if ( Changed )
    {
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}

void wxsChart::AppendPropertyForPoint(wxsPropertyGridManager* Grid,ChartPointsDesc* SetDesc,int Position)
{
    PointDesc* Desc = SetDesc->Points[Position];
    wxString Name = wxString::Format(_("Point %d"),Position+1);

    #if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Desc->Id = Grid->AppendIn(SetDesc->Id,new wxParentProperty(Name,wxPG_LABEL));
    Desc->NameId = Grid->AppendIn(Desc->Id,new wxStringProperty(_("Name"),wxPG_LABEL,Desc->Name));
    Desc->XId = Grid->AppendIn(Desc->Id,new wxStringProperty(_("X"),wxPG_LABEL,wxString::Format(_T("%lf"),Desc->X)));
    Desc->YId = Grid->AppendIn(Desc->Id,new wxStringProperty(_("Y"),wxPG_LABEL,wxString::Format(_T("%lf"),Desc->Y)));
    #else
    Desc->Id = Grid->AppendIn(SetDesc->Id,wxParentProperty(Name,wxPG_LABEL));
    Desc->NameId = Grid->AppendIn(Desc->Id,wxStringProperty(_("Name"),wxPG_LABEL,Desc->Name));
    Desc->XId = Grid->AppendIn(Desc->Id,wxStringProperty(_("X"),wxPG_LABEL,wxString::Format(_T("%lf"),Desc->X)));
    Desc->YId = Grid->AppendIn(Desc->Id,wxStringProperty(_("Y"),wxPG_LABEL,wxString::Format(_T("%lf"),Desc->Y)));
    #endif
}

bool wxsChart::HandleChangeInPoint(wxsPropertyGridManager* Grid,wxPGId Id,ChartPointsDesc* SetDesc,int Position,bool Global)
{
    PointDesc* Desc = SetDesc->Points[Position];

    bool Changed = false;
    if ( Id == Desc->Id ) Global = true;

    if ( Global || Id == Desc->NameId )
    {
        Desc->Name = Grid->GetPropertyValueAsString(Desc->NameId);
        Changed = true;
    }

    if ( Global || Id == Desc->XId )
    {
        Grid->GetPropertyValueAsString(Desc->XId).ToDouble(&Desc->X);
        Changed = true;
    }

    if ( Global || Id == Desc->YId )
    {
        Grid->GetPropertyValueAsString(Desc->YId).ToDouble(&Desc->Y);
        Changed = true;
    }

    return Changed;
}
