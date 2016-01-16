/**  \file wxsgrid.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2008  Bartlomiej Swiecki
* Copyright (C) 2008 Ron Collins
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

#include "wxsgrid.h"
#include <wx/grid.h>

namespace
{
    wxsRegisterItem<wxsGrid> Reg(_T("Grid"),wxsTWidget,_T("Advanced"),70);

    WXS_ST_BEGIN(wxsGridStyles,_T(""))
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsGridEvents)
        WXS_EVI(EVT_CMD_GRID_CELL_LEFT_CLICK,wxEVT_GRID_CELL_LEFT_CLICK,wxGridEvent,CellLeftClick)
        WXS_EVI(EVT_CMD_GRID_CELL_RIGHT_CLICK,wxEVT_GRID_CELL_RIGHT_CLICK,wxGridEvent,CellRightClick)
        WXS_EVI(EVT_CMD_GRID_CELL_LEFT_DCLICK,wxEVT_GRID_CELL_LEFT_DCLICK,wxGridEvent,CellLeftDClick)
        WXS_EVI(EVT_CMD_GRID_CELL_RIGHT_DCLICK,wxEVT_GRID_CELL_RIGHT_DCLICK,wxGridEvent,CellRightDClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_LEFT_CLICK,wxEVT_GRID_LABEL_LEFT_CLICK,wxGridEvent,LabelLeftClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_RIGHT_CLICK,wxEVT_GRID_LABEL_RIGHT_CLICK,wxGridEvent,LabelRightClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_LEFT_DCLICK,wxEVT_GRID_LABEL_LEFT_DCLICK,wxGridEvent,LabelLeftDClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_RIGHT_DCLICK,wxEVT_GRID_LABEL_RIGHT_DCLICK,wxGridEvent,LabelRightDClick)
        WXS_EVI(EVT_CMD_GRID_CELL_CHANGE,wxEVT_GRID_CELL_CHANGE,wxGridEvent,CellChange)
        WXS_EVI(EVT_CMD_GRID_SELECT_CELL,wxEVT_GRID_SELECT_CELL,wxGridEvent,CellSelect)
        WXS_EVI(EVT_CMD_GRID_EDITOR_HIDDEN,wxEVT_GRID_EDITOR_HIDDEN,wxGridEvent,EditorHidden)
        WXS_EVI(EVT_CMD_GRID_EDITOR_SHOWN,wxEVT_GRID_EDITOR_SHOWN,wxGridEvent,EditorShown)

        WXS_EV_DEFAULTS()
    WXS_EV_END()

}

//------------------------------------------------------------------------------

wxsGrid::wxsGrid(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGridEvents,
        wxsGridStyles),
    m_ColsCount(0),
    m_RowsCount(0)
{
    m_ReadOnly       = false;
    m_GridLines      = true;
    m_LabelRowHeight = -1;
    m_LabelColWidth  = -1;
    m_DefaultRowSize = -1;
    m_DefaultColSize = -1;
}

//------------------------------------------------------------------------------

void wxsGrid::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/grid.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/grid.h>"),_T("wxGridEvent"),0);

            Codef( _T("%C(%W, %I, %P, %S, %T, %N);\n") );

            if ( GetPropertiesFlags() & flSource )
            {
                if ( m_ColsCount>=0 && m_RowsCount>=0 && (m_ColsCount>0 || m_RowsCount>0) )
                {
                    Codef( _T("%ACreateGrid(%d,%d);\n"), m_RowsCount, m_ColsCount );
                    BuildSetupWindowCode();

                    Codef( _T("%AEnableEditing(%b);\n"),  !m_ReadOnly);
                    Codef( _T("%AEnableGridLines(%b);\n"), m_GridLines);

                    if (m_LabelRowHeight > 0) Codef(_T("%ASetColLabelSize(%d);\n"), m_LabelRowHeight);
                    if (m_LabelColWidth  > 0) Codef(_T("%ASetRowLabelSize(%d);\n"), m_LabelColWidth);

                    if (m_DefaultRowSize  > 0) Codef(_T("%ASetDefaultRowSize(%d, %b);\n"), m_DefaultRowSize, true);
                    if (m_DefaultColSize  > 0) Codef(_T("%ASetDefaultColSize(%d, %b);\n"), m_DefaultColSize, true);

                    wxString ss = m_LabelTextColour.BuildCode( GetCoderContext() );
                    if (ss.Len() > 0) Codef(_T("%ASetLabelTextColour(%s);\n"), ss.wx_str());

                    ss = GetCoderContext()->GetUniqueName( _T("GridLabelFont") );
                    wxString tt = m_LabelFont.BuildFontCode(ss, GetCoderContext());
                    if (tt.Len() > 0)
                    {
                        Codef(_T("%s"), tt.wx_str());
                        Codef(_T("%ASetLabelFont(%s);\n"), ss.wx_str());
                    }

                    int n = wxMin( (int)m_ColLabels.GetCount(), m_ColsCount );
                    for ( int i=0; i<n; i++ )
                    {
                        Codef(_T("%ASetColLabelValue(%d, %t);\n"), i, m_ColLabels[i].wx_str());
                    }

                    n = wxMin( (int)m_RowLabels.GetCount(), m_RowsCount );
                    for ( int i=0; i<n; i++)
                    {
                        Codef(_T("%ASetRowLabelValue(%d, %t);\n"), i, m_RowLabels[i].wx_str());
                    }

                    n = (int)m_CellText.GetCount();
                    int i = 0;
                    for ( int j=0; j<m_RowsCount && i<n; j++ )
                    {
                        for ( int k=0; k<m_ColsCount && i<n; k++, i++ )
                        {
                            Codef( _T("%ASetCellValue(%d, %d, %t);\n"), j, k, m_CellText[i].wx_str());
                        }
                    }

                    // default cell font and text colour

                    Codef(_T("%ASetDefaultCellFont( %AGetFont() );\n"));

                    Codef(_T("%ASetDefaultCellTextColour( %AGetForegroundColour() );\n"));

                }
            }

            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGrid::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

//------------------------------------------------------------------------------

wxObject* wxsGrid::OnBuildPreview( wxWindow* parent, long flags )
{
    wxGrid* preview = new wxGrid( parent, GetId(), Pos(parent), Size(parent), Style() );

    if ( GetPropertiesFlags() & flSource )
    {
        if ( m_ColsCount>=0 && m_RowsCount>=0 && (m_ColsCount>0 || m_RowsCount>0) )
        {
            preview->CreateGrid( m_RowsCount, m_ColsCount );
            SetupWindow( preview, flags );

            preview->EnableEditing( !m_ReadOnly);
            preview->EnableGridLines( m_GridLines );

            if ( m_LabelRowHeight > 0 ) preview->SetColLabelSize( m_LabelRowHeight );
            if ( m_LabelColWidth  > 0 ) preview->SetRowLabelSize( m_LabelColWidth );

            if ( m_DefaultRowSize > 0 ) preview->SetDefaultRowSize( m_DefaultRowSize, true );
            if ( m_DefaultColSize > 0 ) preview->SetDefaultColSize( m_DefaultColSize, true );

            wxColour cc = m_LabelTextColour.GetColour();
            if ( cc.IsOk() )
            {
                preview->SetLabelTextColour(cc);
            }

            wxFont labelfont = m_LabelFont.BuildFont();
            if ( labelfont.IsOk() )
            {
                preview->SetLabelFont( labelfont );
            }

            int n = wxMin( (int)m_ColLabels.GetCount(), m_ColsCount );
            for ( int i=0; i<n; i++ )
            {
                preview->SetColLabelValue( i, m_ColLabels[i] );
            }

            n = wxMin( (int)m_RowLabels.GetCount(), m_RowsCount );
            for( int i=0; i<n; i++ )
            {
                preview->SetRowLabelValue( i, m_RowLabels[i] );
            }

            n = (int)m_CellText.GetCount();
            int i = 0;
            for ( int j=0; j<m_RowsCount && i<n; j++ )
            {
                for ( int k=0; k<m_ColsCount && i<n; k++, i++ )
                {
                    preview->SetCellValue( j, k, m_CellText[i] );
                }
            }

            wxFont cellfont = preview->GetFont();
            if ( cellfont.IsOk() )
            {
                preview->SetDefaultCellFont(cellfont);
            }

            cc = preview->GetForegroundColour();
            preview->SetDefaultCellTextColour(cc);

        }
    }

    return preview;
}

//------------------------------------------------------------------------------

void wxsGrid::OnEnumWidgetProperties(long Flags)
{
    if ( Flags & flSource )
    {
        WXS_LONG       ( wxsGrid, m_ColsCount,       _("Number of columns"),    _T("cols"),            0);
        WXS_LONG       ( wxsGrid, m_RowsCount,       _("Number of rows"),       _T("rows"),            0);
        WXS_BOOL       ( wxsGrid, m_ReadOnly,        _("Read Only"),            _T("readonly"),       false);
        WXS_BOOL       ( wxsGrid, m_GridLines,       _("Grid Lines"),           _T("gridlines"),      true);
        WXS_LONG       ( wxsGrid, m_LabelRowHeight,  _("Label Row Height"),     _T("labelrowheight"), -1);
        WXS_LONG       ( wxsGrid, m_LabelColWidth,   _("Label Column Width"),   _T("labelcolwidth"),  -1);
        WXS_LONG       ( wxsGrid, m_DefaultRowSize,  _("Default Row Height"),   _T("defaultrowsize"), -1);
        WXS_LONG       ( wxsGrid, m_DefaultColSize,  _("Default Column Width"), _T("defaultcolsize"), -1);
        WXS_COLOUR     ( wxsGrid, m_LabelTextColour, _("Label Text Colour"),    _T("labeltextcolour") );
        WXS_FONT       ( wxsGrid, m_LabelFont,       _("Label Text Font"),      _T("labelfont") );
        WXS_ARRAYSTRING( wxsGrid, m_ColLabels,       _("Column Labels"),        _T("collabels"), _T("item"));
        WXS_ARRAYSTRING( wxsGrid, m_RowLabels,       _("Row Labels"),           _T("rowlabels"), _T("item"));
        WXS_ARRAYSTRING( wxsGrid, m_CellText,        _("Cell Data"),            _T("celltext"),  _T("item"));
    }
}
