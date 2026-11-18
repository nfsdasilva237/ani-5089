#include <iostream>

struct EtatAction {
    bool current;
    bool changed;
    bool active;
};

bool TenirSurLeFront(const EtatAction &saisie) {
    return saisie.current && saisie.changed;
}

bool TenirSurLeNiveau(const EtatAction &saisie) {
    return saisie.current;
}

int main() {
    bool objetTenuAuFront = false;
    bool objetTenuAuNiveau = false;
    bool precedent = false;
    for (int image = 1; image <= 4; ++image) {
        const bool pression = image >= 2 && image <= 4;
        const EtatAction saisie{pression, pression != precedent, true};
        objetTenuAuFront = TenirSurLeFront(saisie);
        objetTenuAuNiveau = TenirSurLeNiveau(saisie);
        std::cout << "image : " << image << ", front_tient : " << objetTenuAuFront
                  << ", niveau_tient : " << objetTenuAuNiveau << '\n';
        precedent = saisie.current;
    }
}
