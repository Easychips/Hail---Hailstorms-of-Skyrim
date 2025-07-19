#include <functions.h>

namespace MyPlugin {

    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();

        SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
        return true;
    }
}