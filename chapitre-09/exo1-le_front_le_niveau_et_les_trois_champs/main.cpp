#include <iostream>

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int precedent = 0;
    int fronts = 0;
    int niveaux = 0;
    for (int i = 1; i <= n; ++i) {
        int presse = 0;
        int lie = 0;
        std::cin >> presse >> lie;
        const int courant = lie ? presse : 0;
        const int change = courant != precedent;
        std::cout << i << ' ' << courant << ' ' << change << ' ' << lie << '\n';
        fronts += courant && change;
        niveaux += courant;
        precedent = courant;
    }
    std::cout << "FRONTS " << fronts << "\nNIVEAUX " << niveaux << '\n';
}
