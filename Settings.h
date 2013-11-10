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

