#ifndef EXAMPLE_H
#define EXAMPLE_H

struct Example {
    int ex;
};

struct Example* Example_Create(int ex);
int Example_GetEx(const struct Example* instance);
void Example_SetEx(struct Example* instance, int newEx);

#endif // EXAMPLE_H
