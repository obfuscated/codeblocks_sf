/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2010 Jens Lody
 *
 * $Revision:$
 * $Id:$
 * $HeadURL:$
 */

#ifndef INCREMENTAL_SEARCH_H_INCLUDED
#define INCREMENTAL_SEARCH_H_INCLUDED

#include <cbplugin.h>
#include <wx/dynarray.h>

class cbProject;

struct PrjEditor
{
    PrjEditor(cbProject* _prj, wxString _fname)
    {
        prj = _prj;
        fname = _fname;
    }
    bool operator==(const PrjEditor& pe) const
    {
        return ((prj == pe.prj) &&
                (((pe.fname).IsEmpty()) || (fname == pe.fname)));
    }

    cbProject* prj;
    wxString fname;
};

WX_DECLARE_OBJARRAY(PrjEditor, ReopenEditorArray);

WX_DEFINE_ARRAY_PTR(cbProject*, ProjectPtrArray);

class ReopenEditor : public cbPlugin
{

public:
    ReopenEditor();
    ~ReopenEditor();
    void BuildMenu(wxMenuBar* menuBar);
    void BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/ = 0) {}
    bool BuildToolBar(wxToolBar* /*toolBar*/) {return false;}

private:
    void OnAttach();
    void OnRelease(bool appShutDown);
    void OnReopenEditor(wxCommandEvent& event);
    void OnProjectClosed(CodeBlocksEvent& event);
    void OnProjectOpened(CodeBlocksEvent& event);
    void OnEditorClosed(CodeBlocksEvent& event);
    ReopenEditorArray m_PrjEditorList;
    ProjectPtrArray m_ClosedProjects;

    DECLARE_EVENT_TABLE()
};

#endif // INCREMENTAL_SEARCH_H_INCLUDED
