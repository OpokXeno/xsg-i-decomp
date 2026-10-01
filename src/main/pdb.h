#ifndef SRC_MAIN_PDB_H
#define SRC_MAIN_PDB_H

/* Variable-length PDB groups contain entry_count records after the header. */
typedef struct PDBEntryRecord {
    char *name;
    unsigned short name_length;
    unsigned short unmodeled_06;
    void *data;
    int data_length;
} PDBEntryRecord;

typedef struct PDBGroup {
    char *name;
    unsigned short name_length;
    unsigned short entry_count;
    PDBEntryRecord entries[1];
} PDBGroup;

typedef struct PDBHeader {
    unsigned int magic;
    unsigned short version;
    unsigned short group_count;
    unsigned char unmodeled_08[4];
    PDBGroup groups[1];
} PDBHeader;

void PDB_getEntry(int class_id, void **groups, int *group_count);

#endif
