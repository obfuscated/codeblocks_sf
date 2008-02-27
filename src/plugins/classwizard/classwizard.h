/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CLASSWIZARD_H
#define CLASSWIZARD_H

#include "cbplugin.h"

class ClassWizard : public cbToolPlugin
{
    public:
        ClassWizard();
        ~ClassWizard();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
        virtual int Configure(){ return -1; }
        virtual int Execute();
};

#endif // CLASSWIZARD_H
