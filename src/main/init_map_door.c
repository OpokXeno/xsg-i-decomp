#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", InitMapDoor);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", MAP_updateUnitDoor);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", HalfAutoDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", MjDoorFunc);

/* MAP_updateUnitDoor's handler for a double door: nothing to do here. */
void DoubleDoorFunc(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorStanbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorOpenOpeFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorOpenFuncSub);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorOpenNowFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorCloseOpeFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorStanbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorOpenOpeFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorOpenNowFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorCloseOpeFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", MjDoorStanbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", HalfAutoDoorOpenNowFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", DoorCommonFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", DoorOpenStanbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorDist);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorSwitch);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorPos);
