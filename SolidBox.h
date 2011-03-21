#ifndef SOLIDBOX_H
#define SOLIDBOX_H

#include "PushableBox.h"

class SolidBox : public PushableBox
{
    public:
        SolidBox(Level* newParent);
        virtual ~SolidBox();

        virtual void hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit);
    protected:
        void move();
    private:

};


#endif // SOLIDBOX_H

