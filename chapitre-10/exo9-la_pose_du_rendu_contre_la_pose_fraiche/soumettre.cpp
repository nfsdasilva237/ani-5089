#include <iostream>

struct VuesSoumises {
    double angleRendu;
    double angleSoumis;
};

VuesSoumises SoumettreVersionJuste(double angleAuRendu) {
    return {angleAuRendu, angleAuRendu};
}

VuesSoumises SoumettreVersionFautive(double angleAuRendu, double angleJusteAvantSoumission) {
    return {angleAuRendu, angleJusteAvantSoumission};
}

int main() {
    const double angleAuRendu = 10.0;
    const double angleAuMomentSoumission = 11.5;
    const VuesSoumises correcte = SoumettreVersionJuste(angleAuRendu);
    const VuesSoumises fautive = SoumettreVersionFautive(angleAuRendu, angleAuMomentSoumission);
    const double poseAffichee = 12.0;
    const double correctionCorrecte = poseAffichee - correcte.angleSoumis;
    const double correctionFautive = poseAffichee - fautive.angleSoumis;
    std::cout << "juste: rendu " << correcte.angleRendu << ", soumis " << correcte.angleSoumis
              << ", correction compositeur " << correctionCorrecte << '\n';
    std::cout << "fautive: rendu " << fautive.angleRendu << ", soumis " << fautive.angleSoumis
              << ", correction compositeur " << correctionFautive << '\n';
}
