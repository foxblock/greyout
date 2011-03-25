#ifndef USERSTATES_H
#define USERSTATES_H

// A list of identifiers for the states
// You may include this in any state you need to manually transition to another
// state by a call to setNextState
enum STATE_MODES
{
    STATE_ERROR,
	STATE_TITLE,
	STATE_MAIN,
	STATE_LEVEL,
	STATE_NEXT, // special state indicating the switch to the next level in a chapter (not an actual state)
	STATE_THIS, // special state used to reset the current level
	STATE_BENCHMARK,
	STATE_LEVELSELECT
};

#endif	//	USERSTATES_H
