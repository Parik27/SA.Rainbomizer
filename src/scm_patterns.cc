#include "scm_patterns.hh"
#include <sstream>
#include <cstdint>
#include "injector/calling.hpp"
#include "util/loader.hh"
#include "logger.hh"
#include "scm.hh"

bool
ScriptVehiclePattern::VehicleTypes::GetValue (uint32_t type)
{
    switch (type)
        {
        case VEHICLE_AUTOMOBILE: return Cars;
        case VEHICLE_BMX: return Bicycles;
        case VEHICLE_BIKE: return Bikes;
        case VEHICLE_BOAT: return Boats;
        case VEHICLE_HELI: return Helicopters;
        case VEHICLE_PLANE: return Planes;
        case VEHICLE_TRAILER: return Trailers;
        case VEHICLE_TRAIN: return Trains;
        case VEHICLE_QUAD: return Quadbikes;
        case VEHICLE_MTRUCK: return Cars;
        case VEHICLE_FHELI: return Helicopters;
        case VEHICLE_FPLANE: return Planes;
        default: return Cars;
        }
}

/*******************************************************/
void
CacheSeats ()
{
    for (int i = 0; i < 212; i++)
        {
            auto err = StreamingManager::AttemptToLoadVehicle (i + 400);
            if (err != ERR_FAILED)
                {
                    ScriptVehicleRandomizer::GetInstance ()->mSeatsCache[i]
                        = CModelInfo::
                            GetMaximumNumberOfPassengersFromNumberOfDoors (
                                i + 400);

                    eVehicleClass vehicleType = GetVehicleType (i + 400);
                    switch (vehicleType)
                        {
                        case VEHICLE_AUTOMOBILE:
                        case VEHICLE_MTRUCK:
                            ScriptVehicleRandomizer::carTypes[int (
                                                                  CarType::CAR)]
                                .push_back (i + 400);
                            break;

                        case VEHICLE_BIKE:
                        case VEHICLE_BMX:
                        case VEHICLE_QUAD:
                            ScriptVehicleRandomizer::carTypes
                                [int (CarType::BIKE)]
                                    .push_back (i + 400);
                            break;

                        case VEHICLE_PLANE:
                        case VEHICLE_FPLANE:
                            if ((i + 400) != 539)
                                ScriptVehicleRandomizer::carTypes
                                    [int (CarType::PLANE)]
                                        .push_back (i + 400);
                            break;

                        case VEHICLE_HELI:
                        case VEHICLE_FHELI:
                            ScriptVehicleRandomizer::carTypes
                                [int (CarType::HELI)]
                                    .push_back (i + 400);
                            break;

                        case VEHICLE_BOAT:
                            ScriptVehicleRandomizer::carTypes
                                [int (CarType::BOAT)]
                                    .push_back (i + 400);
                            break;

                        case VEHICLE_TRAIN:
                            ScriptVehicleRandomizer::carTypes
                                [int (CarType::TRAIN)]
                                    .push_back (i + 400);
                            break;

                        case VEHICLE_TRAILER:
                            ScriptVehicleRandomizer::carTypes
                                [int (CarType::TRAILER)]
                                    .push_back (i + 400);
                        }

                    if (err != ERR_ALREADY_LOADED)
                        CStreaming::RemoveModel (i + 400);
                    continue;
                }

            ScriptVehicleRandomizer::GetInstance ()->mSeatsCache[i]
                = 3; // fallback (safest)
            Logger::GetLogger ()->LogMessage ("Unable to cache seats for model "
                                              + std::to_string (i));
        }
}

/*******************************************************/
eVehicleClass
GetVehicleType (int vehID)
{
    if (CModelInfo::IsBikeModel (vehID))
        return VEHICLE_BIKE;
    else if (CModelInfo::IsBmxModel (vehID))
        return VEHICLE_BMX;
    else if (CModelInfo::IsBoatModel (vehID))
        return VEHICLE_BOAT;
    else if (CModelInfo::IsTrainModel (vehID))
        return VEHICLE_TRAIN;
    else if (CModelInfo::IsTrailerModel (vehID))
        return VEHICLE_TRAILER;
    else if (CModelInfo::IsQuadBikeModel (vehID))
        return VEHICLE_QUAD;
    else if (CModelInfo::IsMonsterTruckModel (vehID))
        return VEHICLE_MTRUCK;
    else if (CModelInfo::IsPlaneModel (vehID)
             || CModelInfo::IsFakePlaneModel (vehID))
        return VEHICLE_PLANE;
    else if (CModelInfo::IsHeliModel (vehID))
        return VEHICLE_HELI;
    else
        return VEHICLE_AUTOMOBILE;
}

/*******************************************************/
bool
ScriptVehiclePattern::DoesVehicleMatchPattern (int vehID)
{
    int numSeats
        = ScriptVehicleRandomizer::GetInstance ()->mSeatsCache[vehID - 400];

    // Seat check
    if (numSeats < m_nSeatCheck)
        return false;

    if (mFlags.Guns && vehID != 425 && vehID != 430 && vehID != 432
        && vehID != 447 && vehID != 464 && vehID != 476 && vehID != 520)
        return false;

    if (mFlags.RC && !CModelInfo::IsRCModel (vehID)
        && (CModelInfo::IsCarModel (vehID) || CModelInfo::IsHeliModel (vehID)
            || CModelInfo::IsPlaneModel (vehID)
            || CModelInfo::IsMonsterTruckModel (vehID)))
        return false;
    else if (mFlags.NoRC && CModelInfo::IsRCModel (vehID))
        return false;

    if ((mFlags.Smallplanes && CModelInfo::IsPlaneModel (vehID))
        && (vehID == 460 || vehID == 464 || vehID == 519 || vehID == 553
            || vehID == 577 || vehID == 592 || vehID == 511))
        return false;

    if ((mFlags.VTOL && CModelInfo::IsPlaneModel (vehID)) && vehID != 520)
        return false;

    if (mFlags.Float
        && (vehID == 406 || vehID == 444 || vehID == 556 || vehID == 557
            || vehID == 573
            || (CModelInfo::IsPlaneModel (vehID) && vehID != 539
                && vehID != 460)
            || (CModelInfo::IsHeliModel (vehID) && vehID != 447
                && vehID != 417)))
        return false;

    if (mFlags.Hovercraft && CModelInfo::IsPlaneModel (vehID) && vehID != 539)
        return false;
    else if (mFlags.NoHovercraft && vehID == 539)
        return false;

    if (mFlags.CanAttach && vehID != 435 && vehID != 450 && vehID != 584
        && vehID != 591 && vehID != 403 && vehID != 514 && vehID != 515)
        return false;

    if (mFlags.SmallCar
        && (CModelInfo::IsCarModel (vehID)
            || CModelInfo::IsMonsterTruckModel (vehID))
        && (vehID == 403 || vehID == 406 || vehID == 408 || vehID == 414
            || vehID == 431 || vehID == 432 || vehID == 433 || vehID == 437
            || vehID == 443 || vehID == 444 || vehID == 455 || vehID == 456
            || vehID == 486 || vehID == 514 || vehID == 515 || vehID == 524
            || vehID == 532 || vehID == 544 || vehID == 556 || vehID == 557
            || vehID == 578 || vehID == 588))
        return false;

    if (mFlags.SmallBoat && CModelInfo::IsBoatModel (vehID)
        && (vehID == 484 || vehID == 453 || vehID == 454))
        return false;

    if (mFlags.CarryObjects && vehID != 406 && vehID != 443 && vehID != 530)
        return false;

    if (mFlags.Spray && vehID != 407 && vehID != 601)
        return false;

    if (mFlags.NoTank && vehID == 432)
        return false;

    if (mFlags.NoWeirdDoors
        && (vehID == 425 || vehID == 431 || vehID == 437 || vehID == 432
            || vehID == 476 || vehID == 520))
        return false;

    if ((GetThreadName () == "zero2" || GetThreadName () == "zero5")
        && GetOriginalVehicle () == 464 && vehID == 520)
        return false;

    // Type check (it has to be both not moved and allowed)
    if (!mAllowedTypes.GetValue (GetVehicleType (vehID))
        && !mMovedTypes.GetValue (GetVehicleType (vehID)))
        return false;

    return true;
}

/*******************************************************/
void
ScriptVehiclePattern::Cache ()
{
    m_aCache.clear ();

    for (int i = 400; i < 612; i++)
        {
            if (i == m_nOriginalVehicle || DoesVehicleMatchPattern (i))
                m_aCache.push_back (i);
        }

    m_bCached = true;
}

/*******************************************************/
uint32_t
ScriptVehiclePattern::GetRandom (Vector3 &pos)
{
    if (!m_bCached)
        Cache ();

    int newVehID = GetRandomElement (m_aCache);

    if (mMovedTypes.GetValue (GetVehicleType (newVehID)))
        pos += GetMovedCoordinates ();

    return newVehID;
}

/*******************************************************/
bool
ScriptVehiclePattern::MatchVehicle (int vehID, std::string thread,
                                    const Vector3 &coords)
{
    if (vehID != GetOriginalVehicle () || thread != GetThreadName ())
        return false;

    // Coordinates check
    if (m_vecCoordsCheck.x && m_vecCoordsCheck.x != int (coords.x))
        return false;
    if (m_vecCoordsCheck.y && m_vecCoordsCheck.y != int (coords.y))
        return false;
    if (m_vecCoordsCheck.z && m_vecCoordsCheck.z != int (coords.z))
        return false;

    return true;
}

/*******************************************************/
void
ScriptVehiclePattern::ReadFlag (const std::string &flag)
{
    m_bCached = false;

    if (flag == "guns")
        mFlags.Guns = true;
    else if (flag == "rc")
        mFlags.RC = true;
    else if (flag == "norc")
        mFlags.NoRC = true;
    else if (flag == "smallplanes")
        mFlags.Smallplanes = true;
    else if (flag == "vtol")
        mFlags.VTOL = true;
    else if (flag == "canattach")
        mFlags.CanAttach = true;
    else if (flag == "float")
        mFlags.Float = true;
    else if (flag == "hovercraft")
        mFlags.Hovercraft = true;
    else if (flag == "nohovercraft")
        mFlags.NoHovercraft = true;
    else if (flag == "smallcar")
        mFlags.SmallCar = true;
    else if (flag == "smallboat")
        mFlags.SmallBoat = true;
    else if (flag == "carryobjects")
        mFlags.CarryObjects = true;
    else if (flag == "spray")
        mFlags.Spray = true;
    else if (flag == "notank")
        mFlags.NoTank = true;
    else if (flag == "standarddoor")
        mFlags.NoWeirdDoors = true;

    // Coordinates
    else if (flag.find ("x=") == 0)
        m_vecCoordsCheck.x = std::stoi (flag.substr (2));
    else if (flag.find ("y=") == 0)
        m_vecCoordsCheck.y = std::stoi (flag.substr (2));
    else if (flag.find ("z=") == 0)
        m_vecCoordsCheck.z = std::stoi (flag.substr (2));
}

/*******************************************************/
void
ScriptVehiclePattern::ParseFlags (const std::string &flags)
{
    std::istringstream flagStream (flags);
    std::string        flag = "";

    while (std::getline (flagStream, flag, '+'))
        ReadFlag (flag);
}