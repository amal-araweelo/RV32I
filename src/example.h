#ifndef EXAMPLE_H
#define EXAMPLE_H

struct Example {
    int ex;
};

void Example_init(struct Example *example, int ex);

int Example_getEx(const struct Example *example);

void Example_setEx(struct Example *example, int newEx);

#endif