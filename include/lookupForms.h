#pragma once

namespace HailData {
    extern RE::SpellItem* hailSpell;
    extern RE::SpellItem* hailSpellSM;

    extern RE::EffectSetting* hailMagicEffect;
    extern RE::EffectSetting* hailMagicEffectSM;

    extern RE::BGSProjectile* smallHailP;
    extern RE::BGSProjectile* largeHailP;

    extern RE::TESGlobal* hailGlobal;

    extern RE::TESForm* activatorObject;

    extern RE::FormID tamrielFormID;
    extern RE::FormID solitudeWorld;
    extern RE::FormID markarthWorld;
    extern RE::FormID whiterunWorld;
    extern RE::FormID riftenWorld;

    extern std::vector<RE::FormID> appropriateWorldSpaces;

    extern RE::TESQuest* hailQuest;
    // extern RE::TESPackage* hailGoInnPackage;
    // extern RE::TESPackage* hailGoHomePackage;

    // extern RE::BGSListForm* HailGoHomeIgnoreFormList;
    //  extern RE::TESPackage* hailChildGoHomePackage;
    extern RE::TESTopic* hailTopic;

   //extern RE::BGSSoundDescriptor* indoorHailLP;  //

    extern RE::BGSKeyword* executionerKeyword;

    extern RE::TESObjectARMO* prisonerTunic;

    extern RE::TESFaction* currentFollowerFaction;

    extern RE::TESFaction* dragonPriestFaction;

    extern RE::TESFaction* creatureFaction;
    extern RE::TESFaction* preyFaction;
    extern RE::TESFaction* farmAnimalsFaction;

    extern std::vector<RE::TESFaction*> factions;

    void Initialize();
}