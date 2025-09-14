#include <functions.h>
#include "HailMenu.h"
#include "HookBuilder.h"



namespace MyPlugin {

    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();

        SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

        UI::Register(); 
        SKSE::AllocTrampoline(14);
        InstallHook();
   
    logger::info("installed hooks");
        return true;
    }
}