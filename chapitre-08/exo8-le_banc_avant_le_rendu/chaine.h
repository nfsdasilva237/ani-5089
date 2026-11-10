#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

class ChaineEchange {
public:
    static constexpr std::uint32_t CapaciteMaximale = 4;

    explicit ChaineEchange(std::uint32_t nombreImages) noexcept
        : mNombreImages(nombreImages), mValide(nombreImages > 0 && nombreImages <= CapaciteMaximale) {}

    bool IsValid() const noexcept {
        return mValide;
    }

    bool RegisterImage(std::uint32_t index, std::uint64_t poignee) noexcept {
        if (!mValide || index >= mNombreImages) return false;
        const std::size_t position = static_cast<std::size_t>(index);
        if (!mEnregistrees[position]) {
            mEnregistrees[position] = true;
            ++mNombreEnregistrements;
        }
        mPoignees[position] = poignee;
        return true;
    }

    bool GetImageHandle(std::uint32_t index, std::uint64_t &poignee) const noexcept {
        if (!mValide || index >= mNombreImages) return false;
        const std::size_t position = static_cast<std::size_t>(index);
        if (!mEnregistrees[position]) return false;
        poignee = mPoignees[position];
        return true;
    }

    bool AcquireImage(std::uint32_t &index) noexcept {
        if (!mValide || mEmpruntee || mNombreEnregistrements != mNombreImages) return false;
        mIndexEmprunte = mProchainIndex;
        mEmpruntee = true;
        index = mIndexEmprunte;
        return true;
    }

    bool ReleaseImage(std::uint32_t &index) noexcept {
        if (!mValide || !mEmpruntee) return false;
        index = mIndexEmprunte;
        mEmpruntee = false;
        mProchainIndex = (mProchainIndex + 1u) % mNombreImages;
        return true;
    }

    std::uint32_t GetLastReleasedIndex() const noexcept {
        if (!mValide) return 0;
        return (mProchainIndex + mNombreImages - 1u) % mNombreImages;
    }

    std::uint32_t GetImageCount() const noexcept {
        return mValide ? mNombreImages : 0;
    }

private:
    std::uint32_t mNombreImages = 0;
    std::uint32_t mProchainIndex = 0;
    std::uint32_t mIndexEmprunte = 0;
    std::uint32_t mNombreEnregistrements = 0;
    bool mValide = false;
    bool mEmpruntee = false;
    std::array<std::uint64_t, CapaciteMaximale> mPoignees{};
    std::array<bool, CapaciteMaximale> mEnregistrees{};
};
