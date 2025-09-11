#include "HailMenu.h"
#include "configuration.h"
#include "skseMenuFramework.h"


struct OurEventSink : public RE::BSTEventSink<RE::InputEvent> {
    OurEventSink() = default;
    OurEventSink(const OurEventSink&&) = delete;
    OurEventSink& operator=(const OurEventSink&) = delete;
    OurEventSink& operator=(OurEventSink&&) = delete;

    static OurEventSink* GetSingleton() {
        static OurEventSink singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::InputEvent* event, RE::BSTEventSource<RE::InputEvent>*) {
        if (event->eventType != RE::INPUT_EVENT_TYPE::kButton) {
            return RE::BSEventNotifyControl::kContinue;
        }

        constexpr int openMenuKey = 72;  // 72 = H Key

        if (auto button = event->AsButtonEvent()) {
            if (button->GetIDCode() == openMenuKey) {
                if (button->IsDown()) {
                    UI::hailMenuWindow->IsOpen = !UI::hailMenuWindow->IsOpen;
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
};


void UI::Register(){ 

    SKSEMenuFramework::SetSection("Hail Menu"); 
    SKSEMenuFramework::AddSectionItem("Font Awesome", UI::Render);
    UI::hailMenuWindow = SKSEMenuFramework::AddWindow(UI::RenderWindow);

}

void __stdcall UI::Render() { 


    ImGui::InputText("string", Configuration::Example2::Buffer, 256);
    ImGui::ColorEdit4("Color", &Configuration::Example2::Color);
    float samples[100];
    for (int n = 0; n < 100; n++) samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
    //ImGui::PlotLines("Samples", static_cast<const float*>(samples), 100);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
   // ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++) ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();

}

void __stdcall UI::RenderWindow() {
    auto viewport = ImGui::GetMainViewport();

    auto center = ImGui::ImVec2Manager::Create();
    ImGui::ImGuiViewportManager::GetCenter(center, viewport);
    //ImGui::SetNextWindowPos(*center, ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
    ImGui::ImVec2Manager::Destroy(center);
    ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.4f, viewport->Size.y * 0.4f}, ImGuiCond_Appearing);
    ImGui::Begin("Hail Menu ##Hail - Hailstorms of Skyrim",nullptr, ImGuiWindowFlags_MenuBar); // If two mods have the same window name, and they open at the same time.
                                                                                         // The window content will be merged, is good practice to add ##ModName after the window name.
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
            }
            if (ImGui::MenuItem("Close", "Ctrl+W")) {
                UI::hailMenuWindow->IsOpen = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
   // if (ImGui::Button("Close Window")) {
    //    UI::hailMenuWindow->IsOpen = false;
   // }
    ImGui::End();
}
