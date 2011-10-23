#include "GreySurfaceCache.h"

#include <iostream>

GreySurfaceCache::GreySurfaceCache() : SurfaceCache()
{
    //
}

GreySurfaceCache::~GreySurfaceCache()
{
    //
}

SDL_Surface* GreySurfaceCache::loadSurface(CRstring filename, CRstring pathOverwrite, CRbool optimize)
{
    if (pathOverwrite[0] == 0) // no overwrite specified
        return loadSurface(filename,optimize);

    std::cout << "Trying to load custom image \"" << pathOverwrite + filename << "\"" << std::endl;

    SDL_Surface* surface = loadSurface(pathOverwrite + filename, optimize);

    if (surface == errorSurface || surface == NULL)
    {
        std::cout << "Custom image not found, loading default!" << std::endl;
        surface = loadSurface(filename,optimize);
    }

    return surface;
}
