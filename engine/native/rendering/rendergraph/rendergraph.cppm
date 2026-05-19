module;

#include <vector>
#include <string>
#include <cstdint>

export module rendering.rendergraph;

import rendering.rhi;

export namespace draco::rendering::rendergraph {

    enum class PassType : uint8_t
    {
        Graphics,
        Transparent,
        Shadow,
        PostProcess,
        UI
    };

    enum class SortMode : uint8_t
    {
        None,
        Material,
        FrontToBack,
        BackToFront
    };

    struct Pass
    {
        std::string name;

        PassType type = PassType::Graphics;
        SortMode sort_mode = SortMode::Material;

        std::vector<std::string> dependencies;

        rhi::ViewID view = 0;
        rhi::FramebufferHandle framebuffer = rhi::InvalidFramebuffer;

        std::vector<rhi::RenderPacket> packets;

        float view_mtx[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        
        float proj_mtx[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        uint16_t width = 0;
        uint16_t height = 0;

        uint32_t clear_flags = 0;
        uint32_t clear_color = 0;
    };

    class RenderGraph
    {
    public:
        void reset();

        Pass& add_pass(const std::string& name);

        Pass* get_pass(const std::string& name);

        void execute();

    private:
        std::vector<Pass> m_passes;
    };
}
