#ifndef INCLUDE_OV12_RG_SHOT_THREAD_H
#define INCLUDE_OV12_RG_SHOT_THREAD_H

/*
 * The weapon handle owned by rg_weapon.c, opaque here.
 */
typedef struct RgWeapon RgWeapon;

typedef struct RgShotMotCont RgShotMotCont;

typedef struct RgShotThread {
    RgWeapon *weapon;
    RgShotMotCont *motCont;
    int motionId;
    int status;
    float elapsedTime;
    int active;
    float frame;
    float duration;
    unsigned int shotCount;
} RgShotThread;

#endif /* INCLUDE_OV12_RG_SHOT_THREAD_H */
