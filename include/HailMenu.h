#pragma once
#include "SKSEMenuFramework.h"

namespace UI { 

    void Register(); 

    void __stdcall Render();

    void __stdcall RenderWindow();

    inline MENU_WINDOW hailMenuWindow;

    struct OurEventSink : public RE::BSTEventSink<RE::InputEvent> {
        OurEventSink() = default;
        OurEventSink(const OurEventSink&&) = delete;
        OurEventSink& operator=(const OurEventSink&) = delete;
        OurEventSink& operator=(OurEventSink&&) = delete;

        static OurEventSink* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::InputEvent* event,
                                              RE::BSTEventSource<RE::InputEvent>* dispatcher) override;
    };

};