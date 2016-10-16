#ifndef JSON_READ_H
#define JSON_READ_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "raycast.h"

typedef struct objMem {
  Object *objArray;
  size_t objNumber;
} objMem;

struct objMem read_scene(FILE* json);
#endif
