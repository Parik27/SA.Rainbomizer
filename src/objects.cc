#include "objects.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"

ObjectsRandomizer *ObjectsRandomizer::mInstance = nullptr;

/*******************************************************/
void __fastcall RandomizeObjectIndices (CRunningScript *script, void *edx,
                                        short count)
{
    script->CollectParameters (count);
    if (ScriptParams[0] < 0)
        {
            int              tempObjectCheck = -(ScriptParams[0]);
            std::vector<int> badObjects
                = ObjectsRandomizer::GetInstance ()->objectsToIgnore;

            // Check list of objects we don't want to change
            if (std::find (badObjects.begin (), badObjects.end (),
                           tempObjectCheck)
                == badObjects.end ())
                {
                    std::vector<int> objectsToUse;

                    // Checks if should use generic good object list or
                    // destructible list
                    if (tempObjectCheck == 75 || tempObjectCheck == 123
                        || tempObjectCheck == 198 || tempObjectCheck == 298
                        || (tempObjectCheck == 140
                            && (CRunningScripts::CheckForRunningScript (
                                    "drugs3")
                                || CRunningScripts::CheckForRunningScript (
                                    "ryder2"))))
                        {
                            objectsToUse = ObjectsRandomizer::GetInstance ()
                                               ->destructibleObjects;
                            ObjectsRandomizer::modifyObject = true;
                        }
                    else
                        {
                            objectsToUse = ObjectsRandomizer::GetInstance ()
                                               ->goodObjects;
                        }

                    // Adds original object to list if not present
                    if (std::find (objectsToUse.begin (), objectsToUse.end (),
                                   tempObjectCheck)
                        == badObjects.end ())
                        {
                            objectsToUse.push_back (tempObjectCheck);
                        }

                    // Selects random object from the final vector
                    ScriptParams[0] = -(GetRandomElement (objectsToUse));
                }
            // ScriptParams[0] = -random (*((int *) 0xA44B6C)); // Original code
        }
}

// Directly modifies the data of the created object to change its properties.
// Values still need to be worked out.
/*******************************************************/
CObject *
ChangeObjectData (int modelId)
{
    CObject *obj = CObject::Create (modelId);
    if (ObjectsRandomizer::modifyObject)
    {
        //    obj->m_pObjectInfo->m_fMass              = 50.0f;
        //obj->m_pObjectInfo->m_fTurnMass              = 150.0f;
        //    obj->m_pObjectInfo->m_fAirResistance     = 0.99f;
        //obj->m_pObjectInfo->m_fElasticity            = 0.0f;
        //    obj->m_pObjectInfo->m_fUprootLimit       = 350.0f;
        //obj->m_pObjectInfo->m_fColDamageMultiplier   = 5.0f;

            obj->m_pObjectInfo->m_bColDamageEffect = 200;
        obj->m_pObjectInfo->m_fSmashMultiplier = 1.0f;
            obj->m_pObjectInfo->m_vBreakVelocity   = {0.0f, 0.0f, 0.1f};
        obj->m_pObjectInfo->m_fBreakVelocityRand   = 0.07f;
            obj->m_pObjectInfo->m_dwGunBreakMode     = 2;
        obj->m_pObjectInfo->m_dwSparksOnImpact     = 0;
        obj->m_fHealth                         = 1.0f;
        ObjectsRandomizer::modifyObject        = false;
    }
    return obj;
}

/*******************************************************/
void
ObjectsRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ObjectRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x469773, (void *) RandomizeObjectIndices}/*,
                    {HOOK_CALL, 0x46979B, (void *) ChangeObjectData}*/});
    Logger::GetLogger ()->LogMessage ("Intialised ObjectsRandomizer");
}

/*******************************************************/
void
ObjectsRandomizer::DestroyInstance ()
{
    if (ObjectsRandomizer::mInstance)
        delete ObjectsRandomizer::mInstance;
}

/*******************************************************/
ObjectsRandomizer *
ObjectsRandomizer::GetInstance ()
{
    if (!ObjectsRandomizer::mInstance)
        {
            ObjectsRandomizer::mInstance = new ObjectsRandomizer ();
            atexit (&ObjectsRandomizer::DestroyInstance);
        }
    return ObjectsRandomizer::mInstance;
}
