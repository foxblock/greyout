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

#include "GreySurfaceCache.h"

#include <iostream>

GreySurfaceCache::GreySurfaceCache() : SurfaceCache()
{
    #ifdef _DEBUG
    superVerbose = true;
    #else
    superVerbose = false;
    #endif
}

GreySurfaceCache::~GreySurfaceCache()
{
    //
}

SDL_Surface* GreySurfaceCache::loadSurface(CRstring filename, CRstring pathOverwrite, CRbool optimize)
{
    if (pathOverwrite[0] == 0) // no overwrite specified
        return loadSurface(filename,optimize);

    if (superVerbose)
        printf("Trying to load custom image \"%s%s\"\n",pathOverwrite.c_str(),filename.c_str());

    verbose = false;
    SDL_Surface* surface = loadSurface(pathOverwrite + filename, optimize);
    verbose = true;

    if (surface == errorSurface || surface == NULL)
    {
        if (superVerbose)
            printf("Custom image not found, loading default!\n");
        surface = loadSurface(filename,optimize);
    }

    return surface;
}
