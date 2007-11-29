/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: snippetsimages.h 58 2007-04-22 04:40:57Z Pecan $
#ifndef SNIPIMAGES_H
#define SNIPIMAGES_H

#include <wx/imaglist.h>

#if not defined(SNIPPETS_TREE_IMAGE_COUNT)
    #define SNIPPETS_TREE_IMAGE_COUNT 6
    #define TREE_IMAGE_ALL_SNIPPETS 0
    #define TREE_IMAGE_CATEGORY     1
    #define TREE_IMAGE_SNIPPET      2
    #define TREE_IMAGE_SNIPPET_TEXT 3
    #define TREE_IMAGE_SNIPPET_FILE 4
    #define TREE_IMAGE_SNIPPET_URL  5
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
