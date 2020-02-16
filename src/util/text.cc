#include "util/text.hh"
#include "functions.hh"
#include "injector/injector.hpp"
#include <algorithm>

const char *tables[] = {
    "INTRO1",  "DUAL",    "SHTR",    "GRAV",    "OTB",     "POOL",    "LOWR",
    "ZERO2",   "INTRO2",  "SWEET1",  "SWEET1B", "SWEET3",  "SWEET2",  "SWEET4",
    "SWEET5",  "SWEET6",  "SWEET7",  "CRASH2",  "CRASH1",  "CRASH3",  "RYDER1",
    "RYDER3",  "RYDER2",  "SMOKE1",  "SMOKE2",  "SMOKE3",  "SMOKE4",  "STRAP1",
    "STRAP2",  "STRAP3",  "STRAP4",  "RACETOR", "CESAR1",  "LAFIN1",  "LAFIN2",
    "BCRASH1", "CAT",     "BCESAR2", "TRU1",    "TRU2",    "BCESAR4", "GARAGE1",
    "GARAGE2", "VALET1",  "SCRASH2", "WUZI1",   "FARLIE4", "FARLIE5", "WUZI2",
    "WUZI4",   "SYN1",    "SYN2",    "SYN3",    "SYN4",    "SYN6",    "SYN7",
    "SYN5",    "FARLIE2", "FARLIE3", "STEAL1",  "STEAL2",  "STEAL4",  "STEAL5",
    "DS",      "ZERO1",   "ZERO4",   "TORENO1", "TORENO2", "DSERT3",  "DSERT4",
    "DSERT6",  "DSERT9",  "DSERT8",  "DSERT10", "DSERT5",  "CASINO1", "CASINO2",
    "CASINO3", "CASINO7", "CASINO4", "CASINO5", "CASINO6", "CASINO9", "CASIN10",
    "VCR1",    "VCR2",    "DOC2",    "HEIST1",  "HEIST3",  "HEIST2",  "HEIST4",
    "HEIST5",  "HEIST9",  "MAN_1",   "MAN_2",   "MAN_3",   "MAN_5",   "GROVE1",
    "GROVE2",  "RIOT1",   "RIOT2",   "RIOT4",   "GYM",     "TRUCK",   "QUARRY",
    "BOAT",    "BS",      "TAXI1",   "AMBULAE", "FIRETRK", "COPCAR",  "BURGLAR",
    "FTRAIN",  "PIMP",    "BLOOD",   "KICKSTT", "BCOU",    "STUNT"};

typedef std::pair<unsigned, std::string> keyPair; // for less writing :P

/*******************************************************/
struct Comp
{
    bool
    operator() (keyPair p, size_t s) const
    {
        return p.first < s;
    }
    bool
    operator() (size_t s, keyPair p) const
    {
        return s < p.first;
    }
};

/*******************************************************/
void
GxtManager::Update (CKeyArray &array)
{
    for (int i = 0; i < array.size; i++)
        mData.push_back (
            std::make_pair (array.data[i].hash, array.data[i].pos));
}

/*******************************************************/
const char *
GxtManager::GetRandomWord ()
{
    const char *str    = mData[random (mData.size () - 1)].second.data ();
    int         length = strlen (str);

    int phrase_start = 0;
    for (phrase_start = random (length - 1); phrase_start >= 0; phrase_start--)
        {
            if (str[phrase_start] == ' ' || str[phrase_start] == '~')
                {
                    phrase_start = phrase_start + 1;
                    break;
                }
        }

    int phrase_end      = phrase_start;
    int last_checkpoint = -1;
    for (; phrase_end - phrase_start <= 8; phrase_end++)
        {
            if (str[phrase_end] == ' ' || str[phrase_end] == '~')
                {
                    last_checkpoint = phrase_end - 1;
                }
            if (phrase_end == length)
                {
                    last_checkpoint = phrase_end - 1;
                    break;
                }
        }
    if (last_checkpoint == -1)
        last_checkpoint = phrase_end;

    mRandomWordData
        = std::string (str + phrase_start, last_checkpoint - phrase_start + 1);

    return mRandomWordData.data ();
}

/*******************************************************/
const char *
GxtManager::GetText (std::string key)
{
    unsigned int hash = CKeyGen::GetUppercaseKey (key.c_str ());

    auto iter = std::lower_bound (mData.begin (), mData.end (), hash, Comp ());
    if (iter != mData.end () && hash == (*iter).first)
        return (*iter).second.data ();
    return "";
}

/*******************************************************/
void
GxtManager::Initialise (CText *text)
{
    if (mData.size () > 0)
        return;

    Update (text->tKeyMain);
    for (int i = 0; i < sizeof (tables) / 4; i++)
        {
            text->LoadMissionText (tables[i]);
            Update (text->tKeyMission);
        }

    std::sort (std::begin (mData), std::end (mData));
}

std::vector<keyPair> GxtManager::mData;
std::string          GxtManager::mRandomWordData;
