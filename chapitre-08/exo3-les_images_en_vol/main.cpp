#include <iostream>
#include <string>

int main() {
    int maximum = 0;
    int commandes = 0;
    if (!(std::cin >> maximum >> commandes)) return 0;

    int enVol = 0;
    int bloque = 0;

    for (int tour = 1; tour <= commandes; ++tour) {
        std::string commande;
        std::cin >> commande;
        bool attend = false;

        if (commande == "SOUMET") {
            if (enVol >= maximum) {
                attend = true;
                if (bloque == 0) bloque = tour;
            } else {
                ++enVol;
            }
        } else if (commande == "FINI") {
            if (enVol > 0) --enVol;
        }

        std::cout << tour << ' ' << commande << ' ' << enVol << ' '
                  << (attend ? "ATTEND" : "PASSE") << '\n';
    }

    std::cout << "BLOQUE " << bloque << '\n';
}
