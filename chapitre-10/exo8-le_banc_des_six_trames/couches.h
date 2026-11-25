#pragma once

struct CoucheProjection {
    const char *espace;
    double vueGauche;
    double vueDroite;
};

struct EtatTrame {
    bool ouverte = false;

    bool Commence() {
        if (ouverte) return false;
        ouverte = true;
        return true;
    }

    void Fin() {
        ouverte = false;
    }
};
