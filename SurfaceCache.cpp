#include "SurfaceCache.h"

#include <iostream>

SurfaceCache* SurfaceCache::self = NULL;

SurfaceCache::SurfaceCache()
{
    //
}

SurfaceCache::~SurfaceCache()
{
    //
}

SurfaceCache* SurfaceCache::getSurfaceCache()
{
	if (not self)
        self = new SurfaceCache();
	return self;
}

SDL_Surface* SurfaceCache::getSurface(CRstring filename, bool &fromCache, CRbool optimize, CRbool suppressOutput)
{
    SDL_Surface *surface;
    std::map<string, SDL_Surface*>::const_iterator iter;

    iter = cachedSurfaces.find(filename);
    if (iter == cachedSurfaces.end())
    {
        // Surface was not found, let's create it
        if (not suppressOutput)
            std::cout << "Loading new image to cache \"" << filename << "\"" << std::endl;
        surface = IMG_Load(filename.c_str());
        fromCache = false;
        if(not surface)
        {
            if (not suppressOutput)
            {
                std::cout << "Error loading \"" << filename << "\"" << std::endl;
                std::cout << "The error was: " << IMG_GetError() << std::endl;
            }
            return NULL;
        }
        if (optimize)
        {
            SDL_Surface* old = surface;
            if(surface->flags & SDL_SRCALPHA)
                surface = SDL_DisplayFormatAlpha(surface);
            else
                surface = SDL_DisplayFormat(surface);
            SDL_FreeSurface(old);
        }
        cachedSurfaces[filename] = surface;
        return surface;
    }
    else
    {
        fromCache = true;
        return iter->second;
    }
}

SDL_Surface* SurfaceCache::getSurface(CRstring filename, CRstring pathOverwrite, bool &fromCache, CRbool optimize)
{
    if (pathOverwrite[0] == 0) // no overwrite specified
        return getSurface(filename,fromCache,optimize);

    std::cout << "Trying to load custom image \"" << pathOverwrite + filename << "\"" << std::endl;

    SDL_Surface* surface = getSurface(pathOverwrite + filename, fromCache, optimize,true);

    if (not surface)
    {
        std::cout << "Custom image not found, loading default!" << std::endl;
        surface = getSurface(filename,fromCache,optimize);
    }

    return surface;
}

void SurfaceCache::clear()
{
    int size = cachedSurfaces.size();
    std::map<string,SDL_Surface*>::iterator I;
    for (I = cachedSurfaces.begin(); I != cachedSurfaces.end(); ++I)
    {
        SDL_FreeSurface((*I).second);
        (*I).second = NULL;
    }
    cachedSurfaces.clear();
    std::cout << "Surface cache cleared - deleted " << size << " images!" << std::endl;
}

void SurfaceCache::removeSurface(CRstring filename, CRbool destroy)
{
    std::map<string, SDL_Surface*>::iterator iter;

    iter = cachedSurfaces.find(filename);
    if (iter != cachedSurfaces.end())
    {
        std::cout << "Removing single surface from cache \"" << filename << "\"" << std::endl;
        if (destroy)
            SDL_FreeSurface(iter->second);
        iter->second = NULL;
        cachedSurfaces.erase(iter);
    }
}

bool SurfaceCache::isCached(CRstring filename) const
{
    std::map<string, SDL_Surface*>::const_iterator iter;

    iter = cachedSurfaces.find(filename);
    if (iter == cachedSurfaces.end())
        return false;
    return true;
}
