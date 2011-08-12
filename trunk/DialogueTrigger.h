#ifndef DIALOGUE_TRIGGER_H
#define DIALOGUE_TRIGGER_H

#include "BaseUnit.h"

class DialogueTrigger : public BaseUnit
{
public:
    DialogueTrigger(Level* newParent);
    virtual ~DialogueTrigger();

    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);

    int getWidth() const {return width;}
    int getHeight() const {return height;}

protected:
    virtual bool processParameter(const PARAMETER_TYPE& value);

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

