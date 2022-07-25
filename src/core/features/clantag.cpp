#include "features.hpp"
#include <cstring>
#include <string>

void shiftMarquee(char* text, int size) {
    char temp;
    for (int i = 0; i < (size - 1); i++) {
        temp = text[size - 1];
        text[size - 1] = text[i];
        text[i] = temp;
    }
}

void updateClantag() {
    if ((TICKCOUNTWITHPING() % 128) > 96) {
        Offsets::sendClantag(Menu::clantag, "ghub/seksea/gamesneeze\xe2\x80\xa8\xe2\x80\xa8\xe2\x80\xa8");
    }
    else {
        Offsets::sendClantag(Menu::clantag, "gamesneeze beste\xe2\x80\xa8\xe2\x80\xa8\xe2\x80\xa8");
    }
}

void Features::ClantagChanger::frameStageNotify(FrameStage frame) {
    if (CONFIGBOOL("Misc>Misc>Clantag>Clantag") && frame == FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
        if (CONFIGBOOL("Misc>Misc>Clantag>Clantag Marquee")) {
            if(TICKCOUNTWITHPING() % 32 == 0) {
                shiftMarquee((Menu::clantag), strlen(Menu::clantag));
                updateClantag();
            }
        } else if (CONFIGBOOL("Misc>Misc>Clantag>Brand Clantag")) {
          static std::string brands[24] = {
              "⚡",           "⚡g",          "⚡ga",         "⚡gam",
              "⚡game",       "⚡games",      "⚡gamesn",     "⚡gamesne",
              "⚡gamesnee",   "⚡gamesneez",  "⚡gamesneeze", "☠gamesneeze",
              "⚡gamesneeze", "☠gamesneeze", "⚡gamesneeze", "⚡amesneeze",
              "⚡mesneeze",   "⚡esneeze",    "⚡sneeze",     "⚡neeze",
              "⚡eeze",       "⚡eze",        "⚡ze",         "⚡e",
          };
          if (TICKCOUNTWITHPING() % 16 == 0) {
            std::string brandCol = brands[(TICKCOUNTWITHPING() / 16) % 24];
            memcpy(Menu::clantag, brandCol.c_str(), 127);
            Menu::clantag[127] = '\0';
            updateClantag();
          }
        }
        else {
            if(TICKCOUNTWITHPING() % 128 == 0) {
                updateClantag();
            }
        }
    }
}