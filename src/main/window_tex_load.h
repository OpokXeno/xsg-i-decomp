/*
 * TU-local declarations of main/tu160 (src/main/window_tex_load.c).
 */

#ifndef SRC_MAIN_WINDOW_TEX_LOAD_H
#define SRC_MAIN_WINDOW_TEX_LOAD_H

/*
 * The UMN database's saved state: the 0x9c-byte window of the save record
 * that build/main/aliases.ld names `UmnDataBaseStateData = SaveData + 66140`
 * (0x1025c).  Two bit sets of the same 0x1d monster ids (ids 0x22..0x3e, the
 * `monster_id - 0x22` and `< 0x1d` test both functions of this pair share)
 * are evidenced, each read and written one byte at a time as
 * `bits[id / 8] & (1 << (id % 8))`:
 * - +0x88, whether the monster has been met: written by
 *   UmnDataBaseMonsterSet (main:0x002761f0, `lbu/sb 0x0($5)` with
 *   $5 = base + id/8 + 0x88) and read by UmnDataBaseMonsterCheck
 *   (main:0x00276248, `lbu $2,0x88($3)`).
 * - +0x98, whether it has been scanned: the same pair of functions one
 *   displacement further on, UmnDataBaseAnalisisSet (main:0x00276298,
 *   `addiu $5,$3,0x98`) and UmnDataBaseAnalisisCheck (main:0x002762f0,
 *   `lbu $2,0x98($3)`), which this candidate recovers as C.  It is what the
 *   0x9c extent of the published UmnDataBaseStateData declaration ends on.
 * The spans around them stay unmodeled byte ranges with no meaning claimed
 * (docs/naming.md), and this is what was searched to leave them so. Every
 * access through this window's base in main and the overlays was enumerated:
 * the head (+0x00, +0x02, +0x03, +0x10..+0x12, +0x2a, +0x31, +0x56, +0x59,
 * +0x84, +0x86) belongs to the UMN mail and plugin screens (UmnMail,
 * UmnTopMenu, UmnMailBoxSet, UmnMailFolderSet, UmnMailAttachSet, UmnPlugin,
 * Java_xeno_util_Runtime_mailFlag), which this allocation does not recover,
 * and +0x8c..+0x97 is touched by nothing at all. The nearby +0x10254,
 * +0x102dc, +0x102e2 and +0x10304 accesses (MenuSaveDataGet, MenuBoxMoneyGet,
 * tyaUmlDispLoad, UmnMailDataGet) address the save record directly rather
 * than this alias, so they witness neighbours of the window, not members.
 */
typedef struct UmnDataBase {
    unsigned char _unmodeled_00[0x88];      /* +0x00..+0x87 */
    unsigned char monster_discovered[4];    /* +0x88: one bit per monster id */
    unsigned char _unmodeled_8c[0x0c];      /* +0x8c..+0x97 */
    unsigned char monster_analysed[4];      /* +0x98: one bit per monster id */
} UmnDataBase;

/*
 * The fourth argument of xglCdReadFile is the completion-callback slot that
 * xglCdReadFilePart stores at +0x0c of the read request (main:0x0021DFAC).
 * It selects a default callback for the small values it tests first - 0 takes
 * the global default, 1 takes xglCdDummyCallback (0x0021D698) and 2 takes
 * xglCdDefaultCallback (0x0021D690), main:0x0021DEFC..0x0021DF50 - and uses
 * any other value as the callback address itself.  The slot is an int, so a
 * caller that supplies its own callback passes its address as one.
 */
int MenuLoadFile(const char *name, void *buffer);

extern int menuCallback(int event);

void UmnDataBaseMonsterSet(int monster_id);

int UmnDataBaseMonsterCheck(int monster_id);

extern unsigned char UmnDataBaseStateData[0x9c];

int MenuLoadSync(void);

extern int MenuLoadCount;

void MenuLoadInit(void);

int MenuModelInit(int work_start);

#endif /* SRC_MAIN_WINDOW_TEX_LOAD_H */
