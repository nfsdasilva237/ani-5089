#include <iostream>
#include <string>

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int fautes = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom;
        std::string intention;
        std::string pose;
        std::cin >> nom >> intention >> pose;
        std::string verdict = "OK";
        if (intention == "TENIR" && pose == "AIM") verdict = "FLOTTE";
        if (intention == "POINTER" && pose == "GRIP") verdict = "PART DE TRAVERS";
        fautes += verdict != "OK";
        std::cout << nom << ' ' << verdict << '\n';
    }
    std::cout << "FAUTES " << fautes << '\n';
}
