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

#ifdef _DEBUG
    virtual string debugInfo();
#endif

protected:
    vector<Rectangle*> mouseRects;

#ifdef _DEBUG
    bool mouseDraw; // switches between drawing and selecting objects
#endif
private:

};


#endif // PLAYGROUND_H

