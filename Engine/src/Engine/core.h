#pragma once
	
#ifdef FOREST_PLATFORM_WINDOWS
	#ifdef FOREST_BUILD_DLL
		#define FOREST_API __declspec(dllexport)
	#else
		#define FOREST_API __declspec(dllimport)
	#endif
#else 
	#error Forest only supports Windows!
#endif

