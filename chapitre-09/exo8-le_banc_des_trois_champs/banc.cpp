#include "actions.h"

#include <array>
#include <iostream>

int main() {
    const std::array<bool, 6> toucheSaisie = {false, true, true, true, false, false};
    const std::array<bool, 6> toucheMenu = {false, false, true, false, false, false};
    bool precedentSaisie = false;
    bool precedentMenu = false;
    for (std::size_t i = 0; i < toucheSaisie.size(); ++i) {
        const EtatAction saisie = MettreAJourAction(toucheSaisie[i], true, precedentSaisie);
        const EtatAction menu = MettreAJourAction(toucheMenu[i], true, precedentMenu);
        const EtatAction nonLiee = MettreAJourAction(true, false, false);
        std::cout << i + 1 << " | saisie " << saisie.courant << ' ' << saisie.change << ' ' << saisie.actif
                  << " | menu " << menu.courant << ' ' << menu.change << ' ' << menu.actif
                  << " | non_liee " << nonLiee.courant << ' ' << nonLiee.change << ' ' << nonLiee.actif << '\n';
        precedentSaisie = saisie.courant;
        precedentMenu = menu.courant;
    }
}
