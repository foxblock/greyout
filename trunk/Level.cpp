#include "Level.h"

#include "StringUtility.h"

#include "BaseUnit.h"
#include "ControlUnit.h"
#include "Physics.h"
#include "MyGame.h"
#include "userStates.h"
#include "SurfaceCache.h"
#include "LevelLoader.h"

Level::Level()
{
    noCollision = MAGENTA;
    levelImage = NULL;
    collisionLayer = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    unitRect.setColour(noCollision);
    rectangleLayer = NULL;
    logicTimer.setRewind(STOP);
    levelFileName = "";
    chapterPath = "";
    errorString = "";

    stringToFlag["repeatx"] = lfRepeatX;
    stringToFlag["repeaty"] = lfRepeatY;
    stringToFlag["disableSwap"] = lfDisableSwap;

    stringToProp["image"] = lpImage;
    stringToProp["flags"] = lpFlags;
    stringToProp["filename"] = lpFilename;

    #ifdef _DEBUG
    debugText.loadFont("fonts/unispace.ttf",12);
    debugText.setColour(GREEN);
    debugString = "";
    fpsDisplay.loadFont("fonts/unispace.ttf",24);
    fpsDisplay.setColour(GREEN);
    fpsDisplay.setPosition(GFX::getXResolution(),0);
    fpsDisplay.setAlignment(RIGHT_JUSTIFIED);
    #endif
}

Level::~Level()
{
    delete levelImage;
    SDL_FreeSurface(collisionLayer);
    if (rectangleLayer)
        SDL_FreeSurface(rectangleLayer);
    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        delete (*curr);
    }
    players.clear();
    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        delete (*curr);
    }
    units.clear();
    for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
    {
        delete (*curr);
    }
    mouseRects.clear();
}

/// ---public---

bool Level::load(const PARAMETER_TYPE& params)
{
    for (PARAMETER_TYPE::const_iterator value = params.begin(); value != params.end(); ++value)
    {
        if (not processParameter(make_pair(value->first,value->second)) && value->first != "class")
        {
            string className = params.front().second;
            cout << "Warning: Unprocessed parameter \"" << value->first << "\" on level \"" << className << "\"" << endl;
        }
    }

    return true;
}

void Level::init()
{
    // The rectangles will be painted to this surface
    rectangleLayer = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    // Fill it with magenta and apply a colour key to make it transparent
    SDL_FillRect(rectangleLayer, NULL, SDL_MapRGB(rectangleLayer->format,255,0,255));
    SDL_SetColorKey(rectangleLayer, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(rectangleLayer->format,255,0,255));
}

void Level::userInput()
{
    input->update();

#ifdef PLATFORM_PC
    if (input->isQuit())
    {
        nullifyState();
        return;
    }
#endif
    if (input->isStart())
    {
        setNextState(STATE_MAIN);
        return;
    }
    if (input->isY())
    {
        list<pair<string,string> > params;
        params.push_back(make_pair("class","pushablebox"));
        params.push_back(make_pair("collision","0,255"));
        BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
        box->position = input->getMouse() - Vector2df(12,12);
        units.push_back(box);

        input->resetY();
    }


    if ((input->isL() || input->isR()) && not flags.hasFlag(lfDisableSwap))
    {
        if (players.size() > 1)
        {
            for (list<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
                (*unit)->takesControl = not (*unit)->takesControl;
        }
        input->resetL();
        input->resetR();
    }

    if (input->isLeftClick())
    {
        Rectangle* temp = new Rectangle;
        temp->setColour(BLACK);
        temp->setDimensions(24,24);
        temp->setPosition(input->getMouse() - Vector2df(12,12));
        mouseRects.push_back(temp);
    }
    else if (input->isRightClick())
    {
        Rectangle* temp = new Rectangle;
        temp->setColour(WHITE);
        temp->setDimensions(24,24);
        temp->setPosition(input->getMouse() - Vector2df(12,12));
        mouseRects.push_back(temp);
    }

    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        if ((*curr)->takesControl)
            (*curr)->control(input);
    }
}

void Level::update()
{
    #ifdef _DEBUG
    debugString = "";
    #endif
    // Check for units to be removed
    for (list<ControlUnit*>::iterator player = players.begin(); player != players.end();)
    {
        if ((*player)->toBeRemoved)
        {
            player = players.erase(player);
        }
        else
        {
            ++player;
        }
    }
    for (list<BaseUnit*>::iterator unit = units.begin();  unit != units.end();)
    {
        if ((*unit)->toBeRemoved)
        {
            unit = units.erase(unit);
        }
        else
        {
            ++unit;
        }
    }

    // physics (acceleration, friction, etc.) and unit collision
    for (list<ControlUnit*>::iterator player = players.begin(); player != players.end(); ++player)
    {
        adjustPosition((*player));
        PHYSICS->applyPhysics((*player));
        for (list<BaseUnit*>::iterator unit = units.begin();  unit != units.end(); ++unit)
        {
            adjustPosition((*unit));
            PHYSICS->applyPhysics((*unit));
            if (not (*unit)->flags.hasFlag(BaseUnit::ufNoUnitCollision))
                PHYSICS->playerUnitCollision(this,(*player),(*unit));
        }
    }

    // map collision
    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        clearUnitFromCollision(collisionLayer,(*curr));
        if (not (*curr)->flags.hasFlag(BaseUnit::ufNoMapCollision))
        {
            // check for overwritten units by last clearUnitFromCollision call and redraw them
            for (list<BaseUnit*>::iterator other = units.begin(); other != units.end(); ++other)
            {
                if ((*curr) != (*other) && PHYSICS->checkUnitCollision(this,(*curr),(*other)))
                    renderUnit(collisionLayer,(*other));
            }
            PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
        }
        // else still update unit on collision surface for player-map collision
        (*curr)->update();
        renderUnit(collisionLayer,(*curr));
    }

    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        // players should always have map collision enabled, so don't check for that here
        PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
        (*curr)->update();
        // players are never drawn to the collision surface to not interfere with
        // unit-map collision testing
    }

    // other update stuff
}

void Level::render()
{
    render(GFX::getVideoSurface());
    // draw the cursor
    Vector2df pos = input->getMouse();
    GFX::setPixel(pos,RED);
    GFX::setPixel(pos+Vector2df(1,1),RED);
    GFX::setPixel(pos+Vector2df(-1,1),RED);
    GFX::setPixel(pos+Vector2df(1,-1),RED);
    GFX::setPixel(pos+Vector2df(-1,-1),RED);
    GFX::renderPixelBuffer();
}

void Level::render(SDL_Surface* screen)
{
    GFX::clearScreen(screen);

    levelImage->renderImage(screen,0,0);

    if (rectangleLayer)
    {
        for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
        {
            (*curr)->render(rectangleLayer);
            delete (*curr);
        }
        mouseRects.clear();
        SDL_BlitSurface(rectangleLayer,NULL,screen,NULL);
    }

    #ifdef _DEBUG_COL
    SDL_Rect temp;
    temp.w = 800;
    temp.h = 480;
    temp.x = 0;
    temp.y = 480;
    SDL_BlitSurface(collisionLayer,NULL,screen,&temp);
    #endif

    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        renderUnit(screen,(*curr));
    }

    // draw to image used for collision testing before players get drawn
    SDL_BlitSurface(screen,NULL,collisionLayer,NULL);

    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        renderUnit(screen,(*curr));
    }


    #ifdef _DEBUG
    debugString += "Players alive: " + StringUtility::intToString(players.size()) + "\n";
    debugString += "Units alive: " + StringUtility::intToString(units.size()) + "\n";
    debugText.setPosition(10,10);
    debugText.print(screen,debugString);
    fpsDisplay.print(screen,StringUtility::intToString(MyGame::getMyGame()->getFPS()));
    #endif

}

void Level::logicPauseToggle()
{
    //
}

void Level::logicPause(CRint time)
{
    logicTimer.start(time);
    while (not logicTimer.hasFinished())
    {
        logicPauseUpdate();
        render();
        GFX::forceBlit();
    }
}

void Level::logicPauseUpdate()
{
    logicTimer.update();
}


int Level::getWidth() const
{
    return GFX::getXResolution();
}

int Level::getHeight() const
{
    #ifdef _DEBUG_COL
    return GFX::getYResolution() / 2;
    #endif
    return GFX::getYResolution();
}

Vector2df Level::transformCoordinate(const Vector2df& coord) const
{
    Vector2df result = coord;

    // cache some values
    int width = getWidth();
    int height = getHeight();

    // assuming the coordinate is not THAT much out of bounds (else should use modulo)
    if (flags.hasFlag(lfRepeatX))
    {
        if (result.x >= width)
            result.x -= width;
        else if (result.x < 0)
            result.x += width;
    }
    if (flags.hasFlag(lfRepeatY))
    {
        if (result.y >= height)
            result.y -= height;
        else if (result.y < 0)
            result.y += height;
    }
    return result;
}

Vector2df Level::boundsCheck(const BaseUnit* const unit) const
{
    Vector2df result = unit->position;

    // cache some values
    int width = getWidth();
    int height = getHeight();

    if (flags.hasFlag(lfRepeatX))
    {
        if (result.x < 0)
            result.x += width;
        else if (result.x + unit->getWidth() >= width)
            result.x -= width;
    }
    if (flags.hasFlag(lfRepeatY))
    {
        if (result.y < 0)
            result.y += height;
        else if (result.y + unit->getHeight() >= height)
            result.y -= height;
    }
    return result;
}

/// ---protected---

void Level::clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit)
{
    unitRect.setDimensions(unit->getSize());
    unitRect.setPosition(unit->position);
    unitRect.render(surface);
    Vector2df pos2 = boundsCheck(unit);
    if (pos2 != unit->position)
    {
        unitRect.setPosition(pos2);
        unitRect.render(surface);
    }
}

void Level::renderUnit(SDL_Surface* const surface, BaseUnit* const unit)
{
    unit->render(surface);
    Vector2df pos2 = boundsCheck(unit);
    if (pos2 != unit->position)
    {
        Vector2df temp = unit->position;
        unit->position = pos2;
        unit->updateScreenPosition();
        unit->render(surface);
        unit->position = temp;
        unit->updateScreenPosition();
    }
}

void Level::adjustPosition(BaseUnit* const unit)
{
    // 1 = out of right/bottom bounds, -1 = out of left/top bounds
    int boundsX = (unit->position.x > GFX::getXResolution()) - (unit->position.x + unit->getWidth() < 0);
    int boundsY = (unit->position.y > GFX::getYResolution()) - (unit->position.y + unit->getHeight() < 0);
    bool changed = false;

    if (boundsX != 0 && flags.hasFlag(lfRepeatX))
    {
        unit->position.x -= getWidth() * boundsX;
        changed = true;
    }
    if (boundsY != 0 && flags.hasFlag(lfRepeatY))
    {
        unit->position.y -= getHeight() * boundsY;
        changed = true;
    }
    if ((boundsX + boundsY != 0) && not changed)
    {
        unit->position = unit->startingPosition;
        changed = true;
    }
    if (changed)
        unit->updateScreenPosition();
}

bool Level::processParameter(const pair<string,string>& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case lpImage:
    {
        levelImage = new Image;
        bool fromCache;
        levelImage->loadImage(SURFACE_CACHE->getSurface(value.second,chapterPath,fromCache));
        levelImage->setSurfaceSharing(true);
        break;
    }
    case lpFlags:
    {
        vector<string> props;
        StringUtility::tokenize(value.second,props,",");
        for (vector<string>::const_iterator str = props.begin(); str < props.end(); ++str)
            flags.addFlag(stringToFlag[*str]);
        break;
    }
    case lpFilename:
    {
        levelFileName = value.second;
        break;
    }
    default:
        parsed = false;
    }
    return parsed;
}
