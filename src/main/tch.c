#include "common.h"

#include "shared.h"

#include "tch.h"

static void getInfoAddr(TimeChart *chart);

struct PlayTCHCurveSet;

static void getInfoAddr(TimeChart *chart)
{
    TchEntry *entry;
    u32 *pointerOffset;
    u32 chartAddress;
    int recordCount;
    int remainingRecords;
    int pointerCount;
    int remainingPointers;

    recordCount = chart->recordCount;
    entry = chart->records;
    chart->relocated = 1;
    chartAddress = (u32)chart;
    if (recordCount != 0) {
        remainingRecords = recordCount;
        do {
            if (entry->name != 0) {
                entry->name = (char *)chartAddress + (u32)entry->name;
            }

            pointerCount = entry->wordCount;
            if (pointerCount != 0) {
                pointerOffset = entry->pointerOffsets;
                remainingPointers = pointerCount;
                do {
                    *pointerOffset = chartAddress + *pointerOffset;
                    pointerOffset++;
                    remainingPointers--;
                } while (remainingPointers != 0);
            }

            remainingRecords--;
            entry = (TchEntry *)&entry->pointerOffsets[pointerCount];
        } while (remainingRecords != 0);
    }
}

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
        do {
            if (strncmp(entry->name, name, searchLength) == 0) {
                return index;
            }
            index += 1;
            entry = (const TchEntry *)((const char *)entry + entry->wordCount * 4 + 8);
        } while (index < chart->recordCount);
    }
    return -1;
}

struct PlayTCHCurveSet *TCH_getInfo(void *timeChart, int index)
{
    TimeChart *chart;
    int entryIndex;
    void *entry;

    chart = timeChart;
    if (chart->relocated == 0) {
        getInfoAddr(chart);
    }
    if (index < 0 || index >= chart->recordCount) {
        return 0;
    }

    entry = (struct PlayTCHCurveSet *)chart->records;
    entryIndex = 0;
    while (entryIndex < chart->recordCount) {
        if (index == entryIndex) {
            return entry;
        }
        entry = &((TchEntry *)entry)->pointerOffsets[((TchEntry *)entry)->wordCount];
        entryIndex++;
    }
    return 0;
}
