#ifndef _GLOBAL_CONTROLS_H_
#define _GLOBAL_CONTROLS_H_

#include "SimpleJoy.h"

static bool isAcceptKey(SimpleJoy *input)
{
	return (
		(input->isB() != SimpleJoy::sjRELEASED)
		#ifdef PLATFORM_PC
		|| (input->isKey("RETURN") != SimpleJoy::sjRELEASED)
		#endif
		);
}

static bool isCancelKey(SimpleJoy *input)
{
	return (
		(input->isX() != SimpleJoy::sjRELEASED)
		#ifdef PLATFORM_PC
		|| (input->isKey("ESCAPE") != SimpleJoy::sjRELEASED)
		#endif
		);
}

#endif // _GLOBAL_CONTROLS_H_

