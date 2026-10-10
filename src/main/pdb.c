#include "common.h"

#include "pdb.h"

enum {
    PDB_MAGIC = 0x30304C46,
    PDB_VERSION_RELOCATED = 1
};

static char *getStrIndex(const char *string, int length, int character);

unsigned int strlen(const char *string);

char *strncpy(char *destination, const char *source, unsigned int count);

int strncmp(const char *left, const char *right, unsigned int count);

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

PDBEntryRecord *PDB_findFile(int class_id, char *path)
{
    char directory[256];
    char filename[256];
    char *filename_buffer;
    char *directory_end;
    unsigned int path_length;
    char *slash;
    int filename_length;
    int directory_length;
    PDBGroup *group;
    int group_count;
    int group_index;

    path_length = strlen(path);
    slash = getStrIndex(path, path_length, '/');
    if (slash != 0) {
        directory_length = slash - path;
        filename_length = path_length - (directory_length + 1);
        strncpy(directory, path, directory_length);
        slash++;
        filename_buffer = filename;
        strncpy(filename_buffer, slash, filename_length);
    } else {
        strncpy(directory, ".", 1);
        directory_length = 1;
        filename_length = path_length;
        filename_buffer = filename;
        strncpy(filename_buffer, path, filename_length);
    }
    directory_end = &directory[directory_length];
    group_index = 0;
    *directory_end = '\0';
    filename_buffer[filename_length] = '\0';

    PDB_getEntry(class_id, (void **)&group, &group_count);
    if (group_index < group_count) {
        do {
            PDBEntryRecord *entry;

            entry = group->entries;
            if (strncmp(directory, group->name, group->name_length) == 0) {
                int entries_remaining;

                entries_remaining = group->entry_count;
                while (entries_remaining > 0) {
                    if (strncmp(filename_buffer, entry->name, entry->name_length) == 0) {
                        return entry;
                    }
                    entry++;
                    entries_remaining--;
                }
            } else {
                entry += group->entry_count;
            }
            group = (PDBGroup *)entry;
            group_index++;
        } while (group_index < group_count);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/pdb", getStrIndex_002F5F30);
