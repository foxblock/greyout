#ifndef EXIT_TRIGGER_H
#define EXIT_TRIGGER_H

#include "BaseTrigger.h"

class ExitTrigger : public BaseTrigger
{
public:
    ExitTrigger(Level* newParent);
    virtual ~ExitTrigger();

    bool load(list<PARAMETER_TYPE >& params);
protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);
private:

};


#endif // EXIT_TRIGGER_H

