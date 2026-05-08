module;

#include <cstdint>
#include <cstring>
#include <cmath>

#include <bx/math.h>

export module scene.transform;

export namespace draco::scene
{
    struct Transform
    {
        float position[3];
        float rotation[3]; // Euler (radians)
        float scale[3];

        bool dirty = true;
    };

    // Creates a default identity transform
    Transform make_transform();

    // Recompute matrix from transform (column-major, bx compatible)
    void compute_matrix(const Transform& t, float out[16]);

    // Helpers
    void set_position(Transform& t, float x, float y, float z);
    void set_rotation(Transform& t, float x, float y, float z);
    void set_scale(Transform& t, float x, float y, float z);

    void mark_dirty(Transform& t);
}