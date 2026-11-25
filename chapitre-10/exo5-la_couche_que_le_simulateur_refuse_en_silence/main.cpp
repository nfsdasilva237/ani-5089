#include <iostream>
#include <string>

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int refusees = 0;
    int averties = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom;
        int commence = 0;
        int rend = 0;
        int gauche = 0;
        int droite = 0;
        std::cin >> nom >> commence >> rend >> gauche >> droite;
        std::string verdict;
        if (!commence) {
            verdict = "REFUSEE";
            ++refusees;
        } else if (!rend) {
            if (gauche || droite) verdict = "COUCHE SANS RENDU";
            else verdict = "OK";
        } else if (gauche && droite) {
            verdict = "OK";
        } else if (!gauche && !droite) {
            verdict = "SOUMISSION VIDE";
        } else if (!gauche) {
            verdict = "OEIL NOIR G";
        } else {
            verdict = "OEIL NOIR D";
        }
        if (verdict != "OK" && verdict != "REFUSEE") ++averties;
        std::cout << nom << ' ' << verdict << '\n';
    }
    std::cout << "REFUSEES " << refusees << "\nAVERTIES " << averties << '\n';
}
