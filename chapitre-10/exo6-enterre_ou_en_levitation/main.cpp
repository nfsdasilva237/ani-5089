#include <iostream>
#include <string>

int main() {
    long long hauteurYeux = 0;
    int n = 0;
    if (!(std::cin >> hauteurYeux >> n)) return 0;
    int decalees = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom;
        std::string espacePose;
        std::string espaceCouche;
        std::cin >> nom >> espacePose >> espaceCouche;
        long long decalage = 0;
        std::string verdict = "OK";
        if (espacePose != espaceCouche) {
            if (espacePose == "STAGE") {
                decalage = hauteurYeux;
                verdict = "LEVITATION";
            } else {
                decalage = -hauteurYeux;
                verdict = "ENTERRE";
            }
            ++decalees;
        }
        std::cout << nom << ' ' << decalage << ' ' << verdict << '\n';
    }
    std::cout << "DECALEES " << decalees << '\n';
}
