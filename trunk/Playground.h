#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "Level.h"

class Playground : public Level
{
public:
    Playground();
    virtual ~Playground();

    virtual bool load(const PARAMETER_TYPE& params);

    virtual void userInput();
    virtual void render(SDL_Surface* screen);

protected:
    vector<Rectangle*> mouseRects;
    SDL_Surface* rectangleLayer;
private:

};


#endif // PLAYGROUND_H

