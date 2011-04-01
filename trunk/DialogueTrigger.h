#ifndef DIALOGUE_TRIGGER_H
#define DIALOGUE_TRIGGER_H

#include "BaseUnit.h"

class DialogueTrigger : public BaseUnit
{
public:
    DialogueTrigger(Level* newParent);
    virtual ~DialogueTrigger();

    virtual void render(SDL_Surface* surf);

    virtual void hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit);

    int getWidth() const {return width;}
    int getHeight() const {return height;}

protected:
    virtual bool processParameter(const pair<string,string>& value);

    int width;
    int height;
    string textKey;
    int time;
    bool triggered;

    enum DialogueTriggerProps
    {
        tpSize=BaseUnit::upEOL,
        tpTextKey,
        tpTime,
        tpEOL
    };
private:

};


#endif // DIALOGUE_TRIGGER_H

