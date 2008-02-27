#ifndef PERSONALITYMANAGER_H
#define PERSONALITYMANAGER_H

#include "settings.h"
#include "manager.h"
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
  */
class DLLIMPORT PersonalityManager : public Mgr<PersonalityManager>
{
    static wxString pers;

    PersonalityManager();

	public:
        friend class Mgr<PersonalityManager>;
        friend class Manager; // give Manager access to our private members

        /// Use this once, on program startup to set the working personality
        void SetPersonality(const wxString& personality, bool createIfNotExist = false);
        /// Get the working personality string
        const wxString GetPersonality();
        /// Get a list of all the known personalities
        const wxArrayString GetPersonalitiesList();
};

#endif // PERSONALITYMANAGER_H
