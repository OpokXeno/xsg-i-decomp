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

void PartyFriendOff(int id);
int PartyFriendCheck(int id);
void PartyOutFriendOn(int id);
void PartyOutFriendOff(int id);
int PartyOutFriendCheck(int id);
int PartyFriendLockCheck(int id, int flag);

/* Collects party member ids into out[] and returns how many were stored.
 * mode 0: the attack line followed by the reserve line,
 * mode 1: every friend that is not locked out (at most six),
 * mode 2: every friend that has joined (at most six). */
int PartyAllPartyGet(int *out, int mode);

#endif /* INCLUDE_MAIN_PARTY_H */
