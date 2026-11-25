#include <cstdint>
#include <iostream>

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int justes = 0;
    int pires = 0;
    for (int i = 0; i < n; ++i) {
        std::int64_t rendu = 0;
        std::int64_t reel = 0;
        std::int64_t soumis = 0;
        std::cin >> rendu >> reel >> soumis;
        const std::int64_t sans = rendu - reel;
        const std::int64_t avec = rendu - soumis;
        const std::uint64_t absSans = sans < 0 ? static_cast<std::uint64_t>(-sans) : static_cast<std::uint64_t>(sans);
        const std::uint64_t absAvec = avec < 0 ? static_cast<std::uint64_t>(-avec) : static_cast<std::uint64_t>(avec);
        const char *verdict = "";
        if (avec == 0) {
            verdict = "JUSTE";
            ++justes;
        } else if (absAvec < absSans) {
            verdict = "PARTIEL";
        } else if (absAvec == absSans) {
            verdict = "SANS EFFET";
        } else {
            verdict = "PIRE";
            ++pires;
        }
        std::cout << "SANS " << sans << " AVEC " << avec << ' ' << verdict << '\n';
    }
    std::cout << "JUSTES " << justes << "\nPIRES " << pires << '\n';
}
