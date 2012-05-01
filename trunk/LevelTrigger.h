#ifndef LEVEL_TRIGGER_H
#define LEVEL_TRIGGER_H

#include "BaseTrigger.h"

class LevelTrigger : public BaseTrigger
{
public:
    LevelTrigger(Level* newParent);
    virtual ~LevelTrigger();

    virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    Level* targetLvl;
private:

};


#endif // LEVEL_TRIGGER_H

