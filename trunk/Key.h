#ifndef _KEY_H
#define _KEY_H

#include "BaseUnit.h"

class Key : public BaseUnit
{
public:
    Key(Level* newParent);
    virtual ~Key();

    virtual bool load(const list<PARAMETER_TYPE >& params);
    virtual void reset();

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);
protected:
    virtual bool processParameter(const PARAMETER_TYPE& value);

    enum KeyProp
    {
        kpTarget=BaseUnit::upEOL,
        kpEOL
    };
    BaseUnit* target;

private:

};


#endif // _KEY_H
