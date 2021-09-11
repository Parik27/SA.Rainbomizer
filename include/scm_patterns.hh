#pragma once

#include <CMath.hh>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "functions.hh"
#include "logger.hh"

void CacheSeats ();

// Determines the type of vehicle of current ID (car, bike, boat, plane, etc)
eVehicleClass GetVehicleType (int vehID);

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
        bool Hovercraft : 1;
        bool NoHovercraft : 1;
        bool SmallCar : 1;
        bool SmallBoat : 1;
        bool CarryObjects : 1;
        bool Spray : 1;
        bool NoTank : 1;
        bool NoWeirdDoors : 1;
    } mFlags;

    Vector3        m_vecMovedCoords = {0.0, 0.0, 0.0};
    Vector3_t<int> m_vecCoordsCheck = {0, 0, 0};

    uint32_t m_nSeatCheck       = 0;
    uint32_t m_nOriginalVehicle = 0;

    std::string m_szThread = "";

    bool                  m_bCached = false;
    std::vector<uint32_t> m_aCache;

public:
    std::unordered_map<std::string, int> vehIDs
        = {{"landstal", 400}, {"bravura", 401},  {"buffalo", 402},
           {"linerun", 403},  {"peren", 404},    {"sentinel", 405},
           {"dumper", 406},   {"firetruk", 407}, {"trash", 408},
           {"stretch", 409},  {"manana", 410},   {"infernus", 411},
           {"voodoo", 412},   {"pony", 413},     {"mule", 414},
           {"cheetah", 415},  {"ambulan", 416},  {"leviathn", 417},
           {"moonbeam", 418}, {"esperant", 419}, {"taxi", 420},
           {"washing", 421},  {"bobcat", 422},   {"mrwhoop", 423},
           {"bfinject", 424}, {"hunter", 425},   {"premier", 426},
           {"enforcer", 427}, {"securica", 428}, {"banshee", 429},
           {"predator", 430}, {"bus", 431},      {"rhino", 432},
           {"barracks", 433}, {"hotknife", 434}, {"artict1", 435},
           {"previon", 436},  {"coach", 437},    {"cabbie", 438},
           {"stallion", 439}, {"rumpo", 440},    {"rcbandit", 441},
           {"romero", 442},   {"packer", 443},   {"monster", 444},
           {"admiral", 445},  {"squalo", 446},   {"seaspar", 447},
           {"pizzaboy", 448}, {"tram", 449},     {"artict2", 450},
           {"turismo", 451},  {"speeder", 452},  {"reefer", 453},
           {"tropic", 454},   {"flatbed", 455},  {"yankee", 456},
           {"caddy", 457},    {"solair", 458},   {"topfun", 459},
           {"skimmer", 460},  {"pcj600", 461},   {"faggio", 462},
           {"freeway", 463},  {"rcbaron", 464},  {"rcraider", 465},
           {"glendale", 466}, {"oceanic", 467},  {"sanchez", 468},
           {"sparrow", 469},  {"patriot", 470},  {"quad", 471},
           {"coastg", 472},   {"dinghy", 473},   {"hermes", 474},
           {"sabre", 475},    {"rustler", 476},  {"zr350", 477},
           {"walton", 478},   {"regina", 479},   {"comet", 480},
           {"bmx", 481},      {"burrito", 482},  {"camper", 483},
           {"marquis", 484},  {"baggage", 485},  {"dozer", 486},
           {"maverick", 487}, {"vcnmav", 488},   {"rancher", 489},
           {"fbiranch", 490}, {"virgo", 491},    {"greenwoo", 492},
           {"jetmax", 493},   {"hotring", 494},  {"sandking", 495},
           {"blistac", 496},  {"polmav", 497},   {"boxville", 498},
           {"benson", 499},   {"mesa", 500},     {"rcgoblin", 501},
           {"hotrina", 502},  {"hotrinb", 503},  {"bloodra", 504},
           {"rnchlure", 505}, {"supergt", 506},  {"elegant", 507},
           {"journey", 508},  {"bike", 509},     {"mtbike", 510},
           {"beagle", 511},   {"cropdust", 512}, {"stunt", 513},
           {"petro", 514},    {"rdtrain", 515},  {"nebula", 516},
           {"majestic", 517}, {"buccanee", 518}, {"shamal", 519},
           {"hydra", 520},    {"fcr900", 521},   {"nrg500", 522},
           {"copbike", 523},  {"cement", 524},   {"towtruck", 525},
           {"fortune", 526},  {"cadrona", 527},  {"fbitruck", 528},
           {"willard", 529},  {"forklift", 530}, {"tractor", 531},
           {"combine", 532},  {"feltzer", 533},  {"remingtn", 534},
           {"slamvan", 535},  {"blade", 536},    {"freight", 537},
           {"streak", 538},   {"vortex", 539},   {"vincent", 540},
           {"bullet", 541},   {"clover", 542},   {"sadler", 543},
           {"firela", 544},   {"hustler", 545},  {"intruder", 546},
           {"primo", 547},    {"cargobob", 548}, {"tampa", 549},
           {"sunrise", 550},  {"merit", 551},    {"utility", 552},
           {"nevada", 553},   {"yosemite", 554}, {"windsor", 555},
           {"monstera", 556}, {"monsterb", 557}, {"uranus", 558},
           {"jester", 559},   {"sultan", 560},   {"stratum", 561},
           {"elegy", 562},    {"raindanc", 563}, {"rctiger", 564},
           {"flash", 565},    {"tahoma", 566},   {"savanna", 567},
           {"bandito", 568},  {"freiflat", 569}, {"streakc", 570},
           {"kart", 571},     {"mower", 572},    {"duneride", 573},
           {"sweeper", 574},  {"broadway", 575}, {"tornado", 576},
           {"at400", 577},    {"dft30", 578},    {"huntley", 579},
           {"stafford", 580}, {"bf400", 581},    {"newsvan", 582},
           {"tug", 583},      {"petrotr", 584},  {"emperor", 585},
           {"wayfarer", 586}, {"euros", 587},    {"hotdog", 588},
           {"club", 589},     {"freibox", 590},  {"artict3", 591},
           {"androm", 592},   {"dodo", 593},     {"rccam", 594},
           {"launch", 595},   {"copcarla", 596}, {"copcarsf", 597},
           {"copcarvg", 598}, {"copcarru", 599}, {"picador", 600},
           {"swatvan", 601},  {"alpha", 602},    {"phoenix", 603},
           {"glenshit", 604}, {"sadlshit", 605}, {"bagboxa", 606},
           {"bagboxb", 607},  {"tugstair", 608}, {"boxburg", 609},
           {"farmtr1", 610},  {"utiltr1", 611}

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
    SetOriginalVehicle (char *veh)
    {
        m_bCached          = false;
        m_nOriginalVehicle = vehIDs[veh]; // steamhappy
    }

    void
    SetThreadName (char threadName[64])
    {
        m_bCached  = false;
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

    // Returns if a vehicle matches a certain pattern
    bool DoesVehicleMatchPattern (int vehID);
    void Cache ();

    uint32_t GetRandom (Vector3 &pos);

    // Reads a flag string in the format "flag=value" or "flag" for just bools
    void ReadFlag (const std::string &flag);

    // Reads a list of flags delimited by a '+'
    void ParseFlags (const std::string &flags);

    bool MatchVehicle (int vehID, std::string thread, const Vector3 &coords);

    ScriptVehiclePattern ()
        : mFlags{false, false, false, false, false, false, false, false,
                 false, false, false, false, false, false, false}
    {
    }
};
