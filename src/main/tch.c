#include "common.h"
#include "shared.h"
#include "tch.h"

INCLUDE_ASM("asm/main/nonmatchings/tch", getInfoAddr);

static void getInfoAddr(TimeChart *chart);

/*
 * TCH_getInfoID: linear search of chart's record index for the entry named
 * "name" (comparing at most "length" characters, or strlen(name) of them
 * when the caller passes a negative length), returning its index or -1.
 * getInfoAddr (this TU, still assembly) performs chart's one-time pointer
 * relocation on the first call.
 */
int TCH_getInfoID(TimeChart *chart, const char *name, int length)
{
    int index;
    int searchLength;
    const TchEntry *entry;

    searchLength = length;
    if (chart->relocated == 0) {
        getInfoAddr(chart);
    }
    if (searchLength < 0) {
        searchLength = strlen(name);
    }
    entry = chart->records;
    index = 0;
    if (chart->recordCount != 0) {
        while (strncmp(entry->name, name, searchLength) != 0) {
            index += 1;
            entry = (const TchEntry *)((const char *)entry + entry->wordCount * 4 + 8);
            if (index >= chart->recordCount) {
                goto not_found;
            }
            /*
             * Unreachable, but keeps the compiler from rotating this loop
             * into a pretest shape that duplicates the strncmp/index/entry
             * work ahead of it.
             */
            continue;
            break;
        }

        return index;
    }
not_found:
    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/tch", TCH_getInfo);
