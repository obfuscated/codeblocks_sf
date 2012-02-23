/** \file wxsimage.h
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

#ifndef WXSIMAGE_H
#define WXSIMAGE_H

#include "../wxstool.h"
#include "../wxsitemresdata.h"

/*! \brief Class for the wxsImage tool.
 *	\note For reasons unknown, GCC emits a warning that the image is declared but not used when "Use Include File" is set.
 */
class wxsImage : public wxsTool
{
    public:

        wxsImage(wxsItemResData* Data);
        wxBitmap  GetPreview(void);
        void            DoBuild(void);


    private:

        virtual void        OnBuildCreatingCode();
        virtual void        OnEnumToolProperties(long Flags);
        virtual void        OnBuildDeclarationsCode();

		void StoreXpmData(void);

        bool            				m_IsBuilt;              //!<  Only build the code once.
        wxsCoderContext 	*m_Context;			//!< Coder context.
        wxArrayString   		m_ImageData;     //!< The XPM data for the image.
        bool            				m_Include;            //!< Save as #include file?
        wxString        			m_Base;                //!< Base file name of source and include files.
        wxString        			m_IDir;                   //!< The absolute path to the image include directory.
        wxString        			m_RDir;                 //!< Relative directory specified for image files.
};


#endif      // WXSIMAGE_H
