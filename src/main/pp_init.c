#include "common.h"
#include "shared.h"
#include "main/pp_init.h"

#define PARTICLE_GRAVITY (-0.05f)
#define PARTICLE_DAMPING (0.8f)

/* Zeroes the particle's positions and velocity, and sets its gravity and damping. */
void ppInit(PpParticle *particle)
{
    float zero = 0.0f;
    register float gravity = PARTICLE_GRAVITY;
    register float damping = PARTICLE_DAMPING;

    particle->damping = damping;
    particle->gravity = gravity;
    particle->position.z = zero;
    particle->position.y = zero;
    particle->position.x = zero;
    particle->previous_position.z = zero;
    particle->previous_position.y = zero;
    particle->previous_position.x = zero;
    particle->velocity.z = zero;
    particle->velocity.y = zero;
    particle->velocity.x = zero;
}

/* Saves the current position as the previous one and advances the position by the velocity. */
void ppNextStart(PpParticle *particle)
{
    particle->previous_position = particle->position;
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    particle->position.z += particle->velocity.z;
}

/* Derives the velocity from the position change since ppNextStart, scaled by damping; gravity is added to y. */
void ppNextEnd(PpParticle *particle)
{
    particle->velocity.x =
        (particle->position.x - particle->previous_position.x) * particle->damping;
    particle->velocity.y =
        (particle->position.y - particle->previous_position.y) * particle->damping +
        particle->gravity;
    particle->velocity.z =
        (particle->position.z - particle->previous_position.z) * particle->damping;
}

/* Keeps the particle's height at or above the floor (y = 0). */
void ppCheckWall(PpParticle *particle)
{
    if (particle->position.y < 0.0f) {
        particle->position.y = 0.0f;
    }
}

void ppMove(PpParticle *particle, const Vector4 *delta)
{
    particle->velocity.x += delta->x;
    particle->velocity.y += delta->y;
    particle->velocity.z += delta->z;
}

/* Places the particle at (x, y, z) with no movement since its last step. */
void ppSetPos(PpParticle *particle, float x, float y, float z)
{
    particle->previous_position.x = x;
    particle->previous_position.y = y;
    particle->previous_position.z = z;
    particle->position.x = x;
    particle->position.y = y;
    particle->position.z = z;
}
