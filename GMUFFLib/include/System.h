/*
 * System.h
 *
 *  Created on: 26 de mai de 2021
 *      Author: josericardo
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "GEMUFFLib_global.h"

namespace GEMUFF
{
	namespace Util
	{
		class GEMUFFLIB_EXPORT System {
		private:
			static long int memUsed;


		public:
			static void informAllocated(int bytes);
			static void informDealocated(int bytes);
			static long int getAllocatedMemory();
		};

	}
}



#endif /* SYSTEM_H_ */
