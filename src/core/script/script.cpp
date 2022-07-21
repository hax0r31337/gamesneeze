#include "script.hpp"
#include <SDL2/SDL_messagebox.h>

static int s_client_notify(lua_State *L) {
  const char *message = lua_tostring(L, 1);
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Notification", message,
                           NULL);
  return 0;
}

void ScriptApi::PushToLua(lua_State *L) {
  // client.*
  lua_newtable(L);

  lua_pushcfunction(L, s_client_notify);
  lua_setfield(L, -2, "notify");

  lua_setglobal(L, "client");
}