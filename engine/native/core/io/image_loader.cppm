module;

#include <vector>
#include <cstdint>
#include <filesystem>

export module core.io.image_loader;

export namespace draco::core::io::image_loader
{
    struct ImageData
    {
        std::vector<uint8_t> pixels;
        uint32_t width = 0;
        uint32_t height = 0;
        uint8_t channels = 0;
        bool is_valid = false;
    };

    // Load an image file (PNG, JPG, etc) & decode it to raw RGBA8
    ImageData load_image(const std::filesystem::path& path);
}