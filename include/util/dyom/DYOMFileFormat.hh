#pragma once

#include <string>
#include <cstdint>
#include <fstream>
#include <vector>

namespace DYOM {
struct DYOMFileStructure
{
private:
    template <typename T, typename F1, typename F2>
    void ReadWriteInternal (T &file, F1 byte, F2 str);

public:
    // Header
    int32_t     g_DYOM_FILE_VERSION;
    std::string g_HEADERSTRINGS[6];

    // Dyom Block
    uint32_t g_DYOM_OBJECTIVE_COUNT = 0;
    uint32_t g_DYOM_ACTOR_COUNT     = 0;
    uint32_t g_DYOM_CAR_COUNT       = 0;
    uint32_t g_DYOM_PICKUP_COUNT    = 0;
    uint32_t g_DYOM_OBJECT_COUNT    = 0;
    uint32_t g_DYOM_TIMELIMIT       = 0;
    uint32_t g_DYOM_TIMEOFDAY       = 0;
    uint32_t g_DYOM_WEATHER         = 0;
    uint32_t g_DYOM_MINWANTEDLEVEL  = 0;
    uint32_t g_DYOM_MAXWANTEDLEVEL  = 0;
    uint32_t g_DYOM_PROPERTIES      = 0;

    // Player Block
    float    g_DYOM_PLAYER_LOCATION_X = 0;
    float    g_DYOM_PLAYER_LOCATION_Y = 0;
    float    g_DYOM_PLAYER_LOCATION_Z = 0;
    float    g_DYOM_PLAYER_LOCATION_A = 0;
    uint32_t g_DYOM_PLAYER_INTERIOR   = 0;
    uint32_t g_DYOM_PLAYER_MODEL      = 0;
    uint32_t g_DYOM_PLAYER_WEAPON     = 0;
    uint32_t g_DYOM_PLAYER_AMMO       = 0;
    uint32_t g_DYOM_PLAYER_HEALTH     = 0;

    // Objective Block
    float       g_DYOM_OBJECTIVE_LOCATION_X[100]  = {};
    float       g_DYOM_OBJECTIVE_LOCATION_Y[100]  = {};
    float       g_DYOM_OBJECTIVE_LOCATION_Z[100]  = {};
    float       g_DYOM_OBJECTIVE_LOCATION_A[100]  = {};
    uint32_t    g_DYOM_OBJECTIVE_INTERIOR[100]    = {};
    uint32_t    g_DYOM_OBJECTIVE_TYPE[100]        = {};
    int32_t     g_DYOM_OBJECTIVE_MODEL[100]       = {};
    uint32_t    g_DYOM_OBJECTIVE_DATA[100]        = {};
    uint32_t    g_DYOM_OBJECTIVE_AMMO[100]        = {};
    uint32_t    g_DYOM_OBJECTIVE_MARKERCOLOR[100] = {};
    uint32_t    g_DYOM_OBJECTIVE_PROPERTIES[100]  = {};
    uint32_t    g_DYOM_OBJECTIVE_HEALTH[100]      = {};
    uint32_t    g_DYOM_OBJECTIVE_ACCURACY[100]    = {};
    uint32_t    g_DYOM_OBJECTIVE_TIMELIMIT[100]   = {};
    uint32_t    g_DYOM_OBJECTIVE_ANIMATION[100]   = {};
    float       g_DYOM_OBJECTIVE_DISTANCE[100]    = {};
    std::string g_TEXTOBJECTIVES[100]             = {};

    // Actor Block
    uint32_t g_DYOM_ACTOR_MODEL[100]      = {};
    uint32_t g_DYOM_ACTOR_GANG[100]       = {};
    float    g_DYOM_ACTOR_LOCATION_X[100] = {};
    float    g_DYOM_ACTOR_LOCATION_Y[100] = {};
    float    g_DYOM_ACTOR_LOCATION_Z[100] = {};
    float    g_DYOM_ACTOR_LOCATION_A[100] = {};
    uint32_t g_DYOM_ACTOR_INTERIOR[100]   = {};
    uint32_t g_DYOM_ACTOR_WEAPON[100]     = {};
    uint32_t g_DYOM_ACTOR_AMMO[100]       = {};
    uint32_t g_DYOM_ACTOR_PROPERTIES[100] = {};
    uint32_t g_DYOM_ACTOR_HEALTH[100]     = {};
    uint32_t g_DYOM_ACTOR_ACCURACY[100]   = {};
    uint32_t g_DYOM_ACTOR_SPAWNWITH[100]  = {};
    uint32_t g_DYOM_ACTOR_HIDEWITH[100]   = {};
    uint32_t g_DYOM_ACTOR_MUSTLIVE[100]   = {};
    uint32_t g_DYOM_ACTOR_ANIMATION[100]  = {};
    float    g_DYOM_ACTOR_DISTANCE[100]   = {};

    // Car Block
    uint32_t g_DYOM_CAR_MODEL[50]      = {};
    uint32_t g_DYOM_CAR_COLOR1[50]     = {};
    uint32_t g_DYOM_CAR_COLOR2[50]     = {};
    float    g_DYOM_CAR_LOCATION_X[50] = {};
    float    g_DYOM_CAR_LOCATION_Y[50] = {};
    float    g_DYOM_CAR_LOCATION_Z[50] = {};
    float    g_DYOM_CAR_LOCATION_A[50] = {};
    uint32_t g_DYOM_CAR_INTERIOR[50]   = {};
    uint32_t g_DYOM_CAR_HEALTH[50]     = {};
    uint32_t g_DYOM_CAR_PROPERTIES[50] = {};
    uint32_t g_DYOM_CAR_SPAWNWITH[50]  = {};
    uint32_t g_DYOM_CAR_HIDEWITH[50]   = {};
    uint32_t g_DYOM_CAR_MUSTLIVE[50]   = {};

    // Pickup Block
    uint32_t g_DYOM_PICKUP_MODEL[50]      = {};
    uint32_t g_DYOM_PICKUP_AMMO[50]       = {};
    uint32_t g_DYOM_PICKUP_MODE[50]       = {};
    float    g_DYOM_PICKUP_LOCATION_X[50] = {};
    float    g_DYOM_PICKUP_LOCATION_Y[50] = {};
    float    g_DYOM_PICKUP_LOCATION_Z[50] = {};
    uint32_t g_DYOM_PICKUP_SPAWNWITH[50]  = {};
    uint32_t g_DYOM_PICKUP_HIDEWITH[50]   = {};

    // Object Block
    uint32_t g_DYOM_OBJECT_MODEL[100]       = {};
    float    g_DYOM_OBJECT_LOCATION_X[100]  = {};
    float    g_DYOM_OBJECT_LOCATION_Y[100]  = {};
    float    g_DYOM_OBJECT_LOCATION_Z[100]  = {};
    float    g_DYOM_OBJECT_LOCATION_RX[100] = {};
    float    g_DYOM_OBJECT_LOCATION_RY[100] = {};
    float    g_DYOM_OBJECT_LOCATION_RZ[100] = {};
    uint32_t g_DYOM_OBJECT_INTERIOR[100]    = {};
    uint32_t g_DYOM_OBJECT_SPAWNWITH[100]   = {};
    uint32_t g_DYOM_OBJECT_HIDEWITH[100]    = {};

    // Routepoints Block
    uint8_t g_8277[1600] = {};
    uint8_t g_8677[1600] = {};
    uint8_t g_9077[1600] = {};
    uint8_t g_9477[1600] = {};

    void Save (std::ostream &file);
    void Read (std::istream &file);

    void Read (const std::string &file);
    void Save (const std::string &file);

    void Read (const std::vector<uint8_t> &bytes);
};
}; // namespace DYOM
