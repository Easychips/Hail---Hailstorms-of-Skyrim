
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


  SKSE::GetTaskInterface()->AddTask([]() { // Im not sure what the consensus is on modifying game objects through a background thread is
                                            // so I add task to main thread
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
      logger::info("1st while loop start");

          if (HailData::hailGlobal) {
          HailData::hailGlobal->value = 0.0; // set global for for dialogue conditions ect
      }

        auto ui = RE::UI::GetSingleton(); // Idk if this is needed. intent was to wait if player was still at main menu by this point.
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
                /* !IsInAppropriateWorldspace(player1, HailData::appropriateWorldSpaces) ||*/ sky->IsSnowing()) {
                std::this_thread::sleep_for(std::chrono::seconds(weatherCheckInterval));
                continue; 
            }

        if (RandomFloat() <= g_HailChance) {

            logger::info("hail checks all cleared, starting to hail");


              if (HailData::hailGlobal) {
                HailData::hailGlobal->value = 1.0; // set the global
               }


            auto activatorBaseObject = HailData::activatorObject->As<RE::TESBoundObject>(); // this invisible activator casts hail spells

            if (!activatorBaseObject) {
                logger::error("Failed to cast ActivatorObject to TESBoundObject");
                continue;
            }

            RE::TESObjectREFR* placedActivator = CreateActivator(player1, activatorBaseObject);

            int intensity = RandomFromThree(5, 7, 9); //hail spells cast /ms

            if (g_PerformanceMode == 1) {
                intensity = RandomFromThree(8, 10, 12);
            }

            logger::info("storm intensity =  {}", intensity);


       auto lastPackageTime =
                std::make_shared<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
            auto var = std::make_shared<int>(3); // start a delayed timer to make npcs react to hail. Then every 12 seconds after that,
                                                //  reset the quest and fill aliases and give go home packages to npcs near player

            auto stormStartTime = std::chrono::steady_clock::now(); // this is for a timer on the hail storm itself

            while (isLightning()) {

                auto player2 = RE::PlayerCharacter::GetSingleton();

                if (!player2) {
                    logger::info("no player 2 ");
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }

              //    logger::info("in inner loop");
            
                
               // must pauses or it will continue to hail during menus and glitch out

if (ui &&  (ui->IsItemMenuOpen() || ui->IsModalMenuOpen() || ui->IsMenuOpen("InventoryMenu") ||
                     ui->IsMenuOpen("MagicMenu") || ui->IsMenuOpen("DialogueMenu") || ui->IsMenuOpen("FavoritesMenu") ||
                     ui->IsMenuOpen("TweenMenu") || ui->IsMenuOpen("SleepWaitMenu") || ui->IsMenuOpen("StatsMenu") ||
                     ui->IsMenuOpen("Console") || ui->IsMenuOpen("Crafting Menu") || ui->IsMenuOpen("MessageBoxMenu") ||
                     ui->IsMenuOpen("LevelUpMenu") || ui->IsMenuOpen("MapMenu") || ui->IsMenuOpen("TutorialMenu"))) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }    


                if (ui && ui->GameIsPaused()) { // game is paused when player transitions through loading screens. this ensures it continues haililng.

                       if (!placedActivator) {
                        logger::warn("placed activator is null or doesent exist");
                           continue; // added this not sure why wasent there
                    }

                    placedActivator->Disable();
                    placedActivator->SetDelete(true); // delete activator in old location

               //     logger::info("game is paused.");

                    while (ui->GameIsPaused()) {
                  //      logger::info("game is paused, waiting");
                         std::this_thread::sleep_for(std::chrono::milliseconds(350));
                         continue;
                    }

                    placedActivator = CreateActivator(player2, activatorBaseObject);  // create new one on other side of loadscreen.
                }

                 if (!IsPlayerInExteriorCell(player2)) { // check if player goes inside a interior, play indoor sound,
                                                        // attach said sound to player, stop palying sounds if player goes to far underground 
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
                     //   logger::info("sound handle is valid start");

                        RE::NiPoint3 pos = player2->GetPosition();
                        soundHandle.SetPosition(pos);
                     //   logger::info("set pos of sound handle");

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

                    auto playerZPosInitial = player2->GetPositionZ();  //get position of player to track if should stop playing indoor hail sounds


                    while (!IsPlayerInExteriorCell(player2)) {            // start loop to track player while inside. 
                        //  logger::info("player is inside waiting loop");
                      
                       auto interiorLoopNow = std::chrono::steady_clock::now();

                            if (interiorLoopNow - stormStartTime >= std::chrono::seconds(300)) {
                            logger::info("timeout reached");
                            break;
                        }

                        auto playerZPosInside = player2->GetPositionZ(); 

                        
                        if (playerZPosInitial - playerZPosInside >= 2000.0f) {  

                              if (soundHandle.IsValid()) {
                                logger::info("sound hadnle is valid emd");
                                soundHandle.Stop();
                            }
                        
                        }
                        
                        std::this_thread::sleep_for(std::chrono::seconds(1));
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
                        SayAOE();  // say hail reaction line short delay after begins hailing
                        alreadySaid = true; 
                    }

                     // (self note should add to main task?)

                      QuestMaintnence();  // reset quest to make npcs fill aliases and get go inside/ seek sheleter packages
                    *lastPackageTime = now; 
                    *var = 15;
                }

                  if (now - stormStartTime >= std::chrono::seconds(240)) { // stop hail after a few mins
                    logger::info("timeout reached");
                      break; 
                }
 
                if (!HailData::hailSpell ||!HailData::hailSpellSM) {
                    logger::info("no hail spells");
                    std::this_thread::sleep_for(std::chrono::seconds(1)); 
                    continue;
                }

          auto spellToCast = (RandomFloat() < 33.0f) ? HailData::hailSpell : HailData::hailSpellSM;  
                SKSE::GetTaskInterface()->AddTask([placedActivator, spellToCast, lastPackageTime, var]() { // add to main task 
                   

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

              //  QuestMaintnence(); 

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




