/*
* Header guards for the lazy. Adds a header guard to every ".h" file that doesn't have one when saving.
* Filenames are hashed to a 32-bit hex number to support umlaut characters (and Kanji, Cyrillic, or whatever)
* regardless of file encoding, and regardless of what's legal as a C/C++ macro name
* Thomas sez: uz tis at yar own risk, an dun blam me.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


// Evil hack, but we need this
// until when some distant day in the future, cbEditor will either
// - be updated to contain an accessor   or
// - has a means to suppress duplicate events   or
// - implements an event *before* writing to the file
#define private public


#include <sdk_precomp.h>
#include <cbplugin.h>
#include <cbstyledtextctrl.h>


class HeaderGuard : public cbPlugin
{
    public:
        HeaderGuard(){};
        virtual ~HeaderGuard(){};

        virtual void BuildMenu(wxMenuBar* menuBar){}
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }

    protected:

        virtual void OnAttach()
		{
        	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<HeaderGuard, CodeBlocksEvent>(this, &HeaderGuard::OnSave));
		}

        void OnSave(CodeBlocksEvent& event)
        {
        	cbEditor* ed = (cbEditor*) event.GetEditor();

        	wxString n = ed->GetFilename();

        	if(!n.Right(2).Lower().IsSameAs(_T(".h")))
				return;

			wxString s(ed->GetControl()->GetText());

			if(s.Contains(_T("#ifndef HEADER_GUARD_")) || s.Contains(_T("_H_INCLUDED")))
				return;

			unsigned int s1 = 257, s2 = 123, b = 9737333;
			for(unsigned int i = 0; i < n.length(); ++i)
			{
				b = (b*33) + (s1 += n[i]);
				s2 += s1;
			}
			b ^= (((s1%255)<<10) | (s2&255));
			b ^= (b << 21); b ^= (b >> 17); b ^= (b << 9);

			n.Printf(_T("#ifndef HEADER_GUARD_%X\n#define HEADER_GUARD_%X\n\n"), b, b);
        	ed->GetControl()->InsertText (0, n);
        	ed->GetControl()->InsertText (ed->GetControl()->GetLength(), _T("\n#endif /* header guard */\n"));

			// Have to save manually since events are fired *after* saving, and there's no way to suppress another event
			cbSaveToFile(ed->GetFilename(), ed->GetControl()->GetText(), ed->GetEncoding(), ed->GetUseBom());
			ed->SetModified(false);

			// TODO: once cbEditor has such a thing, rewrite this using an accessor
			ed->m_LastModified = wxFileName(ed->GetFilename()).GetModificationTime();
		}

        virtual void OnRelease(bool appShutDown){};
};

