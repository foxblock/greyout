#ifndef _GLOBAL_CONTROLS_H_
#define _GLOBAL_CONTROLS_H_

#include "SimpleJoy.h"

static bool isAcceptKey(SimpleJoy *input)
{
	return (
		(input->isB() != SimpleJoy::sjRELEASED)
		#if defined(PLATFORM_PC) || defined(PLATFORM_PANDORA)
		&& !input->isPollingKeyboard() || (input->isKey("RETURN") != SimpleJoy::sjRELEASED) || (input->isKey("KP_ENTER") != SimpleJoy::sjRELEASED)
		#endif
		);
}

static bool isCancelKey(SimpleJoy *input)
{
	return (
		(input->isX() != SimpleJoy::sjRELEASED)
		#if defined(PLATFORM_PC) || defined(PLATFORM_PANDORA)
		&& !input->isPollingKeyboard() || (input->isKey("ESCAPE") != SimpleJoy::sjRELEASED)
		#endif
		);
}

#endif // _GLOBAL_CONTROLS_H_

