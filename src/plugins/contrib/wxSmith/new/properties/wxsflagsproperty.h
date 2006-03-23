#ifndef WXSFLAGSPROPERTY_H
#define WXSFLAGSPROPERTY_H

#include "wxsproperty.h"

/** \brief Property with flags
 *
 * Enum property works almost the same as long property but it uses
 * predefined values from given set usd as bit fields in this value.
 * Note that this class is abstract and should be used as base class for others.
 */
class wxsFlagsProperty: public wxsProperty
{
	public:
		
        /** \brief Ctor
         *  \param PGName           name of property used in Property Grid
         *  \param DataName         name of property used in data structures
         *  \param Offset           offset of long integer holding value (taken from wxsOFFSET macro)
         *  \param Values           array of long integer with values of flags
         *  \param Names            array of names used for items in Values array, ending with NULL string
         *  \param UpdateEnteries   posting true here notifies, that arrays may change
         *  \param Default          defaut value applied on read errors
         */
		wxsFlagsProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            const long* Values,
            const wxChar** Names,
            bool UpdateEnteries=false,
            long Default=0);
		
    protected:
    
        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id, long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id, long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long Offset;
        long Default;
        bool UpdateEnteries;
        const long* Values;
        const wxChar** Names;
};

#endif
