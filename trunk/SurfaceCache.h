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

        // removes a surface from the cache, set destroy to false to only remove
        // the reference from the map and not actually free the surface
        void removeSurface(CRstring filename,CRbool destroy=true);

        size_t size() const {return cachedSurfaces.size();}

        bool isCached(CRstring filename) const;
    protected:
    private:
        SDL_Surface* createErrorSurface(const Uint32 width, const Uint32 height) const;

        std::map<string, SDL_Surface*> cachedSurfaces;
        SDL_Surface* errorSurface;
};


#endif // SURFACE_CACHE_H

