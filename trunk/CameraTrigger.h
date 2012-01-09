#ifndef CAMERA_TRIGGER_H
#define CAMERA_TRIGGER_H

#include "BaseTrigger.h"

class CameraTrigger : public BaseTrigger
{
public:
    CameraTrigger(Level* newParent);
    virtual ~CameraTrigger();

    virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif

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

