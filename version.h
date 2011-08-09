#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2011";
	static const double UBUNTU_VERSION_STYLE = 10.10;

	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";

	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 0;
	static const long REVISION = 5;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 42;
	#define RC_FILEVERSION 1,0,2266,12538
	#define RC_FILEVERSION_STRING "1, 0, 2266, 12538\0"
	static const char FULLVERSION_STRING[] = "1.1.0.5";

	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 270;


}
#endif //VERSION_h
