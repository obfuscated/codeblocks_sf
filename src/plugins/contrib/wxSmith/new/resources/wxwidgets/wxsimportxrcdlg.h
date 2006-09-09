#ifndef WXSIMPORTXRCDLG_H
#define WXSIMPORTXRCDLG_H

#include "wxsnewwindowdlg.h"

/** \brief This class is used to show dialgo window and import xrc file data */
class wxsImportXrcDlg : public wxsNewWindowDlg
{
	public:

        /** \brief Ctor */
		wxsImportXrcDlg(wxWindow* Parent, TiXmlElement* Element);

		/** \brief Dctor */
		virtual ~wxsImportXrcDlg();

    protected:

        /** \brief This procedure will load xrc data to newly created resource */
        virtual bool PrepareResource(wxsWindowRes* Res);

	private:

        /** \brief Xml element containing xrc structure */
        TiXmlElement* Element;
};

#endif
