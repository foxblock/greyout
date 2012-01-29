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
    cout << "Trying to load strings file \"" << filename << "\"" << endl;

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
            cout << "Incorrect key-value pair on line " << lineNumber << endl;
        }
        else
        {
            // add to list
            if (lines.find(tokens[0]) != lines.end())
                cout << "Warning: key \"" << tokens[0] << "\" redefined on line " << lineNumber << "!" << endl;
            lines[tokens[0]] = tokens[1];
        }
    }

    cout << "Successfully loaded " << lines.size() << " strings!" << endl;
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
    if (timer.hasFinished() && queue.size() > 0)
    {
        SDL_FreeSurface(queue.front().surf);
        SAVEGAME->writeTempData(queue.front().key,"true",true);
        queue.erase(queue.begin());
        if (queue.size() > 0)
        {
            timer.start(queue.front().time);
        }
    }
}

void Dialogue::render()
{
    if (queue.size() > 0)
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
        cout << "String \"" << key << "\" not found!" << endl;
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
