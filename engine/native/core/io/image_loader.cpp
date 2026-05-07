module;

#include <vector>
#include <cstdint>
#include <filesystem>
#include <print>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

module core.io.image_loader;

// TODO: I'm too lazy to write code so we need somethin' better

namespace draco::core::io::image_loader
{
    ImageData load_image(const std::filesystem::path& path)
    {
        ImageData result;

        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) {
            std::println("Error: Image path does not exist: {}", path.string());
            return result;
        }

        int width, height, channels;
        // STBI_rgb_alpha forces the output to be 4 bytes per pixel (RGBA)
        unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!data) {
            std::println("Error: Failed to decode image: {}", path.string());
            return result;
        }

        if (width <= 0 || height <= 0) {
            stbi_image_free(data);
            return result;
        }

        const size_t w = static_cast<size_t>(width);
        const size_t h = static_cast<size_t>(height);
        if (w > (std::numeric_limits<size_t>::max() / 4) / h) {
            stbi_image_free(data);
            return result;
        }
        
        size_t size = w * h * 4;
        
        result.pixels.assign(data, data + size);
        result.width = static_cast<uint16_t>(width);
        result.height = static_cast<uint16_t>(height);
        result.channels = 4;
        result.is_valid = true;

        // Free the memory allocated by stb
        stbi_image_free(data);

        return result;
    }
}