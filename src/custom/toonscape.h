#ifndef TOONSCAPE_H
#define TOONSCAPE_H
#include <stdint.h>

int toonscape_avoid_load(int);
int32_t apply_toonscape(int32_t);
#ifdef __NDS__
bool toonscape_allow_load(int id);
#endif
#endif
