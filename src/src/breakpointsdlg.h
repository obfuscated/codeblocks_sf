/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef BREAKPOINTSDLG_H
#define BREAKPOINTSDLG_H

#include <vector>
#include <wx/panel.h>
#include <cbdebugger_interfaces.h>

#ifndef CBPRECOMP
    #include <wx/imaglist.h>
#endif

class cbBreakpoint;
class CodeBlocksEvent;
class cbDebuggerPlugin;
class wxCommandEvent;
class wxListCtrl;
class wxListEvent;

class BreakpointsDlg : public wxPanel, public cbBreakpointsDlg
{
    public:
        BreakpointsDlg();

        wxWindow* GetWindow() { return this; }

        bool AddBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line);
        bool RemoveBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line);
        void EditBreakpoint(const wxString& filename, int line);
        void EnableBreakpoint(const wxString& filename, int line, bool enable);

        void Reload();
    private:

        void OnRemove(wxCommandEvent& event);
        void OnRemoveAll(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnEnable(wxCommandEvent& event);
        void OnShowTemp(wxCommandEvent& event);
        void OnRightClick(wxListEvent& event);
        void OnDoubleClick(wxListEvent& event);
        void OnBreakpointAdd(CodeBlocksEvent& event);
        void OnBreakpointEdit(CodeBlocksEvent& event);
        void OnKeyUp(wxKeyEvent& event);
        void OnUpdateUI(wxUpdateUIEvent &event);
    private:
        enum Columns
        {
            Type = 0,
            FilenameAddress,
            Line,
            Info,
            Debugger
        };

        friend struct FindBreakpointPred;

        struct Item
        {
            Item(const cb::shared_ptr<cbBreakpoint> &breakpoint, cbDebuggerPlugin *plugin, wxString const &pluginName) :
                breakpoint(breakpoint),
                plugin(plugin),
                pluginName(pluginName)
            {
            }

            cb::shared_ptr<cbBreakpoint> breakpoint;
            cbDebuggerPlugin *plugin;
            wxString pluginName;
        };
        typedef std::vector<Item> Items;
    private:
        void RemoveBreakpoint(int sel);
        Items::iterator FindBreakpoint(const wxString &filename, int line);
        void BreakpointProperties(const Item &item);

    private:
        wxListCtrl* m_pList;
        Items m_breakpoints;
        wxImageList m_icons;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // BREAKPOINTSDLG_H
