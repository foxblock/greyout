#ifndef SOLIDBOX_H
#define SOLIDBOX_H

#include "PushableBox.h"

class SolidBox : public PushableBox
{
    public:
        SolidBox(Level* newParent);
        virtual ~SolidBox();

        virtual bool hitUnitCheck(const BaseUnit* const caller) const;
        virtual void hitUnit(const UnitCollisionEntry& entry);
    protected:
        void move();
    private:

};


#endif // SOLIDBOX_H

