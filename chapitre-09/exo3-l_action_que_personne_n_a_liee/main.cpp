#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main() {
    int nombreActions = 0;
    if (!(std::cin >> nombreActions)) return 0;
    std::vector<std::pair<std::string, std::string>> actions;
    for (int i = 0; i < nombreActions; ++i) {
        std::string nom;
        std::string usage;
        std::cin >> nom >> usage;
        actions.emplace_back(nom, usage);
    }
    int nombreUsages = 0;
    std::cin >> nombreUsages;
    std::unordered_set<std::string> usages;
    for (int i = 0; i < nombreUsages; ++i) {
        std::string usage;
        std::cin >> usage;
        usages.insert(usage);
    }
    int mortes = 0;
    for (const auto &action : actions) {
        const bool active = usages.count(action.second) != 0;
        std::cout << action.first << ' ' << (active ? "ACTIVE" : "MORTE") << '\n';
        mortes += !active;
    }
    std::cout << "MORTES " << mortes << '\n';
}
