#pragma once

struct EtatAction {
    bool courant = false;
    bool change = false;
    bool actif = false;
};

inline EtatAction MettreAJourAction(bool presse, bool lie, bool precedent) {
    EtatAction etat;
    etat.actif = lie;
    etat.courant = lie && presse;
    etat.change = etat.courant != precedent;
    return etat;
}
