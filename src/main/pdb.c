#include "common.h"
#include "pdb.h"

enum {
    PDB_MAGIC = 0x30304C46,
    PDB_VERSION_RELOCATED = 1
};

void PDB_getEntry(int class_id, void **groups, int *group_count)
{
    PDBHeader *header;
    PDBGroup *group;

    header = (PDBHeader *)class_id;
    if (header->magic != PDB_MAGIC) {
        *groups = 0;
        *group_count = 0;
        return;
    }

    group = header->groups;
    *groups = group;
    *group_count = header->group_count;

    if (header->version != PDB_VERSION_RELOCATED) {
        int group_index;

        for (group_index = 0; group_index < header->group_count; group_index++) {
            PDBEntryRecord *entry;
            int entries_remaining;

            group->name = &((char *)header)[(unsigned int)group->name];
            entry = group->entries;
            entries_remaining = group->entry_count;
            if (entries_remaining > 0) {
                do {
                    entry->name = &((char *)header)[(unsigned int)entry->name];
                    entry->data = &((char *)header)[(unsigned int)entry->data];
                    entry = &entry[1];
                    entries_remaining--;
                } while (entries_remaining > 0);
            }
            group = (PDBGroup *)entry;
        }
        header->version = PDB_VERSION_RELOCATED;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/pdb", PDB_findFile);

INCLUDE_ASM("asm/main/nonmatchings/pdb", getStrIndex_002F5F30);
