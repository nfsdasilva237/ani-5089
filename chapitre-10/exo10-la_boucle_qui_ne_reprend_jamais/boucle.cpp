#include <iostream>

struct ResultatTrame {
    bool rend;
    bool debut;
    bool fin;
};

void PomperEvenements() {}

void Attendre() {}

void SynchroniserActions() {}

void Rendre() {}

ResultatTrame TrameFautive(bool devraitRendre, bool &ouverte) {
    PomperEvenements();
    Attendre();
    const bool debut = !ouverte;
    if (debut) ouverte = true;
    if (debut) SynchroniserActions();
    bool fin = false;
    if (debut && devraitRendre) {
        Rendre();
        fin = true;
        ouverte = false;
    }
    return {devraitRendre, debut, fin};
}

ResultatTrame TrameCorrigee(bool devraitRendre, bool &ouverte) {
    PomperEvenements();
    Attendre();
    const bool debut = !ouverte;
    if (debut) ouverte = true;
    if (debut) SynchroniserActions();
    if (debut && devraitRendre) Rendre();
    const bool fin = debut;
    if (fin) ouverte = false;
    return {devraitRendre, debut, fin};
}

int main() {
    bool ouverteFautive = false;
    bool ouverteCorrigee = false;
    for (int trame = 1; trame <= 20; ++trame) {
        const bool devraitRendre = !(trame >= 8 && trame <= 12);
        const ResultatTrame fautive = TrameFautive(devraitRendre, ouverteFautive);
        const ResultatTrame corrigee = TrameCorrigee(devraitRendre, ouverteCorrigee);
        std::cout << "trame : " << trame
                  << ", rend : " << fautive.rend
                  << ", debut : " << fautive.debut
                  << ", fin : " << fautive.fin << '\n';
        static_cast<void>(corrigee);
    }
}
