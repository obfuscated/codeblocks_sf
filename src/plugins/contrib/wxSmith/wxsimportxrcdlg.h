#ifndef WXSIMPORTXRCDLG_H
#define WXSIMPORTXRCDLG_H

#include "wxsnewwindowdlg.h"

/** This class is used to show dialgo window and import xrc file data */
class WXSCLASS wxsImportXrcDlg : public wxsNewWindowDlg
{
	public:
        /** Ctor */
		wxsImportXrcDlg(wxWindow* Parent, TiXmlElement* Element);

		/** Dctor */
		virtual ~wxsImportXrcDlg();

    protected:

        /** This procedure will load xrc data to newly created resource */
        virtual bool PrepareResource(wxsWindowRes* Res);

	private:

        /** Xml element containing xrc structure */
        TiXmlElement* Element;
};

#endif // WXSIMPORTXRCDLG_H
