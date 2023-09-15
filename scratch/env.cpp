
#include "env.h"
#include <stdio.h>

int lookup(char *key) {
    auto it = Environment.find(key);
    if (it == Environment.end()) {
        fprintf(stderr, "Could not find %s in environemnt\n", key);
    }
    return it->second;
}