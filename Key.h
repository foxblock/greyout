#ifndef _KEY_H
#define _KEY_H

#include "BaseUnit.h"

class Key : public BaseUnit
{
public:
    Key(Level* newParent);
    virtual ~Key();

    virtual bool load(list<PARAMETER_TYPE >& params);
    virtual bool processParameter(const PARAMETER_TYPE& value);
    virtual void reset();

    virtual void hitUnit(const UnitCollisionEntry& entry);
protected:

    vector<BaseUnit*> targets;

private:

};


#endif // _KEY_H
