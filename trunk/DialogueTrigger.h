#ifndef DIALOGUE_TRIGGER_H
#define DIALOGUE_TRIGGER_H

#include "BaseTrigger.h"

class DialogueTrigger : public BaseTrigger
{
public:
    DialogueTrigger(Level* newParent);
    virtual ~DialogueTrigger();

    virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    string textKey;
    int time;

    enum DialogueTriggerProps
    {
        tpTextKey=BaseTrigger::bpEOL,
        tpTime,
        tpEOL
    };
private:

};


#endif // DIALOGUE_TRIGGER_H

