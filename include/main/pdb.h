#ifndef INCLUDE_MAIN_PDB_H
#define INCLUDE_MAIN_PDB_H

#include "shared.h"

/* Functions and data used by other translation units. */
PDBEntryRecord *PDB_findFile(int class_id, char *path);
void PDB_getEntry(int class_id, void **groups, int *group_count);

#endif /* INCLUDE_MAIN_PDB_H */
