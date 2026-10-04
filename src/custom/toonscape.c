#include "toonscape.h"
#include "../scene.h"
#include <string.h>

/*
 * Convert textures to solid colours to save memory and provide
 * a cartoony appearance. ToonScape.
 */

#define TEXTURE_WALL_DOOR (0)
#define TEXTURE_WATER (1)
#define TEXTURE_WALL (2)
#define TEXTURE_PLANKS (3)
#define TEXTURE_WALL_DOORWAY (4)
#define TEXTURE_WALL_WINDOW (5)
#define TEXTURE_ROOF (6)
#define TEXTURE_WALL_ARROWSLIT (7)
#define TEXTURE_LEAFYTREE (8)
#define TEXTURE_TREESTUMP (9)
#define TEXTURE_FENCE (10)
#define TEXTURE_MOSSY (11)
#define TEXTURE_RAILINGS (12)
#define TEXTURE_PAINTING1 (13)
#define TEXTURE_PAINTING2 (14)
#define TEXTURE_MARBLE (15)
#define TEXTURE_DEADTREE (16)
#define TEXTURE_FOUNTAIN (17)
#define TEXTURE_WALL_STAINEDGLASS (18)
#define TEXTURE_TARGET (19)
#define TEXTURE_BOOKS (20)
#define TEXTURE_TIMBERED (21)
#define TEXTURE_TIMBERED_TIMBERWINDOW (22)
#define TEXTURE_MOSSYBRICKS (23)
#define TEXTURE_GROWINGWHEAT (24)
#define TEXTURE_GUNGYWATER (25)
#define TEXTURE_WEB (26)
#define TEXTURE_WALL_DESERTWINDOW (27)
#define TEXTURE_WALL_CRUMBLED (28)
#define TEXTURE_CAVERN (29)
#define TEXTURE_CAVERN2 (30)
#define TEXTURE_LAVA (31)
#define TEXTURE_PENTAGRAM (32)
#define TEXTURE_MAPLETREE (33)
#define TEXTURE_YEWTREE (34)
#define TEXTURE_HELMET (35)
#define TEXTURE_CANVAS_TENTBOTTOM (36)
#define TEXTURE_CHAINMAIL2 (37)
#define TEXTURE_MUMMY (38)
#define TEXTURE_JUNGLELEAF (39)
#define TEXTURE_JUNGLELEAF3 (40)
#define TEXTURE_JUNGLELEAF4 (41)
#define TEXTURE_JUNGLELEAF5 (42)
#define TEXTURE_JUNGLELEAF6 (43)
#define TEXTURE_MOSSYBRICKS_ARROWSLIT (44)
#define TEXTURE_PLANKS_WINDOW (45)
#define TEXTURE_PLANKS_JUNGLEWINDOW (46)
#define TEXTURE_CARGONET (47)
#define TEXTURE_BARK (48)
#define TEXTURE_CANVAS (49)
#define TEXTURE_CANVAS_TENTDOOR (50)
#define TEXTURE_WALL_LOWCRUMBLED (51)
#define TEXTURE_CAVERN_CRUMBLED (52)
#define TEXTURE_CAVERN2_CRUMBLED (53)
#define TEXTURE_LAVA_FLAMES (54)

const int modded_textures[] = {
TEXTURE_WATER,
TEXTURE_WALL,
TEXTURE_PLANKS,
TEXTURE_ROOF,
TEXTURE_LEAFYTREE,
TEXTURE_TREESTUMP,
TEXTURE_MOSSY,
TEXTURE_MARBLE,
TEXTURE_MOSSYBRICKS,
TEXTURE_GUNGYWATER,
TEXTURE_CAVERN,
TEXTURE_CAVERN2,
TEXTURE_LAVA,
TEXTURE_CANVAS_TENTBOTTOM,
TEXTURE_CHAINMAIL2,
TEXTURE_MUMMY,
TEXTURE_BARK,
TEXTURE_CANVAS,
-1
};

#ifdef __NDS__
bool toonscape_allow_load(int id) {
    // https://chisel.weirdgloop.org/rsc/images/textures17.jag/index.html
    // NOTE: this kills performance and stores them at double the size, also no memory left at all
    return id == TEXTURE_FOUNTAIN || id == TEXTURE_FENCE || id == TEXTURE_GROWINGWHEAT || id == TEXTURE_DEADTREE || id == TEXTURE_WALL_DOORWAY;
    // id == TEXTURE_RAILINGS || 
}
#endif

int toonscape_avoid_load(int id) {
    const int *mod_id = modded_textures;
    while (*mod_id != -1) {
        if (*mod_id == id) {
            return 1;
        }
        mod_id++;
    }
#ifdef __NDS__
    return !toonscape_allow_load(id);
#else
    return 0;
#endif
}

int32_t apply_toonscape(int32_t fill) {
    switch (fill) {
    case TEXTURE_BARK:
        return scene_rgb_to_fill(148, 75, 17);
    case TEXTURE_CANVAS:
    case TEXTURE_CANVAS_TENTBOTTOM:
        return scene_rgb_to_fill(230, 198, 155);
    case TEXTURE_CAVERN:
    case TEXTURE_CAVERN2:
        return scene_rgb_to_fill(90, 41, 24);
    case TEXTURE_GUNGYWATER:
        return scene_rgb_to_fill(54, 84, 102);
    case TEXTURE_LAVA:
        return scene_rgb_to_fill(255, 111, 12);
    case TEXTURE_LEAFYTREE:
        return scene_rgb_to_fill(0, 118, 0);
    case TEXTURE_MARBLE:
        return scene_rgb_to_fill(255, 255, 255);
    case TEXTURE_MOSSY:
        return scene_rgb_to_fill(120, 122, 121);
    case TEXTURE_MOSSYBRICKS:
        return scene_rgb_to_fill(51, 54, 51);
    case TEXTURE_MUMMY:
        return scene_rgb_to_fill(223, 219, 200);
    case TEXTURE_PLANKS:
        return scene_rgb_to_fill(168, 83, 10);
    case TEXTURE_ROOF:
        return scene_rgb_to_fill(115, 42, 22);
    case TEXTURE_TREESTUMP:
        return scene_rgb_to_fill(160, 89, 27);
    case TEXTURE_WALL:
        return scene_rgb_to_fill(49, 49, 49);
    case TEXTURE_WATER:
        return scene_rgb_to_fill(80, 145, 255);
    case TEXTURE_CHAINMAIL2:
        return scene_rgb_to_fill(74, 74, 74);
    }
    return fill;
}
