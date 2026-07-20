#ifdef WASM
#include "mudclient.h"
#include <js/glue.h>
#include <js/dom_pk_codes.h>

// NOTE: temp pdclib fix
double atof( const char * nptr ) {
    return JS_parseFloat(nptr, strlen(nptr));
}

static bool onmousewheel(void* userdata, double deltaY) {
    mudclient *mud = userdata;
    if (mud->options->mouse_wheel) {
        mud->mouse_scroll_delta = (deltaY > 0 ? -1 : 1);

        // TODO add horizontal scroll to wasmlite but i'm unable to test it
        // int direction = deltaX > 0 ? 1 : -1;
        // mud->camera_rotation = (mud->camera_rotation + (direction * 3)) & 0xff;
    }
    return 0;
}


static int mouse_x, mouse_y;
static bool onmousemove(void *userdata, int x, int y) {
    mudclient *mud = userdata;
    mouse_x = x;
    mouse_y = y;
    mudclient_mouse_moved(mud, x, y);

    return 0;
}

static void onmouse(void *userdata, bool pressed, int button) {
    mudclient *mud = userdata;

    button += 1; // match sdl buttons

    if (pressed) {
        mudclient_mouse_pressed(mud, mouse_x, mouse_y, button);
    } else {
        mudclient_mouse_released(mud, mouse_x, mouse_y, button);
    }
}

static int convert_pk(mudclient *mud, int ch, int code, int modifiers, bool pressed) {
    switch(code) {
    case DOM_PK_ARROW_LEFT:
        code = K_LEFT;
        break;
    case DOM_PK_ARROW_RIGHT:
        code = K_RIGHT;
        break;
    case DOM_PK_ARROW_UP:
        code = K_UP;
        break;
    case DOM_PK_ARROW_DOWN:
        code = K_DOWN;
        break;
    case DOM_PK_PAGE_UP:
        code = K_PAGE_UP;
        break;
    case DOM_PK_PAGE_DOWN:
        code = K_PAGE_DOWN;
        break;
    case DOM_PK_HOME:
        code = K_HOME;
        break;
    case DOM_PK_F1:
        code = K_F1;
        break;
    case DOM_PK_ENTER:
        code = K_ENTER;
        ch = '\r';
        break;
    case DOM_PK_BACKSPACE:
        code = K_BACKSPACE;
        ch = '\b';
        break;
    case DOM_PK_ESCAPE:
        code = K_ESCAPE;
        break;
    case DOM_PK_TAB:
        code = K_TAB;
        ch = '\t';
        break;
    case DOM_PK_SLASH:
        code = K_FWD_SLASH;
        break;
    case DOM_PK_MINUS:
        code = K_MINUS;
        break;
    case DOM_PK_EQUAL:
        if (modifiers & KMOD_SHIFT) {
            code = K_PLUS;
        }
        break;
    case DOM_PK_PERIOD:
        code = K_PERIOD;
        break;

    case DOM_PK_0:
        code = K_1;
        break;
    case DOM_PK_1:
        code = K_1;
        break;
    case DOM_PK_2:
        code = K_2;
        break;
    case DOM_PK_3:
        code = K_3;
        break;
    case DOM_PK_4:
        code = K_4;
        break;
    case DOM_PK_5:
        code = K_5;
        break;
    case DOM_PK_6:
        code = K_6;
        break;
    case DOM_PK_7:
        code = K_7;
        break;
    case DOM_PK_8:
        if (modifiers & KMOD_SHIFT) {
            code = K_ASTERISK;
        } else {
            code = K_8;
        }
        break;
    case DOM_PK_9:
        code = K_9;
        break;
    }

    if (pressed) {
        mudclient_key_pressed(mud, code, ch);
    } else {
        mudclient_key_released(mud, code);
    }
    return 0;
}

static bool onkey(void *userdata, bool pressed, int key, int code, int modifiers) {
    mudclient *mud = userdata;

    int rc = convert_pk(mud, key, code, modifiers, pressed);

    return rc;
}

// TODO move to own file with other crap
void mudclient_start_application(mudclient *mud, char *title) {
    JS_setTitle(title);
    JS_createCanvas(mud->game_width, mud->game_height, "2d");
    JS_addMouseEventListener(mud, onmouse, onmousemove, onmousewheel);
    JS_addKeyEventListener(mud, onkey);
}

void mudclient_poll_events(mudclient *mud) {
}
#endif
