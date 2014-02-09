#include "PixelParticle.h"

PixelParticle::PixelParticle(Level* const parent, CRint lifeTime) : BaseUnit(parent)
{
    toBeRemoved = false;
    counter = lifeTime;
    offset = Vector2di(0,0);
}

PixelParticle::~PixelParticle()
{
    //
}

void PixelParticle::update()
{
	if ( counter > 0 )
		--counter;
	else
		toBeRemoved = true;

    position += velocity;
}

void PixelParticle::updateScreenPosition(const Vector2di& offset)
{
    this->offset = offset;
}

void PixelParticle::render(SDL_Surface* screen)
{
    GFX::setPixel(screen,position.x-offset.x,position.y-offset.y,col);
}

void PixelParticle::hitMap(const Vector2df& correction)
{
    if (abs(correction.x) > abs(correction.y))
    {
        velocity.x *= -1;
    }
    else if (correction.y != 0)
    {
        velocity.y *= -0.5;
    }
}
