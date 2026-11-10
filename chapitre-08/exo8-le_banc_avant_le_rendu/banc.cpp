#include "chaine.h"

#include <cstdint>
#include <iostream>

int main() {
    ChaineEchange anneau(3);
    for (std::uint32_t index = 0; index < 3; ++index)
        anneau.RegisterImage(index, 1000u + index);

    for (std::uint32_t tour = 1; tour <= 6; ++tour) {
        std::uint32_t index = 0;
        const bool empruntee = anneau.AcquireImage(index);
        std::cout << "tour " << tour << " EMPRUNTE " << (empruntee ? "OK " : "REFUSE ")
                  << (empruntee ? index : 0) << '\n';
        if (empruntee) {
            const bool rendue = anneau.ReleaseImage(index);
            std::cout << "tour " << tour << " REND " << (rendue ? "OK " : "REFUSE ")
                      << (rendue ? index : 0) << '\n';
        }
    }

    std::uint32_t index = 0;
    const bool premier = anneau.AcquireImage(index);
    const bool doubleAcquire = anneau.AcquireImage(index);
    std::cout << "double emprunt : " << (doubleAcquire ? "ACCEPTE" : "REFUSE") << '\n';
    anneau.ReleaseImage(index);
    const bool renduSansEmprunt = anneau.ReleaseImage(index);
    std::cout << "rendu sans emprunt : " << (renduSansEmprunt ? "ACCEPTE" : "REFUSE") << '\n';
    std::cout << "chaine trop grande : "
              << (ChaineEchange(5).IsValid() ? "ACCEPTEE" : "REFUSEE") << '\n';
    return premier ? 0 : 1;
}
