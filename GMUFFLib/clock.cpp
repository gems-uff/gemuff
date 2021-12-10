/*
 * clock.cpp
 *
 *  Created on: 7 de mai de 2021
 *      Author: josericardo
 */


#include "clock.h"

namespace GEMUFF
{
	namespace Util
	{
		Clock::Clock(){mTimeSpeed = 1.0f;}

		void Clock::reset ()
		{
			#ifdef WIN32
				mStartTime = 0;
				QueryPerformanceCounter((LARGE_INTEGER*) &mStartTime);
			#else
				gettimeofday(&mStartTime, NULL);
			#endif
		}

		float Clock::getMilliseconds () const
		{
			#ifdef WIN32
			 __int64 currentTime = 0;
			 QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);
			 return (currentTime - mStartTime) * mCountsPerSecond * 1000.0f;
			#else
			 struct timeval t_time;
             gettimeofday(&t_time, NULL);
			 return (float) (1000.0f * (float)(t_time.tv_sec - mStartTime.tv_sec)
                                      + (0.001f  * (float)(t_time.tv_usec - mStartTime.tv_usec)) ) * mTimeSpeed;
			#endif
		}

		float Clock::getSeconds ()
		{
			#ifdef WIN32
			 __int64 currentTime = 0;
			 QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);
			 return (currentTime - mStartTime) * mCountsPerSecond;
			#else
				return getMilliseconds () * 0.001f;
			#endif
		}

		void Clock::setTimeSpeed (float speed)
		{
			#ifndef WIN32
				mTimeSpeed = speed;
			#endif
		}
	}
}
