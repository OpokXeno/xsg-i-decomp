#include "common.h"
#include "shared.h"

typedef union PpVector4 {
    Vector4 vector;
    unsigned long long words[2];
} PpVector4;

typedef struct PpParticle {
    PpVector4 position;
    PpVector4 previous_position;
    PpVector4 velocity;
    float gravity;
    float damping;
} PpParticle;

extern const float D_004D8290;
extern const float D_004D8294;

void ppInit(PpParticle *particle)
{
    float zero = 0.0f;
    const float gravity = D_004D8290;
    const float damping = D_004D8294;

    particle->damping = damping;
    particle->gravity = gravity;
    particle->position.vector.z = zero;
    particle->position.vector.y = zero;
    particle->position.vector.x = zero;
    particle->previous_position.vector.z = zero;
    particle->previous_position.vector.y = zero;
    particle->previous_position.vector.x = zero;
    particle->velocity.vector.z = zero;
    particle->velocity.vector.y = zero;
    particle->velocity.vector.x = zero;
}

void ppNextStart(PpParticle *particle)
{
    particle->previous_position = particle->position;
    particle->position.vector.x += particle->velocity.vector.x;
    particle->position.vector.y += particle->velocity.vector.y;
    particle->position.vector.z += particle->velocity.vector.z;
}

void ppNextEnd(PpParticle *particle)
{
    particle->velocity.vector.x =
        (particle->position.vector.x - particle->previous_position.vector.x) * particle->damping;
    particle->velocity.vector.y =
        (particle->position.vector.y - particle->previous_position.vector.y) * particle->damping +
        particle->gravity;
    particle->velocity.vector.z =
        (particle->position.vector.z - particle->previous_position.vector.z) * particle->damping;
}

void ppCheckWall(PpParticle *particle)
{
    if (particle->position.vector.y < 0.0f) {
        particle->position.vector.y = 0.0f;
    }
}

void ppMove(PpParticle *particle, const Vector4 *delta)
{
    particle->velocity.vector.x += delta->x;
    particle->velocity.vector.y += delta->y;
    particle->velocity.vector.z += delta->z;
}

void ppSetPos(PpParticle *particle, float x, float y, float z)
{
    particle->previous_position.vector.x = x;
    particle->previous_position.vector.y = y;
    particle->previous_position.vector.z = z;
    particle->position.vector.x = x;
    particle->position.vector.y = y;
    particle->position.vector.z = z;
}
