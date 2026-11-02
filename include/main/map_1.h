#ifndef INCLUDE_MAIN_MAP_1_H
#define INCLUDE_MAIN_MAP_1_H

void MapDraw(void);

/* Functions and data used by other translation units. */
void MapChange(int map_id);
char *MapGetName(int map_no);
extern void MapChangeFadeSet(void);
void MapInit(void);
const char *MAP_getPath(void);
extern void MapChange2(int map_id);
extern void MapReset(void);
int MapGetNo(void);

#endif /* INCLUDE_MAIN_MAP_1_H */
