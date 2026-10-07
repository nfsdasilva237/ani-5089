#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKTime/NkClock.h"

using namespace nkentseu;

int nkmain(const NkEntryState& etat) {
    (void)etat;

    NkWindowConfig config;
    config.title = "Ma salle - version perso";
    config.width = 1200;
    config.height = 680;

    NkWindow fenetre(config);
    if (!fenetre.IsValid()) {
        return 1;
    }

    while (fenetre.IsOpen()) {
        NkEvents().PollEvents();
        NkClock::Sleep((int64)12);
    }
    return 0;
}
