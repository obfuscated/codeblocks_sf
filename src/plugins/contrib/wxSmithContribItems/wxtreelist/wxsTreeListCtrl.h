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
*/

#ifndef WXSTREELISTCTRL_H
#define WXSTREELISTCTRL_H

#include    "wxswidget.h"
#include    "wxslongproperty.h"
#include    "wxsenumproperty.h"
#include    "wxsboolproperty.h"
#include    "wx/treelistctrl.h"

/** \brief Class for wxsTreeCtrl widget */
class wxsTreeListCtrl: public wxsWidget
{
public:

    wxsTreeListCtrl (wxsItemResData* Data);

private:

    virtual void OnBuildCreatingCode();
    virtual wxObject* OnBuildPreview (wxWindow* Parent,long Flags);
    virtual void OnEnumWidgetProperties (long Flags);


    void BuildItemCode (void);
    void PreviewItemCode (wxTreeListCtrl* inPreview);
    void ParseItems (wxString inSource, int& outLevel, wxArrayString& outItems);




    long m_ColCount;                  // number of columns
    long m_ColWidth;                  // size of the columns
    wxArrayString m_ColTitles;        // heading at top of each column
    long m_TabChar;                   // what separates items?
    wxArrayString m_TreeData;         // text items in the tree
    bool m_ReadOnly;                  // can edit data in tree?


};

#endif  // WXSTREELISTCTRL_H
