#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "Level.h"

class Playground : public Level
{
public:
    Playground();
    virtual ~Playground();

    virtual void userInput();
    virtual void render(SDL_Surface* screen);

protected:
    vector<Rectangle*> mouseRects;
private:

};


#endif // PLAYGROUND_H

