#ifndef INCLUDE_MAIN_PARTY_H
#define INCLUDE_MAIN_PARTY_H

/* The party state is the 0x188-byte region embedded at SaveData + 0x10078. */
unsigned char *PartyDataGet(void);

/*
 * PartyDataInit resets the party state, loads the base technique records
 * (tecinit.bin) and the skill table (skilldat.bin) through the menu work
 * buffer, resets sound output, HDD, pad, radar and UMN mail state, clears the
 * item/weapon/bullet/accessory/event inventories, gives the starting money and
 * weapon 19, clears the simulation number and restarts the play clock.
 */
extern void PartyDataInit(void);

#endif /* INCLUDE_MAIN_PARTY_H */
