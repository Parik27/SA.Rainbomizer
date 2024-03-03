#pragma once

#include "TTSBackend.hh"
#include "util/dyom/Internet.hh"

#include <sstream>
#include <iomanip>
#include <regex>

#include <nlohmann/json.hpp>
#include <base64.hpp>

class TikTokTTS : public TTSBackend
{
    InternetUtils internet;

public:
    virtual std::string *
    DecodeAPIResponse (const std::string &res)
    {
        nlohmann::json j       = nlohmann::json::parse (res);
        auto           success = j["success"].get<bool> ();

        if (!success)
            throw std::runtime_error (j["error"].get<std::string> ());

        auto         data_b64 = j["data"].get<std::string> ();
        std::string *data = new std::string (base64::from_base64 (data_b64));

        return data;
    }

    virtual HSTREAM
    GetSoundStream (const std::string &text, const std::string &voice,
                    void **data)
    {
        auto res
            = internet
                  .Post ("/api/generation", "Content-Type: application/json",
                         nlohmann::json{
                             {"text", text},
                             {"voice", voice},
                         }
                             .dump ())
                  .GetString ();
        try
            {
                std::string *strData = DecodeAPIResponse (res);
                auto stream = BASS_StreamCreateFile (TRUE, strData->data (), 0,
                                                     strData->size (),
                                                     BASS_STREAM_DECODE);

                *data = strData;
                return stream;
            }
        catch (std::exception &e)
            {
                Logger::GetLogger ()->LogMessage (
                    "Tiktok TTS generation failed");
                Logger::GetLogger ()->LogMessage (e.what ());
            }
        catch (...)
            {
                Logger::GetLogger ()->LogMessage (
                    "Tiktok TTS generation failed");
            }

        return 0;
    }

    virtual std::vector<Voice>
    GetVoices ()
    {
        return std::vector<Voice>{{
            {"en_male_jomboy", "GB", GENDER_M},
            {"en_us_002", "US", GENDER_F},
            {"en_male_funny", "GB", GENDER_M},
            {"es_mx_002", "GB", GENDER_M},
            {"en_female_samc", "US", GENDER_F},
            {"en_us_ghostface", "US", GENDER_M},
            {"en_female_makeup", "US", GENDER_F},
            {"en_male_cody", "US", GENDER_M},
            {"en_male_grinch", "US", GENDER_M},
            {"en_female_richgirl", "US", GENDER_F},
            {"en_male_narration", "US", GENDER_M},
            {"en_us_006", "US", GENDER_M},
            {"en_uk_001", "GB", GENDER_M},
            {"en_male_deadpool", "US", GENDER_M},
            {"en_au_001", "AU", GENDER_M},
            {"en_uk_003", "GB", GENDER_M},
            {"en_male_ashmagic", "GB", GENDER_M},
            {"en_male_jarvis", "GB", GENDER_M},
            {"en_male_ukneighbor", "GB", GENDER_M},
            {"en_male_olantekkers", "GB", GENDER_M},
            {"en_female_shenna", "GB", GENDER_F},
            {"en_male_ukbutler", "GB", GENDER_M},
            {"en_male_trevor", "GB", GENDER_M},
            {"en_female_pansino", "GB", GENDER_F},
            {"en_male_m03_classical", "GB", GENDER_M},
            {"en_male_cupid", "GB", GENDER_M},
            {"en_female_betty", "GB", GENDER_F},
            {"en_male_m2_xhxs_m03_christmas", "GB", GENDER_M},
            {"en_female_grandma", "GB", GENDER_F},
            {"en_male_santa_narration", "GB", GENDER_M},
            {"en_male_santa_effect", "GB", GENDER_M},
            {"en_male_wizard", "GB", GENDER_M},
            {"en_female_emotional", "GB", GENDER_F},
            {"en_us_009", "US", GENDER_M},
            {"en_us_007", "US", GENDER_M},
            {"en_au_002", "AU", GENDER_M},
            {"en_us_010", "US", GENDER_M},
            {"en_us_chewbacca", "US", GENDER_M},
            {"en_us_ghostface", "US", GENDER_M},
            {"en_us_stitch", "US", GENDER_M},
            {"en_us_c3po", "US", GENDER_M},
            {"en_us_rocket", "US", GENDER_M},
            {"en_us_stormtrooper", "US", GENDER_M},
            {"en_male_ghosthost", "US", GENDER_M},
            {"en_female_madam_leota", "US", GENDER_F},
            {"fr_001", "FR", GENDER_M},
            {"fr_002", "FR", GENDER_M},
            {"es_002", "ES", GENDER_M},
            {"es_mx_002", "MX", GENDER_M},
            {"br_001", "BR", GENDER_F},
            {"br_003", "BR", GENDER_F},
            {"br_004", "BR", GENDER_F},
            {"br_005", "BR", GENDER_M},
            {"bp_female_ivete", "BR", GENDER_F},
            {"bp_female_ludmilla", "BR", GENDER_F},
            {"pt_female_lhays", "PT", GENDER_F},
            {"pt_female_laizza", "PT", GENDER_F},
            {"pt_male_bueno", "PT", GENDER_M},
            {"de_001", "DE", GENDER_F},
            {"de_002", "DE", GENDER_M},
            {"id_001", "IN", GENDER_F},
            {"jp_001", "JP", GENDER_F},
            {"jp_003", "JP", GENDER_F},
            {"jp_005", "JP", GENDER_F},
            {"jp_006", "JP", GENDER_M},
            {"jp_female_fujicochan", "JP", GENDER_F},
            {"jp_female_hasegawariona", "JP", GENDER_F},
            {"jp_male_keiichinakano", "JP", GENDER_M},
            {"jp_female_oomaeaika", "JP", GENDER_F},
            {"jp_male_yujinchigusa", "JP", GENDER_M},
            {"jp_female_shirou", "JP", GENDER_F},
            {"jp_male_tamawakazuki", "JP", GENDER_M},
            {"jp_female_kaorishoji", "JP", GENDER_F},
            {"jp_female_yagishaki", "JP", GENDER_F},
            {"jp_male_hikakin", "JP", GENDER_M},
            {"jp_female_rei", "JP", GENDER_F},
            {"jp_male_shuichiro", "JP", GENDER_M},
            {"jp_male_matsudake", "JP", GENDER_M},
            {"jp_female_machikoriiita", "JP", GENDER_F},
            {"jp_male_matsuo", "JP", GENDER_M},
            {"jp_male_osada", "JP", GENDER_M},
            {"kr_002", "KR", GENDER_M},
            {"kr_003", "KR", GENDER_F},
            {"kr_004", "KR", GENDER_M},
            {"BV074_streaming", "NO", GENDER_F},
            {"BV075_streaming", "NO", GENDER_M},
        }};
    }

    void
    FreeData (void *data)
    {
        delete (std::string *) data;
    }

    TikTokTTS () { internet.Open ("tiktok-tts.weilnet.workers.dev"); }

    ~TikTokTTS () { internet.Close (); }
};
