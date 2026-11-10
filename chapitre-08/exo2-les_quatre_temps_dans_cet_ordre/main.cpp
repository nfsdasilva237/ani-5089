#include <iostream>
#include <string>

int main() {
    int commandes = 0;
    if (!(std::cin >> commandes)) return 0;

    bool acquise = false;
    bool rendue = false;
    int fautes = 0;

    for (int tour = 1; tour <= commandes; ++tour) {
        std::string appel;
        std::cin >> appel;
        std::string resultat = "OK";

        if (appel == "ACQUIERT") {
            if (acquise) resultat = "DEJA ACQUISE";
            else acquise = true;
        } else if (appel == "DESSINE") {
            if (!acquise) resultat = "PAS ACQUISE";
        } else if (appel == "REND") {
            if (!acquise) resultat = "RIEN ACQUIS";
            else {
                acquise = false;
                rendue = true;
            }
        } else if (appel == "SOUMET") {
            if (!rendue) resultat = "PAS RENDUE";
            else rendue = false;
        } else {
            resultat = "APPEL INCONNU";
        }

        if (resultat != "OK") ++fautes;
        std::cout << tour << ' ' << appel << ' ' << resultat << '\n';
    }

    std::cout << "FAUTES " << fautes << '\n';
}
