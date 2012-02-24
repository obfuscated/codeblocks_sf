/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTBUILDTARGET_H
#define PROJECTBUILDTARGET_H

#include "settings.h"
#include "globals.h"
#include "compiletargetbase.h"
#include "projectfile.h"
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/treectrl.h>

class cbProject;
class ProjectBuildTarget;

WX_DEFINE_ARRAY(ProjectBuildTarget*, BuildTargets);

/** Represents a Code::Blocks project build target. */
class DLLIMPORT ProjectBuildTarget : public CompileTargetBase
{
	public:
		/// Constructor
		ProjectBuildTarget(cbProject* parentProject);
		/// Destructor
		~ProjectBuildTarget();

        /** @return The target's parent project. */
        virtual cbProject* GetParentProject();
        /** @return The full title, i.e. "projectname - targetname". */
        virtual wxString GetFullTitle() const;

        //properties

        /** @return A string containing a list of all the external files this target depends on.
          * If any of the files in this list is newer than the target's output, the target is relinked.
          */
        virtual const wxString& GetExternalDeps() const;

        /** Set a list of all the external files this targets depends on.
          * If any of the files in this list is newer than the target's output, the target is relinked.
          * @param deps A string containing the list of files.
          */
        virtual void SetExternalDeps(const wxString& deps);

        /** @return A string containing a list of additional output files, besides the main output.
          * If any of the files in this list is older than the list returned by
          * GetExternalDeps(), the target is relinked.
          */
        virtual const wxString& GetAdditionalOutputFiles() const;

        /** Set a list of all additional output files this targets creates, besides its main output.
          * If any of the files in this list is older than the list returned by
          * GetExternalDeps(), the target is relinked.
          * @param files A string containing the list of additional files.
          */
        virtual void SetAdditionalOutputFiles(const wxString& files);

        /** Deprecated, do not use at all!
          * @return True if this target should be built when the virtual target "All" is selected, false if not. */
        virtual bool GetIncludeInTargetAll() const;

        /** Deprecated, do not use at all!
          * Set if this target should be built when the virtual target "All" is selected.
          * @param buildIt If true, the target will be built with "All" else it won't. */
        virtual void SetIncludeInTargetAll(bool buildIt);

        /** Valid only for targets generating dynamic libraries (DLLs or SOs).
          * @return True if the target creates a DEF imports file. */
        virtual bool GetCreateDefFile() const;

        /** Set if the target creates a DEF imports file.
          * Valid only for targets generating dynamic libraries (DLLs or SOs).
          * @param createIt If true, a DEF file is generated else it is not. */
        virtual void SetCreateDefFile(bool createIt);

        /** Valid only for targets generating dynamic libraries (DLLs or SOs).
          * @return True if an import library will be created, false if not. */
        virtual bool GetCreateStaticLib();

        /** Set if an import library should be created.
          * Valid only for targets generating dynamic libraries (DLLs or SOs).
          * @param createIt If true, an import library will be created else it will not. */
        virtual void SetCreateStaticLib(bool createIt);

        /** Valid only for targets generating a console executable.
          * ConsoleRunner is an external utility program that waits for a keypress
          * after the target is executed.
          * @return True if ConsoleRunner should be used, false if not. */
        virtual bool GetUseConsoleRunner() const;

        /** Set if ConsoleRunner should be used.
          * Valid only for targets generating a console executable.
          * ConsoleRunner is an external utility program that waits for a keypress
          * after the target is executed.
          * @param useIt If true, ConsoleRunner is used else it is not. */
        virtual void SetUseConsoleRunner(bool useIt);

        virtual void SetTargetType(TargetType pt); // overriden

        /** Targets to be compiled (if necessary) before this one.
          * Add a target to the list of dependencies of this target. Be careful
          * not to add a target more than once.
          * @param target The build target to add as a dependency.
          */
        virtual void AddTargetDep(ProjectBuildTarget* target);

        /** @return A list of dependency targets of this target. */
        virtual BuildTargets& GetTargetDeps();

        /** Provides an easy way to iterate all the files belonging in this target.
          * @return A list of files belonging in this target. */
        virtual FilesList& GetFilesList(){ return m_Files; }

        /** @return The number of files in the target. */
        int GetFilesCount(){ return m_Files.size(); }

        /** Access a file of the target.
          * @param index The index of the file. Must be greater or equal than zero and less than GetFilesCount().
          * @return A pointer to the file or NULL if not found.
          */
        ProjectFile* GetFile(int index);

        /** Remove a file from the target.
          * @param pf The pointer to ProjectFile.
          * @return True if @c pf was a valid project file, false if not.
          */
        bool RemoveFile(ProjectFile* pf);

    private:
        friend class ProjectFile; // to allow it to add/remove files in FilesList

        cbProject*       m_Project;
        wxString         m_ExternalDeps;
        wxString         m_AdditionalOutputFiles;
        BuildTargets     m_TargetDeps;
        FilesList        m_Files;
        ProjectFileArray m_FileArray;
        bool             m_BuildWithAll; // obsolete: left just to convert old projects to use virtual targets
        bool             m_CreateStaticLib;
        bool             m_CreateDefFile;
        bool             m_UseConsoleRunner;
};

#endif // PROJECTBUILDTARGET_H
