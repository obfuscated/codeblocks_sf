/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
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

#ifndef __CB_EDITOR_PANEL_H__
#define __CB_EDITOR_PANEL_H__

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <logmanager.h>
#include <editorbase.h>

#include "FileContent.h"


class wxCommandProcessor;

class cbEditorPanel: public EditorBase
{
    public:
        cbEditorPanel( const wxString& fileName, const wxString& title, FileContent *fc );
        virtual ~cbEditorPanel();
    private:
        cbEditorPanel(const cbEditorPanel &p);
        cbEditorPanel &operator=(const cbEditorPanel &rhs);
    public:
        virtual bool GetModified() const;
        virtual void SetModified(bool modified = true);
        virtual bool Save();
        virtual bool SaveAs();


        virtual void Cut(){}
        virtual void Copy(){}
        virtual void Paste(){}
        virtual void DeleteSelection(){}
        virtual bool CanPaste() const { return false; }
        virtual bool IsReadOnly() const { return false; }
        virtual bool CanSelectAll() const { return false; }
        virtual void SelectAll(){return;}
        virtual bool HasSelection() const { return false; }
        //virtual bool CanCut() const{return false;}
        //virtual bool CanCopy() const{return false;}

        /** @brief Sets the editor's filename.
          * @param filename The filename to set.
          */
        virtual void SetFilename(const wxString& filename);

    private:
        virtual bool CanUndo() const;
        virtual bool CanRedo() const;
        virtual void Undo();
        virtual void Redo();

    protected:
        void UpdateModified();

        bool m_IsOK;
        //wxCommandProcessor *m_cmdprocessor;
        FileContent *m_filecontent;

    private:

        //DECLARE_EVENT_TABLE()
};

#endif






