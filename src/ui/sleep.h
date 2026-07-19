#ifndef _H_SLEEP

#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifndef WASM
#include <strings.h>
#endif

#include "../mudclient.h"

void mudclient_draw_sleep(mudclient *mud);
void mudclient_handle_sleep_input(mudclient *mud);

#endif
