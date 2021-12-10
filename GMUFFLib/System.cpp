/*
 * System.cpp
 *
 *  Created on: 26 de mai de 2021
 *      Author: josericardo
 */

#include "System.h"
long int GEMUFF::Util::System::memUsed = 0;

namespace GEMUFF {
	namespace Util {
		void System::informAllocated(int bytes){ memUsed += bytes; }
		void System::informDealocated(int bytes){ memUsed -= bytes; }
		long int System::getAllocatedMemory(){ return memUsed; }
	}
}


