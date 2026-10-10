#include "common.h"

#include "exm_1.h"

static void CheckSkirtCollisionSub(Actor *actor, float *position, float *matrix, u32 flags);

/* The body accesses only these two fields of the larger actor record. */

typedef struct HairSelectionActor {
    unsigned char unmodeled_000[0x4c2];
    s16 hair_type_code;
    unsigned char unmodeled_4c4[0x80c - 0x4c4];
    signed char *hair_joint_name;
} HairSelectionActor;

/* Palette records are 0x150-byte tables; only their base addresses are used here. */

static u32 kosmoscol[84] = {
    [0] = 0x00010030, [4] = 0x3CCCCCCD, [5] = 0xBD8F5C29,
    [7] = 0x3E000000, [12] = 0x00050030, [16] = 0xBD0F5C29,
    [17] = 0xBDCCCCCD, [19] = 0x3E051EB8, [24] = 0x00020033,
    [31] = 0x3DB851EC, [32] = 0x3DCCCCCD, [35] = 0x3D8F5C29,
    [36] = 0x0002003F, [43] = 0x3DB851EC, [44] = 0x3DCCCCCD,
    [47] = 0x3D8F5C29, [48] = 0x00040027, [52] = 0x3DCCCCCD,
    [53] = 0x3EE66666, [55] = 0x3F0CCCCD, [60] = 0x0004000D,
    [64] = 0x3DCCCCCD, [65] = 0xBEBD70A4, [67] = 0x3F000000
};

static u32 shelleycol[84] = {
    [0] = 0x00010030, [4] = 0x3CF5C28F, [5] = 0xBDA3D70A,
    [7] = 0x3DA3D70A, [12] = 0x00020033, [16] = 0xBCA3D70A,
    [19] = 0x3DCCCCCD, [20] = 0x3DCCCCCD, [23] = 0x3D99999A,
    [24] = 0x0002003F, [28] = 0xBCA3D70A, [31] = 0x3DCCCCCD,
    [32] = 0x3DCCCCCD, [35] = 0x3D99999A, [36] = 0x00010027,
    [40] = 0x3DCCCCCD, [41] = 0x3C23D70A, [42] = 0x3D4CCCCD,
    [43] = 0x3DA3D70A, [48] = 0x00010027, [52] = 0x3DCCCCCD,
    [53] = 0x3C23D70A, [54] = 0xBD4CCCCD, [55] = 0x3DA3D70A,
    [60] = 0x00040027, [64] = 0x3DCCCCCD, [65] = 0x3F051EB8,
    [67] = 0x3F266666
};

static u32 humancol[36] = {
    [0] = 0x00010030, [4] = 0x3C23D70A, [5] = 0xBD8F5C29,
    [7] = 0x3DCCCCCD, [12] = 0x0004002A, [16] = 0x3D4CCCCD,
    [17] = 0x3E19999A, [19] = 0x3E99999A
};

static float skirtColliSize = 1.0f;

static float skirtColliSizeB = 1.0f;

static float skirtColliSizeS = 1.0f;

static float skirtColliFootRad = 0.05f;

static int rootBorn = 0;

/* The body accesses only these two fields of the larger actor record. */

/* Palette records are 0x150-byte tables; only their base addresses are used here. */

static signed char GetLastWord(signed char *text)
{
    int index;

    index = 0;
    if (text != 0) {
        if (text[0] != 0 && !(text[0] & 0x80)) {
            do {
                index++;
            } while (text[index] != 0 && !(text[index] & 0x80));
        }
        if (index != 0) {
            return text[index - 1];
        }
        return 0;
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_PresetHair);

static u32 GetHairTypeSelection(u32 *flags, u32 *palette, HairSelectionActor *actor)
{
    u32 result;
    signed char *jointName;
    signed char kind;

    jointName = actor->hair_joint_name;
    result = 0;
    *flags = 0;
    if (jointName != 0) {
        kind = jointName[0];
        switch (kind) {
        case 'P':
            {
                float unitSize = 1.0f;
                float sizeValue = 0.8f;
                float footRadius = 0.05f;

                *palette = (u32)humancol;
                skirtColliSize = sizeValue;
                skirtColliSizeS = (skirtColliSizeB = unitSize);
                skirtColliFootRad = footRadius;
                switch (actor->hair_type_code) {
                case 'J':
                    *flags = 0x4400;
                    result = 16;
                    break;
                case 'O':
                    *flags = 0x0c00;
                    result = 16;
                    break;
                case 'T':
                    result = 8;
                    break;
                case 'W':
                case '[':
                    result = 5;
                    break;
                case '_':
                case 'd':
                    result = 7;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'S':
            {
                float unitSize = 1.0f;

                *palette = (u32)shelleycol;
                skirtColliSize = unitSize;
                skirtColliSizeB = unitSize;
                skirtColliSizeS = unitSize;
                skirtColliFootRad = 0.05f;
                switch (actor->hair_type_code) {
                case 'J':
                    *flags = 0x4400;
                    result = 16;
                    break;
                case 'M':
                    *flags = 0x0c00;
                    result = 16;
                    break;
                case 'P':
                    *flags = 1;
                    result = 2;
                    break;
                case 'V':
                case 'Z':
                    result = 5;
                    break;
                case '^':
                case 'e':
                    result = 4;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'B':
            {
                float unitSize = 1.0f;

                skirtColliFootRad = 0.15f;
                skirtColliSize = unitSize;
                skirtColliSizeB = unitSize;
                skirtColliSizeS = unitSize;
                switch (actor->hair_type_code) {
                case 'q':
                    *flags = 0x5500;
                    result = 16;
                    break;
                case 'u':
                    *flags = 0x4500;
                    result = 16;
                    break;
                case 'y':
                    *flags = 0x6500;
                    result = 16;
                    break;
                case 0x7d:
                    *flags = 0x11d00;
                    result = 16;
                    break;
                case 0x80:
                    *flags = 0x12d00;
                    result = 16;
                    break;
                case 0x83:
                case 0x87:
                case 0x8b:
                case 0x96:
                case 0x9b:
                    result = 4;
                    break;
                case 0x8e:
                case 0x92:
                    result = 5;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'K':
            *palette = (u32)kosmoscol;
            switch (actor->hair_type_code) {
            case 'J':
            case 'M':
            case 'P':
            case 'S':
                result = 12;
                break;
            case 'V':
                *flags |= 1;
                result = 3;
                break;
            case 0x5c:
            case '_':
                result = 5;
                break;
            case 'b':
            case 'g':
                *flags |= 2;
                result = 4;
                break;
            case 'l':
                result = 8;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_CheckExCollision);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", CheckSkirtCollisionSub);

static void EXM_CheckSkirtCollision(Actor *actor, float *position, float *matrix, u32 flags)
{
    if (flags & 0x80) {
        rootBorn = 4;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        rootBorn = 5;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        return;
    }
    if (flags & 0x100) {
        rootBorn = 6;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        rootBorn = 7;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        return;
    }
    rootBorn = 0;
    CheckSkirtCollisionSub(actor, position, matrix, flags);
    rootBorn = 1;
    CheckSkirtCollisionSub(actor, position, matrix, flags);
    if (flags & 0x8000) {
        rootBorn = 2;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        rootBorn = 3;
        CheckSkirtCollisionSub(actor, position, matrix, flags);
        return;
    }
}

static void AdjustJrCoatWidth(float width, Vector4 *position, const Vector4 *joint_position)
{
    Vector4 difference;
    float adjusted_x;
    float adjusted_y;
    float adjusted_z;
    float adjusted_width;

    difference.x = position->x - joint_position->x;
    difference.y = position->y - joint_position->y;
    difference.z = position->z - joint_position->z;
    xglVectorLength(&difference.w, &difference);
    if (difference.w == 0.0f) {
        difference.w = 1.0f;
    }
    adjusted_width = width / difference.w;
    adjusted_x = ((joint_position->x + difference.x * adjusted_width) * 0.5f) +
                 (position->x * 0.5f);
    adjusted_y = ((joint_position->y + difference.y * adjusted_width) * 0.5f) +
                 (position->y * 0.5f);
    adjusted_z = ((joint_position->z + difference.z * adjusted_width) * 0.5f) +
                 (position->z * 0.5f);
    position->x = adjusted_x;
    position->y = adjusted_y;
    position->z = adjusted_z;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_CalcExMotion);

void EXM_InitMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
        actor->moved_hair_delay = 0.5f;
        actor->moved_hair_sway[0] = 0.0f;
        actor->moved_hair_sway[1] = 0.0f;
        actor->moved_hair_sway[2] = 0.0f;
        actor->moved_hair_sway[3] = 0.0f;
    }
}

void EXM_OnMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
    }
}

void EXM_OffMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 0;
    }
}

void EXM_SetDelayMovedHair(Actor *actor, float delay)
{
    if (actor != 0) {
        actor->moved_hair_delay = delay;
    }
}
