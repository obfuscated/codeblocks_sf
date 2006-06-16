#include <sdk.h>
#include "byocbtris.h"
#include "byocbtriscontent.h"

namespace
{
    class byoCBTrisEditor: public EditorBase
    {
        public:
            byoCBTrisEditor(wxWindow* parent): EditorBase(parent,_("C::B-Tris")), m_BackToWorkTimer(1)
            {
                wxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
                m_Shortname = GetFilename();
                SetTitle(GetFilename());
                m_Content = new byoCBTrisContent(this,-1);
                Sizer->Add(m_Content,1,wxEXPAND);
                SetSizer(Sizer);
                Layout();
                m_Content->SetFocus();
            }

            void OnSetFocus(wxFocusEvent& event)
            {
                m_Content->SetFocus();
            }

            virtual void Activate() { m_Content->SetFocus(); EditorBase::Activate(); }

        private:
            byoCBTrisContent* m_Content;
            BackToWorkTimer m_BackToWorkTimer;

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(byoCBTrisEditor,EditorBase)
        EVT_SET_FOCUS(byoCBTrisEditor::OnSetFocus)
    END_EVENT_TABLE()
}

byoCBTris::byoCBTris(): byoGame(_("C::B-Tris"))
{
}

byoCBTris::~byoCBTris()
{
}

void byoCBTris::Play()
{
    new byoCBTrisEditor((wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook());
}

byoCBTris byoCBTrisInstance;
