#pragma once

#include "OtsuThresholdConfig.h"

#if defined(_WIN32) 
	#if defined(OTSU_THRESHOLD_BUILD_SHARED)
		#if defined(OTSU_THRESHOLD_EXPORTS)
			#define OTSU_THRESHOLD_EXPORT __declspec(dllexport)
		#else
			#define OTSU_THRESHOLD_EXPORT __declspec(dllimport)
		#endif // defined (OSTU_THRESHOLD_EXPORTS)
	#else
		#define OTSU_THRESHOLD_EXPORT
	#endif
#else // defined (_WIN32)
	#define OTSU_THRESHOLD_EXPORT
#endif