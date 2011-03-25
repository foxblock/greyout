#ifndef PIXELPARTICLE_H
#define PIXELPARTICLE_H

#include "CountDown.h"
#include "BaseUnit.h"

/**
A single pixel bouncing around the map, created from a dying unit making it look
like the unit exploded into pixels
**/

class PixelParticle : public BaseUnit
{
    public:
        PixelParticle(Level* const parent, CRint lifeTime);
        virtual ~PixelParticle();

        virtual void update();
        virtual void updateScreenPosition(const Vector2di& offset);
        virtual void render(SDL_Surface* screen);

        virtual void hitMap(const Vector2df& correction);

    protected:
        static void counterCallback(void* data);

        CountDown counter;
        Vector2di offset;
    private:

};


#endif // PIXELPARTICLE_H

