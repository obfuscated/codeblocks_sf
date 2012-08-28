/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSCODER_H
#define WXSCODER_H

#include <cbeditor.h>
#include <wx/string.h>
#include <wx/timer.h>

/** \brief Class putting new code into proper files
 *
 * \warning Current implementation does not schedule coded upgrades which may
 *          cause bad results in performance.
 */
class wxsCoder: public wxEvtHandler
{
    public:

        wxsCoder();
        virtual ~wxsCoder();

        /** \brief Function notifying about change of block of code in file
         *
         * Code should use tabs for indentation and will be automatically
         * indented relatively to code header.
         *
         * \param FileName Full path for processed file
         * \param Header Header beginning code block
         * \param End Sequence ending code block
         * \param Code new code
         * \param Immediately flag forcing immediate apply (currently not used
         *        but may be added in future when updates will be scheduled)
         * \param CodeHasHeader if true, header will also be recreated
         *        (new header should be placed at the beginning of Code)
         * \param CodeHasEnd if true, ending sequence will also be recreated
         *        (new ending sequence should be placed at the end of Code)
         */
        void AddCode(
            const wxString& FileName,
            const wxString& Header,
            const wxString& End,
            const wxString& Code,
            bool Immediately=true,
            bool CodeHasHeader=false,
            bool CodeHasEnd=false);

        /** \brief Function reading block of code from given source file
         *
         * \param FileName Full path for processed file
         * \param Header Header beginning code block
         * \param End Sequence ending code block
         * \param IncludeHeader if true, returned code will also contain header
         * \param IncludeEnd if true, returned code will also contain ending sequence
         */
        wxString GetCode(
            const wxString& FileName,
            const wxString& Header,
            const wxString& End,
            bool IncludeHeader=false,
            bool IncludeEnd=false);

        /** \brief Getting full file code */
        wxString GetFullCode(const wxString& FileName,wxFontEncoding& Encoding,bool &UseBOM);

        /** \brief Writing code for whole file */
        void PutFullCode(const wxString& FileName,const wxString& Code,wxFontEncoding Encoding,bool UseBOM);

        /** \brief Applying all pending code changes */
        void Flush(int Delay);

        /** \brief Function getting singleton object from system */
        static wxsCoder* Get() { return Singleton; }

    private:

        /** \brief Structure which contains one data change */
        struct CodeChange
        {
            wxString Header;
            wxString End;
            wxString Code;
            bool CodeHasHeader;
            bool CodeHasEnd;
            CodeChange* Next;
        };
        WX_DEFINE_ARRAY(CodeChange*,CodeChangeArray);

        /** \brief Mutex for this object - added in case of multi-threading schedules */
        wxMutex DataMutex;

        /** \brief Timer used for delayed flushes */
        wxTimer FlushTimer;

        /** \brief Temporary storage place where changes are stored */
        CodeChangeArray CodeChanges;

        /** \brief File names which are changed */
        wxArrayString CodeChangesFiles;

        /** \brief Function applying changes to currently opened editor */
        bool ApplyChangesEditor(
            cbEditor* Editor,
            const wxString& Header,
            const wxString& End,
            wxString& Code,
            bool CodeHasHeader,
            bool CodeHasEnd,
            wxString& EOL);

        /** \brief Applying changes to string (file's content) */
        bool ApplyChangesString(
            wxString& Content,
            const wxString& Header,
            const wxString& Env,
            wxString& Code,
            bool CodeHasHeader,
            bool CodeHasEnd,
            bool& HasChanged,
            wxString& EOL);

        /** \brief Flushing all changes for given file */
        void FlushFile(const wxString& FileName);

        /** \brief Flushing all files */
        void FlushAll();

        /** \brief Flush timer procedure */
        void FlushTimerEvent(wxTimerEvent& event);

        /** \brief Rebuilding code to support current editor settings */
        wxString RebuildCode(wxString& BaseIndentation,const wxChar* Code,int CodeLen,wxString& EOL);

        /** \brief Cutting off given number of spaces at every new line */
        wxString CutSpaces(wxString Code,int Count);

        /** \brief Normalizing given file name */
        static wxString NormalizeFileName(const wxString& FileName);

        /** \brief Singleton object */
        static wxsCoder* Singleton;
};

/** \page Auto-Code Code automatically generated by wxSmith
 *
 * Here's list of automatically generated code:
 *
 * \li \c //(*EventTable($CLASSNAME) - generated in class source file, contains
 *                                  entries for event table
 * \li \c //(*Initialize($CLASSNAME) - generated in class source file, this code
 *                                  does all resource initialization (loading
 *                                  XRC, adding widgets etc.)
 * \li \c //(*Headers($CLASSNAME) - generated in class header file, this block
 *                                  contains set of #includes including required
 *                                  header files for this resource
 * \li \c //(*Identifiers($CLASSNAME) - generated in class header file, this
 *                                  code generates identifiers for window
 *                                  items (usually enum), in case of XRC
 *                                  resource (with it's own identifier
 *                                  handling system), it will be empty
 * \li \c //(*Handlers($CLASSNAME) - generated in class header. It contains
 *                                  declarations of event handler functions,
 *                                  inside this code, user may put it's own
 *                                  event handler declarations but they must
 *                                  be in form:
 *                                     void HandlerName(eventType& event).
 *                                  This block is parsed when generating list
 *                                  of event handlers which may be used with
 *                                  given event type.
 * \li \c //(*Declarations($CLASSNAME) - declarations of window items. This
 *                                  block will contain declarations of all
 *                                  window items which have "Is Member"
 *                                  property set to true.
 *
 * Blocks which will be added in future:
 *
 * \li \c //(*AppHeaders - declared in main application's source file.
 *                                 This block will contain set of #includes
 *                                 required by application.
 * \li \c //(*AppInitialize - declared in main application's source file.
 *                                 This block will automatically load resources
 *                                 and show main application's window
 */

#endif
