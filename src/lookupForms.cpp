#include "lookupForms.h"
#include "spdlog/spdlog.h"

namespace HailData {
    RE::SpellItem* hailSpell = nullptr;
    RE::SpellItem* hailSpellSM = nullptr;

    RE::EffectSetting* hailMagicEffect = nullptr;
    RE::EffectSetting* hailMagicEffectSM = nullptr;

    RE::BGSProjectile* smallHailP = nullptr;
    RE::BGSProjectile* largeHailP = nullptr;

    RE::TESGlobal* hailGlobal = nullptr;

    RE::TESQuest* hailQuest = nullptr;
   // RE::TESPackage* hailGoInnPackage = nullptr;
   // RE::TESPackage* hailGoHomePackage = nullptr;
   // RE::TESPackage* hailChildGoHomePackage = nullptr;
    RE::TESForm* activatorObject = nullptr;

// RE::BGSListForm* HailGoHomeIgnoreFormList = nullptr;

    RE::FormID tamrielFormID = 0x0000003C;
    RE::FormID solitudeWorld = 0x00037EDF;
    RE::FormID markarthWorld = 0x00016D71;
    RE::FormID whiterunWorld = 0x0001A26F;
    RE::FormID riftenWorld = 0x00016BB4;

    std::vector<RE::FormID> appropriateWorldSpaces = {tamrielFormID, solitudeWorld, markarthWorld, whiterunWorld,
                                                      riftenWorld};

    RE::TESTopic* hailTopic = nullptr;



    RE::TESFaction* currentFollowerFaction = nullptr;

    RE::TESFaction* winNeverFillAliasesFaction = nullptr;
    RE::TESFaction* dragonPriestFaction = nullptr;
  
    RE::TESFaction* creatureFaction = nullptr;
    RE::TESFaction* preyFaction = nullptr;
    RE::TESFaction* farmAnimalsFaction = nullptr;

   // RE::BGSSoundDescriptor* indoorHailLP = nullptr;  // 

    std::vector<RE::TESFaction*> factions;

    void Initialize() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();
        hailSpell = dataHandler->LookupForm<RE::SpellItem>(0x80A, "Hail.esp");
        hailSpellSM = dataHandler->LookupForm<RE::SpellItem>(0xD74, "Hail.esp");

        hailMagicEffect = dataHandler->LookupForm<RE::EffectSetting>(0x804, "Hail.esp");
        hailMagicEffectSM = dataHandler->LookupForm<RE::EffectSetting>(0xD76, "Hail.esp");

        smallHailP = dataHandler->LookupForm<RE::BGSProjectile>(0x805, "Hail.esp");
        largeHailP = dataHandler->LookupForm<RE::BGSProjectile>(0x80B, "Hail.esp");

        hailGlobal = dataHandler->LookupForm<RE::TESGlobal>(0x8D4, "Hail.esp");

        hailQuest = dataHandler->LookupForm<RE::TESQuest>(0xD7D, "Hail.esp");

       //  indoorHailLP = dataHandler->LookupForm<RE::BGSSoundDescriptor>(0x42D, "Hail.esp"); //

      //  hailGoHomePackage = dataHandler->LookupForm<RE::TESPackage>(0xD78, "Hail.esp");
      //  hailChildGoHomePackage = dataHandler->LookupForm<RE::TESPackage>(0xD79, "Hail.esp");
      //  hailGoInnPackage = dataHandler->LookupForm<RE::TESPackage>(0xD7B, "Hail.esp");
        //HailGoHomeIgnoreFormList = dataHandler->LookupForm<RE::BGSListForm>(0xD7F, "Hail.esp");

        hailTopic  = RE::TESForm::LookupByID<RE::TESTopic>(0x00011111);
     

        activatorObject = RE::TESForm::LookupByEditorID("HailActivator");

        currentFollowerFaction = dataHandler->LookupForm<RE::TESFaction>(0x0005C84E, "Skyrim.esm");
        winNeverFillAliasesFaction = dataHandler->LookupForm<RE::TESFaction>(0x000ABDAE, "Skyrim.esm");
        dragonPriestFaction = dataHandler->LookupForm<RE::TESFaction>(0x00106643, "Skyrim.esm");

        creatureFaction = dataHandler->LookupForm<RE::TESFaction>(0x00000013, "Skyrim.esm");
        preyFaction = dataHandler->LookupForm<RE::TESFaction>(0x0002E894, "Skyrim.esm");
        farmAnimalsFaction = dataHandler->LookupForm<RE::TESFaction>(0x0004E849, "Skyrim.esm");
        
        //currentFollowerFaction,
        
        factions = {
                    dragonPriestFaction,
                    creatureFaction,
                    preyFaction,
                    farmAnimalsFaction, currentFollowerFaction};

        if (!hailSpell || !hailSpellSM || !hailMagicEffect || !hailMagicEffectSM || !smallHailP || !largeHailP ||
            !hailGlobal || !activatorObject || !dragonPriestFaction || !hailTopic || !currentFollowerFaction)
        {
            spdlog::error("Failed to load essential Hail and related forms!");
        } else {
            spdlog::info("All essential forms loaded successfully");
        }
    }

}

// !HailGoHomeIgnoreFormList || !currentFollowerFaction