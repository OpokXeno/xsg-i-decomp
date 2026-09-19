/*
 * TU-local declarations of main/tu270 (src/main/map_create_unit_peer.c).
 */

#ifndef SRC_MAIN_MAP_CREATE_UNIT_PEER_H
#define SRC_MAIN_MAP_CREATE_UNIT_PEER_H

#include "shared.h"

/*
 * This TU's view of one entry of the game-wide MapUnit[] array (0x300 bytes
 * apart; MAP_initUnitSequance stores MAP_updateUnitDefault as each entry's
 * +0x04 update callback, and src/main/init_drill.c and
 * src/main/init_uwamono_sys.c keep their own views of the same record).
 * The type is not named MapUnit because that is the array's own symbol.
 * Only the members MAP_updateUnitDefault (0x00320a80) reads or writes are
 * named: `position` is loaded/stored a field at a time (+0x10/+0x14/+0x18)
 * and also handed whole to xglMatrixStackTrans; `rotation` supplies the
 * three xglMatrixStackRot{X,Y,Z} angles (+0x20/+0x24/+0x28); `scale` and
 * `matrix` are passed whole to xglMatrixStackScale/xglMatrixStackSave.
 */
typedef struct MapUnitRecord {
    unsigned int flags; /* +0x00, see MAP_UNIT_FOLLOW_GROUND */
    unsigned char unmodeled_04[0x0c];
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale;    /* +0x30 */
    Matrix4 matrix;   /* +0x40 */
} MapUnitRecord;

/* flags bit: snap position.y to the ground undulation under x/z each update */
#define MAP_UNIT_FOLLOW_GROUND 0x100

/* Undulate.c (main); no header is published for it yet (also declared this
 * way in src/main/near_dir.c and src/ov01/unit_cmd.h). */
extern float UnduGet(float x, float z);

/*
 * Defined in src/main/xgl_2.c (main/tu100) with this spelling, but not in
 * include/main/xgl_2.h: src/ov02/umn_procurator.h declares it as
 * `void xglMatrixStackTrans(float *)`, and header_harvest shares no name
 * with divergent spellings (reports/header-harvest-conflicts.md).
 */
extern void xglMatrixStackTrans(const float translation[4]);

#endif /* SRC_MAIN_MAP_CREATE_UNIT_PEER_H */
