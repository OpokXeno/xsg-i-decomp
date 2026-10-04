#ifndef INCLUDE_MAIN_DB_LIGHT_WRITE_H
#define INCLUDE_MAIN_DB_LIGHT_WRITE_H

#include "shared.h"

/*
 * VW_getCursor copies the three floats stored at cursor+0x10/+0x14/+0x18
 * into a homogeneous output vector and forces w to 1.0f.
 *
 * Bounded users prove discrete facts only: VW_setCursorMode writes a mode
 * word at cursor+0, VW_setCursor writes the XYZ floats at
 * cursor+0x10/+0x14/+0x18, drawCursor reads cursor+0x10 as a translation
 * vector and writes 1.0f at cursor+0x1c, VW_setCursorFunc writes two words
 * at cursor+0x50/+0x54, and updateCursorMode2 copies an actor position into
 * cursor+0x10..+0x18. The complete 96-byte cursor object declaration,
 * its historical type name, its source file and its original TU are not
 * proven, so this header makes no size, member or layout claim beyond the
 * access below: the extern array is incomplete (no element count), and
 * index 1 selects the evidenced 16-byte position slot at +0x10 using only
 * the demonstrated HomogeneousVector type. Slot 0, slots 2..N and the
 * bytes at +0x04..+0x0F, +0x1C..+0x4F and +0x58..+0x5F are unclaimed here.
 */
extern HomogeneousVector cursor[];

#endif /* INCLUDE_MAIN_DB_LIGHT_WRITE_H */
