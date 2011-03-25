#ifndef EXIT_H
#define EXIT_H

#include "BaseUnit.h"

class Exit : public BaseUnit
{
public:
    Exit(Level* newParent);
    virtual ~Exit();

    virtual bool load(const PARAMETER_TYPE& params);

    virtual void hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit);
protected:

private:

};


#endif // EXIT_H

