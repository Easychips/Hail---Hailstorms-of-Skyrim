
#include "functions.h"
#include "global.h"
#include "lookupForms.h"
#include <chrono>
#include <random>
#include <thread>

namespace logger = SKSE::log;

const auto weatherCheckInterval = std::chrono::seconds(45);

void Hail() {
    logger::info("Hail initialized");


  SKSE::GetTaskInterface()->AddTask([]() {
        if (HailData::smallHailP) {
            HailData::smallHailP->data.gravity = g_SmallHailGravity;
            HailData::smallHailP->data.speed = g_SmallHailSpeed;
        } else {
            logger::warn("HailData::smallHailP is nullptr");
        }

        if (HailData::largeHailP) {
            HailData::largeHailP->data.gravity = g_LargeHailGravity;
            HailData::largeHailP->data.speed = g_LargeHailSpeed;
        } else {
            logger::warn("HailData::largeHailP is nullptr");
        }

        if (HailData::hailSpell && HailData::hailMagicEffect) {
            for (auto& effect : HailData::hailSpell->effects) {
                if (effect && effect->baseEffect == HailData::hailMagicEffect) {
                    effect->effectItem.magnitude = g_LargeHailDamageMultiplier;
                }
            }
        } else {
            logger::warn("Missing hailSpell or hailMagicEffect, cannot modify effects.");
        }

        if (HailData::hailSpellSM && HailData::hailMagicEffectSM) {
            for (auto& effect : HailData::hailSpellSM->effects) {
                if (effect && effect->baseEffect == HailData::hailMagicEffectSM) {
                    effect->effectItem.magnitude = g_LargeHailDamageMultiplier;
                }
            }
        } else {
            logger::warn("Missing hailSpellSM or hailMagicEffectSM, cannot modify effects.");
        }
   

     //   logger::info("b4 while loop");
    });

    while (true) {
      logger::info("while loop");

          if (HailData::hailGlobal) {
          HailData::hailGlobal->value = 0.0;
      }

        auto ui = RE::UI::GetSingleton();
        if (ui && ui->GameIsPaused()) {
            std::this_thread::sleep_for(std::chrono::seconds(15));  
            continue;
        }

            auto player1 = RE::PlayerCharacter::GetSingleton();

             auto sky = RE::Sky::GetSingleton();
            if (!sky) {
                logger::error("no sky");
                std::this_thread::sleep_for(std::chrono::seconds(weatherCheckInterval));
                continue;
            }

            if (!player1 || !IsPlayerInExteriorCell(player1) || !isLightning() ||
                !IsInAppropriateWorldspace(player1, HailData::appropriateWorldSpaces) || sky->IsSnowing()) {
                std::this_thread::sleep_for(std::chrono::seconds(weatherCheckInterval));
                continue; 
            }

              logger::info("Lightning detected! Starting hail storm...");


        if (RandomFloat() <= g_HailChance) {


              if (HailData::hailGlobal) {
                HailData::hailGlobal->value = 1.0;
            }

            auto activatorBaseObject = HailData::activatorObject->As<RE::TESBoundObject>();

            if (!activatorBaseObject) {
                logger::error("Failed to cast ActivatorObject to TESBoundObject");
                continue;
            }

            RE::TESObjectREFR* placedActivator = CreateActivator(player1, activatorBaseObject);

            int intensity = RandomFromThree(5, 7, 9);

            if (g_PerformanceMode == 1) {
                intensity = RandomFromThree(8, 10, 12);
            }

            logger::info("storm intensity =  {}", intensity);


       auto lastPackageTime =
                std::make_shared<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            auto var = std::make_shared<int>(3);



            while (isLightning()) {

                auto player2 = RE::PlayerCharacter::GetSingleton();

                if (!player2) {
                    logger::info("no player 2 ");
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }

              //    logger::info("in inner loop");
              
                // this mess is to deal with cgoing in interiors. I will fix this (on work time lol) 
                // while loop to wait while they inside, when player leaves spawn a new activator to keep it hailing. 
                
               

if (ui &&  (ui->IsItemMenuOpen() || ui->IsModalMenuOpen() || ui->IsMenuOpen("InventoryMenu") ||
                     ui->IsMenuOpen("MagicMenu") || ui->IsMenuOpen("DialogueMenu") || ui->IsMenuOpen("FavoritesMenu") ||
                     ui->IsMenuOpen("TweenMenu") || ui->IsMenuOpen("SleepWaitMenu") || ui->IsMenuOpen("StatsMenu") ||
                     ui->IsMenuOpen("Console") || ui->IsMenuOpen("Crafting Menu") || ui->IsMenuOpen("MessageBoxMenu") ||
                     ui->IsMenuOpen("LevelUpMenu") || ui->IsMenuOpen("MapMenu") || ui->IsMenuOpen("TutorialMenu"))) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }

                if (ui && ui->GameIsPaused()) { // game is paused when player transitions from whiterun to whiterun exterior for example this ensures it continues haililng.

                       if (!placedActivator) {
                        logger::warn("placed activator is null or doesent exist");
                    }

                    placedActivator->Disable();
                    placedActivator->SetDelete(true);

               //     logger::info("game is paused.");
                    while (ui->GameIsPaused()) {
                  //      logger::info("game is paused waiting");
                         std::this_thread::sleep_for(std::chrono::milliseconds(350));
                         continue;
                    }


                    placedActivator = CreateActivator(player2, activatorBaseObject);
                }

                 if (!IsPlayerInExteriorCell(player2)) {
                    logger::info("player is in interior");

                    if (!placedActivator) {
                        logger::warn("placed activator is null or doesent exist");
                    }

                    placedActivator->Disable();
                    placedActivator->SetDelete(true);

                    auto audioManager = RE::BSAudioManager::GetSingleton();
                    if (!audioManager) return;

                    RE::BSSoundHandle soundHandle;

                    const char* editorID = "HailSoundInteriorLP";

                    audioManager->BuildSoundDataFromEditorID(soundHandle, editorID, 0x1A);

                    if (soundHandle.IsValid()) {
                     //   logger::error("sound handle is valid start");

                        RE::NiPoint3 pos = player2->GetPosition();
                        soundHandle.SetPosition(pos);
                     //   logger::error("set pos");

                        RE::NiAVObject* playerNode = player2->Get3D();
                        if (playerNode) {
                        //    logger::error("set object");
                            soundHandle.SetObjectToFollow(playerNode);
                        }

                        else {
                            logger::error("no playernode");
                        }

                        //  soundHandle.SetVolume(1.0f);
                        soundHandle.Play();

                    } else {
                        logger::error("Failed to build or play sound from descriptor");
                    }

                    while (!IsPlayerInExteriorCell(player2)) {
                        //  logger::warn("player is inside waiting loop");

                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }

                    if (soundHandle.IsValid()) {
                        logger::info("sound hadnle is valid emd");
                        soundHandle.Stop();
                    }

                    else {
                        logger::info("sound hadnle isent valid end");
                    }

                    placedActivator = CreateActivator(player2, activatorBaseObject);
                }

                  auto now = std::chrono::steady_clock::now(); //////////////
                if (now - *lastPackageTime >= std::chrono::seconds(*var)) {

                    if (!alreadySaid) {
                        SayAOE(); 
                        alreadySaid = true; 
                    }

                      QuestMaintnence(); 
                    *lastPackageTime = now;
                    *var = 15;
                }


                  if (now - *lastPackageTime >= std::chrono::seconds(300)) {
                    logger::info("timeout reached");
                      return; 
                }

 
                if (!HailData::hailSpell ||!HailData::hailSpellSM) {
                    logger::info("no hail spells");
                    continue;
                }

          auto spellToCast = (RandomFloat() < 33.0f) ? HailData::hailSpell : HailData::hailSpellSM;  
                SKSE::GetTaskInterface()->AddTask([placedActivator, spellToCast, lastPackageTime, var]() {
                   

             if (!placedActivator || placedActivator->IsDisabled() || placedActivator->IsDeleted() ||
                        !spellToCast) {
                        logger::warn(" add task variable null ");
                        return;
                    }
                    
                    auto playerInner = RE::PlayerCharacter::GetSingleton();

                    if (!playerInner)
                    {
                        logger::info("no player inner");
                        return;  
                    }

                   //  logger::info("spell to cast  = {}", spellToCast->GetFullName());
                    MoveActivatorRandomly(playerInner, placedActivator);

                    auto caster = placedActivator->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);

                    if (!caster) {
                        logger::error("CastSpell: activator has no magic caster");
                        return;
                    }

                    //   logger::info("CastSpell: casting spell now");
                    caster->CastSpellImmediate(spellToCast, false, nullptr, 0, false, 0, nullptr);
                    //   logger::info("CastSpell: spell cast successfully"); */
                });  

                std::this_thread::sleep_for(std::chrono::milliseconds(intensity));
            } 
             alreadySaid = false; 
            SKSE::GetTaskInterface()->AddTask([placedActivator]() {

                 if (HailData::hailGlobal) {
                    HailData::hailGlobal->value = 0.0;
                }

                QuestMaintnence(); 

                if (!placedActivator) {
                    logger::warn("placed activator is null or doesent exist");
                    return;
                }

                placedActivator->Disable();
                placedActivator->SetDelete(true);
 
                
            });
        }
        std::this_thread::sleep_for(std::chrono::seconds(weatherCheckInterval));
    }
   
   
}




