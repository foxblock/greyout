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
        bool superVerbose;
};


#endif // GREY_SURFACE_CACHE_H

