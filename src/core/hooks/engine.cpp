#include "../../includes.hpp"
#include "hooks.hpp"
#include <SDL2/SDL_scancode.h>
#include <algorithm>
#include <cstring>
#include <streambuf>
#include <string>
#include <string_view>

void Hooks::ClientCmd::hook(void* thisptr, char* szCmdString) {
    if (strstr(szCmdString, "say ") && CONFIGBOOL("Misc>Misc>Misc>Chat Filter Bypass")) {
        // https://www.unknowncheats.me/forum/counterstrike-global-offensive/439677-bypass-chat-swear-filter.html
        std::reverse(szCmdString+5, szCmdString + strlen(szCmdString)-1);
        memmove(szCmdString+7, szCmdString+5, strlen(szCmdString)-6);
        szCmdString[4] = '\xE2';
        szCmdString[5] = '\x80';
        szCmdString[6] = '\xAE';
    }

    original(thisptr, szCmdString);
}

bool Hooks::IsHLTV::hook(void *thisptr) {
  if (CONFIGBOOL("Misc>Misc>Misc>Force square radar") &&
      RETURN_ADDRESS() == Offsets::radarIsHltvCheck) {
    return true;
  }

  return original(thisptr);
}

bool Hooks::IsPlayingDemo::hook(void *thisptr) {
  if (CONFIGBOOL("Misc>Misc>Misc>Reveal Money") && RETURN_ADDRESS() == Offsets::demoOrHLTV) {
    return true;
  }
  return original(thisptr);
}