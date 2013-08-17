/***************************************************************
 * Name:      OccurrenceHighlighter
 * Purpose:   Code::Blocks plugin
 * Author:    danselmi ()
 * Created:   2010-04-06
 * Copyright: danselmi
 * License:   GPL
 **************************************************************/

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
        virtual ~OccurrencesHighlighting();

        virtual void BuildMenu(wxMenuBar* menuBar);
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        virtual int GetConfigurationPriority() const { return 50; }
        virtual int GetConfigurationGroup() const { return cgEditor; }


    protected:
        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);

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
