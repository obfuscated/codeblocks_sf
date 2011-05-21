#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2011";
	static const char UBUNTU_VERSION_STYLE[] = "11.05";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 7;
	static const long BUILD = 658;
	static const long REVISION = 8361;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1328;
	#define RC_FILEVERSION 1,7,658,8361
	#define RC_FILEVERSION_STRING "1, 7, 658, 8361\0"
	static const char FULLVERSION_STRING[] = "1.7.658.8361";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 31;
	

}
#endif //VERSION_H
