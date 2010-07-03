#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "03";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.07";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 553;
	static const long REVISION = 7790;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1176;
	#define RC_FILEVERSION 1,6,553,7790
	#define RC_FILEVERSION_STRING "1, 6, 553, 7790\0"
	static const char FULLVERSION_STRING[] = "1.6.553.7790";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 26;
	

}
#endif //VERSION_H
