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

#include "StateChapterTrial.h"

#include "Vector2di.h"
#include "GreySurfaceCache.h"
#include "gameDefines.h"
#include "userStates.h"
#include "MyGame.h"
#include "Savegame.h"
#include "globalControls.h"

#ifdef _MEOW
#define TIME_TRIAL_OFFSET_X 10
#define TIME_TRIAL_OFFSET_Y 5
#define TIME_TRIAL_SPACING_Y 0
#define TIME_TRIAL_MENU_OFFSET_Y 30

#define NAME_TEXT_SIZE 24
#define NAME_RECT_HEIGHT 18
#else
#define TIME_TRIAL_OFFSET_X 20
#define TIME_TRIAL_OFFSET_Y 10
#define TIME_TRIAL_SPACING_Y 10
#define TIME_TRIAL_MENU_OFFSET_Y 80

#define NAME_TEXT_SIZE 48
#define NAME_RECT_HEIGHT 35
#endif

#define END_TIMER_ANIMATION_STEP 60

StateChapterTrial::StateChapterTrial()
{
	timeTrialText.loadFont(GAME_FONT,int(NAME_TEXT_SIZE * 1.5));
	timeTrialText.setColour(WHITE);
	timeTrialText.setAlignment(RIGHT_JUSTIFIED);
	timeTrialText.setUpBoundary(Vector2di(GFX::getXResolution()-TIME_TRIAL_OFFSET_X,GFX::getYResolution()-TIME_TRIAL_OFFSET_Y));
	timeTrialText.setPosition(TIME_TRIAL_OFFSET_X,TIME_TRIAL_OFFSET_Y);

	menuText.loadFont(GAME_FONT,NAME_TEXT_SIZE);
	menuText.setColour(WHITE);
	menuText.setAlignment(LEFT_JUSTIFIED);
	menuText.setUpBoundary(Vector2di(GFX::getXResolution(),GFX::getYResolution()));
	menuText.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
	menuRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
	menuRect.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f);
	menuRect.setColour(BLACK);

	bg.loadFrames(SURFACE_CACHE->loadSurface("images/menu/error_bg_800_480.png"),1,1,0,0);
	bg.disableTransparentColour();
	bg.setPosition(0,0);

	pauseItems.push_back("RESTART");
	pauseItems.push_back("EXIT");

	int temp = SAVEGAME->getChapterStats(ENGINE->currentChapter->filename).bestSpeedrunTime;
	if (temp >= 0)
		newRecord = (ENGINE->chapterTrialTimer < temp);
	else
		newRecord = true;
	if (newRecord)
	{
		Savegame::ChapterStats temp = {0,ENGINE->chapterTrialTimer};
		SAVEGAME->setChapterStats(ENGINE->currentChapter->filename,temp);
	}

	timeDisplay = 0;
	menuSelection = 0;
}

StateChapterTrial::~StateChapterTrial()
{
	pauseItems.clear();
}

void StateChapterTrial::userInput()
{
	if (timeDisplay < ENGINE->chapterTrialTimer)
		return;

	if (input->isUp() && menuSelection > 0)
	{
		--menuSelection;
		input->resetUp();
	}
	if (input->isDown() && menuSelection < pauseItems.size()-1)
	{
		++menuSelection;
		input->resetDown();
	}
	if (isAcceptKey(input) || input->isLeftClick())
	{
		switch (menuSelection)
		{
		case 0: // Restart
		{
			ENGINE->startChapterTrial();
			string filename = ENGINE->currentChapter->filename;
			ENGINE->playChapter(filename,0);
			break;
		}
		case 1: // Exit
			setNextState(STATE_TITLE);
			break;
		default:
			break;
		}
	}
}

void StateChapterTrial::update()
{
	bg.update();

	if (timeDisplay < ENGINE->chapterTrialTimer)
		timeDisplay += min(ENGINE->chapterTrialTimer - timeDisplay, ENGINE->chapterTrialTimer / END_TIMER_ANIMATION_STEP);
	else
	{
		if (newRecord)
			timeTrialText.setColour(ORANGE);
	}
}

void StateChapterTrial::render()
{
	bg.render();

	timeTrialText.setAlignment(CENTRED);
	timeTrialText.setPosition(TIME_TRIAL_OFFSET_X,TIME_TRIAL_OFFSET_Y);
	if (newRecord && (timeDisplay == ENGINE->chapterTrialTimer))
		timeTrialText.print("NEW RECORD!");
	else
		timeTrialText.print("FINISHED!");

	timeTrialText.setAlignment(LEFT_JUSTIFIED);
	timeTrialText.setPosition(TIME_TRIAL_OFFSET_X,TIME_TRIAL_OFFSET_Y + TIME_TRIAL_SPACING_Y + NAME_TEXT_SIZE * 1.5);
	timeTrialText.print("TIME: ");
	timeTrialText.setAlignment(RIGHT_JUSTIFIED);
	timeTrialText.print(MyGame::ticksToTimeString(timeDisplay));

	if (timeDisplay == ENGINE->chapterTrialTimer)
	{
		timeTrialText.setAlignment(LEFT_JUSTIFIED);
		timeTrialText.setPosition(TIME_TRIAL_OFFSET_X,TIME_TRIAL_OFFSET_Y + TIME_TRIAL_SPACING_Y * 2 + NAME_TEXT_SIZE * 3);
		timeTrialText.print("BEST: ");
		timeTrialText.setAlignment(RIGHT_JUSTIFIED);
		timeTrialText.print(MyGame::ticksToTimeString(SAVEGAME->getChapterStats(ENGINE->currentChapter->filename).bestSpeedrunTime));

		// render text and selection
		Vector2di mousePos = input->getMouse();
		int pos = (GFX::getYResolution() - TIME_TRIAL_SPACING_Y * (pauseItems.size()-1)) / 2 + TIME_TRIAL_MENU_OFFSET_Y;
		if (mousePos != lastPos)
			lastPos = mousePos;
		else
			mousePos = Vector2di(-1,-1);
		for (int I = 0; I < pauseItems.size(); ++I)
		{
			// NOTE: do mouse selection handling here, so I don't have to copy code
			if (mousePos.y >= pos && mousePos.y <= pos + NAME_RECT_HEIGHT)
			{
				menuSelection = I;
			}

			menuRect.setPosition(0,pos);
			menuText.setPosition(TIME_TRIAL_OFFSET_X,pos + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
			if (I == menuSelection)
			{
				menuRect.setColour(WHITE);
				menuText.setColour(BLACK);
			}
			else
			{
				menuRect.setColour(BLACK);
				menuText.setColour(WHITE);
			}
			menuRect.render();
			menuText.print(pauseItems[I]);

			pos += NAME_RECT_HEIGHT + TIME_TRIAL_SPACING_Y;
		}
	}
}
