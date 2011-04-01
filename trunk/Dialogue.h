#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <map>
#include <SDL/SDL.h>

#include "PenjinTypes.h"
#include "CountDown.h"
#include "Rectangle.h"
#include "Text.h"

#define DIALOGUE Dialogue::getDialogue()

class BaseUnit;

class Dialogue
{
private:
    Dialogue();
    static Dialogue *self;
public:
    virtual ~Dialogue();
    static Dialogue* getDialogue();

    bool loadFromFile(CRstring filename);
    void clear();

    void update();
    void render();

    string getLine(CRstring key) const;
    void queueLine(CRstring key, const BaseUnit* const unit, CRint time);

    string errorString;

protected:
    std::map<string,string> lines;
    CountDown timer;
    Rectangle rect;
    Text text;
    vector<pair<SDL_Surface*,int> > queue;
private:

};

#endif // DIALOGUE_H

