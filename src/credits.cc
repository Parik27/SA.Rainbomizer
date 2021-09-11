#include "credits.hh"
#include <string>
#include "base.hh"
#include "injector/calling.hpp"
#include <vector>
#include "config.hh"
#include "generalsettings.hh"

std::vector<std::string> lead_devs = {"Parik", "123robot", "GTA_Madman"};

std::vector<std::string> concept = {"NABN00B", "MrMateczko", "Veigar"};

std::vector<std::string> contribs = {"iguana", "SpeedyFolf", "SRewo"};

std::vector<std::string> beta_testers
    = {"Fryterp23", "Hugo_One", "Lordmau5", "Riekelt", "Waris"};

/*******************************************************/
void
PrintCreditText (float scaleX, float scaleY, const char *text, int *position,
                 float currentOffset, char bHighlighted)
{
    using f
        = injector::cstd<void (float, float, const char *, int *, float, char)>;
    HookManager::CallOriginal<f, 0x5A88AC> (scaleX, scaleY, text, position,
                                            currentOffset, bHighlighted);
}

/*******************************************************/
void
PrintGroup (const std::vector<std::string> &list, std::string title,
            int *position, float currentOffset)
{
    PrintCreditText (1.1, 1.1, title.c_str (), position, currentOffset, 1);
    for (auto i : list)
        {
            PrintCreditText (1.1, 1.1, i.c_str (), position, currentOffset, 0);
        }

    *position += 37.5;
}

/*******************************************************/
void
PrintCredits (float scaleX, float scaleY, char *text, int *position,
              float currentOffset, char bHighlighted)
{
    PrintCreditText (1.6, 1.6, "RAINBOMIZER", position, currentOffset, 1);
    *position += 37.5;

    PrintGroup (lead_devs, "Lead Developers", position, currentOffset);
    PrintGroup (concept, "Original Concept", position, currentOffset);
    PrintGroup (contribs, "Additional Contributors", position, currentOffset);
    PrintGroup (beta_testers, "Beta Testers", position, currentOffset);

    PrintCreditText (1.6, 1.6, text, position, currentOffset, bHighlighted);
}

/*******************************************************/
void
CreditsExtender::Initialise ()
{
    if (GeneralSettings::m_Config.ModifyCredits)
        RegisterHooks ({{HOOK_CALL, 0x5A88AC, (void *) PrintCredits}});
}
