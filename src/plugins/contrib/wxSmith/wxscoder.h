#ifndef WXSCODER_H
#define WXSCODER_H

#include <wx/string.h>
#include <wx/thread.h>
#include <map>
#include <cbeditor.h>
#include <time.h>
#include "wxsglobals.h"


#define WXS_CODE_WAIT_TIME	3	///< Time in sec between putting the code into queue and merging it with real files, currently not used


/** This class is responsible for managing code created on-the-fly
 *
 * Current implementation does not shedule coded upgrades which may cause
 * bad results in performance.
 */
class WXSCLASS wxsCoder
{
	public:

		/** Ctor */
		wxsCoder();

		/** Dctor */
		virtual ~wxsCoder();

		/** Function adding block of code to processing queue */
		void AddCode(const wxString& FileName,const wxString& BlockHeader,const wxString& Code,bool Immediately = true);

		/** Function reading block of code from given source file */
		wxString GetCode(const wxString& FileName,const wxString& BlockHeader);

		/** Function processing all code requests waiting insided queue */
		bool ProcessCodeQueue();

		/** Function dropping all code requests from queue */
		void DropQueue();

		/** Function processing all requests for given file */
		bool ProcessCodeForFile(const wxString& FileName);

		/** Function getting singleton object from system */
		static wxsCoder* Get() { return Singleton; }

	private:

		/** Structure describing one code block */
		struct CodeEntry
		{
			wxString FileName;			///< Name of file
			wxString BlockHeader;		///< Header of auto-generated code
			wxString Code;				///< Code
			CodeEntry* Next;			///< Next code in the list
			time_t TimeStamp;			///< Timestamp of lastest update
		};

		/** List of all enteries */
		CodeEntry* Enteries;

		/** Function applying changed to project file.
		 *
		 * If the file is opened inside editor, editor's buffer will be modified
		 * and midified flag will be set. If file is not open, the real file will
		 * be changed
		 */
        bool ApplyChanges(CodeEntry* Entry);

		/** Function applying hanges to currently opened editor */
		bool ApplyChanges(CodeEntry* Entry, cbEditor* Editor);

		/** Function applying changes to file which is not open inside editor */
		bool ApplyChanges(CodeEntry* Entry, const wxString& FileName);

		/** Singleton object */
		static wxsCoder* Singleton;

		/** Mutex for this object - added in case of multi-threading shedules */
		wxMutex DataMutex;

		/** Variable used to blocking processing of data */
		bool BlockProcess;
};

#endif // WXSCODER_H
