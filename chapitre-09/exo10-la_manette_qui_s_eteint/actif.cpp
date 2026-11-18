#include <iostream>

struct EtatAction {
    bool courant;
    bool change;
    bool actif;
};

bool RelachementSansVerifierActif(const EtatAction &saisie) {
    return saisie.change && !saisie.courant;
}

bool RelachementValide(const EtatAction &saisie) {
    return saisie.actif && saisie.change && !saisie.courant;
}

int main() {
    bool precedent = false;
    bool objetAvant = false;
    bool objetApres = false;
    for (int image = 1; image <= 20; ++image) {
        const bool actif = image <= 5 || image > 15;
        const bool presse = true;
        const bool courant = actif && presse;
        const EtatAction saisie{courant, courant != precedent, actif};
        if (image == 1) {
            objetAvant = true;
            objetApres = true;
        }
        if (RelachementSansVerifierActif(saisie)) objetAvant = false;
        if (RelachementValide(saisie)) objetApres = false;
        std::cout << "image : " << image << ", courant : " << saisie.courant
                  << ", change : " << saisie.change << ", actif : " << saisie.actif << '\n';
        precedent = courant;
    }
    return !objetAvant && objetApres ? 0 : 1;
}
