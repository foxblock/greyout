#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include "BaseUnit.h"

#include "Text.h"

class TextObject : public BaseUnit
{
public:
    TextObject(Level* newParent);
    virtual ~TextObject();

    virtual bool TextObject::load(const list<PARAMETER_TYPE >& params);

    virtual void updateScreenPosition(const Vector2di& offset);
    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;

    int getWidth() const {return currentText.getWidth();}
    int getHeight() const {return currentText.getHeight();}
protected:
    virtual bool processParameter(const PARAMETER_TYPE& value);

    Text currentText;
    string line;

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

