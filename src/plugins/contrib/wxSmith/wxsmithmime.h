#ifndef WXSMITHMIME_H
#define WXSMITHMIME_H

#include <cbplugin.h>

/** Helper plugin dealing with mime types */
class wxSmithMime : public cbMimePlugin
{
    public:

        wxSmithMime();

        /** Returning true if can handle this file
         *
         * This function will handle two file types:
         *  WXS file (will be opened only when project file is also opened)
         *  XRC files (currently not implemented)
         */
        virtual bool CanHandleFile(const wxString& filename) const;

        /** Opening file - if this is wxs file and it's project is opened,
         *                 proper editor will be opened / selected
         * for xrc files, new editor withour project will be used */
        virtual int OpenFile(const wxString& filename);

        /** We do not handle everything */
        virtual bool HandlesEverything() const  { return false; }
};

#endif // WXSMITHMIME_H
