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

#include "Settings.h"

#include "MusicCache.h"
#include "gameDefines.h"
#include "GFX.h"
#include "Savegame.h"
#include "StringUtility.h"
#include "SimpleJoy.h"
#include "SurfaceCache.h"

#ifdef _MEOW
#else
#endif

#ifdef _MEOW
#define SETTINGS_TEXT_SIZE 24
#define SETTINGS_RECT_HEIGHT 18
#define SETTINGS_CHECK_HEIGHT 14
#define SETTINGS_MENU_SPACING 4
#define SETTINGS_MENU_OFFSET_Y -20
#define SETTINGS_VOLUME_SLIDER_SIZE 125
#else
#define SETTINGS_TEXT_SIZE 48
#define SETTINGS_RECT_HEIGHT 35
#define SETTINGS_CHECK_HEIGHT 27
#define SETTINGS_MENU_SPACING 10
#define SETTINGS_MENU_OFFSET_Y 0
#define SETTINGS_VOLUME_SLIDER_SIZE 400
#endif
#define SETTINGS_MENU_OFFSET_X 20
#define SETTINGS_MENU_SPACING_EXTRA 10

Settings::Settings() :
	active( false ),
	sel( 0 ),
	mouseInBounds( false ),
	lastPos( 0, 0 )
{
	menuText.loadFont( GAME_FONT, SETTINGS_TEXT_SIZE );
	menuText.setColour( WHITE );
	menuText.setAlignment( LEFT_JUSTIFIED );
	menuText.setUpBoundary( Vector2di( GFX::getXResolution(), GFX::getYResolution() ) );
	entriesText.loadFont( GAME_FONT, SETTINGS_TEXT_SIZE );
	entriesText.setColour( WHITE );
	entriesText.setAlignment( CENTRED );
	entriesText.setDownBoundary( Vector2di( ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X, 0 ) );
	entriesText.setUpBoundary( Vector2di( ( int )GFX::getXResolution() - SETTINGS_MENU_OFFSET_X, GFX::getYResolution() ) );

	menuItems.push_back( "MUSIC VOL:" );
	menuItems.push_back( "SOUND VOL:" );
	menuItems.push_back( "RENDER WRAP:" );
	menuItems.push_back( "PARTICLES:" );
	menuItems.push_back( "HINT LINKS:" );
	menuItems.push_back( "DRAW FPS:" );
	menuItems.push_back( "WRITE FPS:" );
	menuItems.push_back( "DEBUG:" );
	menuItems.push_back( "RETURN" );

	patternStrings.push_back( "OFF" );
	patternStrings.push_back( "ARROWS" );
	patternStrings.push_back( "SHADED" );
	patternStrings.push_back( "FULL" );

	particleStrings.push_back( "OFF" );
	particleStrings.push_back( "FEW" );
	particleStrings.push_back( "MANY" );
	particleStrings.push_back( "TOO MANY" );

	arrows.loadFrames( "images/general/arrows2.png", 4, 1 );
	arrows.setTransparentColour( MAGENTA );
	arrows.setRotation( 0 );

	loadFromFile();
}

Settings::~Settings()
{
	saveToFile();
}


///--- PUBLIC ------------------------------------------------------------------

int Settings::update()
{
	//
}

void Settings::render( SDL_Surface *screen )
{
	int pos = ( GFX::getYResolution() - ( SETTINGS_MENU_SPACING + SETTINGS_RECT_HEIGHT ) * menuItems.size() ) / 2 + SETTINGS_MENU_OFFSET_Y;

	SDL_Rect rect;

	// render text and selection
	for ( int I = 0; I < menuItems.size(); ++I )
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = SETTINGS_RECT_HEIGHT;
		menuText.setPosition( SETTINGS_MENU_OFFSET_X, pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE );
		if ( I == sel )
		{
			SDL_FillRect( screen, &rect, -1 );
			menuText.setColour( BLACK );
		}
		else
		{
			SDL_FillRect( screen, &rect, 0 );
			menuText.setColour( WHITE );
		}
		menuText.print( menuItems[I] );

		if ( I == 0 || I == 1 )	// render volume sliders
		{
			int value;
			if ( I == 0 )
			{
				value = MUSIC_CACHE->getMusicVolume();
			}
			else
			{
				value = MUSIC_CACHE->getSoundVolume();
			}
			rect.w = ( float )SETTINGS_VOLUME_SLIDER_SIZE * ( float )value / ( float )MUSIC_CACHE->getMaxVolume();
			rect.x = ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X;
			if ( I == sel )
				SDL_FillRect( screen, &rect, 0 );
			else
				SDL_FillRect( screen, &rect, -1 );
			if ( I == sel && value > 0 )
			{
				arrows.setCurrentFrame( 3 );
				arrows.setPosition( ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X - arrows.getWidth(),
									pos + ( SETTINGS_RECT_HEIGHT - arrows.getHeight() ) / 2 );
				arrows.render( screen );
			}
			if ( I == sel && value < MUSIC_CACHE->getMaxVolume() )
			{
				arrows.setCurrentFrame( 1 );
				arrows.setPosition( ( int )GFX::getXResolution() - SETTINGS_MENU_OFFSET_X,
									pos + ( SETTINGS_RECT_HEIGHT - arrows.getHeight() ) / 2 );
				arrows.render( screen );
			}
		}
		else if ( I == 2 || I == 3 )
		{
			entriesText.setPosition( ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X,
									 pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE );
			if ( I == sel )
				entriesText.setColour( BLACK );
			else
				entriesText.setColour( WHITE );
			int value, maxValue;
			if ( I == 2 )
			{
				value = drawPattern;
				maxValue = dpEOL;
				entriesText.print( patternStrings[drawPattern] );
			}
			else
			{
				value = particleDensity;
				maxValue = pdEOL;
				entriesText.print( particleStrings[particleDensity] );
			}
			if ( I == sel && value > 0 )
			{
				arrows.setCurrentFrame( 3 );
				arrows.setPosition( ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.925f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
									pos + ( SETTINGS_RECT_HEIGHT - arrows.getHeight() ) / 2 );
				arrows.render( screen );
			}
			if ( I == sel && value < maxValue - 1 )
			{
				arrows.setCurrentFrame( 1 );
				arrows.setPosition( ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.075f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
									pos + ( SETTINGS_RECT_HEIGHT - arrows.getHeight() ) / 2 );
				arrows.render( screen );
			}
		}
		else if ( I < 8 )
		{
			rect.w = SETTINGS_RECT_HEIGHT;
			rect.x = ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if ( I == sel )
				SDL_FillRect( screen, &rect, 0 );
			else
				SDL_FillRect( screen, &rect, -1 );
			rect.w = SETTINGS_CHECK_HEIGHT;
			rect.h = SETTINGS_CHECK_HEIGHT;
			rect.x += ( SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT ) / 2;
			rect.y += ( SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT ) / 2;
			bool temp;
			if ( I == 4 )
				temp = drawLinks;
			else if ( I == 5 )
				temp = drawFps;
			else if ( I == 6 )
				temp = writeFps;
			else
				temp = debugControls;
			if ( I == sel && !temp )
				SDL_FillRect( screen, &rect, -1 );
			else if ( I != sel && !temp )
				SDL_FillRect( screen, &rect, 0 );
		}

		pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
		if ( I == 7 )
			pos += SETTINGS_MENU_SPACING_EXTRA;
	}
}

void Settings::userInput( SimpleJoy *input )
{
	Vector2di mousePos = input->getMouse();
	int oldSel = sel;
	int pos = ( GFX::getYResolution() - ( SETTINGS_MENU_SPACING + SETTINGS_RECT_HEIGHT ) * menuItems.size() ) / 2 + SETTINGS_MENU_OFFSET_Y;
	mouseInBounds = false;
	if ( mousePos != lastPos || input->isLeftClick() || input->isRightClick() )
	{
		for ( int I = 0; I < menuItems.size(); ++I )
		{
			if ( mousePos.y >= pos && mousePos.y <= pos + SETTINGS_RECT_HEIGHT )
			{
				sel = I;
				mouseInBounds = true;
			}
			pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
			if ( I == 7 )
				pos += SETTINGS_MENU_SPACING_EXTRA;
		}
		lastPos = mousePos;
	}

	if ( input->isUp() && sel > 0 )
	{
		--sel;
		input->resetUp();
	}
	if ( input->isDown() && sel < menuItems.size() - 1 )
	{
		++sel;
		input->resetDown();
	}

	if ( input->isLeft() )
	{
		if ( sel == 0 )
		{
			int vol = getMusicVolume();
			setMusicVolume( max( vol - 2, 0 ) );
		}
		else if ( sel == 1 )
		{
			int vol = getSoundVolume();
			setSoundVolume( max( vol - 2, 0 ) );
		}
		else if ( sel == 2 )
			setDrawPattern( drawPattern - 1 );
		else if ( sel == 3 )
			setParticleDensity( particleDensity - 1 );
		else if ( sel == 4 )
			drawLinks = !drawLinks;
		else if ( sel == 5 )
			drawFps = !drawFps;
		else if ( sel == 6 )
			writeFps = !writeFps;
		else if ( sel == 7 )
			debugControls = !debugControls;
		if ( sel > 1 )
			input->resetLeft();
	}
	else if ( input->isRight() )
	{
		if ( sel == 0 )
		{
			int vol = getMusicVolume();
			setMusicVolume( min( vol + 2, MUSIC_CACHE->getMaxVolume() ) );
		}
		else if ( sel == 1 )
		{
			int vol = getSoundVolume();
			setSoundVolume( min( vol + 2, MUSIC_CACHE->getMaxVolume() ) );
		}
		else if ( sel == 2 )
			setDrawPattern( drawPattern + 1 );
		else if ( sel == 3 )
			setParticleDensity( particleDensity + 1 );
		else if ( sel == 4 )
			drawLinks = !drawLinks;
		else if ( sel == 5 )
			drawFps = !drawFps;
		else if ( sel == 6 )
			writeFps = !writeFps;
		else if ( sel == 7 )
			debugControls = !debugControls;
		if ( sel > 1 )
			input->resetRight();
	}

	if ( ACCEPT_KEY || ( input->isLeftClick() && mouseInBounds ) )
	{
		if ( sel == 0 )
		{
			if ( input->isLeftClick() )
			{
				float factor = ( float )( mousePos.x - ( GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X ) ) / ( float )SETTINGS_VOLUME_SLIDER_SIZE;
				if ( factor >= 0.0f && factor <= 1.0f )
					setMusicVolume( ( float )MUSIC_CACHE->getMaxVolume() * factor );
			}
		}
		else if ( sel == 1 )
		{
			if ( input->isLeftClick() )
			{
				float factor = ( float )( mousePos.x - ( GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X ) ) / ( float )SETTINGS_VOLUME_SLIDER_SIZE;
				if ( factor >= 0.0f && factor <= 1.0f )
					setSoundVolume( ( float )MUSIC_CACHE->getMaxVolume() * factor );
			}
		}
		else if ( sel == 2 )
		{
			if ( mousePos.x > ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.85f - SETTINGS_MENU_OFFSET_X )
				setDrawPattern( drawPattern - 1 );
			else if ( mousePos.x > ( int ) GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.15f - SETTINGS_MENU_OFFSET_X &&
					  mousePos.x < ( int )GFX::getXResolution() - SETTINGS_MENU_OFFSET_X )
				setDrawPattern( drawPattern + 1 );
		}
		else if ( sel == 3 )
		{
			if ( mousePos.x > ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.85f - SETTINGS_MENU_OFFSET_X )
				setParticleDensity( particleDensity - 1 );
			else if ( mousePos.x > ( int ) GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.15f - SETTINGS_MENU_OFFSET_X &&
					  mousePos.x < ( int )GFX::getXResolution() - SETTINGS_MENU_OFFSET_X )
				setParticleDensity( particleDensity + 1 );
		}
		else if ( sel == 4 )
		{
			int temp =  ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if ( ACCEPT_KEY || ( mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT ) )
				drawLinks = !drawLinks;
		}
		else if ( sel == 5 )
		{
			int temp =  ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if ( ACCEPT_KEY || ( mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT ) )
				drawFps = !drawFps;
		}
		else if ( sel == 6 )
		{
			int temp =  ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if ( ACCEPT_KEY || ( mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT ) )
				writeFps = !writeFps;
		}
		else if ( sel == 7 )
		{
			int temp =  ( int )GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if ( ACCEPT_KEY || ( mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT ) )
				debugControls = !debugControls;
		}
		else if ( sel == 8 )
			close();
		if ( sel > 1 )
		{
			input->resetMouseButtons();
			input->resetB();
		}
	}
	if ( CANCEL_KEY )
	{
		if ( sel == 0 )
			MUSIC_CACHE->setMusicVolume( 0 );
		else if ( sel == 1 )
			MUSIC_CACHE->setSoundVolume( 0 );
	}
	if ( sel != oldSel )
	{
		if ( sel == 1 )
			MUSIC_CACHE->playSound( "sounds/menu_testsound.wav", -1 );
		else
			MUSIC_CACHE->stopSounds();
	}

	if ( CANCEL_KEY || input->isRightClick() )
	{
		input->resetKeys();
		input->resetMouseButtons();
		close();
	}
}

void Settings::show()
{
	sel = 0;
	active = true;
}

void Settings::close()
{
	active = false;
	MUSIC_CACHE->stopSounds();
}

void Settings::loadFromFile()
{
	if ( SAVEGAME->hasData( "musicvolume" ) )
		MUSIC_CACHE->setMusicVolume( StringUtility::stringToInt( SAVEGAME->getData( "musicvolume" ) ) );
	if ( SAVEGAME->hasData( "soundvolume" ) )
		MUSIC_CACHE->setSoundVolume( StringUtility::stringToInt( SAVEGAME->getData( "soundvolume" ) ) );

	if ( SAVEGAME->hasData( "drawpattern" ) )
		setDrawPattern( StringUtility::stringToInt( SAVEGAME->getData( "drawpattern" ) ) );
	else
		setDrawPattern( dpShaded );
	if ( SAVEGAME->hasData( "particledensity" ) )
		setParticleDensity( StringUtility::stringToInt( SAVEGAME->getData( "particledensity" ) ) );
	else
		setParticleDensity( pdMany );
	if ( SAVEGAME->hasData( "drawlinks" ) )
		setDrawLinks( StringUtility::stringToBool( SAVEGAME->getData( "drawlinks" ) ) );
	else
		setDrawLinks( true );
	if ( SAVEGAME->hasData( "drawfps" ) )
		setDrawFps( StringUtility::stringToBool( SAVEGAME->getData( "drawfps" ) ) );
	else
		setDrawFps( false );
	if ( SAVEGAME->hasData( "writefps" ) )
		setWriteFps( StringUtility::stringToBool( SAVEGAME->getData( "writefps" ) ) );
	else
		setWriteFps( false );
	if ( SAVEGAME->hasData( "debugcontrols" ) )
		setDebugControls( StringUtility::stringToBool( SAVEGAME->getData( "debugcontrols" ) ) );
	else
		setDebugControls( false );
}

void Settings::saveToFile()
{
	SAVEGAME->writeData( "musicvolume", StringUtility::intToString( MUSIC_CACHE->getMusicVolume() ), true );
	SAVEGAME->writeData( "soundvolume", StringUtility::intToString( MUSIC_CACHE->getSoundVolume() ), true );
	SAVEGAME->writeData( "drawpattern", StringUtility::intToString( drawPattern ), true );
	SAVEGAME->writeData( "particledensity", StringUtility::intToString( particleDensity ), true );
	SAVEGAME->writeData( "drawlinks", StringUtility::boolToString( drawLinks ), true );
	SAVEGAME->writeData( "drawfps", StringUtility::boolToString( drawFps ), true );
	SAVEGAME->writeData( "writefps", StringUtility::boolToString( writeFps ), true );
	SAVEGAME->writeData( "debugcontrols", StringUtility::boolToString( debugControls ), true );
}

/// --- getters and setters ----------------------------------------------------

int Settings::getMusicVolume()
{
	return MUSIC_CACHE->getMusicVolume();
}

void Settings::setMusicVolume( CRint newVol )
{
	MUSIC_CACHE->setMusicVolume( newVol );
}

int Settings::getSoundVolume()
{
	return MUSIC_CACHE->getSoundVolume();
}

void Settings::setSoundVolume( CRint newVol )
{
	MUSIC_CACHE->setSoundVolume( newVol );
}

int Settings::getDrawPattern()
{
	return drawPattern;
}

void Settings::setDrawPattern( CRint newDp )
{
	if ( newDp >= 0 && newDp < dpEOL )
		drawPattern = newDp;
}

int Settings::getParticleDensity()
{
	return particleDensity;
}

void Settings::setParticleDensity( CRint newPd )
{
	if ( newPd >= 0 && newPd < pdEOL )
		particleDensity = newPd;
}

bool Settings::getDrawLinks()
{
	return drawLinks;
}

void Settings::setDrawLinks( CRbool newLinks )
{
	drawLinks = newLinks;
}

bool Settings::getDrawFps()
{
	return drawFps;
}

void Settings::setDrawFps( CRbool newFps )
{
	drawFps = newFps;
}

bool Settings::getWriteFps()
{
	return writeFps;
}

void Settings::setWriteFps( CRbool newFps )
{
	writeFps = newFps;
}

bool Settings::getDebugControls()
{
	return debugControls;
}

void Settings::setDebugControls( CRbool newDebug )
{
	debugControls = newDebug;
}


///--- PROTECTED ---------------------------------------------------------------

///--- PRIVATE -----------------------------------------------------------------
