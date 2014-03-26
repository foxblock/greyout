/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

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

    // time always in FRAMES (!!!)
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
