#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

int main() {
    std::ifstream journal("journal.txt");
    if (!journal) {
        std::cerr << "Fichier journal introuvable\n";
        return 1;
    }

    std::map<std::string, int> compte;
    std::string ligne;
    while (std::getline(journal, ligne)) {
        if (ligne.empty()) {
            continue;
        }

        std::string type;
        double valeur = 0.0;
        std::istringstream flux(ligne);
        flux >> valeur >> type;

        if (!type.empty()) {
            ++compte[type];
        }
    }

    std::cout << "Type le plus frequent :\n";
    for (const auto& [type, total] : compte) {
        std::cout << type << " -> " << total << "\n";
    }

    return 0;
}
