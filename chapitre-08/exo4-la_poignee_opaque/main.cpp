#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::uint64_t images = 0;
    int commandes = 0;
    if (!(std::cin >> images >> commandes)) return 0;

    std::vector<std::uint64_t> poignees(static_cast<std::size_t>(images));
    std::vector<bool> presentes(static_cast<std::size_t>(images), false);
    std::uint64_t comptees = 0;

    for (int i = 0; i < commandes; ++i) {
        std::string commande;
        std::cin >> commande;
        if (commande == "POSE") {
            std::uint64_t index = 0;
            std::uint64_t valeur = 0;
            std::cin >> index >> valeur;
            if (index >= images) {
                std::cout << "HORS ANNEAU\n";
            } else {
                const auto position = static_cast<std::size_t>(index);
                if (!presentes[position]) {
                    presentes[position] = true;
                    ++comptees;
                }
                poignees[position] = valeur;
            }
        } else if (commande == "LIT") {
            std::uint64_t index = 0;
            std::cin >> index;
            if (index >= images || !presentes[static_cast<std::size_t>(index)]) {
                std::cout << "VIDE\n";
            } else {
                std::cout << poignees[static_cast<std::size_t>(index)] << '\n';
            }
        }
    }

    std::cout << "POSEES " << comptees << '\n';
}
