/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
*/

#ifndef PERSONALITYMANAGER_H
#define PERSONALITYMANAGER_H

#include "settings.h"
#include <wx/dynarray.h>

/** @brief Manage different personalities.
  *
  * Personalities are different profiles of appearence for Code::Blocks.
  * For instance, the default personality is the full IDE. You could create
  * a "Text editor" personality that is light-weight, tailored to "simple" text editing.
  * In this example "Text editor" personality, the manager windows would be hidden
  * and no plugins would be loaded, making Code::Blocks perfect for text editing
  * (with all the powerful features of its embedded editor).
  *
  * The user can create as many personalities as needed. The work-in-progress
  * file associations manager (win32 only) will work with personalities so that
  * different file types launch Code::Blocks using a different personality...
  *
  * To select the desired personality when launching Code::Blocks, the user can
  * use the command-line switch "--personality". If "--personality=ask" is passed
  * in the command line, a selection box will be displayed for the user to choose
  * the desired personality.
  *
  * @note: This class must be initialized *after* ConfigManager and MessageManager...
  */
class DLLIMPORT PersonalityManager
{
	public:
        friend class Manager; // give Manager access to our private members
        
        /// Use this once, on program startup to set the working personality
        void SetPersonality(const wxString& personality, bool createIfNotExist = false);
        /// Get the working personality string
        const wxString& GetPersonality();
        /// Get a list of all the known personalities
        const wxArrayString& GetPersonalitiesList();
        /// Get the root configuration for personalities
        const wxString& GetPersonalitiesRoot();
        /// Get the current personality's configuration key (includes root key)
        const wxString& GetPersonalityKey();
	protected:
        void ReadPersonalities();

        wxArrayString m_Personalities;
        int m_CurrentPersonalityIdx; // index to array
	private:
        static PersonalityManager* Get();
		static void Free();
		PersonalityManager();
		~PersonalityManager();
};

#endif // PERSONALITYMANAGER_H
