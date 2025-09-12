#include <functions.h>
#include "HailMenu.h"

namespace MyPlugin {

    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();

        SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

        auto* eventSink = UI::OurEventSink::GetSingleton();
        auto* eventSourceHolder = RE::BSInputDeviceManager::GetSingleton();
        eventSourceHolder->AddEventSink<RE::InputEvent>(eventSink);

        UI::Register();

        return true;
    }
}