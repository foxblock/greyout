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

#ifndef SWITCH_H
#define SWITCH_H

#include "BaseUnit.h"

class Switch : public BaseUnit
{
public:
	Switch(Level* newParent);
	virtual ~Switch();

	virtual bool load(list<PARAMETER_TYPE >& params);
	virtual bool processParameter(const PARAMETER_TYPE& value);
	virtual void generateParameters();
	virtual void reset();

	virtual void hitUnit(const UnitCollisionEntry& entry);

	virtual void update();
protected:

	typedef void (Switch::*FuncPtr)(BaseUnit* unit);
	FuncPtr switchOn;
	FuncPtr switchOff;
	PARAMETER_TYPE paramOn;
	PARAMETER_TYPE paramOff;
	void movementOn(BaseUnit* unit);
	void movementOff(BaseUnit* unit);
	void parameterOn(BaseUnit* unit);
	void parameterOff(BaseUnit* unit);

	int switchTimer;
	vector<BaseUnit*> targets;
	vector<std::string> targetIDs;

	int linkTimer;

	enum SwitchProp
	{
		spFunction=BaseUnit::upEOL,
		bpEOL
	};

	enum SwitchFunction
	{
		sfUnknown=0,
		sfMovement,
		sfParameter,
		sfParameterOn,
		sfParameterOff
	};
	static map<string,int> stringToFunc;
private:
};

#endif // SWITCH_H


