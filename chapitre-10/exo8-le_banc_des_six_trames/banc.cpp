#include "couches.h"

#include <array>
#include <iomanip>
#include <iostream>

int main() {
    const std::array<bool, 6> rend = {true, true, false, false, true, true};
    EtatTrame trame;
    int indexImage = 0;
    int fins = 0;
    for (std::size_t i = 0; i < rend.size(); ++i) {
        trame.Commence();
        const double angle = static_cast<double>(i + 1);
        int couches = 0;
        int imageEmploye = -1;
        if (rend[i]) {
            const CoucheProjection couche{"STAGE", angle, angle};
            couches = 1;
            imageEmploye = indexImage;
            std::cout << "trame " << i + 1 << " | image " << imageEmploye
                      << " | rend OUI | couches " << couches << " | espace " << couche.espace
                      << " | vues " << std::fixed << std::setprecision(1)
                      << couche.vueGauche << ',' << couche.vueDroite << '\n';
            indexImage = (indexImage + 1) % 3;
        } else {
            std::cout << "trame " << i + 1 << " | image - | rend NON | couches " << couches
                      << " | espace - | vues -\n";
        }
        trame.Fin();
        ++fins;
    }
    std::cout << "FINS " << fins << " | PROCHAINE IMAGE " << indexImage << '\n';
}
