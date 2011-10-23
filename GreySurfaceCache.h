#ifndef GREY_SURFACE_CACHE_H
#define GREY_SURFACE_CACHE_H

#include "PenjinTypes.h"
#include "SurfaceCache.h"

/**
All image loading is done through this cache
This helps to center error output and also ensures no graphic is loaded twice,
but rather shared between objects through the SDL_Surface pointer
**/

#ifdef SURFACE_CACHE
#undef SURFACE_CACHE
#endif
#define SURFACE_CACHE ((GreySurfaceCache*)SurfaceCache::getSurfaceCache())

class GreySurfaceCache : public SurfaceCache
{
    protected:
        GreySurfaceCache();
    public:
        virtual ~GreySurfaceCache();

        SDL_Surface* loadSurface(CRstring filename, CRstring pathOverwrite, CRbool optimize = false);

        SDL_Surface* loadSurface(CRstring filename, CRbool optimize = false) {return SurfaceCache::loadSurface(filename,optimize);}

    protected:
};


#endif // GREY_SURFACE_CACHE_H

