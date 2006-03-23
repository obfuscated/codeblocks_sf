#ifndef WXSPROJECTCONFIG_H
#define WXSPROJECTCONFIG_H

/** \brief Structure with configuration of project */
struct wxsProjectConfig
{
    wxString AppFile;               ///< \brief Source file defining application
    wxArrayString LoadedResources;  ///< \brief List of automatically loaded resource files
    wxString MainResource;          ///< \brief Resource used by default
    bool CallInitAll;               ///< \brief True if wxXmlResource::Get()->InitAllHandlers() should be called while initialization
    bool CallInitAllNecessary;      ///< \brief True if should call wxXmlResource::Get()->InitAllHandlers() only when necessary
};

#endif
