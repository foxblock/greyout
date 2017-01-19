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

#include "StateLevelSelect.h"

#include "GFX.h"
#include <limits.h>

#include "MyGame.h"
#include "Level.h"
#include "userStates.h"
#include "GreySurfaceCache.h"
#include "LevelLoader.h"
#include "MusicCache.h"
#include "gameDefines.h"
#include "Savegame.h"
#include "effects/Hollywood.h"
#include "globalControls.h"

#define PREVIEW_COUNT_X 4
#define PREVIEW_COUNT_Y 3
#ifdef _MEOW
#define SPACING_X 10
#define OFFSET_Y 18
#define TITLE_FONT_SIZE 24
#define IMAGE_FONT_SIZE 12
#define CURSOR_BORDER 2
#define NAME_SPACING 2
#else
#define SPACING_X 20
#define OFFSET_Y 35
#define TITLE_FONT_SIZE 48
#define IMAGE_FONT_SIZE 24
#define CURSOR_BORDER 5
#define NAME_SPACING 4
#endif


#define INTERMEDIATE_MENU_ITEM_COUNT 2
#define INTERMEDIATE_LEVEL_MENU_ITEM_COUNT 2
#define INTERMEDIATE_MENU_SPACING 20

struct VecComp
{
	bool operator() (const Vector2di& lhs, const Vector2di& rhs) const
	{
		return (lhs.x + lhs.y * PREVIEW_COUNT_X < rhs.x + rhs.y * PREVIEW_COUNT_X);
	}
};

map<string, pair<string, SDL_Surface*> > StateLevelSelect::previewCache;
Vector2di StateLevelSelect::saveChapterSel = Vector2di(0,0);
map<Vector2di, Vector2di, VecComp> StateLevelSelect::saveLevelSel;

struct PreviewData
{
	string filename;
	string name;
	SDL_Surface* surface;
	bool hasBeenLoaded;
};
StateLevelSelect::StateLevelSelect()
{
	// set up the grid
	spacing.x = SPACING_X;
	size.x = (GFX::getXResolution() - spacing.x * (PREVIEW_COUNT_X + 1)) / PREVIEW_COUNT_X;
	size.y = (float)size.x * ((float)GFX::getYResolution() / (float)GFX::getXResolution());
	spacing.y = (GFX::getYResolution() - OFFSET_Y - size.y * PREVIEW_COUNT_Y) / (PREVIEW_COUNT_Y + 1);
	gridOffsetLast = 0;
	lastDraw = 0;
	firstDraw = true;
	intermediateSelection = 0;
	lastPos = Vector2di(-1, -1);
	mousePos = Vector2di(0, 0);
	mouseInBounds = false;

	// graphic stuff
	// TODO: Use 320x240 bg here (and make that)
	bg.loadFrames(SURFACE_CACHE->loadSurface("images/menu/error_bg_800_480.png"),1,1,0,0);
	bg.disableTransparentColour();
	bg.setPosition(0,0);
	loading.loadFrames(SURFACE_CACHE->loadSurface("images/general/loading.png",true),1,1,0,0);
	loading.disableTransparentColour();
	loading.setScaleX((float)size.x / loading.getWidth());
	loading.setScaleY((float)size.y / loading.getHeight());
	error.loadFrames(SURFACE_CACHE->loadSurface("images/general/error.png",true),1,1,0,0);
	error.disableTransparentColour();
	error.setScaleX((float)size.x / error.getWidth());
	error.setScaleY((float)size.y / error.getHeight());
	locked.loadFrames(SURFACE_CACHE->loadSurface("images/general/locked.png",true),1,1,0,0);
	locked.disableTransparentColour();
	locked.setScaleX((float)size.x / locked.getWidth());
	locked.setScaleY((float)size.y / locked.getHeight());
	arrows.loadFrames(SURFACE_CACHE->loadSurface("images/general/arrows.png"),2,1,0,0);
	arrows.setTransparentColour(MAGENTA);
	cursor.setDimensions(size.x + 2*CURSOR_BORDER,size.y + 2*CURSOR_BORDER);
	cursor.setColour(ORANGE);
	previewDraw = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
	SDL_FillRect(previewDraw, NULL, SDL_MapRGB(previewDraw->format,255,0,255));
	SDL_SetColorKey(previewDraw, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(previewDraw->format,255,0,255));
	imageText.loadFont(GAME_FONT,IMAGE_FONT_SIZE);
	imageText.setUpBoundary(size);
	imageText.setWrapping(true);
	imageText.setColour(WHITE);
	titleText.loadFont(GAME_FONT,TITLE_FONT_SIZE);
	titleText.setAlignment(LEFT_JUSTIFIED);
	titleText.setColour(WHITE);
	titleText.setPosition(0,OFFSET_Y - TITLE_FONT_SIZE);
	titleText.setUpBoundary(Vector2di(GFX::getXResolution(),OFFSET_Y));
	nameText.loadFont(GAME_FONT,IMAGE_FONT_SIZE);
	nameText.setColour(BLACK);
	nameText.setWrapping(false);
	#ifdef _DEBUG
	debugText.loadFont(DEBUG_FONT,24);
	debugText.setColour(GREEN);
	debugText.setPosition(0,TITLE_FONT_SIZE);
	debugText.setAlignment(LEFT_JUSTIFIED);
	#endif // _DEBUG
	menu.setDimensions(GFX::getXResolution(),OFFSET_Y);
	menu.setColour(BLACK);
	menu.setPosition(0,0);
	overlay.setDimensions(GFX::getXResolution(),GFX::getYResolution());
	overlay.setColour(BLACK);
	overlay.setPosition(0,0);
	overlay.setAlpha(100);

	// thread stuff
	levelLock = SDL_CreateMutex();
	levelThread = NULL;
	abortLevelLoading = false;
	chapterLock = SDL_CreateMutex();
	chapterThread = NULL;
	abortChapterLoading = false;

	levelLister.addFilter("txt");
	dirLister.addFilter("DIR");
	state = lsChapter;
	setChapterDirectory(DEFAULT_CHAPTER_FOLDER);
	exChapter = NULL;

	fadeTimer = -1;
	returnToMenu = false;

	selection = saveChapterSel;
	checkSelection(chapterPreviews, selection);
	gridOffset = selection.y;
	checkGridOffset(chapterPreviews, gridOffset);
}

StateLevelSelect::~StateLevelSelect()
{
	clear();
	SDL_FreeSurface(previewDraw);
	SDL_DestroyMutex(levelLock);
	SDL_DestroyMutex(chapterLock);
}

/// ---public---

void StateLevelSelect::init()
{
	MUSIC_CACHE->playMusic("music/title_menu.ogg");
	EFFECTS->fadeIn(30);
}

void StateLevelSelect::clear()
{
	clearLevelListing();
	clearChapterListing();
}

void StateLevelSelect::clearLevelListing()
{
	abortLevelLoading = true;
	int* status = NULL;
	if (levelThread)
	{
		SDL_WaitThread(levelThread,status);
		levelThread = NULL;
	}

	levelPreviews.clear();
	levelLister.clearListing();
	delete exChapter;
	exChapter = NULL;
}

void StateLevelSelect::clearChapterListing()
{
	abortChapterLoading = true;
	int *status = NULL;
	if (chapterThread)
	{
		SDL_WaitThread(chapterThread, status);
		chapterThread = NULL;
	}

	chapterPreviews.clear();
	dirLister.clearListing();
}

void StateLevelSelect::userInput()
{
	if ( fadeTimer >= 0 )
		return;
	mousePos = input->getMouse();
	if (state != lsIntermediate && state != lsIntermediateLevel) // grid navigation
	{
		Vector2di oldSel = selection;
		if (input->isLeft())
		{
			--selection.x;
		}
		else if (input->isRight())
		{
			++selection.x;
		}
		if (input->isUp())
		{
			--selection.y;
		}
		if (input->isDown())
		{
			++selection.y;
		}
		if (input->getMouseWheelDelta() != 0)
		{
			gridOffset -= input->getMouseWheelDelta();
			lastPos = Vector2di(-1,-1);
			if (state == lsLevel)
			{
				checkGridOffset( levelPreviews, gridOffset );
			}
			else if (state == lsChapter)
			{
				checkGridOffset( chapterPreviews, gridOffset );
			}
		}

		if (mousePos != lastPos)
		{
			mouseInBounds = false;
			Vector2di newPos(-1,-1);
			for (int I = 0; I < PREVIEW_COUNT_X; ++I)
			{
				if (mousePos.x >= spacing.x * (I+1) + size.x * I && mousePos.x < spacing.x * (I+1) + size.x * (I+1))
				{
					newPos.x = I;
					break;
				}
			}
			for (int I = 0; I < PREVIEW_COUNT_Y; ++I)
			{
				if (mousePos.y >= OFFSET_Y + spacing.y * (I+1) + size.y * I && mousePos.y < OFFSET_Y + spacing.y * (I+1) + size.y * (I+1))
				{
					newPos.y = I;
					break;
				}
			}
			if (newPos.y != -1)
				newPos.y += gridOffset;
			if (newPos.x >= 0 && newPos.y >= 0)
			{
				selection = newPos;
				mouseInBounds = true;
			}

			lastPos = mousePos;
		}
		if (input->isLeftClick())
		{
			if (mousePos.y > OFFSET_Y + spacing.y * PREVIEW_COUNT_Y + size.y * PREVIEW_COUNT_Y)
			{
				++gridOffset;
				if (state == lsLevel)
				{
					checkGridOffset( levelPreviews, gridOffset );
				}
				else if (state == lsChapter)
				{
					checkGridOffset( chapterPreviews, gridOffset );
				}
			}
			else if (mousePos.y < OFFSET_Y + spacing.y)
			{
				--gridOffset;
				if (state == lsLevel)
				{
					checkGridOffset( levelPreviews, gridOffset );
				}
				else if (state == lsChapter)
				{
					checkGridOffset( chapterPreviews, gridOffset );
				}
			}
		}
		if (state == lsLevel)
		{
			checkSelection( levelPreviews, selection );
		}
		else if (state == lsChapter)
		{
			checkSelection( chapterPreviews, selection );
		}
		if (selection != oldSel)
			MUSIC_CACHE->playSound("sounds/level_select.wav");
	}
	else // intermediate menu navigation
	{
		int menuItemCount = 0;
		if (state == lsIntermediate)
		{
			menuItemCount = INTERMEDIATE_MENU_ITEM_COUNT;
			if (StringUtility::stringToBool(SAVEGAME->getData("speedrun")))
				++menuItemCount;
		}
		else
			menuItemCount = INTERMEDIATE_LEVEL_MENU_ITEM_COUNT;
		if (input->isUp())
		{
			if (intermediateSelection > 0)
			{
				--intermediateSelection;
			}
		}
		else if (input->isDown())
		{
			if (intermediateSelection < menuItemCount-1)
			{
				++intermediateSelection;
			}
		}
		if (mousePos != lastPos)
		{
			int pos = (GFX::getYResolution() - INTERMEDIATE_MENU_SPACING * (menuItemCount-1) - OFFSET_Y * menuItemCount) / 2;
			int temp = -1;
			for (int I = 0; I < menuItemCount; ++I)
			{
				if (mousePos.y >= pos && mousePos.y <= pos + OFFSET_Y)
				{
					temp = I;
					break;
				}
				pos += OFFSET_Y + INTERMEDIATE_MENU_SPACING;
			}
			if ( temp != -1 )
			{
				intermediateSelection = temp;
				mouseInBounds = true;
			}
			else
				mouseInBounds = false;

			lastPos = mousePos;
		}

		if ( isAcceptKey(input) || ( input->isLeftClick() && mouseInBounds ) )
		{
			doSelection();
		}
		else if (isCancelKey(input) || input->isRightClick())
		{
			if (state == lsIntermediate)
				switchState(lsChapter);
			else
				switchState(lsLevel);
		}
		input->resetKeys();
	}

	if (state == lsLevel)
	{
		if (isCancelKey(input) || input->isRightClick()) // return to chapter selection
		{
			saveLevelSel[saveChapterSel] = selection;
			abortLevelLoading = true;
			switchState(lsChapter);
			input->resetKeys();
			return;
		}
		if ( isAcceptKey(input) || ( input->isLeftClick() && mouseInBounds ) )
		{
			saveLevelSel[saveChapterSel] = selection;
			doSelection();
		}
	}
	else if (state == lsChapter)
	{
		if (isCancelKey(input) || input->isRightClick()) // return to menu
		{
			saveChapterSel = selection;
			abortLevelLoading = true;
			abortChapterLoading = true;
			input->resetKeys();
			MUSIC_CACHE->playSound("sounds/menu_back.wav");
			fadeOut();
			returnToMenu = true;
			return;
		}
		if (isAcceptKey(input) || ( input->isLeftClick() && mouseInBounds ) )
		{
			saveChapterSel = selection;
			doSelection();
		}
	}

	input->resetKeys();
}

void StateLevelSelect::update()
{
	if ( fadeTimer > 0 )
		--fadeTimer;
	else if ( fadeTimer == 0 )
		doSelection();
	if (state != lsIntermediate && state != lsIntermediateLevel)
	{
		// check selection and adjust grid offset
		if (selection.y < gridOffset)
			--gridOffset;
		else if (selection.y >= gridOffset + PREVIEW_COUNT_Y)
			++gridOffset;

		if (gridOffset != gridOffsetLast)
		{
			// redraw previews on scroll
			gridOffsetLast = gridOffset;
			firstDraw = true;
			lastDraw = 0;
			SDL_FillRect(previewDraw, NULL, SDL_MapRGB(previewDraw->format,255,0,255));
		}

		cursor.setPosition(selection.x * size.x + (selection.x + 1) * spacing.x - CURSOR_BORDER,
						   OFFSET_Y + (selection.y - gridOffset) * size.y + (selection.y - gridOffset + 1) * spacing.y - CURSOR_BORDER);

	}
	EFFECTS->update();
}

void StateLevelSelect::render()
{
	if (state != lsIntermediate && state != lsIntermediateLevel)
	{
		GFX::clearScreen();

		bg.render();
		cursor.render();
		menu.render();

		vector<PreviewData>* activeData;

		switch (state)
		{
		case lsChapter:
			titleText.print("CHAPTERS");
			activeData = &chapterPreviews;
			break;
		case lsLevel:
			titleText.print("LEVELS");
			activeData = &levelPreviews;
			break;
		default:
			break;
		}

		// render previews to temp surface (checks for newly generated previews and renders them only)
		renderPreviews(*activeData,previewDraw,lastDraw - gridOffset * PREVIEW_COUNT_X);
		// blit temp surface
		SDL_BlitSurface(previewDraw,NULL,GFX::getVideoSurface(),NULL);

		renderName(GFX::getVideoSurface(), activeData->at(selection.y * PREVIEW_COUNT_X + selection.x).name);

		// show arrows
		if (gridOffset > 0)
		{
			arrows.setCurrentFrame(0);
			arrows.setPosition((GFX::getXResolution() - arrows.getWidth()) / 2, OFFSET_Y + 5);
			arrows.render();
		}
		if (activeData->size() - gridOffset * PREVIEW_COUNT_X > PREVIEW_COUNT_X * PREVIEW_COUNT_Y)
		{
			arrows.setCurrentFrame(1);
			arrows.setPosition((GFX::getXResolution() - arrows.getWidth()) / 2, GFX::getYResolution() - arrows.getHeight() - 5);
			arrows.render();
		}

		#ifdef _DEBUG
		debugText.print(StringUtility::vecToString(selection) + ", " +
				StringUtility::intToString(gridOffset) + " (" + StringUtility::intToString(gridOffsetLast) + ")\n" +
				StringUtility::intToString(lastDraw) + ", " + StringUtility::boolToString(firstDraw, true) + "\n" +
				StringUtility::vecToString(mousePos) + " (" + StringUtility::vecToString(lastPos) + "), " +
				StringUtility::boolToString(mouseInBounds, true));
		#endif // _DEBUG
	}
	else
	{
		// previewDraw now holds the background image
		SDL_BlitSurface(previewDraw,NULL,GFX::getVideoSurface(),NULL);

		vector<string> items;
		if (state == lsIntermediate)
		{
			items.push_back("PLAY");
			items.push_back("EXPLORE");
			if (StringUtility::stringToBool(SAVEGAME->getData("speedrun")))
				items.push_back("SPEEDRUN");
		}
		else
		{
			items.push_back("PLAY");
			items.push_back("SPEEDRUN");
		}

		// OFFSET_Y is also the height of the rectangle used
		int pos = (GFX::getYResolution() - INTERMEDIATE_MENU_SPACING * (items.size()-1) - OFFSET_Y * items.size()) / 2;

		for (int I = 0; I < items.size(); ++I)
		{
			menu.setPosition(0,pos);
			titleText.setPosition(0,pos + OFFSET_Y - TITLE_FONT_SIZE);
			if (I == intermediateSelection)
			{
				menu.setColour(WHITE);
				titleText.setColour(BLACK);
			}
			else
			{
				menu.setColour(BLACK);
				titleText.setColour(WHITE);
			}
			menu.render();
			titleText.print(items[I]);
			pos += OFFSET_Y + INTERMEDIATE_MENU_SPACING;
		}
	}
	EFFECTS->render();
}

void StateLevelSelect::renderPreviews(const vector<PreviewData>& data, SDL_Surface* const target, CRint addOffset)
{
	int numOffset = gridOffset * PREVIEW_COUNT_X;
	bool reachedLoaded = false; // set when the first not loaded surface is found

	int maxUnlocked = 0;
	if (state == lsLevel && exChapter)
	{
		maxUnlocked = exChapter->getProgress();
	}

	int I = 0;
	for (I = numOffset + max(addOffset,0); (I < (numOffset + PREVIEW_COUNT_X * PREVIEW_COUNT_Y)) && (I < data.size()); ++I)
	{
		// calculate position of object to render
		Vector2di pos;
		pos.x = (I % PREVIEW_COUNT_X) * size.x + (I % PREVIEW_COUNT_X + 1) * spacing.x;
		pos.y = OFFSET_Y + ((I - numOffset) / PREVIEW_COUNT_X) * size.y + ((I - numOffset) / PREVIEW_COUNT_X + 1) * spacing.y;

		SDL_mutexP(levelLock);
		if (data[I].hasBeenLoaded)
		{
			if (data[I].surface) // render preview
			{
				SDL_Rect rect = {pos.x,pos.y,0,0};
				SDL_BlitSurface(data[I].surface,NULL,target,&rect);
				SDL_mutexV(levelLock);
			}
			else // render error or locked image
			{
				SDL_mutexV(levelLock);
				if (state == lsLevel && exChapter && (I > maxUnlocked)) // locked
				{
					// actually we need to add an offset here as we need to preserve the top-left
					// corner (Penjin scales centred), but I rather scale the image files properly
					// than add code to compensate for my stupidity in that regard
					locked.setPosition(pos);
					locked.render(target);
				}
				else
				{
					error.setPosition(pos);
					error.render(target);
				}
			}
			if (not reachedLoaded)
				lastDraw = I;
		}
		else // render loading image
		{
			SDL_mutexV(levelLock);
			if (not reachedLoaded)
			{
				lastDraw = I;
				reachedLoaded = true;
				// will only render loading pics from here on, but we need to keep the lastDraw variable
			}
			if (firstDraw) // only draw loading images once
			{
				if (state == lsLevel && exChapter && (I > maxUnlocked)) // locked
				{
					locked.setPosition(pos);
					locked.render(target);
				}
				else
				{
					loading.setPosition(pos);
					loading.render(target);
				}
			}
		}
	}
	if (not reachedLoaded)
		lastDraw = I;
	firstDraw = false;
}

void StateLevelSelect::renderName(SDL_Surface *target, CRstring name)
{
	if (name[0] == 0)
		return;
	SDL_Surface *dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,1,1,
			GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
	// reset text object
	nameText.print(dummy, name); // print once to let Penjin calculate the size of the text on screen
	SDL_Rect rect = { cursor.getPosition().x,
			cursor.getPosition().y,
			nameText.getWidth() + NAME_SPACING * 4,
			nameText.getHeight() + NAME_SPACING * 2 };

	if (selection.y == gridOffset + PREVIEW_COUNT_Y -1) // last row on screen
		rect.y -= nameText.getHeight() + NAME_SPACING*2;
	else
		rect.y += cursor.getDimensions().y;
	// last two colums -> check whether text goes off screen and make right-aligned
	if (selection.x >= PREVIEW_COUNT_X / 2.0f && rect.x + rect.w > GFX::getXResolution())
	{
		rect.x += cursor.getDimensions().x - rect.w;
	}
	nameText.setPosition(rect.x + NAME_SPACING * 2, rect.y + NAME_SPACING); // seems to be centred well with double horizontal spacing

	SDL_FillRect(target,&rect,SDL_MapRGB(target->format,255,128,0));
	nameText.print(name);
	return;
}


void StateLevelSelect::setLevelDirectory(CRstring dir)
{
	clearLevelListing();
	levelLister.setPath(dir);
	vector<string> files;
	files = levelLister.getListing();
	files.erase(files.begin()); // delete first element which is the current folder

	//#ifdef _DEBUG
	PreviewData bench = {BENCHMARK_LEVEL,"[BENCHMARK]",NULL,false};
	levelPreviews.push_back(bench);
	//#endif

	// initialize map
	for (vector<string>::const_iterator file = files.begin(); file < files.end(); ++file)
	{
		PreviewData temp = {dir + (*file),"",NULL,false};
		levelPreviews.push_back(temp);
	}
	printf("%i files found in level directory\n",levelPreviews.size());

	abortLevelLoading = false;
	levelThread = SDL_CreateThread(StateLevelSelect::loadLevelPreviews, this);

	files.clear();
}

void StateLevelSelect::setChapterDirectory(CRstring dir)
{
	clearChapterListing();
	dirLister.setPath(dir);
	vector<string> files;
	files = dirLister.getListing();

	// erase dir, . and ..
	for (vector<string>::iterator iter = files.begin(); iter != files.end();)
	{
		if ((*iter) == "." || (*iter) == ".." || (*iter) == dir)
			iter = files.erase(iter);
		else
			++iter;
	}

	// add single level folder
	SDL_Surface* img = SURFACE_CACHE->loadSurface("images/general/levelfolder.png");
	if (img->w != size.x || img->h != size.y)
	{
		img = zoomSurface(img,(float)size.x / (float)img->w , (float)size.y / (float)img->h, SMOOTHING_OFF);
	}
	else
	{
		// remove image from cache to prevent double freeing
		SURFACE_CACHE->removeSurface("images/general/levelfolder.png",false);
	}
	PreviewData temp = {DEFAULT_LEVEL_FOLDER,"[SINGLE LEVELS]",img,true};
	chapterPreviews.push_back(temp);

	// set paths to info.txt files and initialize map
	for (vector<string>::iterator item = files.begin(); item < files.end(); ++item)
	{
		(*item) = dir + (*item) + "/" + DEFAULT_CHAPTER_INFO_FILE;
		PreviewData temp2 = {(*item),"",NULL,false};
		chapterPreviews.push_back(temp2);
	}
	printf("%i chapters found\n",chapterPreviews.size()-1);

	abortChapterLoading = false;
	chapterThread = SDL_CreateThread(StateLevelSelect::loadChapterPreviews,this);
}

void StateLevelSelect::exploreChapter(CRstring filename)
{
	clearLevelListing();
	exChapter = new Chapter;

	if (not exChapter->loadFromFile(filename))
	{
		ENGINE->stateParameter = "ERROR loading chapter!";
		setNextState(STATE_ERROR);
	}

	for (vector<string>::const_iterator file = exChapter->levels.begin(); file != exChapter->levels.end(); ++file)
	{
		PreviewData temp = {exChapter->path + (*file),"",NULL,false};
		levelPreviews.push_back(temp);
	}
	printf("Chapter has %i levels\n",levelPreviews.size());

	abortLevelLoading = false;
	levelThread = SDL_CreateThread(StateLevelSelect::loadLevelPreviews,this);
}

/// ---protected---

void StateLevelSelect::switchState(const LevelSelectState& toState)
{
	if (toState != lsIntermediate && toState != lsIntermediateLevel)
	{
		titleText.setColour(WHITE);
		menu.setColour(BLACK);
		menu.setPosition(0,0);
		lastDraw = 0;
		firstDraw = true;
		SDL_FillRect(previewDraw, NULL, SDL_MapRGB(previewDraw->format,255,0,255));
	}

	if (toState == lsChapter)
	{
		if (state == lsLevel)
		{
			selection = saveChapterSel;
			checkSelection(chapterPreviews, selection);
			gridOffset = selection.y;
			checkGridOffset(chapterPreviews, gridOffset);
		}
		titleText.setPosition(0,OFFSET_Y - TITLE_FONT_SIZE);
		titleText.setAlignment(LEFT_JUSTIFIED);
	}
	else if (toState == lsLevel)
	{
		if (state == lsIntermediate || state == lsChapter)
		{
			if (saveLevelSel.find(selection) != saveLevelSel.end())
				selection = saveLevelSel[selection];
			else
				selection = Vector2di(0,0);
			checkSelection(levelPreviews, selection);
			gridOffset = selection.y;
			checkGridOffset(levelPreviews, gridOffset);
		}
		titleText.setPosition(0,OFFSET_Y - TITLE_FONT_SIZE);
		titleText.setAlignment(RIGHT_JUSTIFIED);
	}
	else if (toState == lsIntermediate || toState == lsIntermediateLevel)
	{
		overlay.render();
		SDL_BlitSurface(GFX::getVideoSurface(),NULL,previewDraw,NULL);
		intermediateSelection = 0;
		titleText.setAlignment(CENTRED);
	}

	state = toState;
}

void StateLevelSelect::doSelection()
{
	if ( returnToMenu )
		setNextState(STATE_MAIN);
	int value = selection.y * PREVIEW_COUNT_X + selection.x;
	if ( state == lsIntermediate )
	{
		switch (intermediateSelection)
		{
		case 0: // play
			if (fadeTimer < 0)
				MUSIC_CACHE->playSound("sounds/level_play.wav");
			if ( fadeOut() ) return;
			ENGINE->playChapter(chapterPreviews[value].filename);
			break;
		case 1: // explore
			exploreChapter(chapterPreviews[value].filename);
			switchState(lsLevel);
			break;
		case 2: // time attack (speedrun!)
			if (fadeTimer < 0)
				MUSIC_CACHE->playSound("sounds/level_play.wav");
			if ( fadeOut() ) return;
			ENGINE->startChapterTrial();
			ENGINE->playChapter(chapterPreviews[value].filename,0);
			break;
		default: // nothing
			break;
		}
	}
	else if ( state == lsIntermediateLevel )// intermediate menu after selecting level
	{
		if (intermediateSelection == 1)
			ENGINE->timeTrial = true;

		if (fadeTimer < 0)
			MUSIC_CACHE->playSound("sounds/level_play.wav");
		if (not exChapter) // level from level folder
		{
			abortLevelLoading = true;
			abortChapterLoading = true;
			if ( fadeOut() ) return;
			ENGINE->playSingleLevel(levelPreviews[value].filename,STATE_LEVELSELECT);
		}
		else // exploring chapter -> start chapter at selected level
		{
			if (value <= exChapter->getProgress())
			{
				abortLevelLoading = true;
				abortChapterLoading = true;
				if ( fadeOut() ) return;
				ENGINE->playChapter(exChapter->filename,value);
			}
		}
	}
	else if ( state == lsLevel )
	{
		if ((not exChapter || value <= exChapter->getProgress()) &&
			StringUtility::stringToBool(SAVEGAME->getData("speedrun")))
		{
			switchState(lsIntermediateLevel);
		}
		else
		{
			if (fadeTimer < 0)
				MUSIC_CACHE->playSound("sounds/level_play.wav");
			if (not exChapter) // level from level folder
			{
				abortLevelLoading = true;
				abortChapterLoading = true;
				if ( fadeOut() ) return;
				ENGINE->playSingleLevel(levelPreviews[value].filename,STATE_LEVELSELECT);
			}
			else // exploring chapter -> start chapter at selected level
			{
				if (value <= exChapter->getProgress())
				{
					abortLevelLoading = true;
					abortChapterLoading = true;
					if ( fadeOut() ) return;
					ENGINE->playChapter(exChapter->filename,value);
				}
			}
		}
	}
	else
	{
		if (value == 0) // level folder
		{
			setLevelDirectory(DEFAULT_LEVEL_FOLDER);
			switchState(lsLevel);
		}
		else // show play/explore menu
		{
			if (chapterPreviews[value].surface && chapterPreviews[value].hasBeenLoaded)
			{
				switchState(lsIntermediate);
			}
		}
	}
}

bool StateLevelSelect::fadeOut()
{
	if ( fadeTimer < 0 )
	{
		fadeTimer = 30;
		EFFECTS->fadeOut(30);
	}
	if ( fadeTimer > 0 )
		return true;
	return false;
}

void StateLevelSelect::checkSelection(const vector<PreviewData>& data, Vector2di& sel)
{
	if (sel.y < 0)
		sel.y = 0;
	else
	{
		int rows = ceil((float)data.size() / (float)PREVIEW_COUNT_X);
		if (sel.y >= rows)
		{
			sel.y = rows - 1;
			mouseInBounds = false;
		}
	}

	if (sel.x < 0)
		sel.x = 0;
	else
	{
		int cols = min(PREVIEW_COUNT_X,(int)data.size() - sel.y * PREVIEW_COUNT_X);
		if (sel.x >= cols)
		{
			sel.x = cols - 1;
			mouseInBounds = false;
		}
	}
}

void StateLevelSelect::checkGridOffset(const vector<PreviewData>& data, int& offset)
{
	if ( offset < 0 )
		offset = 0;
	else
	{
		int temp = max( 0.0f, ceil((float)data.size() / (float)PREVIEW_COUNT_X) - PREVIEW_COUNT_Y );
		if ( offset > temp )
			offset = temp;
	}
	if ( selection.y < offset )
		selection.y = offset;
	else if ( selection.y >= offset + PREVIEW_COUNT_Y )
		selection.y = offset + PREVIEW_COUNT_Y - 1;
}


int StateLevelSelect::loadLevelPreviews(void* data)
{
	printf("Generating level previews images\n");
	StateLevelSelect* self = (StateLevelSelect*)data;
	vector<PreviewData>::iterator iter = self->levelPreviews.begin();
	int levelNumber = 0;
	int maxUnlocked = 0;
	if (self->exChapter)
		maxUnlocked = self->exChapter->getProgress();

	while (not self->abortLevelLoading && iter != self->levelPreviews.end())
	{
		// look for cached images by level filename
		map<string,pair<string,SDL_Surface*> > ::iterator cachedData = self->previewCache.find( (*iter).filename );

		if ( cachedData != self->previewCache.end() ) // found cached image
		{
			SDL_mutexP(self->levelLock);
			(*iter).name = cachedData->second.first;
			(*iter).surface = cachedData->second.second;
			(*iter).hasBeenLoaded = true;
			SDL_mutexV(self->levelLock);
		}
		else
		{
			Level* level;
			if (self->exChapter) // we are browsing a chapter
			{
				// check whether the level has been unlocked, else don't load
				if (levelNumber <= maxUnlocked)
					level = LEVEL_LOADER->loadLevelFromFile(iter->filename,self->exChapter->path);
				else
					level = NULL;
			}
			else
				level = LEVEL_LOADER->loadLevelFromFile(iter->filename);

			SDL_Surface* surf = NULL;
			string levelName = "";
			if (level)
			{
				levelName = level->name;
				SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
				level->init();
				level->update(); // update once to properly position units
				level->render(temp); // render at full resolution
				// scale down, then delete full resolution copy
				surf = zoomSurface(temp,(float)self->size.x / GFX::getXResolution(), (float)self->size.y / GFX::getYResolution(), SMOOTHING_OFF);
				SDL_FreeSurface(temp);
				self->previewCache[iter->filename] = make_pair(levelName, surf);
			}
			else // error on load
			{
				if (LEVEL_LOADER->errorString[0] != 0)
					printf("ERROR: %s\n",LEVEL_LOADER->errorString.c_str());
			}
			SDL_mutexP(self->levelLock);
			(*iter).surface = surf;
			(*iter).name = levelName;
			(*iter).hasBeenLoaded = true;
			SDL_mutexV(self->levelLock);
			delete level;
		}
		++iter;
		++levelNumber;
	}
	if (self->abortLevelLoading)
	{
		printf("Aborted level preview generation\n");
		self->abortLevelLoading = false;
	}
	else
		printf("Finished generating level preview images\n");
	return 0;
}

int StateLevelSelect::loadChapterPreviews(void* data)
{
	StateLevelSelect* self = (StateLevelSelect*)data;

	printf("Loading chapter preview images\n");
	vector<PreviewData>::iterator iter = self->chapterPreviews.begin();
	++iter; // skip level folder
	Chapter chapter;

	while (not self->abortChapterLoading && iter != self->chapterPreviews.end())
	{
		// look for cached images by chapter filename
		map<string,pair<string, SDL_Surface*> >::iterator cachedData = self->previewCache.find( iter->filename );

		if ( cachedData != self->previewCache.end() ) // found cached image
		{
			SDL_mutexP(self->chapterLock);
			(*iter).name = cachedData->second.first;
			(*iter).surface = cachedData->second.second;
			(*iter).hasBeenLoaded = true;
			SDL_mutexV(self->chapterLock);
		}
		else
		{
			if (not chapter.loadFromFile(iter->filename))
			{
				// oops, we have error
				SDL_mutexP(self->chapterLock);
				(*iter).hasBeenLoaded = true;
				SDL_mutexV(self->chapterLock);
				printf("ERROR: %s\n",chapter.errorString.c_str());
				++iter;
				continue;
			}


			SDL_mutexP(self->chapterLock);
			(*iter).name = chapter.name;
			SDL_mutexV(self->chapterLock);


			// loaded chapter file
			if (chapter.imageFile[0] != 0) // we have an image file
			{
				SDL_Surface* img = SURFACE_CACHE->loadSurface(chapter.imageFile,chapter.path,true);
				if (img) // load successful
				{
					if (img->w != self->size.x || img->h != self->size.y) // scale if needed
					{
						img = zoomSurface(img,(float)self->size.x / (float)img->w , (float)self->size.y / (float)img->h, SMOOTHING_OFF);
					}
					else // remove the surface from the cache to prevent double freeing
					{
						SURFACE_CACHE->removeSurface(chapter.imageFile,false);
						SURFACE_CACHE->removeSurface(chapter.path + chapter.imageFile,false);
					}
					self->previewCache[iter->filename] = make_pair((*iter).name, img);
					SDL_mutexP(self->chapterLock);
					(*iter).surface = img;
					(*iter).hasBeenLoaded = true;
					SDL_mutexV(self->chapterLock);
					++iter;
					continue;
				}
			}
			// reaching here we either have no image file or loading it resulted in an error
			// render text instead
			SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE,self->size.x,self->size.y,GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
			SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format,0,0,0));
			self->imageText.print(surf,(*iter).name);
			self->previewCache[iter->filename] = make_pair((*iter).name, surf);
			SDL_mutexP(self->chapterLock);
			(*iter).surface = surf;
			(*iter).hasBeenLoaded = true;
			SDL_mutexV(self->chapterLock);
		}
		++iter;
	}
	if (self->abortChapterLoading)
		printf("Aborted chapter preview generation\n");
	else
		printf("Finished generating chapter preview images\n");
	return 0;
}
