/*
Copyright (c) 2015 by Rosalba Giugno

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef WIN32 // definita nelle direttive del preprocessore in Visual Studio 7.0

#include <Windows.h>
typedef LARGE_INTEGER TIMEHANDLE;

extern LARGE_INTEGER _freq;

inline TIMEHANDLE start_time()
{
    static int first = 1;

    if(first) {
        if (!QueryPerformanceFrequency(&_freq))
			cout << "No high-resolution performance counter installed" << endl;

		if (_freq.QuadPart == 0)
			cout << "High-resolution performance counter available but _freq = 0" << endl;
        first = 0;
    }
    TIMEHANDLE tstart;
	QueryPerformanceCounter(&tstart);
	return tstart;
}
inline double end_time(TIMEHANDLE th)
{
    TIMEHANDLE tend;
    QueryPerformanceCounter(&tend);

	double res = (double)(tend.QuadPart - th.QuadPart)/(_freq.QuadPart); 
	return res;
}

#pragma warning( disable : 4267 4018)

#else // sotto linux

#include <sys/time.h>

typedef struct timeval TIMEHANDLE;

extern struct timezone _tz;
inline TIMEHANDLE start_time()
{
    TIMEHANDLE tstart;
    gettimeofday(&tstart, &_tz);
    return tstart;
}
inline double end_time(TIMEHANDLE th)
{
    TIMEHANDLE tend;
    double t1, t2;

	gettimeofday(&tend,&_tz);

    t1 =  (double)th.tv_sec + (double)th.tv_usec/(1000*1000);
    t2 =  (double)tend.tv_sec + (double)tend.tv_usec/(1000*1000);
    return t2-t1;
}
#endif //WIN32

#ifdef WIN32
LARGE_INTEGER _freq;
#else
struct timezone _tz;
#endif


#endif
