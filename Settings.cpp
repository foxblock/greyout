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
#include "IMG_savepng.h"
#include "globalControls.h"

#ifdef _MEOW
#else
#endif

#ifdef _MEOW
#define SETTINGS_HEADLINE_SIZE 36
#define SETTINGS_TEXT_SIZE 24
#define SETTINGS_RECT_HEIGHT 18
#define SETTINGS_CHECK_HEIGHT 14
#define SETTINGS_MENU_SPACING 4
#define SETTINGS_MENU_OFFSET_Y 46
#define SETTINGS_VOLUME_SLIDER_SIZE 125
#define SETTINGS_RETURN_Y_POS 200
#else
#define SETTINGS_HEADLINE_SIZE 72
#define SETTINGS_TEXT_SIZE 48
#define SETTINGS_RECT_HEIGHT 35
#define SETTINGS_CHECK_HEIGHT 27
#define SETTINGS_MENU_SPACING 10
#define SETTINGS_MENU_OFFSET_Y 92
#define SETTINGS_VOLUME_SLIDER_SIZE 400
#define SETTINGS_RETURN_Y_POS 400
#endif
#define SETTINGS_MENU_OFFSET_X 20
#define SETTINGS_MENU_SPACING_EXTRA 10

map<int,int> Settings::savePos;

Settings::Settings() :
	active(false),
	category(-1),
	sel(0),
	lastPos(0, 0),
	mouseInBounds(false),
	usedMouse(false)
{
	loadFromFile();
	headlineRect.x = 0;
	headlineRect.y = SETTINGS_HEADLINE_SIZE * 0.25f + 1;
	headlineRect.w = GFX::getXResolution();
	headlineRect.h = SETTINGS_HEADLINE_SIZE * 0.75f;
	headline = new Text();
	headline->loadFont(GAME_FONT, SETTINGS_HEADLINE_SIZE);
	headline->setColour(WHITE);
	headline->setAlignment(CENTRED);
	headline->setUpBoundary(Vector2di((int)GFX::getXResolution(),SETTINGS_HEADLINE_SIZE));
	headline->setPosition(0,0);
	menuText = new Text();
	menuText->loadFont(GAME_FONT, SETTINGS_TEXT_SIZE);
	menuText->setColour(WHITE);
	menuText->setAlignment(LEFT_JUSTIFIED);
	menuText->setUpBoundary(Vector2di(GFX::getXResolution(), GFX::getYResolution()));
	entriesText = new Text();
	entriesText->loadFont(GAME_FONT, SETTINGS_TEXT_SIZE);
	entriesText->setColour(WHITE);
	entriesText->setAlignment(CENTRED);

	categoryItems.push_back("AUDIO");
	categoryItems.push_back("GAME");
	categoryItems.push_back("VIDEO");
	categoryItems.push_back("RETURN");

	audioItems.push_back("MUSIC VOL:");
	audioItems.push_back("SOUND VOL:");
	audioItems.push_back("RETURN");
	gameItems.push_back("HINT LINKS:");
	gameItems.push_back("CAMERA:");
	gameItems.push_back("SHOW FPS:");
	gameItems.push_back("LOG FPS:");
	gameItems.push_back("DEBUG:");
	gameItems.push_back("RETURN");
	videoItems.push_back("RENDER WRAP:");
	videoItems.push_back("PARTICLES:");
	videoItems.push_back("FULLSCREEN");
	videoItems.push_back("RETURN");

	patternStrings.push_back("OFF");
	patternStrings.push_back("ARROWS");
	patternStrings.push_back("SHADED");
	patternStrings.push_back("FULL");

	particleStrings.push_back("OFF");
	particleStrings.push_back("FEW");
	particleStrings.push_back("MANY");
	particleStrings.push_back("TOO MANY");

	cameraStrings.push_back("STATIC");
	cameraStrings.push_back("TRAILING");
	cameraStrings.push_back("AHEAD");

	arrows.loadFrames("images/general/arrows2.png", 4, 1);
	arrows.setTransparentColour(MAGENTA);
	arrows.setRotation(0);

	if (savePos.find(-1) != savePos.end())
		sel = savePos[-1];
}

Settings::~Settings()
{
	saveToFile();
	delete headline;
	delete menuText;
	delete entriesText;
}


///--- PUBLIC ------------------------------------------------------------------

int Settings::update()
{
	//
}

void Settings::render(SDL_Surface *screen)
{
	SDL_FillRect(screen, &headlineRect, 0);
	switch (category)
	{
	case 0:
		renderAudio(screen);
		break;
	case 1:
		renderGame(screen);
		break;
	case 2:
		renderVideo(screen);
		break;
	default:
		renderCategories(screen);
		break;
	}
}

void Settings::userInput(SimpleJoy *input)
{
	int temp = category;
	switch (category)
	{
	case 0:
		inputAudio(input);
		break;
	case 1:
		inputGame(input);
		break;
	case 2:
		inputVideo(input);
		break;
	default:
		inputCategories(input);
		break;
	}
	// Save current selection and restore saved selection
	if (temp != category)
	{
		savePos[temp] = sel;
		if (savePos.find(category) != savePos.end())
			sel = savePos[category];
		else
			sel = 0;
		if (usedMouse)
		{
			usedMouse = false;
			lastPos = Vector2di(0,0);
			// prevent flickering (updates rendered selection to current position of mouse)
			userInput(input);
		}
	}
}

void Settings::show()
{
	sel = 0;
	active = true;
}

void Settings::close()
{
	savePos[category] = sel;
	active = false;
	MUSIC_CACHE->stopSounds();
}

void Settings::loadFromFile()
{
	if (SAVEGAME->hasData("musicvolume"))
		setMusicVolume(StringUtility::stringToInt(SAVEGAME->getData("musicvolume")));
	if (SAVEGAME->hasData("soundvolume"))
		setSoundVolume(StringUtility::stringToInt(SAVEGAME->getData("soundvolume")));

	if (SAVEGAME->hasData("drawlinks"))
		setDrawLinks(StringUtility::stringToBool(SAVEGAME->getData("drawlinks")));
	else
		setDrawLinks(true);
	if (SAVEGAME->hasData("camerabehaviour"))
		setCameraBehaviour(StringUtility::stringToInt(SAVEGAME->getData("camerabehaviour")));
	else
		setCameraBehaviour(cbTrailing);
	if (SAVEGAME->hasData("drawfps"))
		setDrawFps(StringUtility::stringToBool(SAVEGAME->getData("drawfps")));
	else
		setDrawFps(false);
	if (SAVEGAME->hasData("writefps"))
		setWriteFps(StringUtility::stringToBool(SAVEGAME->getData("writefps")));
	else
		setWriteFps(false);
	if (SAVEGAME->hasData("debugcontrols"))
		setDebugControls(StringUtility::stringToBool(SAVEGAME->getData("debugcontrols")));
	else
		setDebugControls(false);

	if (SAVEGAME->hasData("drawpattern"))
		setDrawPattern(StringUtility::stringToInt(SAVEGAME->getData("drawpattern")));
	else
		setDrawPattern(dpShaded);
	if (SAVEGAME->hasData("particledensity"))
		setParticleDensity(StringUtility::stringToInt(SAVEGAME->getData("particledensity")));
	else
		setParticleDensity(pdMany);
	if (SAVEGAME->hasData("fullscreen"))
		setFullscreen(StringUtility::stringToBool(SAVEGAME->getData("fullscreen")));
	else
		setFullscreen(GFX::getFullscreen());
	if (SAVEGAME->hasData("particledensity"))
		setParticleDensity(StringUtility::stringToInt(SAVEGAME->getData("particledensity")));
	else
		setParticleDensity(pdMany);
	if (SAVEGAME->hasData("screenshotcompression"))
		setScreenshotCompression(StringUtility::stringToInt(SAVEGAME->getData("screenshotcompression")));
	else
		setScreenshotCompression(-1);
	if (SAVEGAME->hasData("videocompression"))
		setVideoCompression(StringUtility::stringToInt(SAVEGAME->getData("videocompression")));
	else
		setVideoCompression(0);
	if (SAVEGAME->hasData("videoframeskip"))
		setVideoFrameskip(StringUtility::stringToInt(SAVEGAME->getData("videoframeskip")));
	else
		setVideoFrameskip(0);
}

void Settings::saveToFile()
{
	SAVEGAME->writeData("musicvolume", StringUtility::intToString(getMusicVolume()), true);
	SAVEGAME->writeData("soundvolume", StringUtility::intToString(getSoundVolume()), true);
	SAVEGAME->writeData("drawlinks", StringUtility::boolToString(getDrawLinks()), true);
	SAVEGAME->writeData("camerabehaviour", StringUtility::intToString(getCameraBehaviour()), true);
	SAVEGAME->writeData("drawfps", StringUtility::boolToString(getDrawFps()), true);
	SAVEGAME->writeData("writefps", StringUtility::boolToString(getWriteFps()), true);
	SAVEGAME->writeData("debugcontrols", StringUtility::boolToString(getDebugControls()), true);
	SAVEGAME->writeData("drawpattern", StringUtility::intToString(getDrawPattern()), true);
	SAVEGAME->writeData("particledensity", StringUtility::intToString(getParticleDensity()), true);
	SAVEGAME->writeData("fullscreen", StringUtility::boolToString(getFullscreen()), true);
	SAVEGAME->writeData("screenshotcompression", StringUtility::intToString(getScreenshotCompression()), true);
	SAVEGAME->writeData("videocompression", StringUtility::intToString(getVideoCompression()), true);
	SAVEGAME->writeData("videoframeskip", StringUtility::intToString(getVideoFrameskip()), true);
}

/// --- getters and setters ----------------------------------------------------

int Settings::getMusicVolume()
{
	return MUSIC_CACHE->getMusicVolume();
}

void Settings::setMusicVolume(CRint newVol)
{
	MUSIC_CACHE->setMusicVolume(newVol);
}

int Settings::getSoundVolume()
{
	return MUSIC_CACHE->getSoundVolume();
}

void Settings::setSoundVolume(CRint newVol)
{
	MUSIC_CACHE->setSoundVolume(newVol);
}

int Settings::getMaxVolume()
{
	return MUSIC_CACHE->getMaxVolume();
}

bool Settings::getDrawLinks()
{
	return drawLinks;
}

void Settings::setDrawLinks(CRbool newLinks)
{
	drawLinks = newLinks;
}

int Settings::getCameraBehaviour()
{
	return cameraBehaviour;
}

void Settings::setCameraBehaviour(CRint newCb)
{
	if(newCb >= 0 && newCb < cbEOL)
		cameraBehaviour = newCb;
}

bool Settings::getDrawFps()
{
	return drawFps;
}

void Settings::setDrawFps(CRbool newFps)
{
	drawFps = newFps;
}

bool Settings::getWriteFps()
{
	return writeFps;
}

void Settings::setWriteFps(CRbool newFps)
{
	writeFps = newFps;
}

bool Settings::getDebugControls()
{
	return debugControls;
}

void Settings::setDebugControls(CRbool newDebug)
{
	debugControls = newDebug;
}

int Settings::getDrawPattern()
{
	return drawPattern;
}

void Settings::setDrawPattern(CRint newDp)
{
	if(newDp >= 0 && newDp < dpEOL)
		drawPattern = newDp;
}

int Settings::getParticleDensity()
{
	return particleDensity;
}

void Settings::setParticleDensity(CRint newPd)
{
	if(newPd >= 0 && newPd < pdEOL)
		particleDensity = newPd;
}

bool Settings::getFullscreen()
{
	return GFX::getFullscreen();
}

void Settings::setFullscreen(CRbool newFs)
{
	GFX::setFullscreen(newFs);
	GFX::resetScreen();
}

int Settings::getScreenshotCompression()
{
	return screenshotCompression;
}

void Settings::setScreenshotCompression(int newComp)
{
	if (newComp < IMG_COMPRESS_OFF || newComp == IMG_COMPRESS_DEFAULT)
		newComp = IMG_COMPRESS_DEFAULT;
	else if (newComp > IMG_COMPRESS_MAX)
		newComp = IMG_COMPRESS_MAX;
	screenshotCompression = newComp;
}

int Settings::getVideoCompression()
{
	return videoCompression;
}

void Settings::setVideoCompression(int newComp)
{
	if (newComp < IMG_COMPRESS_OFF || newComp == IMG_COMPRESS_DEFAULT)
		newComp = IMG_COMPRESS_DEFAULT;
	else if (newComp > IMG_COMPRESS_MAX)
		newComp = IMG_COMPRESS_MAX;
	videoCompression = newComp;
}

int Settings::getVideoFrameskip()
{
	return videoFrameskip;
}

void Settings::setVideoFrameskip(int newSkip)
{
	if (newSkip < 0)
		newSkip = 0;
	videoFrameskip = newSkip;
}


///--- PROTECTED ---------------------------------------------------------------

void Settings::renderCategories(SDL_Surface* surf)
{
	headline->print("SETTINGS");
	int pos = SETTINGS_MENU_OFFSET_Y;

	for (int I = 0; I < categoryItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = SETTINGS_RECT_HEIGHT;
		menuText->setPosition(SETTINGS_MENU_OFFSET_X, pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
		if (I == sel)
		{
			SDL_FillRect(surf, &rect, -1);
			menuText->setColour(BLACK);
		}
		else
		{
			SDL_FillRect(surf, &rect, 0);
			menuText->setColour(WHITE);
		}
		menuText->print(categoryItems[I]);

		if (I == categoryItems.size()-2)
			pos = SETTINGS_RETURN_Y_POS;
		else
			pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
	}
}

void Settings::renderAudio(SDL_Surface* surf)
{
	headline->print("AUDIO");
	int pos = SETTINGS_MENU_OFFSET_Y;

	// render text and selection
	for (int I = 0; I < audioItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = SETTINGS_RECT_HEIGHT;
		menuText->setPosition(SETTINGS_MENU_OFFSET_X, pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
		if (I == sel)
		{
			SDL_FillRect(surf, &rect, -1);
			menuText->setColour(BLACK);
		}
		else
		{
			SDL_FillRect(surf, &rect, 0);
			menuText->setColour(WHITE);
		}
		menuText->print(audioItems[I]);

		if (I < 2)
		{
			int value = 0;
			if (I == 0)
			{
				value = getMusicVolume();
			}
			else
			{
				value = getSoundVolume();
			}
			rect.w = (float)SETTINGS_VOLUME_SLIDER_SIZE * (float)value / (float)getMaxVolume();
			rect.x = (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X;
			if (I == sel)
				SDL_FillRect(surf, &rect, 0);
			else
				SDL_FillRect(surf, &rect, -1);
			if (I == sel && value > 0)
			{
				arrows.setCurrentFrame(3);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X - arrows.getWidth(),
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
			if (I == sel && value < getMaxVolume())
			{
				arrows.setCurrentFrame(1);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_MENU_OFFSET_X,
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
		}

		if (I == audioItems.size()-2)
			pos = SETTINGS_RETURN_Y_POS;
		else
			pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
	}
}

void Settings::renderGame(SDL_Surface* surf)
{
	headline->print("GAME");
	int pos = SETTINGS_MENU_OFFSET_Y;

	// render text and selection
	for (int I = 0; I < gameItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = SETTINGS_RECT_HEIGHT;
		menuText->setPosition(SETTINGS_MENU_OFFSET_X, pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
		if (I == sel)
		{
			SDL_FillRect(surf, &rect, -1);
			menuText->setColour(BLACK);
		}
		else
		{
			SDL_FillRect(surf, &rect, 0);
			menuText->setColour(WHITE);
		}
		menuText->print(gameItems[I]);

		if (I == 0 || ((I > 1) && (I < 5))) // Checkboxes
		{
			rect.w = SETTINGS_RECT_HEIGHT;
			rect.x = (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if (I == sel)
				SDL_FillRect(surf, &rect, 0);
			else
				SDL_FillRect(surf, &rect, -1);
			rect.w = SETTINGS_CHECK_HEIGHT;
			rect.h = SETTINGS_CHECK_HEIGHT;
			rect.x += (SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT) / 2;
			rect.y += (SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT) / 2;
			bool temp;
			if (I == 0)
				temp = getDrawLinks();
			else if (I ==2)
				temp = getDrawFps();
			else if (I == 3)
				temp = getWriteFps();
			else
				temp = getDebugControls();
			if (I == sel && !temp)
				SDL_FillRect(surf, &rect, -1);
			else if (I != sel && !temp)
				SDL_FillRect(surf, &rect, 0);
		}
		else if (I == 1) // Set of values
		{
			entriesText->setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_MENU_OFFSET_X,
									pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
			if (I == sel)
				entriesText->setColour(BLACK);
			else
				entriesText->setColour(WHITE);
			int value, maxValue;
			if(I == 1)
			{
				value = getCameraBehaviour();
				maxValue = cbEOL;
				entriesText->print(cameraStrings[cameraBehaviour]);
			}
			if (I == sel && value > 0)
			{
				arrows.setCurrentFrame(3);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.925f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
			if (I == sel && value < maxValue - 1)
			{
				arrows.setCurrentFrame(1);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.075f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
		}

		if (I == gameItems.size()-2)
			pos = SETTINGS_RETURN_Y_POS;
		else
			pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
	}
}

void Settings::renderVideo(SDL_Surface* surf)
{
	headline->print("VIDEO");
	int pos = SETTINGS_MENU_OFFSET_Y;

	// render text and selection
	for (int I = 0; I < videoItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = SETTINGS_RECT_HEIGHT;
		menuText->setPosition(SETTINGS_MENU_OFFSET_X, pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
		if (I == sel)
		{
			SDL_FillRect(surf, &rect, -1);
			menuText->setColour(BLACK);
		}
		else
		{
			SDL_FillRect(surf, &rect, 0);
			menuText->setColour(WHITE);
		}
		menuText->print(videoItems[I]);

		if (I < 2)
		{
			entriesText->setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_MENU_OFFSET_X,
									pos + SETTINGS_RECT_HEIGHT - SETTINGS_TEXT_SIZE);
			if (I == sel)
				entriesText->setColour(BLACK);
			else
				entriesText->setColour(WHITE);
			int value, maxValue;
			if(I == 0)
			{
				value = getDrawPattern();
				maxValue = dpEOL;
				entriesText->print(patternStrings[drawPattern]);
			}
			else
			{
				value = getParticleDensity();
				maxValue = pdEOL;
				entriesText->print(particleStrings[particleDensity]);
			}
			if (I == sel && value > 0)
			{
				arrows.setCurrentFrame(3);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.925f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
			if (I == sel && value < maxValue - 1)
			{
				arrows.setCurrentFrame(1);
				arrows.setPosition((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.075f - SETTINGS_MENU_OFFSET_X - arrows.getWidth() / 2,
								   pos + (SETTINGS_RECT_HEIGHT - arrows.getHeight()) / 2);
				arrows.render(surf);
			}
		}
		else if (I == 2)
		{
			rect.w = SETTINGS_RECT_HEIGHT;
			rect.x = (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if (I == sel)
				SDL_FillRect(surf, &rect, 0);
			else
				SDL_FillRect(surf, &rect, -1);
			rect.w = SETTINGS_CHECK_HEIGHT;
			rect.h = SETTINGS_CHECK_HEIGHT;
			rect.x += (SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT) / 2;
			rect.y += (SETTINGS_RECT_HEIGHT - SETTINGS_CHECK_HEIGHT) / 2;
			bool temp;
			if (I == 2)
				temp = getFullscreen();
			if (I == sel && !temp)
				SDL_FillRect(surf, &rect, -1);
			else if (I != sel && !temp)
				SDL_FillRect(surf, &rect, 0);
		}

		if (I == videoItems.size()-2)
			pos = SETTINGS_RETURN_Y_POS;
		else
			pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
	}
}

void Settings::inputCategories(SimpleJoy* input)
{
	Vector2di mousePos = input->getMouse();
	int oldSel = sel;
	int pos = SETTINGS_MENU_OFFSET_Y;
	mouseInBounds = false;
	if (mousePos != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < categoryItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y < pos + SETTINGS_RECT_HEIGHT)
			{
				sel = I;
				mouseInBounds = true;
			}
			if (I == categoryItems.size()-2)
				pos = SETTINGS_RETURN_Y_POS;
			else
				pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
		}
		lastPos = mousePos;
	}

	if(input->isUp() && sel > 0)
	{
		--sel;
		input->resetUp();
	}
	if(input->isDown() && sel < categoryItems.size() - 1)
	{
		++sel;
		input->resetDown();
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		if (sel < 3)
		{
			if (input->isLeftClick())
				usedMouse = true;
			input->resetKeys();
			category = sel;
		}
		else
		{
			input->resetKeys();
			close();
		}
	}
	if (isCancelKey(input) || input->isRightClick())
	{
		input->resetKeys();
		close();
	}
}

void Settings::inputAudio(SimpleJoy* input)
{
	Vector2di mousePos = input->getMouse();
	int oldSel = sel;
	int pos = SETTINGS_MENU_OFFSET_Y;
	mouseInBounds = false;
	if (mousePos != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < audioItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y < pos + SETTINGS_RECT_HEIGHT)
			{
				sel = I;
				mouseInBounds = true;
			}
			if (I == audioItems.size()-2)
				pos = SETTINGS_RETURN_Y_POS;
			else
				pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
		}
		lastPos = mousePos;
	}

	if(input->isUp() && sel > 0)
	{
		--sel;
		input->resetUp();
	}
	if(input->isDown() && sel < audioItems.size() - 1)
	{
		++sel;
		input->resetDown();
	}

	if(input->isLeft())
	{
		if (sel == 0)
		{
			setMusicVolume(max(getMusicVolume() - 2, 0));
		}
		else if (sel == 1)
		{
			setSoundVolume(max(getSoundVolume() - 2, 0));
		}
	}
	else if(input->isRight())
	{
		if (sel == 0)
		{
			setMusicVolume(min(getMusicVolume() + 2, getMaxVolume()));
		}
		else if (sel == 1)
		{
			setSoundVolume(min(getSoundVolume() + 2, getMaxVolume()));
		}
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		if (sel < 2)
		{
			if (input->isLeftClick())
			{
				float factor = (float)(mousePos.x - ((int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X)) / (float)SETTINGS_VOLUME_SLIDER_SIZE;
				float sliderFactor = arrows.getWidth() / (float)SETTINGS_VOLUME_SLIDER_SIZE;
				if(factor >= 0.0f && factor <= 1.0f)
				{
					if (sel == 0)
						setMusicVolume((float)getMaxVolume() * factor);
					else if (sel == 1)
						setSoundVolume((float)getMaxVolume() * factor);
				}
				else if (factor < 0.0f && factor > -sliderFactor)
				{
					if (sel == 0)
						setMusicVolume(max(getMusicVolume() - 2, 0));
					else if (sel == 1)
						setSoundVolume(max(getSoundVolume() - 2, 0));
				}
				else if (factor > 1.0f && factor < 1.0f + sliderFactor)
				{
					if (sel == 0)
						setMusicVolume(min(getMusicVolume() + 2, getMaxVolume()));
					else if (sel == 1)
						setSoundVolume(min(getSoundVolume() + 2, getMaxVolume()));
				}
			}
		}
		else if (sel == 2)
		{
			if (input->isLeftClick())
				usedMouse = true;
			input->resetMouseButtons();
			input->resetB();
			category = -1;
		}
	}
	if (input->isL())
	{
		if(sel == 0)
			setMusicVolume(0);
		else if(sel == 1)
			setSoundVolume(0);
	}
	else if (input->isR())
	{
		if(sel == 0)
			setMusicVolume(getMaxVolume());
		else if(sel == 1)
			setSoundVolume(getMaxVolume());
	}
	if (sel != oldSel)
	{
		if(sel == 1)
			MUSIC_CACHE->playSound("sounds/menu_testsound.wav", -1);
		else
			MUSIC_CACHE->stopSounds();
	}

	if (isCancelKey(input) || input->isRightClick())
	{
		if (input->isRightClick())
			usedMouse = true;
		input->resetKeys();
		category = -1;
	}
}

void Settings::inputGame(SimpleJoy* input)
{
	Vector2di mousePos = input->getMouse();
	int oldSel = sel;
	int pos = SETTINGS_MENU_OFFSET_Y;
	mouseInBounds = false;
	if (mousePos != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < gameItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y < pos + SETTINGS_RECT_HEIGHT)
			{
				sel = I;
				// TODO: Move bounds check here (currently after checking accept key)
				mouseInBounds = true;
			}
			if (I == gameItems.size()-2)
				pos = SETTINGS_RETURN_Y_POS;
			else
				pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
		}
		lastPos = mousePos;
	}

	if (input->isUp() && sel > 0)
	{
		--sel;
		input->resetUp();
	}
	if (input->isDown() && sel < gameItems.size() - 1)
	{
		++sel;
		input->resetDown();
	}

	if (input->isLeft())
	{
		switch (sel)
		{
		case 0:
			setDrawLinks(!getDrawLinks());
			break;
		case 1:
			setCameraBehaviour(getCameraBehaviour() - 1);
			break;
		case 2:
			setDrawFps(!getDrawFps());
			break;
		case 3:
			setWriteFps(!getWriteFps());
			break;
		case 4:
			setDebugControls(!getDebugControls());
			break;
		}
		input->resetLeft();
	}
	else if (input->isRight())
	{
		switch (sel)
		{
		case 0:
			setDrawLinks(!getDrawLinks());
			break;
		case 1:
			setCameraBehaviour(getCameraBehaviour() + 1);
			break;
		case 2:
			setDrawFps(!getDrawFps());
			break;
		case 3:
			setWriteFps(!getWriteFps());
			break;
		case 4:
			setDebugControls(!getDebugControls());
			break;
		}
		input->resetRight();
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		if (sel == 0 || ((sel > 1) && (sel < 5)))
		{
			int temp = (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if(isAcceptKey(input) || (mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT))
			{
				switch (sel)
				{
				case 0:
					setDrawLinks(!getDrawLinks());
					break;
				case 2:
					setDrawFps(!getDrawFps());
					break;
				case 3:
					setWriteFps(!getWriteFps());
					break;
				case 4:
					setDebugControls(!getDebugControls());
					break;
				}
			}
		}
		else if (sel == 1)
		{
			if(mousePos.x > (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.85f - SETTINGS_MENU_OFFSET_X)
				setCameraBehaviour(getCameraBehaviour() - 1);
			else if(mousePos.x > (int) GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.15f - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_MENU_OFFSET_X)
				setCameraBehaviour(getCameraBehaviour() + 1);
		}
		else if (sel == 5)
		{
			if (input->isLeftClick())
				usedMouse = true;
			category = -1;
		}
		input->resetMouseButtons();
		input->resetB();
	}

	if (isCancelKey(input) || input->isRightClick())
	{
		if (input->isRightClick())
			usedMouse = true;
		input->resetKeys();
		category = -1;
	}
}

void Settings::inputVideo(SimpleJoy* input)
{
	Vector2di mousePos = input->getMouse();
	int oldSel = sel;
	int pos = SETTINGS_MENU_OFFSET_Y;
	mouseInBounds = false;
	if (mousePos != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < videoItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y < pos + SETTINGS_RECT_HEIGHT)
			{
				sel = I;
				mouseInBounds = true;
			}
			if (I == videoItems.size()-2)
				pos = SETTINGS_RETURN_Y_POS;
			else
				pos += SETTINGS_RECT_HEIGHT + SETTINGS_MENU_SPACING;
		}
		lastPos = mousePos;
	}

	if (input->isUp() && sel > 0)
	{
		--sel;
		input->resetUp();
	}
	if (input->isDown() && sel < videoItems.size() - 1)
	{
		++sel;
		input->resetDown();
	}

	if(input->isLeft())
	{
		if (sel == 0)
			setDrawPattern(getDrawPattern() - 1);
		else if(sel == 1)
			setParticleDensity(getParticleDensity() - 1);
		else if (sel == 2)
			setFullscreen(!getFullscreen());
		input->resetLeft();
	}
	else if(input->isRight())
	{
		if (sel == 0)
			setDrawPattern(getDrawPattern() + 1);
		else if (sel == 1)
			setParticleDensity(getParticleDensity() + 1);
		else if (sel == 2)
			setFullscreen(!getFullscreen());
		input->resetRight();
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		if (sel == 0)
		{
			if(mousePos.x > (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.85f - SETTINGS_MENU_OFFSET_X)
				setDrawPattern(getDrawPattern() - 1);
			else if(mousePos.x > (int) GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.15f - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_MENU_OFFSET_X)
				setDrawPattern(getDrawPattern() + 1);
		}
		else if (sel == 1)
		{
			if(mousePos.x > (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.85f - SETTINGS_MENU_OFFSET_X)
				setParticleDensity(getParticleDensity() - 1);
			else if(mousePos.x > (int) GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE * 0.15f - SETTINGS_MENU_OFFSET_X &&
					mousePos.x < (int)GFX::getXResolution() - SETTINGS_MENU_OFFSET_X)
				setParticleDensity(getParticleDensity() + 1);
		}
		else if (sel == 2)
		{
			int temp = (int)GFX::getXResolution() - SETTINGS_VOLUME_SLIDER_SIZE / 2 - SETTINGS_RECT_HEIGHT / 2 - SETTINGS_MENU_OFFSET_X;
			if(isAcceptKey(input) || (mousePos.x >= temp && mousePos.x < temp + SETTINGS_RECT_HEIGHT))
				setFullscreen(!getFullscreen());
		}
		else if (sel == 3)
		{
			category = -1;
			if (input->isLeftClick())
				usedMouse = true;
		}
		input->resetMouseButtons();
		input->resetB();
	}

	if (isCancelKey(input) || input->isRightClick())
	{
		if (input->isRightClick())
			usedMouse = true;
		input->resetKeys();
		category = -1;
	}
}


///--- PRIVATE -----------------------------------------------------------------
