#include "Physics.h"

#include "SimpleDirection.h"
#include "Colour.h"
#include "NumberUtility.h"
#include "GFX.h"
#include "Vector2di.h"

#include "BaseUnit.h"
#include "Level.h"

Physics* Physics::self = 0;

Physics::Physics()
{
    gravity = DEFAULT_GRAVITY;
    maximum = DEFAULT_MAXIMUM;

    checkPointsX.push_back(diTOPLEFT);
    checkPointsX.push_back(diTOPRIGHT);
    checkPointsX.push_back(diLEFT);
    checkPointsX.push_back(diRIGHT);

    checkPointsY.push_back(diTOP);
    checkPointsY.push_back(diBOTTOMLEFT);
    checkPointsY.push_back(diBOTTOM);
    checkPointsY.push_back(diBOTTOMRIGHT);
    checkPointsY.push_back(diTOPLEFT);
    checkPointsY.push_back(diTOPRIGHT);
}

Physics::~Physics()
{
    //
}

Physics* Physics::GetSingleton()
{
    if (not self)
        self = new Physics();
    return self;
}

void Physics::applyPhysics(BaseUnit* const unit) const
{
    // Acceleration
    if (unit->acceleration[0].x > 0.0f)
    {
        unit->velocity.x += min(max(unit->acceleration[1].x - unit->velocity.x,0.0f),unit->acceleration[0].x);
    }
    else if (unit->acceleration[0].x < 0.0f)
    {
        unit->velocity.x += max(min(unit->acceleration[1].x - unit->velocity.x,0.0f),unit->acceleration[0].x);
    }
    if (unit->acceleration[0].y > 0.0f)
    {
        unit->velocity.y += min(max(unit->acceleration[1].y - unit->velocity.y,0.0f),unit->acceleration[0].y);
    }
    else if (unit->acceleration[0].y < 0.0f)
    {
        unit->velocity.y += max(min(unit->acceleration[1].y - unit->velocity.y,0.0f),unit->acceleration[0].y);
    }

    // Reset acceleration
    if (unit->velocity.x == unit->acceleration[1].x)
    {
        unit->acceleration[0].x = 0.0f;
        unit->acceleration[1].x = 0.0f;
    }
    if (unit->velocity.y == unit->acceleration[1].y)
    {
        unit->acceleration[0].y = 0.0f;
        unit->acceleration[1].y = 0.0f;
    }

    // Gravity
    if (not unit->flags.hasFlag(BaseUnit::ufNoGravity))
    {
        unit->velocity += gravity;
    }

    // Check for max
    unit->velocity.x = NumberUtility::signMin(unit->velocity.x,maximum.x);
    unit->velocity.y = NumberUtility::signMin(unit->velocity.y,maximum.y);
    /*unit->acceleration[0].x = NumberUtility::signMin(unit->acceleration[0].x,maximum.x);
    unit->acceleration[0].y = NumberUtility::signMin(unit->acceleration[0].x,maximum.y);
    unit->acceleration[1].x = NumberUtility::signMin(unit->acceleration[1].y,maximum.x);
    unit->acceleration[1].y = NumberUtility::signMin(unit->acceleration[1].y,maximum.y);*/
}

/** NOTICE:
New implementation, checking each direction of movement individually. This keeps
the correction of gravity induced movement separate from sideways movement
correction, which is desired in platformers. (assuming gravity in y-direction)
**/
void Physics::unitMapCollision(const Level* const level, SDL_Surface* const colImage, BaseUnit* const unit, const Vector2df& mapOffset) const
{
    /// TODO: Implement step-size and check diBOTTOMLEFT and -RIGHT in x-direction, too
    /// compare to y-correction values and step-size
    /// TODO: Take the unit's velocity into account when returning correction value, so sub-pixel movements get corrected properly

    vector<MapCollisionEntry> collisionDir;
    Vector2df correction(0,0);
    Vector2di pixelCorrection(0,0); // unit will be moved by this step until no collision occurs
    Colour colColour; // the colour taken from the collision surface at the tested point
    Vector2df pixel(0,0); // currently tested pixel

    /// x-direction
    // check which pixels are colliding
    for (vector<SimpleDirection>::const_iterator dir = checkPointsX.begin(); dir != checkPointsX.end(); ++dir)
    {
        pixel = unit->getPixel((*dir));
        pixel.x += unit->velocity.x;
        pixel += unit->collisionInfo.positionCorrection;
        pixel = level->transformCoordinate(pixel);

        // out of bounds check
        if (pixel.x < 0 || pixel.y < 0 || pixel.x >= colImage->w || pixel.y >= colImage->h)
            continue;

        colColour = GFX::getPixel(colImage,pixel.x,pixel.y);

        if (unit->checkCollisionColour(colColour))
        {
            // we have a collision
            MapCollisionEntry entry;
            entry.dir = (*dir);
            entry.pos = pixel;
            entry.col = colColour;
            entry.correction = Vector2df(0,0);
            collisionDir.push_back(entry);
            int temp = dir->xDirection();
            // only use direction if not set before or aiming in the same direction as velocity
            if (pixelCorrection.x == 0 || NumberUtility::sign(unit->velocity.x) == temp)
                pixelCorrection.x = temp * -1;
        }
    }

    // move unit until the collision is solved (maximum by unit's velocity as we
    // are assuming the unit was in a no-collision state before)
    bool stillColliding = (collisionDir.size() > 0); // don't check when there are no colliding pixels
    int correctionX = 0; // total correction to solve collision in this direction
    while (stillColliding && abs(correctionX) <= maximum.x)
    {
        correctionX += pixelCorrection.x;

        vector<MapCollisionEntry>::const_iterator entryPtr;
        for (entryPtr = collisionDir.begin(); entryPtr != collisionDir.end(); ++entryPtr)
        {
            pixel = entryPtr->pos;
            pixel.x += correctionX;
            pixel = level->transformCoordinate(pixel);
            if (pixel.x < 0 || pixel.y < 0 || pixel.x >= colImage->w || pixel.y >= colImage->h)
                continue;

            colColour = GFX::getPixel(colImage,pixel.x,pixel.y);
            if (unit->checkCollisionColour(colColour))
                break;
        }
        if (entryPtr == collisionDir.end()) // all pixel have been checked, so no new collision has been found
            stillColliding = false;
    }

    if (abs(correctionX) < maximum.x)
        correction.x = correctionX;
    unit->collisionInfo.pixels.insert(unit->collisionInfo.pixels.end(),collisionDir.begin(),collisionDir.end());


    /// y-direction
    pixelCorrection = Vector2di(0,0);
    collisionDir.clear();

    for (vector<SimpleDirection>::const_iterator dir = checkPointsY.begin(); dir != checkPointsY.end(); ++dir)
    {
        pixel = unit->getPixel((*dir));
        pixel += unit->velocity;
        pixel.x += correction.x;
        pixel += unit->collisionInfo.positionCorrection;
        pixel = level->transformCoordinate(pixel);

        if (pixel.x < 0 || pixel.y < 0 || pixel.x >= colImage->w || pixel.y >= colImage->h)
            continue;

        colColour = GFX::getPixel(colImage,pixel.x,pixel.y);

        if (unit->checkCollisionColour(colColour))
        {
            // we have a collision
            MapCollisionEntry entry;
            entry.dir = (*dir);
            entry.pos = pixel;
            entry.col = colColour;
            entry.correction = Vector2df(0,0);
            collisionDir.push_back(entry);
            int temp = dir->yDirection();
            // only use direction if not set before or aiming in the same direction as velocity
            if (pixelCorrection.y == 0 || NumberUtility::sign(unit->velocity.y) == temp)
                pixelCorrection.y = temp * -1;
        }
    }

    stillColliding = (collisionDir.size() > 0); // don't check when there are no colliding pixels
    int correctionY = 0; // total correction to solve collision in this direction
    while (stillColliding && abs(correctionY) <= maximum.y)
    {
        correctionY += pixelCorrection.y;

        vector<MapCollisionEntry>::const_iterator entryPtr;
        for (entryPtr = collisionDir.begin(); entryPtr != collisionDir.end(); ++entryPtr)
        {
            pixel = entryPtr->pos;
            pixel.y += correctionY;
            pixel = level->transformCoordinate(pixel);
            if (pixel.x < 0 || pixel.y < 0 || pixel.x >= colImage->w || pixel.y >= colImage->h)
                continue;

            colColour = GFX::getPixel(colImage,pixel.x,pixel.y);
            if (unit->checkCollisionColour(colColour))
                break;
        }
        if (entryPtr == collisionDir.end()) // all pixel have been checked, so no new collision has been found
            stillColliding = false;
    }

    correction.y = correctionY;
    unit->collisionInfo.pixels.insert(unit->collisionInfo.pixels.end(),collisionDir.begin(),collisionDir.end());
    collisionDir.clear();

    unit->collisionInfo.correction = correction;

    unit->hitMap(correction);
}

void Physics::particleMapCollision(const Level* const level, SDL_Surface* const colImage, BaseUnit* const particle) const
{
    Vector2df proPos = particle->position;
    Vector2df correction(0,0);
    Vector2di pixelCorrection(0,0);
    pixelCorrection.x = NumberUtility::sign(particle->velocity.x) * -1;
    pixelCorrection.y = NumberUtility::sign(particle->velocity.y) * -1;

    // x
    bool colliding = true;
    Colour temp;
    if (pixelCorrection.x != 0)
    {
        proPos.x += particle->velocity.x;
        while (colliding && (abs(correction.x) < abs(particle->velocity.x)))
        {
            if (proPos.x + correction.x < 0 || proPos.y < 0 || proPos.x + correction.x >= colImage->w || proPos.y >= colImage->h)
                break;

            temp = GFX::getPixel(colImage,proPos.x + correction.x, proPos.y);

            if (particle->checkCollisionColour(temp)) // collision
            {
                correction.x += pixelCorrection.x;
            }
            else
            {
                colliding = false;
            }
        }
        proPos.x -= particle->velocity.x;
    }
    // y
    if (pixelCorrection.y != 0)
    {
        proPos.y += particle->velocity.y;
        colliding = true;
        while (colliding && (abs(correction.y) < abs(particle->velocity.y)))
        {
            if (proPos.x < 0 || proPos.y + correction.y < 0 || proPos.x >= colImage->w || proPos.y + correction.y >= colImage->h)
                break;

            temp = GFX::getPixel(colImage,proPos.x, proPos.y + correction.y);

            if (particle->checkCollisionColour(temp)) // collision
            {
                correction.y += pixelCorrection.y;
            }
            else
            {
                colliding = false;
            }
        }
    }

    particle->hitMap(correction);
}


/** NOTICE:
The following implementation checks both directions at once instead of one after
another. This, of course, is more accurate, probably a tad faster and has less
quirks and assumptions than the above implementation (and also leads to less copy-pasted
code). BUT on the other hand does not work that well for platformer-type games or
at least not in this, simple implementation.
There are some (different) quirks which I would have to work around and therefore
decided to scrap it. (for example, you "stick" to surfaces)
The code is preserved though and fully functionable, so just replace the above
function with the one below to see how this changes things.
Feel free to use for anything or ask me questions about it (as it is not as well
commented as the above function).
**/
/// TODO: Implement latest changes (the function is not up-to-date)
/*
void Physics::unitMapCollision(SDL_Surface* const colImage, BaseUnit* const unit, const Vector2df& mapOffset) const
{
    Vector2df correction(0,0);
    Vector2di pixelCorrection(0,0); // unit will be moved in this steps until no collision occurs

    /// clear data of previous check
    unit->collisionInfo.clear();

    /// Determine colliding pixels
    // diLEFT to diBOTTOMRIGHT
    for (int dir = 1; dir <= 8; ++dir)
    {
        // current collision pixel
        Vector2df pixel = unit->getPixel(dir) + unit->velocity;
        // pixelCorrection for this pixel (e.g. diTOPLEFT would be corrected to the bottom-right)
        Vector2df temp = SimpleDirection(dir).vectorDirection() * (-1.0f);

        if (pixel.x < 0 || pixel.y < 0 || pixel.x > GFX::getXResolution() || pixel.y > GFX::getYResolution())
            continue;

        Colour colColour = GFX::getPixel(colImage,pixel.x,pixel.y);

        if (unit->checkCollisionColour(colColour))
        {
            // we have a collision
            CollisionEntry entry;
            entry.dir = dir;
            entry.pixel = pixel;
            entry.col = colColour;
            entry.correction = Vector2df(0,0);
            unit->collisionInfo.entries.push_back(entry);
            // only use direction if not set before or aiming in the same direction as velocity
            if (temp.x != 0 && (pixelCorrection.x == 0 || NumberUtility::sign(unit->velocity.x)*-1 == temp.x))
                pixelCorrection.x = temp.x;
            if (temp.y != 0 && (pixelCorrection.y == 0 || NumberUtility::sign(unit->velocity.y)*-1 == temp.y))
                pixelCorrection.y = temp.y;
        }
    }

    /// Calculate correction by moving unit back pixel per pixel
    Vector2df unitVel = unit->velocity;
    bool stillColliding = true;
    while (stillColliding)
    {
        // only correct current movement
        if (unitVel.x ==  0)
            pixelCorrection.x = 0;
        if (unitVel.y == 0)
            pixelCorrection.y = 0;
        if (pixelCorrection.x == 0 && pixelCorrection.y == 0)
        {
            // usually this should not happen as it means there already has been
            // an unsolved collision on the last iteration, but better check for
            // it anyway to prevent an infinite loop
            break;
        }
        unitVel += pixelCorrection;

        vector<CollisionEntry>::const_iterator entryPtr;
        for (entryPtr = unit->collisionInfo.entries.begin(); entryPtr != unit->collisionInfo.entries.end(); ++entryPtr)
        {
            Vector2df pixel = entryPtr->pixel + unitVel - unit->velocity; // unitVel - unitVelocity = pixelCorrection * iteration
            if (pixel.x < 0 || pixel.y < 0 || pixel.x > GFX::getXResolution() || pixel.y > GFX::getYResolution())
                continue;

            Colour colColour = GFX::getPixel(colImage,pixel.x,pixel.y);
            // if unit is still colliding break here and go into next iteration
            if (unit->checkCollisionColour(colColour))
                break;
        }
        if (entryPtr == unit->collisionInfo.entries.end()) // all pixel have been checked, so no new collision has been found
            stillColliding = false;
    }
    correction = unitVel - unit->velocity;

    unit->collisionInfo.correction = correction;

    unit->hitMap();
}
*/


void Physics::playerUnitCollision(const Level* const level, BaseUnit* const player, BaseUnit* const unit) const
{
    /// TODO: Optimize this and remove redundant stuff (aka this is a mess!)
    Vector2df proPosPlayer = player->position + player->velocity;
    Vector2df proPosUnit = unit->position + unit->velocity;
    float xPos = max(proPosPlayer.x, proPosUnit.x);
    float yPos = max(proPosPlayer.y, proPosUnit.y);
    float xPosMax = min(proPosPlayer.x + player->getWidth(), proPosUnit.x + unit->getWidth());
    float yPosMax = min(proPosPlayer.y + player->getHeight(), proPosUnit.y + unit->getHeight());
    if (xPosMax > xPos && yPosMax > yPos)
    {
        float diffX = min(proPosPlayer.x + player->getWidth(), proPosUnit.x + unit->getWidth()) - max(proPosPlayer.x, proPosUnit.x);
        float diffY = min(proPosPlayer.y + player->getHeight(), proPosUnit.y + unit->getHeight()) - max(proPosPlayer.y, proPosUnit.y);
        SimpleDirection dir;
        if (player->position.x < unit->position.x)
            dir = diLEFT;
        else
            dir = diRIGHT;
        UnitCollisionEntry temp = {dir,Vector2df(diffX,diffY),player};
        unit->collisionInfo.units.push_back(temp);
        UnitCollisionEntry temp2 = {-dir,Vector2df(diffX,diffY),unit};
        player->collisionInfo.units.push_back(temp2);
        return;
    }

    Vector2df pos2p = level->boundsCheck(player);
    Vector2df pos2u = level->boundsCheck(unit);
    if (pos2p != player->position || pos2u != unit->position)
    {
        proPosPlayer = pos2p + player->velocity;
        proPosUnit = pos2u + unit->velocity;
        float xPos = max(proPosPlayer.x, proPosUnit.x);
        float yPos = max(proPosPlayer.y, proPosUnit.y);
        float xPosMax = min(proPosPlayer.x + player->getWidth(), proPosUnit.x + unit->getWidth());
        float yPosMax = min(proPosPlayer.y + player->getHeight(), proPosUnit.y + unit->getHeight());
        if (xPosMax > xPos && yPosMax > yPos)
        {
            float diffX = min(proPosPlayer.x + player->getWidth(), proPosUnit.x + unit->getWidth()) - max(proPosPlayer.x, proPosUnit.x);
            float diffY = min(proPosPlayer.y + player->getHeight(), proPosUnit.y + unit->getHeight()) - max(proPosPlayer.y, proPosUnit.y);
            SimpleDirection dir;
            if (pos2p.x < pos2u.x)
                dir = diLEFT;
            else
                dir = diRIGHT;
            UnitCollisionEntry temp = {dir,Vector2df(diffX,diffY),player};
            unit->collisionInfo.units.push_back(temp);
            UnitCollisionEntry temp2 = {-dir,Vector2df(diffX,diffY),unit};
            player->collisionInfo.units.push_back(temp2);
        }
    }
}

bool Physics::checkUnitCollision(const Level* const level, const BaseUnit* const unitA, const BaseUnit* const unitB) const
{
    SDL_Rect rectA = unitA->getRect();
    SDL_Rect rectB = unitB->getRect();
    if (rectCheck(rectA,rectB))
        return true;

    // check warped position
    Vector2df posA2 = level->boundsCheck(unitA);
    Vector2df posB2 = level->boundsCheck(unitB);
    if (posA2 != unitA->position || posB2 != unitB->position)
    {
        rectA.x = posA2.x;
        rectA.y = posA2.y;
        rectB.x = posB2.x;
        rectB.y = posB2.y;
        if (rectCheck(rectA,rectB))
            return true;
    }
    return false;
}

///

bool Physics::rectCheck(const SDL_Rect& rectA, const SDL_Rect& rectB) const
{
    if (((rectB.x - rectA.x) < rectA.w && (rectA.x - rectB.x) < rectB.w) &&
            ((rectB.y - rectA.y) < rectA.h && (rectA.y - rectB.y) < rectB.h))
        return true;
    return false;
}
