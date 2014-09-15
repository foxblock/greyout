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

#ifndef CAMERA_TRIGGER_H
#define CAMERA_TRIGGER_H

#include "BaseTrigger.h"

class CameraTrigger : public BaseTrigger
{
public:
	CameraTrigger(Level* newParent);
	virtual ~CameraTrigger();

	virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
	virtual void doTrigger(const UnitCollisionEntry& entry);

	#ifdef _DEBUG
	virtual string debugInfo();
	#endif

	int time;
	Vector2df dest;

	enum CameraTriggerProps
	{
		cpDestination=BaseTrigger::bpEOL,
		cpTime,
		tpEOL
	};
private:

};


#endif // CAMERA_TRIGGER_H

