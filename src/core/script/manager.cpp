#include "script.hpp"
#include "manager.hpp"
#include <SDL2/SDL_messagebox.h>
#include <dirent.h>
#include <sstream>

void ScriptManager::Execute() {
  lua_State *L = luaL_newstate();

  // Make standard libraries available in the Lua object
  luaL_openlibs(L);

  ScriptApi::PushToLua(L);

  luaL_dostring(L, "client.notify('Hello, world!')");
  return;

/*
  int result = luaL_loadfile(state, filename);
  if (result) {
    // pop a alert message with sdl
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error_Lua_Loadfile",
                             lua_tostring(state, -1), NULL);
    return;
  }
  // Finally, execute the program by calling into it.
  // Change the arguments if you're not running vanilla Lua code.
  result = lua_pcall(state, 0, 0, 0);
  if (result) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error_Lua_Pcall",
                             lua_tostring(state, -1), NULL);
    return;
  }
  */
}