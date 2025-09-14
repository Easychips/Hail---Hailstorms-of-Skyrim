#pragma once
#include "SKSEMenuFramework.h"
#include "lookupForms.h"

namespace UI { 

    void Register(); 

    inline std::string TitleText = std::string("This is an ") + FontAwesome::UnicodeToUtf8(0xf2dc) + " icon example";

    void __stdcall Render();

    void __stdcall RenderWindow();

    inline MENU_WINDOW hailMenuWindow;

};


