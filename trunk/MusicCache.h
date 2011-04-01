#ifndef MUSICCACHE_H
#define MUSICCACHE_H

#include <map>
#include <SDL/SDL_thread.h>

#include "PenjinTypes.h"
class Music;
class Sound;

#define MUSIC_CACHE MusicCache::getMusicCache()

/**
Play music and sounds through this class
Loaded sounds and music will be cached for sharing and quick access
Class will only allow one music track to play at the time, so playing a second one
will stop the first
Sounds on the other hand will also allow playing of multiple instances of the same
sound at one time
**/

class MusicCache
{
private:
    MusicCache();
    static MusicCache *self;
public:
    virtual ~MusicCache();
    static MusicCache* getMusicCache();

    bool playMusic(CRstring filename, CRbool suppressOutput = false);
    bool playMusic(CRstring filename, CRstring pathOverwrite);
    void stopMusic();
    void setMusicVolume(CRint newVol);
    int getMusicVolume() const {return musicVolume;}

    bool playSound(CRstring filename, CRint numLoops = 0, CRbool suppressOutput = false);
    bool playSound(CRstring filename, CRstring pathOverwrite, CRint numLoops = 0);
    void stopSounds();
    void setSoundVolume(CRint newVol);
    int getSoundVolume() const {return soundVolume;}

    int getMaxVolume() const;

    void clear(CRbool clearPlaying=true);
    void clearMusic(CRbool clearPlaying=true);
    void clearSounds(CRbool clearPlaying=true);

    int sizeMusic() const {return music.size();}
    int sizeSounds() const {return sounds.size();}

    bool isLoaded(CRstring filename) const;

    int fadeDuration;
protected:
    void play(Music* const item, CRstring file);
    void stop(Music* const item);

    // passed data is next music object
    static int fadeMusic(void* data);

    std::map<string,Music*> music;
    std::map<string,Sound*> sounds;

    string musicPlaying;
    int soundVolume;
    int musicVolume;
    SDL_Thread* fadeThread;
    string nextTrack;
private:

};

#endif // MUSICCACHE_H

