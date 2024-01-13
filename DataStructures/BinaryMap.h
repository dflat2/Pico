#ifndef BINARY_MAP_H
#define BINARY_MAP_H

typedef struct BinaryMap_ BinaryMap;

BinaryMap* BinaryMap_CreateEmpty(int width, int height, int length);
void BinaryMap_Free(BinaryMap* binaryMap);
void BinaryMap_Set(BinaryMap* binaryMap, int x, int y, int z);
void BinaryMap_Unset(BinaryMap* binaryMap, int x, int y, int z);
int BinaryMap_Get(BinaryMap* binaryMap, int x, int y, int z);

#endif /* BINARY_MAP_H */
