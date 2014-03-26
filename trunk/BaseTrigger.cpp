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

#include "BaseTrigger.h"

#include "StringUtility.h"
#include "Level.h"
#include "ControlUnit.h"

BaseTrigger::BaseTrigger( Level *newParent ) : BaseUnit( newParent )
{
	stringToProp["size"] = BaseUnit::upSize;
	stringToProp["enabled"] = bpEnabled;
	stringToProp["action"] = bpAction;
	stringToProp["activator"] = bpActivator;
	stringToProp["autoreenable"] = bpAutoReEnable;
	size.x = 32;
	size.y = 32;
	collisionColours.insert( Colour( BLACK ).getIntColour() );
	collisionColours.insert( Colour( WHITE ).getIntColour() );
	flags.addFlag( ufNoMapCollision );
	flags.addFlag( ufNoGravity );
	flags.addFlag( ufInvincible );
	unitCollisionMode = 0;
	triggerCol = YELLOW;
	enabled = true;
	autoReEnable = false;
	targetParam.first = "";
	targetParam.second = "";
	enableTimer = 0;
	actionHitTarget = true;
}

BaseTrigger::~BaseTrigger()
{
	targets.clear();
	activators.clear();
	targetIDs.clear();
	activatorIDs.clear();
}

///---public---

bool BaseTrigger::processParameter( const PARAMETER_TYPE &value )
{
	bool parsed = true;

	switch ( stringToProp[value.first] )
	{
	case BaseUnit::upSize:
	{
		size = StringUtility::stringToVec<Vector2di>(value.second);
		break;
	}
	case bpEnabled:
	{
		enabled = StringUtility::stringToBool( value.second );
		break;
	}
	case BaseUnit::upTarget:
	{
		parsed = pLoadUintIDs( value.second, targetIDs );
		break;
	}
	case bpAction:
	{
		vector<string> tokens;
		StringUtility::tokenize( value.second, tokens, VALUE_STRING, 2 );
		if ( tokens.size() < 2 )
		{
			parsed = false;
			break;
		}
		targetParam.first = tokens.front();
		targetParam.second = tokens.back();
		break;
	}
	case bpActivator:
	{
		parsed = pLoadUintIDs( value.second, activatorIDs );
		break;
	}
	case bpAutoReEnable:
	{
		autoReEnable = StringUtility::stringToBool( value.second );
		break;
	}
	default:
		parsed = false;
	}

	if ( not parsed )
		return BaseUnit::processParameter( value );

	return parsed;
}

void BaseTrigger::reset()
{
	enabled = true;
	size.x = 32;
	size.y = 32;
	enableTimer = 0;
	BaseUnit::reset();
}

void BaseTrigger::update()
{
	if ( !targetIDs.empty() )
	{
		targets.clear();
		parent->getUnitsByID( targetIDs, targets );
		targetIDs.clear();
	}
	if ( !activatorIDs.empty() )
	{
		activators.clear();
		parent->getUnitsByID( activatorIDs, activators );
		activatorIDs.clear();
	}
	if ( enableTimer > 0 )
	{
		--enableTimer;
		if ( enableTimer == 0 )
			enabled = true;
	}
	BaseUnit::update();
}

void BaseTrigger::render( SDL_Surface *surf )
{
#ifdef _DEBUG
	SDL_Rect temp;
	Colour tempC = triggerCol;
	if ( !enabled )
		tempC = tempC - 64;
	temp.x = position.x;
	temp.y = position.y;
	temp.w = size.x;
	temp.h = 1;
	SDL_FillRect( surf, &temp, tempC.getSDL_Uint32Colour( surf ) );
	temp.y += size.y;
	SDL_FillRect( surf, &temp, tempC.getSDL_Uint32Colour( surf ) );
	temp.w = 1;
	temp.h = size.y;
	temp.y -= size.y;
	SDL_FillRect( surf, &temp, tempC.getSDL_Uint32Colour( surf ) );
	temp.x += size.x;
	SDL_FillRect( surf, &temp, tempC.getSDL_Uint32Colour( surf ) );
#else
	// Don't render anything
#endif
}

void BaseTrigger::hitUnit( const UnitCollisionEntry &entry )
{
	if ( enabled )
	{
		if ( !activators.empty() )
		{
			bool found = false;
			for ( vector<BaseUnit *>::iterator I = activators.begin(); I != activators.end(); ++I )
			{
				if ( *I == entry.unit )
				{
					found = true;
					break;
				}
			}
			if ( !found )
				return; // not hit by activator unit, exit here
		}
		// activator units may be any explicitly named unit or any player (by default)
		if ( !activators.empty() || entry.unit->isPlayer )
		{
			doTrigger( entry ); // perform specific trigger behaviour
			// perform generic action parameter
			if ( targetParam.first[0] != 0 )
			{
				if ( targets.empty() && actionHitTarget )
				{
					if ( targetParam.first == "order" )
						entry.unit->resetOrder( true ); // clear order list
					entry.unit->processParameter( targetParam );
					// orders need an additional kickstart to work
					if ( targetParam.first == "order" )
						entry.unit->resetOrder( false );
				}
				for ( vector<BaseUnit *>::iterator I = targets.begin(); I != targets.end(); ++I )
				{
					if ( targetParam.first == "order" )
						( *I )->resetOrder( true ); // clear order list
					( *I )->processParameter( targetParam );
					// orders need an additional kickstart to work
					if ( targetParam.first == "order" )
						( *I )->resetOrder( false );
				}
			}
		}
	}
}

bool BaseTrigger::checkCollisionColour( const Colour &col ) const
{
	set<int>::const_iterator iter = collisionColours.find( col.getIntColour() );
	if ( iter != collisionColours.end() )
		return true;
	return false;
}

#ifdef _DEBUG
string BaseTrigger::debugInfo()
{
	string result = BaseUnit::debugInfo();
	if ( enabled )
		result += "enabled\n";
	else
		result += "disabled\n";
	if ( !targets.empty() )
	{
		result += "T: ";
		for ( vector<BaseUnit *>::iterator I = targets.begin(); I != targets.end(); ++I )
			result += ( *I )->id + ", ";
		result += "\n";
	}
	if ( !activators.empty() )
	{
		result += "U: ";
		for ( vector<BaseUnit *>::iterator I = activators.begin(); I != activators.end(); ++I )
			result += ( *I )->id + ", ";
		result += "\n";
	}
	if ( targetParam.first[0] != 0 )
		result += "A: " + targetParam.first + "=" + targetParam.second + "\n";
	return result;
}
#endif

///---protected---

void BaseTrigger::doTrigger( const UnitCollisionEntry &entry )
{
	enabled = false;
	if ( autoReEnable )
		enableTimer = 3;
}

///---private---
