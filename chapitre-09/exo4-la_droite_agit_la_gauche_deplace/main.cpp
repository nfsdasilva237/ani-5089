#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

bool estConnu(const std::string &usage) {
    static const std::unordered_set<std::string> usages = {
        "SELECT", "GRAB", "MENU", "MOVE", "AIM_POSE", "GRIP_POSE", "HAPTIC",
        "SELECT_LEFT", "GRAB_LEFT", "AIM_POSE_LEFT", "GRIP_POSE_LEFT",
        "HAPTIC_LEFT", "MOVE_RIGHT"
    };
    return usages.count(usage) != 0;
}

bool estGauche(const std::string &usage) {
    return usage == "MENU" || usage == "MOVE" ||
           (usage.size() >= 5 && usage.compare(usage.size() - 5, 5, "_LEFT") == 0);
}

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int deuxMains = 0;
    for (int i = 0; i < n; ++i) {
        std::string ligne;
        std::getline(std::cin >> std::ws, ligne);
        std::istringstream entree(ligne);
        std::string nom;
        entree >> nom;
        int droite = 0;
        int gauche = 0;
        std::string usage;
        while (entree >> usage) {
            if (estConnu(usage)) {
                if (estGauche(usage)) ++gauche;
                else ++droite;
            }
        }
        const bool deux = droite > 0 && gauche > 0;
        deuxMains += deux;
        std::cout << nom << ' ' << droite << ' ' << gauche << ' '
                  << (deux ? "DEUX MAINS" : "UNE MAIN") << '\n';
    }
    std::cout << "DEUX MAINS " << deuxMains << '\n';
}
