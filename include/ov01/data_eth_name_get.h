#ifndef INCLUDE_OV01_DATA_ETH_NAME_GET_H
#define INCLUDE_OV01_DATA_ETH_NAME_GET_H

/* Name-table accessors of ov01/tu011 (src/ov01/data_eth_name_get.c). Each
 * returns the address of a name-table pointer, so the name is read as
 * `*dataEthNameGet(id)` and the help text as `dataEthNameGet(id)[1]`.
 * dataEthNameGet prints an error and returns 0 for an id <= 0.
 * dataStatNameGet returns 0 unless statType < 9, although its table holds
 * only eight entries (statType 8 reads past it, as the original does). */
const char **dataEthNameGet(int ethId);
const char **dataStatNameGet(int statType, int index);

#endif
