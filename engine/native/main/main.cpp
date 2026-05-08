#include <print>
#include <cstring>
#include <math.h>

#include <SDL3/SDL.h>
#include <bx/math.h>

import core.io.filesystem;
import core.io.image_loader;

import input;
import platform;
import scene.camera.controller;
import scene.transform;

import rendering.rhi;
import rendering.rhi.vertex;
import rendering.renderer;
import rendering.mesh;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println("SDL init failed: {}", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Draconic Engine",
        1280, 720,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::println("Failed to create window: {}", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    draco::input::set_mouse_captured(window, true);

    auto handles = draco::platform::get_native_handles(window);

    if (!handles.valid) {
        std::println("Failed to get native handles");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Init the RHI & Renderer
    if (!draco::rendering::rhi::init(handles.ndt, handles.nwh, 1280, 720)) {
        std::println("RHI init failed");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    draco::rendering::renderer::init(1280, 720);

    auto cube_mesh     = draco::rendering::mesh::create_cube();
    auto plane_mesh    = draco::rendering::mesh::create_plane(5.0f);
    auto sphere_mesh   = draco::rendering::mesh::create_sphere(24, 16);
    auto cylinder_mesh = draco::rendering::mesh::create_cylinder(24, 2.0f);
    auto capsule_mesh  = draco::rendering::mesh::create_capsule(24, 12, 2.0f);

    auto img = draco::core::io::image_loader::load_image("test.png");

    draco::rendering::rhi::TextureHandle tex = draco::rendering::rhi::InvalidTexture;
    if (img.is_valid) {
        tex = draco::rendering::rhi::create_texture(img.pixels.data(), img.width, img.height);
    }

    auto s_texColor = draco::rendering::rhi::create_uniform("s_texColor", draco::rendering::rhi::UniformType::Sampler);

    auto vs = draco::core::io::filesystem::load_binary("vs.bin");
    auto fs = draco::core::io::filesystem::load_binary("fs.bin");

    if (vs.empty() || fs.empty()) {
        std::println("Shader load failed");
        draco::rendering::rhi::shutdown();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    auto vsh = draco::rendering::rhi::create_shader(vs.data(), (uint32_t)vs.size());
    auto fsh = draco::rendering::rhi::create_shader(fs.data(), (uint32_t)fs.size());

    auto pipeline = draco::rendering::rhi::create_pipeline({
        vsh,
        fsh,
        draco::rendering::rhi::PipelineState::WriteRGB |
        draco::rendering::rhi::PipelineState::WriteAlpha |
        draco::rendering::rhi::PipelineState::MSAA,

        draco::rendering::rhi::BlendMode::None,
        draco::rendering::rhi::DepthTest::Less,
        draco::rendering::rhi::CullMode::CCW,
        true
    });

    draco::scene::CameraController camera;
    camera.init();

    auto u_tint   = draco::rendering::rhi::create_uniform("u_tint",   draco::rendering::rhi::UniformType::Vec4);
    auto u_offset = draco::rendering::rhi::create_uniform("u_offset", draco::rendering::rhi::UniformType::Vec4);

    float tint[4]   = {1,1,1,1};
    float offset[4] = {0,0,0,0};

    bool running = true;
    bool mouse_captured = true;

    while (running)
    {
        // Delta time
        static uint64_t last = SDL_GetTicks();
        uint64_t now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f;
        last = now;

        SDL_Event e;
        draco::input::begin_frame();

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;

            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                if (e.key.key == SDLK_ESCAPE)
                {
                    mouse_captured = !mouse_captured;

                    draco::input::set_mouse_captured(window, mouse_captured);
                }
            }

            draco::input::process_event(e);
        }

        static int dw = 1280; // Default width, it shouldn't be modified
        static int dh = 720; // Default height, it shouldn't be modified

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        if (w != dw || h != dh) {
            draco::rendering::rhi::resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
            draco::rendering::renderer::resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
            dw = w;
            dh = h;
        }

        camera.update(dt);

        auto cam = camera.get_camera();
        draco::rendering::renderer::begin_frame(cam);
        
        struct TestMesh
        {
            draco::rendering::mesh::MeshHandle handle;
            draco::scene::Transform transform;
        };

        TestMesh tests[] =
        { 
            { cube_mesh,     draco::scene::make_transform() },
            { plane_mesh,    draco::scene::make_transform() },
            { sphere_mesh,   draco::scene::make_transform() },
            { cylinder_mesh, draco::scene::make_transform() },
            { capsule_mesh,  draco::scene::make_transform() },
        };

        draco::scene::set_position(tests[0].transform, -12.0f, 0.0f, 0.0f);
        draco::scene::set_position(tests[1].transform,  -6.0f, 0.0f, 0.0f);
        draco::scene::set_position(tests[2].transform,   0.0f, 0.0f, 0.0f);
        draco::scene::set_position(tests[3].transform,   6.0f, 0.0f, 0.0f);
        draco::scene::set_position(tests[4].transform,  12.0f, 0.0f, 0.0f);

        draco::scene::set_rotation(tests[1].transform, -bx::kPiHalf, 0.0f, 0.0f); // Rotate the plane mesh

        for (auto& t : tests)
        {
            const auto* mesh = draco::rendering::mesh::get(t.handle);
            if (!mesh) continue;

            draco::rendering::rhi::RenderPacket p{};

            p.vertex_buffer = mesh->vbh;
            p.index_buffer  = mesh->ibh;

            p.pipeline        = pipeline;
            p.texture_handle  = tex;
            p.texture_unit    = 0;
            p.sampler_uniform = s_texColor;

            p.uniforms.push_back({ u_tint, tint, 1 });
            p.uniforms.push_back({ u_offset, offset, 1 });

            float model[16];
            draco::scene::compute_matrix(t.transform, model);

            std::memcpy(p.model, model, sizeof(model));

            draco::rendering::renderer::submit_entity(p, 0);
        }
        
        draco::rendering::renderer::end_frame();
    }
    draco::rendering::rhi::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
