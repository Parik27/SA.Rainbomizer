#include <util/dyom/DYOMFileFormat.hh>
#include <streambuf>
#include <istream>

namespace DYOM {

// https://stackoverflow.com/questions/13059091/creating-an-input-stream-from-constant-memory/13059195#13059195
struct membuf : std::streambuf
{
    membuf (char const *base, size_t size)
    {
        char *p (const_cast<char *> (base));
        this->setg (p, p, p + size);
    }
};
struct imemstream : virtual membuf, std::istream
{
    imemstream (char const *base, size_t size)
        : membuf (base, size), std::istream (
                                   static_cast<std::streambuf *> (this))
    {
    }
};

template <typename T, typename F1, typename F2>
void
DYOMFileStructure::ReadWriteInternal (T &file, F1 byteFunc, F2 strFunc)
{
    auto Read = [&file, byteFunc] (auto &&data) {
        byteFunc (file, reinterpret_cast<char *> (&data), sizeof (data));
        return data;
    };
    auto ReadBytes = [&file, byteFunc] (auto &data, int size) {
        byteFunc (file, reinterpret_cast<char *> (&data), size);
    };

    auto ReadString = [&file, strFunc] (auto &out) {
        strFunc(file, out);
    };

    auto ReadStrings = [ReadString] (auto &out, int num) {
        for (int i = 0; i < num; i++)
            ReadString (out[i]);
    };

    g_DYOM_FILE_VERSION = abs (Read (g_DYOM_FILE_VERSION));
    switch (g_DYOM_FILE_VERSION)
        {
        case 1:
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 20 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 20 * sizeof (uint32_t));

            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 80 );
            ReadBytes (g_DYOM_CAR_COLOR1, 80 );
            ReadBytes (g_DYOM_CAR_COLOR2, 80 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 80 );
            ReadBytes (g_DYOM_CAR_HEALTH, 80 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 80 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 80 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 80 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 80 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 80 );
            ReadBytes (g_DYOM_PICKUP_MODE, 80 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 80 );

            break;
        case 2:
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 20 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_HEALTH, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ACCURACY, 20 * sizeof (uint32_t));

            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 80 );
            ReadBytes (g_DYOM_CAR_COLOR1, 80 );
            ReadBytes (g_DYOM_CAR_COLOR2, 80 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 80 );
            ReadBytes (g_DYOM_CAR_HEALTH, 80 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 80 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 80 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 80 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 80 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 80 );
            ReadBytes (g_DYOM_PICKUP_MODE, 80 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 80 );

            break;
        case 3:
            ReadStrings (g_HEADERSTRINGS, 5);
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PROPERTIES);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 20 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_HEALTH, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ACCURACY, 20 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TIMELIMIT, 20 * sizeof (uint32_t));
            ReadStrings (g_TEXTOBJECTIVES, 20);
            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 80 );
            ReadBytes (g_DYOM_CAR_COLOR1, 80 );
            ReadBytes (g_DYOM_CAR_COLOR2, 80 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 80 );
            ReadBytes (g_DYOM_CAR_HEALTH, 80 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 80 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 80 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 80 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 80 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 80 );
            ReadBytes (g_DYOM_PICKUP_MODE, 80 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 80 );

            break;
        case 4:
            ReadStrings (g_HEADERSTRINGS, 5);
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_OBJECT_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PROPERTIES);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            Read (g_DYOM_PLAYER_HEALTH);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 50 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 50 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 50 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 50 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 50 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_HEALTH, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ACCURACY, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TIMELIMIT, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ANIMATION, 50 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DISTANCE, 50 * sizeof (uint32_t));

            ReadStrings (g_TEXTOBJECTIVES, 50);
            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_HIDEWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );
            ReadBytes (g_DYOM_ACTOR_ANIMATION, 400 );
            ReadBytes (g_DYOM_ACTOR_DISTANCE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 80 );
            ReadBytes (g_DYOM_CAR_COLOR1, 80 );
            ReadBytes (g_DYOM_CAR_COLOR2, 80 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 20 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 80 );
            ReadBytes (g_DYOM_CAR_HEALTH, 80 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 80 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 80 );
            ReadBytes (g_DYOM_CAR_HIDEWITH, 80 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 80 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 80 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 80 );
            ReadBytes (g_DYOM_PICKUP_MODE, 80 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 20 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 80 );
            ReadBytes (g_DYOM_PICKUP_HIDEWITH, 80 );

            ReadBytes (g_DYOM_OBJECT_MODEL, 400 );
            ReadBytes (g_DYOM_OBJECT_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RX, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RY, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RZ, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_INTERIOR, 400 );
            ReadBytes (g_DYOM_OBJECT_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_OBJECT_HIDEWITH, 400 );

            break;
        case 5:
            ReadStrings (g_HEADERSTRINGS, 5);
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_OBJECT_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PROPERTIES);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            Read (g_DYOM_PLAYER_HEALTH);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 100 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_HEALTH, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ACCURACY, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TIMELIMIT, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ANIMATION, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DISTANCE, 100 * sizeof (uint32_t));

            ReadStrings (g_TEXTOBJECTIVES, 100);
            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_HIDEWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );
            ReadBytes (g_DYOM_ACTOR_ANIMATION, 400 );
            ReadBytes (g_DYOM_ACTOR_DISTANCE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 200 );
            ReadBytes (g_DYOM_CAR_COLOR1, 200 );
            ReadBytes (g_DYOM_CAR_COLOR2, 200 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 200 );
            ReadBytes (g_DYOM_CAR_HEALTH, 200 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 200 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 200 );
            ReadBytes (g_DYOM_CAR_HIDEWITH, 200 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 200 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 200 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 200 );
            ReadBytes (g_DYOM_PICKUP_MODE, 200 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 200 );
            ReadBytes (g_DYOM_PICKUP_HIDEWITH, 200 );

            ReadBytes (g_DYOM_OBJECT_MODEL, 400 );
            ReadBytes (g_DYOM_OBJECT_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RX, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RY, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RZ, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_INTERIOR, 400 );
            ReadBytes (g_DYOM_OBJECT_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_OBJECT_HIDEWITH, 400 );
            ReadBytes (g_8277, 1600 );
            ReadBytes (g_8677, 1600 );
            ReadBytes (g_9077, 1600 );
            ReadBytes (g_9477, 1600 );

            break;
        case 6:
            ReadStrings (g_HEADERSTRINGS, 6);
            Read (g_DYOM_OBJECTIVE_COUNT);
            Read (g_DYOM_ACTOR_COUNT);
            Read (g_DYOM_CAR_COUNT);
            Read (g_DYOM_PICKUP_COUNT);
            Read (g_DYOM_OBJECT_COUNT);
            Read (g_DYOM_TIMELIMIT);
            Read (g_DYOM_TIMEOFDAY);
            Read (g_DYOM_WEATHER);
            Read (g_DYOM_MINWANTEDLEVEL);
            Read (g_DYOM_MAXWANTEDLEVEL);
            Read (g_DYOM_PROPERTIES);
            Read (g_DYOM_PLAYER_LOCATION_X);
            Read (g_DYOM_PLAYER_LOCATION_Y);
            Read (g_DYOM_PLAYER_LOCATION_Z);
            Read (g_DYOM_PLAYER_LOCATION_A);
            Read (g_DYOM_PLAYER_INTERIOR);
            Read (g_DYOM_PLAYER_MODEL);
            Read (g_DYOM_PLAYER_WEAPON);
            Read (g_DYOM_PLAYER_AMMO);
            Read (g_DYOM_PLAYER_HEALTH);
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECTIVE_INTERIOR, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TYPE, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MODEL, 100 * sizeof (int32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DATA, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_AMMO, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_MARKERCOLOR, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_PROPERTIES, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_HEALTH, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ACCURACY, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_TIMELIMIT, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_ANIMATION, 100 * sizeof (uint32_t));
            ReadBytes (g_DYOM_OBJECTIVE_DISTANCE, 100 * sizeof (uint32_t));

            ReadStrings (g_TEXTOBJECTIVES, 100);
            ReadBytes (g_DYOM_ACTOR_MODEL, 400 );
            ReadBytes (g_DYOM_ACTOR_GANG, 400 );
            ReadBytes (g_DYOM_ACTOR_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_LOCATION_A, 100 * sizeof (float));
            ReadBytes (g_DYOM_ACTOR_INTERIOR, 400 );
            ReadBytes (g_DYOM_ACTOR_WEAPON, 400 );
            ReadBytes (g_DYOM_ACTOR_AMMO, 400 );
            ReadBytes (g_DYOM_ACTOR_PROPERTIES, 100 * 4);
            ReadBytes (g_DYOM_ACTOR_HEALTH, 400 );
            ReadBytes (g_DYOM_ACTOR_ACCURACY, 400 );
            ReadBytes (g_DYOM_ACTOR_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_HIDEWITH, 400 );
            ReadBytes (g_DYOM_ACTOR_MUSTLIVE, 400 );
            ReadBytes (g_DYOM_ACTOR_ANIMATION, 400 );
            ReadBytes (g_DYOM_ACTOR_DISTANCE, 400 );

            ReadBytes (g_DYOM_CAR_MODEL, 200 );
            ReadBytes (g_DYOM_CAR_COLOR1, 200 );
            ReadBytes (g_DYOM_CAR_COLOR2, 200 );
            ReadBytes (g_DYOM_CAR_LOCATION_X, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Y, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_Z, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_LOCATION_A, 50 * sizeof (float));
            ReadBytes (g_DYOM_CAR_INTERIOR, 200 );
            ReadBytes (g_DYOM_CAR_HEALTH, 200 );
            ReadBytes (g_DYOM_CAR_PROPERTIES, 200 );
            ReadBytes (g_DYOM_CAR_SPAWNWITH, 200 );
            ReadBytes (g_DYOM_CAR_HIDEWITH, 200 );
            ReadBytes (g_DYOM_CAR_MUSTLIVE, 200 );

            ReadBytes (g_DYOM_PICKUP_MODEL, 200 );
            ReadBytes (g_DYOM_PICKUP_AMMO, 200 );
            ReadBytes (g_DYOM_PICKUP_MODE, 200 );
            ReadBytes (g_DYOM_PICKUP_LOCATION_X, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Y, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_LOCATION_Z, 50 * sizeof (float));
            ReadBytes (g_DYOM_PICKUP_SPAWNWITH, 200 );
            ReadBytes (g_DYOM_PICKUP_HIDEWITH, 200 );

            ReadBytes (g_DYOM_OBJECT_MODEL, 400 );
            ReadBytes (g_DYOM_OBJECT_LOCATION_X, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Y, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_Z, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RX, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RY, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_LOCATION_RZ, 100 * sizeof (float));
            ReadBytes (g_DYOM_OBJECT_INTERIOR, 400 );
            ReadBytes (g_DYOM_OBJECT_SPAWNWITH, 400 );
            ReadBytes (g_DYOM_OBJECT_HIDEWITH, 400 );
            ReadBytes (g_8277, 1600 );
            ReadBytes (g_8677, 1600 );
            ReadBytes (g_9077, 1600 );
            ReadBytes (g_9477, 1600 );
            break;
        }
};

void
DYOMFileStructure::Read (std::istream &file)
{
    ReadWriteInternal (
        file,
        [] (std::istream &file, char *data, int size) {
            file.read (data, size);
        },
        [] (std::istream &file, std::string &str) {
            std::getline (file, str, '\0');
        });
}

void
DYOMFileStructure::Save (std::ostream &file)
{
    ReadWriteInternal (
        file,
        [] (std::ostream &file, char *data, int size) {
            file.write (data, size);
        },

        [] (std::ostream &file, std::string &str) {
            file.write (str.data (), str.size ()+1);
        });
}

void
DYOMFileStructure::Read (const std::string &file)
{
    std::ifstream f(file, std::ios_base::in | std::ios_base::binary);
    Read (f);
}

void
DYOMFileStructure::Save (const std::string &file)
{
    std::ofstream f(file, std::ios_base::out | std::ios_base::binary);
    Save (f);
}

void
DYOMFileStructure::Read (const std::vector<uint8_t> &file)
{
    auto f = imemstream (reinterpret_cast<const char *> (file.data ()),
                         file.size ());
    Read (f);
}
} // namespace DYOM
