#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __NASSI_FILE_CONTENT__
#define __NASSI_FILE_CONTENT__

#include "FileContent.h"


class NassiBrick;
class NassiFileContent : public FileContent
{
    public:
        NassiFileContent();
        virtual ~NassiFileContent(void);
    public:
        virtual wxOutputStream& SaveObject(wxOutputStream& stream);
        virtual wxInputStream& LoadObject(wxInputStream& stream);

        virtual wxString GetWildcard();

    public:
        NassiBrick *GetFirstBrick(void);
        NassiBrick *SetFirstBrick(NassiBrick *brick);
    private:
        NassiFileContent(const NassiFileContent &p);
        NassiFileContent &operator=(const NassiFileContent &rhs);
        NassiBrick *m_firstbrick;
};
#endif
