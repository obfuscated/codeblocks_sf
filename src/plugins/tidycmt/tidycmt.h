/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef HEADER_GUARD_8367E1F8
#define HEADER_GUARD_8367E1F8
/*
* Header guards for the lazy. Adds a header guard to every ".h" file that doesn't have one when saving.
* Filenames are hashed to a 64-bit hex number to support umlaut characters (and Kanji, Cyrillic, or whatever)
* regardless of file encoding, and regardless of what's legal as a C/C++ macro name
* Thomas sez: uz tis at yar own risk, an dun blam me.
*/

class wxMenuBar;
class wxMenu;
class FileTreeData;

#include "cbplugin.h"

class TidyCmt : public cbPlugin
{
    public:
        TidyCmt(){};
        virtual ~TidyCmt(){};

        virtual void BuildMenu(wxMenuBar* menuBar){}
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }

    private:

        virtual void OnAttach();

        void OnSave(CodeBlocksEvent& event);

        virtual void OnRelease(bool appShutDown){};
};

#endif // header guard
