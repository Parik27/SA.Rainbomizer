#pragma once

#include "TTSBackend.hh"
#include "util/dyom/Internet.hh"

#include <sstream>
#include <iomanip>
#include <regex>

class StreamlabsPolly : public TTSBackend
{
    InternetUtils internet;

    static std::string
    EncodeURL (const std::string &s)
    {
        const std::string safe_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgh"
                                            "ijklmnopqrstuvwxyz0123456789-._~";
        std::ostringstream oss;
        for (auto c : s)
            {
                if (safe_characters.find (c) != std::string::npos)
                    oss << c;
                else
                    oss << '%' << std::setfill ('0') << std::setw (2)
                        << std::uppercase << std::hex << (0xff & c);
            }
        return oss.str ();
    }

    std::string
    GetSoundURL (const std::string &text, const std::string &voice)
    {
        auto res
            = internet
                  .Post ("/polly/speak",
                         "Content-Type: application/x-www-form-urlencoded\r\n"
                         "Referer: https://streamlabs.com",
                         "service=Polly&voice=" + voice
                             + "&text=" + EncodeURL (text))
                  .GetString ();

        auto start = res.find ("\"speak_url\"") + 13;
        auto end   = res.find ("\"", start);

        auto url = res.substr (start, end - start);
        return std::regex_replace (url, std::regex ("\\\\/"), "/");
    }

public:
    virtual HSTREAM
    GetSoundStream (const std::string &text, const std::string &voice,
                    void **data)
    {
        auto url = GetSoundURL (text, voice);
        auto stream
            = BASS_StreamCreateURL (url.c_str (), 0, BASS_STREAM_DECODE, 0, 0);
        return stream;
    }

    virtual std::vector<Voice>
    GetVoices ()
    {
        return std::vector<Voice>{{
            {"Brian", "GB", GENDER_M},    {"Amy", "GB", GENDER_F},
            {"Emma", "GB", GENDER_F},     {"Geraint", "GB-WLS", GENDER_M},
            {"Russell", "AU", GENDER_M},  {"Nicole", "AU", GENDER_F},
            {"Joey", "US", GENDER_M},     {"Justin", "US", GENDER_M},
            {"Matthew", "US", GENDER_M},  {"Ivy", "US", GENDER_F},
            {"Joanna", "US", GENDER_F},   {"Kendra", "US", GENDER_F},
            {"Kimberly", "US", GENDER_F}, {"Salli", "US", GENDER_F},
            {"Raveena", "IN", GENDER_F},  {"Zeina", "ARAB", GENDER_F},
            {"Zhiyu", "CN", GENDER_F},    {"Mads", "DK", GENDER_M},
            {"Naja", "DK", GENDER_F},     {"Ruben", "NL", GENDER_M},
            {"Lotte", "NL", GENDER_F},    {"Mathieu", "FR", GENDER_M},
            {"Celine", "FR", GENDER_F},   {"Lea", "FR", GENDER_F},
            {"Chantal", "CA", GENDER_F},  {"Hans", "DE", GENDER_M},
            {"Marlene", "DE", GENDER_F},  {"Vicki", "DE", GENDER_F},
            {"Aditi", "IN", GENDER_F},    {"Karl", "IS", GENDER_M},
            {"Dora", "IS", GENDER_F},     {"Giorgio", "IT", GENDER_M},
            {"Carla", "IT", GENDER_F},    {"Bianca", "IT", GENDER_F},
            {"Takumi", "JP", GENDER_M},   {"Mizuki", "JP", GENDER_F},
            {"Seoyeon", "KR", GENDER_F},  {"Liv", "NO", GENDER_F},
            {"Jacek", "PL", GENDER_M},    {"Jan", "PL", GENDER_M},
            {"Ewa", "PL", GENDER_F},      {"Maja", "PL", GENDER_F},
            {"Ricardo", "BR", GENDER_M},  {"Camila", "BR", GENDER_F},
            {"Vitoria", "BR", GENDER_F},  {"Cristiano", "PT", GENDER_M},
            {"Ines", "PT", GENDER_F},     {"Carmen", "RO", GENDER_F},
            {"Maxim", "RU", GENDER_M},    {"Tatyana", "RU", GENDER_F},
            {"Enrique", "ES", GENDER_M},  {"Conchita", "ES", GENDER_F},
            {"Lucia", "ES", GENDER_F},    {"Mia", "MX", GENDER_F},
            {"Miguel", "US", GENDER_M},   {"Lupe", "US", GENDER_F},
            {"Penelope", "US", GENDER_F}, {"Astrid", "SE", GENDER_F},
            {"Filiz", "TR", GENDER_M},    {"Gwyneth", "GB-WLS", GENDER_F},
        }};
    }

    StreamlabsPolly () { internet.Open ("streamlabs.com"); }

    ~StreamlabsPolly () { internet.Close (); }
};
