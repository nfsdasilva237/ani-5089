#include <iostream>
#include <string>

int main() {
    int images = 0;
    int commandes = 0;
    if (!(std::cin >> images >> commandes)) return 0;

    int suivant = 0;
    int empruntee = 0;
    int prets = 0;
    bool emprunt = false;

    for (int tour = 1; tour <= commandes; ++tour) {
        std::string commande;
        std::cin >> commande;
        if (commande == "EMPRUNTE") {
            if (images <= 0 || emprunt) {
                std::cout << tour << ' ' << commande << " REFUSE\n";
            } else {
                empruntee = suivant;
                emprunt = true;
                ++prets;
                std::cout << tour << ' ' << commande << ' ' << empruntee << '\n';
            }
        } else if (commande == "REND") {
            if (!emprunt) {
                std::cout << tour << ' ' << commande << " REFUSE\n";
            } else {
                std::cout << tour << ' ' << commande << ' ' << empruntee << '\n';
                emprunt = false;
                suivant = (suivant + 1) % images;
            }
        } else {
            std::cout << tour << ' ' << commande << " REFUSE\n";
        }
    }

    std::cout << "PRETS " << prets << '\n';
}
