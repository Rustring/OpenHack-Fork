#include "pch.h"
#include "winmm.h"

#include "config.h"

#include "hacks/hacks.h"
#include "hook.h"
#include "menu/menu.h"
#include "hooks/MenuLayer.h"

bool initialized = false;

DWORD WINAPI MainThread(LPVOID param)
{
#if _DEBUG
    logger::init(true, true, "log.txt");
    utils::set_console_title("OpenHack - Geometry Dash");
#else
    logger::init(false, true, "log.txt");
#endif

    L_INFO("Loading OpenHack...");

    // Check if it's december (for snow particles)
    auto t = std::time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);
    globals::is_december = tm.tm_mon == 11;

    // Initialize menu after the game has loaded
    hooks::MenuLayer::on_init = []()
    {
        if (initialized)
            return;

        hook::set_menu_hotkey(config::menu_hotkey);
        hook::set_menu_toggle_callback(menu::toggle);
        hook::set_menu_draw_callback(menu::draw);
        hook::set_menu_init_callback(menu::init);

        initialized = true;
        L_INFO("OpenHack loaded successfully!");
    };

    // Initialize hooks
    hook::init();
    config::load();

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        DisableThreadLibraryCalls(hModule);
        break;
    default:
        break;
    }
    return TRUE;
}