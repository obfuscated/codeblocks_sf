/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef OCCURRENCESHIGHLIGHTING_H_INCLUDED
#define OCCURRENCESHIGHLIGHTING_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <set>

#include <cbplugin.h>

class Highlighter;
class OccurrencesPanel;

class OccurrencesHighlighting : public cbPlugin
{
    public:
        OccurrencesHighlighting();
        ~OccurrencesHighlighting() override;

        void BuildMenu(wxMenuBar* menuBar) override;
        void BuildModuleMenu(const ModuleType type, wxMenu* menu,
                             const FileTreeData* data = nullptr) override;
        bool BuildToolBar(cb_unused wxToolBar* toolBar) override { return false; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent) override;
        int GetConfigurationPriority() const override { return 50; }
        int GetConfigurationGroup() const override { return cgEditor; }


    protected:
        void OnAttach() override;
        void OnRelease(bool appShutDown) override;

    private:
        void OnListKeyDown(wxListEvent &event);

    private:
        void OnViewOccurrencesPanel(wxCommandEvent& event);
        void OnUpdateViewMenu(wxUpdateUIEvent &event);
        void OnHighlightPermanently(wxCommandEvent &event);
        void OnHighlightRemove(wxCommandEvent &event);
        void OnRemove(wxCommandEvent &event);
        void OnPanelPopupMenu(wxContextMenuEvent &event);

        void OnEditorHook(cbEditor* editor, wxScintillaEvent& event);
        void OnEditorEvent(CodeBlocksEvent& event);
    private:
        wxString GetWordAtCaret()const;
        void RemoveSelected();
        void UpdatePanel();


    private:
        int m_FunctorId;
        Highlighter *m_pHighlighter;

        OccurrencesPanel *m_pPanel;
        wxMenu* m_pViewMenu;

        std::set<wxString> m_texts;

    private:
        DECLARE_EVENT_TABLE();
};

#endif // OCCURRENCESHIGHLIGHTING_H_INCLUDED
