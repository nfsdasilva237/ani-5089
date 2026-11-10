#include <iostream>
#include <sstream>
#include <string>

int main() {
    int nombre = 0;
    if (!(std::cin >> nombre)) return 0;
    std::string ligne;
    std::getline(std::cin, ligne);

    int survivants = 0;
    for (int i = 0; i < nombre; ++i) {
        if (!std::getline(std::cin, ligne)) break;
        std::istringstream entree(ligne);
        std::string nom;
        entree >> nom;

        int positionDetruit = -1;
        int positionRecree = -1;
        bool reenregistre = false;
        std::string operation;
        int position = 0;
        while (entree >> operation) {
            if (operation == "DETRUIT" && positionDetruit == -1)
                positionDetruit = position;
            if (operation == "RECREE" && positionRecree == -1)
                positionRecree = position;
            if (operation == "REENREGISTRE") reenregistre = true;
            ++position;
        }

        std::string resultat;
        if (positionDetruit != -1 && positionRecree != -1 &&
            positionRecree < positionDetruit) {
            resultat = "MAUVAIS ORDRE";
        } else if (positionDetruit == -1 || positionRecree == -1) {
            resultat = "CHAINE PERIMEE";
        } else if (!reenregistre) {
            resultat = "POIGNEES PERDUES";
        } else {
            resultat = "SURVIT";
            ++survivants;
        }
        std::cout << nom << ' ' << resultat << '\n';
    }

    std::cout << "SURVIVANTS " << survivants << '\n';
}
