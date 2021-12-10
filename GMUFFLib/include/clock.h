/*
 * clock.h
 *
 *  Created on: 6 de mai de 2021
 *      Author: josericardo
 */

#ifndef CLOCK_H_
#define CLOCK_H_


//---------------
// INCLUDES
//---------------
#ifdef WIN32
#include <windows.h>
#endif

#include "GEMUFFLib_global.h"
#include <sys/time.h>
#include <cstddef>


#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4244)
#endif


namespace GEMUFF
{
	namespace Util
	{


	//---------------
	// CLASSES
	//---------------


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//  Clock
	//
	//  Descri��o:
	//     - Representa um rel�gio simples.
	//
	//

	class GEMUFFLIB_EXPORT Clock
	{
	  public:

		 Clock ();

	  public:

		 void reset ();

		 float getMilliseconds () const;

		 float getSeconds () ;

		 void setTimeSpeed (float speed) ;

	  private:

		#ifdef WIN32
		 float mCountsPerSecond;
		 __int64 mStartTime;
		#else
		 struct timeval mStartTime;
		 float mTimeSpeed;
		#endif

	};


	} // end-namespace Util
} // end-namespace JRFXGL



#ifdef _MSC_VER
  #pragma warning (pop)
#endif





#endif /* CLOCK_H_ */
