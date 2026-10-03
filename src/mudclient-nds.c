#ifdef __NDS__
#include <nds.h>
#include <filesystem.h>
#include <dswifi9.h>

#include "mudclient.h"

void mudclient_start_application(mudclient *mud, char *title) {
    // NOTE clearing vram is only needed for twilightmenu++ loader
    // from blocksds https://codeberg.org/blocksds/libnds/src/commit/4674cdfed005eb9a3699f4896e5c587ebc8b6a09/source/arm9/video/video.c#L63-L87
    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBanks_EFG(VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
    vramSetBankI(VRAM_I_LCD);

    dmaFillWords(0, BG_PALETTE, 2 * 1024); // Clear main and sub palette
    dmaFillWords(0, OAM, 2 * 1024);        // Clear main and sub OAM
    dmaFillWords(0, VRAM, 656 * 1024);     // Clear all VRAM

    cpuStartTiming(0xdeadbeef); // NOTE unused value, but not in blocksds?
    lcdMainOnBottom();

    consoleDemoInit();
    consoleSetWindow(NULL, 0, 0, 32, 15); // keep console text above keyboard
    keyboardDemoInit();
    // consoleDebugInit(DebugDevice_NOCASH); // has to be disabled on hw for logging errors, need to detect if running in emu?
    videoSetMode(MODE_FB0);

    // NOTE already done above for console/keyboard
    vramSetBankA(VRAM_A_LCD);
    // memset(VRAM_A, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));

    // TODO try for fun after hw accel
    // setCpuClock(false);

    if (!isDSiMode()) {
        mud_error("[ERROR]: NDS detected! only DSi is supported.\n");
        goto err;
    }

    if (!nitroFSInit(NULL)) {
        mud_error("[ERROR]: nitroFS failed to init\n");
        goto err;
    }
    chdir("nitro:/");
    mud->fb = (uint16_t *)VRAM_A;

    // TODO maybe add retry option here
	if (!Wifi_InitDefault(WFC_CONNECT)) {
        mud_error("[ERROR]: Failed to connect!\n");
        goto err;
	}
	return;

	err:
    delay_ticks(2000);
    exit(1);
}

void mudclient_poll_events(mudclient *mud) {
	scanKeys();
	int keys_down = keysDown();
	int keys_up = keysUp();

    if (keys_down & KEY_R) {
        mud_log("button R unused");
    }

    if (keys_down & KEY_A) {
        mud_log("button A unused");
    }

    if (keys_down & KEY_B) {
        mud_log("button B unused");
    }

	if (keys_down & KEY_X) {
	    malloc_stats();
	}

    if (keys_down & KEY_Y) {
        consoleClear();
    }

    if (keys_down & KEY_SELECT) {
        mud->options->interlace = !mud->options->interlace;
    }

    if (keys_down & KEY_LEFT) {
        mudclient_key_pressed(mud, K_LEFT, -1);
    }

    if (keys_down & KEY_RIGHT) {
        mudclient_key_pressed(mud, K_RIGHT, -1);
    }

    if (keys_down & KEY_UP) {
        mudclient_key_pressed(mud, K_UP, -1);
    }

    if (keys_down & KEY_DOWN) {
        mudclient_key_pressed(mud, K_DOWN, -1);
    }

    if (keys_up & KEY_LEFT) {
        mudclient_key_released(mud, K_LEFT);
    }

    if (keys_up & KEY_RIGHT) {
        mudclient_key_released(mud, K_RIGHT);
    }

    if (keys_up & KEY_UP) {
        mudclient_key_released(mud, K_UP);
    }

    if (keys_up & KEY_DOWN) {
        mudclient_key_released(mud, K_DOWN);
    }

    static bool kb;
    if (keys_down & KEY_START) {
        kb = !kb;
        lcdSwap();
        if (kb) {
            keyboardShow();
        } else {
            keyboardHide();
        }
    }

    if (kb) {
        int16_t key = keyboardUpdate();

        if (key != -1) {
            if (key == 10) {
                // NOTE: why this wasn't needed for login/chat, but needed for sleeping bag
                key = K_ENTER;
            }
            mudclient_key_pressed(mud, key, key);
        }
    } else {
        touchPosition touch = {0};
        touchRead(&touch);

        static bool touch_down;
        static bool l_down;

        if (keys_down & KEY_L) {
            l_down = true;
        }

        if (keys_up & KEY_L) {
            l_down = false;
        }

        if (touch.px == 0 && touch.py == 0) {
            if (touch_down != 0) {
                mudclient_mouse_released(mud, mud->mouse_x, mud->mouse_y,
                                         touch_down);
            }

            touch_down = 0;
        } else {
            mudclient_mouse_moved(mud, touch.px, touch.py);

            int mouse_down = l_down ? 3 : 1;

            if (touch_down == 0) {
                mudclient_mouse_pressed(mud, touch.px, touch.py, mouse_down);
            }

            touch_down = mouse_down;
        }
    }
}

// NOTE: from blocksds as devkitpro didn't have this
#define QUIET_NAN       ((255 << 23) | ((1 << 22) | 1))
#define INF             (0xFF << 23)

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect((x), 0)
#define SQRT_MODE_MASK 1

float hw_sqrtf(float x)
{
    if (__builtin_constant_p(x))
        return __builtin_sqrtf(x);

    union
    {
        float f;
        uint32_t i;
    } xu;

    xu.f = x;
    int32_t exponent = (int32_t)xu.i >> 23;

    // check if exponent is 0
    if (likely(exponent > 0))
    {
        if (unlikely(exponent == 255)) // Check if negative or NaN
        {
            // Expected behavior:
            // sqrt(-f) = +qNaN, sqrt(-NaN) = +qNaN, sqrt(-Inf) = +qNaN
            xu.i = (xu.i == INF) ? INF : QUIET_NAN;
            return xu.f;
        }
        else
        {
            uint32_t mantissa = xu.i & ~((uint32_t)exponent << 23);
            exponent = exponent - 127;
            mantissa += 1 << 23; // Adds implicit bit to mantissa.
            mantissa <<= (exponent & 1);

            REG_SQRT_PARAM = ((uint64_t)mantissa) << 25;
            if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_64)
                REG_SQRTCNT = SQRT_64;

            exponent >>= 1;
            // This is meant to be a floor division
            // meaning -1/2= -0.5 should map to -1
            exponent = exponent + 126;
            exponent <<= 23;
            // Wait for the square root operation to complete
            while (REG_SQRTCNT & SQRT_BUSY);

            uint32_t new_mantissa = REG_SQRT_RESULT;
            new_mantissa += 1;
            xu.i = ((uint32_t)exponent) + (new_mantissa >> 1);
            return xu.f;
        }
    }
    else
    {
        if (likely(exponent == 0))
        {
            if (likely(xu.i != 0))
            {
                uint32_t mantissa = xu.i & ~((uint32_t)exponent << 23);
                int32_t shift = __builtin_clz(mantissa) - (31 - 23);
                mantissa <<= shift; // normalize subnormal

                exponent = -126 - shift;
                mantissa <<= (exponent & 1);

                REG_SQRT_PARAM = ((uint64_t)mantissa) << 25;
                if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_64)
                    REG_SQRTCNT = SQRT_64;

                exponent >>= 1;
                exponent = exponent + 126;
                exponent <<= 23;

                // Wait for the square root operation to complete
                while (REG_SQRTCNT & SQRT_BUSY);

                uint32_t new_mantissa = REG_SQRT_RESULT;
                new_mantissa += 1;
                xu.i = ((uint32_t)exponent) + (new_mantissa >> 1);
                return xu.f;
            }
            else // sqrt(+0) = +0
            {
                xu.i = 0;
                return xu.f;
            }
        }
        else if (xu.i == (1u << 31))
        {
            xu.i= 1u << 31; // sqrt(-0) = -0
            return xu.f;
        }
        else
        {
            xu.i=QUIET_NAN; // sqrt(negative) = qNaN
            return xu.f;
        }
    }
}

#endif
