#include "TextObject.h"

#include "StringUtility.h"
#include "Level.h"
#include "MusicCache.h"

TextObject::TextObject(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["font"] = tpFont;
    stringToProp["size"] = tpSize;
    stringToProp["text"] = tpText;

    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = BLACK;
    line = "";
}

TextObject::~TextObject()
{
    //
}

///---public---

bool TextObject::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (line[0] == 0)
    {
        cout << "ERROR: No text specified for TextObject: " << id << endl;
        return false;
    }
    currentText.setColour(col);
    size = currentText.getDimensions(line);

    return result;
}

void TextObject::updateScreenPosition(const Vector2di& offset)
{
    currentText.setPosition(position - offset);
}

void TextObject::render(SDL_Surface* screen)
{
    currentText.setColour(col);
    currentText.print(screen,line);
}

bool TextObject::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void TextObject::explode()
{
    SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE,getWidth(),getHeight(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    Colour none = BLACK;
    if (col != MAGENTA)
    {
        SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format,255,0,255));
        none = MAGENTA;
    }
    currentText.setPosition(0,0);
    currentText.print(temp,line);

    int bpp = temp->format->BytesPerPixel;
    Colour pix = MAGENTA;
    Vector2df vel(0,0);
    int time = 0;
    for (int X = 0; X < getWidth(); X+=2)
    {
        for (int Y = 0; Y < getHeight(); Y+=2)
        {
            Uint8 *p = (Uint8 *)temp->pixels + Y * temp->pitch + X * bpp;

            switch(bpp)
            {
            case 1:
                pix.convertColour(*p,temp->format);
                break;

            case 2:
                pix.convertColour(*(Uint16 *)p,temp->format);
                break;

            case 3:
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    pix.convertColour(p[0] << 16 | p[1] << 8 | p[2],temp->format);
                else
                    pix.convertColour(p[0] | p[1] << 8 | p[2] << 16,temp->format);
                break;
            case 4:
                pix.convertColour( *(Uint32 *)p,temp->format);
                break;

            default:
                pix.setColour(MAGENTA);       /* shouldn't happen, but avoids warnings */
            }

            if (pix != none)
            {
                vel.x = Random::nextFloat(-5,5);
                vel.y = Random::nextFloat(-8,-3);
                time = Random::nextInt(750,1250);
                parent->addParticle(this,pix,position + Vector2df(X,Y),vel,time);
            }
        }
    }
    MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
    toBeRemoved = true;

    SDL_FreeSurface(temp);
}

///---protected---

bool TextObject::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case tpFont:
    {
        currentText.loadFont(value.second);
        break;
    }
    case tpSize:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0)
            currentText.setFontSize(val);
        else
            parsed = false;
        break;
    }
    case tpText:
    {
        line = StringUtility::upper(value.second);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

///---private---
