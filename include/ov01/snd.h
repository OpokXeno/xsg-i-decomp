#ifndef INCLUDE_OV01_SND_H
#define INCLUDE_OV01_SND_H

#include "shared.h"
#include "ov01/battle_init.h"

/* Functions and data used by other translation units. */

/* sndConvSpid (defined in src/ov01/snd.c). */
int sndConvSpid(BattleUnit *unit, int mode, int tecId, int itmId);

/* sndSePlay (defined in src/ov01/snd.c): the unit the sound belongs to, the
 * sound-effect id and the sound number. */
int sndSePlay(BattleUnit *unit, int id, int no);

/* sndSeTransPlay (defined in src/ov01/snd.c): loads and transfers the bank of
 * seId for unit, then queues sndSePlay with pan once the transfer completes. */
void sndSeTransPlay(BattleUnit *unit, int seId, int pan);

#endif /* INCLUDE_OV01_SND_H */
