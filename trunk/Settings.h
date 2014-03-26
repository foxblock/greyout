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
	int sel;
	Vector2di lastPos;
	bool mouseInBounds;

	Text menuText;
	Text entriesText;
	AnimatedSprite arrows;
	vector<string> menuItems;
	vector<string> patternStrings;
	vector<string> particleStrings;

	// Settings
	// music and sound volume are stored in the MusicCache
	int drawPattern;
	int particleDensity;
	bool drawLinks;
	bool drawFps;
	bool writeFps;
	bool debugControls;

	bool active;
private:

};


#endif // _SETTINGS_H_

