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
#include <random>

class RndGen : public cbPlugin
{
    public:
        RndGen() : RandGen(time(0)) {}
        virtual ~RndGen() {}

        virtual void BuildMenu(cb_unused wxMenuBar* menuBar) {}
        virtual void BuildModuleMenu(cb_unused const ModuleType type, cb_unused wxMenu* menu, cb_unused const FileTreeData* data = 0) {}
        virtual bool BuildToolBar(cb_unused wxToolBar* toolBar) { return false; }

    private:
        std::mt19937 RandGen;

        virtual void OnAttach();
        void OnSave(CodeBlocksEvent& event);
        virtual void OnRelease(cb_unused bool appShutDown) {}
};

#endif // header guard
