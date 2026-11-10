#include "../exo8-le_banc_avant_le_rendu/chaine.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>

struct Oeil {
    std::unique_ptr<ChaineEchange> chaine;
    std::array<std::uint64_t, 3> poignees{};
};

static void Enregistrer(Oeil &oeil, std::uint64_t base) {
    oeil.chaine = std::make_unique<ChaineEchange>(3);
    for (std::uint32_t index = 0; index < 3; ++index) {
        oeil.poignees[index] = base + index;
        oeil.chaine->RegisterImage(index, oeil.poignees[index]);
    }
}

static bool UneImage(Oeil &oeil, std::uint32_t &index, std::uint64_t &poignee) {
    if (!oeil.chaine->AcquireImage(index)) return false;
    if (!oeil.chaine->GetImageHandle(index, poignee)) return false;
    const bool rendu = oeil.chaine->ReleaseImage(index);
    if (!rendu) return false;
    return oeil.chaine->GetLastReleasedIndex() == index;
}

int main() {
    Oeil gauche;
    Oeil droit;
    Enregistrer(gauche, 0x10000u);
    Enregistrer(droit, 0x20000u);

    for (int image = 1; image <= 30; ++image) {
        std::uint32_t indexGauche = 0;
        std::uint32_t indexDroit = 0;
        std::uint64_t poigneeGauche = 0;
        std::uint64_t poigneeDroite = 0;
        if (!UneImage(gauche, indexGauche, poigneeGauche) ||
            !UneImage(droit, indexDroit, poigneeDroite)) return 1;
        std::cout << "image : " << image << ", gauche : " << indexGauche
                  << ", droit : " << indexDroit << '\n';
    }

    gauche.chaine.reset();
    droit.chaine.reset();
    Enregistrer(gauche, 0x110000u);
    Enregistrer(droit, 0x120000u);

    std::uint32_t indexGauche = 0;
    std::uint32_t indexDroit = 0;
    std::uint64_t poigneeGauche = 0;
    std::uint64_t poigneeDroite = 0;
    if (!UneImage(gauche, indexGauche, poigneeGauche) ||
        !UneImage(droit, indexDroit, poigneeDroite)) return 1;
}
