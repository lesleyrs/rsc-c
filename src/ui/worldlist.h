#ifndef WORLDLIST_H
#define WORLDLIST_H

#include "../mudclient.h"

#ifdef __NDS__
#define WORLDLIST_SIZE 10
#else
#define WORLDLIST_SIZE (1 << 8)
#endif

void worldlist_new(mudclient *mud);
void worldlist_handle_mouse(mudclient *mud);
#endif
