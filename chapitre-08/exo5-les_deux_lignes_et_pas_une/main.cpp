#include <iostream>
#include <string>

int main() {
    int largeurFenetre = 0;
    int hauteurFenetre = 0;
    int nombre = 0;
    if (!(std::cin >> largeurFenetre >> hauteurFenetre >> nombre)) return 0;

    int bloquantes = 0;
    for (int i = 0; i < nombre; ++i) {
        std::string nom;
        int cible = 0;
        int tailleForcee = 0;
        int largeurCible = 0;
        int hauteurCible = 0;
        std::cin >> nom >> cible >> tailleForcee >> largeurCible >> hauteurCible;

        int largeurReelle = largeurFenetre;
        int hauteurReelle = hauteurFenetre;
        std::string verdict;
        if (!cible) {
            verdict = "PAS RELIEE";
        } else if (tailleForcee) {
            largeurReelle = largeurCible;
            hauteurReelle = hauteurCible;
            verdict = "OK";
        } else if (largeurCible != largeurFenetre || hauteurCible != hauteurFenetre) {
            verdict = "BLOQUE";
            ++bloquantes;
        } else {
            verdict = "SANS EFFET";
        }

        std::cout << nom << ' ' << largeurReelle << ' ' << hauteurReelle
                  << ' ' << verdict << '\n';
    }

    std::cout << "BLOQUANTES " << bloquantes << '\n';
}
