#include "sounds.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include <cctype>
#include "text.hh"

SoundRandomizer *SoundRandomizer::mInstance = nullptr;

/*******************************************************/
void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id)
{
    auto soundPair = SoundRandomizer::GetInstance ()->GetRandomPair ();
    audio->PreloadMissionAudio (slot, soundPair.id);
}

/*******************************************************/
char* __fastcall CorrectSubtitles(CText* text, void* edx, char* key)
{
	auto prevId = SoundRandomizer::GetInstance ()->GetPreviousPairID ();
	if(prevId == -1)
		return text->Get(key);

	printf("Getting ID: %d\n", prevId);
	auto soundPair = SoundRandomizer::GetInstance ()->GetPairByID (prevId);
	printf("%s\n", soundPair.name.c_str());
	return (char*) GxtManager::GetText(soundPair.name);
}

/*******************************************************/
SoundPair
SoundRandomizer::GetRandomPair ()
{
	int id = random (mSoundTable.size () - 1);
	mPreviousPairs.push_back(id);
	
	return mSoundTable[id];
}

/*******************************************************/
SoundPair
SoundRandomizer::GetPairByID (int id)
{
    return mSoundTable[id];
}

/*******************************************************/
int
SoundRandomizer::GetPreviousPairID ()
{
	if(mPreviousPairs.size() < 1)
		return -1;
	
	int id = mPreviousPairs.front();
	mPreviousPairs.erase(mPreviousPairs.begin());

	return id;
}

/*******************************************************/
void
SoundRandomizer::InitaliseSoundTable ()
{
    FILE *audioEventsTable = fopen ("data/AudioEvents.txt", "r");
    if (!audioEventsTable)
        {
            Logger::GetLogger ()->LogMessage ("Unable to load AudioEvents.txt");
            return;
        }

    char line[256] = {0};
    while (fgets (line, 256, audioEventsTable))
        {
            if (!isalpha (line[0]))
                continue;

            char name[128] = {0};
            int  id        = -1;

            sscanf (line, " %s %d ", name, &id);
            if (id >= 2000)
                mSoundTable.push_back ({id, name + 6});

			printf("%d\n", mSoundTable.size());
        }
}

/*******************************************************/
void
SoundRandomizer::Initialise ()
{
    Logger::GetLogger ()->LogMessage ("Intialised SoundRandomizer");
    RegisterHooks ({
			{HOOK_CALL, 0x4851BB, (void *) &RandomizeAudioLoad},
			{HOOK_CALL, 0x468173, (void *) &CorrectSubtitles}
		});
    InitaliseSoundTable ();
	printf("%d\n", mSoundTable.size());
}

/*******************************************************/
void
SoundRandomizer::DestroyInstance ()
{
    if (SoundRandomizer::mInstance)
        delete SoundRandomizer::mInstance;
}

/*******************************************************/
SoundRandomizer *
SoundRandomizer::GetInstance ()
{
    if (!SoundRandomizer::mInstance)
        {
            SoundRandomizer::mInstance = new SoundRandomizer ();
            atexit (&SoundRandomizer::DestroyInstance);
        }
    return SoundRandomizer::mInstance;
}
