/** \file wxsimagelist.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* This code was taken from the wxSmithImage plug-in, copyright Ron Collins
* and released under the GPL.
*
*/

#ifndef wxsIMAGELIST_H
#define wxsIMAGELIST_H

#include    "../wxsitemresdata.h"
#include "../wxstool.h"

/** \brief Class for the wxsImageList widget
 *	\note For reasons unknown, GCC emits warnings that the images are declared but not used when "Use Include File" is set.
 */
class wxsImageList : public wxsTool
{
    public:

        wxsImageList(wxsItemResData* Data);

        int             	GetCount(void);
        wxBitmap  GetPreview(int inIndex);
        void            GetImageList(wxImageList &aImageList);
        void            DoBuild(void);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

		void StoreXpmData(void);

        bool            				m_IsBuilt;                 //!<  Only build the code once.
        wxsCoderContext 	*m_Context;				//!< Coder context.
        wxArrayString   		m_ImageData;         //!< Store all images as XPMs.
        long            				m_Width;					//!< Image width.
        long							m_Height;					//!< Image height.
        long							m_Count;        			//!< Image count .
        bool            				m_Include;                //!< Save as #include file?
        wxString        			m_Base;                    //!< Base file name of source and include files.
        wxString        			m_IDir;                       //!< The absolute path to the image include directory.
        wxString        			m_RDir;                     //!< Relative directory specified for image files.
};


#endif      // wxsIMAGELIST_H
