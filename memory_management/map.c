#include "map.h"

mapPtr createMap() {
    mapPtr map;
    map = (mapPtr) malloc(sizeof(Map));

    map->isValid = false;
    map->frame = -1;
    return map;
}