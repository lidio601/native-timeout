//
// Created by Fabio Cigliano on 02/07/18.
//

#include "common.h"

std::string stringValue(Local<Value> value) {
    if (!value->IsString()) {
        return "";
    }

    // Alloc #1
    char * buffer = (char*) malloc(sizeof(char) * value->ToString()->Utf8Length());
    value->ToString()->WriteUtf8(buffer, value->ToString()->Utf8Length());
    std::string ret(buffer);
    free(buffer);

    return ret;
}

/*
 * a portable time function
 */

#ifdef __GNUC__
#include <time.h>
float getticks()
{
	struct timespec ts;

	if(clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
		return -1.0f;

	return ts.tv_sec + 1e-9f*ts.tv_nsec;
}
#else
#include <windows.h>
float getticks()
{
	static double freq = -1.0;
	LARGE_INTEGER lint;

	if(freq < 0.0)
	{
		if(!QueryPerformanceFrequency(&lint))
			return -1.0f;

		freq = lint.QuadPart;
	}

	if(!QueryPerformanceCounter(&lint))
		return -1.0f;

	return (float)( lint.QuadPart/freq );
}
#endif
