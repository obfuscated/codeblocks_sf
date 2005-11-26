#ifndef FORMATTERSETTINGS_H
#define FORMATTERSETTINGS_H

#include "./astyle/astyle.h"

class FormatterSettings
{
	public:
		FormatterSettings();
		virtual ~FormatterSettings();

		void ApplyTo(astyle::ASFormatter& formatter);
};

#endif // FORMATTERSETTINGS_H
