#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    int nombreActions = 0;
    if (!(std::cin >> nombreActions)) return 0;
    std::unordered_map<std::string, bool> sortie;
    int entrees = 0;
    int sorties = 0;
    for (int i = 0; i < nombreActions; ++i) {
        std::string nom;
        std::string type;
        std::cin >> nom >> type;
        const bool estSortie = type == "HAPTIC";
        sortie[nom] = estSortie;
        if (estSortie) ++sorties;
        else ++entrees;
    }
    int nombreOperations = 0;
    std::cin >> nombreOperations;
    for (int i = 0; i < nombreOperations; ++i) {
        std::string nom;
        std::string operation;
        std::cin >> nom >> operation;
        const auto action = sortie.find(nom);
        if (action == sortie.end()) continue;
        if (action->second && operation == "LIT")
            std::cout << nom << " LIT UNE SORTIE\n";
        else if (!action->second && operation == "ECRIT")
            std::cout << nom << " ECRIT UNE ENTREE\n";
    }
    std::cout << "ENTREES " << entrees << "\nSORTIES " << sorties << '\n';
}
