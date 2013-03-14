#ifndef STATECHAPTERTRIAL_H
#define STATECHAPTERTRIAL_H

#include "BaseState.h"
#include "Text.h"
#include "AnimatedSprite.h"
#include "Rectangle.h"

class StateChapterTrial : public BaseState
{
    public:
        StateChapterTrial();
        virtual ~StateChapterTrial();

        virtual void userInput();
        virtual void update();
        virtual void render();

    protected:
        Text timeTrialText;
        AnimatedSprite bg;
        int menuSelection;
        Vector2di lastPos;
        Rectangle menuRect;
        Text menuText;
		vector<string> pauseItems;
		bool newRecord;
		int timeDisplay;
    private:

};

#endif // STATECHAPTERTRIAL_H
