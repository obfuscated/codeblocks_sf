#ifndef WXSMITHMIME_H
#define WXSMITHMIME_H

#include <cbplugin.h>

/** \brief Helper plugin dealing with mime types */
class wxSmithMime : public cbMimePlugin
{
    public:

        /** \brief Ctor */
        wxSmithMime();

        /** \brief Returning true if can handle this file
         *
         * This function will handle two file types:
         *  - WXS files (will be opened only when project file is also opened)
         *  - XRC files (when there's project associated opened, it will open
         *             it's editor, when there's no such project, XRC will
         *             be edited externally)
         */
        virtual bool CanHandleFile(const wxString& filename) const;

        /** \brief Opening file
         *
         * If this is wxs file and it's project is opened, proper editor will be
         * opened / selected for xrc files, new editor withour project will be used
         */
        virtual int OpenFile(const wxString& filename);

        /** \brief We do not handle everything */
        virtual bool HandlesEverything() const  { return false; }
};

#endif
