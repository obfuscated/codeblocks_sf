/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef DEVCPPLOADER_H
#define DEVCPPLOADER_H

#include "ibaseloader.h"

// forward decls
class cbProject;

class DevCppLoader : public IBaseLoader
{
	public:
		DevCppLoader(cbProject* project);
		virtual ~DevCppLoader();

		bool Open(const wxString& filename);
		bool Save(const wxString& filename);
	protected:
        cbProject* m_pProject;
	private:
        DevCppLoader(){} // no default ctor
};

#endif // DEVCPPLOADER_H

