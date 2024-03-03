#pragma once

#include "TikTok.hh"
#include <map>
#include "functions.hh"

class TikTokSinging : public TikTokTTS
{
public:
    auto &
    GetSingingVoicesList ()
    {
        const static std::map<std::string, std::vector<std::string>> voices
            = {{
                   "singing_male",
                   {{"en_male_sing_deep_jingle"},
                    {"en_male_sing_funny_it_goes_up"},
                    {"en_male_m03_lobby"},
                    {"en_male_m03_sunshine_soon"},
                    {"en_male_m2_xhxs_m03_silly"},
                    {"en_male_sing_funny_thanksgiving"}},
               },

               {
                   "singing_female",
                   {{"en_female_f08_salut_damour"},
                    {"en_female_f08_warmy_breeze"},
                    {"en_female_ht_f08_glorious"},
                    {"en_female_ht_f08_wonderful_world"},
                    {"en_female_ht_f08_halloween"},
                    {"en_female_ht_f08_newyear"},
                    {"en_female_f08_twinkle"}},
               }};

        return voices;
    }

    virtual HSTREAM
    GetSoundStream (const std::string &text, const std::string &voice,
                    void **data) override
    {
        return TikTokTTS::GetSoundStream (
            text, GetRandomElement (GetSingingVoicesList ().at (voice)), data);
    }

    virtual std::vector<Voice>
    GetVoices () override
    {
        return {{"singing_male", "GB", GENDER_M},
                {"singing_female", "GB", GENDER_F}};
    }
};
