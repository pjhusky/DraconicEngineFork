module;

#include <cmath>

#include <bx/math.h>

module scene.transform;

namespace draco::scene
{
    Transform make_transform()
    {
        Transform t;

        t.position[0] = 0.0f;
        t.position[1] = 0.0f;
        t.position[2] = 0.0f;

        t.rotation[0] = 0.0f;
        t.rotation[1] = 0.0f;
        t.rotation[2] = 0.0f;

        t.scale[0] = 1.0f;
        t.scale[1] = 1.0f;
        t.scale[2] = 1.0f;

        t.dirty = true;

        return t;
    }

    void mark_dirty(Transform& t)
    {
        t.dirty = true;
    }

    void set_position(Transform& t, float x, float y, float z)
    {
        t.position[0] = x;
        t.position[1] = y;
        t.position[2] = z;
        t.dirty = true;
    }

    void set_rotation(Transform& t, float x, float y, float z)
    {
        t.rotation[0] = x;
        t.rotation[1] = y;
        t.rotation[2] = z;
        t.dirty = true;
    }

    void set_scale(Transform& t, float x, float y, float z)
    {
        t.scale[0] = x;
        t.scale[1] = y;
        t.scale[2] = z;
        t.dirty = true;
    }

    void compute_matrix(const Transform& t, float out[16])
    {
        float translation[16];
        float rx[16];
        float ry[16];
        float rz[16];
        float scale[16];
        float temp[16];

        bx::mtxIdentity(out);

        bx::mtxScale(scale, t.scale[0], t.scale[1], t.scale[2]);

        bx::mtxRotateX(rx, t.rotation[0]);
        bx::mtxRotateY(ry, t.rotation[1]);
        bx::mtxRotateZ(rz, t.rotation[2]);

        bx::mtxTranslate(translation, t.position[0], t.position[1], t.position[2]);

        // scale * rotation * translation
        bx::mtxMul(temp, scale, rx);
        bx::mtxMul(temp, temp, ry);
        bx::mtxMul(temp, temp, rz);
        bx::mtxMul(out, temp, translation);
    }
}