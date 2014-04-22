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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "PenjinTypes.h"
#include "Vector2di.h"
#include "Text.h"
#include "AnimatedSprite.h"
#include <vector>
#include <string>

class SimpleJoy;

class Settings
{
public:
	Settings();
	virtual ~Settings();

	int update();
	void render(SDL_Surface *screen);
    void userInput(SimpleJoy *input);

	void show();
	void close();

    void loadFromFile();
    void saveToFile();

	// Settings
	int getMusicVolume();
	void setMusicVolume(CRint newVol);
	int getSoundVolume();
	void setSoundVolume(CRint newVol);

	enum DrawPattern
	{
		dpOff=0,
		dpArrows,
		dpShaded,
		dpFull,
		dpEOL
	};
	int getDrawPattern();
	void setDrawPattern(CRint newDp);

	enum ParticleDensity
	{
		pdOff=0,
		pdFew,
		pdMany,
		pdTooMany,
		pdEOL
	};
	int getParticleDensity();
	void setParticleDensity(CRint newPd);

	enum CameraBehaviour
	{
		cbStatic=0,
		cbTrailing,
		cbAhead,
		cbEOL
	};
	int getCameraBehaviour();
	void setCameraBehaviour(CRint newCb);

	bool getDrawLinks();
	void setDrawLinks(CRbool newLinks);

	bool getDrawFps();
	void setDrawFps(CRbool newFps);

	bool getWriteFps();
	void setWriteFps(CRbool newFps);

	bool getDebugControls();
	void setDebugControls(CRbool newDebug);

	bool isActive() const {return active;}

protected:
	void renderCategories(SDL_Surface *surf);
	void renderAudio(SDL_Surface *surf);
	void renderGame(SDL_Surface *surf);
	void renderVideo(SDL_Surface *surf);
	void inputCategories(SimpleJoy *input);
	void inputAudio(SimpleJoy *input);
	void inputGame(SimpleJoy *input);
	void inputVideo(SimpleJoy *input);

	int category;
	int sel;
	Vector2di lastPos;
	bool mouseInBounds;

	Text headline;
	Text menuText;
	Text entriesText;
	AnimatedSprite arrows;
	SDL_Rect rect;
	SDL_Rect headlineRect;
	vector<string> categoryItems;
	vector<string> audioItems;
	vector<string> gameItems;
	vector<string> videoItems;
	vector<string> patternStrings;
	vector<string> particleStrings;
	vector<string> cameraStrings;

	// Settings
	// music and sound volume are stored in the MusicCache
	int drawPattern;
	int particleDensity;
	int cameraBehaviour;
	bool drawLinks;
	bool drawFps;
	bool writeFps;
	bool debugControls;

	bool active;
private:

};


#endif // _SETTINGS_H_

