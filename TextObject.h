#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include "BaseUnit.h"

#include "Text.h"

class TextObject : public BaseUnit
{
public:
    TextObject(Level* newParent);
    virtual ~TextObject();

    virtual bool load(const list<PARAMETER_TYPE >& params);
    virtual bool processParameter(const PARAMETER_TYPE& value);

    virtual void updateScreenPosition(const Vector2di& offset);
    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;

    virtual int getWidth() const {return size.x;}
    virtual int getHeight() const {return size.y;}

    virtual void explode();
protected:
    Text currentText;
    string line;
    Vector2di size;

    enum TextProps
    {
        tpFont=BaseUnit::upEOL,
        tpSize,
        tpText,
        bpEOL
    };
private:

};


#endif // TEXT_OBJECT_H

