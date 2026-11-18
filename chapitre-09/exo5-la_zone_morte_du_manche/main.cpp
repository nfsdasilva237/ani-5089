#include <cstdint>
#include <iostream>

int main() {
    std::int64_t seuil = 0;
    std::int64_t maximum = 0;
    if (!(std::cin >> seuil >> maximum)) return 0;
    int n = 0;
    std::cin >> n;
    int morts = 0;
    for (int i = 0; i < n; ++i) {
        std::int64_t valeur = 0;
        std::cin >> valeur;
        const std::int64_t absolue = valeur < 0 ? -valeur : valeur;
        if (absolue < seuil) {
            std::cout << "0\n";
            ++morts;
            continue;
        }
        const std::int64_t reste = absolue - seuil;
        const std::int64_t course = 1000 - seuil;
        std::int64_t vitesse = course == 0 ? maximum
            : maximum * reste * reste / (course * course);
        if (valeur < 0) vitesse = -vitesse;
        std::cout << vitesse << '\n';
    }
    std::cout << "MORTS " << morts << '\n';
}
