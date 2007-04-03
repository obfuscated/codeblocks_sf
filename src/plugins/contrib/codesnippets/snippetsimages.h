#ifndef SNIPIMAGES_H
#define SNIPIMAGES_H

#include <wx/imaglist.h>

#if not defined(SNIPPETS_TREE_IMAGE_COUNT)
    #define SNIPPETS_TREE_IMAGE_COUNT 5
    #define TREE_IMAGE_ALL_SNIPPETS 0
    #define TREE_IMAGE_CATEGORY     1
    #define TREE_IMAGE_SNIPPET      2
    #define TREE_IMAGE_SNIPPET_TEXT 3
    #define TREE_IMAGE_SNIPPET_FILE 4
#endif

class SnipImages
{
    public:
        SnipImages();
       ~SnipImages();
       void RegisterImage( char** xpm_data );
       wxImageList* GetSnipImageList(){ return m_pSnippetsTreeImageList;}
       wxIcon       GetSnipListIcon(int index);

    protected:

    private:
        wxImageList* m_pSnippetsTreeImageList;

};

#endif // SNIPIMAGES_H
