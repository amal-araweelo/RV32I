#include "example.h"
#include <stdlib.h>

struct Example *Example_Create(int ex) {
	struct Example *instance =
	    (struct Example *)malloc(sizeof(struct Example));
	if (instance) {
		instance->ex = ex;
	}
	return instance;
}

int Example_GetEx(const struct Example *instance) { return instance->ex; }

void Example_SetEx(struct Example *instance, int newEx) {
	instance->ex = newEx;
}
