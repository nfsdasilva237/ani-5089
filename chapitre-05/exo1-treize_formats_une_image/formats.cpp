#include "NKImage/Core/NkImage.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace nkentseu;

namespace fs = std::filesystem;

struct FormatResult {
    const char *name;
    const char *path;
    bool saved;
};

static void WriteResult(std::ofstream &out, const FormatResult &format,
                        int originalWidth, int originalHeight) {
    if (!format.saved) {
        out << format.name << " : 0 octets, illisible\n";
        std::cerr << "Echec d'ecriture : " << format.path << '\n';
        return;
    }

    const auto bytes = fs::file_size(format.path);
    NkImage reread;
    if (!reread.Load(format.path) || !reread.IsValid()) {
        out << format.name << " : " << bytes << " octets, illisible\n";
        std::cerr << "Echec de relecture : " << format.path << '\n';
        return;
    }

    out << format.name << " : " << bytes << " octets, "
        << reread.Width() << " x " << reread.Height() << '\n';
    std::cout << format.name << " relu : " << reread.Width() << " x "
              << reread.Height() << (reread.Width() == originalWidth &&
                                      reread.Height() == originalHeight
                                          ? " (dimensions identiques)\n"
                                          : " (dimensions differentes)\n");
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg") || !photo.IsValid()) {
        std::cerr << "Impossible de charger photo.jpg. Placez votre photo dans le dossier de lancement.\n";
        return 1;
    }

    const auto width = photo.Width();
    const auto height = photo.Height();
    std::cout << "Photo : " << width << " x " << height << '\n';

    const FormatResult results[] = {
        {"png", "photo.png", photo.SavePNG("photo.png")},
        {"jpeg", "photo.jpeg", photo.SaveJPEG("photo.jpeg")},
        {"bmp", "photo.bmp", photo.SaveBMP("photo.bmp")},
        {"tga", "photo.tga", photo.SaveTGA("photo.tga")},
        {"qoi", "photo.qoi", photo.SaveQOI("photo.qoi")},
    };

    std::ofstream output("formats.txt");
    if (!output) {
        std::cerr << "Impossible d'ouvrir formats.txt en ecriture.\n";
        return 2;
    }

    for (const auto &result : results) {
        WriteResult(output, result, width, height);
    }

    std::cout << "Les cinq relectures sont ecrites dans formats.txt.\n";
    std::cout << "Comparez chaque dimension relue a " << width << " x " << height << ".\n";
    return 0;
}
