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

#include "Dialogue.h"

#include <fstream>
#include <vector>

#include "StringUtility.h"

#include "fileTypeDefines.h"
#include "BaseUnit.h"
#include "Savegame.h"

#define DIALOGUE_HEIGHT 90
#define DIALOGUE_SPACING 10

using namespace std;

Dialogue* Dialogue::self = NULL;

Dialogue::Dialogue()
{
    timer.init(1000,MILLI_SECONDS);
    rect.setDimensions(GFX::getXResolution(),DIALOGUE_HEIGHT);
    rect.setPosition(0,0);
    rect.setColour(BLACK);
    text.loadFont(GAME_FONT,24);
    text.setUpBoundary(Vector2di(GFX::getXResolution()-DIALOGUE_SPACING*2,DIALOGUE_HEIGHT-DIALOGUE_SPACING*2));
    text.setWrapping(true);
    text.setColour(BLACK);
    text.setPosition(10,10);
}

Dialogue::~Dialogue()
{
    clear();
}

Dialogue* Dialogue::getDialogue()
{
    if (not self)
        self = new Dialogue();
    return self;
}

///---public---

bool Dialogue::loadFromFile(CRstring filename)
{
    printf("Trying to load strings file \"%s\"\n",filename.c_str());

    string line;
    ifstream file(filename.c_str());
    int lineNumber = 0; // for error output
    errorString = "";

    if (file.fail())
    {
        errorString = "Failed to open file for read!";
        return false;
    }

    clear();

    // parse file line by line
    while (getline(file,line))
    {
        ++lineNumber;

        if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING) // comment line - disregard
            continue;

        // solve win-lin compatibility issues
        line = StringUtility::stripLineEndings(line);

        vector<string> tokens;
        StringUtility::tokenize(line,tokens,VALUE_STRING);
        if (tokens.size() != 2)
        {
           printf("Incorrect key-value pair on line %i\n",lineNumber);
        }
        else
        {
            // add to list
            if (lines.find(tokens[0]) != lines.end())
                printf("WARNING: key \"%s\" redefined on line %i!\n",tokens[0].c_str(),lineNumber);
            lines[tokens[0]] = tokens[1];
        }
    }

    printf("Successfully loaded %i strings!\n",lines.size());
    return true;
}

void Dialogue::clear()
{
    lines.clear();
    for (vector<DialogueItem>::iterator iter = queue.begin(); iter != queue.end(); ++iter)
    {
        SDL_FreeSurface(iter->surf);
    }
    queue.clear();
}

void Dialogue::update()
{
    timer.update();
    if (timer.hasFinished() && !queue.empty())
    {
        SDL_FreeSurface(queue.front().surf);
        SAVEGAME->writeTempData(queue.front().key,"true",true);
        queue.erase(queue.begin());
        if (!queue.empty())
        {
            timer.start(queue.front().time);
        }
    }
}

void Dialogue::render()
{
    if (!queue.empty())
    {
        SDL_Rect temp = {0,GFX::getYResolution()-DIALOGUE_HEIGHT,0,0};
        SDL_BlitSurface(queue.front().surf,NULL,GFX::getVideoSurface(),&temp);
    }
}

string Dialogue::getLine(CRstring key) const
{
    map<string,string>::const_iterator line = lines.find(key);
    if (line != lines.end())
        return line->second;
    return "";
}

void Dialogue::queueLine(CRstring key, const BaseUnit* const unit, CRint time)
{
    if (lines.find(key) == lines.end())
    {
        printf("String \"%s\" not found!\n",key.c_str());
        return;
    }
    if (StringUtility::stringToBool(SAVEGAME->getTempData(key))) // line was already queried
        return;
    string line = lines[key];

    SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),DIALOGUE_HEIGHT,GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    rect.setColour(unit->col);
    if (unit->col == BLACK)
        text.setColour(WHITE);
    rect.render(temp);
    text.print(temp,line);
    DialogueItem item = {temp,time,key};
    queue.push_back(item);
    if (queue.size() == 1)
        timer.start(time);
}

///---protected---

///---private---
