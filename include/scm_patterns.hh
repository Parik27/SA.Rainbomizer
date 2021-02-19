#pragma once

#include <CMath.hh>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "functions.hh"
#include "logger.hh"

void    CacheSeats ();

/*******************************************************/
/* Class to handle a single script vehicle pattern.    */
/*******************************************************/
class ScriptVehiclePattern
{
    struct VehicleTypes
    {
        bool Cars : 1;
        bool Bikes : 1;
        bool Bicycles : 1;
        bool Quadbikes : 1;
        bool Planes : 1;
        bool Helicopters : 1;
        bool Boats : 1;
        bool Trains : 1;
        bool Trailers : 1;

        bool GetValue (uint32_t type);

    } mAllowedTypes, mMovedTypes;

    struct
    {
        bool Guns : 1;
        bool RC : 1;
        bool NoRC : 1;
        bool Smallplanes : 1;
        bool VTOL : 1;
        bool CanAttach : 1;
        bool Float : 1;
        bool NoHovercraft : 1;
    } mFlags;

    Vector3        m_vecMovedCoords = {0.0, 0.0, 0.0};
    Vector3_t<int> m_vecCoordsCheck = {0, 0, 0};

    uint32_t m_nSeatCheck       = 0;
    uint32_t m_nOriginalVehicle = 0;

    std::string m_szThread = "";

    bool                  m_bCached = false;
    std::vector<uint32_t> m_aCache;

public:

        std::unordered_map<int, std::string> vehIDs = {
        {400, "landstal"},  {401, "bravura"},   {402, "buffalo"},   {403, "linerun"},   {404, "peren"},
        {405, "sentinel"},  {406, "dumper"},    {407, "firetruk"},  {408, "trash"},     {409, "stretch"}, 
        {410, "manana"},    {411, "infernus"},  {412, "voodoo"},    {413, "pony"},      {414, "mule"}, 
        {415, "cheetah"},   {416, "ambulan"},   {417, "leviathn"},  {418, "moonbeam"},  {419, "esperant"}, 
        {420, "taxi"},      {421, "washing"},   {422, "bobcat"},    {423, "mrwhoop"},   {424, "bfinject"}, 
        {425, "hunter"},    {426, "premier"},   {427, "enforcer"},  {428, "securica"},  {429, "banshee"}, 
        {430, "predator"},  {431, "bus"},       {432, "rhino"},     {433, "barracks"},  {434, "hotknife"}, 
        {435, "artict1"},   {436, "previon"},   {437, "coach"},     {438, "cabbie"},    {439, "stallion"}, 
        {440, "rumpo"},     {441, "rcbandit"},  {442, "romero"},    {443, "packer"},    {444, "monster"}, 
        {445, "admiral"},   {446, "squalo"},    {447, "seaspar"},   {448, "pizzaboy"},  {449, "tram"}, 
        {450, "artict2"},   {451, "turismo"},   {452, "speeder"},   {453, "reefer"},    {454, "tropic"}, 
        {455, "flatbed"},   {456, "yankee"},    {457, "caddy"},     {458, "solair"},    {459, "topfun"}, 
        {460, "skimmer"},   {461, "pcj600"},    {462, "faggio"},    {463, "freeway"},   {464, "rcbaron"}, 
        {465, "rcraider"},  {466, "glendale"},  {467, "oceanic"},   {468, "sanchez"},   {469, "sparrow"}, 
        {470, "patriot"},   {471, "quad"},      {472, "coastg"},    {473, "dinghy"},    {474, "hermes"}, 
        {475, "sabre"},     {476, "rustler"},   {477, "zr350"},     {478, "walton"},    {479, "regina"}, 
        {480, "comet"},     {481, "bmx"},       {482, "burrito"},   {483, "camper"},    {484, "marquis"}, 
        {485, "baggage"},   {486, "dozer"},     {487, "maverick"},  {488, "vcnmav"},    {489, "rancher"}, 
        {490, "fbiranch"},  {491, "virgo"},     {492, "greenwoo"},  {493, "jetmax"},    {494, "hotring"},
        {495, "sandking"},  {496, "blistac"},   {497, "polmav"},    {498, "boxville"},  {499, "benson"}, 
        {500, "mesa"},      {501, "rcgoblin"},  {502, "hotrina"},   {503, "hotrinb"},   {504, "bloodra"}, 
        {505, "rnchlure"},  {506, "supergt"},   {507, "elegant"},   {508, "journey"},   {509, "bike"}, 
        {510, "mtbike"},    {511, "beagle"},    {512, "cropdust"},  {513, "stunt"},     {514, "petro"}, 
        {515, "rdtrain"},   {516, "nebula"},    {517, "majestic"},  {518, "buccanee"},  {519, "shamal"}, 
        {520, "hydra"},     {521, "fcr900"},    {522, "nrg500"},    {523, "copbike"},   {524, "cement"}, 
        {525, "towtruck"},  {526, "fortune"},   {527, "cadrona"},   {528, "fbitruck"},  {529, "willard"}, 
        {530, "forklift"},  {531, "tractor"},   {532, "combine"},   {533, "feltzer"},   {534, "remingtn"}, 
        {535, "slamvan"},   {536, "blade"},     {537, "freight"},   {538, "streak"},    {539, "vortex"}, 
        {540, "vincent"},   {541, "bullet"},    {542, "clover"},    {543, "sadler"},    {544, "firela"}, 
        {545, "hustler"},   {546, "intruder"},  {547, "primo"},     {548, "cargobob"},  {549, "tampa"}, 
        {550, "sunrise"},   {551, "merit"},     {552, "utility"},   {553, "nevada"},    {554, "yosemite"}, 
        {555, "windsor"},   {556, "monstera"},  {557, "monsterb"},  {558, "uranus"},    {559, "jester"}, 
        {560, "sultan"},    {561, "stratum"},   {562, "elegy"},     {563, "raindanc"},  {564, "rctiger"}, 
        {565, "flash"},     {566, "tahoma"},    {567, "savanna"},   {568, "bandito"},   {569, "freiflat"}, 
        {570, "streakc"},   {571, "kart"},      {572, "mower"},     {573, "duneride"},  {574, "sweeper"}, 
        {575, "broadway"},  {576, "tornado"},   {577, "at400"},     {578, "dft30"},     {579, "huntley"}, 
        {580, "stafford"},  {581, "bf400"},     {582, "newsvan"},   {583, "tug"},       {584, "petrotr"}, 
        {585, "emperor"},   {586, "wayfarer"},  {587, "euros"},     {588, "hotdog"},    {589, "club"}, 
        {590, "freibox"},   {591, "artict3"},   {592, "androm"},    {593, "dodo"},      {594, "rccam"}, 
        {595, "launch"},    {596, "copcarla"},  {597, "copcarsf"},  {598, "copcarvg"},  {599, "copcarru"}, 
        {600, "picador"},   {601, "swatvan"},   {602, "alpha"},     {603, "phoenix"},   {604, "glenshit"}, 
        {605, "sadlshit"},  {606, "bagboxa"},   {607, "bagboxb"},   {608, "tugstair"},  {609, "boxburg"}, 
        {610, "farmtr1"},  {611, "utiltr1"}
    };
    
    /*******************************************************/
    void
    SetSeatsCheck (uint32_t seats)
    {
        m_bCached    = false;
        m_nSeatCheck = seats - 1;
    }
    void
    SetAllowedTypes (VehicleTypes types)
    {
        m_bCached     = false;
        mAllowedTypes = types;
    }
    void
    SetMovedTypes (VehicleTypes types)
    {
        m_bCached   = false;
        mMovedTypes = types;
    }

    void
    SetOriginalVehicle (char veh[64])
    {
        m_bCached          = false;
        for (auto& it : vehIDs)
        {
                if (it.second == veh)
                {
                    m_nOriginalVehicle = it.first;
                }
        }
    }

    void
    SetThreadName (char threadName[64])
    {
        m_bCached = false;
        m_szThread = threadName;
    }

    std::string
    GetThreadName () const
    {
        return m_szThread;
    }

    uint32_t
    GetOriginalVehicle () const
    {
        return m_nOriginalVehicle;
    }

    void
    SetMovedCoordinates (const Vector3 &moveCoords)
    {
        m_bCached        = false;
        m_vecMovedCoords = moveCoords;
    }
    const Vector3 &
    GetMovedCoordinates () const
    {
        return m_vecMovedCoords;
    }
    uint32_t
    GetNumVehicles () const
    {
        return static_cast<uint32_t> (m_aCache.size ());
    }

    // Determines the type of vehicle of current ID (car, bike, boat, plane, etc)
    eVehicleClass GetVehicleType (int vehID);

    // Returns if a vehicle matches a certain pattern
    bool DoesVehicleMatchPattern (int vehID);
    void Cache ();

    uint32_t GetRandom (Vector3 &pos);

    // Reads a flag string in the format "flag=value" or "flag" for just bools
    void ReadFlag (const std::string &flag);

    // Reads a list of flags delimited by a '+'
    void ParseFlags (const std::string &flags);

    bool MatchVehicle (int vehID, std::string thread, const Vector3 &coords);

    ScriptVehiclePattern () : mFlags{false} {}
};