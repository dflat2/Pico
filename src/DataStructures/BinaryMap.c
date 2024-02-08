#include <stdlib.h>

#include "BinaryMap.h"

typedef char byte;

typedef struct BinaryMap_ {
    byte* content;
    int width, height, length;
} BinaryMap;

BinaryMap* BinaryMap_CreateEmpty(int width, int height, int length) {
    int volume = width * height * length;
    int mapSize = width * height * length / 8;

    if (volume % 8 != 0) {
        mapSize += 1;
    }

    byte* content = (byte*)calloc(mapSize, sizeof(byte));

    if (content == NULL) {
        return NULL;
    }

    BinaryMap* map = malloc(sizeof(BinaryMap));

    if (map == NULL) {
        free(content);
        return NULL;
    }

    map->width = width;
    map->height = height;
    map->length = length;
    map->content = content;

    return map;
}

void BinaryMap_Free(BinaryMap* map) {
    free(map->content);
    free(map);
}

void BinaryMap_Set(BinaryMap* map, int x, int y, int z) {
    int position = x + y * map->length + map->length * map->height * z;
    map->content[position / 8] |= (1 << (position % 8));
}

void BinaryMap_Unset(BinaryMap* map, int x, int y, int z) {
    int position = x + y * map->length + map->length * map->height * z;
    map->content[position / 8] &= ~(1 << (position % 8));
}

int BinaryMap_Get(BinaryMap* map, int x, int y, int z) {
    int position = x + y * map->length + map->length * map->height * z;
    return (map->content[position / 8] >> (position % 8)) & 1;
}
