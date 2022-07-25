#include "../../includes.hpp"
#include "hooks.hpp"

void Hooks::SetDrawColor::hook(void *thisptr, int r, int g, int b, int a) {
    if (CONFIGBOOL("Visuals>World>World>No Scope Overlay") && (RETURN_ADDRESS() == Offsets::scopeArc || RETURN_ADDRESS() == Offsets::scopeDust)) {
        a = 0;
    }
    original(thisptr, r, g, b, a);
}