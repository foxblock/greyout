#ifndef HOLLYWOOD_H
#define HOLLYWOOD_H

#define EFFECTS (Hollywood::GetSingleton())

#include <list>

#include "EffectFade.h"
#include "EffectZoom.h"
#include "EffectOverlay.h"
#include "EffectWipe.h"
//#include "EffectRain.h"

class Hollywood
{
private:
    Hollywood();
    static Hollywood* m_self;
public:
    ~Hollywood();
    static Hollywood* GetSingleton();

    // general
    void update();
    void render();

    // state checking
    bool hasFinished(CRint index=-1);

    // time always in milliseconds
    /// fading
    // alpha-fade in or out
    void fadeIn(CRint time, const Colour& col=BLACK); // fade from colour
    void fadeOut(CRint time, const Colour& col=BLACK); // fade to colour
    /// zooming
    // a black frmae zooming in or out (inverted=true makes it a box)
    void zoomIn(CRint time, const Vector2df& pos, const Vector2df& size=Vector2df(1,1), const Colour& col=BLACK, CRbool inverted=false);
    void zoomOut(CRint time, const Vector2df& pos, const Vector2df& size=Vector2df(1,1),const Colour& col=BLACK, CRbool inverted=false);
    /// overlay
    // simple, static colour overlay, supports alpha
    void colourOverlay(const Colour& col=BLACK);
    void removeOverlay();
    /// wiping
    // wiping animation, a growing rectangle from one of the screen's borders to the opposite one (inverted=true starts with a fully covered screen)
    void wipe(CRint time, const SimpleDirection& startingDirection, const Colour& wipeColour=BLACK, CRbool inverted=false);

    void addCustomEffect(BaseEffect* eff);
    /*/// rain
    void startRain(CRchar density, const Vector2df direction, CRbool fade = true);
    void stopRain(){};*/

    void clear();

private:
    void checkEffectLimit(EFFECT_TYPE checkType);
    vector<BaseEffect*> effects;

};

#endif // HOLLYWOOD_H
