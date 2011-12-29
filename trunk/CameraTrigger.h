#ifndef CAMERA_TRIGGER_H
#define CAMERA_TRIGGER_H

#include "BaseTrigger.h"

class CameraTrigger : public BaseTrigger
{
public:
    CameraTrigger(Level* newParent);
    virtual ~CameraTrigger();

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    virtual bool processParameter(const PARAMETER_TYPE& value);

    int time;
    Vector2df dest;

    enum CameraTriggerProps
    {
        cpDestination=BaseTrigger::bpEOL,
        cpTime,
        tpEOL
    };
private:

};


#endif // CAMERA_TRIGGER_H

