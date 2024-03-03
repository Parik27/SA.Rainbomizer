#pragma once

#include <bass/bass.h>
#include <string>
#include <vector>

enum Gender
{
    GENDER_M,
    GENDER_F,
    GENDER_U
};

struct Voice
{
    std::string Name;
    std::string Country;
    int         Gender;
};

class TTSBackend
{
public:
    virtual HSTREAM GetSoundStream (const std::string &text,
                                    const std::string &voice, void **data)
        = 0;

    virtual std::vector<Voice> GetVoices () = 0;
    virtual void               FreeData (void *data){};
};
