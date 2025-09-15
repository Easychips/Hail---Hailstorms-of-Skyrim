#include "HailMenu.h"
#include "configuration.h"
#include "global.h"
namespace logger = SKSE::log; //gotta add this for logger usage

namespace UI {

    void UI::Register() {
        if (!SKSEMenuFramework::IsInstalled()) {
            logger::info("skseMenuframework is not installed");
            return;
        }

        logger::info("Register Called");
        //  SKSEMenuFramework::SetSection("Hail Menu");
        //  SKSEMenuFramework::AddSectionItem("Font Awesome", UI::Render);
        UI::hailMenuWindow = SKSEMenuFramework::AddWindow(UI::RenderWindow);
    }

    void __stdcall UI::Render() {
       // logger::info("Render() called");
        ImGui::InputText("string", Configuration::Example2::Buffer, 256);
        ImGui::ColorEdit4("Color", &Configuration::Example2::Color);
        double samples[100];
        for (int n = 0; n < 100; n++) samples[n] = sin(n * 0.2 + ImGui::GetTime() * 1.5);
        // ImGui::PlotLines("Samples", static_cast<const float*>(samples), 100);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        // ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++) ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
    }

void __stdcall UI::RenderWindow() {
        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.4f, viewport->Size.y * 0.4f}, ImGuiCond_Appearing);

        // Use hidden internal title for ImGui tracking
        ImGui::Begin("##HailMenu", nullptr, ImGuiWindowFlags_NoTitleBar);

       
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));  // baby blue
        FontAwesome::PushSolid();                                                 // style option
        ImGui::Text("%s Hail Menu", FontAwesome::UnicodeToUtf8(0xf2dc).c_str());  // this is the frost icon in the hail menu
        FontAwesome::Pop();                                                       // restores format we set earlier with pushsolid
        ImGui::PopStyleColor();  //change color back 
        ImGui::Separator();                                                       // Optional separation from content

        // --- Menu bar ---
        /* if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Close")) UI::hailMenuWindow->IsOpen = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        } */

         if (ImGui::Button("Close Window")) UI::hailMenuWindow->IsOpen = false;

            ImGui::SameLine();

            if (ImGui::Button("Save")) {
                SaveSettingsToIni(); 

                if (ImGui::IsItemHovered()){
                    ImGui::SetTooltip("Save Settings to Hail.ini file")
                }
            }

          if (ImGui::Checkbox("Performance Mode", &g_PerformanceMode));

           if (ImGui::SliderFloat("Hail Chance", &g_HailChance, 0.0f, 100.0f, %.1f%%)) {
            logger::info("Hail Chance changed to {}", g_HailChance);
              if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("CTRL+ click to type value");
            }
            
        }

        if (ImGui::SliderFloat("Hail Damage Multiplier", &g_LargeHailDamageMultiplier, 0.0f, 10.0f)) {
            if (HailData::hailSpell) {
                for (auto& effect : HailData::hailSpell->effects) {
                    if (effect && effect->baseEffect == HailData::hailMagicEffect)
                        effect->effectItem.magnitude = g_LargeHailDamageMultiplier;
                }
            }
            if (HailData::hailSpellSM) {
                for (auto& effect : HailData::hailSpellSM->effects) {
                    if (effect && effect->baseEffect == HailData::hailMagicEffectSM)
                        effect->effectItem.magnitude = g_LargeHailDamageMultiplier;
                }
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Damage of the hailstones, 4.0 = 5 damage per hit");
        }

    }

        if (ImGui::CollapsingHeader("Advanced Settings", ImGuiTreeNodeFlags_None)) {
     
if (ImGui::SliderFloat("Hail Start Height", &fHeightHailFallsAt, 0.0f, 10000.0f, "%.1f")) {
    if (HailData::largeHailP) HailData::largeHailP->data.startHeight = fHeightHailFallsAt; // or whatever field stores start height
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Height hail falls from (in-game units), default 3800.0");
    }
}


if (ImGui::SliderFloat("Storm Radius", &fStormRadius, 0.0f, 10000.0f, "%.1f")) {
    if (HailData::stormP) HailData::stormP->data.radius = fStormRadius; // or whatever field stores storm radius
        if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Radius of the hail storm (in-game units), default 2800.0");
        }
    }
   
        if (ImGui::SliderFloat("Small Hail Speed", &g_SmallHailSpeed, 0.0f, 2000.0f, %.1f)) { //  &g_SmallHailSpeed, == pointer to the value  ie its memory address
            if (HailData::smallHailP) HailData::smallHailP->data.speed = g_SmallHailSpeed;
              if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("velocity of the hail stone, CTRL+ click to type value");
            }
              
        }
        if (ImGui::SliderFloat("Small Hail Gravity", &g_SmallHailGravity, 0.0f, 5.0f, %.1f)) {
            if (HailData::smallHailP) HailData::smallHailP->data.gravity = g_SmallHailGravity;
              if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("gravity of the hail stone, CTRL+ click to type value");
            }
        }

        if (ImGui::SliderFloat("Large Hail Speed", &g_LargeHailSpeed, 0.0f, 2000.0f, %.1f)) {
            if (HailData::largeHailP) HailData::largeHailP->data.speed = g_LargeHailSpeed;
              if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("velocity of the hail stone, CTRL+ click to type value");
            }
        }
        if (ImGui::SliderFloat("Large Hail Gravity", &g_LargeHailGravity, 0.0f, 5.0f, %.1f)) {
            if (HailData::largeHailP) HailData::largeHailP->data.gravity = g_LargeHailGravity;
              if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("gravity of the hail stone, CTRL+ click to type value");
            }
        }

        ImGui::End();
    }
  }
}

//pointer explanation
/* float value = 42.0f;
float* ptr = &value;  // ptr points to value
*ptr = 50.0f;         // changes value to 50
ptr = nullptr;     // changes pointer itself not value
*/
