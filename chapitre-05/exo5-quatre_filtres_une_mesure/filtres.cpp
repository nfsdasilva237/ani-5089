#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace nkentseu;

struct FilterRun {
    const char *name;
    const char *file;
    NkResizeFilter filter;
    NkImage image;
    double averageMicroseconds;
    int differences;
};

static int CountDifferences(const NkImage &image, const NkImage &bilinear) {
    int count = 0;
    for (int y = 0; y < image.Height(); ++y) {
        for (int x = 0; x < image.Width(); ++x) {
            const auto a = image.GetPixel(x, y);
            const auto b = bilinear.GetPixel(x, y);
            if (a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a) {
                ++count;
            }
        }
    }
    return count;
}

int main() {
    NkImage photo;
    if (!photo.Load("../exo1-treize_formats_une_image/photo.jpg") || !photo.IsValid()) {
        std::cerr << "Impossible de charger la photo de l'exercice 1.\n";
        return 1;
    }

    // La meme source carree 2048 x 2048 est utilisee pour les quatre filtres.
    const NkImage source = photo.Resize(2048, 2048, NkResizeFilter::NK_BILINEAR);
    if (!source.IsValid()) {
        std::cerr << "Echec de la preparation de la source 2048 x 2048.\n";
        return 2;
    }

    std::array<FilterRun, 4> runs = {{
        {"NK_NEAREST", "nearest.png", NkResizeFilter::NK_NEAREST, {}, 0.0, 0},
        {"NK_BILINEAR", "bilinear.png", NkResizeFilter::NK_BILINEAR, {}, 0.0, 0},
        {"NK_BICUBIC", "bicubic.png", NkResizeFilter::NK_BICUBIC, {}, 0.0, 0},
        {"NK_LANCZOS3", "lanczos3.png", NkResizeFilter::NK_LANCZOS3, {}, 0.0, 0},
    }};

    for (auto &run : runs) {
        NkChrono timer;
        for (int repetition = 0; repetition < 20; ++repetition) {
            run.image = source.Resize(256, 256, run.filter);
        }
        run.averageMicroseconds = timer.Elapsed().ToMicroseconds() / 20.0;
        if (!run.image.IsValid() || run.image.Width() != 256 || run.image.Height() != 256 ||
            !run.image.SavePNG(run.file)) {
            std::cerr << "Echec de la reduction ou de l'ecriture : " << run.file << '\n';
            return 3;
        }
    }

    const NkImage &bilinear = runs[1].image;
    for (auto &run : runs) {
        run.differences = CountDifferences(run.image, bilinear);
    }

    std::ofstream measurements("mesures.txt");
    if (!measurements) {
        std::cerr << "Impossible d'ouvrir mesures.txt en ecriture.\n";
        return 4;
    }
    measurements << std::fixed << std::setprecision(1);
    for (const auto &run : runs) {
        measurements << run.name << " : " << run.averageMicroseconds
                     << " us, differences : " << run.differences << '\n';
        std::cout << run.name << " : " << run.averageMicroseconds
                  << " us, differences : " << run.differences << '\n';
    }

    std::cout << "Source : " << photo.Width() << " x " << photo.Height()
              << " -> 2048 x 2048; sorties : 256 x 256.\n";
    return 0;
}
