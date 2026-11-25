#include <cstdint>
#include <iostream>
#include <string>

int main() {
    std::int64_t rendu = 0;
    std::int64_t deformation = 0;
    std::int64_t reprojection = 0;
    std::int64_t seuil = 0;
    if (!(std::cin >> rendu >> deformation >> reprojection >> seuil)) return 0;
    int n = 0;
    std::cin >> n;
    int gagnes = 0;
    int perdus = 0;
    for (int i = 0; i < n; ++i) {
        std::string nom;
        std::int64_t pixels = 0;
        std::cin >> nom >> pixels;
        const std::int64_t apresRendu = pixels * rendu / 1000;
        const std::int64_t apresDeformation = apresRendu * deformation / 1000;
        const std::int64_t dansScene = apresDeformation * reprojection / 1000;
        const bool lisibleScene = dansScene >= seuil;
        const bool lisibleQuad = pixels >= seuil;
        if (!lisibleScene && lisibleQuad) ++gagnes;
        if (lisibleScene && !lisibleQuad) ++perdus;
        std::cout << nom << ' ' << dansScene << ' ' << pixels << ' '
                  << (lisibleScene ? "LISIBLE" : "FLOU") << ' '
                  << (lisibleQuad ? "LISIBLE" : "FLOU") << '\n';
    }
    std::cout << "GAGNES " << gagnes << "\nPERDUS " << perdus << '\n';
}
