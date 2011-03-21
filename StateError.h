#ifndef STATEERROR_H
#define STATEERROR_H

#include "BaseState.h"
#include "Text.h"
#include "AnimatedSprite.h"

class StateError : public BaseState
{
    public:
        StateError();
        virtual ~StateError();

        virtual void userInput();
        virtual void update();
        virtual void render();

        string errorString;
        uint returnState;
    protected:
        Text errorDisplay;
        AnimatedSprite bg;
    private:

};

#endif // STATEERROR_H

