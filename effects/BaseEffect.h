/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

	Greyout is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

#ifndef BASEEFFECT_H
#define BASEEFFECT_H

#include "GFX.h"

#include "PenjinTypes.h"

enum EFFECT_TYPE
{
	etBase,
	etCustom,
	etFade,
	etZoom,
	etOverlay,
	etWipe
};

class BaseEffect
{
public:
	BaseEffect(){finished = false; type = etBase; limit = 0;};
	virtual ~BaseEffect(){};

	bool hasFinished() const {return finished;};

	EFFECT_TYPE getType() const {return type;};
	int getLimit() const {return limit;};

	// general
	virtual void update()=0;

	virtual void render()=0;

protected:
	bool finished;
	EFFECT_TYPE type;
	uint limit;
};

#endif // BASEEFFECT_H

