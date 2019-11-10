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

#ifndef WXSCHART_H
#define WXSCHART_H

#include <wxwidgets/wxswidget.h>
#include <wx/chartctrl.h>
#include <wx/dynarray.h>

class wxsChart : public wxsWidget
{
    public:
        wxsChart(wxsItemResData* Data);
        virtual ~wxsChart();

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id);
        bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);

        struct PointDesc
        {
            wxString Name;
            double X;
            double Y;

            wxPGId Id;
            wxPGId NameId;
            wxPGId XId;
            wxPGId YId;
        };

        WX_DEFINE_ARRAY(PointDesc*,PointList);

        enum PointsType
        {
            Bar,
            Bar3D,
            Pie,
            Pie3D,
            Points,
            Points3D,
            Line,
            Line3D,
            Area,
            Area3D
        };

        struct ChartPointsDesc
        {
            wxPGId Id;
            wxPGId TypeId;
            wxPGId NameId;
            wxPGId PointsCountId;

            PointsType Type;
            wxString   Name;
            PointList  Points;

            ChartPointsDesc(): Type(Bar)
            {}

            ~ChartPointsDesc()
            {
                for ( size_t i=0; i<Points.Count(); i++ )
                {
                    delete Points[i];
                }
                Points.Clear();
            }
        };

        WX_DEFINE_ARRAY(ChartPointsDesc*,List);

        long m_Flags;
        List m_ChartPointsDesc;
        wxPGId m_ChartPointsCountId;

        void AppendPropertyForSet(wxsPropertyGridManager* Grid,int Position);
        bool HandleChangeInSet(wxsPropertyGridManager* Grid,wxPGId Id,int Position);
        void AppendPropertyForPoint(wxsPropertyGridManager* Grid,ChartPointsDesc* SetDesc,int Position);
        bool HandleChangeInPoint(wxsPropertyGridManager* Grid,wxPGId Id,ChartPointsDesc* SetDesc,int Position,bool Global);
};

#endif
