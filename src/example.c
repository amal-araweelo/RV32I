#include "example.h"

struct Example {
    int ex;
};

struct Example* Example_create(int ex) {
    struct Example* example = malloc(sizeof(struct Example));
    example->ex = ex;
    return example;
}

int Example_getEx(const struct Example* example) {
    return example->ex;
}

void Example_setEx(struct Example* example, int newEx) {
    example->ex = newEx;
}
