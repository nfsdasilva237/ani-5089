#include <cstdlib>
#include <iostream>
#include <string>

struct Anneau {
    int images = 0;
    int suivant = 0;
    int empruntee = 0;
    int rendus = 0;
    bool emprunt = false;

    bool Acquire(int &index) {
        if (images <= 0 || emprunt) return false;
        empruntee = suivant;
        index = empruntee;
        emprunt = true;
        return true;
    }

    bool Release(int &index) {
        if (!emprunt) return false;
        index = empruntee;
        emprunt = false;
        suivant = (suivant + 1) % images;
        ++rendus;
        return true;
    }
};

int main() {
    int images = 0;
    int commandes = 0;
    if (!(std::cin >> images >> commandes)) return 0;

    Anneau yeux[2] = {{images}, {images}};
    int desync = 0;

    for (int tour = 1; tour <= commandes; ++tour) {
        char oeil = 'G';
        std::string commande;
        std::cin >> oeil >> commande;
        const int cote = oeil == 'D' ? 1 : 0;
        int index = -1;
        bool ok = false;

        if (commande == "EMPRUNTE") ok = yeux[cote].Acquire(index);
        else if (commande == "REND") ok = yeux[cote].Release(index);

        std::cout << tour << ' ' << oeil << ' ' << commande << ' ';
        if (ok) std::cout << index << '\n';
        else std::cout << "REFUSE\n";

        const int ecart = std::abs(yeux[0].rendus - yeux[1].rendus);
        if (desync == 0 && ecart > 1) desync = tour;
    }

    std::cout << "DECALAGE " << std::abs(yeux[0].rendus - yeux[1].rendus) << '\n';
    std::cout << "DESYNC " << desync << '\n';
}
