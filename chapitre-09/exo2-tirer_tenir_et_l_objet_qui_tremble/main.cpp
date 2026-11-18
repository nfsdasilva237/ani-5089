#include <iostream>

int main() {
    int n = 0;
    if (!(std::cin >> n)) return 0;
    int precedent = 0;
    int ballesFront = 0;
    int ballesNiveau = 0;
    int tenuNiveau = 0;
    int tenuFront = 0;
    for (int i = 0; i < n; ++i) {
        int presse = 0;
        std::cin >> presse;
        const int front = presse && !precedent;
        ballesFront += front;
        ballesNiveau += presse;
        tenuNiveau += presse;
        tenuFront += front;
        precedent = presse;
    }
    std::cout << "BALLES FRONT " << ballesFront
              << "\nBALLES NIVEAU " << ballesNiveau
              << "\nTENU NIVEAU " << tenuNiveau
              << "\nTENU FRONT " << tenuFront << '\n';
}
