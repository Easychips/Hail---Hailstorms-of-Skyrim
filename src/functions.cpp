#pragma once

#include "functions.h"
#include "global.h"
#include "hail.h"
#include "lookupForms.h"


#include <spdlog/sinks/basic_file_sink.h>

namespace logger = SKSE::log;

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}

bool IsPlayerInExteriorCell(RE::PlayerCharacter* player) {
    if (!player) {
        logger::error("no player");
        return false;
    }
    auto cell = player->GetParentCell();
    if (!cell) {
        logger::error("no parent cell");
        return false;
    }
    return !cell->IsInteriorCell();
}

void PapyrusSay(RE::TESObjectREFR* target, RE::TESTopic* toSay, RE::Character* toSpeakAs, bool playerHead) {
    if (target && toSay) {
        using func_t = void (*)(std::int64_t, std::int64_t, RE::TESObjectREFR*, RE::TESTopic*, RE::Character*, bool);
        static REL::Relocation<func_t> func{REL::VariantID(
            55689, 56220, 0x9D1470)};  // takes SE, AE, then VR offset b/c VR might no exist they are filled manually
        return func(NULL, NULL, target, toSay, toSpeakAs, playerHead);
    }
}

bool IsInAppropriateWorldspace(RE::PlayerCharacter* player, const std::vector<RE::FormID>& appropriateWorldSpaces) {
    if (!player || appropriateWorldSpaces.empty()) {
        logger::error(" no player or world space list");
        return false;
    }

    auto* worldspace = player->GetWorldspace();
    if (!worldspace) {
        logger::error("no world space");
        return false;  // Probably in an interior cell
    }

    RE::FormID currentWorldspaceID = worldspace->GetFormID();

    for (RE::FormID appropriateID : appropriateWorldSpaces) {
        if (currentWorldspaceID == appropriateID) {
            return true;
        }
    }
    return false;
}


bool isLightning() {
    RE::Sky* singleton = RE::Sky::GetSingleton();
    if (!singleton) {
        logger::error("no singleton");
        return false;
    }

    auto weather = singleton->currentWeather;
    if (!weather) {
        logger::error("no weather");
        return false;
    }

    std::uint8_t lightningFreq = static_cast<std::uint8_t>(weather->data.thunderLightningFrequency);

    return lightningFreq < 255;
}

float RandomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int RandomFromThree(int a, int b, int c) {
    static std::mt19937 rng(std::random_device{}());
    int values[3] = {a, b, c};
    return values[std::uniform_int_distribution<int>(0, 2)(rng)];
}

RE::TESObjectREFR* CreateActivator(RE::TESObjectREFR* caster, RE::TESBoundObject* activatorBase) {
    if (!caster || !activatorBase) {
        logger::error("CreatePersistentActivator: Missing caster or base");
        return nullptr;
    }

    auto ref = caster->PlaceObjectAtMe(activatorBase, true);
    if (!ref) {
        logger::error("CreatePersistentActivator: Failed to place activator");
        return nullptr;
    }

    // logger::info("CreatePersistentActivator: Created activator at initial location");
    return ref.get();  // raw pointer for reuse
}

void MoveActivatorRandomly(RE::TESObjectREFR* player, RE::TESObjectREFR* activatorRef) {
    if (!player) {
        logger::error("MoveActivatorRandomly: Null pointer on input.");
        return;
    }

    const auto casterPos = player->GetPosition();
    /* if (!std::isfinite(casterPos.x) || !std::isfinite(casterPos.y) || !std::isfinite(casterPos.z)) {
        logger::error("MoveActivatorRandomly: Invalid caster position.");
        return false;
    }*/

    float posX = casterPos.x + RandomFloat(-fPOSRandom, fPOSRandom);
    float posY = casterPos.y + RandomFloat(-fPOSRandom, fPOSRandom);
    float posZ = casterPos.z + fHeight;

    if (!activatorRef) {
        logger::error("MoveActivatorRandomly: Null pointer on input.");
        return;
    }

    activatorRef->SetPosition(posX, posY, posZ);
}

void QuestMaintnence() {
    if (HailData::hailQuest) {
        if (HailData::hailQuest->IsRunning()) {
            logger::info("quest was running");
            HailData::hailQuest->Stop();
         //   HailData::hailQuest->Reset();
        }
        HailData::hailQuest->Start();
    }
}


void SayAOE() {
    logger::info("ApplyPackage started.");
 

    auto processLists = RE::ProcessLists::GetSingleton();

    auto playerProcess = RE::PlayerCharacter::GetSingleton();

    if (!playerProcess || !processLists || !HailData::hailTopic) {
        logger::error("One or more required HailData forms are missing or invalid or process or playerlists.");
        return;
    }

    for (auto& handle : processLists->highActorHandles) {
        auto actor = handle.get();
        if (!actor || !actor->Is3DLoaded() || actor->IsDead() || actor->IsDisabled() ||
            actor->IsDeleted() || HailData::factions.empty()) {
            continue;
        }

     bool isDQd = false;

        // Check factions
        for (auto faction : HailData::factions) {
            if (faction && actor->IsInFaction(faction)) {
                isDQd = true;
                break;
            }
        }

        if (isDQd) {
            continue;
        } 

        RE::NiPoint3 actorPos = actor->GetPosition();
        RE::NiPoint3 playerPos = playerProcess->GetPosition();

        float distance = actorPos.GetDistance(playerPos);

        if (distance >= 3000.0) {
         //   logger::info("player distance > then 3000 skipping.");
            continue;
        }


        auto actorAsObject = actor->As<RE::TESObjectREFR>();

        if (actorAsObject) {
      //      logger::info("check passed, trying to make npc say something");

          if (RandomFloat() >= 50.0) {
                PapyrusSay(actorAsObject, HailData::hailTopic, nullptr, false);
          }

          //  logger::info("Putting package inn on actor with package ptr:");
       
        }
    }

        // logger::info("ApplyPackage finished.");
}  



void IniParser()
{
    logger::info("In plugin load");

    std::ifstream iniFile("Data\\SKSE\\Plugins\\Hail.ini");
    if (!iniFile.is_open()) {
        logger::warn("INI file not found or failed to open, using defaults.");
    } else {
        std::string line;
        while (std::getline(iniFile, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty() || line[0] == ';') continue;

            try {
                if (line.starts_with("fHailChance=")) {
                    g_HailChance = std::stof(line.substr(strlen("fHailChance=")));
                    logger::info("INI override: fHailChance = {}", g_HailChance);
                } else if (line.starts_with("fHailDamageMultiplier=")) {
                    g_LargeHailDamageMultiplier = std::stof(line.substr(strlen("fHailDamageMultiplier=")));
                    logger::info("INI override: fHailDamageMultiplier = {}", g_LargeHailDamageMultiplier);
                } else if (line.starts_with("fLargeHailGravity=")) {
                    g_LargeHailGravity = std::stof(line.substr(strlen("fLargeHailGravity=")));
                    logger::info("INI override: fLargeHailGravity = {}", g_LargeHailGravity);
                } else if (line.starts_with("fLargeHailSpeed=")) {
                    g_LargeHailSpeed = std::stof(line.substr(strlen("fLargeHailSpeed=")));
                    logger::info("INI override: fLargeHailSpeed = {}", g_LargeHailSpeed);
                } else if (line.starts_with("fSmallHailSpeed=")) {
                    g_SmallHailSpeed = std::stof(line.substr(strlen("fSmallHailSpeed=")));
                    logger::info("INI override: fSmallHailSpeed = {}", g_SmallHailSpeed);
                } else if (line.starts_with("fSmallHailGravity=")) {
                    g_SmallHailGravity = std::stof(line.substr(strlen("fSmallHailGravity=")));
                    logger::info("INI override: fSmallHailGravity = {}", g_SmallHailGravity);
                } else if (line.starts_with("fHeightHailFallsAt=")) {
                    fHeight = std::stof(line.substr(strlen("fHeightHailFallsAt=")));
                    logger::info("INI override: fHeight = {}", fHeight);
                } else if (line.starts_with("bPerformanceMode=")) {
                    g_PerformanceMode = std::stoi(line.substr(strlen("bPerformanceMode=")));
                    logger::info("INI override: bPerformanceMode = {}", g_PerformanceMode);
                } else if (line.starts_with("fStormRadius=")) {
                    fPOSRandom = std::stof(line.substr(strlen("fStormRadius=")));
                    logger::info("INI override: fStormRadius = {}", fPOSRandom);
                }
            } catch (const std::exception& e) {
                logger::error("Failed to parse INI line [{}]: {}", line, e.what());
                // leave default value

                    
            }
           
        }
    }
    logger::info("performanc mode = {}", g_PerformanceMode);
 }


void StartPlayerLogicThread() {
     std::thread t(Hail);
     t.detach();
 }


void OnMessage(SKSE::MessagingInterface::Message* message) {
     if (message->type == SKSE::MessagingInterface::kDataLoaded) {
         //   auto dataHandler = RE::TESDataHandler::GetSingleton();
         //  auto lightningRodGlobal = dataHandler->LookupForm<RE::TESGlobal>(0x139, "Lightning Rod.esp");

         //  if (lightningRodGlobal) {
         //        logger::info("LightningRod Detected");
         //   }
         //    else
         //       {
         IniParser(); 
         HailData::Initialize();
         StartPlayerLogicThread();
         //   }

         //  logger::info("PostPostLoadAttemptingtodispatch");
         /* auto* messaging = SKSE::GetMessagingInterface();
         if (messaging) {
             float hailChanceCopy = g_HailChance;
             constexpr std::uint32_t kHailChanceMessage = 0x4321ABCD;
             messaging->Dispatch(kHailChanceMessage, &hailChanceCopy, sizeof(hailChanceCopy), nullptr);
         }*/
     }
 }
