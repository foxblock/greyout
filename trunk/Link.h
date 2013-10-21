#ifndef _LINK_H_
#define _LINK_H_

#include "PenjinTypes.h"
#include "Line.h"
#include "Colour.h"

class BaseUnit;
class Level;

class Link
{
public:
	Link(Level *newParent, BaseUnit *src, BaseUnit *tgt);
	virtual ~Link();

	void update();
	void remove();
	void render(SDL_Surface *screen);

	BaseUnit *source;
	BaseUnit *target;

	bool toBeRemoved;
protected:
	enum LinkState
	{
		lsNone=0,
		lsFadeIn,
		lsIdle,
		lsFadeOut
	};

	LinkState state;
	int counter;
	Line line;
	Colour col;
	Level *parent;
private:

};


#endif // _LINK_H_

