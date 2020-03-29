#ifndef WXSSTATELED_H
#define WXSSTATELED_H

#include "wxswidget.h"


class wxsStateLed : public wxsWidget
{
    public:
        wxsStateLed(wxsItemResData* Data);
        virtual ~wxsStateLed();
    protected:

        void        OnBuildCreatingCode();
        wxObject*   OnBuildPreview(wxWindow* Parent,long Flags);
        void        OnEnumWidgetProperties(long Flags);
        void        InsertPropertyForState(wxsPropertyGridManager* Grid,int Position);
        void        OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id);
        bool        HandleChangeInState(wxsPropertyGridManager *Grid, wxPGId id, int Position);
        void        OnAddExtraProperties(wxsPropertyGridManager* Grid);
        bool        OnXmlWrite(TiXmlElement* Element, bool IsXRC, bool IsExtra);
        bool        OnXmlRead(TiXmlElement* Element, bool IsXRC, bool IsExtra);

        /*! \brief State enum.
         */
        struct StateDesc{
            wxPGId   id;		//!< Tag property ID.
			wxColour colour;    //!< Tag value.
        };

        wxPGId                  m_StateCountId;
        wxPGId                  m_StateCurrentId;
        std::map<int,StateDesc> m_StateColor;
        wxsColourData	        m_Disable;
        int                     m_State;
        int                     m_numberOfState;

    private:
};

#endif // WXSLED_H
