#ifndef SURFACE_CACHE_H
#define SURFACE_CACHE_H

#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "PenjinTypes.h"

/**
All image loading is done through this cache
This helps to center error output and also ensures no graphic is loaded twice,
but rather shared between objects through the SDL_Surface pointer
**/

#define SURFACE_CACHE SurfaceCache::getSurfaceCache()

class SurfaceCache
{
    private:
        SurfaceCache();
        static SurfaceCache *self;
    public:
        virtual ~SurfaceCache();
        static SurfaceCache* getSurfaceCache();

        // loads an image from file or the cache if already loaded previously
        // optimize tries to optimize the surface for fast blitting
        // also use it when having display problems
        SDL_Surface* getSurface(CRstring filename, bool &fromCache, CRbool optimize = false, CRbool suppressOutput = false);

        SDL_Surface* getSurface(CRstring filename, CRstring pathOverwrite, bool &fromCache, CRbool optimize = false);

        // clear the cache - frees all stored surfaces!
        void clear();

        int size() const {return cachedSurfaces.size();}
        bool isCached(CRstring filename) const;
    protected:
    private:
        // map<filename, surface>
        std::map<string, SDL_Surface*> cachedSurfaces;
};


#endif // SURFACE_CACHE_H

