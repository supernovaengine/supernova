#ifndef supernova_h
#define supernova_h

#ifndef MAX_SCENE_LAYERS
#define MAX_SCENE_LAYERS 3
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 6
#endif

#ifndef MAX_SHADOWSMAP
#define MAX_SHADOWSMAP 6
#endif

#ifndef MAX_SHADOWSCUBEMAP
#define MAX_SHADOWSCUBEMAP 1
#endif

#ifndef MAX_SHADOWCASCADES
#define MAX_SHADOWCASCADES 4
#endif

#ifndef MAX_SUBMESHES
#define MAX_SUBMESHES 4
#endif

#ifndef MAX_TILEMAP_TILESRECT
#define MAX_TILEMAP_TILESRECT 200
#endif

#ifndef MAX_TILEMAP_TILES
#define MAX_TILEMAP_TILES 200
#endif

#ifndef MAX_SPRITE_FRAMES
#define MAX_SPRITE_FRAMES 64
#endif

#ifndef MAX_BONES
#define MAX_BONES 70
#endif

#ifndef MAX_MORPHTARGETS
#define MAX_MORPHTARGETS 8
#endif

#ifndef MAX_EXTERNAL_BUFFERS
#define MAX_EXTERNAL_BUFFERS 10
#endif

// to prevent tiled texture getting part of neighborhood tile
#ifndef TEXTURE_CUT_FACTOR
#define TEXTURE_CUT_FACTOR 0.5
#endif

#include "Engine.h"

void init();

#endif /* supernova_h */
