#ifndef DIALOGUE_TRIGGER_H
#define DIALOGUE_TRIGGER_H

#include "BaseTrigger.h"

class DialogueTrigger : public BaseTrigger
{
public:
    DialogueTrigger(Level* newParent);
    virtual ~DialogueTrigger();

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    virtual bool processParameter(const PARAMETER_TYPE& value);

    string textKey;
    int time;
    bool triggered;

    enum DialogueTriggerProps
    {
        tpTextKey=BaseTrigger::bpEOL,
        tpTime,
        tpEOL
    };
private:

};


#endif // DIALOGUE_TRIGGER_H

